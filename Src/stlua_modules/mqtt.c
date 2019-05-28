/*
 * mqtt.c
 *
 *  Created on: May 7, 2019
 *      Author: jacob
 */
#include "stlua_modules/mqtt.h"

#include "lwip/apps/mqtt.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "string.h"
#include "lwip/netif.h"

static mqtt_client_t mqttClient = { 0 };

// TODO: Memory leaks when unsubscribing or connection dies
//       - fix this with external buffer or light LUA user data
// TODO: Problem with stack - check all pushes and pops

struct state_function_pair {
	lua_State *L;
	int functionRef;
};

static void connection_callback(mqtt_client_t *client, void *arg,
		mqtt_connection_status_t status) {
	struct state_function_pair* p = (struct state_function_pair*) arg;

	if (p == NULL) {
		return;
	}

	lua_State * const L = p->L;

	if (p->functionRef != LUA_REFNIL) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, p->functionRef);

		lua_pushinteger(L, status);

		if (lua_pcall(L, 1, 0, 0) != 0) {
			luaL_error(L, "error running mqtt connection callback function: %s",
					lua_tostring(L, -1));
		}

		luaL_unref(L, LUA_REGISTRYINDEX, p->functionRef);
	}

	free(p);
}

static int connect_client(lua_State *L) {
	const char * ip_str = luaL_checkstring(L, 1);
	int port_num = luaL_checkinteger(L, 2);
	const char * port_str = luaL_checkstring(L, 2);
	const char * client_id = luaL_checkstring(L, 3);

	int t4 = lua_type(L, 4);
	int t5 = lua_type(L, 5);

	struct state_function_pair *p = NULL;
	struct mqtt_connect_client_info_t mqttConnectClientInfo;

	memset(&mqttConnectClientInfo, 0, sizeof(mqttConnectClientInfo));

	mqttConnectClientInfo.client_id = client_id;

	if (t5 == LUA_TTABLE) {

		lua_getfield(L, 5, "user");
		if (!lua_isnil(L, -1)) {
			mqttConnectClientInfo.client_user = lua_tostring(L, -1);
		}
		lua_pop(L, 1);

		lua_getfield(L, 5, "pass");
		if (!lua_isnil(L, -1)) {
			mqttConnectClientInfo.client_pass = lua_tostring(L, -1);
		}
		lua_pop(L, 1);

		lua_getfield(L, 5, "keepalive");
		if (!lua_isnil(L, -1)) {
			LUA_INTEGER keepalive = lua_tointeger(L, -1);
			if (keepalive < 0) {
				keepalive = 0;
			} else if (keepalive > UINT16_MAX) {
				keepalive = UINT16_MAX;
			}
			mqttConnectClientInfo.keep_alive = keepalive;
		}
		lua_pop(L, 1);

		lua_getfield(L, 5, "willtopic");
		if (!lua_isnil(L, -1)) {
			mqttConnectClientInfo.will_topic = lua_tostring(L, -1);
		}
		lua_pop(L, 1);

		lua_getfield(L, 5, "willmsg");
		if (!lua_isnil(L, -1)) {
			mqttConnectClientInfo.will_msg = lua_tostring(L, -1);
		}
		lua_pop(L, 1);

		lua_getfield(L, 5, "willqos");
		if (!lua_isnil(L, -1)) {
			LUA_INTEGER willqos = lua_tointeger(L, -1);
			if (willqos < 0) {
				willqos = 0;
			} else if (willqos > UINT8_MAX) {
				willqos = UINT8_MAX;
			}
			mqttConnectClientInfo.will_qos = willqos;
		}
		lua_pop(L, 1);

		lua_getfield(L, 5, "willretain");
		if (!lua_isnil(L, -1)) {
			int willretain = lua_toboolean(L, -1);
			mqttConnectClientInfo.will_retain = willretain;
		}
		lua_pop(L, 1);
	}

	struct addrinfo hints;
	struct addrinfo *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = 0; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
	hints.ai_protocol = 0; /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	int s = getaddrinfo(ip_str, port_str, &hints, &result);

	if (s != 0) {
		return luaL_error(L, "getaddrinfo: status=%d, name=%s, port=%s\n", s,
				ip_str, port_str);
	}

	if (t4 == LUA_TFUNCTION) {
		p = malloc(sizeof(struct state_function_pair));

		lua_pushvalue(L, 4);

		p->L = L;
		p->functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
	} else if (t4 != LUA_TNIL) {
		return luaL_error(L,
				"bad argument #%d (function or nil expected, got %s)", 3,
				lua_typename(L, t4));
	}

	const ip4_addr_t * ip_a =
			(const ip4_addr_t*) &(((struct sockaddr_in*) result->ai_addr)->sin_addr);

	err_t err = mqtt_client_connect(&mqttClient, ip_a, port_num,
			connection_callback, (void*) p, &mqttConnectClientInfo);

	if (err != ERR_OK) {
		if (p != NULL) {
			luaL_unref(L, LUA_REGISTRYINDEX, p->functionRef);
			free(p);
		}
		lua_pushboolean(L, 0);
	} else {
		lua_pushboolean(L, 1);
	}

	return 1;
}

static int disconnect(lua_State *L) {
	mqtt_disconnect(&mqttClient);
	return 0;
}

static int is_connected(lua_State *L) {
	lua_pushboolean(L, mqtt_client_is_connected(&mqttClient));
	return 1;
}

static int publish(lua_State *L) {
	const char * topic = luaL_checkstring(L, 1);
	const char * payload = luaL_checkstring(L, 2);
	uint16_t payload_length = luaL_checkinteger(L, 3);
	uint8_t qos = luaL_checkinteger(L, 4);
	uint8_t retain = lua_toboolean(L, 5);

	int result = mqtt_publish(&mqttClient, topic, payload, payload_length, qos,
			retain, NULL, NULL) != ERR_OK;

	lua_pushboolean(L, result);

	return 1;
}

extern void xprintf(const char *, ...);

void incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
	struct state_function_pair* p = (struct state_function_pair*) arg;

	xprintf("Incoming message on topic: %s\n", topic);

	if (p == NULL) {
		return;
	}

	lua_State * const L = p->L;

	lua_pushstring(L, topic);
	lua_setglobal(L, "MQTT_CURRENT_TOPIC");
}

void incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
	struct state_function_pair* p = (struct state_function_pair*) arg;

	xprintf("Data from topic -> \"%*s\"\n", len, data);

	if (p == NULL) {
		return;
	}

	lua_State * const L = p->L;

	if (p->functionRef != LUA_REFNIL) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, p->functionRef);

		lua_getglobal(L, "MQTT_CURRENT_TOPIC");
		lua_pushlstring(L, (const char *) data, len);
		lua_pushboolean(L, flags == MQTT_DATA_FLAG_LAST);

		if (lua_pcall(L, 3, 0, 0) != 0) {
			luaL_error(L,
					"error running mqtt incoming data callback function: %s",
					lua_tostring(L, -1));
			return;
		}
	}
}

static int subscribe(lua_State *L) {
	const char * topic = luaL_checkstring(L, 1);
	int qos = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);

	struct state_function_pair *p = NULL;

	if (qos < 0 || qos > 2) {
		return luaL_error(L, "QoS has to be either 0, 1 or 2");
	}

	p = malloc(sizeof(struct state_function_pair));

	lua_pushvalue(L, 3);

	p->L = L;
	p->functionRef = luaL_ref(L, LUA_REGISTRYINDEX);

	if (mqtt_subscribe(&mqttClient, topic, qos, NULL, NULL) != ERR_OK) {
		if (p != NULL) {
			luaL_unref(L, LUA_REGISTRYINDEX, p->functionRef);
			free(p);
		}
		lua_pushboolean(L, 0);
		return 1;
	}

	mqtt_set_inpub_callback(&mqttClient, incoming_publish_cb, incoming_data_cb,
			p);

	lua_pushboolean(L, 1);
	return 1;
}

static int unsubscribe(lua_State *L) {
	const char * topic = luaL_checkstring(L, 1);

	err_t res = mqtt_unsubscribe(&mqttClient, topic, NULL, NULL);

	lua_pushboolean(L, res == ERR_OK);

	return 1;
}

static const luaL_Reg lib[] = { { "connect", connect_client }, { "disconnect",
		disconnect }, { "is_connected", is_connected }, { "publish", publish }, {
		"subscribe", subscribe }, { "unsubscribe", unsubscribe }, {
NULL, NULL } };

static int stlua_open_mqtt(lua_State *L) {
	luaL_newlib(L, lib);
	return 1;
}

void stlua_open_lib_mqtt(lua_State *L) {
	luaL_requiref(L, STLUA_MQTT_LIB_NAME, stlua_open_mqtt, 1);
	lua_pop(L, 1);
}
