#include <string>
#include "A1339.h"

// declare an instance of the A1339 class
A1339 sensor;

uint16_t parsed_object_id_;
bool complete_object_parsed_;
uint8_t object_rx_buffer_[255];

void serialReceive(uint8_t in_byte)
{
    const uint8_t msg_frame[4] = {'}', ')', ']', '>'};
    static int8_t msgSectionIndex = 0;
    static uint8_t objID = 0, num_body_bytes =0, bodyindex=0;
    static uint16_t instance;
    
    switch(msgSectionIndex) 
    {
    case 0:                                // looking for first frame byte
        if (in_byte==msg_frame[0]) {msgSectionIndex++;}
        else {msgSectionIndex = 0; bodyindex=0;}
        break;
    case 1:                                 // looking for 2nd frame byte
        if (in_byte==msg_frame[1]) {msgSectionIndex++;}
        else {msgSectionIndex = 0; bodyindex=0;}
        break;
    case 2:                                 // looking for 3rd frame byte
        if (in_byte==msg_frame[2]) {msgSectionIndex++; bodyindex=0;}
        else {msgSectionIndex = 0; bodyindex=0;}
        break;
    case 3:                                 // looking for 4th frame byte
        if (in_byte==msg_frame[3]) {msgSectionIndex++; bodyindex=0;}
        else {msgSectionIndex = 0; bodyindex=0;}
        break;
    case 4:                                 // pulling out length of data
        num_body_bytes = in_byte;
        msgSectionIndex++;
        if(num_body_bytes>255){msgSectionIndex = 0; bodyindex=0;} // never true with num_body_bytes as uint8
        break;
    case 5:                                 // pulling out body
        object_rx_buffer_[bodyindex] = in_byte;
        bodyindex++;
        if(bodyindex >= num_body_bytes) {msgSectionIndex++;}
        break;
    case 6:                                 // decide what to do with the data
        if (object_rx_buffer_[0] == 'U') {sensor.unlock();}
        else if (object_rx_buffer_[0] == ('A' || 'B' || 'C' || 'D' || 'E' || 'F'))
        {
            std::string data_str;
            std::string::size_type sz;

            for (int i=1, i<=bodyindex, ++i)
            {
                data_str += object_rx_buffer_[i];
            }
            float data_flt = std::stof(data_str, &sz)

            if (object_rx_buffer_[0] == 'A') {sensor.setRotDir((int)data_flt);}
            else if (object_rx_buffer_[0] == 'B') {sensor.setZeroPosOffset(data_flt);}
            else if (object_rx_buffer_[0] == 'C') {sensor.setHysteresis(data_flt);}
            else if (object_rx_buffer_[0] == 'D') {sensor.burnRotDir((int)data_flt);}
            else if (object_rx_buffer_[0] == 'E') {sensor.burnZeroPosOffset(data_flt);}
            else if (object_rx_buffer_[0] == 'F') {sensor.burnHysteresis(data_flt);}
        }
        bodyindex=0;
        msgSectionIndex=0;
        break;
    default:                                 // safety resest
        {msgSectionIndex = -1; bodyindex=0;}
    }
}
