import socket
import struct
import time
import math
from pynput.mouse import Controller

class UdpProtocolClient:
    def __init__(self, host="192.168.4.1", port=8700, timeout=2.0):
        """
        UDP interface for ESP32 protocol.
        host: ESP32 IP (default 192.168.4.1 if ESP32 is AP)
        port: UDP port (default 4210)
        timeout: socket timeout for receive
        """
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(timeout)

    def set_pos(self, azimuth: float, elevation: float):
        """
        Send a command packet with azimuth/elevation.
        """
        packet = struct.pack("<BBff", 0x42, 0x01, azimuth, elevation)
        self.sock.sendto(packet, (self.host, self.port))

    def get_pos(self):
        """
        Wait for a response packet.
        Returns (azimuth, elevation) as floats.
        """
        self.sock.sendto(bytes([0x42, 0x02]), (self.host, self.port))
        try:
            data, _ = self.sock.recvfrom(1024)
        except socket.timeout:
            raise TimeoutError("No response received")

        if len(data) != 8:
            raise ValueError(f"Invalid packet length: {len(data)}")

        az, el = struct.unpack("<ff", data[:8])

        return az, el



client = UdpProtocolClient("192.168.4.1", 8700)
mouse = Controller()
if __name__ == "__main__":
    while(True):
        ms = time.time()
        x, y = mouse.position 
        azi = (x/1920)*2-1
        ele = (y/1080)*2-1

        print(azi, ele)
        #print(client.get_pos())
        client.set_pos(float(azi)*45, 90)

        time.sleep(0.05)