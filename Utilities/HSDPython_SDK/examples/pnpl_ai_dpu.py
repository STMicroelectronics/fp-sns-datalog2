import time
import serial
import serial.tools.list_ports as list_ports
from threading import Thread, Event

from st_pnpl.PnPLCmd import PnPLCMDManager

class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\x00")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\x00")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)

class ReadTelemetryThread(Thread):
    def __init__(self, event, rl:ReadLine):
        Thread.__init__(self)
        self.stopped = event
        self.rl = rl

    def run(self):
        while not self.stopped.wait(0.5):
            print("received data:" + self.rl.readline().decode())

ports = list_ports.comports()
selected_port = None
for port in ports:
    if "USB Serial Device" in port.description and port.hwid.split(' ')[1].split('=')[1] == "0483:5740":
        selected_port = port
        print(selected_port)


serial_port = serial.Serial(selected_port.name, 115200, timeout=1)

if serial_port.is_open:
    
    rl = ReadLine(serial_port)
    
    serial_port.write(PnPLCMDManager.create_get_presentation_string_cmd().encode())
    print("-" + rl.readline().decode())

    serial_port.write(PnPLCMDManager.create_get_device_status_cmd().encode())
    print("-" + rl.readline().decode())
    
    serial_port.write(PnPLCMDManager.create_get_component_status_cmd("ism330dhcx_acc").encode())
    print("-" + rl.readline().decode())
    
    serial_port.write(PnPLCMDManager.create_set_property_cmd("firmware_info","alias","Vespucci Summer Demo").encode())
    serial_port.write(PnPLCMDManager.create_get_component_status_cmd("firmware_info").encode())
    print("-" + rl.readline().decode())

    serial_port.write(PnPLCMDManager.create_get_component_status_cmd("deviceinfo").encode())
    print("-" + rl.readline().decode())
    
    start_pnpl_cmd = PnPLCMDManager.create_command_cmd("ai_application","start")
    serial_port.write(start_pnpl_cmd.encode())
    
    stopFlag = Event()
    thread = ReadTelemetryThread(stopFlag, rl)
    thread.start()
    
    time.sleep(10)
    
    stop_pnpl_cmd = PnPLCMDManager.create_command_cmd("ai_application","stop")
    serial_port.write(stop_pnpl_cmd.encode())
    
    stopFlag.set()
    
else:
    print("Selected Serial port is not opened")