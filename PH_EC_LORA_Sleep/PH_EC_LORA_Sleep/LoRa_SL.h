#define Serial_modbus Serial1
void send_cb(void);
bool init_lora(double freq);
bool lora_send(int size_msg, uint8_t* msg);
