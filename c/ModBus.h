#ifndef _MODBUS_H_
#define _MODBUS_H_

#ifdef _MODBUS_GLOBALS_
#define EXT
#else
#define EXT extern
#endif

sbit RT_485		=	P1^4;
sbit R_485		=	P1^2;
sbit D_485		=	P1^3;

#define	RX_485_EN()		(RT_485 = 0)
#define	TX_485_EN()		(RT_485 = 1)

#define TRUE	1
#define FALSE	0

  
//======================  参数宏定义=================================

//---- 本机支持MODBUS功能代码--------------
#define CHECK_COIL_bSTATUS			1 		//  读输出位状态
#define GET_INPUT_bSTATUS			2 		//  读输入位状态
#define GET_HOLDING_REGT			3		//  读保持寄存器
#define GET_INPUT_REGT				4		//  读输入寄存器
#define SET_COIL_SINGLE				5		//  强置单线圈	 
#define PRESET_HOLDING_sREGT		6		//  预置单个保持寄存器
#define SET_COIL_MULTIPLE			15		//  强制多个位输出
#define PRESET_HOLDING_mREGT		16		//  预置多个保持寄存器
#define REEKO_FUN_EXTERN			65		//  REEKO扩展功能
//---- 不正常的代码值-------------------------
#define ERR_NOT						0	
#define ERR_MB_FUN					1		// 不合法功能代码
#define ERR_MB_ADDR					2		// 不合法数据地址
#define ERR_MB_DATA					3		// 不合法数据，超出某功能的极限值
#define ERR_MB_DEVICE				4		// 主模式响应超出等待时间；从模式设备地址出错；包括字符间隔时间 超过1.5个字符
//---- 通信时间参数-----------------------------
#define ONE_DATA_BIT				10		// 采用方式1:10bit
#define BAUD_RATE_MODBUS			19200L	// MODBUS波特列
#define MODBUS_TIME					260		// 260uS	= (10/19200)/2
/*******************************************************
相关时间设置为下面所示，8，4，5，5
设置过长的话会出现读取无回复问题
********************************************************/
#define BUS_IDLE_TIME				8//8	// 帧空闲(开始或结束)， 超过3.5~4个接收字符时间
#define FRAME_ERR_TIME				4	
#define NO_RESPONSE_TIME			5//5	// 通讯中断超时 约500ms//1.5s
#define NO_RESPONSE_TIME_BC			5//5	// 通讯中断超时 约500Ms
//---- 通信状态参数-----------------------------
#define MB_IDEL						0X00	// 总线空闲、结束帧、起始帧
#define MB_WAIT_HOST				0X01	// 主设备等待响应
#define MB_BUSY						0X50	// 总线忙
#define MB_RECIVE					0X51	// 总线接收
#define MB_SEND						0X52	// 总线发送
#define MB_NO_RESPONSE				0X53	// 主模式响应的当前查询的设备，从模式设备地址出错
#define MB_RECIVE_ERR				0X54	// 总线接收过程有出错
//---- 通信地址参数-----------------------------
#define MB_Broadcast_ADDR			0xaa	// 广播地址
#define MB_FMI_ADDR					9		
#define MB_HMI_ADDR					10		// hmi地址
#define MB_REPEAT					2		// 数据传输出错，后重新处理的次数

//=====================函数和变量声明=============================
//协议传输变量
//注: 定义的数据长度不能太大，应据相关单片机进行合理安排，否则可能出现变量溢出现象，从而跑飞
#define LENGTH_MB_DATA				140	// 11*12 +4*2
#define LENGTH_MB_PARA				287

#define DEBUG_FLOW 1


typedef struct  _MODBUS_PARA{
	unsigned char 	sRUN;					// 总线运行状态 
	unsigned char 	sERR;					// 总线出错状态 
	unsigned char 	times;					// 总线计时1
	unsigned char 	ms_50;					// 总线计时2
	unsigned char 	rCnt;					// 总线接收计数
	unsigned char 	HostRept;				// 作为主设备的重复次数
	unsigned char		mAddrs;					// 设备地址
	unsigned char		res;				//保留---凑2的倍数
	unsigned char		rBuf[LENGTH_MB_DATA];	// 待解析的接收数据
	unsigned char		tBuf[LENGTH_MB_DATA];	// 待发送数据
}MODBUS_PARA;
typedef  MODBUS_PARA 		MB_PRAR_X	;

//线圈变量
#define SUM_COIL_BIT				64		// 线圈的位个数
#define LENGTH_COIL_REG			SUM_COIL_BIT/8

//离散量输入变量
#define SUM_DiscreteREG_BIT			32		// 离散量的位个数
#define LENGTH_DiscreteREG			SUM_DiscreteREG_BIT	/8

//保持寄存器变量
#define SUM_HoldingREG_WORD		120		// 保持寄存器的个数
#define LENGTH_HoldingREG			SUM_HoldingREG_WORD*2

//输入寄存器变量
#define SUM_InputREG_WORD			20		// 保持寄存器的个数
#define LENGTH_InputREG				SUM_HoldingREG_WORD*2

//用户定义各个变量、寄存器的使用功能
#define COIL_NUM01				0
#define DisREG_NUM01			(COIL_NUM01 + SUM_COIL_BIT)
#define HoldingREG01				0
#define InputREG01				(HoldingREG01 + SUM_HoldingREG_WORD)

//HMI的通讯字地址分配
// 保持寄存器
#define 	HMI_hREG00_BasePic			0							// 画面地址
#define 	HMI_hREG09_CMD				9							// 控制命令地址

// 输入寄存器

// HMI的通讯位地址分配
// 线圈变量: 0~15it
#define	HMI_COIL_BIT0		0x01	// 
#define	HMI_COIL_BIT1		0x02	// 
#define	HMI_COIL_BIT2		0x04	// 
#define	HMI_COIL_BIT3		0x08	// 
#define	HMI_COIL_BIT4		0x10	// 
#define	HMI_COIL_BIT5		0x20	// 
#define	HMI_COIL_BIT6		0x40	// 
#define	HMI_COIL_BIT7		0x80	// 

//离散输入变量:16~71 bit : 16~23 [0]  24`31[1] 32~39 [2] 40~47 [3] 48~55 [4] 56~63 [5] 64~71 [6] 
#define	HMI_DisIN_BIT0		0x01	// 
#define	HMI_DisIN_BIT1		0x02	// 
#define	HMI_DisIN_BIT2		0x04	// 
#define	HMI_DisIN_BIT3		0x08	// 
#define	HMI_DisIN_BIT4		0x10	// 
#define	HMI_DisIN_BIT5		0x20	// 
#define	HMI_DisIN_BIT6		0x40	// 
#define	HMI_DisIN_BIT7		0x80	// 

//----变量声明----------------
EXT MB_PRAR_X		ModbusPara;
EXT unsigned char 	CoilBitPara[LENGTH_COIL_REG];
EXT unsigned char 	DiscreteBitPara[LENGTH_DiscreteREG];
EXT unsigned char 	HoldingREGPara[LENGTH_HoldingREG];
EXT unsigned char 	InputREGPara[LENGTH_InputREG];

EXT unsigned char	bTime50ms;
EXT unsigned char 	bTsave100ms;
EXT unsigned char 	bTime100ms;
EXT unsigned char 	bTime1s;
EXT unsigned char 	bHost_protocol;		//强制为1设置为主从模式，此为主机程序
EXT unsigned char 	cnt1ms;

//----函数声明----------------
EXT void ModbusInit(void);
EXT void ModbusTimesProcess(void);
EXT void ModbusSend(unsigned char length) ;
EXT void Modbus_ERROR(void) ;
EXT void MB_ReadCoilRegisters(void);
EXT void MB_ReadDiscreteRegisters(void) ;
EXT void MB_ReadHoldingRegisters(void) ;
EXT void MB_ReadInputRegisters(void) ;
EXT void MB_ForceSingleCoil(void);
EXT void MB_PresetSingleHoldingRegister(void);
EXT void MB_ForceMultipleCoil(void) ;
EXT void MB_PresetMultipleHoldingRegisters(void); 
EXT void MB_ReekoExternFunction(void);
EXT void ModbusProces(void);
//#if PROTOCOL_HOST
EXT void ModbusRequest(unsigned char dAddrs, unsigned char style, unsigned char sAdd,unsigned int val);
//#endif


#undef EXT
#endif




