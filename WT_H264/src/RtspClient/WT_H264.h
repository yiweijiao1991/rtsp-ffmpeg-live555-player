#ifndef _WT_H264_H
#define _WT_H264_H
#include "stdafx.h"
#include "targetver.h"
typedef unsigned int WT_H264HANDLE;

// 像素格式
enum WT_PixelFormat_t{
	WT_PIX_FMT_BGR24,	//BGR24
	WT_PIX_FMT_YUV420P,//yuv420P
};

/*解码后图像信息*/
typedef struct
{
	unsigned		uWidth;			/**<宽度*/
	unsigned		uHeight;		/**<高度*/
	unsigned		uPitch[6];		/**<图像宽度的一行像素所占内存字节数
										EP_PIX_FMT_BGR24:
										uPitch[0]: BGR一行像素所占内存字节数
										
										EP_PIX_FMT_YUV420P：
										uPitch[0]: Y一行像素所占内存字节数
										uPitch[1]: U一行像素所占内存字节数
										uPitch[2]: V一行像素所占内存字节数
									*/
	unsigned		uPixFmt;		/**<图像像素格式，参考枚举定义图像格式（PixelFormat_t）*/
	unsigned char	*pBuffer[6];	/**<图像内存的首地址
										EP_PIX_FMT_BGR24:
										pBuffer[0]: BGR首地址
										
										EP_PIX_FMT_YUV420P：
										pBuffer[0]: Y首地址
										pBuffer[1]: U首地址
										pBuffer[2]: V首地址
									*/
}
WT_ImageInfo_t;
/*
解码信息结构体
*/
typedef struct WT_H264Decode_t{
	WT_H264HANDLE	handle;	//解码器句柄
	WT_ImageInfo_t	imageInfo;//解码后图像信息
	char			reserved[100];//预留
}WT_H264Decode_t;

/*******************************************
函数名
	WT_H264Init
函数功能
	初始化解码库
参数
	无
返回值
	成功：0
	失败：<0
特殊说明
	此接口仅需调用一次
*******************************************/
int __stdcall WT_H264Init();

/***************************************************************
函数名称:
	WT_H264Start
函数功能
	打开H.264流，并解码显示	
参数
	device_ip[in]:要连接的设备IP地址
	show_window_hwnd[in]:要显示的控件句柄
	pixelFormat[in]：设置H264解码后的像素格式参照像素格式定义
	show_enable[in]:设置解码后是否主动显示到控件上 1：显示0：不显示

返回值
	 成功：返回H264句柄
	失败：0
*******************************************************************/
WT_H264HANDLE __stdcall WT_H264Start(char *device_ip, HWND show_window_hwnd, enum WT_PixelFormat_t pixelFormat, int show_enable) ;
/**************************************************************************************
函数名称：
	WT_H264End
函数说明：
	结束H264的链接
参数：
	decode_handle[in]:解码器的句柄
返回值：
	成功：0
	失败：<0
说明
	断开H.264后，如果使用了控件显示，需要主动调用ShowWindow(TRUE)函数，使控件出来。
**************************************************************************************/
int __stdcall WT_H264End(WT_H264HANDLE decode_handle);
/*************************************************************************************  
函数名称
		WT_H264Destory
函数功能
		销毁解码库，释放资源	
参数
	无
返回值
	无

说明
	销毁资源后，如果使用了控件显示，需要主动调用ShowWindow(TRUE)函数，使控件出来。
***************************************************************************************/
void __stdcall WT_H264Destory();

/**************************************************************************************
函数名称
		WT_ChangeH264ShowWindows
函数功能
		更改H264显示的窗口
参数
		decode_handle:	解码器句柄
		hwnd:			窗体控件句柄
返回值
		成功：0
		失败：<0
*************************************************************************************/
int __stdcall WT_ChangeH264ShowWindows(WT_H264HANDLE decode_handle, HWND hwnd);

/***********************************************************************
说明
	H.264流解码后的回调，通过该回调可以获取解码后的数据
参数：
	h264_decode_info[in]:解码后的数据和信息具体的参照WT_H264Decode_t结构体
*/
typedef void (*WT_H264DecodeCallback)(WT_H264Decode_t *h264_decode_info);
/***************************************
函数说明
	注册H.264解码后回调函数
参数
	h264_decode_callback[in]:指向回调函数的指针
返回值
	无
***************************************/
void __stdcall WT_RegH264DecodeEvent(WT_H264DecodeCallback h264_decode_callback);
/*********************************************************
函数名称
		WT_Snap_JPEG
函数功能
		抓拍，并保存一张JPEG图像
参数
	image_path[in]：存储图像的路径（包括文件名）
	decode_handle[in]:已经打开的解码器句柄
返回值
	-1：解码器句柄不合法
	-2：存储路径不合法
	-3:	解码器没有打开
	1:成功

***********************************************************/
int __stdcall WT_Snap_JPEG(char *image_path,WT_H264HANDLE decode_handle);

#endif