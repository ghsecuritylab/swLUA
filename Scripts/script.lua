print('LUA SCRIPT STARTING')

local delta = 1.0
local last_seconds = rtos.get_seconds()
local clear_color = lcd.COLOR_WHITE

MAX_BALLS = 10
local balls = {}
local last_ball = 1

function add_ball(x, y)
	if #balls < MAX_BALLS then
		balls[#balls+1] = { ["x"] = x, ["y"] = y}
		last_ball = #balls
	else
		last_ball = (last_ball + 1) % (MAX_BALLS + 1) + 1
		balls[last_ball] = { ["x"] = x, ["y"] = y}
	end
end

function draw_balls()
	lcd.set_text_color(lcd.COLOR_RED)
	for i,v in ipairs(balls) do
		lcd.fill_circle(v.x, v.y, 10)
	end
end

function process_input(input)
	for i,v in ipairs(input.touches) do
		add_ball(v.x, v.y)
	end
end

function process(delta)
	-- Physics and logic

	-- draw & render
	lcd.set_text_color(lcd.COLOR_ORANGE)
	lcd.display_string(1, "Hello to swLUA!")

	lcd.display_string(2, "Delta: " .. delta)

	local fps = 0.0
	if delta > 0 then
		fps = 1 / delta
	end
	lcd.display_string(3, "FPS: " .. fps)

	draw_balls()
end

-- Main Loop
while true do
	-- Delta calculation
	local current_seconds = rtos.get_seconds()
	delta = current_seconds - last_seconds
	last_seconds = current_seconds

	process_input(ts.get_status())

	lcd.clear(clear_color)

	process(delta)

	lcd.swap_buffers()
	rtos.yield()
end
