#include "reg24le1.h"
#include "gpio.h"
#include "rf.h"
#include "uart.h"
#include <stdio.h>

void main(){
	gpio_pin_configure(GPIO_PIN_ID_P1_4, 
			GPIO_PIN_CONFIG_OPTION_DIR_OUTPUT |
			GPIO_PIN_CONFIG_OPTION_OUTPUT_VAL_CLEAR |
			GPIO_PIN_CONFIG_OPTION_PIN_MODE_OUTPUT_BUFFER_NORMAL_DRIVE_STRENGTH);

	gpio_pin_configure(GPIO_PIN_ID_FUNC_RXD,
			GPIO_PIN_CONFIG_OPTION_DIR_INPUT |
			GPIO_PIN_CONFIG_OPTION_PIN_MODE_INPUT_BUFFER_ON_NO_RESISTORS);

	gpio_pin_configure(GPIO_PIN_ID_FUNC_TXD,
			GPIO_PIN_CONFIG_OPTION_DIR_OUTPUT |
			GPIO_PIN_CONFIG_OPTION_OUTPUT_VAL_SET |
			GPIO_PIN_CONFIG_OPTION_PIN_MODE_OUTPUT_BUFFER_NORMAL_DRIVE_STRENGTH);

 	uart_configure_8_n_1_57600();

	while(1){
		putchar('a');
		printf("sdfsdf");

		gpio_pin_val_set(GPIO_PIN_ID_P1_4);;
		gpio_pin_val_set(GPIO_PIN_ID_P1_1);;
		delay_ms(100); 
		gpio_pin_val_clear(GPIO_PIN_ID_P1_4);
		gpio_pin_val_clear(GPIO_PIN_ID_P1_1);
		delay_ms(100); 
	}	 
}

