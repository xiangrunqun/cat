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

  
//======================  �����궨��=================================

//---- ����֧��MODBUS���ܴ���--------------
#define CHECK_COIL_bSTATUS			1 		//  �����λ״̬
#define GET_INPUT_bSTATUS			2 		//  ������λ״̬
#define GET_HOLDING_REGT			3		//  �����ּĴ���
#define GET_INPUT_REGT				4		//  ������Ĵ���
#define SET_COIL_SINGLE				5		//  ǿ�õ���Ȧ	 
#define PRESET_HOLDING_sREGT		6		//  Ԥ�õ������ּĴ���
#define SET_COIL_MULTIPLE			15		//  ǿ�ƶ��λ���
#define PRESET_HOLDING_mREGT		16		//  Ԥ�ö�����ּĴ���
#define REEKO_FUN_EXTERN			65		//  REEKO��չ����
//---- �������Ĵ���ֵ-------------------------
#define ERR_NOT						0	
#define ERR_MB_FUN					1		// ���Ϸ����ܴ���
#define ERR_MB_ADDR					2		// ���Ϸ����ݵ�ַ
#define ERR_MB_DATA					3		// ���Ϸ����ݣ�����ĳ���ܵļ���ֵ
#define ERR_MB_DEVICE				4		// ��ģʽ��Ӧ�����ȴ�ʱ�䣻��ģʽ�豸��ַ���������ַ����ʱ�� ����1.5���ַ�
//---- ͨ��ʱ�����-----------------------------
#define ONE_DATA_BIT				10		// ���÷�ʽ1:10bit
#define BAUD_RATE_MODBUS			19200L	// MODBUS������
#define MODBUS_TIME					260		// 260uS	= (10/19200)/2
/*******************************************************
���ʱ������Ϊ������ʾ��8��4��5��5
���ù����Ļ�����ֶ�ȡ�޻ظ�����
********************************************************/
#define BUS_IDLE_TIME				8//8	// ֡����(��ʼ�����)�� ����3.5~4�������ַ�ʱ��
#define FRAME_ERR_TIME				4	
#define NO_RESPONSE_TIME			5//5	// ͨѶ�жϳ�ʱ Լ500ms//1.5s
#define NO_RESPONSE_TIME_BC			5//5	// ͨѶ�жϳ�ʱ Լ500Ms
//---- ͨ��״̬����-----------------------------
#define MB_IDEL						0X00	// ���߿��С�����֡����ʼ֡
#define MB_WAIT_HOST				0X01	// ���豸�ȴ���Ӧ
#define MB_BUSY						0X50	// ����æ
#define MB_RECIVE					0X51	// ���߽���
#define MB_SEND						0X52	// ���߷���
#define MB_NO_RESPONSE				0X53	// ��ģʽ��Ӧ�ĵ�ǰ��ѯ���豸����ģʽ�豸��ַ����
#define MB_RECIVE_ERR				0X54	// ���߽��չ����г���
//---- ͨ�ŵ�ַ����-----------------------------
#define MB_Broadcast_ADDR			0xaa	// �㲥��ַ
#define MB_FMI_ADDR					9		
#define MB_HMI_ADDR					10		// hmi��ַ
#define MB_REPEAT					2		// ���ݴ�����������´���Ĵ���

//=====================�����ͱ�������=============================
//Э�鴫�����
//ע: ��������ݳ��Ȳ���̫��Ӧ����ص�Ƭ�����к����ţ�������ܳ��ֱ���������󣬴Ӷ��ܷ�
#define LENGTH_MB_DATA				140	// 11*12 +4*2
#define LENGTH_MB_PARA				287

#define DEBUG_FLOW 1


typedef struct  _MODBUS_PARA{
	unsigned char 	sRUN;					// ��������״̬ 
	unsigned char 	sERR;					// ���߳���״̬ 
	unsigned char 	times;					// ���߼�ʱ1
	unsigned char 	ms_50;					// ���߼�ʱ2
	unsigned char 	rCnt;					// ���߽��ռ���
	unsigned char 	HostRept;				// ��Ϊ���豸���ظ�����
	unsigned char		mAddrs;					// �豸��ַ
	unsigned char		res;				//����---��2�ı���
	unsigned char		rBuf[LENGTH_MB_DATA];	// �������Ľ�������
	unsigned char		tBuf[LENGTH_MB_DATA];	// ����������
}MODBUS_PARA;
typedef  MODBUS_PARA 		MB_PRAR_X	;

//��Ȧ����
#define SUM_COIL_BIT				64		// ��Ȧ��λ����
#define LENGTH_COIL_REG			SUM_COIL_BIT/8

//��ɢ���������
#define SUM_DiscreteREG_BIT			32		// ��ɢ����λ����
#define LENGTH_DiscreteREG			SUM_DiscreteREG_BIT	/8

//���ּĴ�������
#define SUM_HoldingREG_WORD		120		// ���ּĴ����ĸ���
#define LENGTH_HoldingREG			SUM_HoldingREG_WORD*2

//����Ĵ�������
#define SUM_InputREG_WORD			20		// ���ּĴ����ĸ���
#define LENGTH_InputREG				SUM_HoldingREG_WORD*2

//�û���������������Ĵ�����ʹ�ù���
#define COIL_NUM01				0
#define DisREG_NUM01			(COIL_NUM01 + SUM_COIL_BIT)
#define HoldingREG01				0
#define InputREG01				(HoldingREG01 + SUM_HoldingREG_WORD)

//HMI��ͨѶ�ֵ�ַ����
// ���ּĴ���
#define 	HMI_hREG00_BasePic			0							// �����ַ
#define 	HMI_hREG09_CMD				9							// ���������ַ

// ����Ĵ���

// HMI��ͨѶλ��ַ����
// ��Ȧ����: 0~15it
#define	HMI_COIL_BIT0		0x01	// 
#define	HMI_COIL_BIT1		0x02	// 
#define	HMI_COIL_BIT2		0x04	// 
#define	HMI_COIL_BIT3		0x08	// 
#define	HMI_COIL_BIT4		0x10	// 
#define	HMI_COIL_BIT5		0x20	// 
#define	HMI_COIL_BIT6		0x40	// 
#define	HMI_COIL_BIT7		0x80	// 

//��ɢ�������:16~71 bit : 16~23 [0]  24`31[1] 32~39 [2] 40~47 [3] 48~55 [4] 56~63 [5] 64~71 [6] 
#define	HMI_DisIN_BIT0		0x01	// 
#define	HMI_DisIN_BIT1		0x02	// 
#define	HMI_DisIN_BIT2		0x04	// 
#define	HMI_DisIN_BIT3		0x08	// 
#define	HMI_DisIN_BIT4		0x10	// 
#define	HMI_DisIN_BIT5		0x20	// 
#define	HMI_DisIN_BIT6		0x40	// 
#define	HMI_DisIN_BIT7		0x80	// 

//----��������----------------
EXT MB_PRAR_X		ModbusPara;
EXT unsigned char 	CoilBitPara[LENGTH_COIL_REG];
EXT unsigned char 	DiscreteBitPara[LENGTH_DiscreteREG];
EXT unsigned char 	HoldingREGPara[LENGTH_HoldingREG];
EXT unsigned char 	InputREGPara[LENGTH_InputREG];

EXT unsigned char	bTime50ms;
EXT unsigned char 	bTsave100ms;
EXT unsigned char 	bTime100ms;
EXT unsigned char 	bTime1s;
EXT unsigned char 	bHost_protocol;		//ǿ��Ϊ1����Ϊ����ģʽ����Ϊ��������
EXT unsigned char 	cnt1ms;

//----��������----------------
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




