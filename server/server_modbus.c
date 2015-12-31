/*****************************************************************************/
/*                                                                           */
/*  cid_modbus  : модуль проверки соединения по протоколу ModBus             */
/*                                                                           */
/*  author: Kuzmin Yaroslav                                                  */
/*  email: kuzmin.yaroslav@gmail.com                                         */
/*  date : 31.12.15                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
#include <glib.h>
#include <glib/gprintf.h>

#include <modbus.h>
#include <total.h>


char ip_address[] = "127.0.0.1";
uint16_t port = 502;
uint16_t id_server = 0;


uint16_t begin_registers = 0x100;
uint16_t amount_registers = 32;

int main(int argc,char * argv[])
{
	int rc;
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;
	uint8_t * query;
	uint16_t registers = 0;
	/*int header_length;*/

	if(argc != 2){
		g_printf("Нет номера сервера!\n");
		return SUCCESS;
	}

	id_server = g_ascii_strtoll(argv[1],NULL,10);
	if(id_server == 0){
		g_printf("Некорректный номер сервера : %s\n",argv[1]);
		return SUCCESS;
	}

	ctx = modbus_new_tcp(ip_address, port);
	query = g_malloc0(MODBUS_TCP_MAX_ADU_LENGTH);
	/*загаловок разный у TCP и RTU*/
	/*header_length = modbus_get_header_length(ctx);*/
	modbus_set_debug(ctx,FALSE);
	mb_mapping = modbus_mapping_new(0,0,begin_registers + amount_registers,0);

	g_printf(" Старт сервера : ID %d\n",id_server);

	socket = modbus_tcp_listen(ctx, 1);

	modbus_tcp_accept(ctx, &socket);
	g_printf(" Клиент подключился к серверу!");
	registers = 0;
	for(;;){
		rc = modbus_receive(ctx, query);
		if(rc == -1){
			g_printf("Клиент закрыл соединение!\n");
			break;
		}
		registers ++;
		mb_mapping->tab_registers[0x100] = registers;
		rc = modbus_reply(ctx, query, rc, mb_mapping);
		if (rc == -1) {
			g_printf("Клиент закрыл соединение!\n");
			break;
		}
	}

	modbus_mapping_free(mb_mapping);
	g_free(query);
	modbus_free(ctx);
	g_printf("\n Стоп сервера : ID %d\n",id_server);

	return SUCCESS;
}
/*****************************************************************************/
