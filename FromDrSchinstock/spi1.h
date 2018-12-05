#ifndef SPI1_H_INCLUDED
#define SPI1_H_INCLUDED

#include <cstdint>
#include "stm32f4xx_spi.h"

// Constants
#define DEFAULT_SPI_FREQUENCY (10000000) // Default frequency for SPI bus


class Spi1
{
public:

    // Constructor
    Spi1(void);

    // Sets the SPI bus frequency [Hz]
    void setFrequency(uint32_t frequency);

    // Sends a word
    // Returns zero if fails
    uint8_t sendWord(uint16_t word_to_send);

    // Reads a word from the data register if available
    // returns zero if fails
    uint8_t getWord(uint16_t *word_returned);

    // reset CS pin to select chip
    void selectChip(void);

    // set CS pin to deselect chip
    void deselectChip(void);

private:


};

#endif
