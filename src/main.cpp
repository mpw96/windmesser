#include <Homie.h>

#define SEND_INTERVAL_MILLIS 10000
#define READ_INTERVAL_MILLIS 1000

// homie firmware
#define FW_NAME_HOMIE "windmesser"
#define FW_VERSION_HOMIE "0.0.1"

const uint8_t ANALOG_IN_PIN(A0);

HomieSetting<double> adcCorrection("adc_correction", "Correction factor for AD converter. [0.5 .. 1.5], default = 1.0");
HomieSetting<double> adcOffset("adc_offset", "Offset for AD converter. [0.0 .. 1.0], default = 0.0");

HomieNode airSpeedNode("airspeed", "Airspeed", "number");
HomieNode voltageNode("voltage", "Voltage", "number");

float readADC() {
    int voltage_raw = analogRead(ANALOG_IN_PIN);
    String voltage_bits(voltage_raw, BIN);
    while (voltage_bits.length() < 10) {
      voltage_bits = "0" + voltage_bits;
    }

    Serial << "read " << voltage_raw << " (" << voltage_bits << ") from ADC." << endl;

    float voltage = (float)voltage_raw / 1024.0f * (float) adcCorrection.get() + (float) adcOffset.get(); 

    if (isnan(voltage)) {
      voltage = 0.0f;
      Serial << "voltage is not a number, set it to "<< voltage << "." << endl;

    }

    Serial << "read voltage " << voltage << "V from ADC." << endl;
    return voltage;    
}

float getAirSpeedFromVoltage(float voltage) {
    float airspeed = voltage * 25.0f * 3.6f;
    Serial << "airspeed is " << airspeed << "km/h." << endl;
    return airspeed;
}

void homieLoopFunction() {
    static long lastSend = 0;
    static long lastRead = 0;

    long now = millis();
    long millisSinceLastRead = now - lastRead;
    long millisSinceLastSend = now - lastSend;

    if (millisSinceLastRead > READ_INTERVAL_MILLIS) {
        // read sensor
        float voltage = readADC();
        float airspeed = getAirSpeedFromVoltage(voltage);
        lastRead = now;
        if (millisSinceLastSend > SEND_INTERVAL_MILLIS) {
            // send data
            String voltage_string = String(voltage);
            String airspeed_string = String(airspeed);
            Serial << "sending voltage " << voltage_string << " and airspeed " << airspeed_string << "." << endl;
            voltageNode.setProperty("voltage").send(voltage_string);
            airSpeedNode.setProperty("airspeed").send(airspeed_string);
            lastSend = now;
        }
    }
}

bool validateOffset(double candidate) {
    Serial << "offset candidate is " << candidate << "." << endl;
    bool accepted = 0.0 <= candidate && candidate <= 1.0;
    Serial << "offset candidate " << candidate << " is " << (accepted?"":"not ") << "accepted." << endl;
    return accepted;
}

bool validateCorrection(double candidate) {
    Serial << "correction candidate is " << candidate << "." << endl;
    bool accepted = 0.5 <= candidate && candidate <= 1.5;
    Serial << "offset candidate " << candidate << " is " << (accepted?"":"not ") << "accepted." << endl;
    return accepted;
}


void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware(FW_NAME_HOMIE, FW_VERSION_HOMIE);
    Homie.setLoopFunction(homieLoopFunction);

    airSpeedNode.advertise("airspeed").setName("Airspeed").setDatatype("float").setUnit("km/h");
    voltageNode.advertise("voltage").setName("Voltage").setDatatype("float").setUnit("V");

    adcCorrection.setValidator(&validateCorrection);
    adcOffset.setValidator(&validateOffset);

    Homie.setup();
    Serial << "Setup done." << endl;
}

void loop() {
    /*
    Do this to reset the ESP (to be able to upload a new config.json)
    Homie.setIdle(true);
    Homie.reset();
    */
    Homie.loop();
    delay(500);
}
