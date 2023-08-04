#include <stdio.h>
#include "common/framework/util/MiniCommon.h"
#include "driver/hal/hal_crc.h"

#undef LOG_TAG
#define LOG_TAG "REMOTE"

typedef enum {
    REMOTE_MSG_TYPE_JOYSTICK = 0,
}

#define FRAME_HEAD  0XAA

#define MAX_FRAME_SIZE  32


static void sendData(uint8_t *buf, uint16_t length) {
    wirelessSend(buf, length);
}
/**
 * @brief send the frame data
 */
static void sendFrameData(CPRFrameType type, uint8_t *data, uint16_t len) {
    uint8_t frame[MAX_FRAME_SIZE];
    uint8_t i = 0;
    frame[i++] = FRAME_HEAD;
    frame[i++] = (len + 5);
    frame[i++] = (len + 5) >> 8;
    frame[i++] = (uint8_t)type;
    while (len) {
        frame[i++] = *data++;
        len--;
    }

    frame[i] = hal_crc8(frame, i);
    i++;

    sendData(frame, i);
}