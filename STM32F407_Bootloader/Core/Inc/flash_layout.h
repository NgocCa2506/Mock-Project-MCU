/*
 * flash_layout.h
 *
 *  Created on: Oct 12, 2026
 *      Author: CA
 *
 *  Flash memory map of the STM32F407VG, shared by Bootloader and Application.
 *  NOTE: keep this file byte-identical in both projects.
 */

#ifndef INC_FLASH_LAYOUT_H_
#define INC_FLASH_LAYOUT_H_

#include <stdint.h>

/* STM32F407VG: 1 MB internal Flash */
#define FLASH_START_ADDRESS         0x08000000UL
#define FLASH_END_ADDRESS           0x080FFFFFUL

/* Sector 0-2: Bootloader, 48 KB */
#define BOOTLOADER_START_ADDRESS    0x08000000UL
#define BOOTLOADER_END_ADDRESS      0x0800BFFFUL
#define BOOTLOADER_SIZE             (48UL * 1024UL)

/* Sector 3: Reserved, 16 KB */
#define RESERVED1_START_ADDRESS     0x0800C000UL
#define RESERVED1_END_ADDRESS       0x0800FFFFUL
#define RESERVED1_SIZE              (16UL * 1024UL)

/* Sector 4: Application Header, reserved for later parts, 64 KB */
#define APP_HEADER_START_ADDRESS    0x08010000UL
#define APP_HEADER_END_ADDRESS      0x0801FFFFUL
#define APP_HEADER_SIZE             (64UL * 1024UL)

/* Sector 5-7: Active Application, 384 KB */
#define APP_START_ADDRESS           0x08020000UL
#define APP_END_ADDRESS             0x0807FFFFUL
#define APP_SIZE                    (384UL * 1024UL)

/* Sector 8: Reserved, 128 KB */
#define RESERVED2_START_ADDRESS     0x08080000UL
#define RESERVED2_END_ADDRESS       0x0809FFFFUL
#define RESERVED2_SIZE              (128UL * 1024UL)

/* Sector 9-11: OTA image, reserved for later parts, 384 KB */
#define OTA_START_ADDRESS           0x080A0000UL
#define OTA_END_ADDRESS             0x080FFFFFUL
#define OTA_SIZE                    (384UL * 1024UL)

/* Application vector table: word 0 = initial MSP, word 1 = Reset_Handler */
#define RESET_HANDLER_OFFSET        4u

#endif /* INC_FLASH_LAYOUT_H_ */
