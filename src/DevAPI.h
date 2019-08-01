#ifndef __DEVAPI_H__
#define __DEVAPI_H__

#include "Common.h"
#include "PciTypes.h"

class Device 
{
private:
	static bool FetchDpath(int index, wchar_t* dpath, int len);					//��ȡ�豸�ӿ��еķ��������ַ���
	static int  FetchDid(int vid, wchar_t* dpath, int len);						//��ȡDID(�豸ID��)
	static int  FetchId(wchar_t* dpath, wchar_t* prefix);						//����ʵ����ȡDID(�豸ID��)

public:
	/**��ָ���豸����, ����ͨ��IoRegisterDeviceInterface����ע���豸�ӿ��������豸**/
	static int  Open(int vid, int did);
	static void Close(int handle);

	/**ͨ��IoCreateDevice����ָ���豸�����������豸**/
	//�򿪡��رա������豸�����ӿ�
	static int HJPCI_DeviceOpen(TAG_DEVICE_LOCATION *pDevice, HANDLE *pHandle);							//���豸
	static int HJPCI_DeviceFind(TAG_DEVICE_LOCATION *pDevice, U32 *pRequestLimit);						//����ָ���������豸
	static int HJPCI_DeviceClose(HANDLE hDevice);														//�ر��豸

	//�ڲ��Ĵ��������ӿ�(���BAR0), ֻΪ�������ţ������û�����
	static int HJRegisterRead(IN int drvHandle, IN unsigned int registerOffset, 
		OUT unsigned int * registerVal);																//�������Ĵ���
	static int HJRegisterWrite(IN int drvHandle, IN unsigned int registerOffset, 
		IN const unsigned int * registerVal);															//д�����Ĵ���
	static int HJRegisterReadAll(IN int drvHandle, IN unsigned int startOffset, 
		IN unsigned int registerCout,OUT unsigned int * regValBuf);										//��ȡ��������ļĴ���
	static int HJRegisterWriteAll(IN int drvHandle, IN unsigned int startOffset,
		IN unsigned int registerCout,IN  const unsigned int * regValBuf);								//д���������ļĴ���

	//�ڲ��Ĵ��������ӿ�(���BAR1~BAR5)�����û�����
	static int HJUserRegisterRead(IN int drvHandle, IN unsigned int barRegisterNumber,
		IN unsigned int registerOffset, OUT unsigned int * registerVal);								//�������Ĵ���
	static int HJUserRegisterWrite(IN int drvHandle, IN unsigned int barRegisterNumber,
		IN unsigned int registerOffset, IN const unsigned int * registerVal);							//д�����Ĵ���
	static int HJUserRegisterReadAll(IN int drvHandle,IN unsigned int barRegisterNumber,
		IN unsigned int startOffset, IN unsigned int registerCout,OUT unsigned int * regValBuf);		//��ȡ��������ļĴ���
	static int HJUserRegisterWriteAll(IN int drvHandle, IN unsigned int barRegisterNumber,
		IN unsigned int startOffset, IN unsigned int registerCout,IN  const unsigned int * regValBuf);	//д���������ļĴ���

	//SGL(��ɢ�ۺ���ʽ)DMA�����ӿ�
	static int HJDmaSglChannelEnable(IN int drvHandle, IN int dmaChannel);								//����SGL(��ɢ�ۺ���ʽ)DMA	
	static int HJDmaSglChannelDisable(IN int drvHandle, IN int dmaChannel);								//ֹͣSGL(��ɢ�ۺ���ʽ)DMA����
	static int HJDmaSglChannelReset(IN int drvHandle, IN int dmaChannel);								//��λSGL(��ɢ�ۺ���ʽ)DMA							
	static int HJDmaSglChannelSetTranType(IN int drvHandle, IN int dmaChannel, IN int type);			//�趨SGL(��ɢ�ۺ���ʽ)DMA��������, 0:FIFO, 1:������д�ļ�
	static int HJDmaSglChannelSetTranDir(IN int drvHandle, IN int dmaChannel, IN int dir);				//�趨SGL(��ɢ�ۺ���ʽ)DMA���䷽��, 0:����, 1:����
	static int HJDmaSglChannelSetDmaSize(IN int drvHandle, IN int dmaChannel, IN int size);				//����SGL(��ɢ�ۺ���ʽ)DMAÿ��DMA�鴫��Ĵ�С
	static int HJDmaSglChannelSetDmaCount(IN int drvHandle, IN int dmaChannel, IN int count);			//����SGL(��ɢ�ۺ���ʽ)DMA�������DMA����		
	static int HJDmaSglChannelSetFilePath(IN int drvHandle, IN int dmaChannel,					
		IN TAG_UPDW_FILEPATH filePath);																	//����SGL(��ɢ�ۺ���ʽ)DMA��������к������ļ�·��������

	//��ȡ����ַ�Ĵ���(BAR0~BAR5)��Ϣ�ӿ�
	static int HJPciBarRangeGet(IN int drvHandle, IN unsigned int barRegisterNumber, 
		OUT unsigned int * data);																		//��ȡָ���Ļ���ַ�Ĵ������ڴ淶Χ
	static int HJPciBaseAddressesGet(IN int drvHandle, OUT PTAG_VIRTUAL_ADDRESSES virtAddr);			//��ȡPCIE�豸����BAR���û������ַ
	
	//��ȡ�汾��Ϣ�ӿ�
	static int HJPci_ApiVersion(
		OUT U8 *pVersionMajor,
		OUT U8 *pVersionMinor,
		OUT U8 *pVersionRevison);		//��ȡSDK API�汾��Ϣ
	static int HJPci_DriverVersion(
		IN int drvHandle,
		OUT U8 *pVersionMajor,
		OUT U8 *pVersionMinor,
		OUT U8 *pVersionRevision);		//��ȡPCIE�����汾��Ϣ	

	//�������üĴ����ӿ�
	static int HJPci_ConfigRegisterRead(
		IN U8  bus,
		IN U8  slot,
		IN U16 offset,
		OUT U32 * pData);				//�����üĴ�������
	static int HJPci_ConfigRegisterWrite( 
		IN U8  bus,
		IN U8  slot,
		IN U16 offset,
		IN U32 * pData);				//д���üĴ�������

	//Ӧ�ó�������������֮������¼������Ĳ����ӿ�
	static int HJPci_AddRefEvent(IN int drvHandle, IN int dmaChannel, IN HANDLE tevent);				//���Ӧ�ó����������������н������¼�����
	static int HJPci_DelRefEvent(IN int drvHandle, IN int dmaChannel);									//ɾ��Ӧ�ó�������������佻�����¼�����
	//FIFO��ʽ�½������ݴ���(������ȡ��д��)�Ĳ����ӿ�
	static int HJPci_DataRead(IN int drvHandle, IN int dmaChannel, OUT unsigned int* buf, IN int len);	//��DMA������ͨ��FIFO��ʽ��ȡ����
	static int HJPci_DataWrite(IN int drvHandle, IN int dmaChannel, IN unsigned int* buf, IN int len);	//��DMA������ͨ��FIFO��ʽд������
	//��ȡ״ֵ̬
	static int HJPci_GetState(IN int drvHandle, IN int dmaChannel, OUT unsigned int*buf, IN int len);
};

#endif 