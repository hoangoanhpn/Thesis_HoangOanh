/*
MIT License

Copyright (c) 2018 ShellAddicted <github.com/ShellAddicted>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "servoControl.h"

// #define servo_pin GPIO_NUM_15;
 servoControl myServo;
 void servo()
{
	
	

		myServo.write(90);
		vTaskDelay(1000 / portTICK_RATE_MS);



	
	

	while (1){
		for (int i = 0; i<180; i++){
			myServo.write(i);
			vTaskDelay(10 / portTICK_RATE_MS);
		}
		for (int i = 180; i>0; i--){
			myServo.write(i);
			vTaskDelay(10 / portTICK_RATE_MS);
		}
	}
	
}
extern "C" void app_main(){
	
	myServo.attach( GPIO_NUM_14 );
	//Defaults: myServo.attach(pin, 400, 2600, LEDC_CHANNEL_0, LEDC_TIMER0);
	// to use more servo set a valid ledc channel and timer
	
	servo();
}
