
#include "config.h"
#include "main.h"
#include "report.h"
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
    WIFI_init();
    MQTT_init();

    n = 0;

    MQTT_subscribe("marcos_practica2/led_bright");
    MQTT_subscribe("marcos_practica2/enable_measurement");
    while (1) {
        // IO_toggleLed();
        if (REPORT_MEASUREMENTReportCheck() != 0) {
            if (IO_getLed() == 0) {
                IO_setLed(1);
                MQTT_publish("marcos_practica2/led", "ON");
                CRONO_timerStart(100);
            }
        }
        else {
            if (IO_getLed() != 0) {
                CRONO_timerStop();
                IO_setLed(0);
                n = 0;
                MQTT_publish("marcos_practica2/led", "OFF");
            }
        }
        CRONO_delayMs(100);
    }
}
