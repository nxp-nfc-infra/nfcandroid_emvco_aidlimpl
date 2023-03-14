/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Copyright 2022-2023 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _EMVCO_CONFIG_H_
#define _EMVCO_CONFIG_H_

/** \addtogroup EMVCO_STACK_UTILITY_API_INTERFACE
 *  @brief interface to read configuration values on stored on config file. Also
 * provides Interface for getting the chip type, linked list and Semphore
 * implementation.
 *  @{
 */

#include "map.h"
#include <emvco_status.h>
#include <stdbool.h>
#include <stdint.h>

extern uint8_t cmd_idle_pwr_off_cfg[];

struct map *fp_config_map;

/**
 * @brief Reads the configuration from a file.
 *
 * This function reads the configuration from a file with the given name and
 * updates the internal configuration.
 *
 * @param[in] name The name of the configuration file.
 *
 * @return True if the configuration was successfully read, false otherwise.
 */
bool read_config(const char *name);

/**
 * @brief Retrieves an unsigned long value associated with a key in the map.
 *
 * @param key Pointer to the key to search for in the map.
 * @param p_value Pointer to a variable that will hold the retrieved value.
 * @param value_len Pointer to a variable that will hold the length of the
 * retrieved value.
 *
 * @return TRUE if the key is found in the map, FALSE otherwise.
 */
int get_byte_value(char *key, unsigned long *p_value, unsigned int *value_len);

/**
 * @brief Retrieves the byte array value of the given key from the map
 *
 * @note Caller have to free the memory of p_value after usage.
 *
 * @param[in] key Key to retrieve the value of
 * @param[out] p_value Pointer to store the retrieved value
 * @param[out] value_len Length of the retrieved value
 *
 * @return Returns `TRUE` if the value was successfully retrieved, `FALSE`
 * otherwise
 */
int get_byte_array_value(char *key, char **p_value, unsigned int *value_len);

/**
 * @brief Determines if a given character is a digit in the specified number
 * base.
 * @param c The character to be evaluated.
 * @param base The number base to be used for evaluation (valid values are 2 to
 * 36).
 * @return True if the character is a digit in the specified number base, false
 * otherwise.
 */
bool isDigit(char c, int base);

/**
 * @brief Converts a character representation of a digit to its integer value in
 * the specified number base.
 *
 * @param c The character representation of a digit.
 * @param base The number base to use for the conversion.
 *
 * @return The integer value of the digit in the specified number base.
 */
int getDigitValue(char c, int base);

/**
 * @brief Determines if the input character is printable.
 *
 * @param c The character to check.
 *
 * @return True if the character is printable, false otherwise.
 */
bool isPrintable(char c);

/**
 * @brief Returns a pointer to the configuration map.
 *
 * This function returns a pointer to the configuration map, which is a `map`
 * struct containing key-value pairs of configuration data.
 *
 * @return A pointer to the configuration map.
 * @retval NULL If the configuration map is not found or cannot be accessed.
 */
struct map *get_config_map(void);

/**
 * @brief Sends the list of set configuration to NFCC which was saved
 *        through setConfig API.
 *
 * @param void
 * @return void.
 */
void send_dynamic_set_config(void);

/* default configuration */
#define emvco_hal_config_path "/vendor/etc/libemvco-nxp.conf"

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif
/** @}*/
#endif //_EMVCO_CONFIG_H_