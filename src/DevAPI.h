#ifndef __DEVAPI_H__
#define __DEVAPI_H__

#include "Common.h"
#include "PciTypes.h"

class Device 
{
private:
	static bool FetchDpath(int index, wchar_t* dpath, int len);					//获取设备接口中的符号链接字符串
	static int  FetchDid(int vid, wchar_t* dpath, int len);						//提取DID(设备ID号)
	static int  FetchId(wchar_t* dpath, wchar_t* prefix);						//具体实现提取DID(设备ID号)

public:
	/**不指定设备名称, 并且通过IoRegisterDeviceInterface函数注册设备接口来操作设备**/
	static int  Open(int vid, int did);
	static void Close(int handle);

	/**通过IoCreateDevice函数指定设备名称来操作设备**/
	//打开、关闭、查找设备操作接口
	static int HJPCI_DeviceOpen(TAG_DEVICE_LOCATION *pDevice, HANDLE *pHandle);							//打开设备
	static int HJPCI_DeviceFind(TAG_DEVICE_LOCATION *pDevice, U32 *pRequestLimit);						//查找指定条件的设备
	static int HJPCI_DeviceClose(HANDLE hDevice);														//关闭设备

	//内部寄存器操作接口(针对BAR0), 只为驱动开放，不对用户开放
	static int HJRegisterRead(IN int drvHandle, IN unsigned int registerOffset, 
		OUT unsigned int * registerVal);																//读单个寄存器
	static int HJRegisterWrite(IN int drvHandle, IN unsigned int registerOffset, 
		IN const unsigned int * registerVal);															//写单个寄存器
	static int HJRegisterReadAll(IN int drvHandle, IN unsigned int startOffset, 
		IN unsigned int registerCout,OUT unsigned int * regValBuf);										//读取多个连续的寄存器
	static int HJRegisterWriteAll(IN int drvHandle, IN unsigned int startOffset,
		IN unsigned int registerCout,IN  const unsigned int * regValBuf);								//写入多个连续的寄存器

	//内部寄存器操作接口(针对BAR1~BAR5)，对用户开放
	static int HJUserRegisterRead(IN int drvHandle, IN unsigned int barRegisterNumber,
		IN unsigned int registerOffset, OUT unsigned int * registerVal);								//读单个寄存器
	static int HJUserRegisterWrite(IN int drvHandle, IN unsigned int barRegisterNumber,
		IN unsigned int registerOffset, IN const unsigned int * registerVal);							//写单个寄存器
	static int HJUserRegisterReadAll(IN int drvHandle,IN unsigned int barRegisterNumber,
		IN unsigned int startOffset, IN unsigned int registerCout,OUT unsigned int * regValBuf);		//读取多个连续的寄存器
	static int HJUserRegisterWriteAll(IN int drvHandle, IN unsigned int barRegisterNumber,
		IN unsigned int startOffset, IN unsigned int registerCout,IN  const unsigned int * regValBuf);	//写入多个连续的寄存器

	//SGL(分散聚合链式)DMA操作接口
	static int HJDmaSglChannelEnable(IN int drvHandle, IN int dmaChannel);								//开启SGL(分散聚合链式)DMA	
	static int HJDmaSglChannelDisable(IN int drvHandle, IN int dmaChannel);								//停止SGL(分散聚合链式)DMA传输
	static int HJDmaSglChannelReset(IN int drvHandle, IN int dmaChannel);								//复位SGL(分散聚合链式)DMA							
	static int HJDmaSglChannelSetTranType(IN int drvHandle, IN int dmaChannel, IN int type);			//设定SGL(分散聚合链式)DMA传输类型, 0:FIFO, 1:驱动读写文件
	static int HJDmaSglChannelSetTranDir(IN int drvHandle, IN int dmaChannel, IN int dir);				//设定SGL(分散聚合链式)DMA传输方向, 0:上行, 1:下行
	static int HJDmaSglChannelSetDmaSize(IN int drvHandle, IN int dmaChannel, IN int size);				//设置SGL(分散聚合链式)DMA每个DMA块传输的大小
	static int HJDmaSglChannelSetDmaCount(IN int drvHandle, IN int dmaChannel, IN int count);			//设置SGL(分散聚合链式)DMA传输的总DMA个数		
	static int HJDmaSglChannelSetFilePath(IN int drvHandle, IN int dmaChannel,					
		IN TAG_UPDW_FILEPATH filePath);																	//设置SGL(分散聚合链式)DMA传输的上行和下行文件路径和名称

	//获取基地址寄存器(BAR0~BAR5)信息接口
	static int HJPciBarRangeGet(IN int drvHandle, IN unsigned int barRegisterNumber, 
		OUT unsigned int * data);																		//获取指定的基地址寄存器的内存范围
	static int HJPciBaseAddressesGet(IN int drvHandle, OUT PTAG_VIRTUAL_ADDRESSES virtAddr);			//获取PCIE设备所有BAR的用户虚拟地址
	
	//获取版本信息接口
	static int HJPci_ApiVersion(
		OUT U8 *pVersionMajor,
		OUT U8 *pVersionMinor,
		OUT U8 *pVersionRevison);		//获取SDK API版本信息
	static int HJPci_DriverVersion(
		IN int drvHandle,
		OUT U8 *pVersionMajor,
		OUT U8 *pVersionMinor,
		OUT U8 *pVersionRevision);		//获取PCIE驱动版本信息	

	//操作配置寄存器接口
	static int HJPci_ConfigRegisterRead(
		IN U8  bus,
		IN U8  slot,
		IN U16 offset,
		OUT U32 * pData);				//读配置寄存器操作
	static int HJPci_ConfigRegisterWrite( 
		IN U8  bus,
		IN U8  slot,
		IN U16 offset,
		IN U32 * pData);				//写配置寄存器操作

	//应用程序与驱动程序之间进行事件交互的操作接口
	static int HJPci_AddRefEvent(IN int drvHandle, IN int dmaChannel, IN HANDLE tevent);				//添加应用程序与驱动程序间进行交互的事件对象
	static int HJPci_DelRefEvent(IN int drvHandle, IN int dmaChannel);									//删除应用程序与驱动程序间交互的事件对象
	//FIFO方式下进行数据传输(包括读取和写入)的操作接口
	static int HJPci_DataRead(IN int drvHandle, IN int dmaChannel, OUT unsigned int* buf, IN int len);	//在DMA上行中通过FIFO方式读取数据
	static int HJPci_DataWrite(IN int drvHandle, IN int dmaChannel, IN unsigned int* buf, IN int len);	//在DMA下行中通过FIFO方式写入数据
	//获取状态值
	static int HJPci_GetState(IN int drvHandle, IN int dmaChannel, OUT unsigned int*buf, IN int len);
};

#endif 