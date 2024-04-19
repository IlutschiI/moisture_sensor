#include <Arduino.h>
#include <U8g2lib.h>
#include <LittleVector.h>

enum OperationMode {
    CALIBRATING_DRY_PRE,
    CALIBRATING_DRY,
    CALIBRATING_DRY_POST,
    CALIBRATING_WET_PRE,
    CALIBRATING_WET,
    CALIBRATING_WET_POST,
    MEASURING
};

unsigned int dryVoltage = 800;
unsigned int wetVoltage = 550;
int buttonPin = 2;
byte buttonState = HIGH;
OperationMode calibration_status = MEASURING;
LittleVector<int> vector;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

void printToDisplay(const char *texts[]) {
    u8g2.clearBuffer();
    constexpr int size = sizeof(texts) / sizeof(texts[0]);
    for (int i = 0; i <= size; i++) {
        u8g2.drawStr(8, 6 * (i + 1) + 2 * i, texts[i]);
    }
    u8g2.sendBuffer();
}

int soilMoistureValue = 0;

void setup() {
    u8g2.begin();
    u8g2.setFont(u8g2_font_5x7_mf);
    Serial.begin(9600);
    pinMode(buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH);
}

bool is_button_pressed() {
    bool buttonPressed = false;
    byte newButtonState = digitalRead(buttonPin);
    if (newButtonState == LOW && newButtonState != buttonState) {
        buttonState = newButtonState;
        buttonPressed = true;
    } else if (newButtonState == HIGH && newButtonState != buttonState) {
        buttonState = newButtonState;
    }
    return buttonPressed;
}

void loop() {
    const bool buttonPressed = is_button_pressed();
    if (buttonPressed) {
        calibration_status = static_cast<OperationMode>((calibration_status + 1) % (MEASURING + 1));
    }

    if (calibration_status == MEASURING) {
        soilMoistureValue = analogRead(A0);
        int percentage = map(soilMoistureValue, dryVoltage, wetVoltage, 0, 100);
        if (percentage < 0) {
            percentage = 0;
        }
        if (percentage > 100) {
            percentage = 100;
        }
        String valueString = String(percentage) + " % " + String(soilMoistureValue);
        Serial.println(valueString);
        const char *texts[] = {"Moisture level:", valueString.c_str()};
        printToDisplay(texts);
    }

    if (calibration_status == CALIBRATING_DRY_PRE) {
        vector.clear();
        const char *text[] = {"Clean the sensor!", "Press to calibrate"};
        printToDisplay(text);
    }
    if (calibration_status == CALIBRATING_DRY) {
        const String samplesText = "Samples collected: " + String(vector.size());
        const char *text[] = {"Calibration Dry", samplesText.c_str()};
        printToDisplay(text);
        const int soilMoistureValue = analogRead(A0);
        Serial.println(vector.size());
        vector.push_back(soilMoistureValue);
    }
    if (calibration_status == CALIBRATING_DRY_POST) {
        unsigned int combinedValue = 0;
        for (const auto value: vector) {
            combinedValue+=value;
        }
        const unsigned int average = combinedValue / vector.size();
        dryVoltage = average;
        const String averageText = "Dry value: " + String(average);
        const char *text[] = {"Calibration finished", averageText.c_str()};
        printToDisplay(text);
    }
    if (calibration_status == CALIBRATING_WET_PRE) {
        vector.clear();
        const char *text[] = {"Clean the sensor!", "Press to calibrate"};
        printToDisplay(text);
    }
    if (calibration_status == CALIBRATING_WET) {
        const String samplesText = "Samples collected: " + String(vector.size());
        const char *text[] = {"Calibration Wet", samplesText.c_str()};
        printToDisplay(text);
        const int soilMoistureValue = analogRead(A0);
        Serial.println(vector.size());
        vector.push_back(soilMoistureValue);
    }
    if (calibration_status == CALIBRATING_WET_POST) {
        int unsigned combinedValue = 0;
        for (const auto value: vector) {
            combinedValue+=value;
        }

        const unsigned int average = combinedValue / vector.size();
        wetVoltage = average;
        const String averageText = "Wet value: " + String(average);
        const char *text[] = {"Calibration finished", averageText.c_str()};
        printToDisplay(text);
    }

    delay(100);
}
