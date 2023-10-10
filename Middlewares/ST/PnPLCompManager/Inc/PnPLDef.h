/**
  ******************************************************************************
  * @file    PnPLDef.h
  * @author  SRA
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef INC_PNPLDEF_H_
#define INC_PNPLDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* PnP-Like commands codes*/
// CMD TYPE
#define PNPL_CMD_GET            (uint8_t)(0x10)
#define PNPL_CMD_SET            (uint8_t)(0x20)
#define PNPL_CMD_UPDATE_DEVICE  (uint8_t)(0x21)
#define PNPL_CMD_COMMAND        (uint8_t)(0x30)
#define PNPL_CMD_SYSTEM_CONFIG  (uint8_t)(0x40)
#define PNPL_CMD_SYSTEM_INFO    (uint8_t)(0x50)
#define PNPL_CMD_COMPUTE_STREAM_IDS (uint8_t)(0x60)
#define PNPL_CMD_ERROR          (uint8_t)(0x70)

#ifndef COMP_KEY_MAX_LENGTH
#define COMP_KEY_MAX_LENGTH     32
#endif

#define COM_MAX_PNPL_COMPONENTS 20

#define PNPL_CMD_NO_ERROR_CODE      (0x0u)
#define PNPL_CMD_ERROR_CODE         (0x1u)

#define PNPL_INT                    (0x0u)
#define PNPL_FLOAT                  (0x1u)
#define PNPL_STRING                 (0x2u)
#define PNPL_BOOLEAN                (0x3u)
#define PNPL_TELEMETRY              (0x4u)


/**
  * Create type name for _PnPLCommand_t.
  */
typedef struct _PnPLCommand_t PnPLCommand_t;

/**
  *  _PnPLCommand_t internal structure.
  */
struct _PnPLCommand_t
{

  /**
    * Specifies the command type.
    */
  uint8_t comm_type;

  /**
    * Specifies the component name.
    */
  char comp_name[COMP_KEY_MAX_LENGTH];

  /**
   * Command string.
   */
  //???

  /**
   * Command response.
   */
  char* response;

};


/**
  * Create type name for _PnPLTelemetry_t.
  */
typedef struct _PnPLTelemetry_t PnPLTelemetry_t;


/**
  *  _PnPLTelemetry_t internal structure.
  */
struct _PnPLTelemetry_t
{
  /**
    * Specifies the telemetry name.
    */
  char telemetry_name[COMP_KEY_MAX_LENGTH];

  /**
    * Specifies the telemetry value.
    */
  void *telemetry_value;

  /**
    * Specifies the telemetry type.
    */
  uint8_t telemetry_type;

  /**
    * Specifies the number of sub-telemetries.
    */
  uint8_t n_sub_telemetries;
};

#ifdef __cplusplus
}
#endif

#endif /* INC_PNPLDEF_H_ */

