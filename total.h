/*****************************************************************************/
/*                                                                           */
/*  cid_modbus  : модуль проверки соединения по протоколу ModBus             */
/*                                                                           */
/*  author: Kuzmin Yaroslav                                                  */
/*  email: kuzmin.yaroslav@gmail.com                                         */
/*  date : 10.08.15                                                          */
/*****************************************************************************/

#ifndef TOTAL_H
#define TOTAL_H

#define DISCONNECT       0
#define CONNECT          1

#define SUCCESS          0
#define FAILURE          1

#define PROTOCOL_TCP     2
#define PROTOCOL_RTU     1
#define PROTOCOL_ASCII   0

#define REGISTERS        0x100

typedef unsigned int    flag_t;

#endif

