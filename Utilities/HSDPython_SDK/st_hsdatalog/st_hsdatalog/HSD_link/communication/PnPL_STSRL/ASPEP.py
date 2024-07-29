
from abc import ABC
from enum import Enum
import struct

class ASPEPType(Enum):
    Beacon = 5
    Data = 1
    Async = 1
    Response = 2
    Ping = 6
    Error = 7

class ASPEPInnerHeader(ABC):
    pass

class ASPEPBeaconHeader(ASPEPInnerHeader):
    def __init__(self, version, crc, rxs_max, txs_max, txa_max):
        self.version = version
        self.crc = crc
        self.rxs_max = rxs_max
        self.txs_max = txs_max
        self.txa_max = txa_max

class ASPEPPingHeader(ASPEPInnerHeader):
    def __init__(self, c, n, liid, packet_number):
        self.c = c
        self.n = n
        self.liid = liid
        self.packet_number = packet_number
    
class ASPEPErrorHeader(ASPEPInnerHeader):
    def __init__(self, error_code):
        self.error_code = error_code

class ASPEPDataHeader(ASPEPInnerHeader):
    def __init__(self, payload_length):
        self.payload_length = payload_length

class ASPEPAsyncHeader(ASPEPDataHeader):
    def __init__(self, payload_length):
        super().__init__(payload_length)

class ASPEPResponseHeader(ASPEPDataHeader):
    def __init__(self, payload_length):
        super().__init__(payload_length)

class ASPEPHeader:
    def __init__(self, p_type:ASPEPType, parity, inner_header:ASPEPInnerHeader, crch):
        self.p_type = p_type
        self.parity = parity
        self.inner_header = inner_header
        self.crch = crch
class ASPEPPacket:
    def __init__(self, header, data = None):
        self.header:ASPEPHeader = header
        self.data = data
class ASPEP:
    DATA_MSG_TYPE = 0x1
    BEACON_PKT_TYPE = 0x5
    PING_PKT_TYPE = 0x6
    HEADER_SIZE = 4
    TXS_SLAVE_MAX = 4096
    RXS_SLAVE_MAX = 2048
    TXA_SLAVE_MAX = 8128
    
    def __init__(self, ser) -> None:
        self.TXS_SLAVE_MAX = ASPEP.TXS_SLAVE_MAX
        self.RXS_SLAVE_MAX = ASPEP.RXS_SLAVE_MAX
        self.TXA_SLAVE_MAX = ASPEP.TXA_SLAVE_MAX #tx -> slave to master
        self.send_beacon(ser)
        self.send_ping(ser)
    
    @staticmethod
    def compute_header_CRC(header):
    
        CRC4_Lookup8 = [
            0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x07, 0x05, 0x03, 0x01, 0x0f, 0x0d, 0x0b, 0x09,
            0x07, 0x05, 0x03, 0x01, 0x0f, 0x0d, 0x0b, 0x09, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e,
            0x0e, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x00, 0x09, 0x0b, 0x0d, 0x0f, 0x01, 0x03, 0x05, 0x07,
            0x09, 0x0b, 0x0d, 0x0f, 0x01, 0x03, 0x05, 0x07, 0x0e, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x00,
            0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05, 0x0c, 0x0e, 0x08, 0x0a, 0x04, 0x06, 0x00, 0x02,
            0x0c, 0x0e, 0x08, 0x0a, 0x04, 0x06, 0x00, 0x02, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05,
            0x05, 0x07, 0x01, 0x03, 0x0d, 0x0f, 0x09, 0x0b, 0x02, 0x00, 0x06, 0x04, 0x0a, 0x08, 0x0e, 0x0c,
            0x02, 0x00, 0x06, 0x04, 0x0a, 0x08, 0x0e, 0x0c, 0x05, 0x07, 0x01, 0x03, 0x0d, 0x0f, 0x09, 0x0b,
            0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x06, 0x04, 0x02, 0x00, 0x0e, 0x0c, 0x0a, 0x08,
            0x06, 0x04, 0x02, 0x00, 0x0e, 0x0c, 0x0a, 0x08, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f,
            0x0f, 0x0d, 0x0b, 0x09, 0x07, 0x05, 0x03, 0x01, 0x08, 0x0a, 0x0c, 0x0e, 0x00, 0x02, 0x04, 0x06,
            0x08, 0x0a, 0x0c, 0x0e, 0x00, 0x02, 0x04, 0x06, 0x0f, 0x0d, 0x0b, 0x09, 0x07, 0x05, 0x03, 0x01,
            0x0a, 0x08, 0x0e, 0x0c, 0x02, 0x00, 0x06, 0x04, 0x0d, 0x0f, 0x09, 0x0b, 0x05, 0x07, 0x01, 0x03,
            0x0d, 0x0f, 0x09, 0x0b, 0x05, 0x07, 0x01, 0x03, 0x0a, 0x08, 0x0e, 0x0c, 0x02, 0x00, 0x06, 0x04,
            0x04, 0x06, 0x00, 0x02, 0x0c, 0x0e, 0x08, 0x0a, 0x03, 0x01, 0x07, 0x05, 0x0b, 0x09, 0x0f, 0x0d,
            0x03, 0x01, 0x07, 0x05, 0x0b, 0x09, 0x0f, 0x0d, 0x04, 0x06, 0x00, 0x02, 0x0c, 0x0e, 0x08, 0x0a
        ]
        CRC4_Lookup4 = [
            0x00, 0x07, 0x0e, 0x09, 0x0b, 0x0c, 0x05, 0x02, 0x01, 0x06, 0x0f, 0x08, 0x0a, 0x0d, 0x04, 0x03
        ]

        crc = 0
        header = header & 0x0fffffff

        crc = CRC4_Lookup8[crc ^ (header & 0xff)]
        crc = CRC4_Lookup8[crc ^ ((header >> 8) & 0xff)]
        crc = CRC4_Lookup8[crc ^ ((header >> 16) & 0xff)]
        crc = CRC4_Lookup4[crc ^ ((header >> 24) & 0x0f)]
        
        header = (crc << 28) | header
        return header

    def __build_data_pkt_header(self, payload_len):
        header = (ASPEP.DATA_MSG_TYPE |
                  (1 << 3) |
                  (payload_len << 4) |
                  (0 << 17)  )
        return ASPEP.compute_header_CRC(header)
    
    def send_data(self, ser, byte_array):
        header = self.__build_data_pkt_header(len(byte_array))
        header = header.to_bytes(ASPEP.HEADER_SIZE,"little")
        cplt_pkt = header + byte_array
        if len(cplt_pkt) <= self.RXS_SLAVE_MAX:
            # Send the byte array via the COM port
            ser.write(cplt_pkt)
            return True
        else:
            print(f"ERROR: packet being sent is too large (> self.RXS_SLAVE_MAX={self.RXS_SLAVE_MAX})")
            return False
        
    def send_and_receive_bytes(self, ser, byte_array, response_len):
        # Send the byte array via the COM port
        ser.write(byte_array)
        # Wait for a response and return it
        response = self.receive_bytes(ser)
        return response

    def send_and_receive(self, ser, hex_str, response_len):
        # Convert the hex string to a byte array
        byte_array = bytes.fromhex(hex_str)
        # Send the byte array via the COM port
        ser.write(byte_array)
        # Wait for a response and return it
        response = self.receive_bytes(ser)
        return response
    
    def calculate_parity_bit(self, binary_str, parity_type='even'):
        # Count the number of 1's in the binary string
        num_ones = binary_str.count('1')
        if parity_type == 'even':
            # If the number of 1's is odd, return 1 to make it even
            return 1 if num_ones % 2 != 0 else 0
        elif parity_type == 'odd':
            # If the number of 1's is even, return 1 to make it odd
            return 1 if num_ones % 2 == 0 else 0
        else:
            raise ValueError("Invalid parity type. Choose 'even' or 'odd'.")
    
    def send_beacon(self, ser):
        # Construct the packet by combining the fields with bitwise operations
        packet = (ASPEP.BEACON_PKT_TYPE |
                ((0 & 0x07) << 4) |  # Assuming version is 3 bits
                (0 << 7) |
                ((int(self.RXS_SLAVE_MAX/32)-1) << 8) |
                ((int(self.TXS_SLAVE_MAX/32)-1) << 14) |
                (int(self.TXA_SLAVE_MAX/64) << 21))
        cplt_packet = self.compute_header_CRC(packet)
        self.send_and_receive_bytes(ser, cplt_packet.to_bytes(ASPEP.HEADER_SIZE, "little"), ASPEP.HEADER_SIZE)

    def send_ping(self, ser): #TODO complete this feature (hardcoded 0 values @ the moment )
        # Construct the packet by combining the fields with bitwise operations
        packet = (ASPEP.PING_PKT_TYPE |
                 (0 << 3) |
                 (0 << 4) |
                 (0 << 5) |
                 (0 << 6) |
                 (0 << 7) |
                 (0 << 8) |
                 (0 << 12) |
                 (0 << 28))
        cplt_packet = self.compute_header_CRC(packet)
        self.send_and_receive_bytes(ser, cplt_packet.to_bytes(ASPEP.HEADER_SIZE, "little"), ASPEP.HEADER_SIZE)
        
    def receive_bytes(self, ser):
        header = ser.read(4)
        p_type = ASPEPType(header[0] & 0b111)
        parity = (header[0] >> 3) & 0b1
        inner_header = None
        aspep_payload = None
        if p_type == ASPEPType.Beacon:
            version = (header[0] >> 4) & 0b111
            crc = (header[0] >> 7) & 0b1
            rxs_max = header[1] & 0b111111
            updated_params = False
            if ((rxs_max+1)*32) != self.RXS_SLAVE_MAX:
                print(f"ASPEP RXS_MAX updated {self.RXS_SLAVE_MAX},{rxs_max}")
                self.RXS_SLAVE_MAX = ((rxs_max+1)*32)
                updated_params = True
            txs_max = (((header[1] >> 6) & 0b11) << 5) + (header[2] & 0b11111)
            if ((txs_max+1)*32) != self.TXS_SLAVE_MAX:
                print(f"ASPEP TXS_MAX updated {self.TXS_SLAVE_MAX},{txs_max}")
                self.TXS_SLAVE_MAX = ((txs_max+1)*32)
                updated_params = True
            txa_max = (((header[2] >> 5) & 0b111) << 4) + (header[3] & 0b1111)
            if (txa_max*64) != self.TXA_SLAVE_MAX:
                print(f"ASPEP TXA_MAX updated {self.TXA_SLAVE_MAX},{txa_max}")
                self.TXA_SLAVE_MAX = (txa_max*64)
                updated_params = True
            if updated_params: #If one of the parameters has changed, then re-send a beacon with updated parameters
                self.send_beacon(ser)
            inner_header = ASPEPBeaconHeader(version, crc, rxs_max, txs_max, txa_max)
        elif p_type == ASPEPType.Ping:
            c = (header[0] >> 4) & 0b1
            c2 = (header[0] >> 5) & 0b1
            if c != c2:
                print("ERROR: ASPEP Ping - different C values in packet")
                return
            n = (header[0] >> 6) & 0b1
            n2 = (header[0] >> 7) & 0b1
            if n != n2:
                print("ERROR: ASPEP Ping - different N values in packet")
                return
            liid = header[1] & 0b1111
            packet_number = (((header[1] >> 4) & 0b1111) << 8) + \
                            ((header[2] & 0b11111111) << 4) + \
                            (header[3] & 0b1111)
            inner_header = ASPEPPingHeader(c, n, liid, packet_number)
        elif p_type == ASPEPType.Error:
            error_code = header[1]
            error_code2 = header[2]
            if error_code != error_code2:
                print("ERROR: ASPEP ERROR - different error code values in packet")
                return
        elif p_type == ASPEPType.Data or p_type == ASPEPType.Async or p_type == ASPEPType.Response:
            full_bit_header = ''.join(f'{byte:08b}' for byte in header[::-1])
            start_index = len(full_bit_header) - 17
            end_index = len(full_bit_header) - 4
            # Now slice the original string using the calculated indices
            extracted_bits = full_bit_header[start_index:end_index]
            # Convert the extracted bit string to an integer
            payload_length = int(extracted_bits, 2)
            if p_type == ASPEPType.Data:
                inner_header = ASPEPDataHeader(payload_length)
            elif p_type == ASPEPType.Async:
                inner_header = ASPEPAsyncHeader(payload_length)
            elif p_type == ASPEPType.Response:
                inner_header = ASPEPResponseHeader(payload_length)
            aspep_payload = ser.read(payload_length)
        crch = (header[3] >> 4) & 0b1111
        aspep_header = ASPEPHeader(p_type, parity, inner_header, crch)
        aspep_packet = ASPEPPacket(aspep_header, aspep_payload)
        return aspep_packet