#include "GloLink.h"
#include <string.h>
#include "gloObjects.h"
#include "GloOS.h"

// Constructor
GloLink::GloLink():
    complete_object_parsed_(false)
{
    serial_port_ = Usart::instance(USART_BUS_1);
}
// Destructor
GloLink::~GloLink()
{
}

// This should be called by sysTimer thread which limits one recieved object
// per sysTimer. In this case all recieved callbacks should be fast.
// Or it should be placed in main() -- i.e. the idle thread
void GloLink::checkForObjects(void)  
{                                       
    uint8_t receive_byte;
    
    while ( serial_port_->getByte(&receive_byte) )
    {
        parseOneByte(receive_byte);
        
        if (complete_object_parsed_) 
        {
            handleParsedObject();
        }
    }
}

// this should be called by a thread handling the gloOS.telemQueue
uint16_t GloLink::sendObject(uint8_t objID, uint16_t instance)
{
    uint32_t instancePtr;
    uint64_t sys_tick_at_start;
    
    instancePtr = (uint32_t)gloObjectPtrs[objID]->md.dataPtr +
        (uint32_t)(instance-1)*gloObjectPtrs[objID]->md.numBytes;
    
    sendBuff[0]='<'; sendBuff[1]='{'; sendBuff[2]='(';
    sendBuff[3] = objID;
    sendBuff[4] = (uint8_t)((instance<<8)>>8);
    sendBuff[5] = (uint8_t)(instance>>8);
    sendBuff[6] = gloObjectPtrs[objID]->md.numBytes;
    sendBuff[gloObjectPtrs[objID]->md.numBytes + 7] = 0x0D;  //TODO CRC if serial
    sendBuff[gloObjectPtrs[objID]->md.numBytes + 8] = 0x0A;  //CR LF
    
    __disable_irq();  // disable all interrupts
    memcpy((void *)&(sendBuff[7]), (void const *)instancePtr, gloObjectPtrs[objID]->md.numBytes);
    __enable_irq(); //  enable interrupts
    
    sys_tick_at_start = gloOS.sysTimer.ticks;
    while(true)                          // keep trying to to send until timeout
    {
        if (serial_port_->sendBuffer(sendBuff, (uint32_t)(gloObjectPtrs[objID]->md.numBytes + 9)))
        {
            numSent++;
            return(1);
        }
        else
        {
            if ((float)(gloOS.sysTimer.ticks-sys_tick_at_start)/gloOS.sysTimer.freq >= 0.1)
            {
                numSentFailed++;
                return(0);              // timeout after 100 ms
            }
        }
    }
}

// *************************************************************
// *************************************************************
void GloLink::parseOneByte(uint8_t in_byte)
{
    const uint8_t msg_frame[3] = {'<', '{', '('};
    static int8_t msgSectionIndex = -1;
    static uint8_t objID = 0, num_body_bytes =0, bodyindex=0;
    static uint16_t instance;
    
    switch(msgSectionIndex) 
    {
    case -1:                                // looking for first frame byte
        if (in_byte==msg_frame[0]) msgSectionIndex++;
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 0:                                 // looking for 2nd frame byte
        if (in_byte==msg_frame[1]) msgSectionIndex++;
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 1:                                 // looking for 3rd frame byte
        if (in_byte==msg_frame[2]) {msgSectionIndex++; bodyindex=0;}
        else {msgSectionIndex = -1; bodyindex=0;}
        break;
    case 2:                                 // pulling out id
        objID = in_byte;
        msgSectionIndex++;
        break;
    case 3:                                 // pulling out byte one of instance
        instance = in_byte;
        msgSectionIndex++;
        break;
    case 4:                                 // pulling out byte two of instance
        instance += (uint16_t)(in_byte)<<8;
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
            parsed_object_instance_ = instance;
            complete_object_parsed_ = 1;
        }
        msgSectionIndex = -1; bodyindex=0;
        break;
    default:                                 // safety resest
        {msgSectionIndex = -1; bodyindex=0;}
    }
}

// *************************************************************
// *************************************************************
void GloLink::handleParsedObject(void)
{
    uint8_t objID=0;
    uint16_t instance=0;
    
    objID = parsed_object_id_;
    instance = parsed_object_instance_;
    if(objID == 0)                          // an object is requested
    {
        uint8_t reqObjID = object_rx_buffer_[0];
        gloOS.telemQueue.put(reqObjID,instance);
    }
    else if (objID == 255)
    {
        // TODO Ack/Nack
    }
    else   // an object has been sent
    {
        if ((objID <= NUMGLOTYPES)&&(instance <= gloObjectPtrs[objID]->md.numInstance)&&(instance>0))
        {
            uint32_t instancePtr;   
            // Do the callback if present
            if (gloObjectPtrs[objID]->md.onReceiveFuncPtr != 0)    
                gloObjectPtrs[objID]->md.onReceiveFuncPtr(instance,(void *)object_rx_buffer_);
            // else copy buffer to object
            else 
            {
                instancePtr = (uint32_t)gloObjectPtrs[objID]->md.dataPtr + (uint32_t)(instance-1)*gloObjectPtrs[objID]->md.numBytes;
                __disable_irq();  // disable all interrupts
                memcpy( (void *)instancePtr, (void const *)object_rx_buffer_, gloObjectPtrs[objID]->md.numBytes);
                __enable_irq(); //  enable interrupts
            }
            numReceived++;
            
            // NOW ACKNOWLEDGE BY SENDING THE OBJECT BACK - COSTLY, BUT SIMPLE
            gloOS.telemQueue.put(objID,instance);
        }
    }
    complete_object_parsed_= false;    // Do this last to prevent buffer from being overwritten during processing
}
