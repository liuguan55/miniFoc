
#ifndef __AP_HAL_STM32_SCHEDULER_H__
#define __AP_HAL_STM32_SCHEDULER_H__

#include "AP_HAL_STM32.h"
#include "cmsis_os2.h"

#define STM32_SCHEDULER_MAX_TIMER_PROCS (10)

class STM32::STM32Scheduler : public AP_HAL::Scheduler {
public:
    STM32Scheduler();
    void     init(void* machtnichts) override;
    void     delay(uint16_t ms) override;
    uint32_t millis() override;
    uint32_t micros() override;
    uint64_t millis64() override;
    uint64_t micros64() override;
    void     delay_microseconds(uint16_t us) override;
    void     register_delay_callback(AP_HAL::Proc,
                uint16_t min_time_ms) override;

    void     register_timer_process(AP_HAL::MemberProc) override;
    void     register_io_process(AP_HAL::MemberProc)    override;
    void     suspend_timer_procs() override;
    void     resume_timer_procs() override;

    bool     in_timerprocess()  override;

    void     register_timer_failsafe(AP_HAL::Proc, uint32_t period_us) override;

    void     begin_atomic();
    void     end_atomic();

    bool     system_initializing() override;
    void     system_initialized() override;

    void     panic(const prog_char_t *errormsg) override;
    void     reboot(bool hold_in_bootloader) override;

private:
    [[noreturn]] void _timer_thread(void);

    [[noreturn]] void _io_thread(void );

    [[noreturn]] void _uart_thread(void);

    void _run_timers(bool called_from_timer_thread);
    void _run_io(void);

private:
    osThreadId_t _timerTaskHandle;
    osThreadId_t _ioTaskHandle;
    osThreadId_t _uartTaskHandle;

    bool _initialized;
    volatile bool _timer_pending;
    volatile bool _timer_suspended;

    volatile bool _timer_event_missed;

    AP_HAL::Proc _delay_cb;
    uint16_t _min_delay_cb_ms;
    AP_HAL::Proc _failsafe;

    AP_HAL::MemberProc _timer_proc[STM32_SCHEDULER_MAX_TIMER_PROCS];
    uint8_t _num_timer_procs;
    volatile bool _in_timer_proc;

    AP_HAL::MemberProc _io_proc[STM32_SCHEDULER_MAX_TIMER_PROCS];
    uint8_t _num_io_procs;
    volatile bool _in_io_proc;
};

#endif // __AP_HAL_STM32_SCHEDULER_H__
