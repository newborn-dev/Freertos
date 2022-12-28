#ifndef _BOARD_SUPPORT_LIBRARY_H
#define _BOARD_SUPPORT_LIBRARY_H

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS (5000)

#include "pico/stdlib.h"

#include <stddef.h>
#include <stdint.h>

#define CPUTIME_TO_MS_DIVISOR                                                  \
  (123456) // XXXADS must tune to platform if needed. some platforms will have a
           // time-sycned source but it will be relative to something


#define LED_PIN (PICO_DEFAULT_LED_PIN)
#define GPIO0_PIN (2)
#define MAX_CORES (2)

extern char pcTestPassedString[];
extern size_t xTestPassedStringLen;
extern char pcTestFailedString[];
extern size_t xTestFailedStringLen;

typedef void (* softwareInterruptHandler)(void);

extern void initTestEnvironment(void);
extern void sendReport(char *buffer, size_t len);
extern void setPin(int pinNum);
extern void clearPin(int pinNum);
extern void delayMs(uint32_t ms);
extern void busyWaitMicroseconds(uint32_t us);
extern uint64_t getCPUTime(void);

extern int AMPLaunchOnCore(int coreNum, void (*function)(void));

extern int registerSoftwareInterruptHandler(softwareInterruptHandler handler);
extern void deleteSoftwareInterruptHandler(int num, softwareInterruptHandler handler);
extern void triggerSoftwareInterrupt(int num);


#define CPUTIME_TO_MS(CPUTIME_INPUT)                                           \
  ((uin32_t)(CPUTIME_INPUT / CPUTIME_TO_MS_DIVISOR))
#define MS_TO_CPUTIME(MS_INPUT) ((uint64_t)(MS_INPUT * CPUTIME_TO_MS_DIVISOR))

#ifdef USE_BSL_DEFAULT_HOOKS
extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
#endif

#endif