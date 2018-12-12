// Name: Chase Lynd
// Class: ME 701
// Assignment: End-of-Semester Project

// -------------------------------------------------
// This file contains the serialReceive() function
// that is used for receiving and processing data
// from the A1339 GUI. The serialReceive() function
// is a modified and simplified version of the
// parseOneByte() function in GloLink.cpp (line 77)
// from Dr. Schinstock.
// -------------------------------------------------

#include <string>
#include "A1339.h"

using std::stof;

// declare an instance of the A1339 class
A1339 sensor;

void serialReceive(uint8_t in_byte)
{
    const uint8_t signalBytes[4] = {'}', ')', ']', '>'};
    static int8_t caseIndex = 0;
    static uint8_t numBodyBytes = 0, msgBodyIndex = 0;
    uint8_t bodyRXbuffer[255];
    
    switch(caseIndex) 
    {
    case 0:                                // looking for first frame byte
        if (in_byte==signalBytes[0]) {caseIndex++;}
        else {caseIndex = 0; msgBodyIndex=0;}
        break;
    case 1:                                 // looking for 2nd frame byte
        if (in_byte==signalBytes[1]) {caseIndex++;}
        else {caseIndex = 0; msgBodyIndex=0;}
        break;
    case 2:                                 // looking for 3rd frame byte
        if (in_byte==signalBytes[2]) {caseIndex++; msgBodyIndex=0;}
        else {caseIndex = 0; msgBodyIndex=0;}
        break;
    case 3:                                 // looking for 4th frame byte
        if (in_byte==signalBytes[3]) {caseIndex++; msgBodyIndex=0;}
        else {caseIndex = 0; msgBodyIndex=0;}
        break;
    case 4:                                 // pulling out length of data
        numBodyBytes = in_byte;
        caseIndex++;
        if(numBodyBytes>255){caseIndex = 0; msgBodyIndex=0;} // never true with numBodyBytes as uint8
        break;
    case 5:                                 // pulling out body
        bodyRXbuffer[msgBodyIndex] = in_byte;
        msgBodyIndex++;
        if(msgBodyIndex >= numBodyBytes) {caseIndex++;}
        break;
    case 6:                                 // decide what to do with the data
        if (bodyRXbuffer[0] == 'U') {sensor.unlock();}
        else if (bodyRXbuffer[0] == ('A' || 'B' || 'C' || 'D' || 'E' || 'F'))
        {
            std::string data_str;
            std::string::size_type sz;

            for (int i=1, i<=msgBodyIndex, ++i)
            {
                data_str += bodyRXbuffer[i];
            }
            float data_flt = std::stof(data_str, &sz);

            if (bodyRXbuffer[0] == 'A') {sensor.setRotDir((int)data_flt);}
            else if (bodyRXbuffer[0] == 'B') {sensor.setZeroPosOffset(data_flt);}
            else if (bodyRXbuffer[0] == 'C') {sensor.setHysteresis(data_flt);}
            else if (bodyRXbuffer[0] == 'D') {sensor.burnRotDir((int)data_flt);}
            else if (bodyRXbuffer[0] == 'E') {sensor.burnZeroPosOffset(data_flt);}
            else if (bodyRXbuffer[0] == 'F') {sensor.burnHysteresis(data_flt);}
        }
        msgBodyIndex=0;
        caseIndex=0;
        break;
    default:                                 // safety resest
        {caseIndex = -1; msgBodyIndex=0;}
    }
}
