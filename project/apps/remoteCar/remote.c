#include <stdio.h>
#include "common/framework/util/MiniCommon.h"
#include "driver/hal/hal_crc.h"
#include "wireless.h"
#include "pwm.h"
#include "controller.h"
#include "commonDefine.h"

#undef LOG_TAG
#define LOG_TAG "REMOTE"

typedef enum {
    REMOTE_MSG_TYPE_JOYSTICK = 0,
}RemoteMsgType_t;

#define FRAME_HEAD   0x55

#define MAX_FRAME_SIZE  32

typedef struct {
    uint8_t magic;
    uint8_t crc;
    RemoteMsgType_t type;
    uint8_t len;
}RemoteFrameHead_t;

typedef struct {
    uint16_t Lx;
    uint16_t Ly;
    uint16_t Rx;
    uint16_t Ry;
}RemoteJoystick_t;


static int sendData(uint8_t *buf, uint16_t length) {
    return wirelessSend(buf, length);
}
/**
 * @brief send the frame data
 */
static int sendFrameData(RemoteMsgType_t type, uint8_t *data, uint16_t len) {
    uint8_t frame[MAX_FRAME_SIZE] = {0};

    RemoteFrameHead_t *head = (RemoteFrameHead_t *)frame;
    head->magic = FRAME_HEAD;
    head->type = type;
    head->len = len + sizeof (RemoteFrameHead_t) ;
    memcpy(frame + sizeof (RemoteFrameHead_t), data, len);
    head->crc = hal_crc8(frame + 2, sizeof(RemoteFrameHead_t) + len - 2);

    return sendData(frame, head->len);
}


int remote_sendJoystickData(uint16_t Lx, uint16_t Ly, uint16_t Rx, uint16_t Ry) {
    RemoteJoystick_t joystick = {0};
    joystick.Lx = Lx;
    joystick.Ly = Ly;
    joystick.Rx = Rx;
    joystick.Ry = Ry;

    return sendFrameData(REMOTE_MSG_TYPE_JOYSTICK, (uint8_t *)&joystick, sizeof (RemoteJoystick_t));
}


int remote_parsePacket(uint8_t *data, uint16_t len) {
    if (len < sizeof (RemoteFrameHead_t)) {
        return -1;
    }

    RemoteFrameHead_t *head = (RemoteFrameHead_t *)data;
    if (head->magic != FRAME_HEAD) {
        return -1;
    }

    if (head->len > len) {
        return -1;
    }

    uint8_t crc = hal_crc8(data + 2, head->len - 2);
    if (head->crc != crc) {
        return -1;
    }

    switch (head->type) {
        case REMOTE_MSG_TYPE_JOYSTICK:
        {
            RemoteJoystick_t *joystick = (RemoteJoystick_t *)(data + sizeof (RemoteFrameHead_t));
//            printf("Lx:%d, Ly:%d, Rx:%d, Ry:%d\n", joystick->Lx, joystick->Ly, joystick->Rx, joystick->Ry);
            pwmSetDuty(REMOTE_PWM_ID_0, joystick->Lx);
            pwmSetDuty(REMOTE_PWM_ID_1, joystick->Lx);
            pwmSetDuty(REMOTE_PWM_ID_2, joystick->Ly);
            pwmSetDuty(REMOTE_PWM_ID_3, joystick->Ly);
            pwmSetDuty(REMOTE_PWM_ID_4, joystick->Rx);
            pwmSetDuty(REMOTE_PWM_ID_5, joystick->Rx);
            pwmSetDuty(REMOTE_PWM_ID_6, joystick->Ry);
            pwmSetDuty(REMOTE_PWM_ID_7, joystick->Ry);

            int32_t  throttle = map_int32(joystick->Ly, 1000, 2000, -1000, 1000);
            int32_t  steering = map_int32(joystick->Rx, 1000, 2000, -1000, 1000);
            controllerRun(throttle, steering);
        }
            break;
        default:
            break;
    }

    return 0;
}