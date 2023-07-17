/**
  ******************************************************************************
  * @file    HS_DataLog.h
  * @author  SRA
  * @brief   header file for high speed datalog library
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
  *
  ******************************************************************************
  */

#ifndef HS_DATALOG_H
#define HS_DATALOG_H

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ST_HS_DATALOG_OK        0
#define ST_HS_DATALOG_ERROR    -1

/**
 * @brief Function
 * @params callbaks
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR otherwise
*/
int hs_datalog_register_usb_hotplug_callback(void(*plug_callback)(), void(*unplug_callback)());

/**
 * @brief Initialization of the ST device managment library.
 *
 * This function populates internal data structures needed to keep track
 * of the st devices currently connected to the host. Each board is automatically
 * assigned with an identification number (id) starting from 0 to
 * (number of devices - 1) and it's caracterized by a univoque serial number.
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 * otherwise
 */
int hs_datalog_open(void);

/**
 * @brief De-initialization of the ST device managment library.
 *
 * This fucntion must be called at the end of the operations.
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 * otherwise
 */
int hs_datalog_close(void);

/**
 * @brief retrieve the number of connected devices.
 *
 * @param nDevices will contain the number of connected devices.
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_device_number(int * nDevices);

//TODO update comments
int hs_datalog_set_boolean_property(int dId, bool value, char * comp_name, char* prop_name, char* sub_prop_name = nullptr);
//TODO update comments
int hs_datalog_set_integer_property(int dId, int value, char * comp_name, char* prop_name, char* sub_prop_name = nullptr);
//TODO update comments
int hs_datalog_set_float_property(int dId, float value, char * comp_name, char* prop_name, char* sub_prop_name = nullptr);
//TODO update comments
int hs_datalog_set_string_property(int dId, char* value, char * comp_name, char* prop_name, char* sub_prop_name = nullptr);

/**
 * @brief to configure a device status
 *
 * @param dId Id of the device to address.
 * @param device_status DeviceStatus.json content
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_set_device_status(int dId, char * device_satus);

/**
 * @brief Send a JSOM message to a device.
 *
 * This function enables the communication with the board by means of user generated
 * JSON messages. It allows the configuration and the managment of the device withouth
 * the need of any additional function. When using this function, the user is in charge
 * of creating and parsing the messages using the correct format.
 *
 * @param dId Id of the device to address.
 * @param msg message to be sent; refer to the documentation for a detailed description
 *        of the message formats
 * @param msg_len length of the message
 * @param data_len length of the returned data in the case of a GET message
 * @param data data returned, internally allocated, to be freed with the provided free function
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_send_message(int dId, char * msg, int msg_len, int * data_len, char ** data);

/**
 * @brief retrieve the boolean value of a specific Property(prop_name) or "sub" Property(sub_prop_name) of the Component(comp_name) on a specific device.
 *
 * @param dId Id of the device to address.
 * @param comp_name name of the DTDL Component to address.
 * @param prop_name name of the DTDL Property to address.
 * @param sub_prop_name [optional] name of the DTDL "sub" Property to address (in case of Object properties).
 * @param value boolean value containing the requested information
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_boolean_property(int dId, bool *value, char* comp_name, char* prop_name, char* sub_prop_name = nullptr);

/**
 * @brief retrieve the integer value of a specific Property(prop_name) or "sub" Property(sub_prop_name) of the Component(comp_name) on a specific device.
 *
 * @param dId Id of the device to address.
 * @param comp_name name of the DTDL Component to address.
 * @param prop_name name of the DTDL Property to address.
 * @param sub_prop_name [optional] name of the DTDL "sub" Property to address (in case of Object properties).
 * @param value integer value containing the requested information
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_integer_property(int dId, int *value, char* comp_name, char* prop_name, char* sub_prop_name = nullptr);

/**
 * @brief retrieve the float value of a specific Property(prop_name) or "sub" Property(sub_prop_name) of the Component(comp_name) on a specific device.
 *
 * @param dId Id of the device to address.
 * @param comp_name name of the DTDL Component to address.
 * @param prop_name name of the DTDL Property to address.
 * @param sub_prop_name [optional] name of the DTDL "sub" Property to address (in case of Object properties).
 * @param value float value containing the requested information
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_float_property(int dId, float *value, char* comp_name, char* prop_name, char* sub_prop_name = nullptr);

/**
 * @brief retrieve the string value of a specific Property(prop_name) or "sub" Property(sub_prop_name) of the Component(comp_name) on a specific device.
 *
 * @param dId Id of the device to address.
 * @param comp_name name of the DTDL Component to address.
 * @param prop_name name of the DTDL Property to address.
 * @param sub_prop_name [optional] name of the DTDL "sub" Property to address (in case of Object properties).
 * @param value string value containing the requested information
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_string_property(int dId, char **value, char* comp_name, char* prop_name, char* sub_prop_name = nullptr);

/**
  * @brief Start data logging on a specific board.
  *
  * @param dId Id of the device to address.
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  *
  * */
int hs_datalog_start_log(int dId, int mode);

/**
  * @brief Stop data logging on a specific board.
  *
  * @param dId Id of the device to address.
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  *
  * */
int hs_datalog_stop_log(int dId);

/**
  * @brief Get the amount of available data from a Component of a specific device.
  *
  * When no callback function is used to handle data ready events, the user should
  * use this function to poll periodically the Device/Component for available data.
  * Data can be retrieved using the hs_datalog_get_data function.
  *
  * @param dId Id of the device.
  * @param comp_name PnPL Component name
  * @param size will contain the amount of data available for the chosen Component
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_available_data_size(int dId, char * comp_name, int * size);

/**
  * @brief Get data from a specific Component.
  *
  * This function is used to get data from a Component when the function
  * hs_datalog_get_available_data_size returns a size > 0
  *
  * @param dId Id of the device.
  * @param comp_name Component name
  * @param data will be filled with data
  * @param size amount of data requested
  * @param actual will be filled with actual size of data retrieved
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_data(int dId, char * comp_name, uint8_t * data, int size, int * actual);

//TODO update comments
int hs_datalog_get_device_status(int dId, char **device);

//TODO update comments
int hs_datalog_get_component_status(int dId, char **comp_status, char * comp_name);

//TODO update comments
int hs_datalog_start_log(int dId, int mode);

//TODO update comments
int hs_datalog_stop_log(int dId);

//TODO update comments
int hs_datalog_set_rtc_time(int dId);

//TODO update comments
int hs_datalog_get_components_number(int dId, int * nComponents);

/**
 * @brief retrieve the number of Sensors Components of a specific device.
 *
 * @param dId Id of the device to address.
 * @param nSensors will contain the number of Sensor Components of the device.
 * @param only_active if true will filter only the active Sensor Components.
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_sensor_components_number(int dId, int *sensor_comp_number, bool only_active);

/**
 * @brief retrieve the number of Algorithm Components of a specific device.
 *
 * @param dId Id of the device to address.
 * @param nof_algo_comps will contain the number of Algorithm Components of the device.
 * @param only_active if true will filter only the active Algorithm Components.
 *
 * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
 */
int hs_datalog_get_algorithm_components_number(int dId, int *nof_algo_comps, bool only_active);

//TODO update comments
int hs_datalog_get_sensor_components_names(int dId, char** names, bool only_active);

//TODO update comments
int hs_datalog_get_algorithm_components_names(int dId, char** names, bool only_active);

//TODO update comments
int hs_datalog_get_presentation(int dId, int *boardId, int *fwId);

//TODO update comments
int hs_datalog_get_identity(int dId, int *boardId, int *fwId);

//TODO update comments
int hs_datalog_get_device_alias(int dId, char ** alias);

//TODO update comments
int hs_datalog_set_device_alias(int dId, char * alias);

/**
  * @brief Send data to a specific device.
  *
  * @param dId Id of the device.
  * @param data data to be sent
  * @param size amount of data
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_send_data(int dId, uint8_t * data, int size);

/**
  * @brief Link a callback to the data ready event of a specific Component.
  *
  * For ease of use, the callback have the following parameters:
  * @param dId Id of the device which generates the event.
  * @param comp_name of the Component which generates the event.
  * @param data pointer to the available data
  * @param size size to the available data
  *
  * The same function can be linked to all the Component events. The specific
  * device and Component which generated the call is then available inside the
  * function.
  *
  * The call is done inside an high priority acquisition thread. User
  * operations inside the callback must be very limited in order not to stuck
  * the acquisition process.
  *
  * User is responsible to read data as soon as possible: data is overwritten
  * when new data is available.
  *
  * This callback hook is meant to be used for advanced operation. For standard
  * scenarios use the polling scheme with the functions:
  * st_usb_datalog_get_data_available_size() and  st_usb_datalog_get_data()
  *
  * @param dId Id of the device.
  * @param comp_name Id of the Component.
  * @param callback callback to be called
  *
  * @return ST_HS_DATALOG_OK if no error occurred or ST_HS_DATALOG_ERROR
  * in case of error
  * */
int hs_datalog_set_data_ready_callback(int dId, char* comp_name, int (*callback)(int dId, char* comp_name, uint8_t *data, int size));

/**
  * @brief Free previously allocated memory.
  *
  * Do not use on memory not allocated by this library.
  *
  * @param ptr the pointer
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_free(char * ptr);

/**
  * @brief Get library version.
  *
  * @param ** version will contain the current library version.
  *
  * @return length of the version string
  * */
int hs_datalog_get_version(char **version);

/**
  * @brief Get the Acquisition information.
  *
  * This function is used to get all the information about the current
  * acquisition. This information includes a name and a description
  * for the acquisition as well as an unique identifier (UUIDAcquisition). It also
  * contains a list of all tags performed during the acquisition procedure.
  *
  * @param dId Id of the device.
  * @param info The name of the acquisition.
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_acquisition_info(int dId, char ** info);

/**
  * @brief Set name and description for the current acquisition.
  *
  * This function is used to set the name and the description for
  * the current acquisition. The acquisition info (json) can be retrived by @hs_datalog_get_acquisition_info.
  *
  * @param dId Id of the device.
  * @param name The name of the acquisition.
  * @param description The description of the acquisition.
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_acquisition_param(int dId, char * name, char * description);

/**
  * @brief Send the UCF (Unico Configuration File) to a specific Sensor Component.
  *
  * Send the UCF (Unico Configuration FIle) to a specific sensor Component in order to
  * configure the Machine Learning Core built-in the sensor component.
  *
  * @param dId Id of the device.
  * @param comp_name Id of the sensor Component.
  * @param UCF_buffer Byte array that contains the UCF file.
  * @param len The length of the UCF FIle.
  *
  *  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_load_ucf_to_mlc(int dID, char* comp_name, uint8_t * ucf_buffer, uint32_t len);

/**
  * @brief Get the maximum number of tags available.
  *
  * Get the maximum number of tags available for each acquisition.
  *
  * @param dId Id of the device.
  * @param n_tags maximum number of tags available.
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_max_tags(int dId, int& n_tags);

/**
  * @brief Get the list of all tags defined for the current acquisition.
  *
  * Get the list of all tags defined for the current acquisition. The function returns a
  * json file.
  *
  * @param dId Id of the device.
  * @param json json file.
  *
  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_available_tags(int dId, char ** json);

/**
  * @brief Set the label of a specific Software tag.
  *
  * The function allows to rename a label for a specific SW tag.
  *
  * @param dId Id of the device.
  * @param tag_comp_name name of the SW Tag Component.
  * @param label name of the label.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_sw_tag_label(int dId, char* tag_comp_name, char * label);

/**
  * @brief Set the label of a specific Software tag.
  *
  * The function allows to rename a label for a specific SW tag.
  *
  * @param dId Id of the device.
  * @param old_label selected SW Tag label.
  * @param new_label new label for the SW Tag.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_sw_tag_label_by_label(int dId, char* old_label, char * new_label);

/**
  * @brief Set the label of a specific Software tag.
  *
  * The function allows to rename a label for a specific SW tag.
  *
  * @param dId Id of the device.
  * @param old_label selected SW Tag label.
  * @param new_label new label for the SW Tag.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_sw_tag_label_by_label(int dId, char* old_label, char * new_label);

/**
  * @brief Get the label of a specific Software tag.
  *
  * The function allows to get the name of a label for a specific SW tag.
  *
  * @param dId Id of the device.
  * @param tag_comp_name name of the HW Tag Component.
  * @param label name of the label.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_sw_tag_label(int dId, char* tag_comp_name, char ** label);

/**
  * @brief Set the label of a specific Hardware tag.
  *
  * The function allows to rename a label for a specific HW tag.
  *
  * @param dId Id of the device.
  * @param tag_comp_name name of the HW Tag Component.
  * @param label name of the label.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_hw_tag_label(int dId, char * tag_comp_name, char * label);

/**
  * @brief Set the label of a specific Hardware tag.
  *
  * The function allows to rename a label for a specific HW tag.
  *
  * @param dId Id of the device.
  * @param old_label selected HW Tag label.
  * @param new_label new label for the HW Tag.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_hw_tag_label_by_label(int dId, char * old_label, char * new_label);

/**
  * @brief Get the label of a specific Hardware tag.
  *
  * The function allows to get the name of a label for a specific HW tag.
  *
  * @param dId Id of the device.
  * @param tag_comp_name HW Tag Component name.
  * @param label name of the label.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_get_hw_tag_label(int dId, char* tag_comp_name, char ** label);

/**
  * @brief Send the command start/stop tagging for a specific Tag.
  *
  * The function allows to send to the device the stop tagging command for a specific
  * Tag.
  *
  * @param dId Id of the device.
  * @param tag_label label of the selected Tag class
  * @param status True to start tagging, False to stop.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_set_on_off_sw_tag(int dId, char* tag_label, bool status);


/**
  * @brief Send the command enable-hw-tag-class for a specific Tag.
  *
  * The function allows to send to the device the enable hw tag class command for a specific
  * HW Tag by Component name.
  *
  * @param dId Id of the device.
  * @param tag_comp_name component name of the hw tag class.
  * @param status tag class enabling status.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_enable_hw_tag(int dId, char * tag_comp_name, bool status);


/**
  * @brief Send the command enable-hw-tag-class for a specific Tag.
  *
  * The function allows to send to the device the enable hw tag class command for a specific
  * HW Tag by label.
  *
  * @param dId Id of the device.
  * @param tag_comp_label hw tag class label.
  * @param status tag class enabling status.

  * @return ST_HS_DATALOG_OK if no error occurred, ST_HS_DATALOG_ERROR
  * otherwise
  * */
int hs_datalog_enable_hw_tag_by_label(int dId, char * tag_comp_label, bool status);




#ifdef __cplusplus
}
#endif


#endif // HS_DATALOG_H
