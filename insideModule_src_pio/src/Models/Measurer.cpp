#include "Measurer.h"
Measurer::Measurer(const char* name)
{
    // this->name = new char[strlen(name)];
    // this->name = name;
    //this->name = new char[200];
    this->name = name;
}
const char* Measurer::getOutput()
{
    delete[] output; // clear output buffer

    // Allocate memory for output:
    output = new char[OUTPUT_BUFFER_SIZE];
    output[0] = '\0';

    // Append name:
    strncat(output, name, OUTPUT_BUFFER_SIZE - strlen(output));

    // Append temp/hum to output
    char dataString[50];
    dataString[0] = '\0';
    snprintf(dataString, sizeof(dataString), "\nT %s C\nHum %s %%", temperature, humidity);
    strncat(output, dataString, OUTPUT_BUFFER_SIZE - strlen(output));

    return output;
}
const char* Measurer::getTemperature()
{
    return temperature;
}
const char* Measurer::getHumidity()
{
    return humidity;
}

Measurer::~Measurer()
{
    delete[] output;
    //delete[] name;
}
