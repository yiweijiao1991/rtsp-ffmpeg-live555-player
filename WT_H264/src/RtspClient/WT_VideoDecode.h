#ifndef __WT_VIDEO_DECODE_H_
#define __WT_VIDEO_DECODE_H_
#include "WT_H264.h"

/*
	JPEG图像的存储结构
*/
typedef struct
{
	WT_H264HANDLE	handle;
	void *			user_ptr;
	unsigned		uWidth;			/**<宽度*/
	unsigned		uHeight;		/**<高度*/
	unsigned char	*pBuffer;		
	unsigned		size;	
	char			device_ip[20];	//设备IP地址
	char			reserved[100];
}WT_JPEGInfo_t;
/* **********************************************************************

接口名称：
		WT_Stream_Start
接口功能：
		打开视频流，并解码显示。
参数：
		device_ip[in]:相机IP地址
		window_hwnd[in]:显示窗口的句柄，不使用的时候传NULL
		pixel_format[in]:解码后的像素格式，参照像素格式枚举定义
		show_enable[in]:设置解码后是否主动显示到控件上   1：显示  0：不显示
		decode_type[in]:设置视频流类型,0:H.264流 1：JPEG流
		user_prt		用户自定义指针


返回值
		0:失败
		>0:返回句柄
****************************************************************************/
WT_H264HANDLE __stdcall WT_Stream_Start(char *device_ip, HWND window_hwnd, enum WT_PixelFormat_t pixel_format, int show_enable,int decode_type,void *user_prt) ;

/* ************************************************* 
说明：
	获取JPEg数据的回调函数定义
参数：
	jpeg_image[in]:JPEG数据		
******************************************************/
typedef void (*WT_JPEGCallback)(WT_JPEGInfo_t *jpeg_image);


/******************************************************
函数名称：
	WT_RegJPEGEvent
函数功能：
	注册JPEG回调函数
参数：
	jpeg_call_back[in]:指向JPEG回调函数的指针
返回值：1
*********************************************************/
int __stdcall WT_RegJPEGEvent(WT_JPEGCallback jpeg_call_back);




/****************************************************
函数名称：
	WT_RegH264DecodeEvent_EX
函数功能:
	为已经打开的H.264解码器，单独传入回调函数
参数：
	h264_decode_callback[in]:H.264解码回调函数
	h264_decode_handle[in]:解码器句柄
返回值：
	-1失败
	1成功
****************************************************/
int __stdcall WT_RegH264DecodeEvent_EX(WT_H264DecodeCallback h264_decode_callback,WT_H264HANDLE h264_decode_handle);
/******************************************************
函数名称：
	WT_RegJPEGEvent_EX
函数功能：
	注册单路JPEG回调函数
参数：
	jpeg_call_back[in]:指向JPEG回调函数的指针
	decode_handle[in]:解码器句柄
返回值：1
*********************************************************/
int __stdcall WT_RegJPEGEvent_EX(WT_JPEGCallback jpeg_call_back,WT_H264HANDLE decode_handle);
#endif