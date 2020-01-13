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
    Serial.println("Send an IR signal and I will record it.");
    Serial.println("Press the left button and we will retransmit it.");
    CircuitPlayground.begin();

    CircuitPlayground.irReceiver.enableIRIn(); // Start the receiver

    clear_data();
}

void loop() {
    /* Receiver will look for a signal and when a complete frame of data
     * has been received, getResults() returns true. Once that happens,
     * the receiver stops reccording so you will need to restart it
     * after you have processed the data.
     */
    if (CircuitPlayground.irReceiver.getResults()) {
        // attempt to decode it
        if (CircuitPlayground.irDecoder.decode()) {
                Serial.println("IR decoded");
                // Print the results. Change parameter to "true" for verbose output.
                CircuitPlayground.irDecoder.dumpResults(false);

                Serial.println("Signal captured. Press left button to save.");
                IR_protocol[data_ptr] = CircuitPlayground.irDecoder.protocolNum;
                IR_value[data_ptr] = CircuitPlayground.irDecoder.value;
                IR_bits[data_ptr] = CircuitPlayground.irDecoder.bits;
        }

        CircuitPlayground.irReceiver.enableIRIn(); // Restart receiver
    }

    /* If the left button is pressed and we have received a code
     * retransmit it using the sender.
     */
    if (CircuitPlayground.leftButton()) {
        if (IR_protocol[data_ptr]) {
            Serial.print("Data #"); Serial.print(data_ptr + 1); Serial.println(" saved.");
            data_ptr++;
        }

        while (CircuitPlayground.leftButton()); // Wait until the button is released
    }

    if (CircuitPlayground.rightButton()) {
        Serial.println("Data cleared.");
        clear_data();
        while (CircuitPlayground.rightButton()); // Wait until the button is released
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