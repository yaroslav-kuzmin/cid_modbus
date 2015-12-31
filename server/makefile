###############################################################################
#                                                                             #
#  cid_modbus  : модуль проверки соединения по протоколу ModBus               #
#                                                                             #
#  author: Kuzmin Yaroslav                                                    #
#  email: kuzmin.yaroslav@gmail.com                                           #
#  date : 10.08.15                                                            #
#                                                                             #
###############################################################################

OBJ_CATALOG=.obj/
DEPEND_CATALOG=.depend/

MODBUS_CATALOG=../libmodbus/
LIB_MODBUS=$(MODBUS_CATALOG)libmodbus.a
LIB_MODBUS_OPTION=-lmodbus

EXEC=../server_modbus.exe
SOURCE=$(wildcard *.c)
OBJS=$(patsubst %.c,$(OBJ_CATALOG)%.o,$(SOURCE))
DEPEND=$(patsubst %.c,$(DEPEND_CATALOG)%.d,$(SOURCE))

CXX=gcc
CFLAGS=-g2 -Wall -I.. -I. -I$(MODBUS_CATALOG) `pkg-config --cflags gtk+-3.0`
LDFLAGS=-g2 -L$(MODBUS_CATALOG) 
LIB=`pkg-config --libs gtk+-3.0` $(LIB_MODBUS_OPTION)

$(EXEC):$(OBJS) 
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIB)

$(OBJ_CATALOG)%.o:%.c
	$(CXX) $(CFLAGS) -c $< -o $@

$(DEPEND_CATALOG)%.d:%.c
	$(CXX) -MM -I.. -I. -I$(MODBUS_CATALOG) $< | sed -e '1s/^/.obj\//' > $@

include $(DEPEND)

.PHONY:clean
clean:
	-rm -f $(EXEC) *~ $(OBJ_CATALOG)*.o $(DEPEND_CATALOG)*.d 

