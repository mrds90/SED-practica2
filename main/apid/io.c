/** \file	io.c
 *  Mar 2022
 *  Maestría en Sistemas Embebidos
 * \brief Contiene funciones principalmente para el manejo de distintos periféricos */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/ledc.h"

/* APID */
#include "../config.h"
#include "../include/io.h"
#include "../include/crono.h"

/* TAGs */
static const char *TAG = "IO/GPIO";


/************************************* ADC ***********************************/
#define DEFAULT_VREF  1100
static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_6;     // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif
static const adc_atten_t atten = ADC_ATTEN_DB_11;//ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;


/*******************************************************************************
 IO_adcInit(): Inicialización del conversor analógico digital
*******************************************************************************/
void IO_adcInit(){

  adc1_config_width(width);
  adc1_config_channel_atten(channel, atten);
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  //esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
  esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
}

/*******************************************************************************
 IO_readAdc(): Devuelve el valor leido como un entero de 12-bits de resolución
*******************************************************************************/
uint16_t IO_readAdc(){

  return (uint16_t)adc1_get_raw((adc1_channel_t)channel);  // entero 12 bitmeasure;

}

/*******************************************************************************
 IO_voltAdc(): Devuelve la lectura del AD en mV
*******************************************************************************/
uint32_t IO_voltAdc(){

  uint32_t measure;
  esp_adc_cal_get_voltage((adc1_channel_t)channel, adc_chars, &measure);
  return measure;

}


/************************************ PWM **************************************/

static ledc_channel_config_t ledc_channel;
#define LEDC_GPIO PWM_PORT   // PWM_PORT se define en config.h

/******************************************************************************
 IO_pwmInit(): inicializa el PWM en el puerto especificado en la constante LEDC_GPIO
******************************************************************************/
void IO_pwmInit(void){

        ledc_timer_config_t ledc_timer = {
          .duty_resolution = LEDC_TIMER_10_BIT,
          .freq_hz = 1000,
          .speed_mode = LEDC_HIGH_SPEED_MODE,
          .timer_num = LEDC_TIMER_0,
          .clk_cfg = LEDC_AUTO_CLK,
      };

      ledc_timer_config(&ledc_timer);
      ledc_channel.channel = LEDC_CHANNEL_0;
      ledc_channel.duty = 0;
      ledc_channel.gpio_num = LEDC_GPIO;
      ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
      ledc_channel.hpoint = 0;
      ledc_channel.timer_sel = LEDC_TIMER_0;
      ledc_channel_config(&ledc_channel);


}


/******************************************************************************
 IO_pwmSet(): recibe un float para setear en nuevo el duty cycle (%) PWM.
*******************************************************************************/
void IO_pwmSet(float duty){

  ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty / 100.0 * 1024);
  ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);

}



/********************************** GPIO **************************************/

/*******************************************************************************
IO_gpioInit(): inicializa periféricos de entrada/salida
*******************************************************************************/
void IO_gpioInit(){

  /* Configure the IOMUX register for pad BLINK_GPIO (some pads are muxed to GPIO
     on reset already, but some default to other functions and need to be switched
     to GPIO. Consult the Technical Reference for a list of pads and their default
     functions.) */

  gpio_reset_pin(BLINK_GPIO);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
  ESP_LOGI(TAG, "GPIO %d MODE %d ",BLINK_GPIO , GPIO_MODE_OUTPUT );
  gpio_set_level(BLINK_GPIO, 0);

}

/******************************************************************************
 IO_setLed(): Setea el estado del LED configurado por defecto en el módulo ESP32
******************************************************************************/
int IO_setLed(int estado){

  gpio_set_level(BLINK_GPIO, estado);
  return estado;

}

/******************************************************************************
 IO_toggleLed(): Togglea estado del LED configurado por defecto en el módulo ESP32
******************************************************************************/
void IO_toggleLed(void){

 static int estado = 0;
 estado = 1 - estado;
 gpio_set_level(BLINK_GPIO, estado);

}


/********************************** MONITOR ************************************/
#define MAX_LENGTH_GRAPH 80 // Nro de caracteres máximos para IO_monitorGraph()

/*******************************************************************************
 IO_monitorGraph(): imprime en el monitor serie un string de caracteres (por defecto '*')
 cuya cantidad es proporcional al entero x pasado por argumento
 Depende de la constante MAX_LENGTH_GRAPH, en donde se especifica cuántos caracteres
 debe abarcar el máximo valor de x (4095). Esto puede ajustarse en base al ancho
 la terminal utilizada
*******************************************************************************/
void IO_monitorStem(int x){

  int v;
  char stem_char[MAX_LENGTH_GRAPH]="";
  v = (int)(sizeof(stem_char) * (float)x/4096.0); // escalar y convertir a entero
  if (v>sizeof(stem_char)-1) v=sizeof(stem_char)-1;   // satura si se supera el máximo largo
  memset(stem_char,'*', v);
  stem_char[v]='\0';
  printf("%s\n", stem_char);

}

/******************************************************************************
 IO_monitoPause(): bloquea la ejecución del programa hasta que el usuario presione
 la tecla Enter. Recibe por argumento el string que debe imprimirse antes del bloqueo
 Importante: comentar o quitar esta función en caso de no utilizar el monitor serie
 ******************************************************************************/
void IO_monitorPause(const char * prmt){
    printf("%s", prmt);
    char c='\0';
    while(c!= '\n'){
      c = getchar();
      CRONO_delayMs(10);
    }
}
