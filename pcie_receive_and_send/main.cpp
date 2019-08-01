//#include "stdafx.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <Windows.h>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "C:\Users\engl\Documents\Visual Studio 2010\Projects\pcie_prj\src\DevAPI.h"
#include "C:\Users\engl\Documents\Visual Studio 2010\Projects\pcie_prj\src\Common.h"
#include "C:\Users\engl\Documents\Visual Studio 2010\Projects\pcie_prj\src\PciTypes.h"

using namespace std;

/***ȫ�ֱ�������***/
int								receiveDMAFileCount = 749;  //�����ܹ���Ҫ���ܵ�DMA����
//pcie0
TAG_DEVICE_LOCATION				Device_pcie0;
int								handle_pcie0		= 0;			//PCIE�忨�豸���
HANDLE							hDevice_pcie0		= NULL;		//PCIE�忨�豸���

//pcie1
TAG_DEVICE_LOCATION				Device_pcie1;
int								handle_pcie1		= 0;			//PCIE�忨�豸���
HANDLE							hDevice_pcie1		= NULL;		//PCIE�忨�豸���

//////////////////////////
/////////User API//////////
//////////////////////////

char* WcharToChar(const wchar_t* wp)
{
	char *m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}
wchar_t* CharToWchar(const char* c)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}


//���豸
void OpenDevice(TAG_DEVICE_LOCATION & Device,int &handle,HANDLE &hDevice)
{
	//TAG_DEVICE_LOCATION Device;
	//Device.BusNumber		= (U8)BusNumber;
	//Device.SlotNumber		= (U8)SlotNumber;
	//Device.FuncNumber		= (U8)-1;
	//Device.VendorId			= (U16)-1;
	//Device.DeviceId			= (U16)-1;
	//strcpy((char *)Device.SerialNumber, "Pcie-0");

	//��PCIE�忨�豸
	handle = Device::HJPCI_DeviceOpen(&Device, &hDevice);
	if(!handle)
	{	
		cout << "��VID:0x10EE, DID:0x7028��PCIE�豸ʧ�ܣ���رղ����´����."<< endl;
		handle = NULL;
		return;
	}
	else
	{
		printf("Device.BusNumber=%d\n",		Device.BusNumber);
		printf("Device.SlotNumber=%d\n",	Device.SlotNumber);
		printf("Device.FuncNumber=%d\n",	Device.FuncNumber);
		printf("Device.VendorId=%04x\n",	Device.VendorId);
		printf("Device.DeviceId=%04x\n",	Device.DeviceId);
		printf("Device.SerialNumber=%S\n",	Device.SerialNumber);
		printf("��VID:0x10EE, DID:0x7028��PCIE�豸�ɹ�.");
	}
}


//�ر��豸
void CloseDevice(HANDLE hDevice)
{
	printf("CloseDevice\n");
	//�ر�PCIE�忨�豸
	int ret = Device::HJPCI_DeviceClose(hDevice);
	if(ret == 0)
	{
		hDevice = NULL;
	}
}

//�����豸
void FindDevice()
{
	U32					i;
	U32					DeviceNum;
	TAG_DEVICE_LOCATION Device;

	//(1)��ѯ���õ���ǰPC����PCIE�豸������
	Device.BusNumber       = (U8)-1;
	Device.SlotNumber      = (U8)-1;
	Device.FuncNumber      = (U8)-1;
	Device.VendorId        = (U16)-1;
	Device.DeviceId        = (U16)-1;
	Device.SerialNumber[0] = '\0';
	DeviceNum = FIND_AMOUNT_MATCHED;
	Device::HJPCI_DeviceFind(&Device, &DeviceNum);
	//(2)��ӡ����ѯ��������PCIE�豸��Ϣ
	for(i = 0; i < DeviceNum; i++)
	{
		Device.BusNumber       = (U8)-1;
		Device.SlotNumber      = (U8)-1;
		Device.FuncNumber      = (U8)-1;
		Device.VendorId        = (U16)-1;
		Device.DeviceId        = (U16)-1;
		Device.SerialNumber[0] = '\0';

		Device::HJPCI_DeviceFind(&Device, &i);
		printf(": Device.BusNumber=%d\n",		Device.BusNumber);
		printf(": Device.SlotNumber=%d\n",		Device.SlotNumber);
		printf(": Device.FuncNumber=%d\n",		Device.FuncNumber);
		printf(": Device.VendorId=%08x\n",		Device.VendorId);
		printf(": Device.DeviceId=%08x\n",		Device.DeviceId);
		printf(": Device.SerialNumber=%S\n",	Device.SerialNumber);

		if(i == 0){
			Device_pcie0.BusNumber       = Device.BusNumber;
			Device_pcie0.SlotNumber      = Device.SlotNumber;
			Device_pcie0.FuncNumber      = Device.FuncNumber;
			Device_pcie0.VendorId        = Device.VendorId;
			Device_pcie0.DeviceId        = Device.DeviceId;
			strcpy((char *)Device_pcie0.SerialNumber, (char *)Device.SerialNumber);
		}
		if(i == 1){
			Device_pcie1.BusNumber       = Device.BusNumber;
			Device_pcie1.SlotNumber      = Device.SlotNumber;
			Device_pcie1.FuncNumber      = Device.FuncNumber;
			Device_pcie1.VendorId        = Device.VendorId;
			Device_pcie1.DeviceId        = Device.DeviceId;
			strcpy((char *)Device_pcie1.SerialNumber, (char *)Device.SerialNumber);
		}
	}
}

//�������Ĵ���
UINT  ReadRegister(int handle,UINT  uRegAddr)
{	
	if(uRegAddr%4 != 0 || uRegAddr < 0x100 || uRegAddr > 0xFFC){
		printf("��ַ�����û����õļĴ�����ַ��ΧΪ0x100~0xFFC���ҵ�ַ����Ϊ4��������\n");
	}
	assert((uRegAddr%4) == 0);
	assert(uRegAddr>= 0x100);
	assert(uRegAddr<= 0xFFC);

	UINT uRegVal;
	Device::HJRegisterRead(handle, uRegAddr, &uRegVal);
	return uRegVal;
}

//д�����Ĵ���
void WriteRegister(int handle,UINT uRegAddr,UINT uRegVal)
{
	if(uRegAddr%4 != 0 || uRegAddr < 0x100 || uRegAddr > 0xFFC){
		printf("��ַ�����û����õļĴ�����ַ��ΧΪ0x100~0xFFC���ҵ�ַ����Ϊ4��������\n");
	}
	assert((uRegAddr%4) == 0);
	assert(uRegAddr>= 0x100);
	assert(uRegAddr<= 0xFFC);

	Device::HJRegisterWrite(handle, uRegAddr, &uRegVal);

	if(uRegVal == ReadRegister(handle,uRegAddr))
		printf("�Ĵ���д��ɹ�\n");
	else
		printf("�Ĵ���д��ʧ�ܣ�������д��\n");
}



//����SGL(��ɢ�ۺ���ʽ)DMA��������
void DmaSglChannelSetTranType(int handle)
{
	int DmaTransType  = 1;  //ֻ֧���ļ�
	Device::HJDmaSglChannelSetTranType(handle, 0, DmaTransType);
	//printf("m_DmaTransType=%d\n", DmaTransType);
}

//����SGL(��ɢ�ۺ���ʽ)DMA���䷽��
void DmaSglChannelSetTranDir(int handle,int DmaTransDir)
{
	if(DmaTransDir != 0 && DmaTransDir != 1){
		printf("�������DMA���䷽��ֻ��Ϊ0��1,1������,0������\n");
	}
	assert(DmaTransDir==0 || DmaTransDir==1);//assert()����Ϊ�٣���ֱ���˳�

	if(DmaTransDir == 1) 
		printf("SET:���з���PC--->FPGA\n");
	else				
		printf("SET:���з���FPGA--->PC\n");
	Device::HJDmaSglChannelSetTranDir(handle, 0, DmaTransDir);
}
//����SGL(��ɢ�ۺ���ʽ)DMAÿ��DMA�鴫��Ĵ�С,��λByte
void DmaSglChannelSetDmaSize(int handle,UINT DmaSize)
{
	if(DmaSize%4096)
		printf("DMA�����Сֻ��Ϊ4K���������������Ϊ1MB\n");
	assert((DmaSize%4096) == 0);  //assert()����Ϊ�٣���ֱ���˳�
	assert(DmaSize <= 1024*1024);

	Device::HJDmaSglChannelSetDmaSize(handle, 0, DmaSize);
	printf("SET DAM SIZE: uDmaSize=%d\n", DmaSize);
}
//����SGL(��ɢ�ۺ���ʽ)DMA����DMA����
void DmaSglChannelSetDmaCount(int handle,int UpDMACount)
{
	Device::HJDmaSglChannelSetDmaCount(handle, 0, UpDMACount);
	printf("SET DAM COUNT: uDmaCount=%d\n", UpDMACount);
}

//����SGL(��ɢ�ۺ���ʽ)DMA	
void DmaSglChannelEnable(int handle,int DmaTransDir,wchar_t FileName[200])//����洢�ļ���·�����ļ���
{	
	if(DmaTransDir == TRANSDIR_UP)//����
	{	
		TAG_UPDW_FILEPATH tagUpFilePath;
		memset(&tagUpFilePath, 0, sizeof(TAG_UPDW_FILEPATH));
		tagUpFilePath.Channel  = Channel0;
		tagUpFilePath.TransDir = DmaTransDir;
		tagUpFilePath.Length   = wcslen(FileName);
		wcsncpy(tagUpFilePath.FilePath,FileName, wcslen(FileName));

		Device::HJDmaSglChannelSetFilePath(handle, Channel0, tagUpFilePath);	//�趨�����ļ�����·��������
		Device::HJDmaSglChannelEnable(handle, Channel0);						//�������ݴ���
	}
	else if(DmaTransDir == TRANSDIR_DW)	//����
	{	
		TAG_UPDW_FILEPATH tagDwFilePath;
		memset(&tagDwFilePath, 0, sizeof(TAG_UPDW_FILEPATH));
		tagDwFilePath.Channel  = Channel0;
		tagDwFilePath.TransDir = DmaTransDir;
		tagDwFilePath.Length   = wcslen(FileName);
		wcsncpy(tagDwFilePath.FilePath, FileName, wcslen(FileName));

		Device::HJDmaSglChannelSetFilePath(handle, Channel0, tagDwFilePath);	//��ȡ�����ļ�·��������
		Device::HJDmaSglChannelEnable(handle, Channel0);						//�������ݴ���
	}
}
//��λSGL(��ɢ�ۺ���ʽ)DMA	
void DmaSglChannelReset(int handle)
{
	Device::HJDmaSglChannelReset(handle, 0);
	printf("��λVID:0x10EE, DID:0x7028��PCIE�豸.\n");
}

//ֹͣSGL(��ɢ�ۺ���ʽ)DMA����	
void DmaSglChannelDisable(int handle)
{
	Device::HJDmaSglChannelDisable(handle, 0);
	printf("DISABLE DAM\n");
}
//��ȡDMA����״̬
void GetPCieState(int handle,unsigned int *buf)
{
	if(buf == NULL)
	{
		printf("New buf Is Failed.\n");
		return;
	}
	Device::HJPci_GetState(handle, 0, buf, 10);
}

//��������,���������˳������DMA�������
void UserDmaTransDwAPI(int handle,int DmaSize,wchar_t FileName[200]){
	char* fileName = WcharToChar(FileName);
	FILE* fp = fopen(fileName,"rb");

	fseek(fp,0,SEEK_END);
	unsigned int fileLegth = ftell(fp);
	fseek(fp,0,SEEK_SET);

	printf("�����ļ� %s һ���� %d Bytes�����Ʒָ�Ϊ %d ��DMA���䣬һ�δ��� %d Bytes��ʣ�� %d ����һ��DMA�������д���\n",
		fileName,fileLegth,fileLegth/DmaSize,DmaSize,fileLegth%DmaSize);

	//����Dma��ʼ��������
	DmaSglChannelEnable(handle,1,FileName);
	//���ϻ�ȡDMA״̬���ж�DMA�Ƿ������
	unsigned int  buf[10];
	while(1){
		GetPCieState(handle,buf);
		if(buf[9] == 1){
			printf("*******************\n");
			printf("intrCount: %d\n", buf[0]);
			printf("intrLoseCount: %d\n", buf[1]);
			printf("dmaIntrCount: %d\n", buf[2]);
			printf("dmaIntrLoseCount: %d\n", buf[3]);
			printf("dma2fifoTranCount: %d\n", buf[4]);
			printf("dma2fifoTranLoseCount: %d\n", buf[5]);
			printf("dma2fileTranCount: %d\n", buf[6]);
			printf("dma2fileTranLoseCount: %d\n", buf[7]);
			printf("fuctionStateValue: %d\n", buf[8]);
			printf("DmaOverStae: %d\n", buf[9]);
			printf("*******************\n");
			printf("���� DMA �������\n");
			break;
		}
	}
}
void UserDmaTransUpAPI(int handle,int UpDMACount,int DMASize,wchar_t FileName[200] ){
	//��������
	char* fileName = WcharToChar(FileName);

	printf("�����ļ�����Ŀ¼ %s��������Ҫ���� %d DMA��ÿ��DMA���� %d Bytes\n",fileName,UpDMACount,DMASize);

	//����Dma��ʼ��������
	DmaSglChannelEnable(handle,0,FileName);
	//���ϻ�ȡDMA״̬���ж�DMA�Ƿ������
	unsigned int  buf[10];
	while(1){
		GetPCieState(handle,buf);
		if(buf[9] == 1){
			printf("���� DMA �������\n");
			break;
		}
	}
}


//////////////////////////
//////////////////////////
//////////////////////////
//����
//param�� handle ΪPCIe�豸���,SendDataFileNameΪ�����ļ�·��
int PCIeDWProcess(int handle,wchar_t SendDataFileName[200]){
	UserDmaTransDwAPI(handle,1024*4*256,SendDataFileName);
	return 0;
}
//����
//param�� handle ΪPCIe�豸�����UpDMACountΪ�������յ�DMA����
int PCIeUProcess(int handle,int UpDMACount){
	wchar_t ReceiveDataFileName[200] = L"C:\\";  //�˴��������޸�·������·������Ҫָ���ļ���
	UserDmaTransUpAPI(handle,UpDMACount,1024*4*128,ReceiveDataFileName);
	return 0;
}

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

void FPGARst(int handle){
	WriteRegister(handle,0x104,0); //��λ
	WriteRegister(handle,0x104,1); //�ָ�
	printf("FPGA ��λ���\n");
}

int main(){

	//����PCIe�豸
	FindDevice();  //������ǵ�Ӧ�ã����������һ�����������ʶ������PCIe����Ϊ��ֻ������handle_pcie0��handle_pcie1��������

	//��PCIe�豸
	OpenDevice(Device_pcie0,handle_pcie0,hDevice_pcie0);  //��pcie0

	//cout << ReadRegister(handle_pcie0,0x104) << endl;
	//��FPGA���и�λ����
	FPGARst(handle_pcie0);

	//����initial
	//��λSGL(��ɢ�ۺ���ʽ)DMA	
	DmaSglChannelReset(handle_pcie0);  //�˴��ḴλDMA��ͬʱFPGA��pcie ip�˵�user_rst������
	//����DMA�������ͣ�ֻ�ܴ��ļ��ж�ȡ
	DmaSglChannelSetTranType(handle_pcie0);
	//����DMA�Ĵ��䷽��1�����У�0������
	DmaSglChannelSetTranDir(handle_pcie0,1); //����
	//����DMA�����Ĵ�С
	DmaSglChannelSetDmaSize(handle_pcie0,1024*1024); //

	//���з�������
	//����weight��bias
	wchar_t WegihtBiasSendDataFileName[200] = L"C:\\SSD\\VS\\pcie_prj\\generate_pcie_weight_bias\\weight_bias.dat";
	PCIeDWProcess(handle_pcie0,WegihtBiasSendDataFileName);

	//�ȴ�weight��biasд�����
	while(1){
		UINT addr = 0x100;
		if(ReadRegister(handle_pcie0,addr) == 0xFFFFFFFF){
			printf("��ʼ�����\n");
			break;
		}
	}

	//���з�������
	//����fmap
	wchar_t FmapSendDataFileName[200] = L"C:\\SSD\\VS\\pcie_prj\\generate_pcie_fmap\\pcie_fmap_fpga_data.dat";
	PCIeDWProcess(handle_pcie0,FmapSendDataFileName);

	///////////////////////////////
	//����initial
	//����DMA�Ĵ��䷽��1�����У�0������
	DmaSglChannelSetTranDir(handle_pcie0,0); //����
	//����DMA�����ĸ���
	DmaSglChannelSetDmaCount(handle_pcie0,1); 
	//����DMA�����Ĵ�С
	DmaSglChannelSetDmaSize(handle_pcie0,1024*1024); 

	PCIeUProcess(handle_pcie0,1);

	//�ر�DMAͨ��
	DmaSglChannelDisable(handle_pcie0);
	//�ر�PCIe�豸
	CloseDevice(hDevice_pcie0);

	ifstream fp("c:\\readback0.tmp",ios::binary);
	assert(fp);
	ofstream fp2("conv8_2_output.dat");
	char* data = new char[2048];
	fp.read(data,2048);
	for(int i=0;i<2048;++i){
		fp2 << int(data[i]) << endl;
	}

	system("pause");
	return 0;
}