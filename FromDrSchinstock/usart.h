#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

// Includes
#include "stm32f4xx.h"
#include "dma_rx.h"
#include "dma_tx.h"

// USART bus select enumeration
typedef uint8_t usart_bus_t;
enum
{
    USART_BUS_1 = 0,
    USART_BUS_2,
    USART_BUS_COUNT
};

// Transfer and receive buffer sizes for each bus
enum
{
    USART1_TX_BUFF_SIZE = 256,
    USART1_RX_BUFF_SIZE = 50,
    USART2_TX_BUFF_SIZE = 1024,
    USART2_RX_BUFF_SIZE = 250, // TODO increase these buffer sizes
};

// Universal [A]synchronous Receiver/Transmitter Driver
// Provide USART implementation using DMA which organizes buses into instances
// which can be referenced by calling the 'instance()' method.
class Usart
{
public: // methods

    // Return the usart object for the given bus. If this is the first time it's been
    // requested then the hardware will be initialized.
    static Usart * instance(usart_bus_t bus);

    // Destructor
    ~Usart(void);

    // Get the next available byte from the usart receive buffer. Returns false if
    // nothing available.  Should be called at high enough rate that the circular
    // buffer is not overwritten by the rx DMA between clearing it.
    bool getByte(uint8_t * byte);

    // Copy 'len' bytes from array at 'data' to the usart send buffer.
    // Return false if not enough room in the buffer.  The buffer is cleared by dma
    // transfers set up in the tx DMA ISR.
    bool sendBuffer(uint8_t const * data, uint16_t len);

    // Return true if there's nothing left in the receive buffer.
    bool empty(void) const { return dma_rx_->empty(); }

    // Interrupt service routines which just delegate off to the DMA handlers.
    static void USART1_TX_ISR(void) { Usart::objs[USART_BUS_1].dma_tx_->handleISR(); }
    static void USART2_TX_ISR(void) { Usart::objs[USART_BUS_2].dma_tx_->handleISR(); }

private: // methods

    Usart() {} // private constructor to limit instances to 'factory' method

    // Set up hardware for different USART buses.
    static void InitBus1(void);
    static void InitBus2(void);

private: // fields

    // Class instances.
    static Usart objs[USART_BUS_COUNT];
    // Flags to check which class instances have already been initialized.
    static bool  init[USART_BUS_COUNT];

    // References to transfer and receive DMA controllers.
    DmaRx * dma_rx_;
    DmaTx * dma_tx_;

    // Bus ID and SPL usart port.
    usart_bus_t bus_;
    USART_TypeDef * USARTx_;

}; // Usart

#endif
