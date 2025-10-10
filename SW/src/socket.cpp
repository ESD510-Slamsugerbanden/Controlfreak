#include <common.h>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
// Replace with your network credentials
#define ssid "The TARM connection"
#define password "12345678"

WiFiUDP udp_socket;
unsigned int localUdpPort = 8700;  // Port to listen on



void task_socket(void* parameters){
    WiFi.softAP(ssid, NULL);
    udp_socket.begin(localUdpPort);
    Serial.println("AP-ready");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.printf("Listening on UDP port %d\n", localUdpPort);

    while(true){
        
        int packetSize = udp_socket.parsePacket();
        if (packetSize) {
            if(udp_socket.read() != 0x42){ //Forkert ID drop pakken
                udp_socket.flush();
            }
            
            long azimuth;
            long elevation;


            switch (udp_socket.read())
            {
            case 0x01:
                udp_socket.read((uint8_t*)&azimuth, sizeof(azimuth));
                udp_socket.read((uint8_t*)&elevation, sizeof(elevation));
                Serial.printf("Setting orientation: azi %d ele %d\n", azimuth, elevation);

                set_azi(azimuth);
                set_ele(elevation);

                break;
            case 0x02:
                azimuth = get_azi();
                elevation = get_ele();
                udp_socket.beginPacket(udp_socket.remoteIP(), udp_socket.remotePort());
                udp_socket.write((byte*)&azimuth, sizeof(azimuth));
                udp_socket.write((byte*)&elevation, sizeof(elevation));
                udp_socket.endPacket();
                break;

            default:
                Serial.print("[SOCKET] error parsing CMD\n");
                break;
            }
        }

        
    }
}

/*
    Okay den helt dumme protokol
    ID     CMD     Azimuth   Elevation
    0       1       2-5     6-9
    0x42 | 0x01 | 4 bytes | 4 bytes |

    ID      CMD    RETUR AZIMUTH    ELEVATION
    0x42 | 0X02 | => | 4 bytes | 4 bytes |
*/