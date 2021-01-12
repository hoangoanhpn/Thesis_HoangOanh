#include "string.h"
#include <stdio.h>
#include "esp_system.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "main_functions.h"
#include "tensorflow/lite/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "model_data.h"
#include "model_operations.h"
#include "app_camera.h" 
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "servoControl.h"


int totalExpectedDataAmount = INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * INPUT_IMAGE_CHANNELS;

tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.

constexpr int kTensorArenaSize =  120*1024;
uint8_t tensor_arena[kTensorArenaSize];

// The name of this function is important for Arduino compatibility.

float *input_tro_dat_cham_f; //Luu lai dia chi cua input->data.f phong khi no thay doi giua hang 224 va 251
void setup()
{
    
    // Set up logging.
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    // Pepare the model.
    model = tflite::GetModel(fashion_mnist_model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report(
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Prepare graph model operations resolver.
    // Here I use resolver that loads all possible set of operations.
    // It will be better to use MicroOpResolver that loads
    // only really needed set of operations.
    static tflite::OpResolver &resolver = getFullMicroOpResolver();

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed.");
        return;
    }

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);
}

void doInference()
{
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        error_reporter->Report("Invoke failed.");
        return;
    }
}

#include "esp_timer.h"


static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
    char tmp[40];
    const char* ahihi = (const char*) data;
    for (int i=0; i< len; i++ )
    {
        itoa( (int)ahihi[i], tmp,10);
        sendData(tmp);
    }
    return len;
}

void jpg_httpd_handler(){
    camera_fb_t * fb = NULL;
    size_t fb_len = 0;
    int64_t fr_start = esp_timer_get_time();
    
    // Code chup hinh ne :)
    fb = esp_camera_fb_get();
    if (!fb) {
        sendData("Camera capture failed");
       // ESP_LOGE(TAG, "Camera capture failed");
        
        return ;
    }
    else
    {
      
    }
    
    if(fb->format == PIXFORMAT_JPEG)
    {
        fb_len = fb->len;
        char tmp[40];
        for (int i=0; i< fb->len; i++ )
        {
            itoa( (int)fb->buf[i], tmp,10);
            strcat( tmp, " ");
            sendData(tmp);
        }
        sendData("  nen ne\n");
        sendData("\n");
        doInference();
        sendBackPredictions(output);
    }
    else 
    {
        fb_len = fb->len;
        char tmp[40];
        for (int i=0; i< fb->len; i++ )
        {
            itoa( (int)fb->buf[i], tmp,10);
            strcat( tmp, " ");
            sendData(tmp);
        }
        sendData("\n");    
        doInference();sendData("hinh chua nen ne\n");
        sendBackPredictions(output);
        
    }
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
}

// code ultrasonic sensor
#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ultrasonic.h>
#include <iostream>

using namespace std;

#define MAX_DISTANCE_CM 500 // 5m max
#define TRIGGER_GPIO GPIO_NUM_12
#define ECHO_GPIO GPIO_NUM_13

#define trigger_taiche GPIO_NUM_15
#define echo_taiche GPIO_NUM_16

#define trigger_khongtaiche GPIO_NUM_2
#define echo_khongtaiche GPIO_NUM_4

uint32_t distance_;
void ultrasonic_test(void *pvParamters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };
    ultrasonic_init(&sensor);
    esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance_); 
    if (res != ESP_OK)
    {
        printf("Error: ");
        switch (res)
        {
            case ESP_ERR_ULTRASONIC_PING:
                cout<<("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                printf("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                printf("Echo timeout (i.e. distance_ too big)\n");
                break;
            default:
                printf("%d\n", res);
        }
    }
    else{
        // printf("Distance: %d cm\n", distance_);
    }
    vTaskDelay(860 / portTICK_PERIOD_MS);
}



uint32_t distance_taiche;
void ultrasonic_taiche()
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = trigger_taiche,
        .echo_pin = echo_taiche 
    };
    ultrasonic_init(&sensor);
    esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance_taiche);  
    if (res != ESP_OK)
    {
        printf("Error: ");
        switch (res)
        {
            case ESP_ERR_ULTRASONIC_PING:
                // cout<<("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                printf("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                printf("Echo timeout (i.e. distance_ too big)\n");
                break;
            default:
                printf("%d\n", res);
        }
    }
    else{
        printf("Distance: TAI CHE %d cm\n", distance_taiche);
    }
}

uint32_t distance_khongtaiche;
void ultrasonic_khongtaiche()
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = trigger_khongtaiche,
        .echo_pin = echo_khongtaiche 
    };

    ultrasonic_init(&sensor);    
    esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance_khongtaiche);
    if (res != ESP_OK)
    {
        printf("Error: ");
        switch (res)
        {
            case ESP_ERR_ULTRASONIC_PING:
                cout<<("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                printf("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                printf("Echo timeout (i.e. distance_ too big)\n");
                break;
            default:
                printf("%d\n", res);
        }
    }
    else{
        
        printf("Distance: KHONG TAI CHE %d cm\n", distance_khongtaiche);
    }
}

// Code dieu khien servo
servoControl myServo;
// Khong tai che
void servo_left()
{       
   for (int i = 86; i>=0; i--){
			myServo.write(i);
			vTaskDelay(10 / portTICK_RATE_MS);
		}
    for (int i = 0; i<=86; i++){
			myServo.write(i);
			vTaskDelay(10 / portTICK_RATE_MS);
		}
}
//Tai che
void servo_right()
{
     for (int i = 86; i<=180; i++){
			myServo.write(i);
			vTaskDelay(10 / portTICK_RATE_MS);
		}
     for (int i =180; i>=86; i--){
			myServo.write(i);
			vTaskDelay(10 / portTICK_RATE_MS);
		}
}
// Tim Max
int arg_max(float *array, int sl ){
    int kq= 0;
    for( int i=1; i <sl; i++){
        if(array[kq]< array[i]){
            kq =i;
        }
    }
    return kq;
}

int chup_hinh()
{
    camera_fb_t * fb = NULL;
    int64_t fr_start = esp_timer_get_time();
    // Code chup hinh ne :)
    fb = esp_camera_fb_get();
    if (!fb) {
        cout<<"Camera capture failed"<<endl;
        // ESP_LOGE(TAG, "Camera capture failed");
        
        return -1;
    }
    else
    {

    }

    input->data.f = input_tro_dat_cham_f;
    for(int i = 0; i < fb->len; i++){
        input->data.f[i] = fb->buf[i]/255.0f;
    }
    doInference(); //Goi tensorflow ra predict
    sendBackPredictions(output); // tra kq predict 
    esp_camera_fb_return(fb);
    int kq_predict = arg_max(output->data.f, output->dims->data[1]);
    char* label[]={"cardboard", "glass", "metal", "paper", "plastic", "trash"};
    cout<< label[kq_predict]<< endl;
    if( kq_predict== 5|| kq_predict== 3 ){
        return 0;
    }
    else
    {
        return 1;
    }
    vTaskDelay(100000 / portTICK_PERIOD_MS);
}

void classification(){
    while (true)
    {   
        ultrasonic_test(0);
        if( distance_ <14 && distance_ >=4 )
        {
            int check = chup_hinh();
            if( check ==1 )// Tai che
            {
                servo_right();
                ultrasonic_taiche();
               
            }
            else if (check ==0 ) // Khong tai che
            {
                servo_left();
                ultrasonic_khongtaiche();
            }
            esp_restart();
        }
    }
}


extern "C" void app_main(void)
{
	
    myServo.attach(GPIO_NUM_14,400,2600U,LEDC_CHANNEL_1,LEDC_TIMER_1);

    app_camera_init();
    initUart(UART_NUMBER);
    setup(); //Set up tensorflow
    input_tro_dat_cham_f = input->data.f;
    classification();
}
