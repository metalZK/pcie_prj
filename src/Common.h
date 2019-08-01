#ifndef __COMMON_H__
#define __COMMON_H__

#include "PciTypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_UPDW_FILEPATH_NUMS		200			//���к������ļ�·�������ַ������������ֵ
#define DMA_MBUF_COUNT				32			//��DMA�������������ڴ�Ŀ���,		 ��λ:�ֽ�
#define DMA_MBUF_LEN				0x00100000	//��DMA�������������ڴ���ÿ��Ĵ�С, ��λ:�ֽ�
#define DMA_TRANS_LEN				0x00100000  //DMA������ʵ��ʹ�õ��ڴ��Ĵ�С,   ��λ:�ֽ�

//SDK Version information
#define HJ_SDK_VERSION_MAJOR	1
#define HJ_SDK_VERSION_MINOR	10
#define HJ_SDK_VERSION_STRING	"1.10"
//Define a large value for a signal to the driver
#define FIND_AMOUNT_MATCHED         80001

//���䷽����������
typedef enum _TAG_TRANSDIR
{
	TRANSDIR_UP,	//����
	TRANSDIR_DW,	//����
}TAG_TRANSDIR, *PTAG_TRANSDIR;

//���䷽ʽ,����FIFO��������������дӲ���ļ����ַ�ʽ
typedef enum _TAG_TRANSTYPE
{
	TRANSTYPE_FIFO_STREAM,	//FIFO������
	TRANSTYPE_DRIVER_FILE,	//������дӲ���ļ�
}TAG_TRANSTYPE, *PTAG_TRANSTYPE;

//DMAͨ�����
typedef enum _TAG_DMA_CHANNEL
{
	Channel0,		//DMAͨ��0
	Channel1,		//DMAͨ��1
	Channel2,		//DMAͨ��2
	Channel3,		//DMAͨ��3
	MaxChannelNums	//DMA��ͨ������
}TAG_DMA_CHANNEL, *PTAG_DMA_CHANNEL;

//BAR����ַ�Ĵ�������������
typedef enum _TAG_BAR_SPACE
{
	BAR0,			//����ַ�Ĵ���0, �ڲ�ʹ��
	BAR1,			//����ַ�Ĵ���1, ���û�����
	BAR2,			//����ַ�Ĵ���2, ���û�����
	BAR3,			//����ַ�Ĵ���3, ���û�����
	BAR4,			//����ַ�Ĵ���4, ���û�����
	BAR5,			//����ַ�Ĵ���5, ���û�����
	MaxBarNums		
}TAG_BAR_SPACE, *PTAG_BAR_SPACE;

//PCI�豸�û������ַ�б���������BAR(BAR0~BAR5)��ֵ
typedef struct _TAG_VIRTUAL_ADDRESSES
{
	unsigned int Va0;
	unsigned int Va1;
	unsigned int Va2;
	unsigned int Va3;
	unsigned int Va4;
	unsigned int Va5;
}TAG_VIRTUAL_ADDRESSES, *PTAG_VIRTUAL_ADDRESSES;

//�������ļ�·������ṹ������
typedef struct _TAG_UPDW_FILEPATH
{
	unsigned int         Channel;								//ͨ����
	unsigned int		 TransDir;								//���䷽��0:����, 1:����
	unsigned int		 Length;								//�ļ�·�������Ƶ���Ч����	
	wchar_t				 FilePath[MAX_UPDW_FILEPATH_NUMS];		//�ļ�·��������
}TAG_UPDW_FILEPATH, *PTAG_UPDW_FILEPATH;

//�豸λ����Ϣ�ṹ������
typedef struct _TAG_DEVICE_LOCATION
{
	/*U8  BusNumber;					//PCI�豸���ڵ����ߺ�
	U8  SlotNumber;						//PCI�豸���ڵĲۺ�
	U8  FuncNumber;						//PCI�豸���ڵĹ��ܺ�
	U16 VendorId;						//PCI�豸���ṩ��ID,���ṩ��ʶ���
	U16 DeviceId;						//PCI�豸���豸ID,	���豸ʶ���
	U8  SerialNumber[20];				//PCI�豸���ͺţ���PCI�豸��Ψһʶ���*/

	unsigned char  BusNumber;			//PCI�豸���ڵ����ߺ�
	unsigned char  SlotNumber;			//PCI�豸���ڵĲۺ�
	unsigned char  FuncNumber;			//PCI�豸���ڵĹ��ܺ�
	unsigned short VendorId;			//PCI�豸�Ĺ�Ӧ��ID
	unsigned short DeviceId;			//PCI�豸���豸ID
	unsigned char  SerialNumber[20];	//PCI�豸���ͺţ���PCI�豸��Ψһʶ���

}TAG_DEVICE_LOCATION, *PTAG_DEVICE_LOCATION;

//�����豸�������������ṹ������
typedef struct _TAG_FIND_DEVICE
{
	unsigned int        RequestLimit;
	TAG_DEVICE_LOCATION Device;
}TAG_FIND_DEVICE, *PTAG_FIND_DEVICE;


#ifdef __cplusplus
}
#endif 

#endif 