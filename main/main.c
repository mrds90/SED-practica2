
#include "config.h"
#include "main.h"

const char *TAG = "MAIN";

volatile int samples[SAMPLES_SIZE];
volatile int n = 0;

/*******************************************************************************
 Programa principal
******************************************************************************/

void app_main(void) {
    /* Inicializaciones */
    IO_adcInit();
    IO_gpioInit();
    IO_pwmInit();
    CRONO_timerInit();

    CRONO_timerStart(20);
    printf("GRABACIÓN INICIADA\n");
    while (n < SAMPLES_SIZE) {
        IO_toggleLed();
        CRONO_delayMs(500);
    }
    CRONO_timerStop();
    printf("GRABACIÓN FINALIZADA\n");
    IO_monitorPause("Presione Enter para continuar...\n");
    printf("REPRODUCCIÓN INICIADA\n");
    for (n = 0; n < SAMPLES_SIZE; n++) {
        IO_pwmSet(samples[n] / 4096.0 * 100);
        IO_monitorStem(samples[n]);
        CRONO_delayMs(20);
    }
    IO_pwmSet(0);
    printf("REPRODUCCIÓN FINALIZADA\n");
    n = 0;

    while (1) {
        IO_toggleLed();
        IO_pwmSet(samples[n] / 4096.0 * 100);
        CRONO_delayMs(250);
    }
}
