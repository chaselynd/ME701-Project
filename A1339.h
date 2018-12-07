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

    // Return the encoder angle (degrees).
    float getAngle();

    
    //---------------------------------- Shadow Memory ----------------------------------
    // Write which rotation direction (CW or CCW) is positive to Shadow Memory
    // CW: dir=0
    // CCW: dir=1
    void setRotDir(int dir);

    // Write the zero position offset to Shadow Memory
    void setZeroPosOffset(float deg);

    // Write the angle hysteresis threshold to Shadow Memory
    // Binary scaling is used
    void setHysteresis(float deg);

    //---------------------------------- EEPROM ----------------------------------
    // Burn which rotation direction (CW or CCW) is positive to EEPROM
    // CW: dir=0
    // CCW: dir=1
    void burnRotDir(int dir);

    // Burn the zero position offset to EEPROM
    void burnZeroPosOffset(float deg);

    // Burn the angle hysteresis threshold to EEPROM
    // Binary scaling is used
    void burnHysterisis(float deg);


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
    uint16_t readRegister16(uint8_t address);

    // Set the register found at address to the specified value.
    void setRegister16(uint8_t address, uint16_t value);

    // Update the register at specified address by clearing the clearbits and setting the setbits.
    void modifyRegister16(uint8_t address,  uint16_t clearbits, uint16_t setbits)

    //---------------------------------- 32-bit functions ----------------------------------
    // Return the value stored in the specified register address.
    uint32_t readRegister32(uint8_t address);

    // Set the register found at address to the specified value.
    void setRegister32(uint8_t address, uint32_t value);

    // Update the register at specified address by clearing the clearbits and setting the setbits.
    void modifyRegister32(uint8_t address,  uint32_t clearbits, uint32_t setbits)


    spi1 *spi_;     // SPI bus object
}

#endif