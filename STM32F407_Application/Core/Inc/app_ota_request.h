/*
 * app_ota_request.h
 *
 *  Ask the Bootloader for an OTA update.
 */

#ifndef INC_APP_OTA_REQUEST_H_
#define INC_APP_OTA_REQUEST_H_

/* Set .ota_flag = 1 in the App header (Sector 4), then reset the MCU. */
void app_ota_request(void);

#endif /* INC_APP_OTA_REQUEST_H_ */
