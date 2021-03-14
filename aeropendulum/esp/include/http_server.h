/*
 * Copyright 2020 Marco Miretti.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef EXAMPLES_PROJECT_INCLUDE_HTTP_SERVER_H_
#define EXAMPLES_PROJECT_INCLUDE_HTTP_SERVER_H_

#include <log.h>


/**
 * Pinout for the ESP8266 D1 MINI
 * https://user-images.githubusercontent.com/26353057/87876217-f3333a00-c9ac-11ea-9a11-893db497dc48.png
 */
#define LED_PIN             2

#define ENCODER_PIN_A       12
#define ENCODER_PIN_B       13

void send_telemetry_task(void *pvParameter);

void update_actuators_task(void *pvParameter);

#endif /* EXAMPLES_PROJECT_INCLUDE_HTTP_SERVER_H_ */
