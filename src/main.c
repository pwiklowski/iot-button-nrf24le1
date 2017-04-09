#include "reg24le1.h"
#include "gpio.h"
#include "rf.h"
#include "uart.h"
#include <stdio.h>
#include "pwr_clk_mgmt.h"
#include "adc.h"

#define SENDER
#define RECEIVER_


#define ONE_BUTTON  0

void init(){
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

	//Set up RF
	rf_configure(RF_CONFIG_EN_CRC | RF_CONFIG_PWR_UP,
				 true,													//don't care for TX
				 RF_EN_AA_ENAA_NONE,									//turn off auto-acknowledge on all pipes
				 RF_EN_RXADDR_ERX_P0,									//enable pipe 0 only
				 RF_SETUP_AW_5BYTES,									//5 byte address
				 RF_SETUP_RETR_DISABLE,									//turn off auto-retransmit
				 RF_RF_CH_DEFAULT_VAL,									//run at 2402 MHz
				 RF_RF_SETUP_RF_DR_250_KBPS | RF_RF_SETUP_RF_PWR_0_DBM,	//full power @ 1 Mbps
				 NULL,													//use default RX address for pipe 0
				 NULL,													//use default RX address for pipe 1
				 RF_RX_ADDR_P2_DEFAULT_VAL,								//use default RX address for pipe 2
				 RF_RX_ADDR_P3_DEFAULT_VAL,								//use default RX address for pipe 3
				 RF_RX_ADDR_P4_DEFAULT_VAL,								//use default RX address for pipe 4
				 RF_RX_ADDR_P5_DEFAULT_VAL,								//use default RX address for pipe 5
				 NULL,													//use default TX address for pipe 0
				 5,														//pipe 0 RX payload width (4 bytes)...doesn't really matter for a TX
				 RF_RX_PW_P1_DEFAULT_VAL,								//use default RX payload for pipe 1
				 RF_RX_PW_P2_DEFAULT_VAL,								//use default RX payload for pipe 2
				 RF_RX_PW_P3_DEFAULT_VAL,								//use default RX payload for pipe 3
				 RF_RX_PW_P4_DEFAULT_VAL,								//use default RX payload for pipe 4
				 RF_RX_PW_P5_DEFAULT_VAL,								//use default RX payload for pipe 5
				 RF_DYNPD_DEFAULT_VAL,									//use default dynamic payload settings
				 RF_FEATURE_DEFAULT_VAL);								//use default feature settings
	delay_us(130); //wait for remote unit to come from standby to RX

	//Set up button pins
	gpio_pin_configure(GPIO_PIN_ID_P1_5,
					   GPIO_PIN_CONFIG_OPTION_DIR_INPUT |
					   GPIO_PIN_CONFIG_OPTION_PIN_MODE_INPUT_BUFFER_ON_PULL_DOWN_RESISTOR);

	//Set up wake up on pin change configuration
	pwr_clk_mgmt_wakeup_pins_configure(PWR_CLK_MGMT_WAKEUP_CONFIG_OPTION_INPUT_P1_5_ENABLE);

	//Configure the system clock (primarly to make sure the 16 MHz crystal is turned off in register retention mode)
	pwr_clk_mgmt_cclk_configure(PWR_CLK_MGMT_CCLK_CONFIG_OPTION_CLK_FREQ_16_MHZ |
								PWR_CLK_MGMT_CCLK_CONFIG_OPTION_WKUP_INT_ON_XOSC16M_DISABLE |
								PWR_CLK_MGMT_CCLK_CONFIG_OPTION_START_XOSC16M_AND_RCOSC16M |
								PWR_CLK_MGMT_CCLK_CONFIG_OPTION_CLK_SRC_XOSC16M_OR_RCOSC16M |
								PWR_CLK_MGMT_CCLK_CONFIG_OPTION_XOSC16M_IN_REGISTER_RET_OFF);


	adc_configure (ADC_CONFIG_OPTION_RESOLUTION_8_BITS | ADC_CONFIG_OPTION_REF_SELECT_INT_1_2V);
}



void main(){
	uint16_t count = 0;
	uint8_t battery = 0;
	uint8_t data[5] = {ONE_BUTTON, 0, 0, 0, 1}; //device type, id, battery, eventdata

	init();
#ifdef RECEIVER
	 rf_set_as_rx(true); //change the device to an RX to get the character back from the other 24L01
	 while(1){

	 		if((rf_irq_pin_active() && rf_irq_rx_dr_active())) {
	 			rf_read_rx_payload(data, 5); //get the payload into data
		    for(count=0;count<5;count++) {
	 				printf("%d ", data[count]);
				}
	 			printf("\n");
	 			rf_irq_clear_all();
	 		}
	 }
#endif
#ifdef SENDER
	while(1){
		data[2] = adc_start_single_conversion_get_value(ADC_CHANNEL_1_THIRD_VDD) >> 8;
		data[3]++;
		 
		rf_write_tx_payload(data, 5, true);
    while(!(rf_irq_pin_active() && rf_irq_tx_ds_active()));

		gpio_pin_val_set(GPIO_PIN_ID_P1_4);
		delay_ms(30); 
		gpio_pin_val_clear(GPIO_PIN_ID_P1_4);
		delay_ms(30); 

    rf_irq_clear_all();
		rf_power_down();
	  pwr_clk_mgmt_enter_pwr_mode_register_ret(); //enter register retention mode for power savings
		rf_power_up(true);
	}	 
#endif
}

