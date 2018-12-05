// Includes
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "dma_rx.h"
#include "dma_tx.h"
#include "usart.h"

// Static class fields
bool  Usart::init[USART_BUS_COUNT];
Usart Usart::objs[USART_BUS_COUNT];

//*****************************************************************************
Usart * Usart::instance(usart_bus_t bus)
{
    if (init[bus])
    {
        return &objs[bus];
    }

    switch (bus)
    {
        case USART_BUS_1:
            InitBus1();
            objs[bus].bus_ = bus;
            objs[bus].USARTx_ = USART1;
            init[bus] = true;
            return &objs[bus];
            break;

        case USART_BUS_2:
            InitBus2();
            objs[bus].bus_ = bus;
            objs[bus].USARTx_ = USART2;
            init[bus] = true;
            return &objs[bus];
            break;
    }

    return NULL;
}

//*****************************************************************************
Usart::~Usart(void)
{
    if (dma_tx_ != NULL)
    {
        delete dma_tx_;
    }

    if (dma_rx_ != NULL)
    {
        delete dma_rx_;
    }
}

//*****************************************************************************
bool Usart::getByte(uint8_t * byte)
{
    return dma_rx_->getByte(byte);
}

//*****************************************************************************
bool Usart::sendBuffer(uint8_t const * data, uint16_t len)
{
    return dma_tx_->sendBuffer(data, len);
}

//*****************************************************************************
void Usart::InitBus1(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // Enable GPIO clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // Enable USART clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // Initialize DMA Rx
    Usart * usart = &objs[USART_BUS_1];
    usart->dma_rx_ = new DmaRx(DMA2_Stream2,
                             DMA_Channel_4,
                             (uint32_t)&USART1->DR,
                             USART1_RX_BUFF_SIZE);
    usart->dma_tx_ = new DmaTx(DMA2_Stream7,
                             DMA_Channel_4,
                             DMA2_Stream7_IRQn,
                             (uint32_t)&USART1->DR,
                             DMA_IT_TCIF7,
                             DMA_IT_TEIF7,
                             USART1_TX_BUFF_SIZE);

    if (usart->dma_rx_ == NULL) while(true);  // catch error here
    if (usart->dma_tx_ == NULL) while(true);  // catch error here

    USART_InitStructure.USART_BaudRate = 921600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // Connect Rx to USART.
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    // Connect Tx to USART.
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    // Connect CTS to USART.
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART1);

    // Configure USART Tx/Rx as alternate function.
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // Configure USART CTS as alternate function.
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    USART_Cmd(USART1, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // lower is higher priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; // subpriority not used
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//*****************************************************************************
void Usart::InitBus2(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // Enable GPIO clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    // Enable USART clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // Initialize DMA Rx and Tx
    Usart * usart = &objs[USART_BUS_2];
    usart->dma_rx_ = new DmaRx(DMA1_Stream5,
                             DMA_Channel_4,
                             (uint32_t)&USART2->DR,
                             USART2_RX_BUFF_SIZE);
    usart->dma_tx_ = new DmaTx(DMA1_Stream6,
                             DMA_Channel_4,
                             DMA1_Stream6_IRQn,
                             (uint32_t)&USART2->DR,
                             DMA_IT_TCIF6,
                             DMA_IT_TEIF6,
                             USART2_TX_BUFF_SIZE);

    if (usart->dma_rx_ == NULL) while(true);  // catch error here
    if (usart->dma_tx_ == NULL) while(true);  // catch error here

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    // Connect Rx to USART.
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
    // Connect Tx to USART.
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    // Connect CTS to USART.
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_USART2);

    // Configure USART Tx/Rx/CTS as alternate function.
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

    USART_Cmd(USART2, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // lower is higher priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; // subpriority not used
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//*****************************************************************************
extern "C" void DMA2_Stream7_IRQHandler(void)
{
    Usart::USART1_TX_ISR();
}

//*****************************************************************************
extern "C" void DMA1_Stream6_IRQHandler(void)
{
    Usart::USART2_TX_ISR();
}

