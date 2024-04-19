#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

void printToDisplay(const char *texts[]) {
    u8g2.clearBuffer();
    int size = sizeof(texts) / sizeof(texts[0]);
    for (int i = 0; i <= size; i++) {
        u8g2.drawStr(8, 8 * (i + 1) + 2 * (i), texts[i]);
        //Serial.println(texts[i]);
    }
    u8g2.sendBuffer();
}

int soilMoistureValue = 0;

void setup() {
    u8g2.begin();
    u8g2.setFont(u8g2_font_t0_11_tr);
    Serial.begin(9600);
}

void loop() {
    soilMoistureValue = analogRead(A0);
    int percentage = map(soilMoistureValue, 800, 550, 0, 100);
    if (percentage < 0) {
        percentage = 0;
    }
    if (percentage > 100) {
        percentage = 100;
    }
    String valueString = String(percentage) + " %";
    Serial.println(valueString);
    const char *texts[] = {"Moisture level:", valueString.c_str()};
    printToDisplay(texts);
    delay(1000);
}
