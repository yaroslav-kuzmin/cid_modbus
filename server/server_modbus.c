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
uint16_t port = 1502;
uint16_t id_server = 0;

uint16_t begin_registers = 0x1064;
uint16_t amount_registers = 0x100;

uint16_t re_state_lafet          = 0x1064;
uint16_t re_tic_vertical         = 0x1065;
uint16_t re_tic_horizontal       = 0x1066;
uint16_t re_encoder_vertical     = 0x1067;
uint16_t re_encoder_horizontal   = 0x1068;
uint16_t re_pressure             = 0x1069;
uint16_t re_amperage_vertical    = 0x106A;
uint16_t re_amperage_horizontal  = 0x106B;

uint16_t re_valva                = 0x106E;
uint16_t re_tic_valve            = 0x106F;

uint16_t re_sensor_fire_state    = 0x1072;
uint16_t re_fire_alarm_state     = 0x1074;

uint16_t re_control_lafet        = 0x10C8;
uint16_t re_speed_vertical       = 0x10C9;
uint16_t re_speed_horizontal     = 0x10CA;

uint16_t re_control_valve        = 0x10CC;
uint16_t re_control_tic_valve    = 0x10CD;

uint16_t re_control_device_robot = 0x10CF;

uint16_t re_name_robot           = 0x112C;
uint16_t re_liter_robot          = 0x112D;
uint16_t re_lafet                = 0x112E;
uint16_t re_sensor_fire          = 0x112F;
uint16_t re_gate                 = 0x1130;
uint16_t re_video                = 0x1131;
uint16_t re_fire_alarm           = 0x1132;
uint16_t re_device_robot         = 0x1133;

char STR_GROUP_CONNETC[] = "connect";
char STR_KEY_PORT[] = "port";
char STR_KEY_ID[] = "ID";

char STR_GROUP_REGISTERS[] = "registers";
char STR_KEY_NAME_ROBOT[] = "name_robot";
char STR_KEY_LITER_ROBOT[] = "liter_robot";
char STR_KEY_LAFET[] = "lafet";
char STR_KEY_SENSOR_FIRE[] = "sensor_fire";
char STR_KEY_GATE[] = "gate";
char STR_KEY_VIDEO[] = "video";
char STR_KEY_FIRE_ALARM[] = "fire_alarm";
char STR_KEY_DEVICE_ROBOT[] = "device_robot";

int init_config(char * name,modbus_mapping_t * mb)
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

	error = NULL;
	port = g_key_file_get_integer(config,STR_GROUP_CONNETC,STR_KEY_PORT,&error);
	if( error != NULL){
		g_printf("\nпорт : %d : %s\n",port,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tпорт : %d\n",port);

	error = NULL;
	id_server = g_key_file_get_integer(config,STR_GROUP_CONNETC,STR_KEY_ID,&error);
	if(error != NULL){
		g_printf("\nID : %d : %s\n",id_server,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tID : %d\n",id_server);

	error = NULL;
	reg = g_key_file_get_integer(config,STR_GROUP_REGISTERS,STR_KEY_NAME_ROBOT,&error);
	if(error != NULL){
		g_printf("\nназвание установки : %d : %s\n",reg,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tназвание установки : %d\n",reg);
	mb->tab_registers[re_name_robot] = reg;

	error = NULL;
	reg = g_key_file_get_integer(config,STR_GROUP_REGISTERS,STR_KEY_LITER_ROBOT,&error);
	if(error != NULL){
		g_printf("\nлитраж установки : %d : %s\n",reg,error->message);
		g_error_free(error);
		return FAILURE;
	}
	g_printf("\tлитраж установки : %d\n",reg);
	mb->tab_registers[re_liter_robot] = reg;

	error = NULL;
	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_LAFET,&error);
	if(error != NULL){
		g_printf("\nЛСД : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	g_printf("\tЛСД : %#x\n",reg);
	mb->tab_registers[re_lafet] = reg;

	error = NULL;
	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_SENSOR_FIRE,&error);
	if(error != NULL){
		g_printf("\nДатчик пламени : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	g_printf("\tДатчик пламени : %#x\n",reg);
	mb->tab_registers[re_sensor_fire] = reg;

	error = NULL;
	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_GATE,&error);
	if(error != NULL){
		g_printf("\nЗаслонка : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	g_printf("\tЗаслонка : %#x\n",reg);
	mb->tab_registers[re_gate] = reg;

	error = NULL;
	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_VIDEO,&error);
	if(error != NULL){
		g_printf("\nВидеокамера : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	g_printf("\tВидеокамера : %#x\n",reg);
	mb->tab_registers[re_video] = reg;

	error = NULL;
	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_FIRE_ALARM,&error);
	if(error != NULL){
		g_printf("\nПожарная сигнализация : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	g_printf("\tПожарная сигнализация : %#x\n",reg);
	mb->tab_registers[re_fire_alarm] = reg;

	error = NULL;
	str = g_key_file_get_string(config,STR_GROUP_REGISTERS,STR_KEY_DEVICE_ROBOT,&error);
	if(error != NULL){
		g_printf("\nИсполнительные устройства : %s\n",error->message);
		g_error_free(error);
		return FAILURE;
	}
	reg = g_ascii_strtoull(str,NULL,16);
	g_printf("\tИсполнительные устройства : %#x\n",reg);
	mb->tab_registers[re_device_robot] = reg;

	g_key_file_free(config);
	return SUCCESS;
}

int set_value_registers(modbus_mapping_t * mb)
{
	/*
	бит 0 - датчик положения "ВВЕРХ"
	бит 1 - датчик положения "ВНИЗ"
	бит 2 - датчик положения "ВЛЕВО"
	бит 3 - датчик положения "ВПРАВО"*/
	mb->tab_registers[re_state_lafet] = 0x000F;
	/*значение 0 - 30*/
	mb->tab_registers[re_tic_vertical] = 30;
	/*значение 0 - 60*/
	mb->tab_registers[re_tic_horizontal] = 60;
	/*значение 0-65535*/
	mb->tab_registers[re_encoder_vertical] = 2000;
	/*значение 0-65535*/
	mb->tab_registers[re_encoder_horizontal] = 3000;
	/*значение 0-65535*/
	mb->tab_registers[re_pressure] = 4000;
	/*значение 0-65535*/
	mb->tab_registers[re_amperage_vertical] = 5000;
	/*значение 0-65535*/
	mb->tab_registers[re_amperage_horizontal] = 6000;
	/*
	бит 0 - датчик состояния "ОТКРЫТ"
	бит 1 - датчик состояния "ЗАКРЫТ"*/
	mb->tab_registers[re_valva] = 3;
	/*значение 0-65535*/
	mb->tab_registers[re_tic_valve] = 7000;
	/*
	бит 0 - датчик в состоянии "ПОЖАР"
	бит 1 - датчик состояния "НЕИСПРАВНОСТЬ"*/
	mb->tab_registers[re_sensor_fire_state] = 3;
	/*
	бит 0 - пожар шлейфа №1
	бит 1 - пожар шлейфа №2
	бит 2 - пожар шлейфа №3
	бит 3 - пожар шлейфа №4
	бит 4 - пожар шлейфа №5
	бит 5 - пожар шлейфа №6
	бит 6 - пожар шлейфа №7
	бит 7 - пожар шлейфа №8
	бит 8 - пожар шлейфа №9
	бит 9 - пожар шлейфа №10
	бит 10 - пожар шлейфа №11
	бит 11 - пожар шлейфа №12
	бит 12 - пожар шлейфа №13
	бит 13 - пожар шлейфа №14
	бит 14 - пожар шлейфа №15
	бит 15 - пожар шлейфа №16*/
	mb->tab_registers[re_fire_alarm_state] = 0xFFFF;

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
