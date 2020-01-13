#include <Adafruit_CircuitPlayground.h>

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
    #error "Infrared support is only for the Circuit Playground Express, it doesn't work with the Classic version"
#endif

uint8_t IR_protocol[256];
uint32_t IR_value[256];
uint16_t IR_bits[256];

uint8_t data_ptr;

void clear_data() {
    data_ptr = 0;
    for (int i = 0; i < 256; i++) {
        IR_protocol[i] = 0;
        IR_value[i] = 0;
        IR_bits[i] = 0;
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    CircuitPlayground.begin();

    CircuitPlayground.irReceiver.enableIRIn();

    clear_data();
}

void loop() {
    if (CircuitPlayground.irReceiver.getResults()) {
        if (CircuitPlayground.irDecoder.decode()) {
                CircuitPlayground.irDecoder.dumpResults(false);

                Serial.println("Signal captured. Press left button to save.");
                IR_protocol[data_ptr] = CircuitPlayground.irDecoder.protocolNum;
                IR_value[data_ptr] = CircuitPlayground.irDecoder.value;
                IR_bits[data_ptr] = CircuitPlayground.irDecoder.bits;
        }

        CircuitPlayground.irReceiver.enableIRIn();
    }

    if (CircuitPlayground.leftButton()) {
        if (IR_protocol[data_ptr]) {
            Serial.print("Data #"); Serial.print(data_ptr + 1); Serial.println(" saved.");
            data_ptr++;
        }

        while (CircuitPlayground.leftButton());
    }

    if (CircuitPlayground.rightButton()) {
        Serial.println("Data cleared.");
        clear_data();
        while (CircuitPlayground.rightButton());
    }

    if (CircuitPlayground.slideSwitch()) {
        for (int i = 0; i < data_ptr; i++) {
            CircuitPlayground.irSend.send(IR_protocol[i], IR_value[i], IR_bits[i]);

            Serial.print("Protocol: "); Serial.print(IR_protocol[i],DEC);
            Serial.print(" Value: 0x"); Serial.print(IR_value[i],HEX);
            Serial.print(" Bits: "); Serial.println(IR_bits[i],DEC);
            delay(100);
        }
    }

    delay(50);
}
