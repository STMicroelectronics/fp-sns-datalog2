/**
  ******************************************************************************
  * @file    bluenrg_lp_events_cb.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.0.0
  * @date    25-September-2024
  * @brief   Event callbacks for external uC - BlueNRG-x in network coprocessor
  *          mode
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

#include "compiler.h"
#include "bluenrg_lp_events.h"

#include "bluenrg_conf.h"
/**
  * @brief The Disconnection Complete event occurs when a connection is terminated.
  *        The status parameter indicates if the disconnection was successful or not. The
  *        reason parameter indicates the reason for the disconnection if the disconnection
  *        was successful. If the disconnection was not successful, the value of the
  *        reason parameter can be ignored by the Host. For example, this can be the
  *        case if the Host has issued the Disconnect command and there was a parameter
  *        error, or the command was not presently allowed, or a connection_handle
  *        that didn't correspond to a connection was given.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *        For proprietary error code refer to Error codes section
  * @param connection_handle connection_handle which was disconnected.
  * Values:
  - 0x0000 ... 0x0EFF
  * @param reason reason for disconnection.
  * Values:
  - 0x00: Success
  - 0x01: Unknown HCI Command
  - 0x02: Unknown Connection Identifier
  - 0x03: Hardware Failure
  - 0x04: Page Timeout
  - 0x05: Authentication Failure
  - 0x06: PIN or Key Missing
  - 0x07: Memory Capacity Exceeded
  - 0x08: Connection Timeout
  - 0x09: Connection Limit Exceeded
  - 0x0A: Synchronous Connection Limit to a Device Exceeded
  - 0x0B: ACL Connection Already Exists
  - 0x0C: Command Disallowed
  - 0x0D: Connection Rejected Due To Limited Resources
  - 0x0E: Connection Rejected Due To Security Reasons
  - 0x0F: Connection Rejected due to Unacceptable BD_ADDR
  - 0x10: Connection Accept Timeout Exceeded
  - 0x11: Unsupported Feature Or Parameter Value
  - 0x12: Invalid HCI Command Parameters
  - 0x13: Remote User Terminated Connection
  - 0x14: Remote Device Terminated Connection due to Low Resources
  - 0x15: Remote Device Terminated Connection due to Power Off
  - 0x16: Connection Terminated By Local Host
  - 0x17: Repeated Attempts
  - 0x18: Pairing Not Allowed
  - 0x19: Unknown LMP PDU
  - 0x1A: Unsupported Remote Feature / Unsupported LMP Feature
  - 0x1B: SCO Offset Rejected
  - 0x1C: SCO Interval Rejected
  - 0x1D: SCO Air Mode Rejected
  - 0x1E: Invalid LMP Parameters
  - 0x1F: Unspecified Error
  - 0x20: Unsupported LMP Parameter Value
  - 0x21: Role Change Not Allowed
  - 0x22: LMP Response Timeout / LL Response Timeout
  - 0x23: LMP Error Transaction Collision
  - 0x24: LMP PDU Not Allowed
  - 0x25: Encryption Mode Not Acceptable
  - 0x26: Link Key cannot be Changed
  - 0x27: Requested QoS Not Supported
  - 0x28: Instant Passed
  - 0x29: Pairing With Unit Key Not Supported
  - 0x2A: Different Transaction Collision
  - 0x2C: QoS Unacceptable Parameter
  - 0x2D: QoS Rejected
  - 0x2E: Channel Assessment Not Supported
  - 0x2F: Insufficient Security
  - 0x30: Parameter Out Of Mandatory Range
  - 0x32: Role Switch Pending
  - 0x34: Reserved Slot Violation
  - 0x35: Role Switch Failed
  - 0x36: Extended Inquiry Response Too Large
  - 0x37: Secure Simple Pairing Not Supported by Host
  - 0x38: Host Busy - Pairing
  - 0x39: Connection Rejected due to No Suitable Channel Found
  - 0x3A: Controller Busy
  - 0x3B: Unacceptable Connection Interval
  - 0x3C: Directed Advertising Timeout
  - 0x3D: Connection Terminated Due to MIC Failure
  - 0x3E: Connection Failed to be Established
  - 0x3F: MAC of the 802.11 AMP
  - 0x41: Failed
  - 0x42: Invalid parameters
  - 0x43: Busy
  - 0x44: Invalid length
  - 0x45: Pending
  - 0x46: Not allowed
  - 0x47: GATT error
  - 0x48: Address not resolved
  - 0x50: Invalid CID
  - 0x5A: CSRK not found
  - 0x5B: IRK not found
  - 0x5C: Device not found in DB
  - 0x5D: Security DB full
  - 0x5E: Device not bonded
  - 0x5F: Device in blacklist
  - 0x60: Invalid handle
  - 0x61: Invalid parameter
  - 0x62: Out of handles
  - 0x63: Invalid operation
  - 0x64: Insufficient resources
  - 0x65: Insufficient encryption key size
  - 0x66: Characteristic already exist
  - 0x82: No valid slot
  - 0x83: Short window
  - 0x84: New interval failed
  - 0x85: Too large interval
  - 0x86: Slot length failed
  - 0xFA: Flash read failed
  - 0xFB: Flash write failed
  - 0xFC: Flash erase failed
  * @retval None
  */
WEAK_FUNCTION(void hci_disconnection_complete_event(uint8_t status,
                                                    uint16_t connection_handle,
                                                    uint8_t reason))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_disconnection_complete_event\r\n");

}
/**
  * @brief The Encryption Change event is used to indicate that the change of the encryption
  *        mode has been completed. The connection_handle will be a connection_handle
  *        for an ACL connection. The encryption_enabled event parameter
  *        specifies the new encryption_enabled parameter for the connection_handle
  *        specified by the connection_handle event parameter. This event will occur on
  *        both devices to notify the Hosts when Encryption has changed for the specified
  *        connection_handle between two devices. Note: This event shall not be generated
  *        if encryption is paused or resumed; during a role switch, for example.
  *        The meaning of the encryption_enabled parameter depends on whether the
  *        Host has indicated support for Secure Connections in the Secure_Connections_Host_Support
  *        parameter. When Secure_Connections_Host_Support is
  *        'disabled' or the connection_handle refers to an LE link, the Controller shall
  *        only use encryption_enabled values 0x00 (OFF) and 0x01 (ON).
  *        (See Bluetooth Specification v.4.1, Vol. 2, Part E, 7.7.8)
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *        For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle for which the command is given.
  * Values:
  - 0x0000 ... 0x0EFF
  * @param encryption_enabled Link Level Encryption.
  * Values:
  - 0x00: Link Level Encryption OFF
  - 0x01: Link Level Encryption is ON with AES-CCM
  * @retval None
  */
WEAK_FUNCTION(void hci_encryption_change_event(uint8_t status,
                                               uint16_t connection_handle,
                                               uint8_t encryption_enabled))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_encryption_change_event\r\n");
}
/**
  * @brief The Read Remote Version Information Complete event is used to indicate the
  *        completion of the process obtaining the version information of the remote Controller
  *        specified by the connection_handle event parameter. The connection_handle
  *        shall be for an ACL connection.
  *        The Version event parameter defines the specification version of the LE Controller.
  *        The manufacturer_name event parameter indicates the manufacturer
  *        of the remote Controller. The Subversion event parameter is controlled
  *        by the manufacturer and is implementation dependent. The Subversion
  *        event parameter defines the various revisions that each version of the Bluetooth
  *        hardware will go through as design processes change and errors are
  *        fixed. This allows the software to determine what Bluetooth hardware is being
  *        used and, if necessary, to work around various bugs in the hardware.
  *        When the connection_handle is associated with an LE-U logical link, the Version
  *        event parameter shall be Link Layer VersNr parameter, the manufacturer_name
  *        event parameter shall be the CompId parameter, and the Subversion
  *        event parameter shall be the SubVersNr parameter.
  *        (See Bluetooth Specification v.4.1, Vol. 2, Part E, 7.7.12)
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *        For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle for which the command is given.
  * Values:
  - 0x0000 ... 0x0EFF
  * @param version Version of the Current LMP in the remote Controller
  * @param manufacturer_name Manufacturer Name of the remote Controller
  * @param subversion Subversion of the LMP in the remote Controller
  * @retval None
  */
WEAK_FUNCTION(void hci_read_remote_version_information_complete_event(uint8_t status,
                                                                      uint16_t connection_handle,
                                                                      uint8_t version,
                                                                      uint16_t manufacturer_name,
                                                                      uint16_t subversion))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_read_remote_version_information_complete_event\r\n");
}
/**
  * @brief The Hardware Error event is used to indicate some implementation specific type of hardware
  *        failure for the controller. This event is used to notify the Host that a hardware failure
  *        has occurred in the Controller.
  * @param hardware_code Hardware Error Event code.
  *        Error code 0x01 and 0x02 are errors generally caused by hardware issue on the PCB;
  *        another possible cause is a slow crystal startup.
  *        In the latter case, the HS_STARTUP_TIME in the device configuration needs to be tuned.
  *        Error code 0x03 indicates an internal error of the protocol stack.
  *        After this event is recommended to force device reset.
  *        Values:
  *        0x01: Radio state error
  *        0x02: Timer overrun error
  *        0x03: Internal queue overflow error
  * @retval None
  */
WEAK_FUNCTION(void hci_hardware_error_event(uint8_t hardware_code))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_hardware_error_event\r\n");
}
/**
  * @brief 'The Number Of Completed Packets event is used by the Controller to indicate
  *        to the Host how many HCI Data Packets have been completed (transmitted or
  *        flushed) for each connection_handle since the previous Number Of Completed
  *        Packets event was sent to the Host. This means that the corresponding
  *        buffer space has been freed in the Controller. Based on this information, and
  *        the HC_Total_Num_ACL_Data_Packets and HC_Total_Num_Synchronous_-
  *        Data_Packets return parameter of the Read_Buffer_Size command, the Host
  *        can determine for which Connection_Handles the following HCI Data Packets
  *        should be sent to the Controller. The Number Of Completed Packets event
  *        must not be sent before the corresponding Connection Complete event. While
  *        the Controller has HCI data packets in its buffer, it must keep sending the Number
  *        Of Completed Packets event to the Host at least periodically, until it finally
  *        reports that all the pending ACL Data Packets have been transmitted or
  *        flushed.
  * @param number_of_handles The number of Connection_Handles and Num_HCI_Data_Packets parameters
  *        pairs contained in this event
  * @param handle_packets_pair_entry See @ref handle_packets_pair_entry_t
  * @retval None
  */
WEAK_FUNCTION(void hci_number_of_completed_packets_event(uint8_t number_of_handles,
                                                         handle_packets_pair_entry_t handle_packets_pair_entry[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_number_of_completed_packets_event\r\n");
}
/**
  * @brief 'This event is used to indicate that the Controller's data buffers have been overflowed.
  *        This can occur if the Host has sent more packets than allowed. The
  *        link_type parameter is used to indicate that the overflow was caused by ACL data.
  * @param link_type On which type of channel overflow has occurred.
  *        Values:
  *        - 0x01: ACL Buffer Overflow
  * @retval None
  */
WEAK_FUNCTION(void hci_data_buffer_overflow_event(uint8_t link_type))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_data_buffer_overflow_event\r\n");
}
/**
  * @brief 'The Encryption Key Refresh Complete event is used to indicate to the Host
  *        that the encryption key was refreshed on the given connection_handle any
  *        time encryption is paused and then resumed.
  *        If the Encryption Key Refresh Complete event was generated due to an
  *        encryption pause and resume operation embedded within a change connection
  *        link key procedure, the Encryption Key Refresh Complete event shall be sent
  *        prior to the Change Connection Link Key Complete event.
  *        If the Encryption Key Refresh Complete event was generated due to an
  *        encryption pause and resume operation embedded within a role switch procedure,
  *        the Encryption Key Refresh Complete event shall be sent prior to the
  *        Role Change event.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *        For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle for which the command is given.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @retval None
  */
WEAK_FUNCTION(void hci_encryption_key_refresh_complete_event(uint8_t status,
                                                             uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_encryption_key_refresh_complete_event\r\n");
}

/**
  * @brief The HCI_Authenticated_Payload_Timeout_Expired event is used to
  *        indicate that a packet containing a valid MIC on the connection_handle
  *        was not received within the authenticatedPayloadTO. Note: A Host may
  *        choose to disconnect the link when this occurs.
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @retval None
  */
WEAK_FUNCTION(void hci_authenticated_payload_timeout_expired_event(uint16_t onnection_Handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_authenticated_payload_timeout_expired_event\r\n");
}

/**
  * @brief The LE Connection Complete event indicates to both of the Hosts forming the
  *        connection that a new connection has been created. Upon the creation of the
  *        connection a connection_handle shall be assigned by the Controller, and
  *        passed to the Host in this event. If the connection establishment fails this event
  *        shall be provided to the Host that had issued the LE_Create_Connection command.
  *        This event indicates to the Host which issued a LE_Create_Connection
  *        command and received a Command status event if the connection
  *        establishment failed or was successful.
  *        The master_clock_accuracy parameter is only valid for a slave. On a master,
  *        this parameter shall be set to 0x00.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *               For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle to be used to identify the connection with the peer device.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param role Role of the local device in the connection.
  *        Values:
  *        - 0x00: Master
  *        - 0x01: Slave
  * @param peer_address_type The address type of the peer device.
  *        Values:
  *        - 0x00: Public Device Address
  *        - 0x01: Random Device Address
  * @param peer_address Public Device Address or Random Device Address of the peer device
  * @param conn_interval Connection interval used on this connection.
  *        Time = N * 1.25 msec
  *        Values:
  *        - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
  * @param conn_latency Slave latency for the connection in number of connection events.
  *        Values:
  *        - 0x0000 ... 0x01F3
  * @param supervision_timeout Supervision timeout for the LE Link.
  *        It shall be a multiple of 10 ms and larger than (1 + connSlaveLatency) * connInterval * 2.
  *        Time = N * 10 msec.
  *        Values:
  - 0x000A (100 ms)  ... 0x0C80 (32000 ms)
  * @param master_clock_accuracy Master clock accuracy. Only valid for a slave.
  *        Values:
  *        - 0x00: 500 ppm
  *        - 0x01: 250 ppm
  *        - 0x02: 150 ppm
  *        - 0x03: 100 ppm
  *        - 0x04: 75 ppm
  *        - 0x05: 50 ppm
  *        - 0x06: 30 ppm
  *        - 0x07: 20 ppm
  * @retval None
  */
WEAK_FUNCTION(void hci_le_connection_complete_event(uint8_t status,
                                                    uint16_t connection_handle,
                                                    uint8_t role,
                                                    uint8_t peer_address_type,
                                                    uint8_t peer_address[6],
                                                    uint16_t conn_interval,
                                                    uint16_t conn_latency,
                                                    uint16_t supervision_timeout,
                                                    uint8_t master_clock_accuracy))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_connection_complete_event\r\n");
}
/**
  * @brief The LE Advertising Report event indicates that a Bluetooth device or multiple
  *        Bluetooth devices have responded to an active scan or received some information
  *        during a passive scan. The Controller may queue these advertising reports
  *        and send information from multiple devices in one LE Advertising Report event.
  * @param num_reports Number of responses in this event.
  *        Values:
  *        - 0x01
  * @param advertising_report See @ref advertising_report_t
  * @retval None
  */
WEAK_FUNCTION(void hci_le_advertising_report_event(uint8_t num_reports,
                                                   advertising_report_t advertising_report[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_advertising_report_event\r\n");
}
/**
  * @brief The LE Connection Update Complete event is used to indicate that the Controller
  *        process to update the connection has completed.
  *        On a slave, if no connection parameters are updated, then this event shall not be issued.
  *        On a master, this event shall be issued if the Connection_Update command was sent.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *               For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle to be used to identify the connection with the peer device.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param conn_interval Connection interval used on this connection.
  *        Time = N * 1.25 msec
  *        Values:
  *        - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
  * @param conn_latency Slave latency for the connection in number of connection events.
  *        Values:
  *        - 0x0000 ... 0x01F3
  * @param supervision_timeout Supervision timeout for the LE Link.
  *        It shall be a multiple of 10 ms and larger than (1 + connSlaveLatency) * connInterval * 2.
  *        Time = N * 10 msec.
  *        Values:
  *        - 0x000A (100 ms)  ... 0x0C80 (32000 ms)
  * @retval None
  */
WEAK_FUNCTION(void hci_le_connection_update_complete_event(uint8_t status,
                                                           uint16_t connection_handle,
                                                           uint16_t conn_interval,
                                                           uint16_t conn_latency,
                                                           uint16_t supervision_timeout))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_connection_update_complete_event\r\n");
}
/**
  * @brief The LE Read Remote Used Features Complete event is used to indicate the
  *        completion of the process of the Controller obtaining the used features of the
  *        remote Bluetooth device specified by the connection_handle event parameter.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *               For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle to be used to identify the connection with the peer device.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param le_features Bit Mask List of used LE features. For details see LE Link Layer specification.
  * @retval None
  */
WEAK_FUNCTION(void hci_le_read_remote_used_features_complete_event(uint8_t status,
                                                                   uint16_t connection_handle,
                                                                   uint8_t le_features[8]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_read_remote_used_features_complete_event\r\n");
}
/**
  * @brief The LE Long Term Key Request event indicates that the master device is
  *        attempting to encrypt or re-encrypt the link and is requesting the Long Term
  *        Key from the Host. (See [Vol 6] Part B, Section 5.1.3).
  * @param connection_handle Connection handle to be used to identify the connection with the peer device.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param random_number 64-bit random number
  * @param encrypted_diversifier 16-bit encrypted diversifier
  * @retval None
  */
WEAK_FUNCTION(void hci_le_long_term_key_request_event(uint16_t connection_handle,
                                                      uint8_t random_number[8],
                                                      uint16_t encrypted_diversifier))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_long_term_key_request_event\r\n");
}
/**
  * @brief The LE Data Length Change event notifies the Host of a change to either the maximum Payload length
  *        or the maximum transmission time of Data Channel PDUs in either direction.
  *        The values reported are the maximum that will actually be used on the connection following the change.
  * @param connection_handle connection_handle to be used to identify a connection.
  * @param max_tx_octets The maximum number of payload octets in a Link Layer Data Channel PDU that the local
  *                      Controller will send on this connection
  *                      (connEffectiveMaxTxOctets defined in [Vol 6] Part B, Section 4.5.10).
  *                      Range 0x001B-0x00FB (0x0000 - 0x001A and 0x00FC - 0xFFFF Reserved for future use)
  * @param max_tx_time The maximum time that the local Controller will take to send a Link Layer Data Channel PDU
  *                    on this connection (connEffectiveMaxTx-Time defined in [Vol 6] Part B, Section 4.5.10).
  *                    Range 0x0148-0x0848 (0x0000 - 0x0127 and 0x0849 - 0xFFFF Reserved for future use)
  * @param max_rx_octets The maximum number of payload octets in a Link Layer Data Channel PDU that the local
  *                      controller expects to receive on this connection
  *                      (connEfectiveMaxRxOctets defined in [Vol 6] Part B, Section 4.5.10).
  *                      Range 0x001B-0x00FB (0x0000 - 0x001A and 0x00FC - 0xFFFF Reserved for future use)
  * @param max_rx_time The maximum time that the local Controller expects to take to receive a Link Layer Data Channel
  *                    PDU on this connection (connEffectiveMax-RxTime defined in [Vol 6] Part B, Section 4.5.10).
  *                    Range 0x0148-0x0848 (0x0000 - 0x0127 and 0x0849 - 0xFFFF Reserved for future use)
  * @retval None
  */
WEAK_FUNCTION(void hci_le_data_length_change_event(uint16_t connection_handle,
                                                   uint16_t max_tx_octets,
                                                   uint16_t max_tx_time,
                                                   uint16_t max_rx_octets,
                                                   uint16_t max_rx_time))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_data_length_change_event\r\n");
}
/**
  * @brief This event is generated when local P-256 key generation is complete.
  * @param status For standard error codes see Bluetooth specification,
  *               Vol. 2, part D. For proprietary error code refer to Error codes section
  * @param local_p256_public_key Local P-256 public key.
  * @retval None
  */
WEAK_FUNCTION(void hci_le_read_local_p256_public_key_complete_event(uint8_t status,
                                                                    uint8_t local_p256_public_key[64]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_read_local_p256_public_key_complete_event\r\n");
}
/**
  * @brief This event indicates that LE Diffie Hellman key generation has been completed
by the Controller.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *               For proprietary error code refer to Error codes section
  * @param dh_key Diffie Hellman Key
  * @retval None
  */
WEAK_FUNCTION(void hci_le_generate_dhkey_complete_event(uint8_t status,
                                                        uint8_t dh_key[32]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_generate_dhkey_complete_event\r\n");
}
/**
  * @brief The LE Enhanced Connection Complete event indicates to both of the Hosts
  *        forming the connection that a new connection has been created. Upon the
  *        creation of the connection a connection_handle shall be assigned by the
  *        Controller, and passed to the Host in this event. If the connection establishment
  *        fails, this event shall be provided to the Host that had issued the
  *        LE_Create_Connection command.
  *        If this event is unmasked and LE Connection Complete event is unmasked,
  *        only the LE Enhanced Connection Complete event is sent when a new
  *        connection has been completed.
  *        This event indicates to the Host that issued a LE_Create_Connection
  *        command and received a Command status event if the connection
  *        establishment failed or was successful.
  *        The master_clock_accuracy parameter is only valid for a slave. On a master,
  *        this parameter shall be set to 0x00.
  * @param status For standard error codes see Bluetooth specification, Vol. 2, part D.
  *               For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle to be used to identify the connection with the peer device.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param role Role of the local device in the connection.
  *        Values:
  *        - 0x00: Master
  *        - 0x01: Slave
  * @param peer_address_type
  *        Values:
  *        - 0x00: Public Device Address
  *        - 0x01: Random Device Address
  *        - 0x02: Public Identity Address (Corresponds to Resolved Private Address)
  *        - 0x03: Random (Static) Identity Address (Corresponds to Resolved Private Address)
  * @param peer_address Public Device Address, Random Device Address, Public Identity
  *                     Address or Random (static) Identity Address of the device to be connected.
  * @param local_resolvable_private_address Resolvable Private Address being used by the local device for this
  *                                          connection. This is only valid when the own_address_type is set to
  *                                          0x02 or 0x03. For other own_address_type values, the Controller shall
  *                                          return all zeros.
  * @param peer_resolvable_private_address Resolvable Private Address being used by the peer device for this
  *                                        connection. This is only valid for peer_address_type 0x02 and 0x03.
  *                                        For other peer_address_type values, the Controller shall return all zeros.
  * @param conn_interval Connection interval used on this connection.
  *        Time = N * 1.25 msec
  *        Values:
  *        - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
  * @param conn_latency Slave latency for the connection in number of connection events.
  *        Values:
  *        - 0x0000 ... 0x01F3
  * @param supervision_timeout Supervision timeout for the LE Link.
  *                            It shall be a multiple of 10 ms and larger than (1+connSlaveLatency)*connInterval*2.
  *        Time = N * 10 msec.
  *        Values:
  *        - 0x000A (100 ms)  ... 0x0C80 (32000 ms)
  * @param master_clock_accuracy Master clock accuracy. Only valid for a slave.
  *        Values:
  *        - 0x00: 500 ppm
  *        - 0x01: 250 ppm
  *        - 0x02: 150 ppm
  *        - 0x03: 100 ppm
  *        - 0x04: 75 ppm
  *        - 0x05: 50 ppm
  *        - 0x06: 30 ppm
  *        - 0x07: 20 ppm
  * @retval None
  */
WEAK_FUNCTION(void hci_le_enhanced_connection_complete_event(uint8_t status,
                                                             uint16_t connection_handle,
                                                             uint8_t role,
                                                             uint8_t peer_address_type,
                                                             uint8_t peer_address[6],
                                                             uint8_t local_resolvable_private_address[6],
                                                             uint8_t peer_resolvable_private_address[6],
                                                             uint16_t conn_interval,
                                                             uint16_t conn_latency,
                                                             uint16_t supervision_timeout,
                                                             uint8_t master_clock_accuracy))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_enhanced_connection_complete_event\r\n");
}
/**
  * @brief The LE Direct Advertising Report event indicates that directed advertisements
have been received where the advertiser is using a resolvable private address
for the InitA field in the ADV_DIRECT_IND PDU and the
scanning_filter_policy is equal to 0x02 or 0x03, see HCI_LE_Set_Scan_Parameters.
direct_address_type and Direct_Addres is the address the directed
advertisements are being directed to. address_type and Address is the
address of the advertiser sending the directed advertisements.
  * @param num_reports Number of responses in this event.
  * Values:
  - 0x01
  * @param direct_advertising_report See @ref direct_advertising_report_t
  * @retval None
  */
WEAK_FUNCTION(void hci_le_direct_advertising_report_event(uint8_t num_reports,
                                                          direct_advertising_report_t direct_advertising_report[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_direct_advertising_report_event\r\n");
}
/**
  * @}
  */
/**
  *@defgroup ACI_GAP_events ACI GAP events
  *@brief ACI GAP events layer.
  *@{
  */
/* ACI GAP events */
/**
  * @brief This event is generated by the controller when the limited discoverable mode ends due to
timeout. The timeout is 180 seconds.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_limited_discoverable_event(void))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_limited_discoverable_event\r\n");
}
/**
  * @brief This event is generated when the pairing process has completed successfully or a pairing
procedure timeout has occurred or the pairing has failed. This is to notify the application that
we have paired with a remote device so that it can take further actions or to notify that a
timeout has occurred so that the upper layer can decide to disconnect the link.
  * @param connection_handle Connection handle on which the pairing procedure completed
  * @param status Pairing status
  * Values:
  - 0x00: Success
  - 0x01: Timeout
  - 0x02: Failed
  * @param reason Pairing reason error code
  * Values:
  - 0x00
  - 0x01: PASSKEY_ENTRY_FAILED
  - 0x02: OOB_NOT_AVAILABLE
  - 0x03: AUTH_REQ_CANNOT_BE_MET
  - 0x04: CONFIRM_VALUE_FAILED
  - 0x05: PAIRING_NOT_SUPPORTED
  - 0x06: INSUFF_ENCRYPTION_KEY_SIZE
  - 0x07: CMD_NOT_SUPPORTED
  - 0x08: UNSPECIFIED_REASON
  - 0x09: VERY_EARLY_NEXT_ATTEMPT
  - 0x0A: SM_INVALID_PARAMS
  - 0x0B: SMP_SC_DHKEY_CHECK_FAILED
  - 0x0C: SMP_SC_NUMCOMPARISON_FAILED
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_pairing_complete_event(uint16_t connection_handle,
                                                  uint8_t status,
                                                  uint8_t reason))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_pairing_complete_event\r\n");
}
/**
  * @brief This event is generated by the Security manager to the application when a passkey is
required for pairing. When this event is received, the application has to respond with the
@ref aci_gap_pass_key_resp command.
  * @param connection_handle Connection handle for which the passkey has been requested.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_pass_key_req_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_pass_key_req_event\r\n");
}
/**
  * @brief This event is generated by the Security manager to the application when the application has
set that authorization is required for reading/writing of attributes. This event will be
generated as soon as the pairing is complete. When this event is received,
@ref aci_gap_authorization_resp command should be used to respond by the application.
  * @param connection_handle Connection handle for which authorization has been requested.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_authorization_req_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_authorization_req_event\r\n");
}
/**
  * @brief This event is generated when the slave security request is successfully sent to the master.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_slave_security_initiated_event(void))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_slave_security_initiated_event\r\n");
}
/**
  * @brief This event is generated when a pairing request is issued in response to a slave security
request from a master which has previously bonded with the slave. When this event is
received, the upper layer has to issue the command @ref aci_gap_allow_rebond in order to
allow the slave to continue the pairing process with the master.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_bond_lost_event(void))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_bond_lost_event\r\n");
}
/**
  * @brief This event is sent by the GAP to the upper layers when a procedure previously started has
  *        been terminated by the upper layer or has completed for any other reason
  * @param procedure_code Terminated procedure.
  * Values:
  - 0x01: LIMITED_DISCOVERY_PROC
  - 0x02: GENERAL_DISCOVERY_PROC
  - 0x04: NAME_DISCOVERY_PROC
  - 0x08: AUTO_CONNECTION_ESTABLISHMENT_PROC
  - 0x10: GENERAL_CONNECTION_ESTABLISHMENT_PROC
  - 0x20: SELECTIVE_CONNECTION_ESTABLISHMENT_PROC
  - 0x40: DIRECT_CONNECTION_ESTABLISHMENT_PROC
  - 0x80: OBSERVATION_PROC
  * @param status For standard error codes see Bluetooth specification,
  *               Vol. 2, part D. For proprietary error code refer to Error codes section
  * @param data_length Length of Data in octets
  * @param data Procedure Specific Data:
- For Name Discovery Procedure: the name of the peer device if the procedure completed successfully.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_proc_complete_event(uint8_t procedure_code,
                                               uint8_t status,
                                               uint8_t data_length,
                                               uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_proc_complete_event\r\n");
}
/**
  * @brief This event is sent only by a privacy enabled Peripheral. The event is sent to the
upper layers when the peripheral is unsuccessful in resolving the resolvable
address of the peer device after connecting to it.
  * @param connection_handle Connection handle for which the private address could not be
resolved with any of the stored IRK's.
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_addr_not_resolved_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_addr_not_resolved_event\r\n");
}
/**
  * @brief This event is sent only during SC v.4.2 Pairing, when Numeric Comparison Association model is selected,
  *        in order to show the Numeric Value generated, and to ask for Confirmation to the User. When this event
  *        is received, the application has to respond with the
  * @ref aci_gap_numeric_comparison_value_confirm_yesno command
  * @param connection_handle Connection handle related to the underlying Pairing
  * @param numeric_value
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_numeric_comparison_value_event(uint16_t connection_handle,
                                                          uint32_t numeric_value))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_numeric_comparison_value_event\r\n");
}
/**
  * @brief This event is sent only during SC v.4.2 Pairing, when Keypress Notifications are supported,
  *        in order to show the input type signalled by the peer device, having Keyboard only I/O capabilities.
  *        When this event is received, no action is required to the User.
  * @param connection_handle Connection handle related to the underlying Pairing
  * @param notification_type Type of Keypress input notified/signaled by peer device
  *                          (having Keyboard only I/O capabilities)
  * @retval None
  */
WEAK_FUNCTION(void aci_gap_keypress_notification_event(uint16_t connection_handle,
                                                       uint8_t notification_type))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gap_keypress_notification_event\r\n");
}

/**
  * @brief This event is generated to the application by the GATT server when a client modifies any
  *        attribute on the server, as consequence of one of the following GATT procedures:
  *        - write without response
  *        - signed write without response
  *        - write characteristic value
  *        - write long characteristic value
  *        - reliable write.
  * @param connection_handle The connection handle which modified the attribute.
  * @param attr_handle Handle of the attribute that was modified.
  * @param offset SoC mode: the offset is never used and it is always 0. Network coprocessor mode:
  *               - Bits 0-14: offset of the reported value inside the attribute.
  *               - Bit 15: if the entire value of the attribute does not fit inside a single
  *                         ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that
  *                         other ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param attr_data_length Length of attr_data in octets
  * @param attr_data The modified value
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_attribute_modified_event(uint16_t connection_handle,
                                                     uint16_t attr_handle,
                                                     uint16_t offset,
                                                     uint16_t attr_data_length,
                                                     uint8_t attr_data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_attribute_modified_event\r\n");
}
/**
  * @brief This event is generated by the client/server to the application on a GATT timeout (30 seconds).
  *        This is a critical event that should not happen during normal operating conditions.
  *        It is an indication of either a major disruption in the communication link or a mistake
  *        in the application which does not provide a reply to GATT procedures.
  *        After this event, the GATT channel is closed and no more GATT communication can be performed.
  *        The applications is expected to issue an @ref aci_gap_terminate to disconnect from the peer device.
  *        It is important to leave an 100 ms blank window before sending the @ref aci_gap_terminate, since
  *        immediately after this event, system could save important information in non volatile memory.
  * @param connection_handle Connection handle on which the GATT procedure has timed out
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_proc_timeout_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_proc_timeout_event\r\n");
}
/**
  * @brief This event is generated in response to an Exchange MTU request. See
  *        @ref aci_gatt_exchange_config.
  * @param connection_handle Connection handle related to the response
  * @param server_rx_mtu ATT_MTU value agreed between server and client
  * @retval None
  */
WEAK_FUNCTION(void aci_att_exchange_mtu_resp_event(uint16_t connection_handle,
                                                   uint16_t server_rx_mtu))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_exchange_mtu_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Find Information Request. See
  *        @ref aci_att_find_info_req and Find Information Response in Bluetooth Core v4.1 spec.
  * @param connection_handle Connection handle related to the response
  * @param format Format of the hanndle-uuid pairs
  * @param event_data_length Length of handle_uuid_pair in octets
  * @param handle_uuid_pair A sequence of handle-uuid pairs. if format=1, each pair is:[2 octets for handle,
  *                         2 octets for UUIDs], if format=2, each pair is:[2 octets for handle, 16 octets for UUIDs]
  * @retval None
  */
WEAK_FUNCTION(void aci_att_find_info_resp_event(uint16_t connection_handle,
                                                uint8_t format,
                                                uint8_t event_data_length,
                                                uint8_t handle_uuid_pair[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_find_info_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a @ref aci_att_find_by_type_value_req
  * @param connection_handle Connection handle related to the response
  * @param num_of_handle_pair Number of attribute, group handle pairs
  * @param attr_group_handle_pair See @ref attribute_group_handle_pair_t
  * @retval None
  */
WEAK_FUNCTION(void aci_att_find_by_type_value_resp_event(uint16_t connection_handle,
                                                         uint8_t num_of_handle_pair,
                                                         attribute_group_handle_pair_t attr_group_handle_pair[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_find_by_type_value_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a @ref aci_att_read_by_type_req. See
  *        @ref aci_gatt_find_included_services and @ref aci_gatt_disc_all_char_desc.
  * @param connection_handle Connection handle related to the response
  * @param handle_value_pair_length The size of each attribute handle-value pair
  * @param data_length Length of handle_value_pair_data in octets
  * @param handle_value_pair_data Attribute Data List as defined in Bluetooth Core v4.1 spec.
  *                               A sequence of handle-value pairs: [2 octets for Attribute Handle,
  *                               (handle_value_pair_length - 2 octets) for Attribute Value]
  * @retval None
  */
WEAK_FUNCTION(void aci_att_read_by_type_resp_event(uint16_t connection_handle,
                                                   uint8_t handle_value_pair_length,
                                                   uint8_t data_length,
                                                   uint8_t handle_value_pair_data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_read_by_type_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Read Request. See @ref aci_gatt_read_char_value.
  * @param connection_handle Connection handle related to the response
  * @param event_data_length Length of following data
  * @param attribute_value The value of the attribute.
  * @retval None
  */
WEAK_FUNCTION(void aci_att_read_resp_event(uint16_t connection_handle,
                                           uint8_t event_data_length,
                                           uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_read_resp_event\r\n");
}
/**
  * @brief This event can be generated during a read long characteristic value procedure.
  *        See @ref aci_gatt_read_long_char_value.
  * @param connection_handle Connection handle related to the response
  * @param event_data_length Length of following data
  * @param attribute_value Part of the attribute value.
  * @retval None
  */
WEAK_FUNCTION(void aci_att_read_blob_resp_event(uint16_t connection_handle,
                                                uint8_t event_data_length,
                                                uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_read_blob_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Read Multiple Request.
  * @param connection_handle Connection handle related to the response
  * @param event_data_length Length of following data
  * @param set_of_values A set of two or more values.
  *                      A concatenation of attribute values for each of the attribute handles
  *                      in the request in the order that they were requested.
  * @retval None
  */
WEAK_FUNCTION(void aci_att_read_multiple_resp_event(uint16_t connection_handle,
                                                    uint8_t event_data_length,
                                                    uint8_t set_of_values[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_read_multiple_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Read By Group Type Request. See
  *        @ref aci_gatt_disc_all_primary_services.
  * @param connection_handle Connection handle related to the response
  * @param Attribute_Data_Length The size of each attribute data
  * @param data_length Length of attribute_data_list in octets
  * @param attribute_data_list Attribute Data List as defined in Bluetooth Core v4.1 spec.
  *                            A sequence of attribute handle, end group handle, attribute value tuples:
  *                            [2 octets for Attribute Handle, 2 octets End Group Handle,
  *                             (Attribute_Data_Length - 4 octets) for Attribute Value]
  * @retval None
  */
WEAK_FUNCTION(void aci_att_read_by_group_type_resp_event(uint16_t connection_handle,
                                                         uint8_t Attribute_Data_Length,
                                                         uint8_t data_length,
                                                         uint8_t attribute_data_list[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_read_by_group_type_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a @ref aci_att_prepare_write_req.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute to be written
  * @param offset The offset of the first octet to be written.
  * @param part_attribute_value_length Length of part_attribute_value in octets
  * @param part_attribute_value The value of the attribute to be written
  * @retval None
  */
WEAK_FUNCTION(void aci_att_prepare_write_resp_event(uint16_t connection_handle,
                                                    uint16_t attribute_handle,
                                                    uint16_t offset,
                                                    uint8_t part_attribute_value_length,
                                                    uint8_t part_attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_prepare_write_resp_event\r\n");
}
/**
  * @brief This event is generated in response to an Execute Write Request.
  * @param connection_handle Connection handle related to the response
  * @retval None
  */
WEAK_FUNCTION(void aci_att_exec_write_resp_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_exec_write_resp_event\r\n");
}
/**
  * @brief This event is generated when a GATT client procedure completes either with error or
successfully.
  * @param connection_handle Connection handle related to the response
  * @param error_code Indicates whether the procedure completed with an error or was successful
  * Values:
  - 0x00: Success
  - 0x01: Unknown HCI Command
  - 0x02: Unknown Connection Identifier
  - 0x03: Hardware Failure
  - 0x04: Page Timeout
  - 0x05: Authentication Failure
  - 0x06: PIN or Key Missing
  - 0x07: Memory Capacity Exceeded
  - 0x08: Connection Timeout
  - 0x09: Connection Limit Exceeded
  - 0x0A: Synchronous Connection Limit to a Device Exceeded
  - 0x0B: ACL Connection Already Exists
  - 0x0C: Command Disallowed
  - 0x0D: Connection Rejected Due To Limited Resources
  - 0x0E: Connection Rejected Due To Security Reasons
  - 0x0F: Connection Rejected due to Unacceptable BD_ADDR
  - 0x10: Connection Accept Timeout Exceeded
  - 0x11: Unsupported Feature Or Parameter Value
  - 0x12: Invalid HCI Command Parameters
  - 0x13: Remote User Terminated Connection
  - 0x14: Remote Device Terminated Connection due to Low Resources
  - 0x15: Remote Device Terminated Connection due to Power Off
  - 0x16: Connection Terminated By Local Host
  - 0x17: Repeated Attempts
  - 0x18: Pairing Not Allowed
  - 0x19: Unknown LMP PDU
  - 0x1A: Unsupported Remote Feature / Unsupported LMP Feature
  - 0x1B: SCO Offset Rejected
  - 0x1C: SCO Interval Rejected
  - 0x1D: SCO Air Mode Rejected
  - 0x1E: Invalid LMP Parameters
  - 0x1F: Unspecified Error
  - 0x20: Unsupported LMP Parameter Value
  - 0x21: Role Change Not Allowed
  - 0x22: LMP Response Timeout / LL Response Timeout
  - 0x23: LMP Error Transaction Collision
  - 0x24: LMP PDU Not Allowed
  - 0x25: Encryption Mode Not Acceptable
  - 0x26: Link Key cannot be Changed
  - 0x27: Requested QoS Not Supported
  - 0x28: Instant Passed
  - 0x29: Pairing With Unit Key Not Supported
  - 0x2A: Different Transaction Collision
  - 0x2C: QoS Unacceptable Parameter
  - 0x2D: QoS Rejected
  - 0x2E: Channel Assessment Not Supported
  - 0x2F: Insufficient Security
  - 0x30: Parameter Out Of Mandatory Range
  - 0x32: Role Switch Pending
  - 0x34: Reserved Slot Violation
  - 0x35: Role Switch Failed
  - 0x36: Extended Inquiry Response Too Large
  - 0x37: Secure Simple Pairing Not Supported by Host
  - 0x38: Host Busy - Pairing
  - 0x39: Connection Rejected due to No Suitable Channel Found
  - 0x3A: Controller Busy
  - 0x3B: Unacceptable Connection Interval
  - 0x3C: Directed Advertising Timeout
  - 0x3D: Connection Terminated Due to MIC Failure
  - 0x3E: Connection Failed to be Established
  - 0x3F: MAC of the 802.11 AMP
  - 0x41: Failed
  - 0x42: Invalid parameters
  - 0x43: Busy
  - 0x44: Invalid length
  - 0x45: Pending
  - 0x46: Not allowed
  - 0x47: GATT error
  - 0x48: Address not resolved
  - 0x50: Invalid CID
  - 0x5A: CSRK not found
  - 0x5B: IRK not found
  - 0x5C: Device not found in DB
  - 0x5D: Security DB full
  - 0x5E: Device not bonded
  - 0x5F: Device in blacklist
  - 0x60: Invalid handle
  - 0x61: Invalid parameter
  - 0x62: Out of handles
  - 0x63: Invalid operation
  - 0x64: Insufficient resources
  - 0x65: Insufficient encryption key size
  - 0x66: Characteristic already exist
  - 0x82: No valid slot
  - 0x83: Short window
  - 0x84: New interval failed
  - 0x85: Too large interval
  - 0x86: Slot length failed
  - 0xFA: Flash read failed
  - 0xFB: Flash write failed
  - 0xFC: Flash erase failed
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_proc_complete_event(uint16_t connection_handle,
                                                uint8_t error_code))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_proc_complete_event\r\n");
}
/**
  * @brief This event is generated when an Error Response is received from the server. The error
response can be given by the server at the end of one of the GATT discovery procedures.
This does not mean that the procedure ended with an error, but this error event is part of the
procedure itself.
  * @param connection_handle Connection handle related to the response
  * @param req_op_code The request that generated this error response
  * @param attribute_handle The attribute handle that generated this error response
  * @param error_code The reason why the request has generated an error response (ATT error codes)
  * Values:
  - 0x01: Invalid handle
  - 0x02: Read not permitted
  - 0x03: Write not permitted
  - 0x04: Invalid PDU
  - 0x05: Insufficient authentication
  - 0x06: Request not supported
  - 0x07: Invalid offset
  - 0x08: Insufficient authorization
  - 0x09: Prepare queue full
  - 0x0A: Attribute not found
  - 0x0B: Attribute not long
  - 0x0C: Insufficient encryption key size
  - 0x0D: Invalid attribute value length
  - 0x0E: Unlikely error
  - 0x0F: Insufficient encryption
  - 0x10: Unsupported group type
  - 0x11: Insufficient resources
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_error_resp_event(uint16_t connection_handle,
                                             uint8_t req_op_code,
                                             uint16_t attribute_handle,
                                             uint8_t error_code))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_error_resp_event\r\n");
}
/**
  * @brief This event can be generated during a "Discover Characteristics By UUID" procedure or a
  *        "Read using Characteristic UUID" procedure.
  *        The attribute value will be a service declaration as defined in Bluetooth Core v4.1spec
  *        (vol.3, Part G, ch. 3.3.1), when a "Discover Characteristics By UUID" has been started. It will
  *        be the value of the Characteristic if a* "Read using Characteristic UUID" has been
  *        performed.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The attribute value will be a service declaration as defined in Bluetooth Core v4.0 spec
  *        (vol.3, Part G, ch. 3.3.1), when a "Discover Characteristics By UUID" has been started.
  *        It will be the value of the Characteristic if a "Read using Characteristic UUID" has been performed.
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t connection_handle,
                                                              uint16_t attribute_handle,
                                                              uint8_t attribute_value_length,
                                                              uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_disc_read_char_by_uuid_resp_event\r\n");
}
/**
  * @brief This event is given to the application when a write request, write command or signed write
  *        command is received by the server from the client. This event will be given to the application
  *        only if the event bit for this event generation is set when the characteristic was added.
  *        When this event is received, the application has to check whether the value being requested
  *        for write can be allowed to be written and respond with the command @ref aci_gatt_write_resp.
  *        The details of the parameters of the command can be found. Based on the response from
  *        the application, the attribute value will be modified by the stack. If the write is rejected by the
  *        application, then the value of the attribute will not be modified. In case of a write REQ, an
  *        error response will be sent to the client, with the error code as specified by the application.
  *        In case of write/signed write commands, no response is sent to the client but the attribute is
  *        not modified.
  * @param connection_handle Handle of the connection on which there was the request to write the attribute
  * @param attribute_handle The handle of the attribute
  * @param data_length Length of Data field
  * @param data The data that the client has requested to write
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_write_permit_req_event(uint16_t connection_handle,
                                                   uint16_t attribute_handle,
                                                   uint8_t data_length,
                                                   uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_write_permit_req_event\r\n");
}
/**
  * @brief This event is given to the application when a read request or read blob request is received
  *        by the server from the client. This event will be given to the application only if the event bit
  *        for this event generation is set when the characteristic was added.
  *        On receiving this event, the application can update the value of the handle if it desires and
  *        when done, it has to send the @ref aci_gatt_allow_read command to indicate to the stack that it
  *        can send the response to the client.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param offset Contains the offset from which the read has been requested
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_read_permit_req_event(uint16_t connection_handle,
                                                  uint16_t attribute_handle,
                                                  uint16_t offset))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_read_permit_req_event\r\n");
}
/**
  * @brief Each time BLE FW stack raises the error code @ref ble_status_insufficient_resources (0x64),
the @ref aci_gatt_tx_pool_available_event event is generated as soon as the available buffer size
is greater than maximum ATT MTU (on stack versions below v2.1 this event is generated when at least 2 packets
with MTU of 23 bytes are available).
  * @param connection_handle Connection handle related to the request
  * @param available_buffers Not used.
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_tx_pool_available_event(uint16_t connection_handle,
                                                    uint16_t available_buffers))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_tx_pool_available_event\r\n");
}
/**
  * @brief This event is generated when the client has sent the confirmation to a previously sent indication
  * @param connection_handle Connection handle related to the event
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_server_confirmation_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_server_confirmation_event\r\n");
}
/**
  * @brief This event is given to the application when a prepare write request
  *        is received by the server from the client. This event will be given to the application
  *        only if the event bit for this event generation is set when the characteristic was added.
  *        When this event is received, the application has to check whether the value being requested
  *        for write can be allowed to be written and respond with the command @ref aci_gatt_write_resp.
  *        Based on the response from the application, the attribute value will be modified by the stack.
  *        If the write is rejected by the application, then the value of the attribute will not be modified
  *        and an error response will be sent to the client, with the error code as specified by the application.
  * @param connection_handle Handle of the connection on which there was the request to write the attribute
  * @param attribute_handle The handle of the attribute
  * @param offset The offset from which the prepare write has been requested
  * @param data_length Length of Data field
  * @param data The data that the client has requested to write
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_prepare_write_permit_req_event(uint16_t connection_handle,
                                                           uint16_t attribute_handle,
                                                           uint16_t offset,
                                                           uint8_t data_length,
                                                           uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_prepare_write_permit_req_event\r\n");
}
/**
  * @}
  */
/**
  *@defgroup ACI_L2CAP_events ACI L2CAP events
  *@brief ACI L2CAP events layer.
  *@{
  */
/* ACI L2CAP events */
/**
  * @brief This event is generated when the master responds to the connection update request packet
with a connection update response packet.
  * @param connection_handle Connection handle referring to the COS Channel where the Disconnection has been received.
  * @param result
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_connection_update_resp_event(uint16_t connection_handle,
                                                          uint16_t result))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_connection_update_resp_event\r\n");
}
/**
  * @brief This event is generated when the master does not respond to the connection update
request packet with a connection update response packet or a command reject packet
within 30 seconds.
  * @param connection_handle Handle of the connection related to this
L2CAP procedure.
  * @param data_length Length of following data
  * @param data
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_proc_timeout_event(uint16_t connection_handle,
                                                uint8_t data_length,
                                                uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_proc_timeout_event\r\n");
}
/**
  * @brief The event is given by the L2CAP layer when a connection update request is received from
the slave. The upper layer which receives this event has to respond by sending a
@ref aci_l2cap_connection_parameter_update_resp command.
  * @param connection_handle Handle of the connection related to this
L2CAP procedure.
  * @param identifier This is the identifier which associate the request to the
response.
  * @param l2_cap_length Length of the L2CAP connection update request.
  * @param interval_min Minimum value for the connection event interval. This shall be less
than or equal to conn_interval_max.
Time = N * 1.25 msec.
  * Values:
  - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
  * @param interval_max Maximum value for the connection event interval. This shall be
greater than or equal to conn_interval_min.
Time = N * 1.25 msec.
  * Values:
  - 0x0006 (7.50 ms)  ... 0x0C80 (4000.00 ms)
  * @param slave_latency Slave latency for the connection in number of connection events.
  * Values:
  - 0x0000 ... 0x01F3
  * @param timeout_multiplier Defines connection timeout parameter in the following manner: Timeout Multiplier * 10ms.
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_connection_update_req_event(uint16_t connection_handle,
                                                         uint8_t identifier,
                                                         uint16_t l2_cap_length,
                                                         uint16_t interval_min,
                                                         uint16_t interval_max,
                                                         uint16_t slave_latency,
                                                         uint16_t timeout_multiplier))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_connection_update_req_event\r\n");
}
/**
  * @brief This event is generated when the master responds to the connection update request packet
with a command reject packet.
  * @param connection_handle Connection handle referring to the COS Channel where the Disconnection has been received.
  * @param identifier This is the identifier which associate the request to the
response.
  * @param reason reason
  * @param data_length Length of following data
  * @param data Data field associated with reason
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_command_reject_event(uint16_t connection_handle,
                                                  uint8_t identifier,
                                                  uint16_t reason,
                                                  uint8_t data_length,
                                                  uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_command_reject_event\r\n");
}
/**
  * @}
  */
/**
  *@defgroup ACI_HAL_events ACI HAL events
  *@brief ACI HAL events layer.
  *@{
  */
/* ACI HAL events */
/**
  * @brief This event inform the application that the network coprocessor has been reset.
  *        If the reason code is a system crash, a following event @ref aci_blue_crash_info_event
  *        will provide more information regarding the system crash details.
  * @param reason_code reason code describing why device was reset and in which mode is operating
  *                    (Updater or Normal mode)
  *                    Values:
  *                    - 0x01: Firmware started properly
  *                    - 0x02: Updater mode entered with ACI command
  *                    - 0x03: Updater mode entered due to bad Blue Flag
  *                    - 0x04: Updater mode entered due to IRQ pin
  *                    - 0x05: System reset due to watchdog
  *                    - 0x06: System reset due to lockup
  *                    - 0x07: System reset due to brownout reset
  *                    - 0x08: System reset due to crash
  *                    - 0x09: System reset due to ECC error
  * @retval None
  */
WEAK_FUNCTION(void aci_blue_initialized_event(uint8_t reason_code))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_blue_initialized_event reason_code=%x\r\n", reason_code);
}
/**
  * @brief 'This event is generated when an overflow occurs in the event queue read by the
external microcontroller. This is normally caused when the external microcontroller does
not read pending events. The returned bitmap indicates which event has been lost. Please
 note that one bit set to 1 indicates one or more occurrences of the particular events.
The event ACI_BLUE_EVENTS_LOST_EVENT cannot be lost and it will inserted in the
event queue as soon as a position is freed in the event queue. This event should not happen under normal
operating condition where external microcontroller promptly reads events signaled by IRQ pin.
It is provided to detected unexpected behavior of the external microcontroller or to
allow application to recover situations where critical events are lost.
  * @param lost_events Bitmap of lost events. Each bit indicates one or more occurrences of the specific event.
  * Flags:
  - 0x0000000000000001: HCI_DISCONNECTION_COMPLETE_EVENT
  - 0x0000000000000002: HCI_ENCRYPTION_CHANGE_EVENT
  - 0x0000000000000004: HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT
  - 0x0000000000000008: HCI_COMMAND_COMPLETE_EVENT
  - 0x0000000000000010: HCI_COMMAND_STATUS_EVENT
  - 0x0000000000000020: HCI_HARDWARE_ERROR_EVENT
  - 0x0000000000000040: HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT
  - 0x0000000000000080: HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT
  - 0x0000000000000100: ACI_BLUE_INITIALIZED_EVENT
  - 0x0000000000000200: ACI_GAP_LIMITED_DISCOVERABLE_EVENT
  - 0x0000000000000400: ACI_GAP_PAIRING_COMPLETE_EVENT
  - 0x0000000000000800: ACI_GAP_PASS_KEY_REQ_EVENT
  - 0x0000000000001000: ACI_GAP_AUTHORIZATION_REQ_EVENT
  - 0x0000000000002000: ACI_GAP_SLAVE_SECURITY_INITIATED_EVENT
  - 0x0000000000004000: ACI_GAP_BOND_LOST_EVENT
  - 0x0000000000008000: ACI_GAP_PROC_COMPLETE_EVENT
  - 0x0000000000010000: ACI_GAP_ADDR_NOT_RESOLVED_EVENT
  - 0x0000000000020000: ACI_L2CAP_CONNECTION_UPDATE_RESP_EVENT
  - 0x0000000000040000: ACI_L2CAP_PROC_TIMEOUT_EVENT
  - 0x0000000000080000: ACI_L2CAP_CONNECTION_UPDATE_REQ_EVENT
  - 0x0000000000100000: ACI_GATT_ATTRIBUTE_MODIFIED_EVENT
  - 0x0000000000200000: ACI_GATT_PROC_TIMEOUT_EVENT
  - 0x0000000000400000: ACI_ATT_EXCHANGE_MTU_RESP_EVENT
  - 0x0000000000800000: ACI_ATT_FIND_INFO_RESP_EVENT
  - 0x0000000001000000: ACI_ATT_FIND_BY_TYPE_VALUE_RESP_EVENT
  - 0x0000000002000000: ACI_ATT_READ_BY_TYPE_RESP_EVENT
  - 0x0000000004000000: ACI_ATT_READ_RESP_EVENT
  - 0x0000000008000000: ACI_ATT_READ_BLOB_RESP_EVENT
  - 0x0000000010000000: ACI_ATT_READ_MULTIPLE_RESP_EVENT
  - 0x0000000020000000: ACI_ATT_READ_BY_GROUP_TYPE_RESP_EVENT
  - 0x0000000040000000: ACI_ATT_WRITE_RESP_EVENT
  - 0x0000000080000000: ACI_ATT_PREPARE_WRITE_RESP_EVENT
  - 0x0000000100000000: ACI_ATT_EXEC_WRITE_RESP_EVENT
  - 0x0000000200000000: ACI_GATT_INDICATION_EVENT
  - 0x0000000400000000: ACI_GATT_NOTIFICATION_EVENT
  - 0x0000000800000000: ACI_GATT_PROC_COMPLETE_EVENT
  - 0x0000001000000000: ACI_GATT_ERROR_RESP_EVENT
  - 0x0000002000000000: ACI_GATT_DISC_READ_CHAR_BY_UUID_RESP_EVENT
  - 0x0000004000000000: ACI_GATT_WRITE_PERMIT_REQ_EVENT
  - 0x0000008000000000: ACI_GATT_READ_PERMIT_REQ_EVENT
  - 0x0000010000000000: ACI_GATT_READ_MULTI_PERMIT_REQ_EVENT
  - 0x0000020000000000: ACI_GATT_TX_POOL_AVAILABLE_EVENT
  - 0x0000040000000000: ACI_GATT_SERVER_CONFIRMATION_EVENT
  - 0x0000080000000000: ACI_GATT_PREPARE_WRITE_PERMIT_REQ_EVENT
  - 0x0000100000000000: HCI_LE_CONNECTION_COMPLETE_EVENT
  - 0x0000200000000000: HCI_LE_ADVERTISING_REPORT_EVENT
  - 0x0000400000000000: HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT
  - 0x0000800000000000: HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT
  - 0x0001000000000000: HCI_LE_LONG_TERM_KEY_REQUEST_EVENT
  - 0x0002000000000000: HCI_LE_DATA_LENGTH_CHANGE_EVENT
  - 0x0004000000000000: HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT
  - 0x0008000000000000: HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT
  - 0x0010000000000000: HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT
  - 0x0020000000000000: HCI_LE_DIRECT_ADVERTISING_REPORT_EVENT
  - 0x0040000000000000: ACI_GAP_NUMERIC_COMPARISON_VALUE_EVENT
  - 0x0080000000000000: ACI_GAP_KEYPRESS_NOTIFICATION_EVENT
  * @retval None
  */
WEAK_FUNCTION(void aci_blue_events_lost_event(uint8_t lost_events[8]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_blue_events_lost_event\r\n");
}
/**
  * @brief 'This event is given to the application after the @ref aci_blue_initialized_event
when a system crash is detected. This events returns system crash information for debugging purposes.
Information reported are useful to understand the root cause of the crash.
  * @param crash_type Crash type
  * Values:
  - 0x00: Assert failed
  - 0x01: NMI fault
  - 0x02: Hard fault
  * @param sp Stack pointer
  * @param r0 Register r0
  * @param r1 Register r1
  * @param r2 Register r2
  * @param r3 Register r3
  * @param r12 Register r12
  * @param lr Link register
  * @param pc Program counter where crash occurred
  * @param x_psr x_psr register
  * @param debug_data_length Length of debug_data field
  * @param debug_data Debug data
  * @retval None
  */
WEAK_FUNCTION(void aci_blue_crash_info_event(uint8_t crash_type,
                                             uint32_t sp,
                                             uint32_t r0,
                                             uint32_t r1,
                                             uint32_t r2,
                                             uint32_t r3,
                                             uint32_t r12,
                                             uint32_t lr,
                                             uint32_t pc,
                                             uint32_t x_psr,
                                             uint8_t debug_data_length,
                                             uint8_t debug_data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_blue_crash_info_event\r\n");
}
/**
  * @brief 'This event is generated when the device completes a radio activity and provide information when
  *        a new radio activity will be performed.
  *        Informtation provided includes type of radio activity and absolute time in system ticks when a new
  *        radio activity is schedule, if any. Application can use this information to schedule user activities
  *        synchronous to selected radio activitities. A command @ref aci_hal_set_radio_activity_mask is provided
  *        to enable radio activity events of user interests, by default no events are enabled.
  *        User should take into account that enablinng radio events in application with intense radio activity
  *        could lead to a fairly high rate of events generated.
  *        Application use cases includes synchronizing notification with connection interval, switiching antenna
  *        at the end of advertising or performing flash erase operation while radio is idle.
  * @param last_state Completed radio events
  *        Values:
  *        - 0x00: Idle
  *        - 0x01: Advertising
  *        - 0x02: Connection event slave
  *        - 0x03: Scanning
  *        - 0x04: Connection request
  *        - 0x05: Connection event master
  *        - 0x06: TX test mode
  *        - 0x07: RX test mode
  * @param next_state Incoming radio events
  *        Values:
  *        - 0x00: Idle
  *        - 0x01: Advertising
  *        - 0x02: Connection event slave
  *        - 0x03: Scanning
  *        - 0x04: Connection request
  *        - 0x05: Connection event master
  *        - 0x06: TX test mode
  *        - 0x07: RX test mode
  * @param next_state_sys_time 32bit absolute current time expressed in internal time units.
  * @retval None
  */
WEAK_FUNCTION(void aci_hal_end_of_radio_activity_event(uint8_t last_state,
                                                       uint8_t next_state,
                                                       uint32_t next_state_sys_time))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_hal_end_of_radio_activity_event\r\n");
}
/**
  * @brief This event is reported to the application after a scan request is received and a scan response
is scheduled to be transmitted.
  * @param rssi N Size: 1 Octet (signed integer)
Units: dBm
  * Values:
  - 127: RSSI not available
  - -127 ... 20
  * @param peer_address_type 0x00 Public Device Address
0x01 Random Device Address
0x02 Public Identity Address (Corresponds to Resolved Private Address)
0x03 Random (Static) Identity Address (Corresponds to Resolved Private Address)
  * Values:
  - 0x00: Public Device Address
  - 0x01: Random Device Address
  - 0x02: Public Identity Address
  - 0x03: Random (Static) Identity Address
  * @param peer_address Public Device Address or Random Device Address of the peer device
  * @retval None
  */
WEAK_FUNCTION(void aci_hal_scan_req_report_event(int8_t rssi,
                                                 uint8_t peer_address_type,
                                                 uint8_t peer_address[6]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_hal_scan_req_report_event\r\n");
}
/**
  * @brief This event is generated to report firmware error information.
  * @param fw_error_type FW Error type
  * Values:
  - 0x01: L2CAP recombination failure
  * @param data_length Length of Data in octets
  * @param data The error event info
  * @retval None
  */
WEAK_FUNCTION(void aci_hal_fw_error_event(uint8_t fw_error_type,
                                          uint8_t data_length,
                                          uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_hal_fw_error_event\r\n");
}
/**
  * @brief This event is generated when the amount of transmitted test packets
  *        specified with aci_hal_transmitter_test_packets() has been reached.
  * @param number_of_packets The number of test packets actually transmitted.
  * @retval None
  */
WEAK_FUNCTION(void aci_hal_le_test_end_event(uint16_t number_of_packets))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_hal_le_test_end_event\r\n");
}
/**
  * @brief Event generated when a request or a response is received from the peer
  *        to create an L2CAP channel.
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param event_type
  *        Values:
  *        - 0x00: L2CAP_CONN_REQ
  *        - 0x01: L2CAP_CONN_RESP
  * @param result Result of the connection request if event_type is
  *        L2CAP_CONN_RESP. To be ignored if event_type is L2CAP_CONN_REQ.
  *        Values:
  *        - 0x0000: L2CAP_CONN_SUCCESSFUL
  *        - 0x0002: L2CAP_SPSM_NOT_SUPPORTED
  *        - 0x0004: L2CAP_NO_RESOURCE_AVAILABLE
  *        - 0x0005: L2CAP_INSUFFICIENT_AUTHENTICATION
  *        - 0x0006: L2CAP_INSUFFICIENT_AUTHORIZATION
  *        - 0x0007: L2CAP_INSUFFICIENT_ENCRYPTION_KEY_SIZE
  *        - 0x0008: L2CAP_INSUFFICIENT_ENCRYPTION
  *        - 0x0009: L2CAP_INVALID_SOURCE_CID
  *        - 0x000A: L2CAP_SOURCE_CID_ALREADY_ALLOCATED
  *        - 0x000B: L2CAP_UNACCEPTABLE_PARAMETERS
  * @param identifier This is the identifier which associates the request to the
  *        response.
  * @param spsm Simplified Protocol/Service Multiplexer
  *        Values:
  *        - 0x0001 ... 0x00FF
  * @param cid The local channel endpoint that identifies the L2CAP channel. To
  *        be ignored if event_type is L2CAP_CONN_REQ.
  * @param remote_cid The channel endpoint on the remote device.
  * @param peer_mtu The maximum SDU size (in octets) that the L2CAP layer entity
  *        sending the L2CAP_LE_CREDIT_BASED_CONNECTION_REQ can receive on this
  *        channel.
  *        Values:
  *        - 23 ... 65535
  * @param peer_mps The maximum PDU payload size (in octets) that the L2CAP layer
  *        entity sending the L2CAP_LE_CREDIT_BASED_CONNECTION_REQ is capable of
  *        receiving on this channel.
  *        Values:
  *        - 23 ... 65535
  * @param initial_credits The number of K-frames that the peer device can send
  *        to the L2CAP layer entity as soon as the L2CAP channel is established.
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_cfc_connection_event(uint16_t connection_handle,
                                                  uint8_t event_type,
                                                  uint16_t result,
                                                  uint8_t identifier,
                                                  uint16_t spsm,
                                                  uint16_t cid,
                                                  uint16_t remote_cid,
                                                  uint16_t peer_mtu,
                                                  uint16_t peer_mps,
                                                  uint16_t initial_credits))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_cfc_connection_event\r\n");
}
/**
  * @brief Event raised when an L2CAP channel using LE Credit Based Flow Control
  *        mode is terminated.
  * @param connection_handle Handle identifying the connection.
  * @param cid The local channel endpoint that identifies the L2CAP channel.
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_disconnection_complete_event(uint16_t connection_handle,
                                                          uint16_t cid))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_disconnection_complete_event\r\n");
}
/**
  * @brief Events raised when an L2CAP_FLOW_CONTROL_CREDIT_IND is received from
  *        the peer, which means that it is capable of receiving additional
  *        K-frames (for example after it has processed one or more K-frames) in
  *        LE Credit Based Flow Control.
  * @param connection_handle Handle identifying the connection.
  * @param cid The local channel endpoint that identifies the L2CAP channel.
  * @param tx_credits The number of additional k-frames that the peer's L2CAP
  *        layer entity can receive.
  * @param tx_credit_balance Remaining number of K-frames that local L2CAP layer
  *        entity can currently send to the peer.
  *        Values:
  *        - 1 ... 65535
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_flow_control_credit_event(uint16_t connection_handle,
                                                       uint16_t cid,
                                                       uint16_t tx_credits,
                                                       uint16_t tx_credit_balance))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_flow_control_credit_event\r\n");
}
/**
  * @brief Event raised when an SDU to be transmitted has been processed by the
  *        local L2CAP layer entity.
  * @param connection_handle Handle identifying the connection.
  * @param cid The local channel endpoint that identifies the L2CAP channel.
  * @param sdu_length
  * @param tx_credit_balance Remaining number of K-frames that local L2CAP layer
  *        entity can currently send to the peer.
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_sdu_data_tx_nwk_event(uint16_t connection_handle,
                                                   uint16_t cid,
                                                   uint16_t sdu_length,
                                                   uint16_t tx_credit_balance))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_sdu_data_tx_nwk_event\r\n");
}
/**
  * @brief Event raised when an SDU has been received.
  * @param connection_handle Handle identifying the connection.
  * @param cid The local channel endpoint that identifies the L2CAP channel.
  * @param rx_credit_balance Remaining number of K-frames that local L2CAP layer
  *        entity can currently receive from the peer. If automatic management of
  *        credits is enabled, this number is automatically changed after the SDU
  *        is extracted.
  * @param sdu_length
  * @param sdu_data
  * @retval None
  */
WEAK_FUNCTION(void aci_l2cap_sdu_data_rx_nwk_event(uint16_t connection_handle,
                                                   uint16_t cid,
                                                   uint16_t rx_credit_balance,
                                                   uint16_t sdu_length,
                                                   uint8_t sdu_data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_l2cap_sdu_data_rx_nwk_event\r\n");
}
/**
  * @brief This event is generated to the application by the GATT server when a
  *        client modifies any attribute on the server, as consequence of one of
  *        the following GATT procedures: - write without response - signed write
  *        without response - write characteristic value - write long
  *        characteristic value - reliable write.
  * @param connection_handle The connection handle which modified the attribute.
  * @param attr_handle Handle of the attribute that was modified.
  * @param attr_data_length Length of attr_data in octets
  * @param attr_data The modified value
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_srv_attribute_modified_event(uint16_t connection_handle,
                                                         uint16_t attr_handle,
                                                         uint16_t attr_data_length,
                                                         uint8_t attr_data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_srv_attribute_modified_event\r\n");
}
/**
  * @brief This event is generated in response to a Find Information Request
  *        during a discovery procedure for all the characteristic descriptors.
  *        See aci_gatt_clt_disc_all_char_desc() and Find Information Response in
  *        Bluetooth Core spec.
  * @param connection_handle Connection handle related to the response
  * @param format Format of the hanndle-uuid pairs
  * @param event_data_length Length of handle_uuid_pair in octets
  * @param handle_uuid_pair A sequence of handle-uuid pairs. if format=1, each
  *        pair is:[2 octets for handle, 2 octets for UUIDs], if format=2, each
  *        pair is:[2 octets for handle, 16 octets for UUIDs]
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_find_info_resp_event(uint16_t connection_handle,
                                                    uint8_t format,
                                                    uint16_t event_data_length,
                                                    uint8_t handle_uuid_pair[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_find_info_resp_event\r\n");
}
/**
  * @brief This event is generated during a "discover service by UUID" procedure.
  *        See aci_gatt_clt_disc_primary_service_by_uuid().
  * @param connection_handle Connection handle related to the response
  * @param num_of_handle_pair Number of attribute, group handle pairs
  * @param attr_group_handle_pair See @ref attribute_group_handle_pair_t
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_find_by_type_value_resp_event(uint16_t connection_handle,
                                                             uint8_t num_of_handle_pair,
                                                             attribute_group_handle_pair_t *attr_group_handle_pair))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_find_by_type_value_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a ATT_READ_BY_TYPE_REQ, during
  *        a "find included service" procedure or a "discover all
  *        characteristics" procedure. See aci_gatt_clt_find_included_services()
  *        and aci_gatt_clt_disc_all_char_of_service().
  * @param connection_handle Connection handle related to the response
  * @param handle_value_pair_length The size of each attribute handle-value pair
  * @param data_length Length of handle_value_pair_data in octets
  * @param handle_value_pair_data Attribute Data List as defined in Bluetooth
  *        Core v4.1 spec. A sequence of handle-value pairs: [2 octets for
  *        Attribute Handle, (handle_value_pair_length - 2 octets) for Attribute
  *        Value]
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_read_by_type_resp_event(uint16_t connection_handle,
                                                       uint8_t handle_value_pair_length,
                                                       uint16_t data_length,
                                                       uint8_t handle_value_pair_data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_read_by_type_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Read Request. See
  *        aci_gatt_clt_read().
  * @param connection_handle Connection handle related to the response
  * @param event_data_length Length of following data
  * @param attribute_value The value of the attribute.
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_read_resp_event(uint16_t connection_handle,
                                               uint16_t event_data_length,
                                               uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_read_resp_event\r\n");
}
/**
  * @brief This event can be generated during a read long characteristic value
  *        procedure. See aci_gatt_clt_read_long().
  * @param connection_handle Connection handle related to the response
  * @param event_data_length Length of following data
  * @param attribute_value Part of the attribute value.
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_read_blob_resp_event(uint16_t connection_handle,
                                                    uint16_t event_data_length,
                                                    uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_read_blob_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Read Multiple Request. See
  *        aci_gatt_clt_read_multiple_char_value().
  * @param connection_handle Connection handle related to the response
  * @param event_data_length Length of following data
  * @param set_of_values A set of two or more values. A concatenation of
  *        attribute values for each of the attribute handles in the request in
  *        the order that they were requested.
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_read_multiple_resp_event(uint16_t connection_handle,
                                                        uint16_t event_data_length,
                                                        uint8_t set_of_values[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_read_multiple_resp_event\r\n");
}
/**
  * @brief This event is generated in response to a Read By Group Type Request,
  *        during a "discover all primary services" procedure. See
  *        aci_gatt_clt_disc_all_primary_services().
  * @param connection_handle Connection handle related to the response
  * @param Attribute_Data_Length The size of each attribute data
  * @param data_length Length of attribute_data_list in octets
  * @param attribute_data_list Attribute Data List as defined in Bluetooth Core
  *        v4.1 spec. A sequence of attribute handle, end group handle, attribute
  *        value tuples: [2 octets for Attribute Handle, 2 octets End Group
  *        Handle, (Attribute_Data_Length - 4 octets) for Attribute Value]
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_read_by_group_type_resp_event(uint16_t connection_handle,
                                                             uint8_t Attribute_Data_Length,
                                                             uint16_t data_length,
                                                             uint8_t attribute_data_list[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_read_by_group_type_resp_event\r\n");
}
/**
  * @brief This event is generated in response to an ATT_PREPARE_WRITE_REQ during
  *        a write long characteristic value procedure. See
  *        aci_gatt_clt_write_long().
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute to be written
  * @param offset The offset of the first octet to be written.
  * @param part_attribute_value_length Length of part_attribute_value in octets
  * @param part_attribute_value The value of the attribute to be written
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_prepare_write_resp_event(uint16_t connection_handle,
                                                        uint16_t attribute_handle,
                                                        uint16_t offset,
                                                        uint16_t part_attribute_value_length,
                                                        uint8_t part_attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_prepare_write_resp_event\r\n");
}
/**
  * @brief This event is generated in response to an ATT Execute Write Request,
  *        during a write long characteristic value procedure. See
  *        aci_gatt_clt_write_long().
  * @param connection_handle Connection handle related to the response
  * @retval None
  */
WEAK_FUNCTION(void aci_att_clt_exec_write_resp_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_clt_exec_write_resp_event\r\n");
}
/**
  * @brief This event is generated when an indication is received from the
  *        server.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The current value of the attribute
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_clt_indication_event(uint16_t connection_handle,
                                                 uint16_t attribute_handle,
                                                 uint16_t attribute_value_length,
                                                 uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_clt_indication_event\r\n");
}
/**
  * @brief This event is generated when a notification is received from the
  *        server.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The current value of the attribute
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_clt_notification_event(uint16_t connection_handle,
                                                   uint16_t attribute_handle,
                                                   uint16_t attribute_value_length,
                                                   uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_clt_notification_event\r\n");
}
/**
  * @brief This event is generated when a GATT client procedure completes either
  *        with error or successfully.
  * @param connection_handle Connection handle related to the response
  * @param error_code Indicates whether the procedure completed with an error or
  *        was successful.
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_clt_proc_complete_event(uint16_t connection_handle,
                                                    uint8_t error_code))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_clt_proc_complete_event\r\n");
}
/**
  * @brief This event is generated when an Error Response is received from the
  *        server. The error response can be given by the server at the end of
  *        one of the GATT discovery procedures. This does not mean that the
  *        procedure ended with an error, but this error event is part of the
  *        procedure itself.
  * @param connection_handle Connection handle related to the response
  * @param req_op_code The request that generated this error response
  * @param attribute_handle The attribute handle that generated this error
  *        response
  * @param error_code The reason why the request has generated an error response
  *        (ATT error codes)
  *        Values:
  *        - 0x01: Invalid handle
  *        - 0x02: Read not permitted
  *        - 0x03: Write not permitted
  *        - 0x04: Invalid PDU
  *        - 0x05: Insufficient authentication
  *        - 0x06: Request not supported
  *        - 0x07: Invalid offset
  *        - 0x08: Insufficient authorization
  *        - 0x09: Prepare queue full
  *        - 0x0A: Attribute not found
  *        - 0x0B: Attribute not long
  *        - 0x0C: Insufficient encryption key size
  *        - 0x0D: Invalid attribute value length
  *        - 0x0E: Unlikely error
  *        - 0x0F: Insufficient encryption
  *        - 0x10: Unsupported group type
  *        - 0x11: Insufficient resources
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_clt_error_resp_event(uint16_t connection_handle,
                                                 uint8_t req_op_code,
                                                 uint16_t attribute_handle,
                                                 uint8_t error_code))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_clt_error_resp_event\r\n");
}
/**
  * @brief This event can be generated during a "Discover Characteristics By
  *        UUID" procedure or a "Read using Characteristic UUID" procedure.
  *        During a "Discover Characteristics By UUID" procedure, attribute_value
  *        is a service declaration as defined in Bluetooth Core v5.2 spec
  *        (vol.3, Part G, ch. 3.3.1), i.e. it is composed by: Characteristic
  *        Properties (1 octet), Characteristic Value Handle (2 octets) and
  *        Characteristic UUID (2 or 16 octets). During a "Read using
  *        Characteristic UUID" procedure, attribute_value is the value of the
  *        characteristic.
  * @param connection_handle Connection handle related to the response
  * @param attribute_handle The handle of the attribute
  * @param attribute_value_length Length of attribute_value in octets
  * @param attribute_value The attribute value will be a service declaration as
  *        defined in Bluetooth Core v4.0 spec  (vol.3, Part G, ch. 3.3.1), when
  *        a "Discover Characteristics By UUID" has been started.  It will be the
  *        value of the Characteristic if a "Read using Characteristic UUID" has
  *        been performed.
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_clt_disc_read_char_by_uuid_resp_event(uint16_t connection_handle,
                                                                  uint16_t attribute_handle,
                                                                  uint8_t attribute_value_length,
                                                                  uint8_t attribute_value[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_clt_disc_read_char_by_uuid_resp_event\r\n");
}
/**
  * @brief This event is generated when the client has sent the confirmation to a
  *        previously sent indication.
  * @param connection_handle Connection handle related to the event.
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_srv_confirmation_event(uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_srv_confirmation_event\r\n");
}
/**
  * @brief This event is generated when an execute write request is received from
  *        the peer. This happens when the client wants to write a long attribute
  *        (i.e. an attribute with a size greater than ATT_MTU -3) or more than
  *        one attribute in a single operation. The aci_gatt_srv_exec_write_resp
  *        command must be sent to give a response to the peer.
  * @param connection_handle Handle identifying the connection where the execute
  *        write operation has been received.
  * @param flags If 1, peer wants to execute all the queued writes. If 0, all
  *        queued writes must be discarded.
  *        Values:
  *        - 0x00: FLUSH
  *        - 0x01: EXECUTE
  * @retval None
  */
WEAK_FUNCTION(void aci_att_srv_exec_write_req_event(uint16_t connection_handle,
                                                    uint8_t flags))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_att_srv_exec_write_req_event\r\n");
}
/**
  * @brief This event is generated if authorization is needed to access the
  *        attribute value. aci_gatt_srv_authorize_resp_nwk command must be sent
  *        in response to this event.
  * @param conn_handle Connection handle to be used to identify the connection
  *        with the peer device.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param attr_handle
  * @param operation_type
  *        Values:
  *        - 0x00: Read
  *        - 0x10: Write Request
  *        - 0x11: Write Command or Signed Write Command
  *        - 0x12: Prepare Write Request
  * @param attr_val_offset Offset from which the value needs to be read or write
  *        Values:
  *        - 0 ... 511
  * @param data_length Length of Data field
  * @param data The data that the client has requested to write
  * @retval None
  */
WEAK_FUNCTION(void aci_gatt_srv_authorize_nwk_event(uint16_t connection_handle,
                                                    uint16_t attr_handle,
                                                    uint8_t operation_type,
                                                    uint16_t attr_val_offset,
                                                    uint8_t data_length,
                                                    uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("aci_gatt_srv_authorize_nwk_event\r\n");
}
/**
  * @brief The LE PHY Update Complete Event is used to indicate that the
  *        Controller has changed the transmitter PHY or receiver PHY in use. If
  *        the Controller changes the transmitter PHY, the receiver PHY, or both
  *        PHYs, this event shall be issued. If an LE_Set_PHY command was sent
  *        and the Controller determines that neither PHY will change as a
  *        result, it issues this event immediately.
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param connection_handle connection_handle to be used to identify a
  *        connection.
  * @param tx_phy The transmitter PHY for the connection
  *        Values:
  *        - 0x01: The transmitter PHY for the connection is LE 1M
  *        - 0x02: The transmitter PHY for the connection is LE 2M
  *        - 0x03: The transmitter PHY for the connection is LE Coded
  * @param rx_phy The receiver PHY for the connection
  *        Values:
  *        - 0x01: The receiver PHY for the connection is LE 1M
  *        - 0x02: The receiver PHY for the connection is LE 2M
  *        - 0x03: The receiver PHY for the connection is LE Coded
  * @retval None
  */
WEAK_FUNCTION(void hci_le_phy_update_complete_event(uint8_t status,
                                                    uint16_t connection_handle,
                                                    uint8_t tx_phy,
                                                    uint8_t rx_phy))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_phy_update_complete_event\r\n");
}
/**
  * @brief The LE Extended Advertising Report event indicates that one or more
  *        Bluetooth devices have responded to an active scan or have broadcast
  *        advertisements that were received during a passive scan. The
  *        Controller may coalesce multiple advertising reports from the same or
  *        different advertisers into a single LE Extended Advertising Report
  *        event, provided all the parameters from all the advertising reports
  *        fit in a single HCI event. This event shall only be generated if
  *        scanning was enabled using the LE Set Extended Scan Enable command. It
  *        reports advertising events using either legacy or extended advertising
  *        PDUs. The Controller may split the data from a single advertisement
  *        (whether one PDU or several) into several reports. If so, each report
  *        except the last shall have an event_type with a data status field of
  *        "incomplete, more data to come", while the last shall have the value
  *        "complete"; the address_type, Address, advertising_sid, primary_phy,
  *        and secondary_phy fields shall be the same in all the reports. When a
  *        scan response is received, bits 0-2 and 4 of the event type shall
  *        indicate the properties of the original advertising event. An
  *        event_type with a data status field of "incomplete, data truncated"
  *        indicates that the Controller attempted to receive an AUX_CHAIN_IND
  *        PDU but was not successful.
  * @param num_reports Number of separate reports in the event
  *        Values:
  *        - 0x01 ... 0x0A
  * @param extended_adv_report See @ref extended_advertising_report_t
  * @retval None
  */
WEAK_FUNCTION(void hci_le_extended_advertising_report_event(uint8_t num_reports,
                                                            extended_advertising_report_t extended_adv_report[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_extended_advertising_report_event\r\n");
}
/**
  * @brief The LE Periodic Advertising Report event indicates that the Controller
  *        has received a Periodic Advertising packet. The sync_handle parameter
  *        indicates the identifier for the periodic advertisements specified by
  *        the Advertising SID subfield of the ADI field in the ADV_EXT_IND PDU.
  *        The Controller may split the data from a single periodic advertisement
  *        (whether one PDU or several) into several reports. If so, each report
  *        except the last shall have a data_status of "incomplete, more data to
  *        come", while the last shall have the value "complete". A data_status
  *        of "incomplete, data truncated" indicates that the Controller
  *        attempted to receive an AUX_CHAIN_IND PDU but was not successful. The
  *        Unused parameter shall be set to 0xFF by the Controller and ignored by
  *        the Host.
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param sync_handle Sync handle that identifies the synchronization
  *        information about the periodic advertising train.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param advertising_sid Advertising SID subfield in the ADI field of the PDU
  *        Values:
  *        - 0x00 ... 0x0F: Value of the Advertising SID subfield in the ADI field of the PDU
  *        - 0x10 ... 0xFF: Reserved for future use
  * @param advertiser_address_type Advertising Address Type
  *        Values:
  *        - 0x00: Public Device Address or Public Identity Address
  *        - 0x01: Random Device Address or Random (static) Identity Address
  *        - 0x02: Public Identity Address (corresponds to Resolved Private Address)
  *        - 0x03: Random (static) Identity Address (corresponds to Resolved Private Address)
  *        - 0x04 ... 0xFF: Reserved for future use
  * @param advertising_address Public Device Address, Random Device Address,
  *        Public Identity Address, or Random (static) Identity Address of the
  *        advertiser
  * @param advertiser_phy Advertiser PHY
  *        Values:
  *        - 0x01: Advertiser PHY is LE 1M
  *        - 0x02: Advertiser PHY is LE 2M
  *        - 0x03: Advertiser PHY is LE Coded
  *        - 0x04 ... 0xFF: Reserved for future use
  * @param periodic_advertising_interval Periodic Advertising Interval Time = N *
  *        1.25 ms Time Range: 7.5 ms to 81.91875 s
  *        Values:
  *        - 0x0006 ... 0xFFFF: Periodic Advertising Interval
  * @param advertiser_clock_accuracy Advertiser Clock Accuracy
  *        Values:
  *        - 0x00: 500 ppm
  *        - 0x01: 250 ppm
  *        - 0x02: 150 ppm
  *        - 0x03: 100 ppm
  *        - 0x04: 75 ppm
  *        - 0x05: 50 ppm
  *        - 0x06: 30 ppm
  *        - 0x07: 20 ppm
  *        - 0x08 ... 0xFF: Reserved for future use
  * @retval None
  */
WEAK_FUNCTION(void hci_le_periodic_advertising_sync_established_event(uint8_t status,
                                                                      uint16_t sync_handle,
                                                                      uint8_t advertising_sid,
                                                                      uint8_t advertiser_address_type,
                                                                      uint8_t advertising_address[6],
                                                                      uint8_t advertiser_phy,
                                                                      uint16_t periodic_advertising_interval,
                                                                      uint8_t advertiser_clock_accuracy))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_periodic_advertising_sync_established_event\r\n");
}
/**
  * @brief The LE Periodic Advertising Report event indicates that the Controller
  *        has received a Periodic Advertising packet. The sync_handle parameter
  *        indicates the identifier for the periodic advertisements specified by
  *        the Advertising SID subfield of the ADI field in the ADV_EXT_IND PDU.
  *        The Controller may split the data from a single periodic advertisement
  *        (whether one PDU or several) into several reports. If so, each report
  *        except the last shall have a data_status of "incomplete, more data to
  *        come", while the last shall have the value "complete". A data_status
  *        of "incomplete, data truncated" indicates that the Controller
  *        attempted to receive an AUX_CHAIN_IND PDU but was not successful. The
  *        Unused parameter shall be set to 0xFF by the Controller and ignored by
  *        the Host.
  * @param sync_handle Sync handle that identifies the synchronization
  *        information about the periodic advertising train.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param tx_power TX Power. Units: dBm
  *        Values:
  *        - 127: NA
  *        - -127 ... 126
  * @param rssi RSSI value Units: dBm
  *        Values:
  *        - 127: RSSI is not available
  *        - -127 ... 20: RSSI value
  * @param cte_type
  *        Values:
  *        - 0x00: AoA Constant Tone Extension
  *        - 0x01: AoD Constant Tone Extension with 1 microsecond slots
  *        - 0x02: AoD Constant Tone Extension with 2 microseconds slots
  *        - 0xFF: No Constant Tone Extension
  * @param data_status Data status
  *        Values:
  *        - 0x00: Data complete
  *        - 0x01: Data incomplete, more data to come
  *        - 0x02: Data incomplete, data truncated, no more to come
  *        - 0x03 ... 0xFF: Reserved for future use
  * @param data_length Length of the Data field
  *        Values:
  *        - 0 ... 247: Length of the Data field
  *        - 248 ... 255: Reserved for future use
  * @param data Data received from a Periodic Advertising packet
  * @retval None
  */
WEAK_FUNCTION(void hci_le_periodic_advertising_report_event(uint16_t sync_handle,
                                                            int8_t tx_power,
                                                            int8_t rssi,
                                                            uint8_t cte_type,
                                                            uint8_t data_status,
                                                            uint8_t data_length,
                                                            uint8_t data[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_periodic_advertising_report_event\r\n");
}
/**
  * @brief The LE Periodic Advertising Sync Lost event indicates that the
  *        Controller has not received a Periodic Advertising packet identified
  *        by sync_handle within the timeout period.
  * @param sync_handle Sync handle that identifies the synchronization
  *        information about the periodic advertising train.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @retval None
  */
WEAK_FUNCTION(void hci_le_periodic_advertising_sync_lost_event(uint16_t sync_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_periodic_advertising_sync_lost_event\r\n");
}
/**
  * @brief The LE Scan Timeout event indicates that scanning has ended because
  *        the duration has expired. This event shall only be generated if
  *        scanning was enabled using the LE Set Extended Scan Enable command.
  * @retval None
  */
WEAK_FUNCTION(void hci_le_scan_timeout_event(void))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_scan_timeout_event\r\n");
}
/**
  * @brief The LE Advertising Set Terminated event indicates that the Controller
  *        has terminated advertising in the advertising sets specified by the
  *        advertising_handle parameter. This event shall be generated every time
  *        connectable advertising in an advertising set results in a connection
  *        being created. This event shall only be generated if advertising was
  *        enabled using the LE Set Extended Advertising Enable command. The
  *        connection_handle parameter is only valid when advertising ends
  *        because a connection was created. If the
  *        max_extended_advertising_events parameter in the
  *        LE_Set_Extended_Advertising_Enable command was non-zero, the
  *        num_completed_extended_advertising_events parameter shall be set to
  *        the number of completed extended advertising events the Controller had
  *        transmitted when either the duration elapsed or the maximum number of
  *        extended advertising events was reached; otherwise it shall be set to
  *        zero. If advertising has terminated as a result of the advertising
  *        duration elapsing, the status parameter shall be set to the error code
  *        Advertising Timeout (0x3C). If advertising has terminated because the
  *        max_extended_advertising_events was reached, the status parameter
  *        shall be set to the error code Limit Reached (0x43).
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param advertising_handle Advertising handle in which advertising has ended
  *        Values:
  *        - 0x00 ... 0xEF: advertising_handle in which advertising has ended
  *        - 0xF0 ... 0xFF: Reserved for future use
  * @param connection_handle connection_handle to be used to identify a
  *        connection.
  * @param num_completed_extended_advertising_events Number of completed extended
  *        advertising events transmitted by the Controller
  * @retval None
  */
WEAK_FUNCTION(void hci_le_advertising_set_terminated_event(uint8_t status,
                                                           uint8_t advertising_handle,
                                                           uint16_t connection_handle,
                                                           uint8_t num_completed_extended_advertising_events))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_advertising_set_terminated_event\r\n");
}
/**
  * @brief The LE Scan Request Received event indicates that a SCAN_REQ PDU or an
  *        AUX_SCAN_REQ PDU has been received by the advertiser. The request
  *        contains a device address from a scanner that is allowed by the
  *        advertising filter policy. The advertising set is identified by
  *        advertising_handle. This event shall only be generated if advertising
  *        was enabled using the LE Set Extended Advertising Enable command. The
  *        scanner_address_type and scanner_address indicates the type of the
  *        address and the address of the scanner device.
  * @param advertising_handle It is used to identify an advertising set
  *        Values:
  *        - 0x00 ... 0xEF: Used to identify an advertising set
  *        - 0xF0 ... 0xFF: Reserved for future use
  * @param scanner_address_type Scanner address  type
  *        Values:
  *        - 0x00: Public Device Address
  *        - 0x01: Random Device Address
  *        - 0x02: Public Identity Address (corresponds to Resolved Private Address)
  *        - 0x03: Random (static) Identity Address (corresponds to Resolved Private Address)
  *        - 0x04 ... 0xFF: Reserved for future use
  * @param scanner_address Public Device Address, Random Device Address, Public
  *        Identity Address or Random (static) Identity Address of the
  *        advertising device
  * @retval None
  */
WEAK_FUNCTION(void hci_le_scan_request_received_event(uint8_t advertising_handle,
                                                      uint8_t scanner_address_type,
                                                      uint8_t scanner_address[6]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_scan_request_received_event\r\n");
}
/**
  * @brief The LE Channel Selection Algorithm Event indicates which channel
  *        selection algorithm is used on a data channel connection (see [Vol 6]
  *        Part B, Section 4.5.8).
  * @param connection_handle connection_handle to be used to identify a
  *        connection.
  * @param channel_selection_algorithm Channel selection algorithm to be  used on
  *        a data channel connection
  *        Values:
  *        - 0x00: LE Channel Selection Algorithm #1 is used
  *        - 0x01: LE Channel Selection Algorithm #2 is used
  *        - 0x02 ... 0xFF: Reserved for future use
  * @retval None
  */
WEAK_FUNCTION(void hci_le_channel_selection_algorithm_event(uint16_t connection_handle,
                                                            uint8_t channel_selection_algorithm))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_channel_selection_algorithm_event\r\n");
}
/**
  * @brief The HCI_LE_Connectionless_IQ_Report event is used by the Controller to
  *        report IQ information from the Constant Tone Extension of a received
  *        advertising packet forming part of the periodic advertising train
  *        identified by sync_handle and to report IQ information from the
  *        Constant Tone Extension of a received Test Mode packet (see Section
  *        7.8.28). The index of the channel on which the packet was received,
  *        the RSSI of the packet (excluding the Constant Tone Extension), the ID
  *        of the antenna on which this was measured, the type of Constant Tone
  *        Extension, the value of paEventCounter, and the IQ samples of the
  *        Constant Tone Extension of the advertisement are reported in the
  *        corresponding parameters. For any given sample, either both or neither
  *        of i_sample[i] and q_sample[i] shall equal 0x80. The slot_durations
  *        parameter specifies the sampling rate used by the Controller. The
  *        packet_status parameter indicates whether the received packet had a
  *        valid CRC and, if not, whether the Controller has determined the
  *        position and size of the Constant Tone Extension using the Length and
  *        CTETime fields. Note: A Controller is not required to generate this
  *        event for packets that have a bad CRC. The Constant Tone Extension
  *        format is defined in [Vol 6] Part B, Section 2.5.1. If the PDU
  *        contains AdvData, then the HCI_LE_Periodic_Advertising_Report event
  *        shall be generated before this event. The Controller is not required
  *        to generate this event for a Constant Tone Extension with a type that
  *        it does not support. This event is also used by the Controller to
  *        report that it has insufficient resources to report IQ samples for all
  *        received Constant Tone Extensions and has failed to sample at least
  *        once. In this case packet_status shall be set to 0xFF and sample_count
  *        to 0x00.
  * @param sync_handle Sync handle that identifies the synchronization
  *        information about the periodic advertising train.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param channel_index 0x25-0x27 can be used only for packets generated during
  *        test modes.
  *        Values:
  *        - 0x00 ... 0x27: The index of the channel on which the packet was received.
  * @param rssi RSSI of the packet. Units: 0.1 dBm
  *        Values:
  *        - -1270 ... 200
  * @param rssi_antenna_id Antenna ID
  * @param cte_type Type of Constant Tone Extension.
  *        Values:
  *        - 0x00: AoA_CTE
  *        - 0x01: AoD_CTE_1us
  *        - 0x02: AoD_CTE_2us
  * @param slot_durations Sampling rate used by the Controller.
  *        Values:
  *        - 0x01: SLOT_1us
  *        - 0x02: SLOT_2us
  * @param packet_status It indicates whether the received packet had a valid CRC
  *        and, if not, whether the Controller has determined the position and
  *        size of the Constant Tone Extension using the Length and CTETime
  *        fields. 0x00 - CRC was correct;  0x01 - CRC was incorrect and the
  *        Length and CTETime fields of the packet were used to determine
  *        sampling points;  0x02 - CRC was incorrect but the Controller has
  *        determined the position and length of the Constant Tone Extension in
  *        some other way;  0xFF - Insufficient resources to sample
  *        (channel_index, cte_type, and slot_durations invalid).
  *        Values:
  *        - 0x00: CRC_OK
  *        - 0x01: CRC_ERROR_1
  *        - 0x02: CRC_ERROR_2
  *        - 0xFF: NO_SAMPLES
  * @param periodic_event_counter The value of paEventCounter (see [Vol 6] Part
  *        B, Section 4.4.2.1) for the reported AUX_SYNC_IND PDU
  * @param sample_count Total number of sample pairs (there shall be the same
  *        number of I samples and Q samples). Note: This number is dependent on
  *        the switch and sample slot durations used.
  *        Values:
  *        - 0x00
  *        - 0x09 ... 0x52
  * @param Samples See @ref samples_t
  * @retval None
  */
WEAK_FUNCTION(void hci_le_connectionless_iq_report_event(uint16_t sync_handle,
                                                         uint8_t channel_index,
                                                         int16_t rssi,
                                                         uint8_t rssi_antenna_id,
                                                         uint8_t cte_type,
                                                         uint8_t slot_durations,
                                                         uint8_t packet_status,
                                                         uint16_t periodic_event_counter,
                                                         uint8_t sample_count,
                                                         samples_t Samples[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_connectionless_iq_report_event\r\n");
}
/**
  * @brief The HCI_LE_Connection_IQ_Report event is used by the Controller to
  *        report the IQ samples from the Constant Tone Extension of a received
  *        packet (see [Vol 6] Part B, Section 2.4.2.26). The connection_handle
  *        parameter identifies the connection that corresponds to the reported
  *        information. The receiver PHY, the index of the data channel, the RSSI
  *        value of the packet (excluding the Constant Tone Extension), the ID of
  *        the antenna on which this was measured, the type of Constant Tone
  *        Extension, the value of connEventCounter, and the IQ samples of the
  *        Constant Tone Extension of the received packet are reported in the
  *        corresponding parameters. For any given sample, either both or neither
  *        of i_sample[i] and q_sample[i] shall equal 0x80. The slot_durations
  *        parameter specifies the sampling rate used by the Controller. The
  *        packet_status parameter indicates whether the received packet had a
  *        valid CRC and, if not, whether the Controller has determined the
  *        position and size of the Constant Tone Extension using the Length and
  *        CTETime fields. Note: A Controller is not required to generate this
  *        event for packets that have a bad CRC. This event is also used by the
  *        Controller to report that it has insufficient resources to report IQ
  *        samples for all received Constant Tone Extensions and has failed to
  *        sample at least once. In this case packet_status shall be set to 0xFF
  *        and sample_count to 0x00. The Constant Tone Extension format is
  *        defined in [Vol 6] Part B, Section 2.1.5.
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param rx_phy
  *        Values:
  *        - 0x01: The receiver PHY for the connection is LE 1M
  *        - 0x02: The receiver PHY for the connection is LE 2M
  * @param data_channel_index
  *        Values:
  *        - 0x00 ... 0x24: The index of the data channel on which the Data Physical Channel PDU was received.
  * @param rssi
  *        Values:
  *        - -1270 ... 200: RSSI of the packet. Units: 0.1 dBm
  * @param rssi_antenna_id ID of the antenna on which the RSSI is measured
  * @param cte_type
  *        Values:
  *        - 0x00: AoA Constant Tone Extension
  *        - 0x01: AoD Constant Tone Extension with 1 microsecond slots
  *        - 0x02: AoD Constant Tone Extension with 2 microseconds slots
  * @param slot_durations Sampling rate used by the Controller.
  *        Values:
  *        - 0x01: SLOT_1us
  *        - 0x02: SLOT_2us
  * @param packet_status
  *        Values:
  *        - 0x00: CRC was correct
  *        - 0x01: CRC was incorrect and the Length and CTETime fields of the packet were used to determine
  *                sampling points
  *        - 0x02: CRC was incorrect but the Controller has determined the position and length of the Constant
  *                Tone Extension in some other way
  *        - 0xFF: Insufficient resources to sample (channel_index, cte_type, and slot_durations invalid).
  * @param connection_event_counter The value of connEventCounter (see [Vol 6]
  *        Part B, Section 4.5.1) for the reported PDU
  * @param sample_count
  *        Values:
  *        - 0x00: No samples provided (only permitted if packet_status is 0xFF).
  *        - 0x09 ... 0x52: Total number of sample pairs (there shall be the same number of I samples and Q samples).
  *                         Note: This number is dependent on the switch and sample slot durations used.
  * @param Samples See @ref samples_t
  * @retval None
  */
WEAK_FUNCTION(void hci_le_connection_iq_report_event(uint16_t connection_handle,
                                                     uint8_t rx_phy,
                                                     uint8_t data_channel_index,
                                                     int16_t rssi,
                                                     uint8_t rssi_antenna_id,
                                                     uint8_t cte_type,
                                                     uint8_t slot_durations,
                                                     uint8_t packet_status,
                                                     uint16_t connection_event_counter,
                                                     uint8_t sample_count,
                                                     samples_t Samples[]))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_connection_iq_report_event\r\n");
}
/**
  * @brief This event is used by the Controller to report an issue following a
  *        request to a peer device to reply with a packet containing an
  *        LL_CTE_RSP PDU and a Constant Tone Extension. It shall be generated if
  *        the packet containing the LL_CTE_RSP PDU sent in response did not
  *        contain a Constant Tone Extension or if the peer rejected the request.
  *        It shall not be generated if the packet containing the LL_CTE_RSP PDU
  *        had a CRC error or if the procedure response timeout timer (see [Vol
  *        6] Part B, Section 5.2) expired.
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @retval None
  */
WEAK_FUNCTION(void hci_le_cte_request_failed_event(uint8_t status,
                                                   uint16_t connection_handle))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_cte_request_failed_event\r\n");
}
/**
  * @brief The HCI_LE_Periodic_Advertising_Sync_Transfer_Received event is used
  *        by the Controller to report that it has received periodic advertising
  *        synchronization information from the device referred to by the
  *        connection_handle parameter and either successfully synchronized to
  *        the periodic advertising train or timed out while attempting to
  *        synchronize. The status will be zero if it successfully synchronized
  *        and non-zero otherwise. The service_data value is provided by the Host
  *        of the device sending the information. The sync_handle identifies the
  *        periodic advertising in subsequent commands and events and shall be
  *        assigned by the Controller. The remaining parameters provide
  *        information about the periodic advertising (see Section 7.7.65.14). If
  *        status is non-zero, all parameter values are valid except sync_handle,
  *        which the Host shall ignore. Note: If the Controller is already
  *        synchronized to the periodic advertising train described in the
  *        received information, no event will be generated.
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param service_data A value provided by the peer device
  * @param sync_handle Sync handle that identifies the synchronization
  *        information about the periodic advertising train.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param advertising_sid
  *        Values:
  *        - 0x00 ... 0x0F: Value of the Advertising SID used to advertise the periodic advertising
  * @param advertiser_address_type
  *        Values:
  *        - 0x00: Public Device Address or Public Identity Address
  *        - 0x01: Random Device Address or Random (static) Identity Address
  *        - 0x02: Public Identity Address (corresponds to Resolved Private Address)
  *        - 0x03: Random (static) Identity Address (corresponds to Resolved Private Address)
  * @param advertising_address Public Device Address, Random Device Address,
  *        Public Identity Address, or Random (static) Identity Address of the
  *        advertiser
  * @param advertiser_phy
  *        Values:
  *        - 0x01: Advertiser PHY is LE 1M
  *        - 0x02: Advertiser PHY is LE 2M
  *        - 0x03: Advertiser PHY is LE Coded
  * @param periodic_advertising_interval Periodic advertising interval. Time = N
  *        * 1.25 ms; Time Range: 7.5ms to 81.91875 s.
  *        Values:
  *        - 0x0006 (7.50 ms)  ... 0xFFFF (NaN) : Periodic advertising interval
  * @param advertiser_clock_accuracy Advertiser Clock Accuracy
  *        Values:
  *        - 0x00: 500 ppm
  *        - 0x01: 250 ppm
  *        - 0x02: 150 ppm
  *        - 0x03: 100 ppm
  *        - 0x04: 75 ppm
  *        - 0x05: 60 ppm
  *        - 0x06: 30 ppm
  *        - 0x07: 20 ppm
  * @retval None
  */
WEAK_FUNCTION(void hci_le_periodic_advertising_sync_transfer_received_event(uint8_t status,
                                                                            uint16_t connection_handle,
                                                                            uint16_t service_data,
                                                                            uint16_t sync_handle,
                                                                            uint8_t advertising_sid,
                                                                            uint8_t advertiser_address_type,
                                                                            uint8_t advertising_address[6],
                                                                            uint8_t advertiser_phy,
                                                                            uint16_t periodic_advertising_interval,
                                                                            uint8_t advertiser_clock_accuracy))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_periodic_advertising_sync_transfer_received_event\r\n");
}
/**
  * @brief Report a path loss threshold crossing on the ACL connection identified
  *        by the connection_handle parameter.
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param current_path_loss Current path loss (always zero or positive). Units:
  *        dB.
  *        Values:
  *        - 255: NA
  *        - 0 ... 254
  * @param zone_entered The zone_entered parameter indicates which zone was
  *        entered.
  *        Values:
  *        - 0x00: LOW_ZONE
  *        - 0x01: MIDDLE_ZONE
  *        - 0x02: HIGH_ZONE
  * @retval None
  */
WEAK_FUNCTION(void hci_le_path_loss_threshold_event(uint16_t connection_handle,
                                                    uint8_t current_path_loss,
                                                    uint8_t zone_entered))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_path_loss_threshold_event\r\n");
}
/**
  * @brief Report the transmit power level on the ACL connection identified by
  *        the connection_handle parameter.
  * @param status For standard error codes see Bluetooth specification, Vol. 2,
  *        part D. For proprietary error code refer to Error codes section
  * @param connection_handle Connection handle that identifies the connection.
  *        Values:
  *        - 0x0000 ... 0x0EFF
  * @param reason The reason why the event was sent and the device whose transmit
  *        power level is being reported. - 0x00: Local transmit power changed -
  *        0x01: Remote transmit power changed - 0x02:
  *        HCI_LE_Read_Remote_Transmit_Power_Level command completed
  *        Values:
  *        - 0x00: LOCAL_TX_POWER_CHANGE
  *        - 0x01: REMOTE_TX_POWER_CHANGE
  *        - 0x02: READ_REMOTE_TX_POWER_COMPLETE
  * @param phy PHY associated with the connection  (not necessarily the currently
  *        used one).
  *        Values:
  *        - 0x01: LE_1M_PHY
  *        - 0x02: LE_2M_PHY
  *        - 0x03: LE_CODED_PHY_S8
  *        - 0x04: LE_CODED_PHY_S2
  * @param transmit_power_level The transmit power level for the PHY (dBm).
  *        Values:
  *        - 126: POWER_NOT_MANAGED
  *        - 127: POWER_NA
  *        - -127 ... 20
  * @param Transmit_Power_Level_Flag It indicates whether the transmit power
  *        level that is being reported has reached its minimum and/or maximum
  *        level. Transmit_Power_Level_Flag shall be ignored if the
  *        transmit_power_level parameter is set to 0x7E or 0x7F.
  *        Flags:
  *        - 0x01: MIN_TX_POWER_BIT
  *        - 0x02: MAX_TX_POWER_BIT
  * @param delta The Delta parameter is set to the change in power level for the
  *        transmitter being reported, whenever it changes its transmit power
  *        level. Delta shall be ignored if the transmit_power_level parameter is
  *        set to 0x7E.
  *        Values:
  *        - 127: NA
  *        - 0 ... 126
  * @retval None
  */
WEAK_FUNCTION(void hci_le_transmit_power_reporting_event(uint8_t status,
                                                         uint16_t connection_handle,
                                                         uint8_t reason,
                                                         uint8_t phy,
                                                         int8_t transmit_power_level,
                                                         uint8_t Transmit_Power_Level_Flag,
                                                         int8_t delta))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_le_transmit_power_reporting_event\r\n");
}
/**
  * @}
  */

