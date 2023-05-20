
#include "GPIO.h"
#include "driver/gpio.h"
#include "MiniAssert.h"

using namespace STM32;

struct GpioTypeDef{
    GPIO_TypeDef *GPIO;
    uint16_t Pin ;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
};

static struct GpioTypeDef gpioTypeDef[AP_HAL::GPIO_NR] = {
        {GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_LED1_PIN
        {GPIOC, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_MOTOR_ENABLE
        {GPIOA, GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH}, //GPIO_SPI1_CS1_PIN
        {GPIOB, GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH}, //GPIO_SPI2_CS1
        {GPIOC, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH}, //GPIO_SPI2_CS2
        {GPIOC, GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_BTN_1
        {GPIOC, GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_BTN_2
        {GPIOB, GPIO_PIN_8, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_BTN_3
        {GPIOB, GPIO_PIN_9, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_BTN_4
        {GPIOA, GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_LCD_RST_PIN
        {GPIOB, GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}, //GPIO_LCD_RES_PIN
};

STM32GPIO::STM32GPIO()
{}

void STM32GPIO::init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    for (int i = 0; i < AP_HAL::GPIO_NR; ++i) {
        /*Configure GPIO pins : PC13 PC4 PC5 */
        GPIO_InitStruct.Pin = gpioTypeDef[i].Pin;
        GPIO_InitStruct.Mode = gpioTypeDef[i].Mode;
        GPIO_InitStruct.Pull = gpioTypeDef[i].Pull;
        GPIO_InitStruct.Speed = gpioTypeDef[i].Speed;
        HAL_GPIO_Init(gpioTypeDef[i].GPIO, &GPIO_InitStruct);
    }
}

void STM32GPIO::pinMode(uint8_t pin, uint8_t output)
{
    if (pin >= AP_HAL::GPIO_NR) return ;

    GPIO_InitTypeDef  GPIO_InitStruct;

    uint32_t mode = output ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;

    GPIO_InitStruct.Pin = gpioTypeDef[pin].Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = gpioTypeDef[pin].Pull;
    GPIO_InitStruct.Speed = gpioTypeDef[pin].Speed;
    HAL_GPIO_Init(gpioTypeDef[pin].GPIO, &GPIO_InitStruct);
}

int8_t STM32GPIO::analogPinToDigitalPin(uint8_t pin)
{
    UNUSED(pin);

	return -1;
}


uint8_t STM32GPIO::read(uint8_t pin) {
    if (pin < 0 || pin >= AP_HAL::GPIO_NR) return 0;

    return HAL_GPIO_ReadPin(gpioTypeDef[pin].GPIO, gpioTypeDef[pin].Pin);
}

void STM32GPIO::write(uint8_t pin, uint8_t value)
{
    if ( pin >= AP_HAL::GPIO_NR) return ;

    GPIO_PinState pinState ;

    if (value == HAL_GPIO_HIGH){
        pinState = GPIO_PIN_SET ;
    }else {
        pinState = GPIO_PIN_RESET;
    }
    HAL_GPIO_WritePin(gpioTypeDef[pin].GPIO, gpioTypeDef[pin].Pin, pinState);
}

void STM32GPIO::toggle(uint8_t pin)
{
    write(pin, !read(pin));
}

/* Alternative interface: */
AP_HAL::DigitalSource* STM32GPIO::channel(uint16_t n) {
    return new STM32DigitalSource(n);
}

/* Interrupt interface: */
bool STM32GPIO::attach_interrupt(uint8_t interrupt_num, AP_HAL::Proc p,
        uint8_t mode) {
    UNUSED(interrupt_num);
    UNUSED(p);
    UNUSED(mode);

    return true;
}

bool STM32GPIO::usb_connected()
{
    return false;
}

STM32DigitalSource::STM32DigitalSource(uint8_t v) :
    _v(v)
{

}

void STM32DigitalSource::mode(uint8_t output)
{
    if (_v >= AP_HAL::GPIO_NR) return ;

    GPIO_InitTypeDef  GPIO_InitStruct;

    uint32_t mode = output ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;

    GPIO_InitStruct.Pin = gpioTypeDef[_v].Pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = gpioTypeDef[_v].Pull;
    GPIO_InitStruct.Speed = gpioTypeDef[_v].Speed;
    HAL_GPIO_Init(gpioTypeDef[_v].GPIO, &GPIO_InitStruct);
}

uint8_t STM32DigitalSource::read() {
    if ( _v >= AP_HAL::GPIO_NR) return 0;

    return HAL_GPIO_ReadPin(gpioTypeDef[_v].GPIO, gpioTypeDef[_v].Pin);
}

void STM32DigitalSource::write(uint8_t value) {
    if ( _v >= AP_HAL::GPIO_NR) return ;

    GPIO_PinState pinState ;

    if (value == HAL_GPIO_HIGH){
        pinState = GPIO_PIN_SET ;
    }else {
        pinState = GPIO_PIN_RESET;
    }
    HAL_GPIO_WritePin(gpioTypeDef[_v].GPIO, gpioTypeDef[_v].Pin, pinState);
}

void STM32DigitalSource::toggle() {
    write(!read());
}
