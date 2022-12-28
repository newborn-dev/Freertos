#include "bsl.h"

#include "pico/multicore.h"
#include "pico/mutex.h"
#include "pico/sem.h"
#include "pico/stdlib.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

char pcTestPassedString[] = "TEST PASSED\n\0";
size_t xTestPassedStringLen = sizeof(pcTestPassedString) / sizeof(char);
char pcTestFailedString[] = "TEST FAILED\n\0";
size_t xTestFailedStringLen = sizeof(pcTestFailedString) / sizeof(char);

void initTestEnvironment(void) {
  /* Setup LED I/O */
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_set_irq_enabled(LED_PIN,
    GPIO_IRQ_LEVEL_LOW |
    GPIO_IRQ_LEVEL_HIGH |
    GPIO_IRQ_EDGE_FALL |
    GPIO_IRQ_EDGE_RISE,
    false);

  /* Setup Output on GPIO0 */
  gpio_init(GPIO0_PIN);
  // gpio_set_function(GPIO0_PIN, GPIO_FUNC_NULL);
  gpio_set_dir(GPIO0_PIN, GPIO_OUT);
  gpio_set_slew_rate(GPIO0_PIN, GPIO_SLEW_RATE_FAST);
  gpio_set_drive_strength(GPIO0_PIN, GPIO_DRIVE_STRENGTH_2MA);
  gpio_set_irq_enabled(GPIO0_PIN,
    GPIO_IRQ_LEVEL_LOW |
    GPIO_IRQ_LEVEL_HIGH |
    GPIO_IRQ_EDGE_FALL |
    GPIO_IRQ_EDGE_RISE,
    false);

  /* Want to be able to printf */
  stdio_init_all();
  while (!stdio_usb_connected())
  {
    setPin(LED_PIN);
    setPin(GPIO0_PIN);
    sleep_ms(250);
    clearPin(LED_PIN);
    clearPin(GPIO0_PIN);
    sleep_ms(250);
  }


}

void sendReport(char *buffer, size_t len) { printf("%s", buffer); stdio_flush(); }

void setPin(int pinNum) { gpio_put(pinNum, 1); }

void clearPin(int pinNum) { gpio_put(pinNum, 0); }

void delayMs(uint32_t ms) { sleep_ms(ms); }

void busyWaitMicroseconds(uint32_t us) { busy_wait_us(us); }

uint64_t getCPUTime(void) {
  #ifdef NDEBUG
    return (uint64_t)get_absolute_time();
  #else
    return get_absolute_time()._private_us_since_boot;
  #endif
}

int AMPLaunchOnCore(int coreNum, void (*function)(void)) {
  int rvb = -1;

  if (coreNum == 1) {
    multicore_launch_core1(*function);
    rvb = 0;
  }

  return rvb;
}

int registerSoftwareInterruptHandler(softwareInterruptHandler handler) {
  irq_add_shared_handler(26, (irq_handler_t)handler, 0);
  irq_set_enabled(26, true);
  return 26;
}

void deleteSoftwareInterruptHandler(int num, softwareInterruptHandler handler) {
  irq_remove_handler(num, (irq_handler_t)handler);
}

void triggerSoftwareInterrupt(int num) {
  irq_set_pending(num);
}

#ifdef USE_BSL_DEFAULT_HOOKS
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  char strbuf[] = "ERROR: Stack Overflow\n\0";
  size_t strbuf_len = sizeof(strbuf) / sizeof(char);
  (void)pcTaskName;
  (void)xTask;

  sendReport(strbuf, strbuf_len);

  /* Run time stack overflow checking is performed if
  configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
  function is called if a stack overflow is detected.  pxCurrentTCB can be
  inspected in the debugger if the task name passed into this function is
  corrupt. */
  for (;;)
    ;
}

void vApplicationTickHook(void) {
  static uint32_t ulCount = 0;
  ulCount++;
}

void vApplicationMallocFailedHook(void) {
  char strbuf[] = "ERROR: Malloc Failed\n\0";
  size_t strbuf_len = sizeof(strbuf) / sizeof(char);

  sendReport(strbuf, strbuf_len);
}
#endif