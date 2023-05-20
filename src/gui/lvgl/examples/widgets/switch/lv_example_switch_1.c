#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

static lv_obj_t *obj1 = NULL;
static lv_obj_t *obj2 = NULL;
static lv_obj_t *obj3 = NULL;
static const lv_coord_t obj_width = 90 ;
static const lv_coord_t obj_height = 70 ;
static lv_anim_timeline_t *animTimeline = NULL;

static void anim_set_w(void *obj, int32_t val)
{
    lv_obj_set_width(obj, val);
}
static void anim_set_h(void *obj, int32_t val)
{
    lv_obj_set_height(obj, val);
}
void animTimeInit()
{
    if (animTimeline) return ;

    lv_anim_t anim1;
    lv_anim_init(&anim1);
    lv_anim_set_var(&anim1, obj1);
    lv_anim_set_early_apply(&anim1, false);
    lv_anim_set_values(&anim1,0, obj_width);
    lv_anim_set_time(&anim1,300);
    lv_anim_set_exec_cb(&anim1,anim_set_w);
    lv_anim_set_path_cb(&anim1, lv_anim_path_overshoot);

    lv_anim_t anim2;
    lv_anim_init(&anim2);
    lv_anim_set_var(&anim2, obj1);
    lv_anim_set_early_apply(&anim2, false);
    lv_anim_set_values(&anim2,0, obj_height);
    lv_anim_set_time(&anim2,300);
    lv_anim_set_exec_cb(&anim2,anim_set_h);
    lv_anim_set_path_cb(&anim2, lv_anim_path_ease_out);

    lv_anim_t anim3;
    lv_anim_init(&anim3);
    lv_anim_set_var(&anim3, obj2);
    lv_anim_set_early_apply(&anim3, false);
    lv_anim_set_values(&anim3,0, obj_width);
    lv_anim_set_time(&anim3,1000);
    lv_anim_set_exec_cb(&anim3,anim_set_w);
    lv_anim_set_path_cb(&anim3, lv_anim_path_overshoot);

    lv_anim_t anim4;
    lv_anim_init(&anim4);
    lv_anim_set_var(&anim4, obj2);
    lv_anim_set_early_apply(&anim4, false);
    lv_anim_set_values(&anim4,0, obj_height);
    lv_anim_set_time(&anim4,1000);
    lv_anim_set_exec_cb(&anim4,anim_set_h);
    lv_anim_set_path_cb(&anim4, lv_anim_path_ease_out);

    lv_anim_t anim5;
    lv_anim_init(&anim5);
    lv_anim_set_var(&anim5, obj3);
    lv_anim_set_early_apply(&anim5, false);
    lv_anim_set_values(&anim5,0, obj_width);
    lv_anim_set_time(&anim5,1000);
    lv_anim_set_exec_cb(&anim5,anim_set_w);
    lv_anim_set_path_cb(&anim5, lv_anim_path_overshoot);

    lv_anim_t anim6;
    lv_anim_init(&anim6);
    lv_anim_set_var(&anim6, obj3);
    lv_anim_set_early_apply(&anim6, false);
    lv_anim_set_values(&anim6,0, obj_height);
    lv_anim_set_time(&anim6,1000);
    lv_anim_set_exec_cb(&anim6,anim_set_h);
    lv_anim_set_path_cb(&anim6, lv_anim_path_ease_out);

    animTimeline = lv_anim_timeline_create();
    lv_anim_timeline_add(animTimeline, 0, &anim1);
    lv_anim_timeline_add(animTimeline, 0, &anim2);
    lv_anim_timeline_add(animTimeline, 1000, &anim3);
    lv_anim_timeline_add(animTimeline, 1000, &anim4);
    lv_anim_timeline_add(animTimeline, 2000, &anim5);
    lv_anim_timeline_add(animTimeline, 2000, &anim6);
}

void btn_start_event_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    if (!animTimeline){
        animTimeInit();
    }

    bool reverse = lv_obj_has_state(obj, LV_STATE_CHECKED);
    lv_anim_timeline_set_reverse(animTimeline, reverse);
    lv_anim_timeline_start(animTimeline);
}

lv_obj_t* container;

static void button_event_cb(lv_event_t* event);

void desktop_init(void)
{
    container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scroll_snap_x(container, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_style_bg_opa(container, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(container, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(container, 60, LV_PART_MAIN);   //图标之间的间隙
    lv_obj_center(container);

    //生成演示按钮
    for (int i = 0; i < 10; i++)
    {
        lv_obj_t* btn = lv_btn_create(container);
        lv_obj_set_size(btn, 80, 80);
        lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_ALL, NULL);

        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%d", i);
        lv_obj_center(label);
    }

    uint32_t mid_btn_index = (lv_obj_get_child_cnt(container) - 1) / 2;
    for (uint32_t i = 0; i < mid_btn_index; i++)
    {
        lv_obj_move_to_index(lv_obj_get_child(container, -1), 0);
    }
    /*当按钮数为偶数时，确保按钮居中*/
    lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index), LV_ANIM_OFF);
}


/**
 * @brief 处理按钮事件的回调函数
 * @param event
*/
static void button_event_cb(lv_event_t* event)
{
    lv_obj_t* current_btn = lv_event_get_current_target(event);
    uint32_t current_btn_index = lv_obj_get_index(current_btn);
    uint32_t mid_btn_index = (lv_obj_get_child_cnt(container) - 1) / 2;

    if (event->code == LV_EVENT_FOCUSED)
    {
        if (current_btn_index > mid_btn_index)
        {
            lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index - 1), LV_ANIM_OFF);
            lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index), LV_ANIM_ON);
            lv_obj_move_to_index(lv_obj_get_child(container, 0), -1);
        }
        else if (current_btn_index < mid_btn_index)
        {
            lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index + 1), LV_ANIM_OFF);
            lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index), LV_ANIM_ON);
            lv_obj_move_to_index(lv_obj_get_child(container, -1), 0);
        }
    }
    else if (event->code == LV_EVENT_CLICKED)
    {
        /**/
    }
}

static void event_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t * target = lv_event_get_target(e);

    /*The current target is always the container as the event is added to it*/
    lv_obj_t * cont = lv_event_get_current_target(e);

    /*If container was clicked do nothing*/
    if(target == cont) return;

    /*Make the clicked buttons red*/
    lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        } else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
//        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

static lv_obj_t * chart;
lv_chart_series_t * ser;
/* Source: https://github.com/ankur219/ECG-Arrhythmia-classification/blob/642230149583adfae1e4bd26c6f0e1fd8af2be0e/sample.csv*/
static const lv_coord_t ecg_sample[] = {
        -2, 2, 0, -15, -39, -63, -71, -68, -67, -69, -84, -95, -104, -107, -108, -107, -107, -107, -107, -114, -118, -117,
        -112, -100, -89, -83, -71, -64, -58, -58, -62, -62, -58, -51, -46, -39, -27, -10, 4, 7, 1, -3, 0, 14, 24, 30, 25, 19,
        13, 7, 12, 15, 18, 21, 13, 6, 9, 8, 17, 19, 13, 11, 11, 11, 23, 30, 37, 34, 25, 14, 15, 19, 28, 31, 26, 23, 25, 31,
        39, 37, 37, 34, 30, 32, 22, 29, 31, 33, 37, 23, 13, 7, 2, 4, -2, 2, 11, 22, 33, 19, -1, -27, -55, -67, -72, -71, -63,
        -49, -18, 35, 113, 230, 369, 525, 651, 722, 730, 667, 563, 454, 357, 305, 288, 274, 255, 212, 173, 143, 117, 82, 39,
        -13, -53, -78, -91, -101, -113, -124, -131, -131, -131, -129, -128, -129, -125, -123, -123, -129, -139, -148, -153,
        -159, -166, -183, -205, -227, -243, -248, -246, -254, -280, -327, -381, -429, -473, -517, -556, -592, -612, -620,
        -620, -614, -604, -591, -574, -540, -497, -441, -389, -358, -336, -313, -284, -222, -167, -114, -70, -47, -28, -4, 12,
        38, 52, 58, 56, 56, 57, 68, 77, 86, 86, 80, 69, 67, 70, 82, 85, 89, 90, 89, 89, 88, 91, 96, 97, 91, 83, 78, 82, 88, 95,
        96, 105, 106, 110, 102, 100, 96, 98, 97, 101, 98, 99, 100, 107, 113, 119, 115, 110, 96, 85, 73, 64, 69, 76, 79,
        78, 75, 85, 100, 114, 113, 105, 96, 84, 74, 66, 60, 75, 85, 89, 83, 67, 61, 67, 73, 79, 74, 63, 57, 56, 58, 61, 55,
        48, 45, 46, 55, 62, 55, 49, 43, 50, 59, 63, 57, 40, 31, 23, 25, 27, 31, 35, 34, 30, 36, 34, 42, 38, 36, 40, 46, 50,
        47, 32, 30, 32, 52, 67, 73, 71, 63, 54, 53, 45, 41, 28, 13, 3, 1, 4, 4, -8, -23, -32, -31, -19, -5, 3, 9, 13, 19,
        24, 27, 29, 25, 22, 26, 32, 42, 51, 56, 60, 57, 55, 53, 53, 54, 59, 54, 49, 26, -3, -11, -20, -47, -100, -194, -236,
        -212, -123, 8, 103, 142, 147, 120, 105, 98, 93, 81, 61, 40, 26, 28, 30, 30, 27, 19, 17, 21, 20, 19, 19, 22, 36, 40,
        35, 20, 7, 1, 10, 18, 27, 22, 6, -4, -2, 3, 6, -2, -13, -14, -10, -2, 3, 2, -1, -5, -10, -19, -32, -42, -55, -60,
        -68, -77, -86, -101, -110, -117, -115, -104, -92, -84, -85, -84, -73, -65, -52, -50, -45, -35, -20, -3, 12, 20, 25,
        26, 28, 28, 30, 28, 25, 28, 33, 42, 42, 36, 23, 9, 0, 1, -4, 1, -4, -4, 1, 5, 9, 9, -3, -1, -18, -50, -108, -190,
        -272, -340, -408, -446, -537, -643, -777, -894, -920, -853, -697, -461, -251, -60, 58, 103, 129, 139, 155, 170, 173,
        178, 185, 190, 193, 200, 208, 215, 225, 224, 232, 234, 240, 240, 236, 229, 226, 224, 232, 233, 232, 224, 219, 219,
        223, 231, 226, 223, 219, 218, 223, 223, 223, 233, 245, 268, 286, 296, 295, 283, 271, 263, 252, 243, 226, 210, 197,
        186, 171, 152, 133, 117, 114, 110, 107, 96, 80, 63, 48, 40, 38, 34, 28, 15, 2, -7, -11, -14, -18, -29, -37, -44, -50,
        -58, -63, -61, -52, -50, -48, -61, -59, -58, -54, -47, -52, -62, -61, -64, -54, -52, -59, -69, -76, -76, -69, -67,
        -74, -78, -81, -80, -73, -65, -57, -53, -51, -47, -35, -27, -22, -22, -24, -21, -17, -13, -10, -11, -13, -20, -20,
        -12, -2, 7, -1, -12, -16, -13, -2, 2, -4, -5, -2, 9, 19, 19, 14, 11, 13, 19, 21, 20, 18, 19, 19, 19, 16, 15, 13, 14,
        9, 3, -5, -9, -5, -3, -2, -3, -3, 2, 8, 9, 9, 5, 6, 8, 8, 7, 4, 3, 4, 5, 3, 5, 5, 13, 13, 12, 10, 10, 15, 22, 17,
        14, 7, 10, 15, 16, 11, 12, 10, 13, 9, -2, -4, -2, 7, 16, 16, 17, 16, 7, -1, -16, -18, -16, -9, -4, -5, -10, -9, -8,
        -3, -4, -10, -19, -20, -16, -9, -9, -23, -40, -48, -43, -33, -19, -21, -26, -31, -33, -19, 0, 17, 24, 9, -17, -47,
        -63, -67, -59, -52, -51, -50, -49, -42, -26, -21, -15, -20, -23, -22, -19, -12, -8, 5, 18, 27, 32, 26, 25, 26, 22,
        23, 17, 14, 17, 21, 25, 2, -45, -121, -196, -226, -200, -118, -9, 73, 126, 131, 114, 87, 60, 42, 29, 26, 34, 35, 34,
        25, 12, 9, 7, 3, 2, -8, -11, 2, 23, 38, 41, 23, 9, 10, 13, 16, 8, -8, -17, -23, -26, -25, -21, -15, -10, -13, -13,
        -19, -22, -29, -40, -48, -48, -54, -55, -66, -82, -85, -90, -92, -98, -114, -119, -124, -129, -132, -146, -146, -138,
        -124, -99, -85, -72, -65, -65, -65, -66, -63, -64, -64, -58, -46, -26, -9, 2, 2, 4, 0, 1, 4, 3, 10, 11, 10, 2, -4,
        0, 10, 18, 20, 6, 2, -9, -7, -3, -3, -2, -7, -12, -5, 5, 24, 36, 31, 25, 6, 3, 7, 12, 17, 11, 0, -6, -9, -8, -7, -5,
        -6, -2, -2, -6, -2, 2, 14, 24, 22, 15, 8, 4, 6, 7, 12, 16, 25, 20, 7, -16, -41, -60, -67, -65, -54, -35, -11, 30,
        84, 175, 302, 455, 603, 707, 743, 714, 625, 519, 414, 337, 300, 281, 263, 239, 197, 163, 136, 109, 77, 34, -18, -50,
        -66, -74, -79, -92, -107, -117, -127, -129, -135, -139, -141, -155, -159, -167, -171, -169, -174, -175, -178, -191,
        -202, -223, -235, -243, -237, -240, -256, -298, -345, -393, -432, -475, -518, -565, -596, -619, -623, -623, -614,
        -599, -583, -559, -524, -477, -425, -383, -357, -331, -301, -252, -198, -143, -96, -57, -29, -8, 10, 31, 45, 60, 65,
        70, 74, 76, 79, 82, 79, 75, 62,
};

static void slider_x_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(obj);
    lv_chart_set_zoom_x(chart, v);
}

static void slider_y_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(obj);
    lv_chart_set_zoom_y(chart, v);
}
static void add_data(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    lv_chart_set_next_value(chart, ser, lv_rand(-1000, 1000));
}


static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
}
void lv_example_switch_1(void)
{
//    desktop_init();
    lv_obj_t * win = lv_win_create(lv_scr_act(), 40);
    lv_obj_t * btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_LEFT, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_win_add_title(win, "A title");

    btn = lv_win_add_btn(win, LV_SYMBOL_RIGHT, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 60);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "This is\n"
                             "a pretty\n"
                             "long text\n"
                             "to see how\n"
                             "the window\n"
                             "becomes\n"
                             "scrollable.\n"
                             "\n"
                             "\n"
                             "Some more\n"
                             "text to be\n"
                             "sure it\n"
                             "overflows. :)");
}

#endif
