from st_hsdatalog.HSD_link.communication.PnPL_STSRL.ASPEP import ASPEP, ASPEPType
from enum import Enum

class SSTLHeader:
    
    def __init__(self, vv, rrr, cr, fin, ch_num, sequence_num):
        self.vv = vv
        self.rrr = rrr
        self.cr = cr
        self.fin = fin
        self.ch_num = ch_num
        self.sequence_num = sequence_num
class SSTLPacket:
    
    def __init__(self, header, data = None):
        self.header:SSTLHeader = header
        self.data = data

class SSTL:

    HEADER_SIZE = 4
    PROTOCOL_VERSION = 0
    PROTOCOL_RRR = 0
    COMMAND_REQUEST_TYPE = 1
    RESPONSE_TYPE = 2

    def __init__(self, ser) -> None:
        self.aspep_manager = ASPEP(ser)
        self.MAX_RX_SLAVE_PKT_SIZE = self.aspep_manager.RXS_SLAVE_MAX - self.aspep_manager.HEADER_SIZE

    def __build_command_header(self): #TODO: manage channels and long messages
        header = (SSTL.PROTOCOL_VERSION |
                  (SSTL.PROTOCOL_RRR << 2) |
                  (SSTL.COMMAND_REQUEST_TYPE << 5) |
                  (1 << 8) |
                  (0 << 9) |
                  (0 << 31))
        return header

    def send_command(self, ser, command_str):
        byte_array = command_str.encode('utf-8')
        header = self.__build_command_header()
        header = header.to_bytes(SSTL.HEADER_SIZE,"little")
        byte_array = header + byte_array
        print("\n\rFrame: ", byte_array)
        self.aspep_manager.send_data(ser, byte_array)

    #debug
    def send_bytes(self, ser, byte_array):
        self.aspep_manager.send_data(ser, byte_array)
    
    def receive(self,ser):
        response = self.aspep_manager.receive_bytes(ser)
        sstl_packet = None
        if response.header.p_type == ASPEPType.Data or \
            response.header.p_type == ASPEPType.Async or \
            response.header.p_type == ASPEPType.Response:
            # extract the vv value from the response header
            vv = response.data[0] & 0b11
            # extract the rrr value from the response header
            rrr = (response.data[0] >> 2) & 0b111
            # extract the cr value from the response header
            cr = response.data[0] >> 5
            # extract the fin value from the response header
            fin = response.data[1] & 0b1
            # extract the ch_num value from the response header
            ch_num = response.data[1] >> 1
            # extract sequence value from the response header by combining response[3] and response[2] into a single value
            sequence_num = (response.data[3] << 8) | response.data[2]
            sstl_header = SSTLHeader(vv, rrr, cr, fin, ch_num, sequence_num)
            data = response.data[SSTL.HEADER_SIZE:]
            if len(data) == 0:
                print("SSTL packect received with empty data")
            sstl_packet = SSTLPacket(sstl_header, data[:-1] if cr != 0 else data) #TODO why [-1] in commands?
        return sstl_packet