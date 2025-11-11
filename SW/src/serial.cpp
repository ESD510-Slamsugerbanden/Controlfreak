#include <Arduino.h>
#include <common.h>

void task_serial(void *parameters) {
    String input = "";

    while (true) {
        while (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '\n') {
                float azimuth = 0.0;
                float elevation = 0.0;

                // Forventet format: "AZI:123.4 ELE:56.7"
                if (sscanf(input.c_str(), "AZI:%f ELE:%f", &azimuth, &elevation) == 2) {
                    set_azi_deg(azimuth);
                    set_ele_deg(elevation);
                    Serial.printf("Received -> AZI: %.2f°, ELE: %.2f°\n", azimuth, elevation);
                } else {
                    Serial.printf("Invalid input: %s\n", input.c_str());
                }

                input = ""; // nulstil buffer
            } else {
                input += c;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // lidt pause for CPU'en
    }
}
