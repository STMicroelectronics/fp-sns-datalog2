/**
  ******************************************************************************
  * @file    ble_types.h
  * @author  AMS - RF Application team
  * @brief   Header file with macros for byte order conversions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __BLE_TYPES_H__
#define __BLE_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 0
#define __BIG_ENDIAN    1
#endif /* __LITTLE_ENDIAN */

/* Byte order conversions */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define HTOBS(d)  (d)
#define HTOBL(d)  (d)
#define HTOB(d,n) (d)
#define BTOHS(d)  (d)
#define BTOHL(d)  (d)
#define BTOH(d,n) (d)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define HTOBS(d)  (d<<8|d>>8)
#define HTOBL(d)  (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define HTOB(d,n) (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define BTOHS(d)  (d<<8|d>>8)
#define BTOHL(d)  (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define BTOH(d,n) (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#else /* __BYTE_ORDER == __LITTLE_ENDIAN */
#error "Unknown byte order"
#endif /* __BYTE_ORDER == __LITTLE_ENDIAN */

typedef uint8_t BOOL;

#ifndef TRUE
#define TRUE (1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE (0)
#endif /* FALSE */

#ifdef __cplusplus
}
#endif

#endif /* __BLE_TYPES_H__ */
