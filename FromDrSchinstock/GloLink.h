// -----------------------------------------------------------------------
// Copyright: Dale Schinstock, 2013
//------------------------------------------------------------------------

#ifndef GloLink_H
#define GloLink_H

#include <stdint.h>
#include "gloObjects.h"
#include "usart.h"

class GloLink
{
  public:
    //constructor
    explicit GloLink();

    //destructor
    virtual ~GloLink();

    //methods
    void checkForObjects(void);
    uint16_t sendObject(uint8_t objID, uint16_t instance);
    
    //properties
    uint32_t numReceived, numSent, numSentFailed;

  private:
    void parseOneByte(uint8_t in_byte);
    void handleParsedObject(void);
      
    Usart *serial_port_;
    uint8_t sendBuff[264]; //size = 255(obj size) +3(start) +1(objID) +2(instance) +1(numBytes) +2(end bytes)

    uint16_t parsed_object_id_;
    uint16_t parsed_object_instance_;
    bool complete_object_parsed_;
    uint8_t object_rx_buffer_[255];

};

extern GloLink gloLink;

#endif  //GloLink

