/********************************************
MCU: STC12C5A60S2
AUTHOR:
VERSION:
TIME:
*********************************************/

#define _MODBUS_GLOBALS_
#include "common.h"

void ModbusInit(void)
{
	unsigned char cnt;
	RX_485_EN()	;
	R_485	=	1;
	D_485	=	1;

	//参数配置
	ModbusPara.sRUN =  MB_IDEL;
	ModbusPara.sERR = ERR_NOT;
	ModbusPara.times = 0;
	ModbusPara.ms_50 = 0;
	ModbusPara.rCnt = 0;
	ModbusPara.HostRept = 0;
	ModbusPara.mAddrs = MB_ConceDevice_ADDR;
	for(cnt = 0 ;cnt < LENGTH_MB_DATA ;cnt++)
	{
		ModbusPara.rBuf[cnt] = 0;
		ModbusPara.tBuf[cnt] = 0;
	}

	for(cnt = 0 ;cnt <LENGTH_COIL_REG ;cnt++)
	{
		CoilBitPara[cnt] = 0;
	}

	for(cnt = 0 ;cnt <LENGTH_DiscreteREG ;cnt++)
	{
		DiscreteBitPara[cnt] = 0;
	}

	for(cnt = 0 ;cnt <LENGTH_HoldingREG ;cnt++)
	{
		HoldingREGPara[cnt] = 0;
	}

	for(cnt = 0 ;cnt <LENGTH_InputREG ;cnt++)
	{
		InputREGPara[cnt] = 0;
	}
}

void ModbusTimesProcess(void)
{
	ModbusPara.times++;
	if(ModbusPara.times == 200) 
	{
		ModbusPara.ms_50++;
	}

	cnt1ms ++;		//260us
	if(cnt1ms%200 == 199) 
	{// 50ms
		cnt1ms = 0;
		bTime100ms ++;
		bTsave100ms ++;
		bTime1s ++;
		bTime50ms ++;
	}

	if((ModbusPara.sRUN&MB_BUSY) ==  MB_BUSY) 
	{// 总线检测
		if(ModbusPara.times >= BUS_IDLE_TIME) 
		{//总线进入空闲，帧结束或开始
			if(ModbusPara.sRUN == MB_RECIVE_ERR)
			{// 接收过程中 有出现数据存储空间溢出或间隔时间超过T1.5
				ModbusPara.sERR =ERR_MB_DEVICE;
			}
			
			if(ModbusPara.sRUN == MB_SEND) 
			{
				ModbusPara.sRUN = MB_WAIT_HOST;
				ModbusPara.ms_50 = 0;
			}
			else 
			{
				ModbusPara.sRUN =  MB_IDEL;
			}
			ModbusPara.times = 0;
		}
	}
	else if(ModbusPara.sRUN ==  MB_WAIT_HOST)
	{//等待合法设备响应
		if(ModbusPara.ms_50 >= NO_RESPONSE_TIME ||  
		(ModbusPara.ms_50 >=NO_RESPONSE_TIME_BC && ModbusPara.mAddrs == MB_Broadcast_ADDR))
		{//从设备响应超时
			if(ModbusPara.mAddrs == MB_Broadcast_ADDR)
			{
				ModbusPara.sRUN =  MB_IDEL;
				ModbusPara.sERR = ERR_NOT;
			}
			else
			{
				ModbusPara.sRUN = MB_NO_RESPONSE;
				ModbusPara.sERR = ERR_MB_DEVICE;
			}
			ModbusPara.ms_50 = 0;
		}
	}
}

void ModbusSend(unsigned char length) 
{
	unsigned char cnt;

	TX_485_EN()	;
	for(cnt=0; cnt < length; cnt++)
	{// test和延时作用
	//	TxOneData(ModbusPara.tBuf[cnt]);	
	}
	
	if(length)
	{
		ModbusPara.sRUN = MB_SEND;
		ModbusPara.times = 0;
		for(cnt=0; cnt < length; cnt++)
		{	
			usart2_sendb(ModbusPara.tBuf[cnt]);
			ModbusPara.times = 0;  //重新计时
		}
	}
	RX_485_EN()	;
}

void Modbus_ERROR(void) 
{
	unsigned short 	temp16;

	if(bHost_protocol)
	{
		if(ModbusPara.sERR)
		{
			if(ModbusPara.sERR != ERR_MB_DATA &&  ModbusPara.sERR !=  ERR_MB_DEVICE)
			{//  不需要重复发送
				ModbusPara.HostRept = 0;
			}
			if(ModbusPara.HostRept == 0) ModbusPara.sERR= ERR_NOT;
		}
	
		if(ModbusPara.HostRept)
		{// 重复送
			ModbusPara.HostRept --;
			if(ModbusPara.tBuf[1] == CHECK_COIL_bSTATUS || ModbusPara.tBuf[1] == GET_INPUT_bSTATUS 
			|| ModbusPara.tBuf[1] == GET_HOLDING_REGT || ModbusPara.tBuf[1] == GET_INPUT_REGT
			|| ModbusPara.tBuf[1] == SET_COIL_SINGLE || ModbusPara.tBuf[1] == PRESET_HOLDING_sREGT)
			{// 1机器地址+1功能+2数据地址+2寄存器值+2校验值
				ModbusSend(8);	
			}
			else if(ModbusPara.tBuf[1] == SET_COIL_MULTIPLE || ModbusPara.tBuf[1] == PRESET_HOLDING_mREGT )
			{// 1机器地址+1功能+2数据地址+2寄存器数量+1字节数+ 字节数BUF[6]的值+2校验值
				ModbusSend(9+ModbusPara.tBuf[6]);
			}
			else if( ModbusPara.tBuf[1] == REEKO_FUN_EXTERN)
			{
				ModbusSend(ModbusPara.tBuf[3]);	
			}
			else 
			{// 非主机需要的功能不重复
				ModbusPara.HostRept = 0;
			}
			ModbusPara.sERR= ERR_NOT;
		}
	}
	else
	{
		if(ModbusPara.sERR==  ERR_MB_FUN ||ModbusPara.sERR==  ERR_MB_ADDR 
		|| ModbusPara.sERR== ERR_MB_DATA )
		{//从模式,发送响应数据
			ModbusPara.tBuf[0] = ModbusPara.rBuf[0];			// 设备地址
			ModbusPara.tBuf[1] = ModbusPara.rBuf[1] | 0X80;	// 功能码| 0X80
			ModbusPara.tBuf[2] =	ModbusPara.sERR;			// 错误代码
			temp16 = ModbusCRC16( &ModbusPara.tBuf[0], 3);	// 获取CRC
			ModbusPara.tBuf[3] = temp16 >> 8;
			ModbusPara.tBuf[4] = temp16 ;
			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR) ModbusSend(5);	
		}
		ModbusPara.sERR= ERR_NOT;
	}
}

void MB_ReadCoilRegisters(void) 
{ // 功能码1
	unsigned short addr_data,reg_num;
	unsigned char  i,k,byteCount,temp8; 
	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num /8;
		if( reg_num%8) byteCount ++;			// 字节数
		if(ModbusPara.rBuf[2] <= LENGTH_COIL_REG && byteCount == ModbusPara.rBuf[2])
		{//返回的字节数判断
			addr_data = ModbusPara.tBuf[2];	// 起始地址
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			reg_num += addr_data;			//结束 线圈号= 起始 线圈号+ 数量 
			for (k = 0 ; k < byteCount ; k++) 
			{ // 存储主设备查询的状态位
				for (i = 0 ; i < 8 ; i++) 
				{ 
					temp8 = 1;
					temp8 <<= addr_data%8;					//寻找对应的BIT位号
					if(ModbusPara.rBuf[k + 3] & (1<< i))
					{
						CoilBitPara[addr_data/8] |= temp8;		// 将该位置1
					}
					else 
					{
						 temp8 = ~ temp8;						// 取反
						 CoilBitPara[addr_data/8] &= temp8;		// 将该位清零
					}
					addr_data ++;
					if(addr_data >= reg_num) break;			// 填充的不做处理
					else if(addr_data >= SUM_COIL_BIT)
					{// 数据处理过程中出错
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				} 
			} 
		}
		else
		{// 超出系统规定线圈的总数,或者返回的字节数与实际发送的对应不上
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_COIL_BIT)
		{// 线圈个数判断OK
			if(addr_data < SUM_COIL_BIT && (addr_data + reg_num) <= SUM_COIL_BIT)
			{// 位地址范围判断OK,开始响应处理。
				byteCount = reg_num /8;
				if( reg_num%8) byteCount ++;
				reg_num += addr_data;		//结束 线圈号= 起始 线圈号+ 数量 
				for (k = 0 ; k < byteCount ; k++) 
				{ // 提取主设备想要的状态位,处理输出状态
					ModbusPara.tBuf[k + 3] = 0;
					for (i = 0 ; i < 8 ; i++) 
					{ 
						temp8 = 1;
						temp8 <<= addr_data%8;					//寻找对应的BIT位号
						temp8 &=  CoilBitPara[addr_data/8];			// 获取该位号的状态
						if(temp8) ModbusPara.tBuf[k + 3] |= (1<< i);	// =1，在新的BIT位号置1
						addr_data ++;
						if(addr_data >= reg_num) break;			// 填充的不做处理
						else if(addr_data >= SUM_COIL_BIT)
						{// 数据处理过程中出错
							ModbusPara.sERR= ERR_MB_DATA;
							break;
						}
					} 
				} 
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// 设备地址
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// 功能码
				ModbusPara.tBuf[2] = byteCount; 					// 字节数
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// 获取CRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				ModbusSend(byteCount );
			}
			else
			{// 超出系统规定线圈的位地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定线圈的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ReadDiscreteRegisters(void) 
{ // 功能码2
	unsigned short addr_data,reg_num;
	unsigned char  i,k,byteCount,temp8; 
	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num /8;
		if( reg_num%8) byteCount ++;			// 字节数
		if(ModbusPara.rBuf[2] <= LENGTH_DiscreteREG && byteCount == ModbusPara.rBuf[2])
		{//返回的字节数判断
			addr_data = ModbusPara.tBuf[2];	// 起始地址
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			addr_data -= SUM_COIL_BIT;
			
			reg_num += addr_data;			//结束离散量的位= 起始离散量的位+ 数量
			for (k = 0 ; k < byteCount ; k++) 
			{ // 存储主设备查询的状态位
				for (i = 0 ; i < 8 ; i++) 
				{ 
					temp8 = 1;
					temp8 <<= addr_data%8;					//寻找对应的BIT位号
					if(ModbusPara.rBuf[k + 3] & (1<< i))
					{
						DiscreteBitPara[addr_data/8] |= temp8;	// 将该位置1
					}
					else 
					{
						 temp8 = ~ temp8;						// 取反
						 DiscreteBitPara[addr_data/8] &= temp8;// 将该位清零
					}
					addr_data ++;
					if(addr_data >= reg_num) break;			// 填充的不做处理
					else if(addr_data >= SUM_DiscreteREG_BIT)
					{// 数据处理过程中出错
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				} 
			} 
		}
		else
		{// 超出系统规定离散量的总数,或者返回的字节数与实际发送的对应不上
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		addr_data -= SUM_COIL_BIT;
			
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_DiscreteREG_BIT)
		{// 离散量个数判断OK
			if(addr_data < SUM_DiscreteREG_BIT && (addr_data + reg_num) <= SUM_DiscreteREG_BIT)
			{// 位地址范围判断OK,开始响应处理。
				byteCount = reg_num /8;
				if( reg_num%8) byteCount ++;
				reg_num += addr_data;		//结束离散量的位= 起始离散量的位+ 数量
				for (k = 0 ; k < byteCount ; k++) 
				{ // 提取主设备想要的状态位,处理输出状态
					ModbusPara.tBuf[k + 3] = 0;
					for (i = 0 ; i < 8 ; i++) 
					{ 
						temp8 = 1;
						temp8 <<= addr_data%8;					//寻找对应的BIT位号
						temp8 &=  DiscreteBitPara[addr_data/8];	// 获取该位号的状态
						if(temp8) ModbusPara.tBuf[k + 3] |= (1<< i);	// =1，在新的BIT位号置1
						addr_data ++;
						if(addr_data >= reg_num) break;			// 填充的不做处理
						else if(addr_data >= SUM_DiscreteREG_BIT)
						{// 数据处理过程中出错
							ModbusPara.sERR= ERR_MB_DATA;
							break;
						}
					} 
				} 
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// 设备地址
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// 功能码
				ModbusPara.tBuf[2] = byteCount; 					// 字节数
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// 获取CRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
			}
			else
			{// 超出系统规定离散量的位地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定离散量的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ReadHoldingRegisters(void) 
{ // 功能码3
	unsigned short addr_data,reg_num;
	unsigned char  k,byteCount; 

	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		//输出数量---利用发送的数据
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num *2;				// 字节数= 	输出数量*2
		if(ModbusPara.rBuf[2] <= LENGTH_HoldingREG && byteCount == ModbusPara.rBuf[2])
		{//返回的字节数判断
			addr_data = ModbusPara.tBuf[2];	// 保持寄存器号---利用发送的数据
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			addr_data <<= 1;					// 保持寄存器号对应数据起始地址
			reg_num = addr_data + byteCount;	//结束  保持寄存器号= 起始保持寄存器号+ 数量 
			for (k = 0 ; k < byteCount ; k++) 
			{ // 存储主设备查询的保持寄存器内容
				 HoldingREGPara[addr_data] = ModbusPara.rBuf[k + 3];
				 addr_data++;
				if(addr_data >= LENGTH_HoldingREG|| addr_data >= reg_num )
				{// 数据处理过程中出错
					ModbusPara.sERR= ERR_MB_DATA;
					break;
				}
			} 
		}
		else
		{// 超出系统规定保持寄存器的总数,或者返回的字节数与实际发送的对应不上
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
		{// 保持寄存器个数判断OK
			if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
			{// 保持寄存器地址范围判断OK,开始响应处理。
				byteCount = reg_num *2;			// 字节数= 	输出数量*2
				reg_num = addr_data + byteCount ;	//结束  保持寄存器号= 起始保持寄存器号+ 数量 
				for (k = 0 ; k < byteCount ; k++) 
				{ // 提取主设备想要的保持寄存器内容
					ModbusPara.tBuf[k + 3] =  HoldingREGPara[addr_data];
					ModbusPara.tBuf[k + 3] = 0x02;		// test
					addr_data++;
					if(addr_data >= LENGTH_HoldingREG|| addr_data >= reg_num )
					{// 数据处理过程中出错
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				}
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// 设备地址
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// 功能码
				ModbusPara.tBuf[2] = byteCount; 					// 字节数
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// 获取CRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
			}
			else
			{// 超出系统规定保持寄存器地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定保持寄存器的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ReadInputRegisters(void) 
{ // 功能码4
	unsigned short addr_data,reg_num;
	unsigned char  k,byteCount; 
	
	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		// 输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num *2;				 // 字节数=  输出数量*2	
		if(ModbusPara.rBuf[2] <= LENGTH_InputREG && byteCount == ModbusPara.rBuf[2])
		{//返回的字节数判断
			addr_data = ModbusPara.tBuf[2];	// 起始 输入寄存器号
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			addr_data -= SUM_HoldingREG_WORD;
			reg_num = addr_data + byteCount;	//结束  输入寄存器号= 起始 输入寄存器号+ 数量 
			for (k = 0 ; k < byteCount ; k++) 
			{ // 存储主设备查询的保持寄存器内容
				 InputREGPara[addr_data] = ModbusPara.tBuf[k + 3];
				 addr_data++;
				if(addr_data >= LENGTH_InputREG|| addr_data >= reg_num )
				{// 数据处理过程中出错
					ModbusPara.sERR= ERR_MB_DATA;
					break;
				}
			} 
		}
		else
		{// 超出系统规定输入寄存器的总数,或者返回的字节数与实际发送的对应不上
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		addr_data -= SUM_HoldingREG_WORD;
		
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_InputREG_WORD)
		{// 输入寄存器个数判断OK
			if(addr_data < SUM_InputREG_WORD && (addr_data + reg_num) <= SUM_InputREG_WORD)
			{//输入寄存器地址范围判断OK,开始响应处理。
				byteCount = reg_num *2;			//字节数=  输出数量*2	
				reg_num = addr_data + byteCount;	//结束  输入寄存器号= 起始 输入寄存器号+ 数量 
				for (k = 0 ; k < byteCount ; k++) 
				{ // 提取主设备想要的输入寄存器内容
					ModbusPara.tBuf[k + 3] =  InputREGPara[addr_data];
					addr_data++;
					if(addr_data >= LENGTH_InputREG|| addr_data >= reg_num )
					{// 数据处理过程中出错
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				}
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// 设备地址
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// 功能码
				ModbusPara.tBuf[2] = byteCount; 					// 字节数
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// 获取CRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
			}
			else
			{// 超出系统规定输入寄存器地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定输入寄存器的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ForceSingleCoil(void)
{ // 功能码5
	unsigned short addr_data,ON_OFF;
	unsigned char  temp8; 
	
	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		if(addr_data < SUM_COIL_BIT )
		{// 位地址范围判断OK,开始响应处理。
			if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
			|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
			{// 返回的与要求的不一致(可能性很小)
				ModbusPara.sERR= ERR_MB_DEVICE;
			}
		}
		else
		{// 超出系统规定线圈的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		ON_OFF = ModbusPara.tBuf[4];			//输出值
		ON_OFF <<= 8;
		ON_OFF += ModbusPara.tBuf[5];
		if(addr_data < SUM_COIL_BIT  && ON_OFF == 0xff00 && ON_OFF == 0x0000)
		{// 位地址范围判断OK,开始响应处理。
			temp8 = 1;
			temp8 <<= addr_data%8;					//寻找对应的BIT位号
			if(ON_OFF == 0x0000)
			{
				temp8 = ~temp8;						// 取反
				 CoilBitPara[addr_data/8] &= temp8;		// 获取该位号的状态:OFF
			}
			else
			{
				CoilBitPara[addr_data/8] |= temp8;		// 获取该位号的状态:ON
			}
	
			for(temp8 =0 ;temp8 < 8; temp8++)
			{
				ModbusPara.tBuf[temp8] = ModbusPara.rBuf[temp8]; 	
			}
			ModbusSend(8 );
	
		}
		else
		{// 超出系统规定线圈的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
}

void MB_PresetSingleHoldingRegister(void)
{// 功能码6
	unsigned short addr_data;
	unsigned char  temp8 ;

	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//输出数量
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];	
		if(addr_data < SUM_HoldingREG_WORD )
		{// 保持寄存器地址范围判断OK,开始响应处理。
			if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
			|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
			{// 返回的与要求的不一致(可能性很小)
				ModbusPara.sERR= ERR_MB_DEVICE;
			}
			else
			{// 获取写的值
				addr_data <<= 1;  // 保持寄存器号对应数据起始地址
				HoldingREGPara[addr_data] = ModbusPara.rBuf[4];
				HoldingREGPara[addr_data + 1] = ModbusPara.rBuf[5];
			}
		}
		else
		{// 超出系统规定保持寄存器地址范围
			ModbusPara.sERR= ERR_MB_ADDR;
		}
	}
	else
 	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		if(addr_data < SUM_HoldingREG_WORD )
		{// 保持寄存器地址范围判断OK,开始响应处理。
			HoldingREGPara[addr_data] = ModbusPara.rBuf[4];
			HoldingREGPara[addr_data + 1] = ModbusPara.rBuf[5];
			for(temp8 =0 ;temp8 < 8; temp8++)
			{
				ModbusPara.tBuf[temp8] = ModbusPara.rBuf[temp8]; 	
			}
			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(8 );
		}
		else
		{// 超出系统规定保持寄存器地址范围
			ModbusPara.sERR= ERR_MB_ADDR;
		}	
	}
}

void MB_ForceMultipleCoil(void) 
{ // 功能码15
	unsigned short addr_data,reg_num;
	unsigned char  i,k,byteCount,temp8; 

	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_COIL_BIT)
		{// 线圈个数判断OK
			if(addr_data < SUM_COIL_BIT && (addr_data + reg_num) <= SUM_COIL_BIT)
			{// 位地址范围判断OK,开始响应处理。
				if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
				|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
				{// 返回的与要求的不一致(可能性很小)
					ModbusPara.sERR = ERR_MB_DEVICE;
				}
			}
			else
			{// 超出系统规定线圈的位地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定线圈的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_COIL_BIT)
		{// 线圈个数判断OK
			if(addr_data < SUM_COIL_BIT && (addr_data + reg_num) <= SUM_COIL_BIT)
			{// 位地址范围判断OK,开始响应处理。
				byteCount = ModbusPara.rBuf[6];	// 字节数
				reg_num += addr_data;			//结束 线圈号= 起始 线圈号+ 数量 
				for (k = 0 ; k < byteCount ; k++) 
				{ // 从设备存储主设备指定的状态位
					for (i = 0 ; i < 8 ; i++) 
					{ 
						temp8 = 1;
						temp8 <<= addr_data%8;					//寻找对应的BIT位号
						if(ModbusPara.rBuf[k + 7] & (1<< i))
						{
							CoilBitPara[addr_data/8] |= temp8;		// 将该位置1
						}
						else 
						{
							 temp8 = ~ temp8;						// 取反
							 CoilBitPara[addr_data/8] &= temp8;		// 将该位清零
						}
						addr_data ++;
						if(addr_data >= reg_num) break;			// 填充的不做处理
						else if(addr_data >= SUM_COIL_BIT)
						{// 数据处理过程中出错
							ModbusPara.sERR= ERR_MB_DATA;
							break;
						}
					} 
				} 
	
				for(temp8 = 0; temp8 < 6; temp8++)
				{
					ModbusPara.tBuf[temp8] = ModbusPara.rBuf[temp8]; 
				}
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], 6);	// 获取CRC
				ModbusPara.tBuf[6] = reg_num >> 8;
				ModbusPara.tBuf[7] = reg_num ;
				ModbusSend(8);
			}
			else
			{// 超出系统规定线圈的位地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定线圈的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
}

void MB_PresetMultipleHoldingRegisters(void)
{ // 功能码16
	unsigned short addr_data,reg_num;
	unsigned char  k,byteCount; 
	
	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
		{// 保持寄存器个数判断OK
			if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
			{// 保持寄存器地址范围判断OK,开始响应处理?
				if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
				|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
				{// 返回的与要求的不一致(可能性很小)
					ModbusPara.sERR= ERR_MB_DEVICE;
				}
			}
			else
			{// 超出系统规定保持寄存器地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定保持寄存器的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//起始地址
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//输出数量
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
		{// 保持寄存器个数判断OK
			if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
			{// 保持寄存器地址范围判断OK,开始响应处理。
				byteCount = ModbusPara.rBuf[6];	// 字节数= 输出数量*2
				reg_num = addr_data + byteCount;	//结束 保持寄存器号= 起始 保持寄存器号+ 数量 
				for (k = 0 ; k < byteCount ; k++) 
				{ // 提取主设备想要的保持寄存器内容
					HoldingREGPara[addr_data] = ModbusPara.rBuf[7+k];
					addr_data++;
					if(addr_data >= LENGTH_HoldingREG || addr_data >= reg_num )
					{// 数据处理过程中出错
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				}
				for(k = 0; k < 6; k++)
				{
					ModbusPara.tBuf[k] = ModbusPara.rBuf[k]; 
				}
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], 6);	// 获取CRC
				ModbusPara.tBuf[6] = reg_num >> 8;
				ModbusPara.tBuf[7] = reg_num ;
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(8);
			}
			else
			{// 超出系统规定保持寄存器地址范围或者读取个数与起始地址不匹配溢出
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// 超出系统规定保持寄存器的总数
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
}

void ModbusProces(void) 
{ 
#if DEBUG_FLOW
	unsigned char i;
#endif
	if(ModbusPara.sRUN == MB_IDEL)
	{// 总线空闲，可以进行数据的处理	
		if (ModbusPara.rCnt >= 4 )
		{//  1个字节的设备地址，1个字节的功能码，N个内容数据，2个字节的CRC16
			#if DEBUG_FLOW
			Debug_Send(0xbb);
			for(i=0;i<ModbusPara.rCnt ;i++)
			{
				Debug_Send(ModbusPara.rBuf[i]);
			}
			#endif	 	
			if(ModbusCRC16(&ModbusPara.rBuf[0], ModbusPara.rCnt ) ==0)
			{//数据校验没有错误
//				ComLED	= !ComLED;
				if(ModbusPara.rBuf[1] & 0x80 && bHost_protocol)
				{// 返回的功能码有错
					ModbusPara.sERR = ModbusPara.rBuf[2];
				}
				else
				{
					ModbusPara.HostRept = 0;	// 接收成功后，剩余重复的次数不发
					switch(ModbusPara.rBuf[1])
					{// 功能码判断
						case CHECK_COIL_bSTATUS :
							MB_ReadCoilRegisters();
							break;
						case GET_INPUT_bSTATUS :
							MB_ReadDiscreteRegisters();
							break;
						case GET_HOLDING_REGT :
							MB_ReadHoldingRegisters();
							break;
						case GET_INPUT_REGT :
							MB_ReadInputRegisters();
							break;
						case SET_COIL_SINGLE :
							MB_ForceSingleCoil();
							break;
						case PRESET_HOLDING_sREGT :
							MB_PresetSingleHoldingRegister();
							break;
						case SET_COIL_MULTIPLE :
							MB_ForceMultipleCoil();
							break;
						case PRESET_HOLDING_mREGT :
							MB_PresetMultipleHoldingRegisters(); 
							break;
						case FUN_EXTERN :
							break;
						default :
							ModbusPara.sERR=  ERR_MB_FUN;	// 从设备不支持该功能,主设备前面已经判断
							break;
					}
				}
			}
			else
			{//数据校验有错误,则表示在传输过程中数据被干扰。
				ModbusPara.sERR= ERR_MB_DEVICE;
			}
			ModbusPara.rCnt  = 0;	// 清零，准备进行新的数据接收。
		}
		else if(ModbusPara.rCnt) 
		{
			ModbusPara.rCnt  = 0;	// 数据小于4个，出错的情况
		}

		Modbus_ERROR();
	} 
} 

void ModbusRequest(unsigned char dAddrs, unsigned char style, unsigned char sAdd,unsigned short val)
{//主机 发送请求命令
	unsigned short temp16;
	unsigned char i,k; 
	delay_ms(15);
	if(ModbusPara.sRUN ==  MB_IDEL && ModbusPara.HostRept == 0 && ModbusPara.rCnt == 0)
	{
		ModbusPara.HostRept = MB_REPEAT;

		ModbusPara.mAddrs = dAddrs;								// 从 设备地址
		ModbusPara.tBuf[0] =  dAddrs;							// 从 设备地址
		ModbusPara.tBuf[1] =  style;							// 功能码
		if(style == FUN_EXTERN)
		{// 扩展功能



		}
		else	   
		{// MODBUS协议内的功能
			ModbusPara.tBuf[2] = 0x00;						// 变量起始地址---高
			ModbusPara.tBuf[3] = sAdd;						// 变量起始地址---低				 
			ModbusPara.tBuf[4] = (unsigned char )(val >> 8);		// 输入输出数量、寄存器值
			ModbusPara.tBuf[5] = val;
			
			if(style == SET_COIL_MULTIPLE)
			{// 强制多个位输出
				ModbusPara.tBuf[6] = val/8;	// 字节数
				if(val%8) ModbusPara.tBuf[6] ++;
				for (k = 0 ; k < ModbusPara.tBuf[6]; k++)
				{ // 读取指定的状态位
					ModbusPara.tBuf[7+k] = 0;
					for (i = 0 ; i < 8 ; i++) 
					{ 
						ModbusPara.tBuf[7+k] >>= 1;
						if(val) 
						{
							val --;
							style = 1;
							style <<= sAdd%8;				//寻找对应的BIT位号
						}
						else style = 0;						// 零填充
						
						if(CoilBitPara[sAdd/8] & style)
						{
							ModbusPara.tBuf[7+k] |= 0x80;	// 将该位置1
						}
						sAdd ++;
					} 
				} 
				sAdd = ModbusPara.tBuf[6] + 7;
			}
			else if(style == PRESET_HOLDING_mREGT)
			{//预置多个保持寄存器
				sAdd = 6;
				k = val*2;
				ModbusPara.tBuf[sAdd++]= k;
				style = ModbusPara.tBuf[3]*2;

				for(i=0; i < k; i++)
				{
					ModbusPara.tBuf[sAdd] = HoldingREGPara[style+i] ;
					sAdd++;
				}
			}
			else sAdd = 6;
		}
		
		temp16 = ModbusCRC16( &ModbusPara.tBuf[0], sAdd);	// 获取CRC
		ModbusPara.tBuf[sAdd] =  temp16 >> 8;
		sAdd ++;
		ModbusPara.tBuf[sAdd] =  temp16 ;
		sAdd ++;
	
		if(ModbusPara.mAddrs == MB_Broadcast_ADDR)
		{
			ModbusPara.HostRept = 0;
		} 
		ModbusSend(sAdd);

#if DEBUG_FLOW
		Debug_Send(0xaa);
		for(i=0; i < sAdd; i++)
		{
			Debug_Send(ModbusPara.tBuf[i]);
		}
#endif

	}
}







