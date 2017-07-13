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

	//��������
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
	{// ���߼��
		if(ModbusPara.times >= BUS_IDLE_TIME) 
		{//���߽�����У�֡������ʼ
			if(ModbusPara.sRUN == MB_RECIVE_ERR)
			{// ���չ����� �г������ݴ洢�ռ��������ʱ�䳬��T1.5
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
	{//�ȴ��Ϸ��豸��Ӧ
		if(ModbusPara.ms_50 >= NO_RESPONSE_TIME ||  
		(ModbusPara.ms_50 >=NO_RESPONSE_TIME_BC && ModbusPara.mAddrs == MB_Broadcast_ADDR))
		{//���豸��Ӧ��ʱ
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
	{// test����ʱ����
	//	TxOneData(ModbusPara.tBuf[cnt]);	
	}
	
	if(length)
	{
		ModbusPara.sRUN = MB_SEND;
		ModbusPara.times = 0;
		for(cnt=0; cnt < length; cnt++)
		{	
			usart2_sendb(ModbusPara.tBuf[cnt]);
			ModbusPara.times = 0;  //���¼�ʱ
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
			{//  ����Ҫ�ظ�����
				ModbusPara.HostRept = 0;
			}
			if(ModbusPara.HostRept == 0) ModbusPara.sERR= ERR_NOT;
		}
	
		if(ModbusPara.HostRept)
		{// �ظ���
			ModbusPara.HostRept --;
			if(ModbusPara.tBuf[1] == CHECK_COIL_bSTATUS || ModbusPara.tBuf[1] == GET_INPUT_bSTATUS 
			|| ModbusPara.tBuf[1] == GET_HOLDING_REGT || ModbusPara.tBuf[1] == GET_INPUT_REGT
			|| ModbusPara.tBuf[1] == SET_COIL_SINGLE || ModbusPara.tBuf[1] == PRESET_HOLDING_sREGT)
			{// 1������ַ+1����+2���ݵ�ַ+2�Ĵ���ֵ+2У��ֵ
				ModbusSend(8);	
			}
			else if(ModbusPara.tBuf[1] == SET_COIL_MULTIPLE || ModbusPara.tBuf[1] == PRESET_HOLDING_mREGT )
			{// 1������ַ+1����+2���ݵ�ַ+2�Ĵ�������+1�ֽ���+ �ֽ���BUF[6]��ֵ+2У��ֵ
				ModbusSend(9+ModbusPara.tBuf[6]);
			}
			else if( ModbusPara.tBuf[1] == REEKO_FUN_EXTERN)
			{
				ModbusSend(ModbusPara.tBuf[3]);	
			}
			else 
			{// ��������Ҫ�Ĺ��ܲ��ظ�
				ModbusPara.HostRept = 0;
			}
			ModbusPara.sERR= ERR_NOT;
		}
	}
	else
	{
		if(ModbusPara.sERR==  ERR_MB_FUN ||ModbusPara.sERR==  ERR_MB_ADDR 
		|| ModbusPara.sERR== ERR_MB_DATA )
		{//��ģʽ,������Ӧ����
			ModbusPara.tBuf[0] = ModbusPara.rBuf[0];			// �豸��ַ
			ModbusPara.tBuf[1] = ModbusPara.rBuf[1] | 0X80;	// ������| 0X80
			ModbusPara.tBuf[2] =	ModbusPara.sERR;			// �������
			temp16 = ModbusCRC16( &ModbusPara.tBuf[0], 3);	// ��ȡCRC
			ModbusPara.tBuf[3] = temp16 >> 8;
			ModbusPara.tBuf[4] = temp16 ;
			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR) ModbusSend(5);	
		}
		ModbusPara.sERR= ERR_NOT;
	}
}

void MB_ReadCoilRegisters(void) 
{ // ������1
	unsigned short addr_data,reg_num;
	unsigned char  i,k,byteCount,temp8; 
	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num /8;
		if( reg_num%8) byteCount ++;			// �ֽ���
		if(ModbusPara.rBuf[2] <= LENGTH_COIL_REG && byteCount == ModbusPara.rBuf[2])
		{//���ص��ֽ����ж�
			addr_data = ModbusPara.tBuf[2];	// ��ʼ��ַ
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			reg_num += addr_data;			//���� ��Ȧ��= ��ʼ ��Ȧ��+ ���� 
			for (k = 0 ; k < byteCount ; k++) 
			{ // �洢���豸��ѯ��״̬λ
				for (i = 0 ; i < 8 ; i++) 
				{ 
					temp8 = 1;
					temp8 <<= addr_data%8;					//Ѱ�Ҷ�Ӧ��BITλ��
					if(ModbusPara.rBuf[k + 3] & (1<< i))
					{
						CoilBitPara[addr_data/8] |= temp8;		// ����λ��1
					}
					else 
					{
						 temp8 = ~ temp8;						// ȡ��
						 CoilBitPara[addr_data/8] &= temp8;		// ����λ����
					}
					addr_data ++;
					if(addr_data >= reg_num) break;			// ���Ĳ�������
					else if(addr_data >= SUM_COIL_BIT)
					{// ���ݴ�������г���
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				} 
			} 
		}
		else
		{// ����ϵͳ�涨��Ȧ������,���߷��ص��ֽ�����ʵ�ʷ��͵Ķ�Ӧ����
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_COIL_BIT)
		{// ��Ȧ�����ж�OK
			if(addr_data < SUM_COIL_BIT && (addr_data + reg_num) <= SUM_COIL_BIT)
			{// λ��ַ��Χ�ж�OK,��ʼ��Ӧ����
				byteCount = reg_num /8;
				if( reg_num%8) byteCount ++;
				reg_num += addr_data;		//���� ��Ȧ��= ��ʼ ��Ȧ��+ ���� 
				for (k = 0 ; k < byteCount ; k++) 
				{ // ��ȡ���豸��Ҫ��״̬λ,�������״̬
					ModbusPara.tBuf[k + 3] = 0;
					for (i = 0 ; i < 8 ; i++) 
					{ 
						temp8 = 1;
						temp8 <<= addr_data%8;					//Ѱ�Ҷ�Ӧ��BITλ��
						temp8 &=  CoilBitPara[addr_data/8];			// ��ȡ��λ�ŵ�״̬
						if(temp8) ModbusPara.tBuf[k + 3] |= (1<< i);	// =1�����µ�BITλ����1
						addr_data ++;
						if(addr_data >= reg_num) break;			// ���Ĳ�������
						else if(addr_data >= SUM_COIL_BIT)
						{// ���ݴ�������г���
							ModbusPara.sERR= ERR_MB_DATA;
							break;
						}
					} 
				} 
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
				ModbusPara.tBuf[2] = byteCount; 					// �ֽ���
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				ModbusSend(byteCount );
			}
			else
			{// ����ϵͳ�涨��Ȧ��λ��ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨��Ȧ������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ReadDiscreteRegisters(void) 
{ // ������2
	unsigned short addr_data,reg_num;
	unsigned char  i,k,byteCount,temp8; 
	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num /8;
		if( reg_num%8) byteCount ++;			// �ֽ���
		if(ModbusPara.rBuf[2] <= LENGTH_DiscreteREG && byteCount == ModbusPara.rBuf[2])
		{//���ص��ֽ����ж�
			addr_data = ModbusPara.tBuf[2];	// ��ʼ��ַ
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			addr_data -= SUM_COIL_BIT;
			
			reg_num += addr_data;			//������ɢ����λ= ��ʼ��ɢ����λ+ ����
			for (k = 0 ; k < byteCount ; k++) 
			{ // �洢���豸��ѯ��״̬λ
				for (i = 0 ; i < 8 ; i++) 
				{ 
					temp8 = 1;
					temp8 <<= addr_data%8;					//Ѱ�Ҷ�Ӧ��BITλ��
					if(ModbusPara.rBuf[k + 3] & (1<< i))
					{
						DiscreteBitPara[addr_data/8] |= temp8;	// ����λ��1
					}
					else 
					{
						 temp8 = ~ temp8;						// ȡ��
						 DiscreteBitPara[addr_data/8] &= temp8;// ����λ����
					}
					addr_data ++;
					if(addr_data >= reg_num) break;			// ���Ĳ�������
					else if(addr_data >= SUM_DiscreteREG_BIT)
					{// ���ݴ�������г���
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				} 
			} 
		}
		else
		{// ����ϵͳ�涨��ɢ��������,���߷��ص��ֽ�����ʵ�ʷ��͵Ķ�Ӧ����
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		addr_data -= SUM_COIL_BIT;
			
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_DiscreteREG_BIT)
		{// ��ɢ�������ж�OK
			if(addr_data < SUM_DiscreteREG_BIT && (addr_data + reg_num) <= SUM_DiscreteREG_BIT)
			{// λ��ַ��Χ�ж�OK,��ʼ��Ӧ����
				byteCount = reg_num /8;
				if( reg_num%8) byteCount ++;
				reg_num += addr_data;		//������ɢ����λ= ��ʼ��ɢ����λ+ ����
				for (k = 0 ; k < byteCount ; k++) 
				{ // ��ȡ���豸��Ҫ��״̬λ,�������״̬
					ModbusPara.tBuf[k + 3] = 0;
					for (i = 0 ; i < 8 ; i++) 
					{ 
						temp8 = 1;
						temp8 <<= addr_data%8;					//Ѱ�Ҷ�Ӧ��BITλ��
						temp8 &=  DiscreteBitPara[addr_data/8];	// ��ȡ��λ�ŵ�״̬
						if(temp8) ModbusPara.tBuf[k + 3] |= (1<< i);	// =1�����µ�BITλ����1
						addr_data ++;
						if(addr_data >= reg_num) break;			// ���Ĳ�������
						else if(addr_data >= SUM_DiscreteREG_BIT)
						{// ���ݴ�������г���
							ModbusPara.sERR= ERR_MB_DATA;
							break;
						}
					} 
				} 
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
				ModbusPara.tBuf[2] = byteCount; 					// �ֽ���
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
			}
			else
			{// ����ϵͳ�涨��ɢ����λ��ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨��ɢ��������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ReadHoldingRegisters(void) 
{ // ������3
	unsigned short addr_data,reg_num;
	unsigned char  k,byteCount; 

	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		//�������---���÷��͵�����
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num *2;				// �ֽ���= 	�������*2
		if(ModbusPara.rBuf[2] <= LENGTH_HoldingREG && byteCount == ModbusPara.rBuf[2])
		{//���ص��ֽ����ж�
			addr_data = ModbusPara.tBuf[2];	// ���ּĴ�����---���÷��͵�����
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			addr_data <<= 1;					// ���ּĴ����Ŷ�Ӧ������ʼ��ַ
			reg_num = addr_data + byteCount;	//����  ���ּĴ�����= ��ʼ���ּĴ�����+ ���� 
			for (k = 0 ; k < byteCount ; k++) 
			{ // �洢���豸��ѯ�ı��ּĴ�������
				 HoldingREGPara[addr_data] = ModbusPara.rBuf[k + 3];
				 addr_data++;
				if(addr_data >= LENGTH_HoldingREG|| addr_data >= reg_num )
				{// ���ݴ�������г���
					ModbusPara.sERR= ERR_MB_DATA;
					break;
				}
			} 
		}
		else
		{// ����ϵͳ�涨���ּĴ���������,���߷��ص��ֽ�����ʵ�ʷ��͵Ķ�Ӧ����
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
		{// ���ּĴ��������ж�OK
			if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
			{// ���ּĴ�����ַ��Χ�ж�OK,��ʼ��Ӧ����
				byteCount = reg_num *2;			// �ֽ���= 	�������*2
				reg_num = addr_data + byteCount ;	//����  ���ּĴ�����= ��ʼ���ּĴ�����+ ���� 
				for (k = 0 ; k < byteCount ; k++) 
				{ // ��ȡ���豸��Ҫ�ı��ּĴ�������
					ModbusPara.tBuf[k + 3] =  HoldingREGPara[addr_data];
					ModbusPara.tBuf[k + 3] = 0x02;		// test
					addr_data++;
					if(addr_data >= LENGTH_HoldingREG|| addr_data >= reg_num )
					{// ���ݴ�������г���
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				}
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
				ModbusPara.tBuf[2] = byteCount; 					// �ֽ���
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
			}
			else
			{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨���ּĴ���������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ReadInputRegisters(void) 
{ // ������4
	unsigned short addr_data,reg_num;
	unsigned char  k,byteCount; 
	
	if(bHost_protocol)
	{
		reg_num = ModbusPara.tBuf[4];		// �������
		reg_num <<= 8;
		reg_num += ModbusPara.tBuf[5];
		byteCount = reg_num *2;				 // �ֽ���=  �������*2	
		if(ModbusPara.rBuf[2] <= LENGTH_InputREG && byteCount == ModbusPara.rBuf[2])
		{//���ص��ֽ����ж�
			addr_data = ModbusPara.tBuf[2];	// ��ʼ ����Ĵ�����
			addr_data <<= 8;
			addr_data += ModbusPara.tBuf[3];
			addr_data -= SUM_HoldingREG_WORD;
			reg_num = addr_data + byteCount;	//����  ����Ĵ�����= ��ʼ ����Ĵ�����+ ���� 
			for (k = 0 ; k < byteCount ; k++) 
			{ // �洢���豸��ѯ�ı��ּĴ�������
				 InputREGPara[addr_data] = ModbusPara.tBuf[k + 3];
				 addr_data++;
				if(addr_data >= LENGTH_InputREG|| addr_data >= reg_num )
				{// ���ݴ�������г���
					ModbusPara.sERR= ERR_MB_DATA;
					break;
				}
			} 
		}
		else
		{// ����ϵͳ�涨����Ĵ���������,���߷��ص��ֽ�����ʵ�ʷ��͵Ķ�Ӧ����
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		addr_data -= SUM_HoldingREG_WORD;
		
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_InputREG_WORD)
		{// ����Ĵ��������ж�OK
			if(addr_data < SUM_InputREG_WORD && (addr_data + reg_num) <= SUM_InputREG_WORD)
			{//����Ĵ�����ַ��Χ�ж�OK,��ʼ��Ӧ����
				byteCount = reg_num *2;			//�ֽ���=  �������*2	
				reg_num = addr_data + byteCount;	//����  ����Ĵ�����= ��ʼ ����Ĵ�����+ ���� 
				for (k = 0 ; k < byteCount ; k++) 
				{ // ��ȡ���豸��Ҫ������Ĵ�������
					ModbusPara.tBuf[k + 3] =  InputREGPara[addr_data];
					addr_data++;
					if(addr_data >= LENGTH_InputREG|| addr_data >= reg_num )
					{// ���ݴ�������г���
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				}
				ModbusPara.tBuf[0] = ModbusPara.rBuf[0]; 			// �豸��ַ
				ModbusPara.tBuf[1] = ModbusPara.rBuf[1];  			// ������
				ModbusPara.tBuf[2] = byteCount; 					// �ֽ���
				byteCount += 3; 
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], byteCount);	// ��ȡCRC
				ModbusPara.tBuf[byteCount] = reg_num >> 8;
				byteCount ++; 
				ModbusPara.tBuf[byteCount] = reg_num ;
				byteCount ++; 
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(byteCount );
			}
			else
			{// ����ϵͳ�涨����Ĵ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨����Ĵ���������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
} 

void MB_ForceSingleCoil(void)
{ // ������5
	unsigned short addr_data,ON_OFF;
	unsigned char  temp8; 
	
	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		if(addr_data < SUM_COIL_BIT )
		{// λ��ַ��Χ�ж�OK,��ʼ��Ӧ����
			if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
			|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
			{// ���ص���Ҫ��Ĳ�һ��(�����Ժ�С)
				ModbusPara.sERR= ERR_MB_DEVICE;
			}
		}
		else
		{// ����ϵͳ�涨��Ȧ������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		ON_OFF = ModbusPara.tBuf[4];			//���ֵ
		ON_OFF <<= 8;
		ON_OFF += ModbusPara.tBuf[5];
		if(addr_data < SUM_COIL_BIT  && ON_OFF == 0xff00 && ON_OFF == 0x0000)
		{// λ��ַ��Χ�ж�OK,��ʼ��Ӧ����
			temp8 = 1;
			temp8 <<= addr_data%8;					//Ѱ�Ҷ�Ӧ��BITλ��
			if(ON_OFF == 0x0000)
			{
				temp8 = ~temp8;						// ȡ��
				 CoilBitPara[addr_data/8] &= temp8;		// ��ȡ��λ�ŵ�״̬:OFF
			}
			else
			{
				CoilBitPara[addr_data/8] |= temp8;		// ��ȡ��λ�ŵ�״̬:ON
			}
	
			for(temp8 =0 ;temp8 < 8; temp8++)
			{
				ModbusPara.tBuf[temp8] = ModbusPara.rBuf[temp8]; 	
			}
			ModbusSend(8 );
	
		}
		else
		{// ����ϵͳ�涨��Ȧ������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
}

void MB_PresetSingleHoldingRegister(void)
{// ������6
	unsigned short addr_data;
	unsigned char  temp8 ;

	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//�������
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];	
		if(addr_data < SUM_HoldingREG_WORD )
		{// ���ּĴ�����ַ��Χ�ж�OK,��ʼ��Ӧ����
			if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
			|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
			{// ���ص���Ҫ��Ĳ�һ��(�����Ժ�С)
				ModbusPara.sERR= ERR_MB_DEVICE;
			}
			else
			{// ��ȡд��ֵ
				addr_data <<= 1;  // ���ּĴ����Ŷ�Ӧ������ʼ��ַ
				HoldingREGPara[addr_data] = ModbusPara.rBuf[4];
				HoldingREGPara[addr_data + 1] = ModbusPara.rBuf[5];
			}
		}
		else
		{// ����ϵͳ�涨���ּĴ�����ַ��Χ
			ModbusPara.sERR= ERR_MB_ADDR;
		}
	}
	else
 	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		if(addr_data < SUM_HoldingREG_WORD )
		{// ���ּĴ�����ַ��Χ�ж�OK,��ʼ��Ӧ����
			HoldingREGPara[addr_data] = ModbusPara.rBuf[4];
			HoldingREGPara[addr_data + 1] = ModbusPara.rBuf[5];
			for(temp8 =0 ;temp8 < 8; temp8++)
			{
				ModbusPara.tBuf[temp8] = ModbusPara.rBuf[temp8]; 	
			}
			if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(8 );
		}
		else
		{// ����ϵͳ�涨���ּĴ�����ַ��Χ
			ModbusPara.sERR= ERR_MB_ADDR;
		}	
	}
}

void MB_ForceMultipleCoil(void) 
{ // ������15
	unsigned short addr_data,reg_num;
	unsigned char  i,k,byteCount,temp8; 

	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_COIL_BIT)
		{// ��Ȧ�����ж�OK
			if(addr_data < SUM_COIL_BIT && (addr_data + reg_num) <= SUM_COIL_BIT)
			{// λ��ַ��Χ�ж�OK,��ʼ��Ӧ����
				if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
				|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
				{// ���ص���Ҫ��Ĳ�һ��(�����Ժ�С)
					ModbusPara.sERR = ERR_MB_DEVICE;
				}
			}
			else
			{// ����ϵͳ�涨��Ȧ��λ��ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨��Ȧ������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_COIL_BIT)
		{// ��Ȧ�����ж�OK
			if(addr_data < SUM_COIL_BIT && (addr_data + reg_num) <= SUM_COIL_BIT)
			{// λ��ַ��Χ�ж�OK,��ʼ��Ӧ����
				byteCount = ModbusPara.rBuf[6];	// �ֽ���
				reg_num += addr_data;			//���� ��Ȧ��= ��ʼ ��Ȧ��+ ���� 
				for (k = 0 ; k < byteCount ; k++) 
				{ // ���豸�洢���豸ָ����״̬λ
					for (i = 0 ; i < 8 ; i++) 
					{ 
						temp8 = 1;
						temp8 <<= addr_data%8;					//Ѱ�Ҷ�Ӧ��BITλ��
						if(ModbusPara.rBuf[k + 7] & (1<< i))
						{
							CoilBitPara[addr_data/8] |= temp8;		// ����λ��1
						}
						else 
						{
							 temp8 = ~ temp8;						// ȡ��
							 CoilBitPara[addr_data/8] &= temp8;		// ����λ����
						}
						addr_data ++;
						if(addr_data >= reg_num) break;			// ���Ĳ�������
						else if(addr_data >= SUM_COIL_BIT)
						{// ���ݴ�������г���
							ModbusPara.sERR= ERR_MB_DATA;
							break;
						}
					} 
				} 
	
				for(temp8 = 0; temp8 < 6; temp8++)
				{
					ModbusPara.tBuf[temp8] = ModbusPara.rBuf[temp8]; 
				}
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], 6);	// ��ȡCRC
				ModbusPara.tBuf[6] = reg_num >> 8;
				ModbusPara.tBuf[7] = reg_num ;
				ModbusSend(8);
			}
			else
			{// ����ϵͳ�涨��Ȧ��λ��ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨��Ȧ������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
}

void MB_PresetMultipleHoldingRegisters(void)
{ // ������16
	unsigned short addr_data,reg_num;
	unsigned char  k,byteCount; 
	
	if(bHost_protocol)
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
		{// ���ּĴ��������ж�OK
			if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
			{// ���ּĴ�����ַ��Χ�ж�OK,��ʼ��Ӧ����?
				if( ModbusPara.rBuf[2] != ModbusPara.tBuf[2] || ModbusPara.rBuf[3] != ModbusPara.tBuf[3]
				|| ModbusPara.rBuf[4] != ModbusPara.tBuf[4] || ModbusPara.rBuf[5] != ModbusPara.tBuf[5])
				{// ���ص���Ҫ��Ĳ�һ��(�����Ժ�С)
					ModbusPara.sERR= ERR_MB_DEVICE;
				}
			}
			else
			{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨���ּĴ���������
			ModbusPara.sERR= ERR_MB_DATA;
		}
	}
	else
	{
		addr_data = ModbusPara.rBuf[2];		//��ʼ��ַ
		addr_data <<= 8;
		addr_data += ModbusPara.rBuf[3];
		reg_num = ModbusPara.rBuf[4];		//�������
		reg_num <<= 8;
		reg_num += ModbusPara.rBuf[5];
		if(reg_num >= 1 && reg_num <= SUM_HoldingREG_WORD)
		{// ���ּĴ��������ж�OK
			if(addr_data < SUM_HoldingREG_WORD && (addr_data + reg_num) <= SUM_HoldingREG_WORD)
			{// ���ּĴ�����ַ��Χ�ж�OK,��ʼ��Ӧ����
				byteCount = ModbusPara.rBuf[6];	// �ֽ���= �������*2
				reg_num = addr_data + byteCount;	//���� ���ּĴ�����= ��ʼ ���ּĴ�����+ ���� 
				for (k = 0 ; k < byteCount ; k++) 
				{ // ��ȡ���豸��Ҫ�ı��ּĴ�������
					HoldingREGPara[addr_data] = ModbusPara.rBuf[7+k];
					addr_data++;
					if(addr_data >= LENGTH_HoldingREG || addr_data >= reg_num )
					{// ���ݴ�������г���
						ModbusPara.sERR= ERR_MB_DATA;
						break;
					}
				}
				for(k = 0; k < 6; k++)
				{
					ModbusPara.tBuf[k] = ModbusPara.rBuf[k]; 
				}
				reg_num = ModbusCRC16( &ModbusPara.tBuf[0], 6);	// ��ȡCRC
				ModbusPara.tBuf[6] = reg_num >> 8;
				ModbusPara.tBuf[7] = reg_num ;
				if(ModbusPara.tBuf[0] != MB_Broadcast_ADDR)  ModbusSend(8);
			}
			else
			{// ����ϵͳ�涨���ּĴ�����ַ��Χ���߶�ȡ��������ʼ��ַ��ƥ�����
				ModbusPara.sERR= ERR_MB_ADDR;
			}	
		}
		else
		{// ����ϵͳ�涨���ּĴ���������
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
	{// ���߿��У����Խ������ݵĴ���	
		if (ModbusPara.rCnt >= 4 )
		{//  1���ֽڵ��豸��ַ��1���ֽڵĹ����룬N���������ݣ�2���ֽڵ�CRC16
			#if DEBUG_FLOW
			Debug_Send(0xbb);
			for(i=0;i<ModbusPara.rCnt ;i++)
			{
				Debug_Send(ModbusPara.rBuf[i]);
			}
			#endif	 	
			if(ModbusCRC16(&ModbusPara.rBuf[0], ModbusPara.rCnt ) ==0)
			{//����У��û�д���
//				ComLED	= !ComLED;
				if(ModbusPara.rBuf[1] & 0x80 && bHost_protocol)
				{// ���صĹ������д�
					ModbusPara.sERR = ModbusPara.rBuf[2];
				}
				else
				{
					ModbusPara.HostRept = 0;	// ���ճɹ���ʣ���ظ��Ĵ�������
					switch(ModbusPara.rBuf[1])
					{// �������ж�
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
							ModbusPara.sERR=  ERR_MB_FUN;	// ���豸��֧�ָù���,���豸ǰ���Ѿ��ж�
							break;
					}
				}
			}
			else
			{//����У���д���,���ʾ�ڴ�����������ݱ����š�
				ModbusPara.sERR= ERR_MB_DEVICE;
			}
			ModbusPara.rCnt  = 0;	// ���㣬׼�������µ����ݽ��ա�
		}
		else if(ModbusPara.rCnt) 
		{
			ModbusPara.rCnt  = 0;	// ����С��4������������
		}

		Modbus_ERROR();
	} 
} 

void ModbusRequest(unsigned char dAddrs, unsigned char style, unsigned char sAdd,unsigned short val)
{//���� ������������
	unsigned short temp16;
	unsigned char i,k; 
	delay_ms(15);
	if(ModbusPara.sRUN ==  MB_IDEL && ModbusPara.HostRept == 0 && ModbusPara.rCnt == 0)
	{
		ModbusPara.HostRept = MB_REPEAT;

		ModbusPara.mAddrs = dAddrs;								// �� �豸��ַ
		ModbusPara.tBuf[0] =  dAddrs;							// �� �豸��ַ
		ModbusPara.tBuf[1] =  style;							// ������
		if(style == FUN_EXTERN)
		{// ��չ����



		}
		else	   
		{// MODBUSЭ���ڵĹ���
			ModbusPara.tBuf[2] = 0x00;						// ������ʼ��ַ---��
			ModbusPara.tBuf[3] = sAdd;						// ������ʼ��ַ---��				 
			ModbusPara.tBuf[4] = (unsigned char )(val >> 8);		// ��������������Ĵ���ֵ
			ModbusPara.tBuf[5] = val;
			
			if(style == SET_COIL_MULTIPLE)
			{// ǿ�ƶ��λ���
				ModbusPara.tBuf[6] = val/8;	// �ֽ���
				if(val%8) ModbusPara.tBuf[6] ++;
				for (k = 0 ; k < ModbusPara.tBuf[6]; k++)
				{ // ��ȡָ����״̬λ
					ModbusPara.tBuf[7+k] = 0;
					for (i = 0 ; i < 8 ; i++) 
					{ 
						ModbusPara.tBuf[7+k] >>= 1;
						if(val) 
						{
							val --;
							style = 1;
							style <<= sAdd%8;				//Ѱ�Ҷ�Ӧ��BITλ��
						}
						else style = 0;						// �����
						
						if(CoilBitPara[sAdd/8] & style)
						{
							ModbusPara.tBuf[7+k] |= 0x80;	// ����λ��1
						}
						sAdd ++;
					} 
				} 
				sAdd = ModbusPara.tBuf[6] + 7;
			}
			else if(style == PRESET_HOLDING_mREGT)
			{//Ԥ�ö�����ּĴ���
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
		
		temp16 = ModbusCRC16( &ModbusPara.tBuf[0], sAdd);	// ��ȡCRC
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







