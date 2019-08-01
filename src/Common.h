#ifndef __COMMON_H__
#define __COMMON_H__

#include "PciTypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_UPDW_FILEPATH_NUMS		200			//上行和下行文件路径名称字符串个数的最大值
#define DMA_MBUF_COUNT				32			//给DMA分配连续物理内存的块数,		 单位:字节
#define DMA_MBUF_LEN				0x00100000	//给DMA分配连续物理内存中每块的大小, 单位:字节
#define DMA_TRANS_LEN				0x00100000  //DMA传输中实际使用的内存块的大小,   单位:字节

//SDK Version information
#define HJ_SDK_VERSION_MAJOR	1
#define HJ_SDK_VERSION_MINOR	10
#define HJ_SDK_VERSION_STRING	"1.10"
//Define a large value for a signal to the driver
#define FIND_AMOUNT_MATCHED         80001

//传输方向共用体类型
typedef enum _TAG_TRANSDIR
{
	TRANSDIR_UP,	//上行
	TRANSDIR_DW,	//下行
}TAG_TRANSDIR, *PTAG_TRANSDIR;

//传输方式,包括FIFO数据流、驱动读写硬盘文件两种方式
typedef enum _TAG_TRANSTYPE
{
	TRANSTYPE_FIFO_STREAM,	//FIFO数据流
	TRANSTYPE_DRIVER_FILE,	//驱动读写硬盘文件
}TAG_TRANSTYPE, *PTAG_TRANSTYPE;

//DMA通道序号
typedef enum _TAG_DMA_CHANNEL
{
	Channel0,		//DMA通道0
	Channel1,		//DMA通道1
	Channel2,		//DMA通道2
	Channel3,		//DMA通道3
	MaxChannelNums	//DMA的通道个数
}TAG_DMA_CHANNEL, *PTAG_DMA_CHANNEL;

//BAR基地址寄存器共用体类型
typedef enum _TAG_BAR_SPACE
{
	BAR0,			//基地址寄存器0, 内部使用
	BAR1,			//基地址寄存器1, 对用户开放
	BAR2,			//基地址寄存器2, 对用户开放
	BAR3,			//基地址寄存器3, 对用户开放
	BAR4,			//基地址寄存器4, 对用户开放
	BAR5,			//基地址寄存器5, 对用户开放
	MaxBarNums		
}TAG_BAR_SPACE, *PTAG_BAR_SPACE;

//PCI设备用户虚拟地址列表，用来保存BAR(BAR0~BAR5)的值
typedef struct _TAG_VIRTUAL_ADDRESSES
{
	unsigned int Va0;
	unsigned int Va1;
	unsigned int Va2;
	unsigned int Va3;
	unsigned int Va4;
	unsigned int Va5;
}TAG_VIRTUAL_ADDRESSES, *PTAG_VIRTUAL_ADDRESSES;

//上下行文件路径传输结构体类型
typedef struct _TAG_UPDW_FILEPATH
{
	unsigned int         Channel;								//通道号
	unsigned int		 TransDir;								//传输方向，0:上行, 1:下行
	unsigned int		 Length;								//文件路径和名称的有效长度	
	wchar_t				 FilePath[MAX_UPDW_FILEPATH_NUMS];		//文件路径和名称
}TAG_UPDW_FILEPATH, *PTAG_UPDW_FILEPATH;

//设备位置信息结构体类型
typedef struct _TAG_DEVICE_LOCATION
{
	/*U8  BusNumber;					//PCI设备所在的总线号
	U8  SlotNumber;						//PCI设备所在的槽号
	U8  FuncNumber;						//PCI设备所在的功能号
	U16 VendorId;						//PCI设备的提供商ID,即提供商识别号
	U16 DeviceId;						//PCI设备的设备ID,	即设备识别号
	U8  SerialNumber[20];				//PCI设备串型号，是PCI设备的唯一识别号*/

	unsigned char  BusNumber;			//PCI设备所在的总线号
	unsigned char  SlotNumber;			//PCI设备所在的槽号
	unsigned char  FuncNumber;			//PCI设备所在的功能号
	unsigned short VendorId;			//PCI设备的供应商ID
	unsigned short DeviceId;			//PCI设备的设备ID
	unsigned char  SerialNumber[20];	//PCI设备串型号，是PCI设备的唯一识别号

}TAG_DEVICE_LOCATION, *PTAG_DEVICE_LOCATION;

//查找设备输入和输出参数结构体类型
typedef struct _TAG_FIND_DEVICE
{
	unsigned int        RequestLimit;
	TAG_DEVICE_LOCATION Device;
}TAG_FIND_DEVICE, *PTAG_FIND_DEVICE;


#ifdef __cplusplus
}
#endif 

#endif 