/**
  ******************************************************************************
  * @file    STALS.h
  * @author  IMG SW Application Team
  * @brief   Driver header file
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __STALS__
#define __STALS__

#ifndef __KERNEL__
#include <stdint.h>
#else
#include <linux/types.h>
#endif

/*! Constants  */
/*!\{*/
#define STALS_ALS_MAX_CHANNELS 6              /*!< Number of channels of the STALS */
/*!\}*/

/**
 * @enum STALS_Channel_Id_t
 *
 * Constants listing the channels of the device.
 */
enum STALS_Channel_Id_t {
    STALS_CHANNEL_1             = 0x01,        /*!< channel 1*/
    STALS_CHANNEL_2             = 0x02,        /*!< channel 2*/
    STALS_CHANNEL_3             = 0x04,        /*!< channel 3*/
    STALS_CHANNEL_4             = 0x08,        /*!< channel 4*/
    STALS_CHANNEL_5             = 0x10,        /*!< channel 5*/
    STALS_CHANNEL_6             = 0x20,        /*!< channel 6*/
};


/**
 * @enum STALS_Color_Id_t
 *
 * Constants listing the color light. this is used by the \ref STALS_GetChannelColor function to get what are color filers set on the channels
 */
enum STALS_Color_Id_t {
    STALS_COLOR_IR              = 0x01,        /*!< Color IR*/
    STALS_COLOR_RED             = 0x02,        /*!< Color RED*/
    STALS_COLOR_GREEN           = 0x03,        /*!< Color GREEN*/
    STALS_COLOR_BLUE            = 0x04,        /*!< Color BLUE*/
    STALS_COLOR_UV              = 0x05,        /*!< Color UV*/
    STALS_COLOR_CLEAR           = 0x06,        /*!< Clear filter*/
    STALS_COLOR_CLEAR_IR_CUT    = 0x07,        /*!< Clear filter with ir cut */
    STALS_COLOR_DARK            = 0x08,        /*!< Opaque filter*/
    STALS_COLOR_INVALID         = 0xFF,        /*!< Channel is invalid and cannot be used*/
};

/**
 * @enum STALS_ErrCode_t
 *
 * This enumeration is aimed at defining the different errors that can be returned by the STALS driver
 */
typedef enum  {
    STALS_NO_ERROR = 0,                        /*!< No error */
    STALS_ERROR_INVALID_PARAMS,                /*!< Provided parameters are invalid*/
    STALS_ERROR_INIT,                          /*!< Error in the initialization of the VD6283 device*/
    STALS_ERROR_TIME_OUT,                      /*!< A time out has expired before an operation was completed */
    STALS_ERROR_INVALID_DEVICE_ID,             /*!< The Provided device identifier is invalid*/
    STALS_ERROR_WRITE,                         /*!< The trial to write on the I2C bus has failed*/
    STALS_ERROR_READ,                          /*!< The trial to read from the I2C bus has failed*/
    STALS_ERROR_ALREADY_STARTED,               /*!< The device is already started */
    STALS_ERROR_NOT_STARTED,                   /*!< The device is not started */
    STALS_ERROR_NOT_SUPPORTED,                 /*!< The called function is not supported, likely because not yet implemented*/
    STALS_ERROR_FNCT_DEPRECATED,               /*!< The called function is deprecated*/
    STALS_ERROR_LAST_ERROR_CODE,
} STALS_ErrCode_t;



/**
 * @enum STALS_Control_t
 *
 * This enumeration is aimed at defining the Enable and disable flags
 */
enum STALS_Control_t
{
    STALS_CONTROL_DISABLE = 0,                  /*!< Disable the feature*/
    STALS_CONTROL_ENABLE = 1,                   /*!< Enable the feature*/
};

/**
 * @enum STALS_Control_Id_t
 *
 * This enumeration is aimed at defining the different parameters identifiers
 */
enum STALS_Control_Id_t {
    /*!
     * Control to be used to enable or disable the pedestal\n
     * To enable the pedestal : \ref STALS_SetControl(pHandle, STALS_PEDESTAL_ENABLE, STALS_CONTROL_ENABLE);\n
     * To disable the pedestal : \ref STALS_SetControl(pHandle, STALS_PEDESTAL_ENABLE, STALS_CONTROL_DISABLE);\n
     * To know is the pedestal is enabled : \n
     * enum  STALS_Control_t Flag; \ref STALS_GetControl(pHandle, STALS_PEDESTAL_ENABLE, &Flag);
     */
    STALS_PEDESTAL_ENABLE       = 0,

    /*!
     * Control to set the value of the pedestal\n
     * For example : \ref STALS_SetControl(pHandle, STALS_PEDESTAL_VALUE, 6);
     */
    STALS_PEDESTAL_VALUE        = 1,

    /*!
     * Control to set if dark is output. For VD6283 dark count will be output on channel 2.
     */
    STALS_OUTPUT_DARK_ENABLE    = 3,

    /*!
     * Control to set drive current in sda pad in mA when device is driving sda line.
     * Possible values for VD6283 are 4, 8, 12, 16, 20 mA.
    */
    STALS_SDA_DRIVE_VALUE_MA    = 4,

    /*!
     * Control to read saturation counter value.
    */
    STALS_SATURATION_VALUE      = 5,

    /*!
     * Control to configure als autogain feature\n
     * This feature is deprecated.
     */
    STALS_ALS_AUTOGAIN_CTRL     = 6,

    /*!
     * Control to set if OTP information is use by driver. It is enable by default. Unless you know what you are doing don't change this value\n
     * For example : \ref STALS_SetControl(pHandle, STALS_OTP_USAGE_ENABLE, STALS_CONTROL_DISABLE).
     */
    STALS_OTP_USAGE_ENABLE      = 128,

    /*!
     * Control to get/set workaround state:
     * - use STALS_SetControl() to enable/disable given workaround. Msb bit of
     *   ControlValue control if wa is enable or disable. Others bits are the wa
     *   number.
     * - use STALS_GetControl() to get wa status. pControlValue is then an inout
     *   parameter. you call it with the wa for which you want to know state. on
     *   exit you read msb bit to know wa status.
     */
    STALS_WA_STATE              = 129
};


/**
 * @struct STALS_FlickerInfo_t
 *
 * This structure contains the fields filled by the driver and that contains the discovered information related to the flicker
 */
struct STALS_FlickerInfo_t {
    uint32_t Frequency;                          /*!< Value of the frequency*/
    uint8_t  ConfidenceLevel;                    /*!< Confidence on the relevance of the measured value.*/
    uint8_t  IsMeasureFinish;                    /*!< Value is 1 if measure is finish*/
};

/**
 * @enum STALS_Mode_t
 *
 * This enumeration is aimed at defining the different behavior modes of the STALS device
 */
enum STALS_Mode_t {
    /*!
     * Single shot mode. In this mode, the STALS records and accumulates the light for 1 single period. \n
     * The STALS driver client is responsible for starting the next integration period, by calling the STALS_Start function again
     */
    STALS_MODE_ALS_SINGLE_SHOT  = 0,

    /*!
     * Synchronous mode. This mode, is a continuous measurement mode meaning that this needs to be stopped by calling \ref STALS_Stop.
     * But an handshake is necessary to have the registers updated with the values of the on going measurement. This handshake is performed by
     * the \ref STALS_GetAlsValues function\n
     * Two options are available for the handshake \n
     * 1. by I2C reading. in this case the AC data is available on the GPIO1 pin
     * 2. by interrupt acknowledgment. in this case the AC data is NOT available
     */
    STALS_MODE_ALS_SYNCHRONOUS  = 1,

    /*!
     * Flicker mode. This mode outputs PDM on the GPIO1 or GPIO2 pin.
     */
    STALS_MODE_FLICKER          = 2
};


/**
 * @enum STALS_Mode_t
 *
 * This enumeration is aimed at defining the different behavior modes of the STALS device
 */
enum STALS_FlickerOutputType_t
{
    STALS_FLICKER_OUTPUT_ANALOG = 2,            /*!< Analog. PDM is output to GPIO2.*/
    STALS_FLICKER_OUTPUT_DIGITAL_PDM = 1,       /*!< PDM Digital. This modes needs an external clock to feed the device through the corresponding pin.*/
};


/**
 * @struct STALS_Als_t
 *
 * This structure is aimed at defining the parameters providing the event counts values of the last light integration, for the selected channels
 */
struct STALS_Als_t {
    uint8_t Channels;                                /*!< Flag to be ORED by the driver client in order to understand what are the channels that provide a valid value. 0x3F means that all the channels are valid*/
    uint32_t CountValue[STALS_ALS_MAX_CHANNELS];     /*!< Array providing the event counts value for each of the selected channels. The data is encoded on 24 bits and represents a value in 16.8 fixed point unit. This is value after per device calibration.*/
    uint32_t CountValueRaw[STALS_ALS_MAX_CHANNELS];  /*!< Array providing the event counts value for each of the selected channels. The data is encoded on 24 bits and represents a value in 16.8 fixed point unit. This is value before per device calibration.*/
    uint16_t Gains[STALS_ALS_MAX_CHANNELS];          /*!< Array providing the gains used for measure. The data is encoded on 16 bits and represents a value in 8.8 fixed point unit. */
};


/**
 * This function Initializes the STALS driver
 *
 * @param pDeviceName                      Name of the device. Shall be VD6283
 * @param pClient                          Pointer on an client specific platform specific structure, provided up to the underlying platform adaptation layers
 * @param pHandle                          Pointer on an opaque pointer to be used as the id of the instance of the driver
 *
 * @note SlaveAddress. If set to the default address of the device, then the irq pin does not need to be set to low.\n
 * WARNING : to set a new I2C slave address, the GPIO1 pin MUST be set to low and the Init function will then perform an I2C transaction with this I2C slave address. This transaction, being the first one after the power up of the device,
 * will set this new slave address and all further I2C address shall be performed with this I2C slave address.
 *
 * @note A call to the \ref STALS_Init function shall be done to set the VD6283 device in IDLE mode.
 * @note The VD6283 device needs a delay between powering it and calling \ref STALS_Init. Please refer to the device user manual for more details.
 *
 * \retval  STALS_NO_ERROR                Success
 * \retval  STALS_ERROR_INIT              Could not initialize the driver
 * \retval  STALS_ERROR_INVALID_PARAMS    At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_WRITE             Could not write any data into the device through I2C
 */
STALS_ErrCode_t STALS_Init(char * pDeviceName, void * pClient, void ** pHandle);

/**
 * This function terminates the provided STALS driver instance
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 */
STALS_ErrCode_t STALS_Term(void * pHandle);

/**
 * This function .......
 *
 * @param pVersion                         Pointer on a value that contains the version of the driver once this function is called
 * @param pRevision                        Pointer on a value that contains the revision of the driver once this function is called
 *
 * @note The MAJOR number of version sits in the upper 16 bits of *pVersion, the MINOR number of version sits in the lower 16 bits of *pVersion
 * The *pRevision value contains the revision of the driver
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 */
STALS_ErrCode_t STALS_GetVersion(uint32_t *pVersion, uint32_t *pRevision);


/**
 * This function returns the color filter of the provided channel identifier
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param ChannelId                        Channel Id. Permitted values are 0 up to \ref STALS_ALS_MAX_CHANNELS - 1
 * @param pColor                           Pointer on a value in which the color of the channel is returned
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 */
STALS_ErrCode_t STALS_GetChannelColor(void * pHandle, enum STALS_Channel_Id_t ChannelId, enum STALS_Color_Id_t * pColor);

/**
 * This function sets the exposure time into the device, after having tuned to the closest value that the device can support.
 * Note that a fixed readout period of ~6 ms takes place just after exposure time, needed by the device to set the event count values in the registers
 * It also returns the actual applied value in the device
 *
 * For VD6283  possible values are multiples of 1.6 ms with a range of 1.6 ms to 1.6 s.
 *
 * @note Note Exposure time is irrelevant for flicker detection.
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param ExpoTimeInUs                     Exposure time in microseconds
 * @param pAppliedExpoTimeUs               Pointer on in which the value of the actual exposure time is returned
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_ALREADY_STARTED    Exposure can not be set when the device is running
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 */
STALS_ErrCode_t STALS_SetExposureTime(void * pHandle, uint32_t ExpoTimeInUs, uint32_t *pAppliedExpoTimeUs);

/**
 * This function returns the actual exposure time
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param pAppliedExpoTimeUs               Pointer in which the value of the actual exposure time is returned
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 */
STALS_ErrCode_t STALS_GetExposureTime(void * pHandle, uint32_t *pAppliedExpoTimeUs);

/**
 * This function sets an inter measurement time into the device, after having tuned to the closest value that the device can support.
 * It also returns the actual applied value in the device
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param InterMeasurmentInUs              Inter measurement
 * @param pAppliedInterMeasurmentInUs      Pointer on in which the value of the actual inter measurement time is returned
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_ALREADY_STARTED    Inter measurement can not be set when the device is running
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 */
STALS_ErrCode_t STALS_SetInterMeasurementTime(void * pHandle, uint32_t InterMeasurmentInUs, uint32_t *pAppliedInterMeasurmentInUs);

/**
 * This function returns the actual inter measurement
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param pAppliedInterMeasurmentInUs      Pointer on in which the value of the actual inter measurement time is returned
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 */
STALS_ErrCode_t STALS_GetInterMeasurementTime(void * pHandle, uint32_t *pAppliedInterMeasurmentInUs);


/**
 * This function returns the version of the device
 *
 * @param Handle                           Handle on the driver instance
 * @param pDeviceID                        Pointer in which the ID of the device is returned
 * @param pRevisionID                      Pointer in which the revision of the device is returned
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 */
STALS_ErrCode_t STALS_GetProductVersion(void * pHandle, uint8_t *pDeviceID, uint8_t *pRevisionID );

/**
 * This function sets an analog gain on the provided channel id.
 *
 * @note The gain impacts the signal output amplitude, but not the values returned by STALS_GetFlicker,
 * unless the gain is exceeds a maximum value that will flatten the signal output because of its impact on saturation
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param ChannelId                        this id identifies the channel number. See \ref STALS_Channel_Id_t
 * @param Gain                             Gain in 8.8 fixed point unit
 * @param pAppliedGain                     Pointer in which the value of the actual gain applied in the device is returned. Value in 8.8 fixed point unit
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_ALREADY_STARTED    Gain can not be set when the device is running
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
 */
STALS_ErrCode_t STALS_SetGain(void * pHandle, enum STALS_Channel_Id_t ChannelId, uint16_t Gain, uint16_t *pAppliedGain);

/**
 * This function gets the actual gain applied in the device
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param ChannelId                        this Id identifies the channel number. see \ref STALS_Channel_Id_t
 * @param pAppliedGain                     Pointer in which the value of the actual gain applied in the device is returned. Value in 8.8 fixed point unit
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 */
STALS_ErrCode_t STALS_GetGain(void * pHandle, enum STALS_Channel_Id_t ChannelId, uint16_t *pAppliedGain);

/**
 * This function set GPIO2 and GPIO1 gpio's settings according to FlickerOutputType. Note that those parameters are
 * effectively applied when flicker is started.
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param FlickerOutputType                Data output type for the flicker mode. see \ref STALS_FlickerOutputType_t
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
 */
STALS_ErrCode_t STALS_SetFlickerOutputType(void * pHandle, enum STALS_FlickerOutputType_t FlickerOutputType);

/**
 * This function starts the device
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param Mode                             Mode. shall be \ref STALS_MODE_ALS_SINGLE_SHOT, \ref STALS_MODE_ALS_SYNCHRONOUS or \ref STALS_MODE_FLICKER\n
 * @param Channels.                        For the ALS modes, this is an ORED value of the \ref STALS_Channel_Id_t channels.
 *                                         For the flicker mode, this is one of the \ref STALS_Channel_Id_t channels.\n
 *
 * @note As the ALS and FLICKER modes can run independently, Two consecutive calls to this START function with \ref STALS_MODE_ALS_SYNCHRONOUS and \ref STALS_MODE_FLICKER modes are permitted.
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ALREADY_STARTED          The device is already running in the provided mode or in an incompatible mode
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 */
STALS_ErrCode_t STALS_Start(void * pHandle, enum STALS_Mode_t Mode, uint8_t Channels);

/**
 * This function stops the device
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param Mode                             Mode. shall be \ref STALS_MODE_ALS_SINGLE_SHOT, \ref STALS_MODE_ALS_SYNCHRONOUS or \ref STALS_MODE_FLICKER.
 *
 * @note note
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ALREADY_NOT_STARTED      The device is not running the provided mode
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 */
STALS_ErrCode_t STALS_Stop(void * pHandle, enum STALS_Mode_t Mode);

/**
 * This function that provides the event counts values for the selected channels
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param Channels                         an ORED value of the \ref STALS_Channel_Id_t that permits to select the channels from which the event counts values are to be retrieved.
 * @param pAlsValue                        Pointer on a structure storing the counted events values
 * @param pMeasureValid                    Pointer on a flag telling if the measurement is valid.
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
 * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
 */
STALS_ErrCode_t STALS_GetAlsValues(void * pHandle, uint8_t Channels, struct STALS_Als_t * pAlsValue, uint8_t * pMeasureValid);


/**
 * This function Gets the flicker main harmonic frequency
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param pFlickerInfo                     A pointer on an \ref STALS_FlickerInfo_t structure
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
 * \retval  STALS_ERROR_FNCT_DEPRECATED    The function is not supported
 */
STALS_ErrCode_t STALS_GetFlickerFrequency(void * pHandle, struct STALS_FlickerInfo_t * pFlickerInfo);


/**
 * This function sets a control to the STALS driver
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param ParamId                          Identifier of the param provided
 * @param ControlValue                     The value of the control
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
 * \retval  STALS_ERROR_FNCT_DEPRECATED    The function is not supported
 */
STALS_ErrCode_t STALS_SetControl(void * pHandle, enum STALS_Control_Id_t ControlId, uint32_t ControlValue);

/**
 * This function gets a control from the STALS driver
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param ParamId                          Identifier of the param provided
 * @param pControlValue                    Pointer on a parameter in which the parameter value is set
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
*/
STALS_ErrCode_t STALS_GetControl(void * pHandle, enum STALS_Control_Id_t ControlId, uint32_t * pControlValue);

/**
 * This function return device unique id. Unique id is a null terminated string.
 *
 * @param pHandle                          Opaque pointer used as the id of the instance of the driver
 * @param Uid                              Pointer on a char pointer in which address of uid string will be set.
 *
 * \retval  STALS_NO_ERROR                 Success
 * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
*/
STALS_ErrCode_t STALS_GetUid(void * pHandle, char ** pUid);

#endif

#ifdef __cplusplus
}
#endif
