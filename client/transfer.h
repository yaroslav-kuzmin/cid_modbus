/*****************************************************************************/
/*                                                                           */
/*  cid_modbus  : модуль проверки соединения по протоколу ModBus             */
/*                                                                           */
/*  author: Kuzmin Yaroslav                                                  */
/*  email: kuzmin.yaroslav@gmail.com                                         */
/*  date : 10.08.15                                                          */
/*****************************************************************************/

#ifndef TRANSFER_H
#define TRANSFER_H

int get_status_connect(void);
int set_device_name(char * name);
int set_baud(int baud);
int set_parity(char parity);
int set_data_bit(int data_bit);
int set_stop_bit(int stop_bit);
int	set_slave_id(int id);
int set_modbus_debug(int debug);
int set_protocol(int protocol);

int disconnect_device(void);
int connect_device(void);

uint8_t * send_read_bits_device(int addr,int number);
uint8_t * send_read_input_bits_device(int addr,int number);
uint16_t * send_read_registers_device(int addr,int number);
uint16_t * send_read_input_registers_device(int addr,int number);
int send_write_bit_device(int addr,int status);
int send_write_register_device(int addr,int value);

#endif

