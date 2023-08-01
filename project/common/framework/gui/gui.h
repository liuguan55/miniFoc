// /* Copyright (C) 2023 dengzhijun. All rights reserved.
//  *
//  *  Redistribution and use in source and binary forms, with or without
//  *  modification, are permitted provided that the following conditions
//  *  are met:
//  *    1. Redistributions of source code must retain the above copyright
//  *       notice, this list of conditions and the following disclaimer.
//  *    2. Redistributions in binary form must reproduce the above copyright
//  *       notice, this list of conditions and the following disclaimer in the
//  *       documentation and/or other materials provided with the
//  *       distribution.
//  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  *

#ifndef FOC_DRIVER_GUI_H
#define FOC_DRIVER_GUI_H
#include "common/framework/MiniFoc/MiniFoc.h"
#include "driver/component/lcd/st7735/st7735.h"

#define MY_DISP_HOR_RES ST7735_WIDTH
#define MY_DISP_VER_RES ST7735_HEIGHT

#ifndef MY_DISP_HOR_RES
#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 128 is used for now.
#define MY_DISP_HOR_RES    128
#endif

#ifndef MY_DISP_VER_RES
#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 128 is used for now.
#define MY_DISP_VER_RES    128
#endif

#define MINI_BUTTON_MAX (4)

//#define LIGHT_MODE
#define DARK_MODE


#ifdef LIGHT_MODE
#define BACKGROUND_COLOR ST7735_WHITE
#define TEXT_COLOR ST7735_BLACK
#define TITLE_COLOR ST7735_BLUE
#define SELECTED_COLOR ST7735_BLUE
#define BAR_COLOR ST7735_RED
#endif

#ifdef DARK_MODE
#define BACKGROUND_COLOR ST7735_BLACK
#define TEXT_COLOR ST7735_WHITE
#define TITLE_COLOR ST7735_YELLOW
#define SELECTED_COLOR ST7735_BLUE
#define BAR_COLOR ST7735_RED
#endif


extern const char foc_mode_name[FOC_CONTROL_MODE_NUM][20];

void gui_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void gui_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t fill, uint16_t color);
void gui_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void gui_draw_init(const char *init_info, uint8_t refresh);
void gui_draw_mode_selection(FOC_CONTROL_MODE mode);
void gui_draw_position_mode(float angle, uint8_t refresh);
void gui_draw_knob_mode(uint8_t sector_num, uint8_t k, uint8_t max_force, uint8_t select_param, uint8_t change,
                        uint8_t refresh);
void
gui_draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *string, uint16_t color, uint16_t bgcolor);
void gui_draw_parameter(uint16_t x, uint16_t y, const char *item, int16_t value, uint8_t change, uint8_t selected);
void gui_draw_test();


int MiniGui_lvglInit(void);

#endif //FOC_DRIVER_GUI_H
