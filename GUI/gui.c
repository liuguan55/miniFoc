#include <string.h>
#include <stdlib.h>
#include "gui.h"
#include "st7735/image.h"
//#include "FOC_utils.h"
#include "lvgl/lvgl.h"
#include "../framework/MiniButton.h"
#include "elog.h"

#undef LOG_TAG
#define LOG_TAG "GUI"

const char foc_mode_name[FOC_CONTROL_MODE_NUM][20] = {
        "OPEN POS",
        "OPEN VEL",
        "TORQUE",
        "VELOCITY",
        "POSITION",
        "SPRING",
        "SPRING DMP",
        "DAMP",
        "KNOB",
        "0 RESIST"
};


void gui_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    if (x1 == x2) { // slope == infinite
        if (y2 < y1) { // Make sure that y1 <= y2
            uint8_t temp = y2;
            y2 = y1;
            y1 = temp;
        }
        for (uint8_t y0 = y1; y0 <= y2; y0++) {
            ST7735_DrawPixel(x1, y0, color);
        }
        return;
    }

    if (x2 < x1) { // Make sure that x1 <= x2
        // Swap two variable in-place
        uint8_t temp = x2;
        x2 = x1;
        x1 = temp;

        temp = y2;
        y2 = y1;
        y1 = temp;
    }


    for (uint8_t x0 = x1; x0 <= x2; x0++) { // iterate each pixel of the line
        uint8_t y0 = (y2 - y1) * (x0 - x1) / (x2 - x1) + y1;
        ST7735_DrawPixel(x0, y0, color);
    }
    return;
}

void gui_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t fill, uint16_t color) {
    // Clock wise from left top
    if (fill) {
        for (int i = y1; i <= y2; i++) {
            for (int j = x1; j <= x2; j++) {
                ST7735_DrawPixel(j, i, color);
            }
        }
        return;
    }

    gui_draw_line(x1, y1, x2, y1, color); // Line1
    gui_draw_line(x2, y1, x2, y2, color); // Line2
    gui_draw_line(x2, y2, x1, y2, color); // Line3
    gui_draw_line(x1, y2, x1, y1, color); // Line4
    return;
}

void gui_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {
    // Clock wise from left top

    gui_draw_line(x1, y1, x2, y2, color); // Line1
    gui_draw_line(x2, y2, x3, y3, color); // Line2
    gui_draw_line(x3, y3, x1, y1, color); // Line3
}

void gui_draw_init(const char *init_info, uint8_t refresh) {
    if (refresh) {
        ST7735_FillScreen(BACKGROUND_COLOR);
//        ST7735_DrawImage(0, 0, ST7735_WIDTH, ST7735_HEIGHT, gImage_1);

        ST7735_WriteString(20, 20, "FOC", Font_16x16, TITLE_COLOR, BACKGROUND_COLOR);
        ST7735_WriteString(20, 38, "Driver", Font_8x16, TITLE_COLOR, BACKGROUND_COLOR);
        gui_draw_line(20, 18, 120, 18, TEXT_COLOR);
        gui_draw_line(20, 76, 120, 76, TEXT_COLOR);

        ST7735_WriteString(20, 56, init_info, Font_8x16, TEXT_COLOR, BACKGROUND_COLOR);
    } else {
        ST7735_FillRectangle(20, 56, 80, 10, BACKGROUND_COLOR);
        ST7735_WriteString(20, 56, init_info, Font_8x16, TEXT_COLOR, BACKGROUND_COLOR);
    }
}

static int modeSelectRefresh = 1;
void gui_draw_mode_selection(FOC_CONTROL_MODE mode) {
    static const int y = 80, width = 32, height = 20;

	log_d("mode %d\n",mode);
    if (modeSelectRefresh){
	  modeSelectRefresh = 0;
	  ST7735_FillScreenFast(BACKGROUND_COLOR);

	  gui_draw_line(0, y - 3, 128, y - 3, TEXT_COLOR);
	  gui_draw_line(0, 20, 128, 20, TEXT_COLOR);

	  gui_draw_button(3, y, width, height, "lef", TEXT_COLOR, BACKGROUND_COLOR);
	  gui_draw_button(50, y, width, height, "cfm", TEXT_COLOR, BACKGROUND_COLOR);
////    gui_draw_button(90, y, width, height, "cel", ST7735_BLACK, ST7735_WHITE);
	  gui_draw_button(95, y, width, height, "rig", TEXT_COLOR, BACKGROUND_COLOR);

	  ST7735_WriteString(0, 0, "Mode Selection", Font_8x16, TEXT_COLOR, BACKGROUND_COLOR);
    }

    ST7735_WriteString(0, 30, foc_mode_name[mode], Font_8x16, TITLE_COLOR, BACKGROUND_COLOR);
    ST7735_FillRectangle(0, 57, ST7735_WIDTH * (mode + 1) / FOC_CONTROL_MODE_NUM , 3, BAR_COLOR);
  	ST7735_FillRectangle(ST7735_WIDTH * (mode + 1) / FOC_CONTROL_MODE_NUM, 57, ST7735_WIDTH , 3, BACKGROUND_COLOR);

//    gui_draw_parameter(0, 57, "A", 10);
}

void gui_draw_position_mode(float angle, uint8_t refresh) {
  	static const int y = 80, width = 32, height = 20;
    if (refresh) {
	    modeSelectRefresh = 1;
        ST7735_FillScreenFast(BACKGROUND_COLOR);

        gui_draw_line(0, y - 3, 180, y - 3, TEXT_COLOR);
        gui_draw_line(0, 20, 180, 20, TEXT_COLOR);

        gui_draw_button(3, y, width, height, "lef", TEXT_COLOR, BACKGROUND_COLOR);
//    gui_draw_button(50, y, width, height, "cfm", ST7735_BLACK, ST7735_WHITE);
        gui_draw_button(50, y, width, height, "cel", TEXT_COLOR, BACKGROUND_COLOR);
        gui_draw_button(95, y, width, height, "rig", TEXT_COLOR, BACKGROUND_COLOR);

        ST7735_WriteString(0, 0, "Position Mode", Font_8x16, TITLE_COLOR, BACKGROUND_COLOR);
    } else {
        ST7735_FillRectangle(0, 21, 128, 40, BACKGROUND_COLOR);
    }
    // show parameters
    gui_draw_parameter(0, 22, "Angle", angle * 57.295779513f, 0, 0);
}

void
gui_draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *string, uint16_t color, uint16_t bgcolor) {

    gui_draw_line(x, y, x + w, y, color);
    gui_draw_line(x + w, y, x + w, y + h, color);
    gui_draw_line(x + w, y + h, x, y + h, color);
    gui_draw_line(x, y + h, x, y, color);
    ST7735_WriteString(x + 1, y + 1, string, Font_8x16, color, bgcolor);
}

void gui_draw_parameter(uint16_t x, uint16_t y, const char *item, int16_t value, uint8_t change, uint8_t selected) {
    char data[20], num[10];
    strcpy(data, item);
    strcat(data, ":");
    itoa(value, num, 10);
    strcat(data, num);
    if (selected) {
        if (change) {
            ST7735_WriteString(x, y, data, Font_8x16, TEXT_COLOR, SELECTED_COLOR);
        } else {
            ST7735_WriteString(x, y, data, Font_8x16, BACKGROUND_COLOR, SELECTED_COLOR);
        }
    } else {
        ST7735_WriteString(x, y, data, Font_8x16, TEXT_COLOR, BACKGROUND_COLOR);
    }
}

void gui_draw_knob_mode(uint8_t sector_num, uint8_t k, uint8_t max_force, uint8_t select_param, uint8_t change,
                        uint8_t refresh) {
    static const int y = 65, width = 23, height = 12;
    static const char param_name[3][10] = {
            "Sector",
            "k",
            "max_F",
    };
    static uint8_t values[3];

    if (refresh) {
        ST7735_FillScreenFast(BACKGROUND_COLOR);

        gui_draw_line(0, y - 3, 180, y - 3, TEXT_COLOR);
        gui_draw_line(0, 20, 180, 20, TEXT_COLOR);

        gui_draw_button(10, y, width, height, "lef", TEXT_COLOR, BACKGROUND_COLOR);
//    gui_draw_button(50, y, width, height, "cfm", ST7735_BLACK, ST7735_WHITE);
        gui_draw_button(90, y, width, height, "cel", TEXT_COLOR, BACKGROUND_COLOR);
        gui_draw_button(130, y, width, height, "rig", TEXT_COLOR, BACKGROUND_COLOR);

        ST7735_WriteString(0, 0, "Knob Mode", Font_8x16, TITLE_COLOR, BACKGROUND_COLOR);
    } else {
        ST7735_FillRectangle(0, 21, 180, 40, BACKGROUND_COLOR);
    }

    // show parameters
    values[0] = sector_num, values[1] = k, values[2] = max_force;
    for (int i = 0; i < 3; ++i) {
        if (select_param == i + 1) {
            gui_draw_parameter(0, 22 + i * 10, param_name[i], values[i], change, 1);
        } else {
            gui_draw_parameter(0, 22 + i * 10, param_name[i], values[i], change, 0);
        }
    }
}
static void display_flush(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
  const lv_coord_t hres = disp_drv->physical_hor_res == -1 ? disp_drv->hor_res : disp_drv->physical_hor_res;
  const lv_coord_t vres = disp_drv->physical_ver_res == -1 ? disp_drv->ver_res : disp_drv->physical_ver_res;

//    printf("x1:%d,y1:%d,x2:%d,y2:%d\n", area->x1, area->y1, area->x2, area->y2);

  /*Return if the area is out the screen*/
  if(area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) {
	lv_disp_flush_ready(disp_drv);
	return;
  }

  /* TYPICALLY YOU DO NOT NEED THIS
  * If it was the last part to refresh update the texture of the window.*/
  if(lv_disp_flush_is_last(disp_drv)) {
	ST7735_DrawImage(area->x1, area->y1, area->x2 - area->x1, area->y2 - area->y1, (uint8_t *)color_p);
  }

  /*IMPORTANT! It must be called to tell the system the flush is ready*/
  lv_disp_flush_ready(disp_drv);
}



/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

  /*Your code comes here*/
  if (MiniButton_read(id)){
    return true;
  }

  return false;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
  uint8_t i;

  /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
  for(i = 0; i < MINI_BUTTON_MAX; i++) {
	/*Return the pressed button's ID*/
	if(button_is_pressed(i)) {
	  return i;
	}
  }

  /*No button pressed*/
  return -1;
}


/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

  static uint8_t last_btn = 0;

  /*Get the pressed button's ID*/
  int8_t btn_act = button_get_pressed_id();

  if(btn_act >= 0) {
	data->state = LV_INDEV_STATE_PR;
	last_btn = btn_act;
  }
  else {
	data->state = LV_INDEV_STATE_REL;
  }

  /*Save the last pressed button's ID*/
  data->btn_id = last_btn;
}


/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  ST7735_Init();
  ST7735_FillScreenFast(BACKGROUND_COLOR);
  ST7735_LedOn();

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf_1_1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10];            /*A screen sized buffer*/
  static lv_color_t buf_1_2[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10];            /*Another screen sized buffer*/
  lv_disp_draw_buf_init(&disp_buf1, buf_1_1, buf_1_2, ST7735_WIDTH * MY_DISP_VER_RES / 10);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = display_flush;
  disp_drv.hor_res = MY_DISP_HOR_RES;
  disp_drv.ver_res = MY_DISP_VER_RES;
  disp_drv.full_refresh = 1;
  lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  lv_group_t * g = lv_group_create();
  lv_group_set_default(g);

  /* Add the mouse as input device
   * Use the 'mouse' driver which reads the PC's mouse*/
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
  indev_drv_1.type = LV_INDEV_TYPE_BUTTON;
  /*This function will be called periodically (by the library) to get the mouse position and state*/
  indev_drv_1.read_cb = button_read;
  lv_indev_t *button_indev = lv_indev_drv_register(&indev_drv_1);

  lv_indev_set_group(button_indev,g);

  /*Assign buttons to points on the screen*/
  static const lv_point_t btn_points[MINI_BUTTON_MAX] = {
	  {10, 10},   /*Button 0 -> x:10; y:10*/
	  {40, 100},  /*Button 1 -> x:40; y:100*/
	  {10, 20},   /*Button 0 -> x:10; y:20*/
	  {40, 40},  /*Button 1 -> x:40; y:40*/
  };
  lv_indev_set_button_points(button_indev, btn_points);
}


int MiniGui_lvglInit(void)
{
  /*Initialize LVGL*/
  lv_init();

  hal_init();

  return 0;
}


