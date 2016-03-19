/*****************************************************************************/
/*                                                                           */
/*  cid_modbus  : модуль проверки соединения по протоколу ModBus             */
/*                                                                           */
/*  author: Kuzmin Yaroslav                                                  */
/*  email: kuzmin.yaroslav@gmail.com                                         */
/*  date : 10.08.15                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Дополнительные файлы                                                      */
/*****************************************************************************/
#include <glib.h>

#include <modbus.h>
#include <total.h>

/*****************************************************************************/
/* Глобальные переменые                                                      */
/*****************************************************************************/
int status_connect = DISCONNECT;

char * device_name = NULL;
int baud = 9600;
char parity = 'N';
int data_bit = 8;
int stop_bit = 1;
int slave_id = 1;
int modbus_debug = FALSE;
int protocol = PROTOCOL_RTU;
/*****************************************************************************/
/* Вспомогательные функция                                                   */
/*****************************************************************************/

/*****************************************************************************/
/* Основная функция                                                          */
/*****************************************************************************/
int get_status_connect(void)
{
	return status_connect;
}
int set_device_name(char * name)
{
	if(device_name != NULL){
		g_free(device_name);
	}
	device_name = name;
	g_message("device : %s",device_name);
	return 0;
}
int set_baud(int b)
{
	baud = b;
	g_message("baud : %d",baud);
	return 0;
}
int set_parity(char p)
{
	switch(p){
		case 'N':
		case 'E':
		case 'O':{
			parity = p;
			break;
						 }
		default:
			parity = 'N';
	}
	g_message("parity : %c",parity);
	return 0;
}
int set_data_bit(int db)
{
	if((db >= 5) && (db <= 8) ){
		data_bit = db;
	}
	g_message("data bit : %d",data_bit);
	return 0;
}
int set_stop_bit(int sb)
{
	if( (sb >= 1) && (sb <= 2)){
		stop_bit = sb;
	}
	g_message("stop bit : %d",stop_bit);
	return 0;
}
int	set_slave_id(int id)
{
	if((id > 0) && (id < 248)){
		slave_id = id;
	}
	g_message("slave ID : %d",slave_id);
	return 0;
}
int set_modbus_debug(int d)
{
	if(d == FALSE){
		modbus_debug = FALSE;
	}
	else{
		modbus_debug = TRUE;
	}
	g_message("modbus debug : %d",modbus_debug);
	return 0;
}

char STR_PROTOCOL_RTU[] = "rtu";
char STR_PROTOCOL_ASCII[] = "ascii";

int set_protocol(int p)
{
	char * str = STR_PROTOCOL_RTU;
	protocol = PROTOCOL_RTU;
	switch(p){
		case PROTOCOL_RTU:{
			protocol = p;
			str = STR_PROTOCOL_RTU;
			break;
		}
		case PROTOCOL_ASCII:{
			protocol = p;
			str = STR_PROTOCOL_ASCII;
			break;
		}
	}
	g_message("protocol : %s",str);
	return SUCCESS;
}

modbus_t *ctx = NULL;
int header_length = 0;

int disconnect_device(void)
{
	status_connect = DISCONNECT;
	modbus_free(ctx);
	g_message("diconnect : %s",device_name);
	return DISCONNECT;
}

int connect_device(void)
{
	int rc = 0;

	if(status_connect == CONNECT){
		g_message("status connect");
		return CONNECT;
	}

	if(device_name == NULL){
		g_message("Not device");
		return DISCONNECT;
	}
	if(protocol == PROTOCOL_ASCII){
		ctx = modbus_new_rtu(device_name,baud,parity,data_bit,stop_bit);
	}else{
		ctx = modbus_new_rtu(device_name,baud,parity,data_bit,stop_bit);
	}

	if(ctx == NULL){
		g_message("Not alloc modbus protocol");
	 	return DISCONNECT;
	}

	modbus_set_debug(ctx,modbus_debug);
	/*MODBUS_ERROR_RECOVERY_NONE
	  MODBUS_ERROR_RECOVERY_LINK
	  MODBUS_ERROR_RECOVERY_PROTOCOL*/
	modbus_set_error_recovery(ctx,MODBUS_ERROR_RECOVERY_NONE);

	modbus_set_slave(ctx, slave_id);

	rc = modbus_connect(ctx);
	if(rc == -1){
		g_message("Not connect : %s",device_name);
		return DISCONNECT;
	}

	status_connect = CONNECT;
	g_message("connect : %s",device_name);
	return CONNECT;
}

uint8_t * tab_rp_bits = NULL;
gsize size_tab_rp_bits = 0;

uint8_t * send_read_bits_device(int addr,int number)
{
	/*int i;*/
	int rc = 0;
	if(ctx == NULL){
		g_message("Not connect");
		return NULL;
	}

	if(number > size_tab_rp_bits){
		if(tab_rp_bits != NULL){
			g_free(tab_rp_bits);
		}
		size_tab_rp_bits = number;
		tab_rp_bits = g_malloc0(size_tab_rp_bits*sizeof(uint8_t));
	}
/*
	else{
		memset(tab_rp_bits,0,(size_tab_rp_bits* sizeof(uint8_t)));
	}
*/
	rc = modbus_read_bits(ctx,addr,number,tab_rp_bits);
	if(rc == -1){
		g_message("error read bits");
		return NULL;
	}
/*
	g_message("read bits : addr %d : number %d",addr,number);
	g_message("read bits : get byte :> %d",rc);
	for(i = 0;i < rc;i++)
		g_message("read bits : %#x",tab_rp_bits[i]);
*/
	return tab_rp_bits;
}

uint8_t * send_read_input_bits_device(int addr,int number)
{
	int rc = 0;
	if(ctx == NULL){
		g_message("Not connect");
		return NULL;
	}
	if(number > size_tab_rp_bits){
		if(tab_rp_bits != NULL){
			g_free(tab_rp_bits);
		}
		size_tab_rp_bits = number;
		tab_rp_bits = g_malloc0(size_tab_rp_bits*sizeof(uint8_t));
	}
/*
	else{
		memset(tab_rp_bits,0,(size_tab_rp_bits* sizeof(uint8_t)));
	}
*/
	rc = modbus_read_input_bits(ctx,addr,number,tab_rp_bits);
	if(rc == -1){
		g_message("error read input bits");
	 	return NULL;
	}
 	return tab_rp_bits;
}

uint16_t * tab_rp_registers = NULL;
gsize size_tab_rp_registers = 0;

uint16_t * send_read_registers_device(int addr,int number)
{
	int rc = 0;
	if(ctx == NULL){
		g_message("Not connect");
		return NULL;
	}
	if(number > size_tab_rp_registers){
		if(tab_rp_registers != NULL){
			g_free(tab_rp_registers);
		}
		size_tab_rp_registers = number;
		tab_rp_registers = g_malloc0(size_tab_rp_registers*sizeof(uint16_t));
	}
/*
	else{
		memset(tab_rp_registers,0,(size_tab_rp_registers* sizeof(uint16_t)));
	}
*/
	rc = modbus_read_registers(ctx,addr,number,tab_rp_registers);
	if(rc == -1){
		g_message("error read registers");
		return NULL;
	}
	return tab_rp_registers;
}

uint16_t * send_read_input_registers_device(int addr,int number)
{
	int rc = 0;
	if(ctx == NULL){
		g_message("Not connect");
		return NULL;
	}
	if(number > size_tab_rp_registers){
		if(tab_rp_registers != NULL){
			g_free(tab_rp_registers);
		}
		size_tab_rp_registers = number;
		tab_rp_registers = g_malloc0(size_tab_rp_registers*sizeof(uint16_t));
	}
/*
	else{
		memset(tab_rp_registers,0,(size_tab_rp_registers* sizeof(uint16_t)));
	}
*/
	rc = modbus_read_input_registers(ctx,addr,number,tab_rp_registers);
	if(rc == -1){
		g_message("error read input registers");
		return NULL;
	}
	return tab_rp_registers;
}

int send_write_bit_device(int addr,int status)
{
	int rc = 0;
	if(ctx == NULL){
		g_message("Not connect");
		return -1;
	}
	if(status > 0){
		status = TRUE;
	}
	else{
		status = FALSE;
	}
	rc = modbus_write_bit(ctx,addr,status);
	if(rc == -1){
		g_message("error write bits");
	}
	return rc;
}

int send_write_register_device(int addr,int value)
{
	int rc = 0;
	if(ctx == NULL){
		g_message("Not connect");
		return -1;
	}
	rc = modbus_write_register(ctx,addr,value);
	if(rc == -1){
		g_message("error write registers");
	}
	return rc;
}
/*****************************************************************************/
