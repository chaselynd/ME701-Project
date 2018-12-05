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
        if (in_byte==msg_frame[0]) msgSectionIndex++;
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 1:                                 // looking for 2nd frame byte
        if (in_byte==msg_frame[1]) msgSectionIndex++;
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 2:                                 // looking for 3rd frame byte
        if (in_byte==msg_frame[2]) {msgSectionIndex++; bodyindex=0;}
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 3:                                 // looking for 4th frame byte
        if (in_byte==msg_frame[3]) {msgSectionIndex++; bodyindex=0;}
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 4:                                 // pulling out id
        objID = in_byte;
        msgSectionIndex++;
        break;
    case 5:                                 // pulling out length of data
        num_body_bytes = in_byte;
        msgSectionIndex++;
        if(num_body_bytes>255){msgSectionIndex = -1; bodyindex=0;} // never true with num_body_bytes as uint8
        break;
    case 6:                                // pulling out body
        object_rx_buffer_[bodyindex] = in_byte;
        bodyindex++;
        if(bodyindex >= num_body_bytes) msgSectionIndex++;
        break;
    case 7:                                // looking for CR (0x0D) -- should add CRC check for serial
        if (in_byte==0x0D) msgSectionIndex++;
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 8:                                // looking for LF (0x0A) 
        if (in_byte==0x0A)
        {
            msgSectionIndex = -1;
            parsed_object_id_ = objID;
            complete_object_parsed_ = 1;
        }
        msgSectionIndex = -1; bodyindex=0;
        break;
    default:                                 // safety resest
        {msgSectionIndex = -1; bodyindex=0;}
    }
}
