// Includes
#include "A1339.h"


// Primary Serial Interface Registers
#define NOP        (0x00) // Null Register
#define EWA        (0x02) // Extended Write Address
#define EWDH       (0x04) // Extended Write Data High
#define EWDL       (0x06) // Extended Write Data Low
#define EWCS       (0x08) // Extended Write Control and Status
#define ERA        (0x0A) // Extended Read Adress
#define ERCS       (0x0C) // Extended Read Control and Status
#define ERDH       (0x0E) // Extended Read Data High
#define ERDL       (0x10) // Extended Read Data High

#define CTRL       (0x1E) // Device Control
#define ANG        (0x20) // Current Angle Reading (12 bits)
#define STA        (0x22) // Device Status
#define ERR        (0x24) // Device Error Flags
#define WARN       (0x26) // Device Warning Flags
#define TSEN       (0x28) // Temperature Sensor
#define FIELD      (0x2A) // Field Strength (in gauss)
#define TURNS      (0x2C) // Turns Counter

#define HANG       (0x30) // Hysteresis Angle Value (12 bits)
#define ANG15      (0x32) // Current Angle Reading (15 bits)
#define ZANG       (0x34) // ZCD Angle (low power signal path)

#define IKEY       (0x3C) // Key Register


// EEPROM and Shadow Memory Registers
// NAME_E is the EEPROM address
// NAME_S is the Shadow Memory address
#define PWE_E      (0x18) // PWM Error Enable
#define PWE_S      (0x58)
#define ABI_E      (0x19) // ABI Control
#define ABI_S      (0x59)
#define MSK_E      (0x1A) // Mask Bits
#define MSK_S      (0x5A)
#define PWI_E      (0x1B) // PWM Interface Control
#define PWI_S      (0x5B)
#define ANG_E      (0x1C) // Angle Measurement
#define ANG_S      (0x5C)
#define LPC_E      (0x1D) // Low Power Mode
#define LPC_S      (0x5D)
#define COM_E      (0x1E) // COM (Communication? or Commutation?)
#define COM_S      (0x5E)
#define CUS_E      (0x1F) // Customer EEPROM Space


// Constants below are used in the readRegister and setRegister funcitons
#define CMD_READ        (0x80)
#define CMD_WRITE       (0x00)
#define ADDR_MASK       (0x7F)
#define CMD_READ_16     (0x8000)
#define CMD_WRITE_16    (0x0000)
#define ADDR_MASK_16    (0x7FFF)
#define CMD_READ_32     (0x80000000)
#define CMD_WRITE_32    (0x00000000)
#define ADDR_MASK_32    (0x7FFFFFFF)


A1339::A1339() :
    spi_(NULL)
{
}

void A1339::unlock()
{
    // the follow sequence of writes unlocks EEPROM and Shadow Memory
    setRegister(0x3C, 0x00);
    setRegister(0x3C, 0x27);
    setRegister(0x3C, 0x81);
    setRegister(0x3C, 0x1F);
    setRegister(0x3C, 0x77);
}

float A1339::getAngle()
{
    uint16_t value = readRegister(ANG);
    float angle = (value & 0x0FFF)*(360/4096);

    return angle;
}

void A1339::setRotDir(int dir)
{
    if (dir <= 0) // angle increases in CW direction
    {
        modifyRegister32(ANG_S, 0x00040000, 0x00000000);
    }
    else
    {
        modifyRegister32(ANG_S, 0x00000000, 0x00040000);
    }
}

void A1339::setZeroPosOffset(float deg)
{
    uint32_t conv = (deg*(4096/360)) & 0x00000FFF;
    modifyRegister32(ANG_S, ~conv, conv);
}

void A1339::setHysterisis(float deg)
{
    uint32_t conv = (deg*(4096/360)) & 0x0003F000;
    modifyRegister32(ANG_S, ~conv, conv);
}

void A1339::burnRotDir(int dir)
{
    if (dir <= 0) // angle increases in CW direction
    {
        modifyRegister32(ANG_E, 0x00040000, 0x00000000);
    }
    else
    {
        modifyRegister32(ANG_E, 0x00000000, 0x00040000);
    }
}

void A1339::burnZeroPosOffset(float deg)
{
    uint32_t conv = (deg*(4096/360)) & 0x00000FFF;
    modifyRegister32(ANG_E, ~conv, conv);
}

void A1339::burnHysterisis(float deg)
{
    uint32_t conv = (deg*(4096/360)) & 0x0003F000;
    modifyRegister32(ANG_E, ~conv, conv);
}

uint8_t A1339::readRegister(uint8_t address)
{
    address &= ADDR_MASK;

    selectChip();

    spi_->sendWord(CMD_READ | address);
    uint8_t value = spi_->sendWord(0);

    deselectChip();

    return value;
}

void A1339::setRegister(uint8_t address, uint8_t value)
{
    address &= ADDR_MASK;

    selectChip();
    usleep(1);
    spi_->sendWord(CMD_WRITE | address);
    spi_->sendWord(value);
    usleep(1);
    deselectChip();
}

void A1339::modifyRegister(uint8_t address,  uint8_t clearbits, uint8_t setbits)
{
    uint8_t value = readRegister(address);
    value &= ~clearbits;
    value |=  setbits;
    setRegister(address, value);
}

uint16_t A1339::readRegister16(uint8_t address)
{
    address &= ADDR_MASK_16;

    selectChip();

    spi_->sendWord(CMD_READ_16 | address);
    uint16_t value = spi_->sendWord(0);

    deselectChip();

    return value;
}

void A1339::setRegister16(uint8_t address, uint16_t value)
{
    address &= ADDR_MASK_16;

    selectChip();
    usleep(1);
    spi_->sendWord(CMD_WRITE_16 | address);
    spi_->sendWord(value);
    usleep(1);
    deselectChip();
}

void A1339::modifyRegister16(uint8_t address,  uint16_t clearbits, uint16_t setbits)
{
    uint16_t value = readRegister(address);
    value &= ~clearbits;
    value |=  setbits;
    setRegister(address, value);
}

uint32_t A1339::readRegister32(uint8_t address)
{
    address &= ADDR_MASK_32;

    selectChip();

    spi_->sendWord(CMD_READ_32 | address);
    uint32_t value = spi_->sendWord(0);

    deselectChip();

    return value;
}

void A1339::setRegister32(uint8_t address, uint32_t value)
{
    address &= ADDR_MASK_32;

    selectChip();
    usleep(1);
    spi_->sendWord(CMD_WRITE_32 | address);
    spi_->sendWord(value);
    usleep(1);
    deselectChip();
}

void A1339::modifyRegister32(uint8_t address,  uint32_t clearbits, uint32_t setbits)
{
    uint32_t value = readRegister(address);
    value &= ~clearbits;
    value |=  setbits;
    setRegister(address, value);
}
