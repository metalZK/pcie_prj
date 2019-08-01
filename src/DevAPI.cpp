#include "stdafx.h"

#include <windows.h>
#include <initguid.h> 
#include "Ioctl.h"

#include <winioctl.h>
#include <setupapi.h>

#include <stdio.h>
#include <string.h>

#include "DevAPI.h"
#include <io.h>
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>

// Path to drivers
#if defined (_UNICODE) || defined (UNICODE)				//Unicode字符集
#define DRIVER_PATH                    L"\\\\.\\"    
#else													//多字节字符集
#define DRIVER_PATH						"\\\\.\\"	  
#endif 

// List of Hanju drivers to search for
static char *HJDrivers[] = 
{
	"Pcie",
	"0"          // Must be last item to mark end of list
};


//链表节点
typedef struct _TAG_DEVICE_NODE
{
	HANDLE						hDevice;
	TAG_DEVICE_LOCATION			Location;
	struct _TAG_DEVICE_NODE	  * pNext;
}TAG_DEVICE_NODE, *PTAG_DEVICE_NODE;

#define LockGlobals()            EnterCriticalSection(&GlobalDataLock)
#define UnlockGlobals()          LeaveCriticalSection(&GlobalDataLock)

static TAG_DEVICE_NODE   * pHeadDeviceList = NULL;  // 设备链表头指针
static CRITICAL_SECTION    GlobalDataLock;          // Synchronization object for global data protection


//增加设备节点, 从链表尾部插入
TAG_DEVICE_NODE * DeviceListAdd(HANDLE hDevice, TAG_DEVICE_LOCATION *pLocation);
//在链表中查找指定的设备句柄节点
TAG_DEVICE_NODE *DeviceListFind(HANDLE hDevice);
//删除链表中指定的设备句柄节点
BOOLEAN DeviceListDelete(HANDLE hDevice);


//获取设备接口中的符号链接字符串
bool Device::FetchDpath(int index, wchar_t* dpath, int len) 
{
	LPGUID guid = (LPGUID)&DEV_GUID;

	// Get handle to relevant device information set
	HDEVINFO info = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	if (info == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// Get interface data for the requested instance
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	if (!SetupDiEnumDeviceInterfaces(info, NULL, guid, index, &ifdata)) 
	{
		SetupDiDestroyDeviceInfoList(info);
		return false;
	}

	// Get size of symbolic link name
	DWORD size;
	SetupDiGetDeviceInterfaceDetail(info, &ifdata, NULL, 0, &size, NULL);
	PSP_INTERFACE_DEVICE_DETAIL_DATA ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(new char[size]);
	if (!ifDetail)
	{
		SetupDiDestroyDeviceInfoList(info);
		return false;
	}

	// Get symbolic link name
	ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
	if (!SetupDiGetDeviceInterfaceDetail(info, &ifdata, ifDetail, size, NULL, NULL))
	{
		SetupDiDestroyDeviceInfoList(info);
		delete ifDetail;
		return false;
	}
	SetupDiDestroyDeviceInfoList(info);

	if (len <= (int)wcslen(ifDetail->DevicePath))
	{
		delete ifDetail;
		return false;
	}

	wcscpy_s(dpath, len, ifDetail->DevicePath);

	delete ifDetail;
	return true;
}

//提取DID(设备ID号)
int Device::FetchDid(int vid, wchar_t* dpath, int len)
{
	// ...ven_****&dev_****...
	_wcslwr_s(dpath, len);
	if (FetchId(dpath, L"ven_") != vid)
	{
		return 0;
	}
	return FetchId(dpath, L"dev_");
}

//具体实现提取DID(设备ID号)
int Device::FetchId(wchar_t* dpath, wchar_t* prefix)
{
	wchar_t* p = wcsstr(dpath, prefix);
	if (!p) 
	{
		return 0;
	}
	return (int)wcstol(p + wcslen(prefix), NULL, 16);
}

// TODO: need to check slot later
//  31 - 16 15 - 0
//   slot     did
int Device::Open(int vid, int did) 
{
	wchar_t dpath[256];
	for (int i = 0; ; ++i)
	{
		if (!FetchDpath(i, dpath, sizeof(dpath) / sizeof(wchar_t)))
		{
			return 0;
		}

		int tdid = FetchDid(vid, dpath, sizeof(dpath) / sizeof(wchar_t));
		if (!tdid || tdid != did) 
		{
			continue;
		}

		HANDLE handle = CreateFile(dpath, 
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		return (int)handle;
	}
	return 0;
}

void Device::Close(int handle)
{
	if (handle)
	{
		CloseHandle((HANDLE)handle);
	}
}

//增加设备节点, 从链表尾部插入
TAG_DEVICE_NODE * DeviceListAdd(HANDLE hDevice, TAG_DEVICE_LOCATION *pLocation)
{
	TAG_DEVICE_NODE  *pCurrentDevice;
	TAG_DEVICE_NODE  *pPreviousDevice;

	// Lock access to globals
//	LockGlobals();

	// Allocate a new node
	pCurrentDevice = (TAG_DEVICE_NODE *)malloc(sizeof(TAG_DEVICE_NODE));
	if(pCurrentDevice == NULL)
	{
		UnlockGlobals();
		return NULL;
	}
	// Fill in new data
	pCurrentDevice->hDevice  = hDevice;
	pCurrentDevice->Location = *pLocation;
	pCurrentDevice->pNext    = NULL;

	// 调整链表指针
	if(pHeadDeviceList == NULL)
	{
		pHeadDeviceList = pCurrentDevice;
	}
	else
	{
		pPreviousDevice = pHeadDeviceList;
		while(pPreviousDevice->pNext != NULL)
		{
			pPreviousDevice = pPreviousDevice->pNext;
		}
		pPreviousDevice->pNext = pCurrentDevice;
	}

	// Release access to globals
//	UnlockGlobals();
	return pCurrentDevice;
}

//在链表中查找指定的设备句柄节点
TAG_DEVICE_NODE *DeviceListFind(HANDLE hDevice)
{
	TAG_DEVICE_NODE  *pCurrentDevice;

	// Lock access to globals
//	LockGlobals();

	pCurrentDevice = pHeadDeviceList;
	while(pCurrentDevice)
	{
		if(pCurrentDevice->hDevice == hDevice)
		{
			UnlockGlobals();
			return pCurrentDevice;
		}
		pCurrentDevice = pCurrentDevice->pNext;
	}

	// Release access to globals
//	UnlockGlobals();
	return NULL;
}

//删除链表中指定的设备句柄节点
BOOLEAN DeviceListDelete(HANDLE hDevice)
{
	TAG_DEVICE_NODE  *pCurrentDevice;
	TAG_DEVICE_NODE  *pPreviousDevice;

	// Lock access to globals
//	LockGlobals();

	pCurrentDevice = DeviceListFind(hDevice);
	if(pCurrentDevice == NULL)	//链表中没有要删除的句柄
		return FALSE;

	if(pCurrentDevice == pHeadDeviceList)		//删除对象为表头
	{
		if(pCurrentDevice->pNext != NULL)		//头后面还有节点
			pHeadDeviceList = pCurrentDevice->pNext;
		else									//链表只有一个节点
			pHeadDeviceList = NULL;
	}
	else										//删除对象为中间节点或者表尾
	{
		pPreviousDevice = pHeadDeviceList;
		pCurrentDevice  = pPreviousDevice;
		while(pCurrentDevice)
		{
			if(pCurrentDevice->hDevice == hDevice)
				break;
			pPreviousDevice = pCurrentDevice;
			pCurrentDevice  = pCurrentDevice->pNext;
		}

		if(pCurrentDevice->pNext != NULL)		//删除对象为中间节点
			pPreviousDevice->pNext = pCurrentDevice->pNext;
		else									//删除对象为表尾
			pPreviousDevice->pNext = NULL;
	}

	// Release access to globals
//	UnlockGlobals();
	// Release memory
	free(pCurrentDevice);
	return TRUE;
}


//打开设备
int Device::HJPCI_DeviceOpen(TAG_DEVICE_LOCATION *pDevice, HANDLE *pHandle)
{
	U32	i;
	U32 rc;
	wchar_t  DriverName[50];

	memset(DriverName, 0, sizeof(DriverName));
	if(pDevice == NULL || pHandle == NULL)
		return 0;
#if (1)
	// Get the Serial number of the device, if not provided
	if(pDevice->SerialNumber[0] == '\0')
	{
		i = 0;
		// Search for the device matching the criteria
		rc = HJPCI_DeviceFind(pDevice, &i);
		if(rc != 0)
		{
			*pHandle = NULL;
			return 0;
		}
	}
#endif 

#if defined (_UNICODE) || defined (UNICODE)		//UNICODE字符集(编码),对应windows下WideChar
	// 在UNICODE字符集下，将unsigned char转化为wchar_t(unsigned short)
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pDevice->SerialNumber, -1, NULL,0);
	wchar_t *w_string = new wchar_t[len];
	memset(w_string, 0, sizeof(wchar_t) * len);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pDevice->SerialNumber, -1, w_string, len);

	// If provided, the SerialNumber is sufficient to open a device
	swprintf(DriverName, DRIVER_PATH L"%s", w_string);
	//wcscpy(DriverName, DRIVER_PATH);
	//wcscat(DriverName, w_string);
	delete [] w_string;
	printf("HJPCI_DeviceOpen: pDevice->SerialNumber:%S\n",	pDevice->SerialNumber);
	printf("HJPCI_DeviceOpen: DriverName:%s\n",				DriverName);
	printf("HJPCI_DeviceOpen: DriverName+1:%s\n",			DriverName+1);
#else											//ANSI字符集(编码), 对应windows下MultiByteChar
	// If provided, the SerialNumber is sufficient to open a device
	sprintf(DriverName, DRIVER_PATH "%s", pDevice->SerialNumber);
	printf("HJPCI_DeviceOpen: pDevice->SerialNumber:%s\n",	pDevice->SerialNumber);
	printf("HJPCI_DeviceOpen: DriverName:%S\n",				DriverName);
	printf("HJPCI_DeviceOpen: DriverName+1:%S\n",				DriverName+1);
#endif 

	// Open the device
	*pHandle = CreateFile(
		DriverName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		//FILE_FLAG_DELETE_ON_CLOSE,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (*pHandle == INVALID_HANDLE_VALUE)
	{
		*pHandle = NULL;
		printf("HJPCI_DeviceOpen is Failed.\n");
		return 0;
	}

	// Add the handle to the device list.
	if(DeviceListAdd(*pHandle, pDevice) == NULL)
	{
		CloseHandle(*pHandle);
		*pHandle = NULL;
		return 0;
	}

	int ret = 0;
	// Get device data from driver
	if(!DeviceIoControl(*pHandle, IOCTL_PCI_DEVICE_INIT, NULL, 0, 
		pDevice, sizeof(TAG_DEVICE_LOCATION), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return (int)(*pHandle);
}

//查找指定条件的设备
int Device::HJPCI_DeviceFind(TAG_DEVICE_LOCATION *pDevice, U32 *pRequestLimit)
{
	U32					i;
	U32					rc;
	U32					totalDevFound;
	HANDLE				hDevice;
	BOOLEAN				driverOpened;
	TAG_DEVICE_LOCATION DevInfo;

	if(pDevice == NULL || pRequestLimit == NULL)
		return -1;

	totalDevFound = 0;
	driverOpened  = FALSE;
	i = 0;

	TAG_FIND_DEVICE findDevice;

	// Scan through present drivers for matches
	while (HJDrivers[i][0] != '0')
	{
		sprintf(
			(char *)DevInfo.SerialNumber,
			"%s-0",
			HJDrivers[i]
		);

		i++;
		rc = HJPCI_DeviceOpen(&DevInfo, &hDevice);
		if(rc == 0)
			continue;

		driverOpened = TRUE;
		if(*pRequestLimit != FIND_AMOUNT_MATCHED)
			findDevice.RequestLimit = *pRequestLimit - totalDevFound;
		else
			findDevice.RequestLimit = FIND_AMOUNT_MATCHED;
		memcpy(&findDevice.Device, pDevice, sizeof(TAG_DEVICE_LOCATION));

		int ret = 0;
		if(!DeviceIoControl(hDevice, IOCTL_PCI_DEVICE_FIND, &findDevice, sizeof(TAG_FIND_DEVICE), \
			&findDevice, sizeof(TAG_FIND_DEVICE), (LPDWORD)&ret, NULL))
		{
			ret = -1;
		}
		//得到驱动的反馈值
		totalDevFound += findDevice.RequestLimit;	

		if(*pRequestLimit != FIND_AMOUNT_MATCHED)
		{
			// Check to see if we have reached the specified device
			if(*pRequestLimit < totalDevFound)
			{
				memcpy(pDevice, &findDevice.Device, sizeof(TAG_DEVICE_LOCATION));
				HJPCI_DeviceClose(hDevice);			
				return 0;
			}
		}
		HJPCI_DeviceClose(hDevice);
	}

	if(driverOpened == FALSE)
	{
		if(*pRequestLimit == FIND_AMOUNT_MATCHED)
		{
			*pRequestLimit = 0;
		}
		return -1;
	}
	if(*pRequestLimit == FIND_AMOUNT_MATCHED)
	{
		*pRequestLimit = totalDevFound;
		if (totalDevFound == 0)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	return -1;
}

//关闭设备
int Device::HJPCI_DeviceClose(HANDLE hDevice)
{
	if(DeviceListDelete(hDevice) == FALSE)
		return -1;

	// Close the handle
	if(hDevice)
		CloseHandle(hDevice);
	return 0;
}


//读单个寄存器
int Device::HJRegisterRead(IN int drvHandle, IN unsigned int registerOffset, OUT unsigned int *registerVal)
{
	if(!registerVal || !drvHandle || registerOffset % 4)
		return -1;

	int ret = 0;
	unsigned int   len = 2;
	unsigned int * buf = new unsigned int[len];
	buf[0] = BAR0;				//BAR0
	buf[1] = registerOffset;	//地址偏移量
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_REGISTER_READ, buf, len * sizeof(unsigned int), 
		registerVal, sizeof(unsigned int), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}

//读取多个连续的寄存器
int Device::HJRegisterReadAll(IN int drvHandle, IN unsigned int startOffset, IN unsigned int registerCout, 
	OUT unsigned int *regValBuf)
{
	if(!regValBuf || !drvHandle || (registerCout <= 0) || startOffset % 4)
		return -1;

	int ret  = 0;
	int len  = 3;
	unsigned int *buf = new unsigned int [len];
	buf[0] = BAR0;			//BAR0
	buf[1] = startOffset;	//地址偏移量
	buf[2] = registerCout;	//读取寄存器个数
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_REGISTER_READALL, buf, len * sizeof(unsigned int), \
		regValBuf, sizeof(unsigned int) * registerCout, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}

//写单个寄存器
int Device::HJRegisterWrite(IN int drvHandle, IN unsigned int registerOffset, IN const unsigned int *registerVal)
{
	if(!registerVal || !drvHandle || registerOffset % 4)
		return -1;

	unsigned int  len = 3; 
	unsigned int *buf = new unsigned int[len];
	buf[0] = BAR0;			//BAR0
	buf[1] = registerOffset;//地址偏移量
	memcpy(buf + 2, registerVal, sizeof(unsigned int));

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_REGISTER_WRITE, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}

//写入多个连续的寄存器
int Device::HJRegisterWriteAll(IN int drvHandle, IN unsigned int startOffset, IN unsigned int registerCout, 
	IN const unsigned int *regValBuf)
{
	if(!regValBuf || !drvHandle || registerCout <= 0 || startOffset % 4)
		return -1;

	unsigned int len  = registerCout + 3;
	unsigned int *buf = new unsigned int [len];
	buf[0] = BAR0;			//BAR0
	buf[1] = startOffset;	//地址偏移量
	buf[2] = registerCout;	//写入的寄存器个数
	memcpy(buf + 3, regValBuf, sizeof(unsigned int) * registerCout);

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_REGISTER_WRITEALL, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}

//读取单个寄存器(BAR1~BAR5)
int Device::HJUserRegisterRead(IN int drvHandle, IN unsigned int barRegisterNumber, IN unsigned int registerOffset, 
	OUT unsigned int * registerVal)
{
	if(!registerVal || !drvHandle || barRegisterNumber <= BAR0 || barRegisterNumber >= MaxBarNums \
		|| registerOffset % 4)
		return -1;

	int ret = 0;
	unsigned int   len = 2;
	unsigned int * buf = new unsigned int[len];
	buf[0] = barRegisterNumber;	//基地址寄存器序号
	buf[1] = registerOffset;	//地址偏移量
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_USER_REGISTER_READ, buf, len * sizeof(unsigned int), 
		registerVal, sizeof(unsigned int), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}
//写单个寄存器(BAR1~BAR5)
int Device::HJUserRegisterWrite(IN int drvHandle, IN unsigned int barRegisterNumber, IN unsigned int registerOffset, 
	IN const unsigned int * registerVal)
{
	if(!registerVal || !drvHandle || barRegisterNumber <= BAR0 || barRegisterNumber >= MaxBarNums || registerOffset % 4)
		return -1;

	unsigned int  len = 3;
	unsigned int *buf = new unsigned int[len];
	buf[0] = barRegisterNumber;	//基地址寄存器序号
	buf[1] = registerOffset;	//地址偏移量
	memcpy(buf + 2, registerVal, sizeof(unsigned int));

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_USER_REGISTER_WRITE, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}
//读取多个连续的寄存器(BAR1~BAR5)
int Device::HJUserRegisterReadAll(IN int drvHandle, IN unsigned int barRegisterNumber, IN unsigned int startOffset, 
	IN unsigned int registerCout, OUT unsigned int * regValBuf)
{
	if(!regValBuf || !drvHandle || (registerCout <= 0) || barRegisterNumber <= BAR0 || barRegisterNumber >= MaxBarNums \
		|| startOffset % 4)
		return -1;

	int ret  = 0;
	unsigned int  len = 3;
	unsigned int *buf = new unsigned int [len];
	buf[0] = barRegisterNumber;	//基地址寄存器序号
	buf[1] = startOffset;		//地址偏移量
	buf[2] = registerCout;		//读取寄存器个数
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_USER_REGISTER_READALL, buf, len * sizeof(unsigned int), \
		regValBuf, sizeof(unsigned int) * registerCout, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}
//写入多个连续的寄存器(BAR1~BAR5)	
int Device::HJUserRegisterWriteAll(IN int drvHandle, IN unsigned int barRegisterNumber, IN unsigned int startOffset, 
	IN unsigned int registerCout, IN const unsigned int * regValBuf)
{
	if(!regValBuf || !drvHandle || registerCout <= 0 || barRegisterNumber <= BAR0 || barRegisterNumber >= MaxBarNums \
		|| startOffset % 4)
		return -1;

	unsigned int len  = registerCout + 3;
	unsigned int *buf = new unsigned int [len];
	buf[0] = barRegisterNumber;	//基地址寄存器序号
	buf[1] = startOffset;		//地址偏移量
	buf[2] = registerCout;		//写入的寄存器个数
	memcpy(buf + 3, regValBuf, sizeof(unsigned int) * registerCout);

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_USER_REGISTER_WRITEALL, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}

//开启SGL(分散聚合链式)DMA
int Device::HJDmaSglChannelEnable(IN int drvHandle, IN int dmaChannel)
{
	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_ENABLE, &dmaChannel, sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret;
}

//停止SGL(分散聚合链式)DMA传输
int Device::HJDmaSglChannelDisable(IN int drvHandle, IN int dmaChannel)
{
	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_DISABLE, &dmaChannel, sizeof(unsigned int),
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret;
}

//复位SGL(分散聚合链式)DMA	
int Device::HJDmaSglChannelReset(IN int drvHandle, IN int dmaChannel)
{
	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_RESET, &dmaChannel, sizeof(unsigned int),
		NULL, 0, (LPDWORD)&ret, NULL)) 
	{
		ret = -1;
	}
	return ret;
}

//设定SGL(分散聚合链式)DMA传输类型, 0:FIFO, 1:驱动读写文件
int Device::HJDmaSglChannelSetTranType(IN int drvHandle, IN int dmaChannel, IN int type)
{
	unsigned int  len = 2;
	unsigned int *buf = new unsigned int [len];
	buf[0] = dmaChannel;	//通道序号
	buf[1] = type;			//传输类型

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_SETTRANSTYPE, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}

	delete [] buf;
	return ret;
}

//设定SGL DMA传输方向, 0:上行, 1:下行
int Device::HJDmaSglChannelSetTranDir(IN int drvHandle, IN int dmaChannel, IN int dir)
{
	unsigned int len   = 2;
	unsigned int * buf = new unsigned int [len];
	buf[0] = dmaChannel;	//通道序号
	buf[1] = dir;			//传输方向

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_SETTRANSDIR, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}

	delete [] buf;
	return ret;
}

//设置SGL(分散聚合链式)DMA每个DMA块传输的大小
int Device::HJDmaSglChannelSetDmaSize(IN int drvHandle, IN int dmaChannel, IN int size)
{
	int len = 2;
	unsigned int * buf = new unsigned int [len];
	buf[0] = dmaChannel;	//通道序号
	buf[1] = size;			//单个DMA传输的大小

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_SETDMASIZE, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret;
}

//设置SGL(分散聚合链式)DMA传输的总DMA个数
int Device::HJDmaSglChannelSetDmaCount(IN int drvHandle, IN int dmaChannel, IN int count)
{
	int len = 2;
	unsigned int * buf = new unsigned int [len];
	buf[0] = dmaChannel;	//通道序号
	buf[1] = count;			//DMA个数

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_SETDMACOUNT, buf, len * sizeof(unsigned int), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret;
}

//设置SGL(分散聚合链式)DMA传输的上行和下行文件路径和名称
int Device::HJDmaSglChannelSetFilePath(IN int drvHandle, IN int dmaChannel, IN TAG_UPDW_FILEPATH filePath)
{
	int ret = 0;
	if(filePath.Length <= 0 || filePath.Length >= MAX_UPDW_FILEPATH_NUMS)
		return (ret = -1);
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_SGLDMA_CHANNEL_SETFILEPATH, &filePath, sizeof(TAG_UPDW_FILEPATH), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret;
}

//获取指定的基地址寄存器的内存范围
int Device::HJPciBarRangeGet(IN int drvHandle, IN unsigned int barRegisterNumber, OUT unsigned int * data)
{
	if(!drvHandle)
		return -1;
	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCIBAR_GETRANGE, &barRegisterNumber, sizeof(unsigned int), 
		data, sizeof(unsigned int), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret > 0 ? ret / 4 : ret;
}

//获取PCIE设备所有BAR的用户虚拟地址
int Device::HJPciBaseAddressesGet(IN int drvHandle, OUT PTAG_VIRTUAL_ADDRESSES virtAddr)
{
	if(!drvHandle || !virtAddr)
		return -1;
	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCIBAR_GETBASEADDRESS, NULL, 0, 
		virtAddr, sizeof(TAG_VIRTUAL_ADDRESSES), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret;
}

//获取SDK API版本信息
int Device::HJPci_ApiVersion(
	OUT U8 *pVersionMajor, 
	OUT U8 *pVersionMinor, 
	OUT U8 *pVersionRevison)
{
	if((pVersionMajor	== NULL) ||
	   (pVersionMinor	== NULL) ||
	   (pVersionRevison == NULL))
	{
		return -1;
	}
	*pVersionMajor = HJ_SDK_VERSION_MAJOR;
	*pVersionMinor = HJ_SDK_VERSION_MINOR / 10;
	*pVersionRevison = HJ_SDK_VERSION_MINOR - (*pVersionMinor * 10);
	return 0;
}

//获取PCIE驱动版本信息
int Device::HJPci_DriverVersion(
	IN int drvHandle, 
	OUT U8 *pVersionMajor, 
	OUT U8 *pVersionMinor, 
	OUT U8 *pVersionRevision)
{
	if ((pVersionMajor == NULL)    ||
		(pVersionMinor == NULL)    ||
		(pVersionRevision == NULL) ||
		!drvHandle)
	{
		return -1;
	}
	// Clear version information in case of error
	*pVersionMajor	  = 0;
	*pVersionMinor	  = 0;
	*pVersionRevision = 0;

	int ret = 0;
	U32 buf = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_GET_DRIVER_VERSION_INF, NULL, 0, 
		&buf, sizeof(U32), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	*pVersionMajor	  = (U8)((buf >> 16) & 0xFF);
	*pVersionMinor	  = (U8)((buf >> 8)  & 0xFF) / 10;
	*pVersionRevision = (U8)((buf >> 8) & 0xFF) - (*pVersionMinor * 10);
	return ret;
}

//读配置寄存器操作
int Device::HJPci_ConfigRegisterRead(IN U8 bus, IN U8 slot, IN U16 offset, OUT U32 * pData)
{
	if(!pData)
		return -1;

	U32 *buf = new U32[3];
	buf[0] = bus;						//总线号
	buf[1] = slot;						//设备号
	buf[2] = offset;					//地址偏移量

	TAG_DEVICE_LOCATION Device;
	Device.BusNumber		= (U8)-1;
	Device.SlotNumber		= (U8)-1;
	Device.FuncNumber		= (U8)-1;
	Device.VendorId			= (U16)-1;
	Device.DeviceId			= (U16)-1;
	//strcpy((char *)Device.SerialNumber, "Pcie-0");
	Device.SerialNumber[0]	= '\0';

	//打开PCIE板卡设备
	int     tmpDevice;
	HANDLE  tmpHandle;
	tmpDevice = Device::HJPCI_DeviceOpen(&Device, &tmpHandle);
	if(!tmpHandle)
	{
		return -1;
	}
	
	int  ret = 0;
	BOOL Result = DeviceIoControl(tmpHandle, IOCTL_PCI_REGISTER_READ, buf, 3 * sizeof(U32), \
		pData, sizeof(U32), (LPDWORD)&ret, NULL);
	if(Result == FALSE)
	{
		ret = -1;
	}

	//关闭板卡设备
	if(tmpHandle)
		Device::HJPCI_DeviceClose(tmpHandle);

	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}

//写配置寄存器操作
int Device::HJPci_ConfigRegisterWrite(IN U8 bus, IN U8 slot, IN U16 offset, IN U32 * pData)
{
	if(!pData)
		return -1;

	U32 *buf = new U32[4];
	buf[0] = bus;						//总线号
	buf[1] = slot;						//设备号
	buf[2] = offset;					//地址偏移量
	memcpy(buf + 3, pData, sizeof(U32));//要写入的数据

	TAG_DEVICE_LOCATION Device;
	Device.BusNumber		= (U8)-1;
	Device.SlotNumber		= (U8)-1;
	Device.FuncNumber		= (U8)-1;
	Device.VendorId			= (U16)-1;
	Device.DeviceId			= (U16)-1;
	//strcpy((char *)Device.SerialNumber, "Pcie-0");
	Device.SerialNumber[0]	= '\0';

	//打开PCIE板卡设备
	int     tmpDevice;
	HANDLE  tmpHandle;
	tmpDevice = Device::HJPCI_DeviceOpen(&Device, &tmpHandle);
	if(!tmpHandle)
	{
		return -1;
	}

	int ret = 0;
	if(!DeviceIoControl(tmpHandle, IOCTL_PCI_REGISTER_WRITE, buf, 4 * sizeof(U32), 
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}

	//关闭板卡设备
	if(tmpHandle)
		Device::HJPCI_DeviceClose(tmpHandle);

	delete [] buf;
	return ret > 0 ? ret / 4 : ret;
}


//添加应用程序与驱动程序间进行交互的事件对象
int Device::HJPci_AddRefEvent(IN int drvHandle, IN int dmaChannel, IN HANDLE tevent) 
{
	int			   len = 2;
	unsigned int * buf = new unsigned int[len];
	buf[0] = dmaChannel;
	buf[1] = (unsigned int)tevent;

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCI_ADD_REFEVENT, buf, len * sizeof(unsigned int),
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] buf;
	return ret;
}

//删除应用程序与驱动程序间交互的事件对象
int Device::HJPci_DelRefEvent(IN int drvHandle, IN int dmaChannel) 
{
	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCI_DEL_REFEVENT, &dmaChannel, sizeof(unsigned int),
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret;
}

//在DMA上行中通过FIFO方式读取数据
int Device::HJPci_DataRead(IN int drvHandle, IN int dmaChannel, OUT unsigned int* buf, IN int len) 
{
	if(!buf || len <= 0)
	{
		return -1;
	}

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCI_FIFO_READDATA, &dmaChannel, sizeof(unsigned int),
		buf, len * sizeof(unsigned int), (LPDWORD)&ret, NULL))
	{
		ret = -1;
		//printf("GetLastError()=%d\n", GetLastError());
	}
	//return ret > 0 ? ret / 4 : ret;
	return ret;	//返回读取到的字节数
}

//在DMA下行中通过FIFO方式写入数据
int Device::HJPci_DataWrite(IN int drvHandle, IN int dmaChannel, IN unsigned int* buf, IN int len)
{
	if(!buf || len <= 0) 
	{
		return -1;
	}

	unsigned int   tlen = len + 1;
	unsigned int * tbuf = new unsigned int[tlen];
	tbuf[0] = dmaChannel;								//通道序号
	memcpy(tbuf + 1, buf, len * sizeof(unsigned int));	//数据内容

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCI_FIFO_WRITEDATA, tbuf, tlen * sizeof(unsigned int),
		NULL, 0, (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	delete [] tbuf;
	//return ret > 0 ? ret / 4 : ret;
	return ret;
}

//获取状态值
int Device::HJPci_GetState(IN int drvHandle, IN int dmaChannel, OUT unsigned int*buf, IN int len)
{
	if(!buf || len <= 0) 
	{
		return -1;
	}

	int ret = 0;
	if(!DeviceIoControl((HANDLE)drvHandle, IOCTL_PCI_GET_STAT, &dmaChannel, sizeof(unsigned int),
		buf, len * sizeof(unsigned int), (LPDWORD)&ret, NULL))
	{
		ret = -1;
	}
	return ret > 0 ? ret / 4 : ret;
}
