#ifndef AcVoltage_h
#define AcVoltage_h

#include <Arduino.h>

class AcVoltage
{
private:
    uint8_t _pin;
    unsigned long _lastReadTime;
    static const int READ_VOLTAGE_SIZE = 100;  // Static array size
    float readVoltage[READ_VOLTAGE_SIZE];  // Static array instead of std::vector
    int _index = 0;
    static const int FILTER_SIZE = 15;
    float bufferFilter[FILTER_SIZE];
    int _filterIndex = 0;
    float adjustMeasure(float measureV);
public:
    AcVoltage(uint8_t pin);
    void begin();
    void update();
    float getMinimum();
    float getMaximum();
    float getAverage();
    float getVoltageRMS();  // Corrected return type to match .cpp file
    float calibrationVoltageRMS(float value);
    float calculateRMSFromPeakToPeakValues(float minVoltage, float maxVoltage);

    // Optional: Method to access readings (for debugging or direct access)
    const float* getReadings() const { return readVoltage; }
};

#endif
