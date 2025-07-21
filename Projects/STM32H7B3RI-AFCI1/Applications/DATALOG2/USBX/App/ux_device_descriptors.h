/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_descriptors.h
  * @author  MCD Application Team
  * @brief   USBX Device descriptor header file
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
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UX_DEVICE_DESCRIPTORS_H__
#define __UX_DEVICE_DESCRIPTORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "ux_api.h"
#include "ux_stm32_config.h"
#include "ux_device_class_sensor_streaming.h"

/* Defines -------------------------------------------------------------------*/

#define USBD_MAX_NUM_CONFIGURATION                     1U
#define USBD_MAX_SUPPORTED_CLASS                       3U
#define USBD_MAX_CLASS_ENDPOINTS                       9U
#define USBD_MAX_CLASS_INTERFACES                      9U

#define USBD_CONFIG_MAXPOWER                           25U
#define USBD_COMPOSITE_USE_IAD                         0U
#define USBD_DEVICE_FRAMEWORK_BUILDER_ENABLED          1U

#define USB_SIZ_STRING_SERIAL                          0x06U

#define USB_DESC_TYPE_INTERFACE                        0x04U
#define USB_DESC_TYPE_ENDPOINT                         0x05U
#define USB_DESC_TYPE_CONFIGURATION                    0x02U
#define USB_DESC_TYPE_IAD                              0x0BU
#define USB_DESC_TYPE_STRING                           0x03U

#define USBD_EP_TYPE_CTRL                              0x00U
#define USBD_EP_TYPE_ISOC                              0x01U
#define USBD_EP_TYPE_BULK                              0x02U
#define USBD_EP_TYPE_INTR                              0x03U

#define USBD_FULL_SPEED                                0x00U
#define USBD_HIGH_SPEED                                0x01U

#define USB_BCDUSB                                     0x0200U
#define LANGUAGE_ID_MAX_LENGTH                         2U

#define USBD_IDX_MFC_STR                               0x01U
#define USBD_IDX_PRODUCT_STR                           0x02U
#define USBD_IDX_SERIAL_STR                            0x03U
#define USBD_IDX_MSFOS_STR                             0xEEU

#ifndef USBD_MAX_EP0_SIZE
#define USBD_MAX_EP0_SIZE                              64U
#endif

#ifndef USBD_STRING_FRAMEWORK_MAX_LENGTH
#define USBD_STRING_FRAMEWORK_MAX_LENGTH               512U
#endif

/* Vendor ID */
#ifndef USBD_VID
#define USBD_VID                                       0x0483U
#endif

/* Product ID */
#ifndef USBD_PID
#define USBD_PID                                       0x5744U
#endif

#ifndef USBD_LANGID_STRING
#define USBD_LANGID_STRING                             1033U
#endif

#ifndef USBD_MANUFACTURER_STRING
#define USBD_MANUFACTURER_STRING                       "STMicroelectronics"
#endif

#ifndef USBD_PRODUCT_STRING
#define USBD_PRODUCT_STRING                            "Multi_Sensor_Streaming"
#endif

/* This is the maximum supported configuration descriptor size
 User may redefine this value in order to optimize */
#ifndef USBD_FRAMEWORK_MAX_DESC_SZ
#define USBD_FRAMEWORK_MAX_DESC_SZ                     200U
#endif /* USBD_FRAMEWORK_MAX_DESC_SZ */

#ifndef USBD_CONFIG_STR_DESC_IDX
#define USBD_CONFIG_STR_DESC_IDX                       0U
#endif /* USBD_CONFIG_STR_DESC_IDX */

#ifndef USBD_CONFIG_BMATTRIBUTES
#define USBD_CONFIG_BMATTRIBUTES                       0xC0U
#endif /* USBD_CONFIG_BMATTRIBUTES */

/* Exported types ------------------------------------------------------------*/

/* Enum Class Type */
typedef enum
{
  CLASS_TYPE_NONE = 0,
  CLASS_TYPE_SENSOR_STREAMING = 1
} USBD_CompositeClassTypeDef;

/* USB endpoint handle structure */
typedef struct
{
  uint8_t add;
  uint8_t type;
  uint16_t size;
  uint8_t is_used;
} USBD_EPTypeDef;

/* USB endpoint descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
} __PACKED USBD_EpDescTypedef;

/* USB Composite handle structure */
typedef struct
{
  USBD_CompositeClassTypeDef ClassType;
  uint32_t ClassId;
  uint32_t Active;
  uint32_t NumEps;
  USBD_EPTypeDef Eps[USBD_MAX_CLASS_ENDPOINTS];
  uint32_t NumIf;
  uint8_t Ifs[USBD_MAX_CLASS_INTERFACES];
} USBD_CompositeElementTypeDef;

/* USB Device handle structure */
typedef struct _USBD_DevClassHandleTypeDef
{
  uint8_t Speed;
  uint32_t classId;
  uint32_t NumClasses;
  USBD_CompositeElementTypeDef tclasslist[USBD_MAX_SUPPORTED_CLASS];
  uint32_t CurrDevDescSz;
  uint32_t CurrConfDescSz;
} USBD_DevClassHandleTypeDef;

/* USB Device endpoint direction */
typedef enum
{
  OUT = 0x00,
  IN = 0x80,
} USBD_EPDirectionTypeDef;

/* USB Device descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} __PACKED USBD_DeviceDescTypedef;

/* USB interface descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} __PACKED USBD_IfDescTypedef;

/* USB Config descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wDescriptorLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} __PACKED USBD_ConfigDescTypedef;

/* USB Qualifier descriptors structure */
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdDevice;
  uint8_t Class;
  uint8_t SubClass;
  uint8_t Protocol;
  uint8_t bMaxPacketSize;
  uint8_t bNumConfigurations;
  uint8_t bReserved;
} __PACKED USBD_DevQualiDescTypedef;

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

uint8_t *USBD_Get_Device_Framework_Speed(ULONG *length);
uint8_t *USBD_Get_StringDescription_Framework(ULONG *length);
uint8_t *USBD_Get_Language_Id_Framework(ULONG *length);

#ifdef __cplusplus
}
#endif
#endif  /* __UX_DEVICE_DESCRIPTORS_H__ */
