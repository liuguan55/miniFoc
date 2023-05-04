
#include "compiler.h"
#include "Scheduler.h"
#include "MiniCommon.h"
#include "AP_HAL/AP_HAL_Board.h"
#include "AP_HAL_STM32_Private.h"

using namespace STM32;

union funcProc{
    osThreadFunc_t threadFunc;
    void (STM32Scheduler::*schdulerFunc)();
};

STM32Scheduler::STM32Scheduler() :
        _timerTaskHandle(NULL),
        _ioTaskHandle(NULL),
        _uartTaskHandle(NULL),
        _delay_cb(NULL),
        _min_delay_cb_ms(0) {
    _initialized = false;
    _timer_pending = false;
    _timer_suspended = false;
    _timer_event_missed = false;
}

void test_task(void *){
    for(;;){
        hal.scheduler->delay(1000);
        printf("test_task\n");
    }
}


void STM32Scheduler::init(void *machtnichts) {
    UNUSED(machtnichts);

    static const osThreadAttr_t uartTask_attributes = {
            .name = "_uart_thread",
            .stack_size = 512 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };

    funcProc proc = {.schdulerFunc = &STM32Scheduler::_uart_thread};
    _uartTaskHandle = osThreadNew(proc.threadFunc, this, &uartTask_attributes);
    osThreadResume(_uartTaskHandle);

    static const osThreadAttr_t ioTask_attributes = {
            .name = "_io_thread",
            .stack_size = 512 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };
    proc.schdulerFunc = &STM32Scheduler::_io_thread;
    _ioTaskHandle = osThreadNew(proc.threadFunc, this, &ioTask_attributes);
    osThreadResume(_ioTaskHandle);

    static const osThreadAttr_t timerTask_attributes = {
            .name = "_timer_thread",
            .stack_size = 512 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };
    proc.schdulerFunc = &STM32Scheduler::_timer_thread;
    _timerTaskHandle = osThreadNew(proc.threadFunc, this, &timerTask_attributes);
    osThreadResume(_timerTaskHandle);
}

void STM32Scheduler::delay(uint16_t ms) {
    uint32_t start = millis();

    while ((millis() - start) < ms) {
        // this yields the CPU to other apps
        MiniCommon_delayMs(1);
        if (_min_delay_cb_ms <= ms) {
            if (_delay_cb) {
                _delay_cb();
            }
        }
    }
}

uint64_t STM32Scheduler::millis64() {
    return MiniCommon_millis();
}

uint64_t STM32Scheduler::micros64() {
    return MiniCommon_millis() * 1000;
}

uint32_t STM32Scheduler::millis() {
    return millis64();
}

uint32_t STM32Scheduler::micros() {
    return micros64();
}

void STM32Scheduler::delay_microseconds(uint16_t us) {
    delay(us / 1000);
}

void STM32Scheduler::register_delay_callback(AP_HAL::Proc proc,
                                             uint16_t min_time_ms) {
    _delay_cb = proc;
    _min_delay_cb_ms = min_time_ms;
}

void STM32Scheduler::register_timer_process(AP_HAL::MemberProc proc) {
    for (uint8_t i = 0; i < _num_timer_procs; i++) {
        if (_timer_proc[i] == proc) {
            return;
        }
    }

    if (_num_timer_procs < STM32_SCHEDULER_MAX_TIMER_PROCS) {
        _timer_proc[_num_timer_procs] = proc;
        _num_timer_procs++;
    } else {
        hal.console->printf("Out of timer processes\n");
    }
}

void STM32Scheduler::register_io_process(AP_HAL::MemberProc proc) {
    for (uint8_t i = 0; i < _num_io_procs; i++) {
        if (_io_proc[i] == proc) {
            return;
        }
    }

    if (_num_io_procs < STM32_SCHEDULER_MAX_TIMER_PROCS) {
        _io_proc[_num_io_procs] = proc;
        _num_io_procs++;
    } else {
        hal.console->printf("Out of IO processes\n");
    }
}

void STM32Scheduler::register_timer_failsafe(AP_HAL::Proc proc, uint32_t period_us) {
    UNUSED(period_us);

    _failsafe = proc;
}

void STM32Scheduler::suspend_timer_procs() {
    _timer_suspended = true;
    while (_in_timer_proc) {
        hal.scheduler->delay(1);
    }
}

void STM32Scheduler::resume_timer_procs() {
    _timer_suspended = false;
    if (_timer_event_missed) {
        _run_timers(false);
        _timer_event_missed = false;
    }
}

bool STM32Scheduler::in_timerprocess() {
    return _in_timer_proc;
}

void STM32Scheduler::begin_atomic() {}

void STM32Scheduler::end_atomic() {}

bool STM32Scheduler::system_initializing() {
    return !_initialized;
}

void STM32Scheduler::system_initialized() {
    if (_initialized) {
        panic("PANIC: scheduler::system_initialized called more than once");
    }
    _initialized = true;
}

void STM32Scheduler::panic(const prog_char_t *errormsg) {
    UNUSED(errormsg);
    hal.console->println_P(errormsg);

    for (;;);
}

void STM32Scheduler::reboot(bool hold_in_bootloader) {
    NVIC_SystemReset();
    for (;;);
}

[[noreturn]] void STM32Scheduler::_timer_thread() {
    while (system_initializing()) {
        hal.scheduler->delay(10);
    }

    while (true) {
        hal.scheduler->delay(1);

        // run registered timer processes
        _run_timers(true);
    }
}

[[noreturn]] void STM32Scheduler::_io_thread() {
    while (system_initializing()) {
        hal.scheduler->delay(10);
    }

    while (true) {
        hal.scheduler->delay(10);

        // process any pending storage writes
        ((STM32Storage *) hal.storage)->_timer_tick();

        // run registered IO processes
        _run_io();
    }
}

[[noreturn]] void STM32Scheduler::_uart_thread() {
    while (system_initializing()) {
        hal.scheduler->delay(10);
    }
    while (true) {
        hal.scheduler->delay(10);

        // process any pending serial bytes
        ((STM32UARTDriver *) hal.uartA)->_timer_tick();
        ((STM32UARTDriver *) hal.uartB)->_timer_tick();
        ((STM32UARTDriver *) hal.uartC)->_timer_tick();
    }
}

void STM32Scheduler::_run_timers(bool called_from_timer_thread) {
    UNUSED(called_from_timer_thread);
    if (_in_timer_proc) {
        return;
    }
    _in_timer_proc = true;

    if (!_timer_suspended) {
        // now call the timer based drivers
        for (int i = 0; i < _num_timer_procs; i++) {
            if (_timer_proc[i] != NULL) {
                _timer_proc[i]();
            }
        }
    } else if (called_from_timer_thread) {
        _timer_event_missed = true;
    }

    // and the failsafe, if one is setup
    if (_failsafe != NULL) {
        _failsafe();
    }

    _in_timer_proc = false;
}

void STM32Scheduler::_run_io(void) {
    if (_in_io_proc) {
        return;
    }
    _in_io_proc = true;

    if (!_timer_suspended) {
        // now call the IO based drivers
        for (int i = 0; i < _num_io_procs; i++) {
            if (_io_proc[i] != NULL) {
                _io_proc[i]();
            }
        }
    }

    _in_io_proc = false;
}

