/**
  ******************************************************************************
  * @file    BLE_ManagerCommon.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Macros used for exporting the BLE Characteristics
  *          and Services
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _UUID_BLE_MANGER_COMMON_H_
#define _UUID_BLE_MANGER_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
  {\
    (uuid_struct[0 ] = uuid_0) ; (uuid_struct[1 ] = uuid_1) ; (uuid_struct[2 ] = uuid_2) ; (uuid_struct[3 ] = uuid_3) ; \
    (uuid_struct[4 ] = uuid_4) ; (uuid_struct[5 ] = uuid_5) ; (uuid_struct[6 ] = uuid_6) ; (uuid_struct[7 ] = uuid_7) ; \
    (uuid_struct[8 ] = uuid_8) ; (uuid_struct[9 ] = uuid_9) ; (uuid_struct[10] = uuid_10); (uuid_struct[11] = uuid_11); \
    (uuid_struct[12] = uuid_12); (uuid_struct[13] = uuid_13); (uuid_struct[14] = uuid_14); (uuid_struct[15] = uuid_15); \
  }

/* Store Value into a buffer in Little Endian Format */
#if 0
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (0xFFU&(val)   )) , \
                                   ((buf)[1] =  (uint8_t) (0xFFU&((val)>>8)) ) )

#define STORE_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (0xFFU&(val)      ) ) , \
                                   ((buf)[1] =  (uint8_t) (0xFFU&((val)>>8))  ) , \
                                   ((buf)[2] =  (uint8_t) (0xFFU&((val)>>16)) ) , \
                                   ((buf)[3] =  (uint8_t) (0xFFU&((val)>>24)) ) )

#define STORE_BE_32(buf, val)    ( ((buf)[3] =  (uint8_t) (0xFFU&(val)     )) , \
                                   ((buf)[2] =  (uint8_t) (0xFFU&((val)>>8))  ) , \
                                   ((buf)[1] =  (uint8_t) (0xFFU&((val)>>16)) ) , \
                                   ((buf)[0] =  (uint8_t) (0xFFU&((val)>>24)) ) )
#else
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (((uint32_t )0xFF)&(((uint32_t)(val))   ))) , \
                                   ((buf)[1] =  (uint8_t) (((uint32_t )0xFF)&(((uint32_t)(val))>>((uint32_t)8))) ) )

#define STORE_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (((uint32_t )0xFF)&(((uint32_t)(val))      ) )) , \
                                   ((buf)[1] =  (uint8_t) (((uint32_t )0xFF)&(((uint32_t)(val))>>((uint32_t)8)))  ) , \
                                   ((buf)[2] =  (uint8_t) (((uint32_t )0xFF)&(((uint32_t)(val))>>((uint32_t)16))) ) , \
                                   ((buf)[3] =  (uint8_t) (((uint32_t )0xFF)&(((uint32_t)(val))>>((uint32_t)24))) ) )

#define STORE_BE_32(buf, val)    ( ((buf)[3] =  (uint8_t) (0xFFU&(val)     )) , \
                                   ((buf)[2] =  (uint8_t) (0xFFU&((val)>>8))  ) , \
                                   ((buf)[1] =  (uint8_t) (0xFFU&((val)>>16)) ) , \
                                   ((buf)[0] =  (uint8_t) (0xFFU&((val)>>24)) ) )
#endif
#ifdef __cplusplus
}
#endif

#endif /* _UUID_BLE_MANGER_COMMON_H_ */

