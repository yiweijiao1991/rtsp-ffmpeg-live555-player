/*
* Copyright (c) 2014, ����ǧ��
* All rights reserved.
* 
* �ļ����ƣ�vxRTSPClient.h
* �������ڣ�2014��7��24��
* �ļ���ʶ��
* �ļ�ժҪ��RTSP �ͻ��˹�������ز�����ӿ����������͡�
* 
* ��ǰ�汾��1.0.0.0
* ��    �ߣ�
* ������ڣ�2014��7��24��
* �汾ժҪ��
* 
* ȡ���汾��
* ԭ����  ��
* ������ڣ�
* �汾ժҪ��
* 
*/

#ifndef __VXRTSPCLIENT_H__
#define __VXRTSPCLIENT_H__
#include "WT_VideoDecode.h"
#include "d3dshow.h"

///////////////////////////////////////////////////////////////////////////

#ifdef RTSPCLIENT_EXPORTS
#define RTSPCLIENT_API __declspec(dllexport)
#else
#define RTSPCLIENT_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////////////////////////////



#define X_FALSE           0
#define X_TRUE            1
#define X_NULL            0
#define X_ERR_OK          0
#define X_ERR_UNKNOW      (-1)

#define X_REAL_TYPE_H264  1000  //264����
#define X_REAL_TYPE_RGB   1001	//RGB����


///////////////////////////////////////////////////////////////////////////

typedef char              x_int8_t;
typedef unsigned char     x_uint8_t;
typedef short             x_int16_t;
typedef unsigned short    x_uint16_t;
typedef int               x_int32_t;
typedef unsigned int      x_uint32_t;
typedef unsigned int      x_bool_t;
typedef void *            x_handle_t;

typedef char *            x_string_t;

#define  _RECONNECT_TIME_  10

class RTSPCLIENT_API vxRTSPClient
{
	// constructor/destructor
public: 
	vxRTSPClient(void);
	virtual ~vxRTSPClient(void);

	// static invoking methods
public:
	/**************************************************************************
	* FunctionName:
	*     initial_lib
	* Description:
	*     ���ʼ��������
	* Parameter:
	*     @[in ] xt_pv_param: Ԥ��������
	* ReturnValue:
	*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
	*/
	static x_int32_t initial_lib(x_handle_t xt_pv_param);

	/**************************************************************************
	* FunctionName:
	*     uninitial_lib
	* Description:
	*     �ⷴ��ʼ��������
	*/
	static void uninitial_lib(void);

protected:
	/**************************************************************************
	* FunctionName:
	*     thread_work_recv
	* Description:
	*     ���ݲɼ����߳���ں�����
	*/
	static x_uint32_t __stdcall thread_work_recv(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     thread_work_decode
	* Description:
	*     ���ݽ����߳���ں�����
	*/
	static x_uint32_t __stdcall thread_work_decode(x_handle_t pv_param);

	//add by yiweijiao
	/**************************************************************************
	* FunctionName:
	*     thread_work_decode
	* Description:
	*     ���ݽ����߳���ں�����
	*/
	static x_uint32_t __stdcall thread_ReConnect(x_handle_t pv_param);

	/**************************************************************************
	* FunctionName:
	*     realframe_cbk_entry
	* Description:
	*     ʵʱ����֡�ص��ӿڡ�
	*/
	static void realframe_cbk_entry(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type, x_handle_t xt_user);

	// public interfaces
public:
	/**************************************************************************
	* FunctionName:
	*     open
	* Description:
	*     RTSP��ַ�Ĵ򿪲�����
	* Parameter:
	*     @[in ] xt_rtsp_url: RTSP URL ��ַ��
	*     @[in ] xfunc_realcbk: �������ݻص������ӿڡ�
	*    
	* ReturnValue:
	*     �ɹ������� 0��ʧ�ܣ����� �����롣
	*/
	x_int8_t open(const x_string_t xt_rtsp_url,HWND hwnd,enum WT_PixelFormat_t pixelFormat, int nEnable,WT_H264HANDLE handle,int decode_type);

	/**************************************************************************
	* FunctionName:
	*     close
	* Description:
	*     ����رղ�����
	*/
	void close(void);

	/**************************************************************************
	* FunctionName:
	*     is_working
	* Description:
	*     �����Ƿ��ڹ���״̬��
	*/
	inline x_bool_t is_working(void) const
	{
		return (!m_xt_bexitflag);
	}

	/**************************************************************************
	* FunctionName:
	*     real_decode_width
	* Description:
	*     ʵʱ����ʱ�õ���ͼ���ȡ�
	*/
	inline x_int32_t real_decode_width(void) const { return m_xt_width; }

	/**************************************************************************
	* FunctionName:
	*     real_decode_height
	* Description:
	*     ʵʱ����ʱ�õ���ͼ��߶ȡ�
	*/
	inline x_int32_t real_decode_height(void) const { return m_xt_height; }

	/**************************************************************************
	* FunctionName:
	*     real_decode_enable_flip
	* Description:
	*     ʵʱ����ʱ���Ƿ��ͼ����д�ֱ��ת��
	*/
	inline void real_decode_enable_flip(x_bool_t xt_enable) { m_xt_flip = xt_enable; }

	/**************************************************************************
	* FunctionName:
	*     set_max_cached_block_nums
	* Description:
	*     ������󻺴��ڴ���������
	* Parameter:
	*     @[in ] xt_max_nums: ��󻺴��ڴ���������
	* ReturnValue:
	*     void
	*/
	void set_max_cached_block_nums(x_uint32_t xt_max_nums);

	/**************************************************************************
	* FunctionName:
	*     get_max_cached_block_nums
	* Description:
	*     ��ȡ��󻺴��ڴ���������
	*/
	x_uint32_t get_max_cached_block_nums(void) const;

	/**************************************************************************
	* FunctionName:
	*     get_realframe_context
	* Description:
	*     ��ȡ ��Ƶ����������Ϣ��
	* Parameter:
	*     @[out] xt_buf: ��Ϣ������档
	*     @[in out] xt_size: ��Σ���Ϣ�������Ĵ�С���زΣ���Ƶ����������Ϣ�Ĵ�С��
	* ReturnValue:
	*     �ɹ������� X_TRUE��ʧ�ܣ����� X_FALSE��
	*/
	x_bool_t get_realframe_context(x_uint8_t * xt_buf, x_uint32_t & xt_size);

	// inner invoking methods

protected:
	/**************************************************************************
	* FunctionName:
	*     recv_loop
	* Description:
	*     ���ݽ��յ��¼��������̣����� thread_work_recv() �ӿڻص��ò�������
	*/
	void recv_loop(void);

	/**************************************************************************
	* FunctionName:
	*     decode_loop
	* Description:
	*     ���ݽ�����¼��������̣����� thread_work_recv() �ӿڻص��ò�������
	*/
	void decode_loop(void);

	/**************************************************************************
	* FunctionName:
	*     reconnect_loop
	* Description:
	*     ���ݽ�����¼��������̣����� thread_work_recv() �ӿڻص��ò�������
	*/
	void reconnect_loop(void);

	/**************************************************************************
	* FunctionName:
	*     realframe_proc
	* Description:
	*     ʵʱ����֡�ص����մ������̣����� realframe_cbk_entry() �ӿڻص��ò�������
	*/
	void realframe_proc(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type); 

	/**************************************************************************
	* FunctionName:
	*     is_exit_work
	* Description:
	*     ���ع����̵߳��˳���ʶ���Ƿ��˳�����״̬����
	*/
	inline x_bool_t is_exit_work(void) const { return (x_bool_t)m_xt_bexitflag; }

	// class data
protected:
	x_handle_t       m_xt_hthread_recv;         ///< RTSP ���ݲɼ��߳�
	x_handle_t       m_xt_hthread_decode;       ///< H264 ���ݽ����߳�
	x_handle_t       m_xt_hthread_reconnect;       ///�����߳�
	x_int8_t         m_xt_bexitflag;            ///< �����̵߳��˳���ʶ

	x_handle_t       m_xt_rtsp_client;          ///< RTSP �ͻ��˲�������
	x_handle_t       m_xt_realframe_queue;      ///< ���ջص�����Ƶ����֡�Ļ��ζ���

	x_bool_t         m_xt_real_context_valid;   ///< ��ʶ��ǰ������Ƶͷ��������Ϣ�Ƿ��Ѿ���Ч
	x_handle_t       m_xt_real_context_info;    ///< ������Ƶͷ��������Ϣ

	x_string_t       m_xt_rtsp_url;             ///< �򿪵� RTSP URL ��ַ

	x_int32_t        m_xt_width;                ///< ����ʱ�õ���ͼ����
	x_int32_t        m_xt_height;               ///< ����ʱ�õ���ͼ��߶�
	x_bool_t         m_xt_flip;                 ///< ����ʱ�Ƿ��ͼ����д�ֱ��ת
public:
	WT_H264DecodeCallback   m_xfunc_realcbk;           ///< ʵʱ���������ݻص������ӿ�
	WT_JPEGCallback			m_jpeg_call_back_func;	//JPEG �ص�����
	
	//add by yiweijiao
	int m_disconnect_times;//�Ͽ����ӵĳ���ʱ��
	int m_getdata;//�����Ƿ��ȡ������ ��������Ƿ�Ͽ�����
	char m_rtsp_url[200];


	HWND m_hwnd;
	enum WT_PixelFormat_t m_PixelFormat;
	int m_Enable;
	WT_H264HANDLE m_RTSP_handle;
	//��ʾ
//	void ShowImage(unsigned char *pFrameRGB,UINT nSize, int nWidth, int nHeight);
	
	int m_WorkStatue;//0 �û�������ֹ���� 1 �û��������ֻ��߿�ʼ����
	void DestroySource(void);
	/*
	�����̺߳ͽ����̵߳�����״̬
	*/
	int m_decodeThreadStatue; // 0ֹͣ�� 1������
	int m_ReciveThreadStatue; // 0ֹͣ�� 1 ������

	//����������
	int m_decode_type;//0  h264 1jpeg
	char m_snap_image_path[1024];
	void save_snap_Jpeg(char *path,WT_JPEGInfo_t *jpeg_image);
	char m_deviceip[20];
	void* m_user_ptr;
};

///////////////////////////////////////////////////////////////////////////

#endif // __VXRTSPCLIENT_H__

