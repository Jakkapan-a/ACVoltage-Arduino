#include "AcVoltage.h"

AcVoltage::AcVoltage(uint8_t pin) : _pin(pin) // No vector initialization needed
{
    // Other initialization if needed
}

void AcVoltage::begin()
{
    pinMode(_pin, INPUT);
}

void AcVoltage::update()
{
    unsigned long currentMicros = micros();
    if (currentMicros - _lastReadTime >= 200) // 5kHz sampling rate
    {
        _lastReadTime = currentMicros;         // Update last read time
        if(_index < READ_VOLTAGE_SIZE)
        {
            readVoltage[_index] = analogRead(_pin);
            _index++;
        }
        else
        {
            _index = 0;
        }
    }
    else if (currentMicros < _lastReadTime) // Handling micros() overflow
    {
        _lastReadTime = currentMicros;
    }
}

float AcVoltage::getMaximum()
{
    float max = 0;
    for (int i = 0; i < READ_VOLTAGE_SIZE; i++) // Loop through the static array
    {
        if (readVoltage[i] > max)
        {
            max = readVoltage[i];
        }
    }
    return max;
}

float AcVoltage::getMinimum()
{
    float min = 1023;
    for (int i = 0; i < READ_VOLTAGE_SIZE; i++) // Loop through the static array
    {
        if (readVoltage[i] < min)
        {
            min = readVoltage[i];
        }
    }
    return min;
}

float AcVoltage::getAverage()
{
    float sum = 0;
    for (int i = 0; i < READ_VOLTAGE_SIZE; i++) // Loop through the static array
    {
        sum += readVoltage[i];
    }
    return sum / READ_VOLTAGE_SIZE;
}
 
float AcVoltage::getVoltageRMS()
{
    float voltage = getMaximum();
    voltage = voltage * 1100 / 1023; // Convert ADC value to voltage (mV)
    voltage /= sqrt(2);              // Calculate RMS voltage
    return this->calibrationVoltageRMS(voltage);
}


float AcVoltage::calibrationVoltageRMS(float value)
{
    this->bufferFilter[this->_filterIndex++] = value;
    if (this->_filterIndex >= FILTER_SIZE)
    {
        this->_filterIndex = 0;
    }

    // return value;
    // Apply the calibration factors obtained from the curve fitting
    // return value * 0.483 + 1.232;
    // return static_cast<uint32_t>(value * 0.488 + 1.030);
    float max = 0;


    for (int i = 0; i < FILTER_SIZE; i++)
    {
        if (this->bufferFilter[i] > max)
        {
            max = this->bufferFilter[i];
        }
    }
    // max = max < 1.6 ? 0 : max;
    Serial.print("max");
    Serial.print(1);
    Serial.print(":");
    Serial.print(max,2);
    Serial.print("V");

    Serial.print(" ");
    Serial.print("value");
    Serial.print(":");
    Serial.print(value,2);
    Serial.print("V");
    Serial.println();
    return this->adjustMeasure(max);
}

float AcVoltage::calculateRMSFromPeakToPeakValues(float minVoltage, float maxVoltage)
{
    float peakToPeakVoltage = maxVoltage - minVoltage;
    float rmsVoltage = peakToPeakVoltage / (2 * sqrt(2));
    return rmsVoltage;
}

float AcVoltage::adjustMeasure(float measureV) {
  float m = 0.9598; // Slope
  float b = 2.7405; // Intercept
  float adjustedV = (measureV - b) / m; // Adjust measureV to match Meter(V)
  return adjustedV;
}