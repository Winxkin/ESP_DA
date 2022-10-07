#include "uart.h"

uart_callback_t uart_callback = NULL;
static int BUF_SIZE = 1024;
static intr_handle_t handle_console;

static void IRAM_ATTR uart_intr_handle(void *arg)
{
    uart_callback();
}


void start_uart_init(uart_port_t uart_num)
{
    uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};
   ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0));
   ESP_ERROR_CHECK (uart_param_config(uart_num, &uart_config));
   ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

   //ESP_ERROR_CHECK(uart_isr_register(uart_num,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

}

void start_uart_init_config_pin(uart_port_t uart_num, int tx_io, int rx_io)
{
    uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};
   ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0));
   ESP_ERROR_CHECK (uart_param_config(uart_num, &uart_config));
   ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_io, rx_io,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));
   
  // ESP_ERROR_CHECK(uart_isr_free(uart_num));
  // ESP_ERROR_CHECK(uart_isr_register(uart_num,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));
  // ESP_ERROR_CHECK(uart_enable_rx_intr(uart_num));

}

void sentdata(uart_port_t uart_num,char* data)
{
    const int len = strlen(data);
    uart_write_bytes(uart_num, data, len);
}

void setcallback_uart(void *cb)
{
    uart_callback = cb;
}


void cleardata (uint8_t *data, int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        *data = 0;
        data++;
    }
}

// void checkdata(uint8_t *data,int length)
// {
//     if(data[0] == 0x44 && data[1] == 0x48)
//     {
//         uart_write_bytes(UART_NUM_0,data,length);
//     }
// }