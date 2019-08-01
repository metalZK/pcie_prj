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

/***全局变量定义***/
int								receiveDMAFileCount = 749;  //设置总共需要接受的DMA个数
//pcie0
TAG_DEVICE_LOCATION				Device_pcie0;
int								handle_pcie0		= 0;			//PCIE板卡设备句柄
HANDLE							hDevice_pcie0		= NULL;		//PCIE板卡设备句柄

//pcie1
TAG_DEVICE_LOCATION				Device_pcie1;
int								handle_pcie1		= 0;			//PCIE板卡设备句柄
HANDLE							hDevice_pcie1		= NULL;		//PCIE板卡设备句柄

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


//打开设备
void OpenDevice(TAG_DEVICE_LOCATION & Device,int &handle,HANDLE &hDevice)
{
	//TAG_DEVICE_LOCATION Device;
	//Device.BusNumber		= (U8)BusNumber;
	//Device.SlotNumber		= (U8)SlotNumber;
	//Device.FuncNumber		= (U8)-1;
	//Device.VendorId			= (U16)-1;
	//Device.DeviceId			= (U16)-1;
	//strcpy((char *)Device.SerialNumber, "Pcie-0");

	//打开PCIE板卡设备
	handle = Device::HJPCI_DeviceOpen(&Device, &hDevice);
	if(!handle)
	{	
		cout << "打开VID:0x10EE, DID:0x7028的PCIE设备失败，请关闭并重新打开软件."<< endl;
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
		printf("打开VID:0x10EE, DID:0x7028的PCIE设备成功.");
	}
}


//关闭设备
void CloseDevice(HANDLE hDevice)
{
	printf("CloseDevice\n");
	//关闭PCIE板卡设备
	int ret = Device::HJPCI_DeviceClose(hDevice);
	if(ret == 0)
	{
		hDevice = NULL;
	}
}

//查找设备
void FindDevice()
{
	U32					i;
	U32					DeviceNum;
	TAG_DEVICE_LOCATION Device;

	//(1)查询并得到当前PC机中PCIE设备的总数
	Device.BusNumber       = (U8)-1;
	Device.SlotNumber      = (U8)-1;
	Device.FuncNumber      = (U8)-1;
	Device.VendorId        = (U16)-1;
	Device.DeviceId        = (U16)-1;
	Device.SerialNumber[0] = '\0';
	DeviceNum = FIND_AMOUNT_MATCHED;
	Device::HJPCI_DeviceFind(&Device, &DeviceNum);
	//(2)打印出查询到的所有PCIE设备信息
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

//读单个寄存器
UINT  ReadRegister(int handle,UINT  uRegAddr)
{	
	if(uRegAddr%4 != 0 || uRegAddr < 0x100 || uRegAddr > 0xFFC){
		printf("地址错误！用户可用的寄存器地址范围为0x100~0xFFC，且地址必须为4的整倍数\n");
	}
	assert((uRegAddr%4) == 0);
	assert(uRegAddr>= 0x100);
	assert(uRegAddr<= 0xFFC);

	UINT uRegVal;
	Device::HJRegisterRead(handle, uRegAddr, &uRegVal);
	return uRegVal;
}

//写单个寄存器
void WriteRegister(int handle,UINT uRegAddr,UINT uRegVal)
{
	if(uRegAddr%4 != 0 || uRegAddr < 0x100 || uRegAddr > 0xFFC){
		printf("地址错误！用户可用的寄存器地址范围为0x100~0xFFC，且地址必须为4的整倍数\n");
	}
	assert((uRegAddr%4) == 0);
	assert(uRegAddr>= 0x100);
	assert(uRegAddr<= 0xFFC);

	Device::HJRegisterWrite(handle, uRegAddr, &uRegVal);

	if(uRegVal == ReadRegister(handle,uRegAddr))
		printf("寄存器写入成功\n");
	else
		printf("寄存器写入失败，请重新写入\n");
}



//设置SGL(分散聚合链式)DMA传输类型
void DmaSglChannelSetTranType(int handle)
{
	int DmaTransType  = 1;  //只支持文件
	Device::HJDmaSglChannelSetTranType(handle, 0, DmaTransType);
	//printf("m_DmaTransType=%d\n", DmaTransType);
}

//设置SGL(分散聚合链式)DMA传输方向
void DmaSglChannelSetTranDir(int handle,int DmaTransDir)
{
	if(DmaTransDir != 0 && DmaTransDir != 1){
		printf("输入错误！DMA传输方向只能为0或1,1：下行,0：上行\n");
	}
	assert(DmaTransDir==0 || DmaTransDir==1);//assert()条件为假，则直接退出

	if(DmaTransDir == 1) 
		printf("SET:下行方向PC--->FPGA\n");
	else				
		printf("SET:上行方向FPGA--->PC\n");
	Device::HJDmaSglChannelSetTranDir(handle, 0, DmaTransDir);
}
//设置SGL(分散聚合链式)DMA每个DMA块传输的大小,单位Byte
void DmaSglChannelSetDmaSize(int handle,UINT DmaSize)
{
	if(DmaSize%4096)
		printf("DMA传输大小只能为4K的整数倍，且最大为1MB\n");
	assert((DmaSize%4096) == 0);  //assert()条件为假，则直接退出
	assert(DmaSize <= 1024*1024);

	Device::HJDmaSglChannelSetDmaSize(handle, 0, DmaSize);
	printf("SET DAM SIZE: uDmaSize=%d\n", DmaSize);
}
//设置SGL(分散聚合链式)DMA传输DMA个数
void DmaSglChannelSetDmaCount(int handle,int UpDMACount)
{
	Device::HJDmaSglChannelSetDmaCount(handle, 0, UpDMACount);
	printf("SET DAM COUNT: uDmaCount=%d\n", UpDMACount);
}

//开启SGL(分散聚合链式)DMA	
void DmaSglChannelEnable(int handle,int DmaTransDir,wchar_t FileName[200])//输入存储文件的路径和文件名
{	
	if(DmaTransDir == TRANSDIR_UP)//上行
	{	
		TAG_UPDW_FILEPATH tagUpFilePath;
		memset(&tagUpFilePath, 0, sizeof(TAG_UPDW_FILEPATH));
		tagUpFilePath.Channel  = Channel0;
		tagUpFilePath.TransDir = DmaTransDir;
		tagUpFilePath.Length   = wcslen(FileName);
		wcsncpy(tagUpFilePath.FilePath,FileName, wcslen(FileName));

		Device::HJDmaSglChannelSetFilePath(handle, Channel0, tagUpFilePath);	//设定上行文件存盘路径和名称
		Device::HJDmaSglChannelEnable(handle, Channel0);						//开启数据传输
	}
	else if(DmaTransDir == TRANSDIR_DW)	//下行
	{	
		TAG_UPDW_FILEPATH tagDwFilePath;
		memset(&tagDwFilePath, 0, sizeof(TAG_UPDW_FILEPATH));
		tagDwFilePath.Channel  = Channel0;
		tagDwFilePath.TransDir = DmaTransDir;
		tagDwFilePath.Length   = wcslen(FileName);
		wcsncpy(tagDwFilePath.FilePath, FileName, wcslen(FileName));

		Device::HJDmaSglChannelSetFilePath(handle, Channel0, tagDwFilePath);	//获取下行文件路径和名称
		Device::HJDmaSglChannelEnable(handle, Channel0);						//开启数据传输
	}
}
//复位SGL(分散聚合链式)DMA	
void DmaSglChannelReset(int handle)
{
	Device::HJDmaSglChannelReset(handle, 0);
	printf("复位VID:0x10EE, DID:0x7028的PCIE设备.\n");
}

//停止SGL(分散聚合链式)DMA传输	
void DmaSglChannelDisable(int handle)
{
	Device::HJDmaSglChannelDisable(handle, 0);
	printf("DISABLE DAM\n");
}
//获取DMA传输状态
void GetPCieState(int handle,unsigned int *buf)
{
	if(buf == NULL)
	{
		printf("New buf Is Failed.\n");
		return;
	}
	Device::HJPci_GetState(handle, 0, buf, 10);
}

//下行设置,程序正常退出则表明DMA传输完成
void UserDmaTransDwAPI(int handle,int DmaSize,wchar_t FileName[200]){
	char* fileName = WcharToChar(FileName);
	FILE* fp = fopen(fileName,"rb");

	fseek(fp,0,SEEK_END);
	unsigned int fileLegth = ftell(fp);
	fseek(fp,0,SEEK_SET);

	printf("下行文件 %s 一共有 %d Bytes，共计分割为 %d 次DMA传输，一次传输 %d Bytes，剩余 %d 不够一个DMA，不进行传输\n",
		fileName,fileLegth,fileLegth/DmaSize,DmaSize,fileLegth%DmaSize);

	//开启Dma开始传输数据
	DmaSglChannelEnable(handle,1,FileName);
	//不断获取DMA状态，判断DMA是否传输完成
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
			printf("下行 DMA 传输完成\n");
			break;
		}
	}
}
void UserDmaTransUpAPI(int handle,int UpDMACount,int DMASize,wchar_t FileName[200] ){
	//上行设置
	char* fileName = WcharToChar(FileName);

	printf("上行文件存盘目录 %s，共计需要进行 %d DMA，每次DMA传输 %d Bytes\n",fileName,UpDMACount,DMASize);

	//开启Dma开始传输数据
	DmaSglChannelEnable(handle,0,FileName);
	//不断获取DMA状态，判断DMA是否传输完成
	unsigned int  buf[10];
	while(1){
		GetPCieState(handle,buf);
		if(buf[9] == 1){
			printf("上行 DMA 传输完成\n");
			break;
		}
	}
}


//////////////////////////
//////////////////////////
//////////////////////////
//下行
//param： handle 为PCIe设备句柄,SendDataFileName为发送文件路径
int PCIeDWProcess(int handle,wchar_t SendDataFileName[200]){
	UserDmaTransDwAPI(handle,1024*4*256,SendDataFileName);
	return 0;
}
//上行
//param： handle 为PCIe设备句柄，UpDMACount为期望接收的DMA次数
int PCIeUProcess(int handle,int UpDMACount){
	wchar_t ReceiveDataFileName[200] = L"C:\\";  //此处不建议修改路径，且路径不需要指定文件名
	UserDmaTransUpAPI(handle,UpDMACount,1024*4*128,ReceiveDataFileName);
	return 0;
}

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

void FPGARst(int handle){
	WriteRegister(handle,0x104,0); //复位
	WriteRegister(handle,0x104,1); //恢复
	printf("FPGA 复位完成\n");
}

int main(){

	//发现PCIe设备
	FindDevice();  //针对我们的应用，程序可以在一个主板上最多识别两个PCIe，因为我只定义了handle_pcie0和handle_pcie1两个变量

	//打开PCIe设备
	OpenDevice(Device_pcie0,handle_pcie0,hDevice_pcie0);  //打开pcie0

	//cout << ReadRegister(handle_pcie0,0x104) << endl;
	//对FPGA进行复位操作
	FPGARst(handle_pcie0);

	//下行initial
	//复位SGL(分散聚合链式)DMA	
	DmaSglChannelReset(handle_pcie0);  //此处会复位DMA，同时FPGA中pcie ip核的user_rst会拉高
	//设置DMA传输类型，只能从文件中读取
	DmaSglChannelSetTranType(handle_pcie0);
	//设置DMA的传输方向，1：下行，0：上行
	DmaSglChannelSetTranDir(handle_pcie0,1); //下行
	//设置DMA传输块的大小
	DmaSglChannelSetDmaSize(handle_pcie0,1024*1024); //

	//下行发送数据
	//发送weight、bias
	wchar_t WegihtBiasSendDataFileName[200] = L"C:\\SSD\\VS\\pcie_prj\\generate_pcie_weight_bias\\weight_bias.dat";
	PCIeDWProcess(handle_pcie0,WegihtBiasSendDataFileName);

	//等待weight、bias写入完成
	while(1){
		UINT addr = 0x100;
		if(ReadRegister(handle_pcie0,addr) == 0xFFFFFFFF){
			printf("初始化完成\n");
			break;
		}
	}

	//下行发送数据
	//发送fmap
	wchar_t FmapSendDataFileName[200] = L"C:\\SSD\\VS\\pcie_prj\\generate_pcie_fmap\\pcie_fmap_fpga_data.dat";
	PCIeDWProcess(handle_pcie0,FmapSendDataFileName);

	///////////////////////////////
	//上行initial
	//设置DMA的传输方向，1：下行，0：上行
	DmaSglChannelSetTranDir(handle_pcie0,0); //上行
	//设置DMA传输块的个数
	DmaSglChannelSetDmaCount(handle_pcie0,1); 
	//设置DMA传输块的大小
	DmaSglChannelSetDmaSize(handle_pcie0,1024*1024); 

	PCIeUProcess(handle_pcie0,1);

	//关闭DMA通道
	DmaSglChannelDisable(handle_pcie0);
	//关闭PCIe设备
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