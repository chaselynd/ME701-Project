// Includes
#include <cstdint>
#include <cstdio>
#include "spi1.h"

#define SPI_MAX_ATTEMPTS (100) // Timeout count until bus timeout

#define SCLK_GPIO_PORT (GPIOA               )
#define SCLK_GPIO_CLK  (RCC_AHB1Periph_GPIOA)
#define SCLK_GPIO_PIN  (GPIO_Pin_5          )
#define SCLK_AF_PIN    (GPIO_PinSource5     )
#define SCLK_AF_SELECT (GPIO_AF_SPI1        )

#define MISO_GPIO_PORT (GPIOA               )
#define MISO_GPIO_CLK  (RCC_AHB1Periph_GPIOA)
#define MISO_GPIO_PIN  (GPIO_Pin_6          )
#define MISO_AF_PIN    (GPIO_PinSource6     )
#define MISO_AF_SELECT (GPIO_AF_SPI1        )

#define MOSI_GPIO_PORT (GPIOA               )
#define MOSI_GPIO_CLK  (RCC_AHB1Periph_GPIOA)
#define MOSI_GPIO_PIN  (GPIO_Pin_7          )
#define MOSI_AF_PIN    (GPIO_PinSource7     )
#define MOSI_AF_SELECT (GPIO_AF_SPI1        )

#define CS_GPIO_PORT (GPIOB               )
#define CS_GPIO_CLK  (RCC_AHB1Periph_GPIOB)
#define CS_GPIO_PIN  (GPIO_Pin_12         )

#define SPI_PERIPH_CLK (RCC_APB2Periph_SPI1       )
#define SPI_BASE_ADDR  (SPI1                      )
#define SPI_APB_FREQ   (RCC_Clocks.PCLK2_Frequency)

//*****************************************************************************
Spi1::Spi1(void)
{
    // Enable Required clocks for SPI
    RCC_APB2PeriphClockCmd(SPI_PERIPH_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(SCLK_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(MISO_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(MOSI_GPIO_CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(CS_GPIO_CLK, ENABLE);
    
    // Chip select initialize GPIO chip select
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(CS_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = CS_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(CS_GPIO_PORT, &GPIO_InitStructure);
    CS_GPIO_PORT->BSRRL = CS_GPIO_PIN;
    
    // Set GPIO Alternate Function for MOSI, MISO, SCLK
    GPIO_PinAFConfig(SCLK_GPIO_PORT, SCLK_AF_PIN, SCLK_AF_SELECT);
    GPIO_PinAFConfig(MOSI_GPIO_PORT, MOSI_AF_PIN, MOSI_AF_SELECT);
    GPIO_PinAFConfig(MISO_GPIO_PORT, MISO_AF_PIN, MISO_AF_SELECT);
    
    // Configure GPIO for MOSI, MISO, SCLK
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    
    // SPI SCK pin configuration
    GPIO_InitStructure.GPIO_Pin = SCLK_GPIO_PIN;
    GPIO_Init(SCLK_GPIO_PORT, &GPIO_InitStructure);
    
    // SPI MOSI pin configuration
    GPIO_InitStructure.GPIO_Pin =  MOSI_GPIO_PIN;
    GPIO_Init(MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    // SPI MISO pin configuration
    GPIO_InitStructure.GPIO_Pin =  MISO_GPIO_PIN;
    GPIO_Init(MISO_GPIO_PORT, &GPIO_InitStructure);
    
    // Configure SPI port
    SPI_I2S_DeInit(SPI_BASE_ADDR);
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // set below by setFrequency
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_Init(SPI_BASE_ADDR, &SPI_InitStructure);
    
    setFrequency(DEFAULT_SPI_FREQUENCY);
    
    SPI_Cmd(SPI_BASE_ADDR, ENABLE);
}

//*****************************************************************************
void Spi1::setFrequency(uint32_t frequency)
{
    // Obtain PCLK
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    uint32_t pclk = SPI_APB_FREQ;

    // Get current CR1 settings
    uint16_t reg = SPI_BASE_ADDR->CR1 & ~(0x0038);

    // Find closest frequency that does not exceed the given frequency
    if (frequency >= (pclk >> 1))
    {
        reg |= (0x00 << 3);
    }
    else if (frequency >= (pclk >> 2))
    {
        reg |= (0x01 << 3);
    }
    else if (frequency >= (pclk >> 3))
    {
        reg |= (0x02 << 3);
    }
    else if (frequency >= (pclk >> 4))
    {
        reg |= (0x03 << 3);
    }
    else if (frequency >= (pclk >> 5))
    {
        reg |= (0x04 << 3);
    }
    else if (frequency >= (pclk >> 6))
    {
        reg |= (0x05 << 3);
    }
    else if (frequency >= (pclk >> 7))
    {
        reg |= (0x06 << 3);
    }
    else if (frequency >= (pclk >> 8))
    {
        reg |= (0x07 << 3);
    }
    else
    {
        // Invalid frequency attempted to be set;
        return;
    }

    SPI_BASE_ADDR->CR1 = reg;
}

//*****************************************************************************
uint8_t Spi1::sendWord(uint16_t word_to_send)
{
    // Loop while DR register in not empty
    uint32_t attempts = 0;
    while (SPI_I2S_GetFlagStatus(SPI_BASE_ADDR, SPI_I2S_FLAG_TXE) == RESET)
    {
        attempts++;
        if (attempts >= SPI_MAX_ATTEMPTS)
        {
            // TODO: Reset SPI, possibly not just return 0
            return 0;
        }
    }

    // Send word through the SPI peripheral
    SPI_I2S_SendData(SPI_BASE_ADDR, word_to_send);
    
    return 1;
}

//*****************************************************************************
uint8_t Spi1::getWord(uint16_t *word_returned)
{    
    // Wait to receive a byte
    uint32_t attempts = 0;
    while (SPI_I2S_GetFlagStatus(SPI_BASE_ADDR, SPI_I2S_FLAG_RXNE) == RESET)
    {
        attempts++;
        if (attempts >= SPI_MAX_ATTEMPTS)
        {
            // TODO: Reset SPI, possibly not just return 0
            return 0;
        }
    }

    // Return the word read from the SPI bus
    *word_returned = (SPI_I2S_ReceiveData(SPI_BASE_ADDR) & 0xFFFF);
    return 1;
}

//*****************************************************************************
void Spi1::selectChip(void)
{
    CS_GPIO_PORT->BSRRH = CS_GPIO_PIN;
}
    
//*****************************************************************************
void Spi1::deselectChip(void)
{
    CS_GPIO_PORT->BSRRL = CS_GPIO_PIN;
}
