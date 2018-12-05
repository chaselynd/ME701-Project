#ifndef A1339_H_INCLUDED
#define A1339_H_INCLUDED

// Includes
#include "stm32f4xx.h"
#include "spi1.h"                 // from the gimbal_board project
#include "system_timer.h"         // from the eeva project

class A1339
{
public:
    
    // Constructor.
    A1339(void);

    // Initialize the encoder settings.
    void initialize();

    // Set the encoder zero position to wherever the encoder is currently positioned.
    void setZeroPos();

    void setRotDir();

    void setHysterisis();

    // Return the 15-bit encoder angle (radians).
    double getAngle();

    double getJuncTemp();

    double getFieldStrength();

private:

    // Return the value stored in the specified register address.
    uint16_t readRegister(uint8_t address);

    // Set the register found at address to the specified value.
    void setRegister(uint8_t address, uint9_t value);

    // Update the register at specified address by clearing the clearbits and setting the setbits.
    void modifyRegister(uint8_t address,  uint8_t clearbits, uint8_t setbits)

    // SPI bus information.
    spi1 *spi_;     // SPI bus object
    //spi_bus_id_t spi_bus_; // SPI bus ID

    
    
}

#endif