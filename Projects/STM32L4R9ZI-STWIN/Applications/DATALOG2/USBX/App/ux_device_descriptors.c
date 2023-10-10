/**
  ******************************************************************************
  * @file    ux_device_descriptors.c
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

/* Includes ------------------------------------------------------------------*/
#include "ux_device_descriptors.h"
#include "ux_device_stack.h"
/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define  USB_DESC_TYPE_OS_FEATURE_EXT_PROPERTIES      4U
#define  USB_DESC_TYPE_OS_FEATURE_EXT_COMPAT_ID       5U
#define  USB_DESC_TYPE_CUSTOM_IN_EP_DESCRIPTION       7U
#define  USB_DESC_TYPE_USER_CONTROL_ID                0U

/* Private macro -------------------------------------------------------------*/
#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

/* Private function prototypes -----------------------------------------------*/
static void USBD_Desc_GetString(uint8_t *desc, uint8_t *Buffer, uint16_t *len);
static uint8_t USBD_Desc_GetLen(uint8_t *buf);
static uint8_t *USBD_Device_Framework_Builder(USBD_DevClassHandleTypeDef *pdev, uint8_t *pDevFrameWorkDesc,
                                              uint8_t *UserClassInstance, uint8_t Speed);
static uint8_t USBD_FrameWork_SS_WCID_ConfigurationDesc(USBD_DevClassHandleTypeDef *pdev, uint8_t Speed,
                                                        uint8_t *pCmpstConfDesc);
static uint8_t USBD_FrameWork_AddClass(USBD_DevClassHandleTypeDef *pdev, USBD_CompositeClassTypeDef class,
                                       uint8_t cfgidx, uint8_t Speed, uint8_t *pCmpstConfDesc);
static uint8_t USBD_FrameWork_FindFreeIFNbr(USBD_DevClassHandleTypeDef *pdev);
static void USBD_FrameWork_AssignEp(USBD_DevClassHandleTypeDef *pdev, uint8_t address, uint8_t type, uint32_t size);
static void USBD_FrameWork_SS_WCID_If_Ep_Desc(USBD_DevClassHandleTypeDef *pdev, uint32_t p_conf, uint32_t *size);
static UINT vendor_request_function(ULONG request, ULONG request_value, ULONG request_index, ULONG request_length,
                                    UCHAR *buff, ULONG *len);
static void UlongToStr(uint32_t value, uint8_t *pbuf, uint8_t len);

/* Private variables ---------------------------------------------------------*/
USBD_DevClassHandleTypeDef USBD_Device_FS;
uint8_t UserClassInstance[USBD_MAX_CLASS_INTERFACES] = { CLASS_TYPE_SENSOR_STREAMING };

/* The generic device descriptor buffer that will be filled by builder
 Size of the buffer is the maximum possible device FS descriptor size. */
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN static uint8_t DevFrameWorkDesc_FS[USBD_FRAMEWORK_MAX_DESC_SZ] __ALIGN_END = { 0 };
static uint8_t *pDevFrameWorkDesc_FS = DevFrameWorkDesc_FS;

/* String Device Framework :
 Byte 0 and 1 : Word containing the language ID : 0x0904 for US
 Byte 2       : Byte containing the index of the descriptor
 Byte 3       : Byte containing the length of the descriptor string
 */
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN UCHAR USBD_string_framework[USBD_STRING_FRAMEWORK_MAX_LENGTH] __ALIGN_END = { 0 };

/* Multiple languages are supported on the device, to add
 a language besides English, the Unicode language code must
 be appended to the language_id_framework array and the length
 adjusted accordingly. */

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
UCHAR USBD_language_id_framework[LANGUAGE_ID_MAX_LENGTH] =
{
  0
};

/** WCID Descriptors **/

/* Microsoft OS String Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN static uint8_t USBD_OSStringDesc[] __ALIGN_END =
{
  0x09, /* Length */
  //USB_DESC_TYPE_STRING,       /* bDescriptorType added automatically by the MW (in this case) */
  0x4D,
  0x53,
  /*Signature MSFT100*/
  0x46,
  0x54,
  0x31,
  0x30,
  0x30,
  /******end sig*****/
  0x07, /* Vendor Code */
};

/* OS ID feature Descriptor */
__ALIGN_BEGIN static uint8_t OS_IDFeatureDescriptor[] __ALIGN_END =
{
  0x28, /*Length */
  0x00, /* "     */
  0x00, /* "     */
  0x00, /* "     */
  0x00, /*Version 0.1*/
  0x01,
  0x04, /*Compatibility ID Descriptor index (0x0004) */
  0x00,
  0x01, /*N of sections*/
  0x00, /* 7 bytes reserved */
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00, /*Interface number (0) */
  0x01, /*reserved*/
  /* Compatible ID: "WINUSB\0\"*/
  0x57,
  0x49,
  0x4E,
  0x55,
  0x53,
  0x42,
  0x00,
  0x00,
  /* Sub Compatible ID: Unused*/
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  /*reserved*/
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

/* Microsoft Extended Properties Feature Descriptor Typedef*/
typedef struct
{
  // Header
  uint32_t dwLength;
  uint16_t bcdVersion;
  uint16_t wIndex;
  uint16_t wCount;
  // Custom Property Section 1
  uint32_t dwSize;
  uint32_t dwPropertyDataType;
  uint16_t wPropertyNameLength;
  uint16_t bPropertyName[20];
  uint32_t dwPropertyDataLength;
  uint16_t bPropertyData[39];
} USBD_ExtPropertiesDescStruct;

/* Microsoft Extended Properties Feature Descriptor */
static USBD_ExtPropertiesDescStruct USBD_ExtPropertiesDesc =
{
  sizeof(USBD_ExtPropertiesDesc),
  0x0100,
  0x0005,
  0x0001,
  0x00000084,
  0x00000001,
  0x0028,
  {
    'D',
    'e',
    'v',
    'i',
    'c',
    'e',
    'I',
    'n',
    't',
    'e',
    'r',
    'f',
    'a',
    'c',
    'e',
    'G',
    'U',
    'I',
    'D',
    0
  },
  0x0000004E,
  {
    '{',
    'F',
    '7',
    '0',
    '2',
    '4',
    '2',
    'C',
    '7',
    '-',
    'F',
    'B',
    '2',
    '5',
    '-',
    '4',
    '4',
    '3',
    'B',
    '-',
    '9',
    'E',
    '7',
    'E',
    '-',
    'A',
    '4',
    '2',
    '6',
    '0',
    'F',
    '3',
    '7',
    '3',
    '9',
    '8',
    '2',
    '}',
    0
  }
};

/* Public functions */

/**
  * @brief  USBD_Get_Device_Framework_Speed
  *         Return the device speed descriptor
  * @param  Speed : HIGH or FULL SPEED flag
  * @param  length : length of HIGH or FULL SPEED array
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_Get_Device_Framework_Speed(ULONG *length)
{
  uint8_t *pFrameWork = NULL;

  USBD_Device_Framework_Builder(&USBD_Device_FS, pDevFrameWorkDesc_FS, UserClassInstance, USBD_FULL_SPEED);

  /* Get the length of USBD_device_framework_full_speed */
  *length = (ULONG)(USBD_Device_FS.CurrDevDescSz + USBD_Device_FS.CurrConfDescSz);

  pFrameWork = pDevFrameWorkDesc_FS;
  return pFrameWork;
}

/**
  * @brief  USBD_Get_StringDescription_Framework
  *         Return the language_id_framework
  * @param  Length : Length of String_Framework
  * @retval Pointer to language_id_framework buffer
  */
uint8_t *USBD_Get_StringDescription_Framework(ULONG *length)
{
  uint16_t len = 0U;
  uint8_t count = 0U;

  /* Set the Manufacturer language Id and index in USBD_string_framework */
  USBD_string_framework[count++] = USBD_LANGID_STRING & 0xFF;
  USBD_string_framework[count++] = USBD_LANGID_STRING >> 8;
  USBD_string_framework[count++] = USBD_IDX_MFC_STR;

  /* Set the Manufacturer string in string_framework */
  USBD_Desc_GetString((uint8_t *) USBD_MANUFACTURER_STRING, USBD_string_framework + count, &len);

  /* Set the Product language Id and index in USBD_string_framework */
  count += len + 1;
  USBD_string_framework[count++] = USBD_LANGID_STRING & 0xFF;
  USBD_string_framework[count++] = USBD_LANGID_STRING >> 8;
  USBD_string_framework[count++] = USBD_IDX_PRODUCT_STR;

  /* Set the Product string in USBD_string_framework */
  USBD_Desc_GetString((uint8_t *) USBD_PRODUCT_STRING, USBD_string_framework + count, &len);

  /* Set Serial language Id and index in string_framework */
  count += len + 1;
  USBD_string_framework[count++] = USBD_LANGID_STRING & 0xFF;
  USBD_string_framework[count++] = USBD_LANGID_STRING >> 8;
  USBD_string_framework[count++] = USBD_IDX_SERIAL_STR;

  /* Set the Serial number in USBD_string_framework */
  uint32_t stm32_UID;
  uint8_t usb_serial[25];

  stm32_UID = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UlongToStr(stm32_UID, usb_serial, 9);

  stm32_UID = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UlongToStr(stm32_UID, &usb_serial[8], 9);

  stm32_UID = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
  UlongToStr(stm32_UID, &usb_serial[16], 9);

  USBD_Desc_GetString((uint8_t *) usb_serial, USBD_string_framework + count, &len);

  /*Microsoft OS String Descriptor*/
  count += len + 1;
  USBD_string_framework[count++] = 0; //USBD_LANGID_STRING & 0xFF;
  USBD_string_framework[count++] = 0; //USBD_LANGID_STRING >> 8;
  USBD_string_framework[count++] = USBD_IDX_MSFOS_STR;

  uint16_t size = sizeof(USBD_OSStringDesc);
  for (uint32_t i = 0; i < size; i++)
  {
    USBD_string_framework[count + i] = USBD_OSStringDesc[i];
  }
  count += size + 1;
  *length = count;

  /** register vendor request_function **/
  UINT ret = UX_SUCCESS;
  ret = _ux_device_stack_microsoft_extension_register(0x07, vendor_request_function); //0x07 defined in the MS OS descriptor
  if (ret != UX_SUCCESS)
  {
    *length = 0;
    return NULL;
  }

  return USBD_string_framework;
}


/**
  * @brief  USBD_Get_Language_Id_Framework
  *         Return the language_id_framework
  * @param  Length : Length of Language_Id_Framework
  * @retval Pointer to language_id_framework buffer
  */
uint8_t *USBD_Get_Language_Id_Framework(ULONG *length)
{
  uint8_t count = 0U;

  /* Set the language Id in USBD_language_id_framework */
  USBD_language_id_framework[count++] = USBD_LANGID_STRING & 0xFF;
  USBD_language_id_framework[count++] = USBD_LANGID_STRING >> 8;

  /* Get the length of USBD_language_id_framework */
  *length = strlen((const char *) USBD_language_id_framework);

  return USBD_language_id_framework;
}

/* Private functions */

/**
  * @brief  vendor_request_function
  *         Return the device speed descriptor
  * @param  Speed : HIGH or FULL SPEED flag
  * @param  length : length of HIGH or FULL SPEED array
  * @retval Pointer to descriptor buffer
  */
static UINT vendor_request_function(ULONG request, ULONG request_value, ULONG request_index, ULONG request_length,
                                    UCHAR *buff, ULONG *len)
{
  switch (request_index)
  {
    case USB_DESC_TYPE_OS_FEATURE_EXT_PROPERTIES:
    {
      *len = (ULONG) MIN(OS_IDFeatureDescriptor[0], request_length);
      for (int i = 0; i < *len; i++)
      {
        buff[i] = OS_IDFeatureDescriptor[i];
      }
      break;
    }
    case USB_DESC_TYPE_OS_FEATURE_EXT_COMPAT_ID:
    {
      *len = (ULONG) MIN(sizeof(USBD_ExtPropertiesDesc), request_length);
      memcpy(buff, &USBD_ExtPropertiesDesc, *len);
      break;
    }

    default:
      break;
  }

  return UX_SUCCESS;

}

/**
  * @brief  USBD_Desc_GetString
  *         Convert ASCII string into Unicode one
  * @param  desc : descriptor buffer
  * @param  Unicode : Formatted string buffer (Unicode)
  * @param  len : descriptor length
  * @retval None
  */
static void USBD_Desc_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len)
{
  uint8_t idx = 0U;
  uint8_t *pdesc;

  if (desc == NULL)
  {
    return;
  }

  pdesc = desc;
  *len = (uint16_t) USBD_Desc_GetLen(pdesc);

  unicode[idx++] = *(uint8_t *) len;

  while (*pdesc != (uint8_t) '\0')
  {
    unicode[idx++] = *pdesc;
    pdesc++;
  }
}

/**
  * @brief  USBD_Desc_GetLen
  *         return the string length
  * @param  buf : pointer to the ASCII string buffer
  * @retval string length
  */
static uint8_t USBD_Desc_GetLen(uint8_t *buf)
{
  uint8_t len = 0U;
  uint8_t *pbuff = buf;

  while (*pbuff != (uint8_t) '\0')
  {
    len++;
    pbuff++;
  }

  return len;
}

/**
  * @brief  USBD_Device_Framework_Builder
  *         Device Framework builder
  * @param  pdev: device instance
  * @param  pDevFrameWorkDesc: Pointer to the device framework descriptor
  * @param  UserClassInstance: type of the class to be added
  * @param  Speed: Speed parameter HS or FS
  * @retval status
  */
static uint8_t *USBD_Device_Framework_Builder(USBD_DevClassHandleTypeDef *pdev, uint8_t *pDevFrameWorkDesc,
                                              uint8_t *UserClassInstance, uint8_t Speed)
{
  static USBD_DeviceDescTypedef *pDevDesc;
  static USBD_DevQualiDescTypedef *pDevQualDesc;
  uint8_t Idx_Instance = 0U;
  uint8_t NumberClass = 1U;

  /* Set Dev and conf descriptors size to 0 */
  pdev->CurrConfDescSz = 0U;
  pdev->CurrDevDescSz = 0U;

  /* Set the pointer to the device descriptor area*/
  pDevDesc = (USBD_DeviceDescTypedef *) pDevFrameWorkDesc;

  /* Start building the generic device descriptor common part */
  pDevDesc->bLength = (uint8_t) sizeof(USBD_DeviceDescTypedef);
  pDevDesc->bDescriptorType = UX_DEVICE_DESCRIPTOR_ITEM;
  pDevDesc->bcdUSB = USB_BCDUSB;
  pDevDesc->bDeviceClass = 0x00;
  pDevDesc->bDeviceSubClass = 0x00;
  pDevDesc->bDeviceProtocol = 0x00;
  pDevDesc->bMaxPacketSize = USBD_MAX_EP0_SIZE;
  pDevDesc->idVendor = USBD_VID;
  pDevDesc->idProduct = USBD_PID;
  pDevDesc->bcdDevice = 0x0200;
  pDevDesc->iManufacturer = USBD_IDX_MFC_STR;
  pDevDesc->iProduct = USBD_IDX_PRODUCT_STR;
  pDevDesc->iSerialNumber = USBD_IDX_SERIAL_STR;
  pDevDesc->bNumConfigurations = USBD_MAX_NUM_CONFIGURATION;
  pdev->CurrDevDescSz += (uint32_t) sizeof(USBD_DeviceDescTypedef);

  /* qualifier descriptor */
  pDevQualDesc = (USBD_DevQualiDescTypedef *)(pDevFrameWorkDesc + pdev->CurrDevDescSz);
  pDevQualDesc->bLength = (uint8_t) sizeof(USBD_DevQualiDescTypedef);
  pDevQualDesc->bDescriptorType = UX_DEVICE_QUALIFIER_DESCRIPTOR_ITEM;
  pDevQualDesc->bcdDevice = 0x0200;
  pDevQualDesc->Class = 0x00;
  pDevQualDesc->SubClass = 0x00;
  pDevQualDesc->Protocol = 0x00;
  pDevQualDesc->bMaxPacketSize = 0x40;
  pDevQualDesc->bNumConfigurations = 0x01;
  pDevQualDesc->bReserved = 0x00;
  pdev->CurrDevDescSz += (uint32_t) sizeof(USBD_DevQualiDescTypedef);

  /* Build the device framework */
  while (Idx_Instance < USBD_MAX_SUPPORTED_CLASS)
  {
    if ((pdev->classId < USBD_MAX_SUPPORTED_CLASS) && (pdev->NumClasses < USBD_MAX_SUPPORTED_CLASS))
    {
      /* Call the composite class builder */
      (void) USBD_FrameWork_AddClass(pdev, (USBD_CompositeClassTypeDef) UserClassInstance[Idx_Instance], 0, Speed,
                                     (pDevFrameWorkDesc + pdev->CurrDevDescSz));

      /* Increment the ClassId for the next occurrence */
      pdev->classId++;
      pdev->NumClasses++;
    }
    Idx_Instance++;

    /* Count the number of Classes different of CLASS_TYPE_NONE */
    if (UserClassInstance[Idx_Instance] != CLASS_TYPE_NONE)
    {
      NumberClass++;
    }
  }

  return pDevFrameWorkDesc;
}

/**
  * @brief  USBD_FrameWork_AddClass
  *         Register a class in the class builder
  * @param  pdev: device instance
  * @param  pclass: pointer to the class structure to be added
  * @param  class: type of the class to be added (from USBD_CompositeClassTypeDef)
  * @param  cfgidx: configuration index
  * @retval status
  */
static uint8_t USBD_FrameWork_AddClass(USBD_DevClassHandleTypeDef *pdev, USBD_CompositeClassTypeDef class,
                                       uint8_t cfgidx, uint8_t Speed,
                                       uint8_t *pCmpstConfDesc)
{
  if ((pdev->classId < USBD_MAX_SUPPORTED_CLASS) && (pdev->tclasslist[pdev->classId].Active == 0U))
  {
    /* Store the class parameters in the global tab */
    pdev->tclasslist[pdev->classId].ClassId = pdev->classId;
    pdev->tclasslist[pdev->classId].Active = 1U;
    pdev->tclasslist[pdev->classId].ClassType = class;

    /* Call configuration descriptor builder and endpoint configuration builder */
    if (USBD_FrameWork_SS_WCID_ConfigurationDesc(pdev, Speed, pCmpstConfDesc) != UX_SUCCESS)
    {
      return UX_ERROR;
    }
  }

  UNUSED(cfgidx);

  return UX_SUCCESS;
}

/**
  * @brief  USBD_FrameWork_SS_WCID_ConfigurationDesc
  *         Add a new class to the configuration descriptor
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_FrameWork_SS_WCID_ConfigurationDesc(USBD_DevClassHandleTypeDef *pdev, uint8_t Speed,
                                                        uint8_t *pCmpstConfDesc)
{
  uint8_t interface = 0U;

  /* USER CODE FrameWork_AddToConfDesc_0 */

  /* USER CODE FrameWork_AddToConfDesc_0 */

  /* The USB drivers do not set the speed value, so set it here before starting */
  pdev->Speed = Speed;

  /* start building the config descriptor common part */
  if (pdev->classId == 0U)
  {
    /* Intermediate variable to comply with MISRA-C Rule 11.3 */
    USBD_ConfigDescTypedef *ptr = (USBD_ConfigDescTypedef *) pCmpstConfDesc;

    ptr->bLength = (uint8_t) sizeof(USBD_ConfigDescTypedef);
    ptr->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
    ptr->wDescriptorLength = 0U; //USB_SS_WCID_CONFIG_DESC_SIZ;
    ptr->bNumInterfaces = 0U;
    ptr->bConfigurationValue = 1U;
    ptr->iConfiguration = USBD_CONFIG_STR_DESC_IDX;
    ptr->bmAttributes = USBD_CONFIG_BMATTRIBUTES;
    ptr->bMaxPower = USBD_CONFIG_MAXPOWER;
    pdev->CurrConfDescSz += sizeof(USBD_ConfigDescTypedef);

  }

  switch (pdev->tclasslist[pdev->classId].ClassType)
  {
    case CLASS_TYPE_SENSOR_STREAMING:

      /* Find the first available interface slot and Assign number of interfaces */
      interface = USBD_FrameWork_FindFreeIFNbr(pdev);
      pdev->tclasslist[pdev->classId].NumIf = 1U;
      pdev->tclasslist[pdev->classId].Ifs[0] = interface;
      pdev->tclasslist[pdev->classId].Ifs[1] = (uint8_t)(interface + 1U);

      /* Assign endpoint numbers */
      pdev->tclasslist[pdev->classId].NumEps = SS_N_IN_ENDPOINTS;

      //FULL SPEED
      /* Assign IN Endpoints */
      USBD_FrameWork_AssignEp(pdev, DATA_IN_EP1, USBD_EP_TYPE_BULK, SS_WCID_DATA_FS_MAX_PACKET_SIZE);

      USBD_FrameWork_AssignEp(pdev, DATA_IN_EP2, USBD_EP_TYPE_BULK, SS_WCID_DATA_FS_MAX_PACKET_SIZE);

      USBD_FrameWork_AssignEp(pdev, DATA_IN_EP3, USBD_EP_TYPE_BULK, SS_WCID_DATA_FS_MAX_PACKET_SIZE);

      USBD_FrameWork_AssignEp(pdev, DATA_IN_EP4, USBD_EP_TYPE_BULK, SS_WCID_DATA_FS_MAX_PACKET_SIZE);

      USBD_FrameWork_AssignEp(pdev, DATA_IN_EP5, USBD_EP_TYPE_BULK, SS_WCID_DATA_FS_MAX_PACKET_SIZE);

      /* Assign OUT Endpoints */
      USBD_FrameWork_AssignEp(pdev, DATA_OUT_EP1, USBD_EP_TYPE_BULK, SS_WCID_DATA_FS_MAX_PACKET_SIZE);

      /* Configure and Append the Descriptor */
      USBD_FrameWork_SS_WCID_If_Ep_Desc(pdev, (uint32_t) pCmpstConfDesc, &pdev->CurrConfDescSz);

      break;

    default:
      /* USER CODE FrameWork_AddToConfDesc_2 */

      /* USER CODE FrameWork_AddToConfDesc_2 */
      break;
  }

  return UX_SUCCESS;
}

static void USBD_FrameWork_SS_WCID_If_Ep_Desc(USBD_DevClassHandleTypeDef *pdev, uint32_t p_conf, uint32_t *size)
{
  static USBD_IfDescTypedef *pIfDesc;
  static USBD_EpDescTypedef *pEpDesc;

  /* Control Interface Descriptor */

  /* Interface Descriptor */
  pIfDesc = ((USBD_IfDescTypedef *)((uint32_t) p_conf + *size));
  pIfDesc->bLength = (uint8_t) sizeof(USBD_IfDescTypedef);
  pIfDesc->bDescriptorType = USB_DESC_TYPE_INTERFACE;
  pIfDesc->bInterfaceNumber = pdev->tclasslist[pdev->classId].Ifs[0];
  pIfDesc->bAlternateSetting = 0U;
  pIfDesc->bNumEndpoints = SS_N_IN_ENDPOINTS + SS_N_OUT_ENDPOINTS;
  pIfDesc->bInterfaceClass = 0U;
  pIfDesc->bInterfaceSubClass = 0x00U;
  pIfDesc->bInterfaceProtocol = 0x00U;
  pIfDesc->iInterface = 0U;
  *size += (uint32_t) sizeof(USBD_IfDescTypedef);

  for (int i = 0; i < SS_N_IN_ENDPOINTS + SS_N_OUT_ENDPOINTS; i++)
  {
    /* Append Endpoint descriptor to Configuration descriptor */

    /* Append Endpoint descriptor to Configuration descriptor */
    pEpDesc = ((USBD_EpDescTypedef *)((uint32_t) p_conf + *size));
    pEpDesc->bLength = (uint8_t) sizeof(USBD_EpDescTypedef);
    pEpDesc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;
    pEpDesc->bEndpointAddress = pdev->tclasslist[pdev->classId].Eps[i].add;
    pEpDesc->bmAttributes = USBD_EP_TYPE_BULK;
    pEpDesc->wMaxPacketSize = (uint16_t)(pdev->tclasslist[pdev->classId].Eps[i].size);
    if (pdev->Speed == USBD_HIGH_SPEED)
    {
      pEpDesc->bInterval = 0U;
    }
    else
    {
      pEpDesc->bInterval = 0U;
    }
    *size += (uint32_t) sizeof(USBD_EpDescTypedef);

  }

  /* Update Config Descriptor  descriptor */
  ((USBD_ConfigDescTypedef *) p_conf)->bNumInterfaces += 1U;
  ((USBD_ConfigDescTypedef *) p_conf)->wDescriptorLength = *size;
}
/**
  * @brief  USBD_FrameWork_FindFreeIFNbr
  *         Find the first interface available slot
  * @param  pdev: device instance
  * @retval The interface number to be used
  */
static uint8_t USBD_FrameWork_FindFreeIFNbr(USBD_DevClassHandleTypeDef *pdev)
{
  uint32_t idx = 0U;

  /* Unroll all already activated classes */
  for (uint32_t i = 0U; i < pdev->NumClasses; i++)
  {
    /* Unroll each class interfaces */
    for (uint32_t j = 0U; j < pdev->tclasslist[i].NumIf; j++)
    {
      /* Increment the interface counter index */
      idx++;
    }
  }

  /* Return the first available interface slot */
  return (uint8_t) idx;
}

/**
  * @brief  USBD_FrameWork_AssignEp
  *         Assign and endpoint
  * @param  pdev: device instance
  * @param  Add: Endpoint address
  * @param  Type: Endpoint type
  * @param  Sze: Endpoint max packet size
  * @retval none
  */
static void USBD_FrameWork_AssignEp(USBD_DevClassHandleTypeDef *pdev, uint8_t address, uint8_t type, uint32_t size)
{
  uint32_t idx = 0U;

  /* Find the first available endpoint slot */
  while (((idx < (pdev->tclasslist[pdev->classId]).NumEps) && ((pdev->tclasslist[pdev->classId].Eps[idx].is_used) != 0U)))
  {
    /* Increment the index */
    idx++;
  }

  /* Configure the endpoint */
  pdev->tclasslist[pdev->classId].Eps[idx].add = address;
  pdev->tclasslist[pdev->classId].Eps[idx].type = type;
  pdev->tclasslist[pdev->classId].Eps[idx].size = (uint16_t) size;
  pdev->tclasslist[pdev->classId].Eps[idx].is_used = 1U;
}


/**
  * @brief  Convert unsigned long value into string (Hex)
  *         2712847316 --> 0xA1B2C3D4 --> "A1B2C3D4"
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer
  * @param  len: buffer length
  * @retval None
  */
static void UlongToStr(uint32_t value, uint8_t *pbuf, uint8_t len)
{
  uint8_t idx = 0;

  for (idx = 0; idx < (len - 1); idx++)
  {
    if (((value >> 28)) < 0xA)
    {
      pbuf[idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[idx] = (value >> 28) + 'A' - 10;
    }
    value = value << 4;
  }
  pbuf[len - 1] = '\0';
}


