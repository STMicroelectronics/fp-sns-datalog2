/**
 ******************************************************************************
 * @file      syscalls.c
 * @author    Auto-generated by STM32CubeIDE
 * @brief     STM32CubeIDE Minimal System calls file
 *
 *            For more information about which c-functions
 *            need which of these lowlevel functions
 *            please consult the Newlib libc-manual
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes */
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

/* integration with eLooM framework: */
/* 1. we map the low level put_char to the framework function used for the log. */
//#if defined(DEBUG) || defined(SYS_DEBUG)
//extern int SysDebugLowLevelPutchar(int x);
//#define __io_putchar SysDebugLowLevelPutchar
//#endif

#if defined (__GNUC__) && !defined(__ARMCC_VERSION)

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>


/* Variables */
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));


char *__env[1] = { 0 };
char **environ = __env;


/* Functions */
void initialise_monitor_handles()
{
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
	while (1) {}		/* Make sure we hang here */
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		*ptr++ = __io_getchar();
	}

return len;
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		__io_putchar(*ptr++);
	}
	return len;
}

int _close(int file)
{
	return -1;
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
	return 0;
}

int _open(char *path, int flags, ...)
{
	/* Pretend like we always fail */
	return -1;
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


#elif defined (__ARMCC_VERSION) //KEIL AC6
#if defined(DEGUG) || defined(SYS_DEBUG)
extern int __io_putchar(int x);

/**
 * @brief stdout_putchar call for standard output implementation
 * @param ch Character to print
 * @retval Character printed or -1 if error.
 */
int stdout_putchar (int ch)
{
  /* remap the stderr on the console */
  __io_putchar(ch);
  return ch;
}
#endif
#elif defined (__IAR_SYSTEMS_ICC__)

/* Forward function declaration. */
__weak int __io_putchar(int ch);
__weak int __io_getchar(void);

size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize);
size_t __read(int Handle, unsigned char *Buf, size_t Bufsize);

/** @brief IAR specific low level standard input
 * @param Handle IAR internal handle
 * @param Buf Buffer where to store characters read from stdin
 * @param Bufsize Number of characters to read
 * @retval Number of characters read
 */
size_t __read(int Handle, unsigned char *Buf, size_t Bufsize)
{
  if (Handle != 0){
    return -1;
  }
  return Bufsize;
}

/** @brief IAR specific low level standard output
 * @param Handle IAR internal handle
 * @param Buf Buffer containing characters to be written to stdout
 * @param Bufsize Number of characters to write
 * @retval Number of characters write
 */
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
   /* Check for the command to flush all handles */
  if (Handle == -1)
  {
    return 0;
  }

  /* Only write to stdout (1) and stderr (2), otherwise return failure */
  if (Handle != 1 && Handle != 2)
  {
    return -1;
  }

  int DataIdx;
  for (DataIdx = 0; DataIdx < Bufsize; DataIdx++)
  {
    __io_putchar( *Buf++ );
  }

  return Bufsize;
}

#else
#error "Toolchain not supported"
#endif