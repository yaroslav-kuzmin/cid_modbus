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


static char ip_address[] = "127.0.0.1";
static uint16_t port = 1502;
static uint16_t id_server = 0;

static uint16_t begin_registers = 0x1064;
static uint16_t amount_registers = 0x100;

static uint16_t re_name_robot  = 0x112C;
static uint16_t re_liter_robot = 0x112D;
static uint16_t re_lafet       = 0x112E;
static uint16_t re_sensor_fire = 0x112F;
static uint16_t re_gate        = 0x1130;
static uint16_t re_video       = 0x1131;
static uint16_t re_fire_alarm  = 0x1132;
static uint16_t re_device_robot  = 0x1133;

static char STR_GROUP_CONNETC[] = "connect";
static char STR_KEY_PORT[] = "port";
static char STR_KEY_ID[] = "ID";

static char STR_GROUP_REGISTERS[] = "registers";
static char STR_KEY_NAME_ROBOT[] = "name_robot";
static char STR_KEY_LITER_ROBOT[] = "liter_robot";
static char STR_KEY_LAFET[] = "lafet";
static char STR_KEY_SENSOR_FIRE[] = "sensor_fire";
static char STR_KEY_GATE[] = "gate";
static char STR_KEY_VIDEO[] = "video";
static char STR_KEY_FIRE_ALARM[] = "fire_alarm";
static char STR_KEY_DEVICE_ROBOT[] = "device_robot";

static int init_config(char * name,modbus_mapping_t * mb)
{
	int rc;
	GError * error = NULL;
	GKeyFile * config = g_key_file_new();
	uint16_t reg;
	char * str;

	rc = g_key_file_load_from_file(config,name,G_KEY_FILE_NONE,&error);
	if(rc == FALSE){
		g_printf("\n%s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("Файл конфигурации : %s\n",name);

	port = g_key_file_get_integer(config,STR_GROUP_CONNETC,STR_KEY_PORT,&error);
	if(port == 0){
		g_printf("\nпорт : %d : %s\n",port,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tпорт : %d\n",port);

	id_server = g_key_file_get_integer(config,STR_GROUP_CONNETC,STR_KEY_ID,&error);
	if(id_server == 0){
		g_printf("\nID : %d : %s\n",id_server,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tID : %d\n",id_server);

	reg = g_key_file_get_integer(config,STR_GROUP_REGISTERS,STR_KEY_NAME_ROBOT,&error);
	if(reg == 0){
		g_printf("\nназвание установки : %d : %s\n",reg,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tназвание установки : %d\n",reg);
	mb->tab_registers[re_name_robot] = reg;

	reg = g_key_file_get_integer(config,STR_GROUP_REGISTERS,STR_KEY_LITER_ROBOT,&error);
	if(reg == 0){
		g_printf("\nлитраж установки : %d : %s\n",reg,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tлитраж установки : %d\n",reg);
	mb->tab_registers[re_liter_robot] = reg;

	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_LAFET,&error);
	if(str == NULL){
		g_printf("\nЛСД : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	if(reg == 0){
		g_printf("\nЛСД : %d\n",reg);
		return FAILURE;
	}
	g_printf("\tЛСД : %#x\n",reg);
	mb->tab_registers[re_lafet] = reg;

	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_SENSOR_FIRE,&error);
	if(str == NULL){
		g_printf("\nДатчик пламени : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	if(reg == 0){
		g_printf("\nДатчик пламени : %d\n",reg);
		return FAILURE;
	}
	g_printf("\tДатчик пламени : %#x\n",reg);
	mb->tab_registers[re_sensor_fire] = reg;

	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_GATE,&error);
	if(str == NULL){
		g_printf("\nЗаслонка : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	if(reg == 0){
		g_printf("\nЗаслонка : %d\n",reg);
		return FAILURE;
	}
	g_printf("\tЗаслонка : %#x\n",reg);
	mb->tab_registers[re_gate] = reg;

	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_VIDEO,&error);
	if(str == NULL){
		g_printf("\nВидеокамера : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	if(reg == 0){
		g_printf("\nВидеокамера : %d\n",reg);
		return FAILURE;
	}
	g_printf("\tВидеокамера : %#x\n",reg);
	mb->tab_registers[re_video] = reg;

	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_FIRE_ALARM,&error);
	if(str == NULL){
		g_printf("\nПожарная сигнализация : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	if(reg == 0){
		g_printf("\nПожарная сигнализация : %d\n",reg);
		return FAILURE;
	}
	g_printf("\tПожарная сигнализация : %#x\n",reg);
	mb->tab_registers[re_fire_alarm] = reg;

	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_DEVICE_ROBOT,&error);
	if(str == NULL){
		g_printf("\nИсполнительные устройства : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	if(reg == 0){
		g_printf("\nИсполнительный устройства : %d\n",reg);
		return FAILURE;
	}
	g_printf("\tИсполнительные устройства : %#x\n",reg);
	mb->tab_registers[re_device_robot] = reg;

	g_key_file_free(config);
	return SUCCESS;
}

static int set_value_registers(modbus_mapping_t * mb)
{
	return SUCCESS;
}

int main(int argc,char * argv[])
{
	int rc;
	char * name_file;
	int socket;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;
	uint8_t * query;

	if(argc != 2){
		g_printf("Нет имени конфигурационного файла!\n\n");
		return SUCCESS;
	}
	name_file = argv[1];

	mb_mapping = modbus_mapping_new(0,0,begin_registers + amount_registers,0);
	if(mb_mapping == NULL){
		return SUCCESS;
	}
	rc = init_config(name_file,mb_mapping);
	if(rc == FAILURE){
		g_printf("Некорректный файл конфигурации!\n\n");
		return SUCCESS;
	}

	/*return SUCCESS;*/

	ctx = modbus_new_tcp(ip_address, port);
	query = g_malloc0(MODBUS_TCP_MAX_ADU_LENGTH);
	/*загаловок разный у TCP и RTU*/
	/*header_length = modbus_get_header_length(ctx);*/
	modbus_set_debug(ctx,FALSE);

	g_printf(" Старт сервера : port %d : ID %d\n",port,id_server);

	socket = modbus_tcp_listen(ctx, 1);
	if(socket == -1){
		g_printf("Несмог создать соединение!\n\n");
		return SUCCESS;
	}

	modbus_tcp_accept(ctx, &socket);
	g_printf(" Клиент подключился к серверу!\n");
	for(;;){
		rc = modbus_receive(ctx, query);
		if(rc == -1){
			g_printf("Клиент закрыл соединение!\n\n");
			break;
		}

		set_value_registers(mb_mapping);

		rc = modbus_reply(ctx,query,rc,mb_mapping);
		if (rc == -1) {
			g_printf(" Клиент закрыл соединение!\n\n");
			break;
		}
	}

	modbus_mapping_free(mb_mapping);
	g_free(query);
	modbus_free(ctx);
	g_printf(" Стоп сервера : ID %d\n\n",id_server);

	return SUCCESS;
}
/*****************************************************************************/
