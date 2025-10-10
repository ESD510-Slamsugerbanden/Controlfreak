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

    def send_command(self, azimuth: int, elevation: int):
        """
        Send a command packet with azimuth/elevation.
        """
        packet = struct.pack("<BBii", 0x42, 0x01, azimuth, elevation)
        self.sock.sendto(packet, (self.host, self.port))

    def receive_response(self):
        """
        Wait for a response packet.
        Returns (azimuth, elevation) as floats.
        """
        try:
            data, _ = self.sock.recvfrom(1024)
        except socket.timeout:
            raise TimeoutError("No response received")

        if len(data) < 10:
            raise ValueError(f"Invalid packet length: {len(data)}")

        id_byte, cmd, az, el = struct.unpack("<BBff", data[:10])

        if id_byte != 0x42 or cmd != 0x02:
            raise ValueError(f"Unexpected response: id={id_byte}, cmd={cmd}")

        return az, el

    def send_and_receive(self, azimuth: float, elevation: float):
        """
        Send command and wait for reply in one call.
        """
        self.send_command(azimuth, elevation)
        return self.receive_response()


client = UdpProtocolClient("192.168.4.1", 8700)
mouse = Controller()

while(True):
    ms = time.time()
    x, y = mouse.position
    azi = (x-1920/2) * -2
    ele = (y-1080/2)

    print(azi, ele)
    client.send_command(int(azi), int(ele))

    time.sleep(0.05)