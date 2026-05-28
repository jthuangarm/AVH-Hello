/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2024 Arm Limited (or its affiliates).
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    Hello.c
 *      Purpose: Hello World based on RTX
 *
 *---------------------------------------------------------------------------*/

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "cmsis_compiler.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2


/*---------------------------------------------------------------------------
 * RTX idle thread hook
 *---------------------------------------------------------------------------*/

volatile uint32_t idle_count = 0U;

__NO_RETURN void osRtxIdleThread (void *argument) {
  (void)argument;

  for (;;) {
    idle_count++;
  }
}

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

static int count = 0;

static const uint32_t worker_ids[] = { 1U, 2U };

static void worker_thread (void *argument) {
  uint32_t worker_id = *((uint32_t *)argument);
  uint32_t local_counter = 0U;

  while (1) {
    uint32_t local_delay_ms = 25000U * worker_id;
    local_counter++;

    printf ("Worker %lu local_counter=%lu\r\n",
            (unsigned long)worker_id,
            (unsigned long)local_counter);

    osDelay (local_delay_ms);
  }
}

static void app_main_thread (void *argument) {
  (void)argument;

  while (1)  {
    printf ("Hello World %d\r\n", count);
    if (count >= 100) printf ("\x04");  // EOT (0x04) stops simulation
    count++;
    osDelay (10000);
  }
}

/*---------------------------------------------------------------------------
 * Application initialization
 *---------------------------------------------------------------------------*/
int app_main (void) {
  const osThreadAttr_t app_main_attr = {
    .name = "app_main"
  };
  const osThreadAttr_t worker1_attr = {
    .name = "worker_1"
  };
  const osThreadAttr_t worker2_attr = {
    .name = "worker_2"
  };

  osKernelInitialize();                 // Initialize CMSIS-RTOS2
  osThreadNew(app_main_thread, NULL, &app_main_attr);
  osThreadNew(worker_thread, (void *)&worker_ids[0], &worker1_attr);
  osThreadNew(worker_thread, (void *)&worker_ids[1], &worker2_attr);
  osKernelStart();                      // Start thread execution
  return 0;
}
