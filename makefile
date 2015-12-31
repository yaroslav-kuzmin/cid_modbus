###############################################################################
#                                                                             #
#  cid_modbus  : модуль проверки соединения по протоколу ModBus               #
#                                                                             #
#  author: Kuzmin Yaroslav                                                    #
#  email: kuzmin.yaroslav@gmail.com                                           #
#  date : 10.08.15                                                            #
#                                                                             #
###############################################################################

all:
	make -C libmodbus
	make -C client
	make -C server	

.PHONY:clean
clean:
	make -C libmodbus clean
	make -C client clean
	make -C server clean	

