#ifndef __PCITYPES_H__
#define __PCITYPES_H__


#ifdef __cplusplus
extern "C" {
#endif 

/*******************************************
 *      Windows Type Definitions
 ******************************************/
//#if defined(_WIN32)
    typedef signed char           S8;
    typedef unsigned char         U8;
    typedef signed short          S16;
    typedef unsigned short        U16;
    typedef signed long           S32;
    typedef unsigned long         U32;
    typedef signed _int64         LONGLONG;
    typedef unsigned _int64       ULONGLONG;

    typedef HANDLE                HJ_DRIVER_HANDLE;  // Windows-specific driver handle
//#endif


#ifdef __cplusplus
}
#endif 

#endif 

