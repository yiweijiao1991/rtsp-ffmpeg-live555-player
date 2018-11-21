#ifndef __WT_VIDEO_DECODE_H_
#define __WT_VIDEO_DECODE_H_
#include "WT_H264.h"

/*
	JPEGͼ��Ĵ洢�ṹ
*/
typedef struct
{
	WT_H264HANDLE	handle;
	void *			user_ptr;
	unsigned		uWidth;			/**<���*/
	unsigned		uHeight;		/**<�߶�*/
	unsigned char	*pBuffer;		
	unsigned		size;	
	char			device_ip[20];	//�豸IP��ַ
	char			reserved[100];
}WT_JPEGInfo_t;
/* **********************************************************************

�ӿ����ƣ�
		WT_Stream_Start
�ӿڹ��ܣ�
		����Ƶ������������ʾ��
������
		device_ip[in]:���IP��ַ
		window_hwnd[in]:��ʾ���ڵľ������ʹ�õ�ʱ��NULL
		pixel_format[in]:���������ظ�ʽ���������ظ�ʽö�ٶ���
		show_enable[in]:���ý�����Ƿ�������ʾ���ؼ���   1����ʾ  0������ʾ
		decode_type[in]:������Ƶ������,0:H.264�� 1��JPEG��
		user_prt		�û��Զ���ָ��


����ֵ
		0:ʧ��
		>0:���ؾ��
****************************************************************************/
WT_H264HANDLE __stdcall WT_Stream_Start(char *device_ip, HWND window_hwnd, enum WT_PixelFormat_t pixel_format, int show_enable,int decode_type,void *user_prt) ;

/* ************************************************* 
˵����
	��ȡJPEg���ݵĻص���������
������
	jpeg_image[in]:JPEG����		
******************************************************/
typedef void (*WT_JPEGCallback)(WT_JPEGInfo_t *jpeg_image);


/******************************************************
�������ƣ�
	WT_RegJPEGEvent
�������ܣ�
	ע��JPEG�ص�����
������
	jpeg_call_back[in]:ָ��JPEG�ص�������ָ��
����ֵ��1
*********************************************************/
int __stdcall WT_RegJPEGEvent(WT_JPEGCallback jpeg_call_back);




/****************************************************
�������ƣ�
	WT_RegH264DecodeEvent_EX
��������:
	Ϊ�Ѿ��򿪵�H.264����������������ص�����
������
	h264_decode_callback[in]:H.264����ص�����
	h264_decode_handle[in]:���������
����ֵ��
	-1ʧ��
	1�ɹ�
****************************************************/
int __stdcall WT_RegH264DecodeEvent_EX(WT_H264DecodeCallback h264_decode_callback,WT_H264HANDLE h264_decode_handle);
/******************************************************
�������ƣ�
	WT_RegJPEGEvent_EX
�������ܣ�
	ע�ᵥ·JPEG�ص�����
������
	jpeg_call_back[in]:ָ��JPEG�ص�������ָ��
	decode_handle[in]:���������
����ֵ��1
*********************************************************/
int __stdcall WT_RegJPEGEvent_EX(WT_JPEGCallback jpeg_call_back,WT_H264HANDLE decode_handle);
#endif