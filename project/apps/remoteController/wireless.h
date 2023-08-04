#pragma once
#include <stdint.h>


/**
  * @brief  The wireless init.
  */
void wirelessInit(void);

/**
 * @brief Send data to wireless
 * @param buf pointer to data
 * @param len pointer to data length
 * @return send length
 */
int wirelessSend(uint8_t *buf, uint8_t len);