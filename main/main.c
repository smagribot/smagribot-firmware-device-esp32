// #include <esp_event_loop.h>
// #include <esp_log.h>
// #include <esp_system.h>
// #include <esp_timer.h>
// // #include <nvs_flash.h>
// #include <sys/param.h>
// #include <string.h>

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #include "sdkconfig.h"

// #include "lwip/err.h"
// #include "lwip/sys.h"
// #include "sdkconfig.h"

// #include "esp_common.h"
// #include "azure_hub_client.h"
// #include "dht.h"

// /*! Number of measurement retries */
// static const int NUM_MEASUREMENT_RETRIES = 3;

// /*! GPIO port for DHT22 temperature and humidity sensor */
// static const gpio_num_t DHT_PORT = GPIO_NUM_25;

// static bool read_sensor_data(float &temperature, float &humidity)
// {
//     DHT dht;
//     dht.setDHTgpio(DHT_PORT);
//     int counter = 0;
//     int dht_status = DHT_TIMEOUT_ERROR;

//     while ((dht_status != DHT_OK) && (counter < NUM_SENSOR_READ_RETRIES))
//     {
//         vTaskDelay(DHT_DELAY_MS / portTICK_RATE_MS);
//         dht_status = dht.readDHT();
//         counter++;
//     }

//     temperature = dht.getTemperature();
//     humidity = dht.getHumidity();

//     return (dht_status == DHT_OK);
// }

// void azure_task(void *pvParameter)
// {
//     char device_message[128];
//     float temperature,
//         humidity;
//     uint32_t free_heap_sz,
//         min_free_heap_sz;

//     TickType_t msgdelay = ONE_SECOND_DELAY * 5;

//     IOTHUB_DEVICE_CLIENT_LL_HANDLE azure_client = init_iothub_client();
//     if (azure_client == NULL)
//     {
//         ESP_LOGE(TAG, "APP INFO: No Azure IoT Hub Handle was obtained");
//         vTaskDelete(NULL);
//         return;
//     }

//     while (1)
//     {
//         //Get Heap Info
//         free_heap_sz = esp_get_free_heap_size();
//         min_free_heap_sz = esp_get_minimum_free_heap_size();
//         ESP_LOGI(TAG, "Free Heap Size: %lu", (unsigned long)free_heap_sz);
//         ESP_LOGI(TAG, "Minimum Free Heap Size: %lu", (unsigned long)min_free_heap_sz);

//         read_sensor_data(&temperature, &humidity);

//         ESP_LOGI(TAG, "temperature: %.1f", temperature);
//         ESP_LOGI(TAG, "humidity: %.1f", humidity);

//         /* Create message to send to Azure IoT  - TEW*/
//         snprintf(device_message, sizeof(device_message),
//                  "{\"FreeHeap\": %lu,\"Temperature\": %.1f,\"Humidity\": %.1f}",
//                  (unsigned long)free_heap_sz, temperature,
//                  humidity);

//         //Send message to Azure IoT Hub
//         int sendresult = send_azure_msgs(device_message, azure_client);
//         if (sendresult == 0)
//         {
//             printf("APP INFO: Azure Message Sent Successfully\n");
//             ESP_LOGI(TAG, "SUCCESS: Message Queued to be sent to Azure");
//         }
//         else
//         {
//             printf("APP INFO: Error - Message not sent successfully\n");
//             ESP_LOGE(TAG, "ERROR: Message Not Queued for Azure");
//         }

//         //Process Any Messages
//         process_azure_msgs(azure_client);
//         vTaskDelay(ONE_SECOND_DELAY * 2);

//         //Delay for 5 seconds
//         vTaskDelay(msgdelay);
//     }

//     //Clean up Azure IoT Connection
//     destroy_iothub_client(azure_client);

//     vTaskDelete(NULL);
// }

// void app_main()
// {
//     /* Print chip information */
//     esp_chip_info_t chip_info;
//     esp_chip_info(&chip_info);
//     ESP_LOGI(TAG, "This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
//              chip_info.cores,
//              (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
//              (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

//     ESP_LOGI(TAG, "silicon revision %d, ", chip_info.revision);

//     ESP_LOGI(TAG, "%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
//              (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

//     // esp_err_t retvalue;
//     // retvalue = nvs_init();
//     // ESP_ERROR_CHECK(retvalue);

//     retvalue = wifi_init();
//     ESP_ERROR_CHECK(retvalue);

//     vTaskDelay(ONE_SECOND_DELAY);
//     ESP_LOGI(TAG, "ESP32 Board Initialized...");

//     //High Priority Task
//     taskResult = xTaskCreate(&azure_task, "azure_task", 1024 * 6, NULL, 5, NULL);
//     if (taskResult != pdPASS)
//     {
//         ESP_LOGE(TAG, "Create azure task failed");
//     }
// }

/* esp-azure example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "nvs_flash.h"
#include "iothub_client_sample_mqtt.h"

#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

EventGroupHandle_t wifi_event_group;

#ifndef BIT0
#define BIT0 (0x1 << 0)
#endif
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static const char *TAG = "azure";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP platform WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void azure_task(void *pvParameter)
{
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP success!");

    iothub_client_sample_mqtt_run();

    vTaskDelete(NULL);
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    initialise_wifi();

    if ( xTaskCreate(&azure_task, "azure_task", 1024 * 5, NULL, 5, NULL) != pdPASS ) {
        printf("create azure task failed\r\n");
    }

}

