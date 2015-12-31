/*****************************************************************************/
/*                                                                           */
/*  cid_modbus  : модуль проверки соединения по протоколу ModBus             */
/*                                                                           */
/*  author: Kuzmin Yaroslav                                                  */
/*  email: kuzmin.yaroslav@gmail.com                                         */
/*  date : 10.08.15                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Дополнительные файлы                                                      */
/*****************************************************************************/
#include <stdlib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#include <stdint.h>

#include <total.h>
#include "transfer.h"
/*****************************************************************************/
/* Глобальные переменые                                                      */
/*****************************************************************************/
char STR_TITEL[] = "Control Industrial Device";

GError * check = NULL;
char LOG_FILE[] = "modbus.log";

GIOChannel * log_file = NULL;

char DEFAULT_INI_FILE[] = "modbus.ini";
GtkEntryBuffer * ini_file_name = NULL;
GKeyFile * ini_file = NULL;

/*****************************************************************************/
/* Вспомогательные функция                                                   */
/*****************************************************************************/

GTimeVal current_time;
char STR_CURRENT_TIME[] = "\n[ 00.00.0000 00:00:00 ] ";
#define LEN_STR_CURRENT_TIME    25

void save_file(const gchar *log_domain,GLogLevelFlags log_level,
             const gchar *message,gpointer user_data)
{
	GDateTime * p_dt;
	GIOStatus rc;
	gsize bw;
	if(log_file != NULL){
		g_get_current_time (&current_time);
		p_dt = g_date_time_new_from_timeval_local(&current_time);
		g_sprintf(STR_CURRENT_TIME,"\n[ %02d.%02d.%04d %02d:%02d:%02d ] "
		         ,g_date_time_get_day_of_month(p_dt)
		         ,g_date_time_get_month(p_dt)
		         ,g_date_time_get_year(p_dt)
		         ,g_date_time_get_hour(p_dt)
		         ,g_date_time_get_minute(p_dt)
		         ,g_date_time_get_second(p_dt));
		check = NULL;
		rc = g_io_channel_write_chars(log_file
				,STR_CURRENT_TIME,LEN_STR_CURRENT_TIME
				,&bw,&check);
		rc = g_io_channel_write_chars(log_file
		,message,-1,&bw,&check);
		if(rc != G_IO_STATUS_NORMAL){
		 	/*TODO*/;
			g_error_free(check);
		}

	}
}

static void startup_app(GtkApplication * app,gpointer data)
{
	check = NULL;
	log_file = g_io_channel_new_file (LOG_FILE,"a",&check);
	if(log_file == NULL){
		g_message("Not open log file : %s : %s",LOG_FILE,check->message);
		exit(0);
	}
	g_log_set_default_handler(save_file,NULL);
	g_message("Startup!");
}

static void shutdown_app(GtkApplication * app,gpointer data)
{
	check = NULL;
	g_key_file_free(ini_file);
	g_message("Shutdown!");
	g_io_channel_shutdown(log_file,TRUE,&check);
}

GtkWidget * ini_name_device = NULL;
GtkWidget * ini_baud = NULL;
GtkWidget * ini_parity = NULL;
GtkWidget * ini_data_bit = NULL;
GtkWidget * ini_stop_bit = NULL;
GtkWidget * ini_slave_id = NULL;
GtkWidget * ini_debug = NULL;
GtkWidget * ini_protocol = NULL;

GtkWidget * con_status = NULL;

char GROUP_GLOBAL[] = "global";
char GROUP_MASTER[] = "master";
char GROUP_CLIENT[] = "slave";

void ini_load(GtkButton * button,gpointer user_data)
{
	int rc;
	GtkEntryBuffer * entry_buff = (GtkEntryBuffer*)user_data;
	const char * name = gtk_entry_buffer_get_text(entry_buff);
	char *str = NULL;
	int value;
	char str_value[] = "0000000000";

	g_message("ini file : %s",name);
	ini_file = g_key_file_new();
	check = NULL;
	rc = g_key_file_load_from_file(ini_file,name,G_KEY_FILE_NONE,&check);
	if(rc == FALSE){
		g_message("Not open ini file : %s : %s",name,check->message);
		g_error_free(check);
		return ;
	}
	g_message("Open ini file : %s",name);
	check = NULL;
	str = g_key_file_get_string (ini_file,GROUP_GLOBAL,"device",&check);
	if(str == NULL){
		g_message("Not key device : %s",check->message);
		g_error_free(check);
		return ;
	}
	set_device_name(str);
	gtk_label_set_text(GTK_LABEL(ini_name_device),str);

	check = NULL;
	value = g_key_file_get_integer(ini_file,GROUP_GLOBAL,"baud",&check);
	if(value == 0){
		g_message("Not key baud : %s",check->message);
		g_error_free(check);
	}
	else{
		set_baud(value);
		g_sprintf(str_value,"%d",value);
		gtk_label_set_text(GTK_LABEL(ini_baud),str_value);
	}

	check = NULL;
	str = g_key_file_get_string(ini_file,GROUP_GLOBAL,"parity",&check);
	if(str == NULL){
		g_message("Not key parity : %s",check->message);
		g_error_free(check);
	}
	else{
		set_parity(str[0]);
		gtk_label_set_text(GTK_LABEL(ini_parity),str);
	}

	check = NULL;
	value = g_key_file_get_integer(ini_file,GROUP_GLOBAL,"data_bit",&check);
	if(value == 0){
		g_message("Not key data bit : %s",check->message);
		g_error_free(check);
	}
	else{
		set_data_bit(value);
		g_sprintf(str_value,"%d",value);
		gtk_label_set_text(GTK_LABEL(ini_data_bit),str_value);
	}

	check = NULL;
	value = g_key_file_get_integer(ini_file,GROUP_GLOBAL,"stop_bit",&check);
	if(value == 0){
		g_message("Not key stop bit : %s",check->message);
		g_error_free(check);
	}
	else{
		set_stop_bit(value);
		g_sprintf(str_value,"%d",value);
		gtk_label_set_text(GTK_LABEL(ini_stop_bit),str_value);
	}

	check = NULL;
	value = g_key_file_get_integer(ini_file,GROUP_CLIENT,"ID",&check);
	if(value == 0){
		g_message("Not key slave ID : %s",check->message);
		g_error_free(check);
	}
	else{
		set_slave_id(value);
		g_sprintf(str_value,"%d",value);
		gtk_label_set_text(GTK_LABEL(ini_slave_id),str_value);
	}

	check = NULL;
	str = g_key_file_get_string(ini_file,GROUP_MASTER,"debug",&check);
	if(str == NULL){
		g_message("Not key master debug : %s",check->message);
		g_error_free(check);
	}
	else{
		if(!g_strcmp0(str,"TRUE")){
			value = TRUE;
		}
		else{
			value = FALSE;
		}
		set_modbus_debug(value);
		gtk_label_set_text(GTK_LABEL(ini_debug),str);
	}

	check = NULL;
	str = g_key_file_get_string(ini_file,GROUP_GLOBAL,"protocol",&check);
	if(str == NULL){
		g_message("Not key protocol : %s",check->message);
		g_error_free(check);
	}
	else{
		if(!g_strcmp0(str,"ascii")){
			value = PROTOCOL_ASCII;
		}
		else{
			value = PROTOCOL_RTU;
		}
		set_protocol(value);
		gtk_label_set_text(GTK_LABEL(ini_protocol),str);
	}
	return ;
}

void con_device(GtkButton * button,gpointer user_data)
{
	int rc;
	if(get_status_connect() == DISCONNECT){
		rc = connect_device();
		if(rc == CONNECT ){
			gtk_label_set_text(GTK_LABEL(con_status),"connect");
			gtk_button_set_label(button,"disconnect");
		}
	}
	else{
		rc = disconnect_device();
		if(rc == DISCONNECT){
			gtk_label_set_text(GTK_LABEL(con_status),"not connect");
			gtk_button_set_label(button,"connect");
		}
 	}
}

GtkEntryBuffer * send_read_bits_addr_buff = NULL;
GtkEntryBuffer * send_read_bits_number_buff = NULL;
GtkEntryBuffer * send_read_bits_answer_buff = NULL;

void send_read_bits(GtkButton * button,gpointer user_data)
{
	guint64 addr;
	guint64 number = 0;
	uint8_t * answer = NULL;
	gchar temp[] = "0 ";
	int i;

	if(get_status_connect() == DISCONNECT){
		g_message("Not connect");
		return ;
	}

/*	g_message("addr :> %s",gtk_entry_buffer_get_text(send_read_bits_addr_buff));*/
	addr = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_bits_addr_buff),NULL,16);
	if(addr > 0xFFFF ){
		g_message("Not correct address : %#lx",(long unsigned int)addr);
		gtk_entry_buffer_set_text(send_read_bits_answer_buff,"not correct addres",-1);
		return;
	}
/*	g_message("data :> %s",gtk_entry_buffer_get_text(send_read_bits_number_buff));*/
	number = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_bits_number_buff),NULL,16);
	if(number > 0xFFFF){
		g_message("Not correct number : %#lx",(long unsigned int)number);
		gtk_entry_buffer_set_text(send_read_bits_answer_buff,"not correct number",-1);
		return;
	}
	answer = send_read_bits_device(addr,number);
	gtk_entry_buffer_set_text(send_read_bits_answer_buff," ",-1);

	if(answer == NULL){
		return ;
	}

	for(i=0;i < number;i++){
		if(answer[i] == TRUE){
			temp[0] = '1';
		}
		else{
			temp[0] = '0';
		}
		gtk_entry_buffer_insert_text(send_read_bits_answer_buff,(i*2),temp,2);
	}
	return ;
}

GtkEntryBuffer * send_read_input_bits_addr_buff = NULL;
GtkEntryBuffer * send_read_input_bits_number_buff = NULL;
GtkEntryBuffer * send_read_input_bits_answer_buff = NULL;

void send_read_input_bits(GtkButton * button,gpointer user_data)
{
	guint64 addr;
	guint64 number = 0;
	uint8_t * answer = NULL;
	gchar temp[] = "0 ";
	int i;

	if(get_status_connect() == DISCONNECT){
		g_message("Not connect");
		return ;
	}

/*	g_message("addr :> %s",gtk_entry_buffer_get_text(send_read_input_bits_addr_buff));*/
	addr = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_input_bits_addr_buff),NULL,16);
	if(addr > 0xFFFF ){
		g_message("Not correct address : %#lx",(long unsigned int)addr);
		gtk_entry_buffer_set_text(send_read_input_bits_answer_buff,"not correct addres",-1);
		return;
	}
/*	g_message("data :> %s",gtk_entry_buffer_get_text(send_read_input_bits_number_buff));*/
	number = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_input_bits_number_buff),NULL,16);
	if(number > 0xFFFF){
		g_message("Not correct number : %#lx",(long unsigned int)number);
		gtk_entry_buffer_set_text(send_read_input_bits_answer_buff,"not correct number",-1);
		return;
	}
	answer = send_read_input_bits_device(addr,number);
	gtk_entry_buffer_set_text(send_read_input_bits_answer_buff," ",-1);

	if(answer == NULL){
		return ;
	}

	for(i=0;i < number;i++){
		if(answer[i] == TRUE){
			temp[0] = '1';
		}
		else{
			temp[0] = '0';
		}
		gtk_entry_buffer_insert_text(send_read_input_bits_answer_buff,(i*2),temp,2);
	}
	return ;
}

GtkEntryBuffer * send_read_registers_addr_buff = NULL;
GtkEntryBuffer * send_read_registers_number_buff = NULL;
GtkEntryBuffer * send_read_registers_answer_buff = NULL;

void send_read_registers(GtkButton * button,gpointer user_data)
{
	guint64 addr;
	guint64 number = 0;
	uint16_t * answer = NULL;
	gchar temp[] = "0x0000 ";
	int i;

	if(get_status_connect() == DISCONNECT){
		g_message("Not connect");
		return ;
	}

/*	g_message("addr :> %s",gtk_entry_buffer_get_text(send_read_registers_addr_buff));*/
	addr = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_registers_addr_buff),NULL,16);
	if(addr > 0xFFFF ){
		g_message("Not correct address : %#lx",(long unsigned int)addr);
		gtk_entry_buffer_set_text(send_read_registers_answer_buff,"not correct addres",-1);
		return;
	}
/*	g_message("data :> %s",gtk_entry_buffer_get_text(send_read_registers_number_buff));*/
	number = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_registers_number_buff),NULL,16);
	if(number > 0xFFFF){
		g_message("Not correct number : %#lx",(long unsigned int)number);
		gtk_entry_buffer_set_text(send_read_registers_answer_buff,"not correct number",-1);
		return;
	}
	answer = send_read_registers_device(addr,number);
	gtk_entry_buffer_set_text(send_read_registers_answer_buff," ",-1);

	if(answer == NULL){
		return ;
	}
	for(i=0;i < number;i++){
		g_sprintf(temp,"%#4x ",answer[i]);
		gtk_entry_buffer_insert_text(send_read_registers_answer_buff,(i*7),temp,7);
	}
	return ;
}

GtkEntryBuffer * send_read_input_registers_addr_buff = NULL;
GtkEntryBuffer * send_read_input_registers_number_buff = NULL;
GtkEntryBuffer * send_read_input_registers_answer_buff = NULL;

void send_read_input_registers(GtkButton * button,gpointer user_data)
{
	guint64 addr;
	guint64 number = 0;
	uint16_t * answer = NULL;
	gchar temp[] = "0x0000 ";
	int i;

	if(get_status_connect() == DISCONNECT){
		g_message("Not connect");
		return ;
	}

/*	g_message("addr :> %s",gtk_entry_buffer_get_text(send_read_input_registers_addr_buff));*/
	addr = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_input_registers_addr_buff),NULL,16);
	if(addr > 0xFFFF ){
		g_message("Not correct address : %#lx",(long unsigned int)addr);
		gtk_entry_buffer_set_text(send_read_input_registers_answer_buff,"not correct addres",-1);
		return;
	}
/*	g_message("data :> %s",gtk_entry_buffer_get_text(send_read_input_registers_number_buff));*/
	number = g_ascii_strtoull(gtk_entry_buffer_get_text(send_read_input_registers_number_buff),NULL,16);
	if(number > 0xFFFF){
		g_message("Not correct number : %#lx",(long unsigned int)number);
		gtk_entry_buffer_set_text(send_read_input_registers_answer_buff,"not correct number",-1);
		return;
	}
	answer = send_read_input_registers_device(addr,number);
	gtk_entry_buffer_set_text(send_read_input_registers_answer_buff," ",-1);

	if(answer == NULL){
		return ;
	}

	for(i=0;i < number;i++){
		g_sprintf(temp,"%#4x ",answer[i]);
		gtk_entry_buffer_insert_text(send_read_input_registers_answer_buff,(i*7),temp,7);
	}
	return ;
}

GtkEntryBuffer * send_write_bit_addr_buff = NULL;
GtkEntryBuffer * send_write_bit_number_buff = NULL;
GtkEntryBuffer * send_write_bit_answer_buff = NULL;

void send_write_bit(GtkButton * button,gpointer user_data)
{
	guint64 addr;
	guint64 number = 0;
	int rc;

	if(get_status_connect() == DISCONNECT){
		g_message("Not connect");
		return ;
	}

/*	g_message("addr :> %s",gtk_entry_buffer_get_text(send_write_bit_addr_buff));*/
	addr = g_ascii_strtoull(gtk_entry_buffer_get_text(send_write_bit_addr_buff),NULL,16);
	if(addr > 0xFFFF ){
		g_message("Not correct address : %#lx",(long unsigned int)addr);
		gtk_entry_buffer_set_text(send_write_bit_answer_buff,"not correct addres",-1);
		return;
	}
/*	g_message("data :> %s",gtk_entry_buffer_get_text(send_write_bit_number_buff));*/
	number = g_ascii_strtoull(gtk_entry_buffer_get_text(send_write_bit_number_buff),NULL,16);
	if(number > 2){
		g_message("Not correct status : %#lx",(long unsigned int)number);
		gtk_entry_buffer_set_text(send_write_bit_answer_buff,"not correct number",-1);
		number = 1;
	}
	rc = send_write_bit_device(addr,number);

	if(rc == -1){
		gtk_entry_buffer_set_text(send_write_bit_answer_buff,"-1",-1);
		return ;
	}
	if(number == 0){
		gtk_entry_buffer_set_text(send_write_bit_answer_buff,"0",-1);
	}
	else{
		gtk_entry_buffer_set_text(send_write_bit_answer_buff,"1",-1);
	}
	return ;
}

static int create_frame_send(GtkGrid * main_grid)
{
	GtkWidget * send_frame;
	GtkWidget * send_grid;

	GtkWidget * send_read_bits_func;
	GtkWidget * send_read_bits_button;
	GtkWidget * send_read_bits_addr;
	GtkWidget * send_read_bits_number;
	GtkWidget * send_read_bits_answer;

	GtkWidget * send_read_input_bits_func;
	GtkWidget * send_read_input_bits_button;
	GtkWidget * send_read_input_bits_addr;
	GtkWidget * send_read_input_bits_number;
	GtkWidget * send_read_input_bits_answer;

	GtkWidget * send_read_registers_func;
	GtkWidget * send_read_registers_button;
	GtkWidget * send_read_registers_addr;
	GtkWidget * send_read_registers_number;
	GtkWidget * send_read_registers_answer;

	GtkWidget * send_read_input_registers_func;
	GtkWidget * send_read_input_registers_button;
	GtkWidget * send_read_input_registers_addr;
	GtkWidget * send_read_input_registers_number;
	GtkWidget * send_read_input_registers_answer;

	GtkWidget * send_write_bit_func;
	GtkWidget * send_write_bit_button;
	GtkWidget * send_write_bit_addr;
	GtkWidget * send_write_bit_number;
	GtkWidget * send_write_bit_answer;

	send_frame = gtk_frame_new("send");
	send_grid = gtk_grid_new();

	gtk_grid_attach(main_grid,send_frame,0,2,1,1);
	gtk_container_add(GTK_CONTAINER(send_frame),send_grid);
	gtk_grid_set_column_spacing(GTK_GRID(send_grid),5);
	gtk_grid_set_row_spacing(GTK_GRID(send_grid),3);

	send_read_bits_func = gtk_label_new("func 0x01");
	send_read_bits_button = gtk_button_new_with_label("read bits");

	send_read_bits_addr_buff = gtk_entry_buffer_new("addr",-1);
	send_read_bits_addr = gtk_entry_new_with_buffer(send_read_bits_addr_buff);
	send_read_bits_number_buff = gtk_entry_buffer_new("number",-1);
	send_read_bits_number = gtk_entry_new_with_buffer(send_read_bits_number_buff);
	send_read_bits_answer_buff = gtk_entry_buffer_new("answer",-1);
	send_read_bits_answer = gtk_entry_new_with_buffer(send_read_bits_answer_buff);

	gtk_entry_set_max_length(GTK_ENTRY(send_read_bits_addr),4);
	gtk_entry_set_max_length(GTK_ENTRY(send_read_bits_number),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_bits_addr),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_bits_number),4);
	gtk_entry_set_overwrite_mode(GTK_ENTRY(send_read_bits_answer),FALSE);
	g_signal_connect(send_read_bits_button,"clicked",G_CALLBACK(send_read_bits),NULL);

	gtk_grid_attach(GTK_GRID(send_grid),send_read_bits_func  ,0,0,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_bits_button,1,0,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_bits_addr  ,2,0,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_bits_number,3,0,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_bits_answer,4,0,1,1);

	send_read_input_bits_func = gtk_label_new("func 0x02");
	send_read_input_bits_button = gtk_button_new_with_label("read input bits");

	send_read_input_bits_addr_buff = gtk_entry_buffer_new("addr",-1);
	send_read_input_bits_addr = gtk_entry_new_with_buffer(send_read_input_bits_addr_buff);
	send_read_input_bits_number_buff = gtk_entry_buffer_new("number",-1);
	send_read_input_bits_number = gtk_entry_new_with_buffer(send_read_input_bits_number_buff);
	send_read_input_bits_answer_buff = gtk_entry_buffer_new("answer",-1);
	send_read_input_bits_answer = gtk_entry_new_with_buffer(send_read_input_bits_answer_buff);

	gtk_entry_set_max_length(GTK_ENTRY(send_read_input_bits_addr),4);
	gtk_entry_set_max_length(GTK_ENTRY(send_read_input_bits_number),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_input_bits_addr),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_input_bits_number),4);
	gtk_entry_set_overwrite_mode(GTK_ENTRY(send_read_input_bits_answer),FALSE);
	g_signal_connect(send_read_input_bits_button,"clicked",G_CALLBACK(send_read_input_bits),NULL);

	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_bits_func  ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_bits_button,1,1,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_bits_addr  ,2,1,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_bits_number,3,1,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_bits_answer,4,1,1,1);

	send_read_registers_func = gtk_label_new("func 0x03");
	send_read_registers_button = gtk_button_new_with_label("read registers");

	send_read_registers_addr_buff = gtk_entry_buffer_new("addr",-1);
	send_read_registers_addr = gtk_entry_new_with_buffer(send_read_registers_addr_buff);
	send_read_registers_number_buff = gtk_entry_buffer_new("number",-1);
	send_read_registers_number = gtk_entry_new_with_buffer(send_read_registers_number_buff);
	send_read_registers_answer_buff = gtk_entry_buffer_new("answer",-1);
	send_read_registers_answer = gtk_entry_new_with_buffer(send_read_registers_answer_buff);

	gtk_entry_set_max_length(GTK_ENTRY(send_read_registers_addr),4);
	gtk_entry_set_max_length(GTK_ENTRY(send_read_registers_number),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_registers_addr),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_registers_number),4);
	gtk_entry_set_overwrite_mode(GTK_ENTRY(send_read_registers_answer),FALSE);
	g_signal_connect(send_read_registers_button,"clicked",G_CALLBACK(send_read_registers),NULL);

	gtk_grid_attach(GTK_GRID(send_grid),send_read_registers_func  ,0,2,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_registers_button,1,2,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_registers_addr  ,2,2,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_registers_number,3,2,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_registers_answer,4,2,1,1);

	send_read_input_registers_func = gtk_label_new("func 0x04");
	send_read_input_registers_button = gtk_button_new_with_label("read input registers");

	send_read_input_registers_addr_buff = gtk_entry_buffer_new("addr",-1);
	send_read_input_registers_addr = gtk_entry_new_with_buffer(send_read_input_registers_addr_buff);
	send_read_input_registers_number_buff = gtk_entry_buffer_new("number",-1);
	send_read_input_registers_number = gtk_entry_new_with_buffer(send_read_input_registers_number_buff);
	send_read_input_registers_answer_buff = gtk_entry_buffer_new("answer",-1);
	send_read_input_registers_answer = gtk_entry_new_with_buffer(send_read_input_registers_answer_buff);

	gtk_entry_set_max_length(GTK_ENTRY(send_read_input_registers_addr),4);
	gtk_entry_set_max_length(GTK_ENTRY(send_read_input_registers_number),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_input_registers_addr),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_read_input_registers_number),4);
	gtk_entry_set_overwrite_mode(GTK_ENTRY(send_read_input_registers_answer),FALSE);
	g_signal_connect(send_read_input_registers_button,"clicked",G_CALLBACK(send_read_input_registers),NULL);

	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_registers_func  ,0,3,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_registers_button,1,3,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_registers_addr  ,2,3,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_registers_number,3,3,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_read_input_registers_answer,4,3,1,1);

	send_write_bit_func = gtk_label_new("func 0x05");
	send_write_bit_button = gtk_button_new_with_label("write bit");

	send_write_bit_addr_buff = gtk_entry_buffer_new("addr",-1);
	send_write_bit_addr = gtk_entry_new_with_buffer(send_write_bit_addr_buff);
	send_write_bit_number_buff = gtk_entry_buffer_new("status",-1);
	send_write_bit_number = gtk_entry_new_with_buffer(send_write_bit_number_buff);
	send_write_bit_answer_buff = gtk_entry_buffer_new("answer",-1);
	send_write_bit_answer = gtk_entry_new_with_buffer(send_write_bit_answer_buff);

	gtk_entry_set_max_length(GTK_ENTRY(send_write_bit_addr),4);
	gtk_entry_set_max_length(GTK_ENTRY(send_write_bit_number),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_write_bit_addr),4);
	gtk_entry_set_width_chars(GTK_ENTRY(send_write_bit_number),4);
	gtk_entry_set_overwrite_mode(GTK_ENTRY(send_write_bit_answer),FALSE);
	g_signal_connect(send_write_bit_button,"clicked",G_CALLBACK(send_write_bit),NULL);

	gtk_grid_attach(GTK_GRID(send_grid),send_write_bit_func  ,0,4,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_write_bit_button,1,4,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_write_bit_addr  ,2,4,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_write_bit_number,3,4,1,1);
	gtk_grid_attach(GTK_GRID(send_grid),send_write_bit_answer,4,4,1,1);

	return 0;
}

static void activate_app(GtkApplication * app,gpointer data)
{
	GtkWidget * main_window;
	GtkWidget * main_grid;

	GtkWidget * ini_frame;
	GtkWidget * ini_grid;
	GtkWidget * ini_entry;
	GtkWidget * ini_load_button;

	GtkWidget * con_frame;
	GtkWidget * con_grid;
	GtkWidget * con_button;

	/*main windows*/
	main_window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (main_window), STR_TITEL);
	gtk_window_set_default_size (GTK_WINDOW (main_window), 400, 500);

	main_grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(main_window),main_grid);

	ini_frame = gtk_frame_new("setting connect");
	ini_grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(ini_grid),7);
	gtk_grid_set_row_spacing(GTK_GRID(ini_grid),3);

	ini_file_name = gtk_entry_buffer_new(DEFAULT_INI_FILE,-1);
	/*gtk_entry_buffer_set_max_length (GTK_ENTRY_BUFFER(ini_file_name),40);*/
	ini_entry = gtk_entry_new_with_buffer(ini_file_name);
	gtk_entry_set_max_length (GTK_ENTRY(ini_entry),40);

	ini_load_button = gtk_button_new_with_label("load setting");
	gtk_grid_attach(GTK_GRID(main_grid),ini_frame,0,0,1,1);

	gtk_container_add(GTK_CONTAINER(ini_frame),ini_grid);
	gtk_grid_attach(GTK_GRID(ini_grid),ini_entry,0,0,3,1);
	gtk_grid_attach(GTK_GRID(ini_grid),ini_load_button,3,0,3,1);
	g_signal_connect(ini_load_button,"clicked",G_CALLBACK(ini_load),ini_file_name);

	ini_name_device = gtk_label_new("device");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_name_device,0,1,1,1);
	ini_baud = gtk_label_new("baud");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_baud,1,1,1,1);
	ini_parity = gtk_label_new("parity");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_parity,2,1,1,1);
	ini_data_bit = gtk_label_new("data bit");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_data_bit,3,1,1,1);
	ini_stop_bit = gtk_label_new("stop bit");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_stop_bit,4,1,1,1);
	ini_slave_id = gtk_label_new("slave ID");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_slave_id,0,2,1,1);
	ini_debug = gtk_label_new("debug");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_debug,1,2,1,1);
	ini_protocol = gtk_label_new("protocol");
	gtk_grid_attach(GTK_GRID(ini_grid),ini_protocol,2,2,1,1);

	con_frame = gtk_frame_new("connect");
	con_grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(con_grid),40);

	gtk_grid_attach(GTK_GRID(main_grid),con_frame,0,1,1,1);
	gtk_container_add(GTK_CONTAINER(con_frame),con_grid);

	con_button = gtk_button_new_with_label("connect");
	con_status = gtk_label_new("not connect");

	gtk_grid_attach(GTK_GRID(con_grid),con_button,0,0,1,1);
	gtk_grid_attach(GTK_GRID(con_grid),con_status,2,0,1,1);
	g_signal_connect(con_button,"clicked",G_CALLBACK(con_device),NULL);

	create_frame_send(GTK_GRID(main_grid));

	gtk_widget_show_all (main_window);
}
/*****************************************************************************/
/* Основная функция                                                          */
/*****************************************************************************/
int main(int argc,char * argv[])
{
	GtkApplication *app;
	int status;

	app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"startup", G_CALLBACK(startup_app),NULL);
	g_signal_connect(app,"activate",G_CALLBACK(activate_app),NULL);
	g_signal_connect(app,"shutdown",G_CALLBACK(shutdown_app),NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
/*****************************************************************************/
