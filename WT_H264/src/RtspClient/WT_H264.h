#ifndef _WT_H264_H
#define _WT_H264_H
#include "stdafx.h"
#include "targetver.h"
typedef unsigned int WT_H264HANDLE;

// ���ظ�ʽ
enum WT_PixelFormat_t{
	WT_PIX_FMT_BGR24,	//BGR24
	WT_PIX_FMT_YUV420P,//yuv420P
};

/*�����ͼ����Ϣ*/
typedef struct
{
	unsigned		uWidth;			/**<���*/
	unsigned		uHeight;		/**<�߶�*/
	unsigned		uPitch[6];		/**<ͼ���ȵ�һ��������ռ�ڴ��ֽ���
										EP_PIX_FMT_BGR24:
										uPitch[0]: BGRһ��������ռ�ڴ��ֽ���
										
										EP_PIX_FMT_YUV420P��
										uPitch[0]: Yһ��������ռ�ڴ��ֽ���
										uPitch[1]: Uһ��������ռ�ڴ��ֽ���
										uPitch[2]: Vһ��������ռ�ڴ��ֽ���
									*/
	unsigned		uPixFmt;		/**<ͼ�����ظ�ʽ���ο�ö�ٶ���ͼ���ʽ��PixelFormat_t��*/
	unsigned char	*pBuffer[6];	/**<ͼ���ڴ���׵�ַ
										EP_PIX_FMT_BGR24:
										pBuffer[0]: BGR�׵�ַ
										
										EP_PIX_FMT_YUV420P��
										pBuffer[0]: Y�׵�ַ
										pBuffer[1]: U�׵�ַ
										pBuffer[2]: V�׵�ַ
									*/
}
WT_ImageInfo_t;
/*
������Ϣ�ṹ��
*/
typedef struct WT_H264Decode_t{
	WT_H264HANDLE	handle;	//���������
	WT_ImageInfo_t	imageInfo;//�����ͼ����Ϣ
	char			reserved[100];//Ԥ��
}WT_H264Decode_t;

/*******************************************
������
	WT_H264Init
��������
	��ʼ�������
����
	��
����ֵ
	�ɹ���0
	ʧ�ܣ�<0
����˵��
	�˽ӿڽ������һ��
*******************************************/
int __stdcall WT_H264Init();

/***************************************************************
��������:
	WT_H264Start
��������
	��H.264������������ʾ	
����
	device_ip[in]:Ҫ���ӵ��豸IP��ַ
	show_window_hwnd[in]:Ҫ��ʾ�Ŀؼ����
	pixelFormat[in]������H264���������ظ�ʽ�������ظ�ʽ����
	show_enable[in]:���ý�����Ƿ�������ʾ���ؼ��� 1����ʾ0������ʾ

����ֵ
	 �ɹ�������H264���
	ʧ�ܣ�0
*******************************************************************/
WT_H264HANDLE __stdcall WT_H264Start(char *device_ip, HWND show_window_hwnd, enum WT_PixelFormat_t pixelFormat, int show_enable) ;
/**************************************************************************************
�������ƣ�
	WT_H264End
����˵����
	����H264������
������
	decode_handle[in]:�������ľ��
����ֵ��
	�ɹ���0
	ʧ�ܣ�<0
˵��
	�Ͽ�H.264�����ʹ���˿ؼ���ʾ����Ҫ��������ShowWindow(TRUE)������ʹ�ؼ�������
**************************************************************************************/
int __stdcall WT_H264End(WT_H264HANDLE decode_handle);
/*************************************************************************************  
��������
		WT_H264Destory
��������
		���ٽ���⣬�ͷ���Դ	
����
	��
����ֵ
	��

˵��
	������Դ�����ʹ���˿ؼ���ʾ����Ҫ��������ShowWindow(TRUE)������ʹ�ؼ�������
***************************************************************************************/
void __stdcall WT_H264Destory();

/**************************************************************************************
��������
		WT_ChangeH264ShowWindows
��������
		����H264��ʾ�Ĵ���
����
		decode_handle:	���������
		hwnd:			����ؼ����
����ֵ
		�ɹ���0
		ʧ�ܣ�<0
*************************************************************************************/
int __stdcall WT_ChangeH264ShowWindows(WT_H264HANDLE decode_handle, HWND hwnd);

/***********************************************************************
˵��
	H.264�������Ļص���ͨ���ûص����Ի�ȡ����������
������
	h264_decode_info[in]:���������ݺ���Ϣ����Ĳ���WT_H264Decode_t�ṹ��
*/
typedef void (*WT_H264DecodeCallback)(WT_H264Decode_t *h264_decode_info);
/***************************************
����˵��
	ע��H.264�����ص�����
����
	h264_decode_callback[in]:ָ��ص�������ָ��
����ֵ
	��
***************************************/
void __stdcall WT_RegH264DecodeEvent(WT_H264DecodeCallback h264_decode_callback);
/*********************************************************
��������
		WT_Snap_JPEG
��������
		ץ�ģ�������һ��JPEGͼ��
����
	image_path[in]���洢ͼ���·���������ļ�����
	decode_handle[in]:�Ѿ��򿪵Ľ��������
����ֵ
	-1��������������Ϸ�
	-2���洢·�����Ϸ�
	-3:	������û�д�
	1:�ɹ�

***********************************************************/
int __stdcall WT_Snap_JPEG(char *image_path,WT_H264HANDLE decode_handle);

#endif