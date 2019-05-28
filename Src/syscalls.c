/**
 ******************************************************************************
  * File Name          : syscalls.c
  * Description        : Support files for GNU libc. Files in the system
  *                      namespace go here. Files in the C namespace (ie those
  *                      that do not start with an underscore) go in .c.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
 /******************************************************************************
 * If LWIP_SOCKET option is enabled:
 *  - include this syscalls.c file
 ******************************************************************************/

#ifdef __cplusplus
 extern "C" {
#endif

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>

/* USER CODE BEGIN 1 */

#include "fatfs.h"
#include "stm32f7xx_it.h"
#include "usb_host.h"

/* USER CODE END 1 */

#define MAX_STACK_SIZE 0x2000

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

/* USER CODE BEGIN 2 */

#define MAX_OPEN_FILES 32

#define FILE_DESCRIPTOR_BASE 1001

typedef struct {
	short d;
	union {
		FIL * fil_ptr;
	} h; // Handle
} FileDescriptorRecord_t;

FileDescriptorRecord_t fileDescriptorMapTable[MAX_OPEN_FILES] = {0};

SemaphoreHandle_t fileDescriptorMapTableMutex = NULL;
SemaphoreHandle_t fileDescriptorMapTableAvailableEntriesSemaphore = NULL;

#define fileDescriptorToMapTableIndex(d) ((d) - FILE_DESCRIPTOR_BASE)

static int obtainFirstFreeDescriptorTableEntry() {
	int ret = -1;
	if(xSemaphoreTake(fileDescriptorMapTableAvailableEntriesSemaphore, 0) == pdFALSE) {
		return -1;
	}

	while(xSemaphoreTake(fileDescriptorMapTableMutex, portMAX_DELAY) != pdTRUE) {
		;
	}

	for(int i = 0; i < MAX_OPEN_FILES; ++i) {
		if (fileDescriptorMapTable[i].d == 0) {
			ret = i;
			goto cleanup;
		}
	}

	xSemaphoreGive(fileDescriptorMapTableAvailableEntriesSemaphore);
cleanup:
	xSemaphoreGive(fileDescriptorMapTableMutex);

	return ret;
}

static void freeDescriptorTableEntry(int no) {
	if(fileDescriptorMapTable[no].d != 0) {
		while(xSemaphoreTake(fileDescriptorMapTableMutex, portMAX_DELAY) != pdTRUE) {
			;
		}
		{
			fileDescriptorMapTable[no].d = 0;
			fileDescriptorMapTable[no].h.fil_ptr = NULL;
		}
		xSemaphoreGive(fileDescriptorMapTableMutex);


		xSemaphoreGive(fileDescriptorMapTableAvailableEntriesSemaphore);
	}
}

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end,*min_stack_ptr;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;

	/* Use the NVIC offset register to locate the main stack pointer. */
	min_stack_ptr = (char*)(*(unsigned int *)*(unsigned int *)0xE000ED08);
	/* Locate the STACK bottom address */
	min_stack_ptr -= MAX_STACK_SIZE;

	if (heap_end + incr > min_stack_ptr)
	{
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
/*
 * _gettimeofday primitive (Stub function)
 * */

int _gettimeofday (struct timeval * tp, struct timezone * tzp)
{
  /* Return fixed data for the timezone.  */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void initialise_monitor_handles()
{
	for(int i = 0; i < MAX_OPEN_FILES; ++i) {
		fileDescriptorMapTable[i].d = 0;
		fileDescriptorMapTable[i].h.fil_ptr = NULL;
	}

	fileDescriptorMapTableMutex = xSemaphoreCreateMutex();
	xSemaphoreGive(fileDescriptorMapTableMutex);


	fileDescriptorMapTableAvailableEntriesSemaphore = xSemaphoreCreateCounting(MAX_OPEN_FILES, MAX_OPEN_FILES);
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}
}

int _write(int file, char *ptr, int len)
{
	if (file == STDOUT_FILENO || file == STDERR_FILENO || file == STDIN_FILENO) {
		int DataIdx;

		for (DataIdx = 0; DataIdx < len; DataIdx++)
		{
		   __io_putchar( *ptr++ );
		}

		return len;
	} else {
		if (!(file >= FILE_DESCRIPTOR_BASE && file <= FILE_DESCRIPTOR_BASE + MAX_OPEN_FILES)) {
			return -1;
		}

		int descriptorTableIndex = fileDescriptorToMapTableIndex(file);

		if (fileDescriptorMapTable[descriptorTableIndex].d != file) {
			return -1;
		}

		FIL * fil = fileDescriptorMapTable[descriptorTableIndex].h.fil_ptr;
		UINT bw = 0;

//		while(xSemaphoreTake(USBHFatFSBinarySemaphore, portMAX_DELAY) != pdTRUE) {
//			;
//		}
		{
			if (f_write(fil, ptr, len, &bw) != FR_OK) {
				_Error_Handler(__FILE__, __LINE__);
				return 0;
			}
		}
//		xSemaphoreGive(USBHFatFSBinarySemaphore);

		return bw;
	}
}

int _close(int file)
{
	if (file != STDIN_FILENO && file != STDOUT_FILENO && file != STDERR_FILENO) {
		if (!(file >= FILE_DESCRIPTOR_BASE && file <= FILE_DESCRIPTOR_BASE + MAX_OPEN_FILES)) {
			return -1;
		}

		int descriptorTableIndex = fileDescriptorToMapTableIndex(file);

		if (fileDescriptorMapTable[descriptorTableIndex].d != file) {
			return -1;
		}

		FIL * fil = fileDescriptorMapTable[descriptorTableIndex].h.fil_ptr;

		if(f_close(fil) != FR_OK) {
			return -1;
		} else {
			freeDescriptorTableEntry(descriptorTableIndex);
			vPortFree(fil);
			return 0;
		}
	}
	return 0;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	if (!(file >= FILE_DESCRIPTOR_BASE && file <= FILE_DESCRIPTOR_BASE + MAX_OPEN_FILES)) {
		return -1;
	}

	int descriptorTableIndex = fileDescriptorToMapTableIndex(file);

	if (fileDescriptorMapTable[descriptorTableIndex].d != file) {
		return -1;
	}

	FIL * fil = fileDescriptorMapTable[descriptorTableIndex].h.fil_ptr;
	FSIZE_t pos = 0;

	if (dir == SEEK_SET)
	{
		pos = ptr;
	}
	else if (dir == SEEK_CUR)
	{
		pos = f_tell(fil) + ptr;
	}
	else if(dir == SEEK_END)
	{
		pos = f_size(fil);
	}
	else
	{
		errno = EINVAL;
		return -1;
	}

	return !(f_lseek(fil, pos) == FR_OK);
}

int _read(int file, char *ptr, int len)
{
	if (file == STDIN_FILENO) {
		int DataIdx;

		for (DataIdx = 0; DataIdx < len; DataIdx++)
		{
		  *ptr++ = __io_getchar();
		}

	   return len;
	} else {
		if (ptr == NULL || len == 0) {
			return 0;
		}

		if (!(file >= FILE_DESCRIPTOR_BASE && file <= FILE_DESCRIPTOR_BASE + MAX_OPEN_FILES)) {
			return -1;
		}

		int descriptorTableIndex = fileDescriptorToMapTableIndex(file);

		if (fileDescriptorMapTable[descriptorTableIndex].d != file) {
			return -1;
		}

		FIL * fil = fileDescriptorMapTable[descriptorTableIndex].h.fil_ptr;

		UINT br = 0;

		if (f_read(fil, ptr, len, &br) != FR_OK) {
			return -1;
		}

		return br;
	}
	return -1;
}

int _open(char *path, int flags, ...)
{
	FIL * fil = NULL;
	BYTE mode = 0;
	int descriptorTableIndex = -1;

	descriptorTableIndex = obtainFirstFreeDescriptorTableEntry();

	if (descriptorTableIndex == -1) {
		return -1;
	}

	fil = pvPortMalloc(sizeof(FIL));

	if (fil == NULL) {
		errno = ENOMEM;
		freeDescriptorTableEntry(descriptorTableIndex);
		return -1;
	}

	if (flags & ~(O_RDONLY | O_WRONLY | O_RDWR | O_CREAT | O_APPEND | O_TRUNC))
	{
		errno = ENOSYS;
		freeDescriptorTableEntry(descriptorTableIndex);
		return -1;
	}

	if ((flags & O_ACCMODE) == O_RDONLY) {
		mode |= FA_READ;
	} if (flags & O_WRONLY) {
		mode |= FA_WRITE;
	} else if (flags & O_RDWR) {
		mode |= FA_READ | FA_WRITE;
	}

	if (flags & O_CREAT) {
		mode |= FA_OPEN_ALWAYS;
	} else if (flags & O_TRUNC) {
		mode |= FA_CREATE_ALWAYS;
	} else if (mode & O_APPEND) {
		mode |= FA_OPEN_APPEND;
	}

//	extern void debug_msg(const char*);
//	debug_msg("Waiting for USB semaphore");
//	while(xSemaphoreTake(USBHFatFSBinarySemaphore, portMAX_DELAY) != pdTRUE) {
//		;
//	}
//	debug_msg("USB semaphore got");
	{
		if (f_open(fil, path, mode) != FR_OK) {
			freeDescriptorTableEntry(descriptorTableIndex);
			return -1;
		}
	}
//	xSemaphoreGive(USBHFatFSBinarySemaphore);

	fileDescriptorMapTable[descriptorTableIndex].d = descriptorTableIndex + FILE_DESCRIPTOR_BASE;
	fileDescriptorMapTable[descriptorTableIndex].h.fil_ptr = fil;

	return fileDescriptorMapTable[descriptorTableIndex].d;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	return -1;
}

int _stat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}
/* USER CODE END 5 */

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

