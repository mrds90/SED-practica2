/** \file	mqtt.c
 *  Mar 2022
 *  Maestría en Sistemas Embebidos - Sistemas embebidos distribuidos
 * \brief Contiene las funciones de inicializacion y manejo del protocolo mqtt
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "mqtt_client.h"
#include "report.h"
/* APID */
#include "../config.h"
#include "../include/wifi.h"
#include "../include/io.h"
#include "../include/mqtt.h"

/* TAGs */
static const char *TAG = "MQTT";

/********************************** MQTT **************************************/

/* Definiciones */
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t);
static void mqtt_event_handler(void *, esp_event_base_t , int32_t , void *);
const char* mqtt_server = IP_BROKER_MQTT;
const int mqttPort = PORT_MQTT;
const char* mqttUser = USER_MQTT;
const char* mqttPassword = PASSWD_MQTT;
static esp_mqtt_client_handle_t client;
esp_mqtt_client_handle_t MQTT_getClient(void);

// parámetros Wifi
extern wifi_ap_record_t wifidata;

/*******************************************************************************
MQTT_subscribe(): Subscripción al topic especificado.
*******************************************************************************/
void MQTT_subscribe(const char * topic){

  esp_mqtt_client_subscribe(client, topic, 0);

}

/*******************************************************************************
 MQTT_publish(): publica mensaje en el topic especificado.
*******************************************************************************/
void MQTT_publish(const char * topic, const char * mensaje) {

  /* CON PUPLISH *********************************************************
  esp_mqtt_client_publish(client, topic, data, len, qos, retain) */
  esp_mqtt_client_publish(client, topic, mensaje, 0, 0, 0);

  /* CON ENQUEUE ********************************************************
  esp_mqtt_client_enqueue(client, topic, data, len, qos, retain, store);
  //esp_mqtt_client_enqueue(client, topic, mensaje, 0, 0, 0, 1);*/
}


/*******************************************************************************
 MQTT_processTopic(): lee el mensaje MQTT recibido cuando se dispara el evento
 ******************************************************************************/
bool is_number(const char *str) {
    bool decimal_point = false;
    if (*str == '-' || *str == '+') str++; // opcional manejo de signo
    if (!*str) return false; // string vacío después de signo
    while (*str) {
        if (*str == '.') {
            if (decimal_point) return false; // más de un punto decimal
            decimal_point = true;
        } else if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

float data_to_int(const char *data) {
    if (!is_number(data)) {
        printf("Error: data no es un número válido.\n");
        return -1.0; // Valor indicador de error
    }

    float value = atof(data);
    if (value < 0.0 || value > 4096.0) {
        printf("Error: data fuera del rango permitido (0 a 4096.0).\n");
        return -1.0; // Valor indicador de error
    }

    return value;
}

 void MQTT_processTopic(const char * topic, const char * data) {
    

    if(strcmp("marcos_practica2/led_bright", topic)==0) {
        
        float data_number = data_to_int(data);
        IO_pwmSet(data_number);
    
    }

    if (strcmp("marcos_practica2/enable_measurement", topic) == 0) {
        if (strcmp("On", data) == 0) {
            REPORT_MEASUREMENTReportEnable(ENABLE_MEASUREMENT_MEASUREMENT);
        }
        else {
            REPORT_MEASUREMENTReportEnable(DISABLE_MEASUREMENT_MEASUREMENT);
        }
    }
 }


 /*******************************************************************************
  MQTT_init(): Inicialización de MQTT
  ******************************************************************************/
void MQTT_init(void){
#ifdef ESPIDF_v4
   esp_mqtt_client_config_t mqtt_cfg = {
           //.uri = CONFIG_BROKER_URL,
           .host= mqtt_server,
           //.username = mqttUser,
           //.password = mqttPassword,
           .port = mqttPort,
   };
#else
	esp_mqtt_client_config_t mqtt_cfg = {
	.broker.address.hostname = mqtt_server,
	.broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
	.broker.address.port = mqttPort,
	//.credentials.username = mqttUser,
	//.credentials.authentication.password = mqttPassword,
  };
#endif

   ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
   client = esp_mqtt_client_init(&mqtt_cfg); //   Creates mqtt client handle based on the configuration.
   esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
   esp_mqtt_client_start(client); // Starts mqtt client with already created client handle.
   vTaskDelay(50 / portTICK_PERIOD_MS); // waiting 50 ms

 }

 /* handle cliente MQTT ************************************************/
 esp_mqtt_client_handle_t MQTT_getClient(void)
 {
         return client;
 }


/*****************************************************************************/
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
        //esp_mqtt_client_handle_t client = event->client;
        client = event->client;

        // your_context_t *context = event->context;
        switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
                ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
                break;

        case MQTT_EVENT_DISCONNECTED:
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
                break;

        case MQTT_EVENT_SUBSCRIBED:
                ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
                break;

        case MQTT_EVENT_UNSUBSCRIBED:
                ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
                break;

        case MQTT_EVENT_PUBLISHED:
                ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
                break;

        case MQTT_EVENT_DATA:   // Cuando recibo un mensaje a un topic que estoy subcripto.
                ESP_LOGI(TAG, "MQTT_EVENT_DATA");

                // obtener topic y mensaje recibido
                char rcv_topic[MAX_TOPIC_LENGTH]="";
                char rcv_message[MAX_MSG_LENGTH]="";
                strncpy(rcv_topic, event->topic, event->topic_len);
                strncpy(rcv_message, event->data, event->data_len);
                //ESP_LOGI(TAG, "TOPIC RECEIVED: %s", rcv_topic );
                //ESP_LOGI(TAG, "MESSAGE RECEIVED: %s", rcv_message);
                MQTT_processTopic(rcv_topic, rcv_message);
                break;

        case MQTT_EVENT_ERROR:
                ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
                break;

        default:
                ESP_LOGI(TAG, "Other event id:%d", event->event_id);
                break;
        }
        return ESP_OK;
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
        ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
        mqtt_event_handler_cb(event_data);
}
