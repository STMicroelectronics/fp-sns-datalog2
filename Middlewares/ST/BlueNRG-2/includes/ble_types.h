/**
  ******************************************************************************
  * @file    ble_types.h
  * @author  AMS - RF Application team
  * @brief   Header file with macros for byte order conversions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
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

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 0
#define __BIG_ENDIAN    1
#endif

/* Byte order conversions */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htobs(d)  (d)
#define htobl(d)  (d)
#define htob(d,n) (d)
#define btohs(d)  (d)
#define btohl(d)  (d)
#define btoh(d,n) (d)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define htobs(d)  (d<<8|d>>8)
#define htobl(d)  (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define htob(d,n) (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define btohs(d)  (d<<8|d>>8)
#define btohl(d)  (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define btoh(d,n) (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#else
#error "Unknown byte order"
#endif

typedef uint8_t BOOL;

#ifndef TRUE 
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif



#endif /* __BLE_TYPES_H__ */
