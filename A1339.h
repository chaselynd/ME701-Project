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

    // Unlock the A1339 for access to EEPROM and Shadow Memory
    void unlock();

    // Return the encoder angle (radians).
    double getAngle();

    
    //---------------------------------- Shadow Memory ----------------------------------
    // Write which rotation direction (CW or CCW) is positive to Shadow Memory
    // CW: dir=0
    // CCW: dir=1
    void setRotDir(int dir);

    // Write the zero position offset to Shadow Memory
    void setZeroPosOffset(double rad);

    // Write the angle hysteresis threshold to Shadow Memory
    void setHysterisis(double deg);

    //---------------------------------- EEPROM ----------------------------------
    // Burn which rotation direction (CW or CCW) is positive to EEPROM
    // CW: dir=0
    // CCW: dir=1
    void burnRotDir(int dir);

    // Burn the zero position offset to EEPROM
    void burnZeroPosOffset(double rad);

    // Burn the angle hysteresis threshold to EEPROM
    void burnHysterisis(double deg);


private:
    //---------------------------------- 8-bit functions ----------------------------------
    // Return the value stored in the specified register address.
    uint8_t readRegister(uint8_t address);

    // Set the register found at address to the specified value.
    void setRegister(uint8_t address, uint8_t value);

    // Update the register at specified address by clearing the clearbits and setting the setbits.
    void modifyRegister(uint8_t address,  uint8_t clearbits, uint8_t setbits)

    //---------------------------------- 16-bit functions ----------------------------------
    // Return the value stored in the specified register address.
    uint16_t readRegister16(uint16_t address);

    // Set the register found at address to the specified value.
    void setRegister16(uint16_t address, uint16_t value);

    // Update the register at specified address by clearing the clearbits and setting the setbits.
    void modifyRegister16(uint16_t address,  uint16_t clearbits, uint16_t setbits)


    // SPI bus information.
    spi1 *spi_;     // SPI bus object
    //spi_bus_id_t spi_bus_; // SPI bus ID
}

#endif