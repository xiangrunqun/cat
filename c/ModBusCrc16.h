#ifndef _MODBUS_CRC_H_
#define _MODBUS_CRC_H_

#ifdef _MODBUS_CRC_GLOBALS_
#define EXT
#else
#define EXT extern
#endif


EXT unsigned int  ModbusCRC16( unsigned char  *puchMsg, unsigned char usDataLen) ;

#undef EXT
#endif

