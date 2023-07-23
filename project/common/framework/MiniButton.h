//
// Created by 邓志君 on 2023/1/18.
//

#ifndef MINI_FOC_CORE_SRC_MINIBUTTON_H_
#define MINI_FOC_CORE_SRC_MINIBUTTON_H_


#define BUTTON_LEFT_Port GPIOC
#define BUTTON_LEFT_PIN GPIO_PIN_14

#define BUTTON_RIGHT_Port GPIOC
#define BUTTON_RIGHT_PIN GPIO_PIN_15

#define BUTTON_CANCEL GPIOB
#define BUTTON_CANCEL_PIN GPIO_PIN_8

#define BUTTON_CONFIRM_Port  GPIOB
#define BUTTON_CONFIRM_PIN GPIO_PIN_9

extern uint8_t button_press_pending_flag;

extern uint8_t button_left_press_pending_flag;
extern uint8_t button_right_press_pending_flag;
extern uint8_t button_confirm_press_pending_flag;
extern uint8_t button_cancel_press_pending_flag;

void button_reset_all_flags();
void MiniButton_init(void);
void MiniButton_run(void);

uint8_t MiniButton_read(uint8_t id);
#endif //MINI_FOC_CORE_SRC_MINIBUTTON_H_
