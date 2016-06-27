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
uint16_t re_work                 = 0x106C;

uint16_t re_valve                = 0x106E;
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


uint16_t tic_vertical = 0;
uint16_t tic_horizontal = 0;
uint16_t valve = 0;
uint16_t valve_delay = 0;
uint16_t pressure = 0;
uint16_t fire_alarm_state = 1;
uint16_t fire_alarm_state_count = 0;
uint16_t work = 0x0100;
uint16_t work_delay = 0;

int set_value_registers(modbus_mapping_t * mb)
{
 	/*
	бит 0 - датчик положения "ВВЕРХ"
	бит 1 - датчик положения "ВНИЗ"
	бит 2 - датчик положения "ВЛЕВО"
	бит 3 - датчик положения "ВПРАВО"*/
	mb->tab_registers[re_state_lafet] = 0x000F;
/*****************************************************************************/

	/*значение 0 - 30  (угол от 0 до 180 градусов) */
	tic_vertical ++;
	if( tic_vertical > 30){
		tic_vertical = 0;
	}
	mb->tab_registers[re_tic_vertical] = tic_vertical;
/*****************************************************************************/

	/*значение 0 - 60 (угол от 0 до 360 грудусав)*/
	tic_horizontal ++;
	if( tic_horizontal > 60){
		tic_horizontal =0;
	}
	mb->tab_registers[re_tic_horizontal] = tic_horizontal;
/*****************************************************************************/

	/*значение 0-65535*/
	mb->tab_registers[re_encoder_vertical] = 2000;
/*****************************************************************************/
	/*значение 0-65535*/
	mb->tab_registers[re_encoder_horizontal] = 3000;
/*****************************************************************************/

	/*значение 0-65535*/
	switch(pressure){
		case 0:
			pressure = 1;
			break;
		case 1:
			pressure = 2;
			break;
		case 2:
			pressure = 3;
			break;
		case 3:
			pressure = 0;
			break;
		default:
			pressure = 0;
	}
	mb->tab_registers[re_pressure] = pressure;
/*****************************************************************************/
	/*значение 0-65535*/
	mb->tab_registers[re_amperage_vertical] = 5000;
/*****************************************************************************/
	/*значение 0-65535*/
	mb->tab_registers[re_amperage_horizontal] = 6000;
/*****************************************************************************/
	valve_delay++;
	if(valve_delay == 10){
		valve_delay = 0;
	/*
	бит 0 - датчик состояния "ОТКРЫТ"
	бит 1 - датчик состояния "ЗАКРЫТ"
	бит 2 - датчик состояния "ОТКРЫВАЕТСЯ"
	бит 3 - датчик состояния "ЗАКРЫВАЕТСЯ"*/

		switch(valve){
			case 0:
				valve = 0x0001;
				break;
			case 0x0001:
				valve = 0x0008;
				break;
			case 0x0008:
				valve = 0x0002;
				break;
			case 0x0002:
				valve = 0x0004;
				break;
			case 0x0004:
				valve = 0x0001;
				break;
			default:
				valve = 0;
		}
	}
	mb->tab_registers[re_valve] = valve;
	/*значение 0-65535*/
/*****************************************************************************/
	mb->tab_registers[re_tic_valve] = 7000;
/*****************************************************************************/
	/*
	бит 0 - датчик в состоянии "ПОЖАР"
	бит 1 - датчик состояния "НЕИСПРАВНОСТЬ"*/
	mb->tab_registers[re_sensor_fire_state] = 3;
/*****************************************************************************/
	/*
	0 - нет пожар
	1 - пожар
	*/
	fire_alarm_state_count ++;
	if(fire_alarm_state_count == 20){
		fire_alarm_state_count = 0;
		if(	fire_alarm_state == 0){
			fire_alarm_state = 1;
		}
		else{
			fire_alarm_state = 0;
		}

	}
	/*0x1074*/
	mb->tab_registers[re_fire_alarm_state] = fire_alarm_state;
/*****************************************************************************/

	work_delay++;
	if(work_delay == 20){
		work_delay = 0;
/*
	 бит 0 - ошибка вертикальной оси
	 бит 1 - ошибка горизонтальной оси
	 бит 2 - ошибка привода актуатора 1
	 бит 3 - ошибка привода актуатора 2
	 бит 4 - ошибка привода заслонки
	 бит 5 - резерв
	 бит 6 - резерв
	 бит 7 - резерв
	 бит 8 - автоматический режим работы
	 бит 9 - ручной режим работы (управление местное)
	 бит 10 - ручной режим работы (управление оператора)
	 бит 11 - режим тестирования
	 бит 12 - режим  горизонтальной осцилляции
	 бит 13 - режим вертикальной осцилляции
	 бит 14 - режим осцилляции SAW
	 бит 15 - режим осцилляции STEP
	 0 - датчик OFF, 1 - датчик ON
 */
	/*0x106C*/
		switch(work){
			case 0x0100:
				work = 0x0101;
				break;
			case 0x0101:
				work = 0x0102;
				break;
			case 0x0102:
				work = 0x0104;
				break;
			case 0x0104:
				work = 0x0108;
				break;
			case 0x0108:
				work = 0x0110;
				break;
#if 0
			case 0x0110:
				work = 0x011f;
				break;
			case 0x011f:
				work = 0x011A;
				break;
			case 0x011A:
				work = 0x0119;
				break;
			case 0x0119:
				work = 0x011D;
				break;
			case 0x011D:
				work = 0x0200;
				break;
			case 0x0200:
				work = 0x0201;
				break;
			case 0x0201:
				work = 0x0202;
				break;
			case 0x0202:
				work = 0x0204;
				break;
			case 0x0204:
				work = 0x0208;
				break;
			case 0x0208:
				work = 0x0210;
				break;
			case 0x0210:
				work = 0x021f;
				break;
			case 0x021f:
				work = 0x1100;
				break;
			case 0x1100:
				work = 0x2100;
				break;
			case 0x2100:
				work = 0x4100;
				break;
			case 0x4100:
				work = 0x8100;
				break;
			case 0x8100:
				work = 0x0100;
				break;
#endif
			default:
				work = 0x0100;
		}
	}
	mb->tab_registers[re_work] = work;


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
