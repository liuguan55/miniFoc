
#ifndef __AP_HAL_STM32_UARTDRIVER_H__
#define __AP_HAL_STM32_UARTDRIVER_H__

#include "AP_HAL_STM32.h"
#include "driver/usart.h"


class STM32::STM32UARTDriver : public AP_HAL::UARTDriver {
public:
    enum {
        HAL_STM32_UART1,
        HAL_STM32_UART2,
        HAL_STM32_UART3,
        HAL_STM32_UART4,
        HAL_STM32_UART5,
        HAL_STM32_UART_NR,
    };

    explicit STM32UARTDriver(int32_t uart_num);
    /* STM32 implementations of UARTDriver virtual methods */
    void begin(uint32_t b) override;
    void begin(uint32_t b, uint16_t rxS, uint16_t txS) override;
    void end() override;
    void flush() override;
    bool is_initialized() override;
    void set_blocking_writes(bool blocking) override;
    bool tx_pending() override;

    /* STM32 implementations of Stream virtual methods */
    int16_t available() override;
    int16_t txspace() override;
    int16_t read() override;

    void _timer_tick();


    /* STM32 implementations of Print virtual methods */
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    void set_flow_control(enum flow_control flow_control) override ;
    enum flow_control get_flow_control() override { return _flow_control; }

private:
    UART_HandleTypeDef *_huart;
    lwrb_t *_lwrb;
    int32_t _uart_num;
    uint32_t _baudrate;
    volatile bool _initialised;
    volatile bool _in_timer;

    bool _nonblocking_writes;

    // we use in-task ring buffers to reduce the system call cost
    // of ::read() and ::write() in the main loop
    uint8_t *_readbuf;
    uint16_t _readbuf_size;

    // _head is where the next available data is. _tail is where new
    // data is put
    volatile uint16_t _readbuf_head;
    volatile uint16_t _readbuf_tail;

    uint8_t *_writebuf;
    uint16_t _writebuf_size;
    volatile uint16_t _writebuf_head;
    volatile uint16_t _writebuf_tail;

    int _write(const uint8_t *buf, uint16_t n);
    int _read(uint8_t *buf, uint16_t n);
    uint64_t _last_write_time;

    void try_initialise(void);
    uint32_t _last_initialise_attempt_ms;

    uint32_t _os_write_buffer_size;
    uint32_t _total_read;
    uint32_t _total_written;
    enum flow_control _flow_control;
};

#endif // __AP_HAL_STM32_UARTDRIVER_H__
