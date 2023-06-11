
#include <stdio.h>
#include "stdlib.h"
#include "UARTDriver.h"
#include "AP_HAL/AP_HAL_Board.h"
#include "sys/MiniDebug.h"

using namespace STM32;

static HAL_UART_ID uartHandles[STM32UARTDriver::HAL_STM32_UART_NR] = {
        HAL_UART_1,
        HAL_UART_2,
        HAL_UART_3,
        HAL_UART_4,
        HAL_UART_5,
};
STM32UARTDriver::STM32UARTDriver(int32_t uart_num) :
        _uart_num(uart_num),
        _baudrate(57600),
        _initialised(false),
        _in_timer(false),
        _flow_control(FLOW_CONTROL_DISABLE)
{
}

void STM32UARTDriver::begin(uint32_t b) {
    begin(b, 0, 0);
}
void STM32UARTDriver::begin(uint32_t b, uint16_t rxS, uint16_t txS) {
    uint16_t min_tx_buffer = 1024;
    uint16_t min_rx_buffer = 512;

    // receive buffer for all ports. This means we don't get delays
    // while waiting to write GPS config packets
    if (txS < min_tx_buffer) {
        txS = min_tx_buffer;
    }
    if (rxS < min_rx_buffer) {
        rxS = min_rx_buffer;
    }

    /*
      allocate the read buffer
      we allocate buffers before we successfully open the device as we
      want to allocate in the early stages of boot, and cause minimum
      thrashing of the heap once we are up. The ttyACM0 driver may not
      connect for some time after boot
     */
    if (rxS != 0 && rxS != _readbuf_size) {
        _initialised = false;
        while (_in_timer) {
            hal.scheduler->delay(1);
        }
        _readbuf_size = rxS;
        if (_readbuf != NULL) {
            free(_readbuf);
        }
        _readbuf = (uint8_t *)malloc(_readbuf_size);
        _readbuf_head = 0;
        _readbuf_tail = 0;
    }

    if (b != 0) {
        _baudrate = b;
    }

    /*
      allocate the write buffer
     */
    if (txS != 0 && txS != _writebuf_size) {
        _initialised = false;
        while (_in_timer) {
            hal.scheduler->delay(1);
        }
        _writebuf_size = txS;
        if (_writebuf != nullptr) {
            free(_writebuf);
        }
        _writebuf = (uint8_t *)malloc(_writebuf_size+16);
        _writebuf_head = 0;
        _writebuf_tail = 0;
    }

    HAL_UartCfg_t cfg = {
        .baudrate = static_cast<HAL_UART_BAUDRATE>(_baudrate),
        .databits = HAL_UART_DATABITS_8,
        .stopbits = HAL_UART_STOPBITS_1,
        .parity = HAL_UART_PARITY_NONE,
    };
    HAL_UartInit(uartHandles[_uart_num], &cfg);

    if (_writebuf_size != 0 && _readbuf_size != 0) {
        if (!_initialised) {
            ::printf("initialised com%ld OK %u %u\n", _uart_num,
                     (unsigned)_writebuf_size, (unsigned)_readbuf_size);
        }
        _initialised = true;
    }

}
void STM32UARTDriver::end() {
    _initialised = false;
    while (_in_timer) hal.scheduler->delay(1);

    HAL_UartDeInit(uartHandles[_uart_num]);

    if (_readbuf) {
        free(_readbuf);
        _readbuf = NULL;
    }
    if (_writebuf) {
        free(_writebuf);
        _writebuf = NULL;
    }
    _readbuf_size = _writebuf_size = 0;
    _writebuf_head = 0;
    _writebuf_tail = 0;
    _readbuf_head = 0;
    _readbuf_tail = 0;
}
void STM32UARTDriver::flush() {

}
bool STM32UARTDriver::is_initialized() {
    try_initialise();

    return _initialised;
}
void STM32UARTDriver::set_blocking_writes(bool blocking) {
    _nonblocking_writes = !blocking;
}
bool STM32UARTDriver::tx_pending() {
    return false;
}

/*
  buffer handling macros
 */
#define BUF_AVAILABLE(buf) ((buf##_head > (_tail=buf##_tail))? (buf##_size - buf##_head) + _tail: _tail - buf##_head)
#define BUF_SPACE(buf) (((_head=buf##_head) > buf##_tail)?(_head - buf##_tail) - 1:((buf##_size - buf##_tail) + _head) - 1)
#define BUF_EMPTY(buf) (buf##_head == buf##_tail)
#define BUF_ADVANCETAIL(buf, n) buf##_tail = (buf##_tail + n) % buf##_size
#define BUF_ADVANCEHEAD(buf, n) buf##_head = (buf##_head + n) % buf##_size

/* STM32 implementations of Stream virtual methods */
int16_t STM32UARTDriver::available() {
    if (!_initialised) {
        try_initialise();
        return 0;
    }

    uint16_t _tail;
    return BUF_AVAILABLE(_readbuf);
}
int16_t STM32UARTDriver::txspace() {
    if (!_initialised) {
        try_initialise();
        return 0;
    }

    uint16_t _head;
    return BUF_SPACE(_writebuf);
}
int16_t STM32UARTDriver::read() {
    uint8_t c;

    if (!_initialised) {
        try_initialise();
        return -1;
    }

    if (_readbuf == NULL) {
        return -1;
    }

    if (BUF_EMPTY(_readbuf)) {
        return -1;
    }

    c = _readbuf[_readbuf_head];
    BUF_ADVANCEHEAD(_readbuf, 1);
    return c;
}

/* STM32 implementations of Print virtual methods */
size_t STM32UARTDriver::write(uint8_t c) {
    if (!_initialised) {
        try_initialise();
        return 0;
    }

    if (hal.scheduler->in_timerprocess()) {
        // not allowed from timers
        return 0;
    }

    uint16_t _head;

    while (BUF_SPACE(_writebuf) == 0) {
        if (_nonblocking_writes) {
            return 0;
        }
        hal.scheduler->delay(1);
    }
    _writebuf[_writebuf_tail] = c;
    BUF_ADVANCETAIL(_writebuf, 1);

    return 1;
}

size_t STM32UARTDriver::write(const uint8_t *buffer, size_t size)
{
    if (!_initialised) {
        try_initialise();
        return 0;
    }

    if (hal.scheduler->in_timerprocess()) {
        // not allowed from timers
        return 0;
    }

    if (!_nonblocking_writes) {
        /*
          use the per-byte delay loop in write() above for blocking writes
         */
        size_t ret = 0;
        while (size--) {
            if (write(*buffer++) != 1) break;
            ret++;
        }
        return ret;
    }

    uint16_t _head, space;
    space = BUF_SPACE(_writebuf);
    if (space == 0) {
        return 0;
    }
    if (size > space) {
        size = space;
    }
    if (_writebuf_tail < _head) {
        // perform as single memcpy
        assert(_writebuf_tail+size <= _writebuf_size);
        memcpy(&_writebuf[_writebuf_tail], buffer, size);
        BUF_ADVANCETAIL(_writebuf, size);
        return size;
    }

    // perform as two memcpy calls
    uint16_t n = _writebuf_size - _writebuf_tail;
    if (n > size) n = size;
    assert(_writebuf_tail+n <= _writebuf_size);
    memcpy(&_writebuf[_writebuf_tail], buffer, n);
    BUF_ADVANCETAIL(_writebuf, n);
    buffer += n;
    n = size - n;
    if (n > 0) {
        assert(_writebuf_tail+n <= _writebuf_size);
        memcpy(&_writebuf[_writebuf_tail], buffer, n);
        BUF_ADVANCETAIL(_writebuf, n);
    }

    return size;
}

void STM32UARTDriver::_timer_tick() {
    uint16_t n;

    if (!_initialised) return;

    _in_timer = true;

    // write any pending bytes
    uint16_t _tail;
    n = BUF_AVAILABLE(_writebuf);
    if (n > 0) {
        if (_tail > _writebuf_head) {
            // do as a single write
            _write(&_writebuf[_writebuf_head], n);
        } else {
            // split into two writes
            uint16_t n1 = _writebuf_size - _writebuf_head;
            int ret = _write(&_writebuf[_writebuf_head], n1);
            if (ret == n1 && n > n1) {
                _write(&_writebuf[_writebuf_head], n - n1);
            }
        }
    }

    // try to fill the read buffer
    uint16_t _head;
    n = BUF_SPACE(_readbuf);
    if (n > 0) {
        if (_readbuf_tail < _head) {
            // one read will do
            assert(_readbuf_tail+n <= _readbuf_size);
            _read(&_readbuf[_readbuf_tail], n);
        } else {
            uint16_t n1 = _readbuf_size - _readbuf_tail;
            assert(_readbuf_tail+n1 <= _readbuf_size);
            int ret = _read(&_readbuf[_readbuf_tail], n1);
            if (ret == n1 && n > n1) {
                assert(_readbuf_tail+(n-n1) <= _readbuf_size);
                _read(&_readbuf[_readbuf_tail], n - n1);
            }
        }
    }

    _in_timer = false;
}

void STM32UARTDriver::set_flow_control(AP_HAL::UARTDriver::flow_control flow_control) {
    UARTDriver::set_flow_control(flow_control);
}

int STM32UARTDriver::_write(const uint8_t *buf, uint16_t n) {
    HAL_Status status = HAL_UartSend(uartHandles[_uart_num], (uint8_t *)buf, n);
    if (status == HAL_STATUS_OK) {
        BUF_ADVANCEHEAD(_writebuf, n);
        _last_write_time = hal.scheduler->millis();
        _total_written += n;
        return n;
    }

    if (hal.scheduler->millis() - _last_write_time > 2 &&
        _flow_control == FLOW_CONTROL_DISABLE) {
        _last_write_time = hal.scheduler->millis();
        // we haven't done a successful write for 2ms, which means the
        // port is running at less than 500 bytes/sec. Start
        // discarding bytes, even if this is a blocking port. This
        // prevents the ttyACM0 port blocking startup if the endpoint
        // is not connected
        BUF_ADVANCEHEAD(_writebuf, n);
        return n;
    }

    return 0;
}

int STM32UARTDriver::_read(uint8_t *buf, uint16_t n) {
    uint32_t ret = 0;

    if (_uart_num == HAL_STM32_UART1){
        return ret ;
    }

    HAL_UartRecv(uartHandles[_uart_num], (uint8_t *)buf, n, &ret);
    if (ret > 0) {
        BUF_ADVANCETAIL(_readbuf, ret);
        _total_read += ret;
    }

    return ret;
}

void STM32UARTDriver::try_initialise() {
    if (_initialised) {
        return;
    }

    if ((hal.scheduler->millis() - _last_initialise_attempt_ms) < 2000) {
        return;
    }
    _last_initialise_attempt_ms = hal.scheduler->millis();
    if (hal.util->safety_switch_state() != AP_HAL::Util::SAFETY_ARMED) {
        begin(0);
    }
}

