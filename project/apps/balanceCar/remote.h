#pragma once

#include <stdint.h>


/**
 * @brief send the joystick data
 */
int remote_sendJoystickData(uint16_t Lx, uint16_t Ly, uint16_t Rx, uint16_t Ry);

/**
 * @brief parse the packet
 */
int remote_parsePacket(uint8_t *data, uint16_t len);