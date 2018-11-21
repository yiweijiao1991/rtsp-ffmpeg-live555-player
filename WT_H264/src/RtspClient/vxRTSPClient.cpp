/*
* Copyright (c) 2014, ����ǧ��
* All rights reserved.
* 
* �ļ����ƣ�vxRTSPClient.cpp
* �������ڣ�2014��7��24��
* �ļ���ʶ��
* �ļ�ժҪ��RTSP �ͻ��˹�������ز����ӿ����������͡�
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "vxRTSPClient.h"
#include "stdafx.h"
#include "d3dshow.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
};
#endif // __cplusplus

#include <process.h>
#include <cassert>

#include "vxRTSPClient.inl"
#include "vxRTSPMemory.inl"


///////////////////////////////////////////////////////////////////////////

/**************************************************************************
* FunctionName:
*     destroy_thread
* Description:
*     ���ٹ����̡߳�
* Parameter:
*     @[in ] hthread: �߳̾����
*     @[in ] timeout: �ȴ��ĳ�ʱʱ�䡣
* ReturnValue:
*     void
*/
static void destroy_thread(x_handle_t hthread, x_uint32_t xt_timeout)
{
	DWORD dwExitCode = 0;
	if ((X_NULL != hthread) && GetExitCodeThread(hthread, &dwExitCode) && (STILL_ACTIVE == dwExitCode))
	{
		DWORD dwWait = WaitForSingleObject(hthread, xt_timeout);
		if (WAIT_TIMEOUT == dwWait)
		{
			TerminateThread(hthread, -1);
		}
	}

	CloseHandle(hthread);
}
static char *get_time_string(void)   
{   
	time_t tt;
	struct tm *t;
	static char timebuf[64];

	time(&tt);
	t = gmtime(&tt); 
	sprintf(&timebuf[0],"%04d-%02d-%02d %02d:%02d:%02d ",
		t->tm_year + 1900, t->tm_mon+1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec);

	return  &timebuf[0];  
}

//int log_write(fmt, args...)
int log_write(const char *pFormat, ...)
{	
	char 		*timestr;
	char		loginfo[1024] = {0};
	va_list		args;

	va_start(args, pFormat);
	vsprintf(loginfo,pFormat,args);
	va_end(args);
	FILE* fp = fopen("H264.log", "ab+");
	if(fp == NULL)
	{
		return -1;
	}

	timestr = get_time_string();
	fwrite(timestr,strlen(timestr), 1, fp);
	fwrite(loginfo,strlen(loginfo), 1, fp);
	fwrite("\r\n",strlen("\r\n"), 1, fp);
	fclose(fp);

	return 0;	
}


///////////////////////////////////////////////////////////////////////////
// vxFFmpegDecode definition

class vxFFmpegDecode
{
	// constructor/destructor
public:
	vxFFmpegDecode(void);
	~vxFFmpegDecode(void);

	// public interfaces
public:
	/**************************************************************************
	* FunctionName:
	*     initial
	* Description:
	*     �����ʼ��������
	* Parameter:
	*     @[in ] xt_decode_id: ������ ID��
	* ReturnValue:
	*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
	*/
	x_int32_t initial(x_int32_t xt_decode_id = AV_CODEC_ID_H264);

	/**************************************************************************
	* FunctionName:
	*     release
	* Description:
	*     ���󷴳�ʼ��������
	*/
	void release(void);

	/**************************************************************************
	* FunctionName:
	*     input_nalu_data
	* Description:
	*     ���뵥�� NALU ���ݰ�ִ�н��롣
	* Parameter:
	*     @[in ] xt_buf: ���ݰ����档
	*     @[in ] xt_size: ���ݰ����泤�ȡ�
	*     @[in ] xt_flip: ��ʶ�Ƿ�Խ���ͼ����д�ֱ��ת��
	* ReturnValue:
	*     ���� x_true����ʾ���һ֡������ͼ����빤�������� X_FALSE, ��ʾδ��ɡ�
	*/
	x_bool_t input_nalu_data(x_uint8_t * xt_buf, x_uint32_t xt_size, x_bool_t xt_flip,WT_ImageInfo_t	*imageInfo,enum WT_PixelFormat_t m_PixelFormat,WT_JPEGInfo_t *jpeg_image,int decode_type,HWND hwnd,int show_enable);

	/**************************************************************************
	* FunctionName:
	*     decode_buf
	* Description:
	*     ���� �������ͼ������ �Ļ����ַ��
	*/
	inline x_uint8_t * decode_data(void) { return (x_uint8_t *)m_xmem_decode.data(); }

	/**************************************************************************
	* FunctionName:
	*     decode_width
	* Description:
	*     ���� �������ͼ�� �Ŀ�ȡ�
	*/
	inline x_int32_t decode_width(void) const { return m_xt_width; }

	/**************************************************************************
	* FunctionName:
	*     decode_height
	* Description:
	*     ���� �������ͼ�� �ĸ߶ȡ�
	*/
	inline x_int32_t decode_height(void) const { return m_xt_height; }

	/*JPEGѹ��
		width ��
		height ��
		xt_buf ѹ�����JPEG���ݴ洢��ַ
		yuv420_fram ԭʼ����֡

	*/
	int	jpeg_endoce(int width,int height,x_uint8_t * xt_buf,AVFrame * yuv420_fram);
	void ShowImageYUV420P(AVFrame * decode_frame,int width,int height);

	// class data
protected:
	AVCodecContext    * m_avcodec_context_ptr;	//H.264����������
	AVCodec           * m_avcodec_ptr;			//H.264������
	AVFrame           * m_avframe_decode;		//������֡
	AVFrame           * m_avframe_rgb_buf;	    //RGB֡
	struct SwsContext * m_sws_context_ptr;		//��ʽת��������
	AVPacket            m_avpacket_decode;		//��Ҫ������İ�����
	//add by yiweijiao
	AVCodec			 *m_jpeg_enccode;	//jpeg������
	AVCodecContext	 *m_jpeg_encode_contex; //JPEG����������

	x_int32_t           m_xt_width;	//ͼ���
	x_int32_t           m_xt_height;//ͼ���
	xmemblock           m_xmem_decode;//����ͼ�񻺳�

	d3d_handle_t m_d3d;
	HWND m_hwnd;

};

///////////////////////////////////////////////////////////////////////////
// vxFFmpegDecode implementation

//=========================================================================

// 
// vxFFmpegDecode constructor/destructor
// 

vxFFmpegDecode::vxFFmpegDecode(void)
			: m_avcodec_context_ptr(X_NULL)
			, m_avcodec_ptr(X_NULL)
			, m_avframe_decode(X_NULL)
			, m_avframe_rgb_buf(X_NULL)
			, m_sws_context_ptr(X_NULL)
			, m_xt_width(0)
			, m_xt_height(0)
{
	m_jpeg_enccode = NULL;
	m_jpeg_encode_contex = NULL;
	memset(&m_d3d,0,sizeof(d3d_handle_t));
}

vxFFmpegDecode::~vxFFmpegDecode(void)
{
	release();
}

//=========================================================================

// 
// vxFFmpegDecode public interfaces
// 

/**************************************************************************
* FunctionName:
*     initial
* Description:
*     �����ʼ��������
* Parameter:
*     @[in ] xt_decode_id: ������ ID��
* ReturnValue:
*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
*/
x_int32_t vxFFmpegDecode::initial(x_int32_t xt_decode_id )
{
	x_int32_t xt_err = X_ERR_UNKNOW;

	do 
	{
		// ������Ч�ж�
		if ((X_NULL != m_avcodec_context_ptr) ||
			(X_NULL != m_avcodec_ptr) ||
			(X_NULL != m_avframe_decode))
		{
			break;
		}

		// ��ʼ�� ffmpeg �������
		av_init_packet(&m_avpacket_decode);

		// ������
		m_avcodec_ptr = avcodec_find_decoder((AVCodecID)xt_decode_id);
		if (X_NULL == m_avcodec_ptr)
		{
			break;
		}

		m_avcodec_context_ptr = avcodec_alloc_context3(m_avcodec_ptr);
		if (X_NULL == m_avcodec_context_ptr)
		{
			break;
		}

		//if (avcodec_open(m_avcodec_context_ptr, m_avcodec_ptr) < 0)
		if (avcodec_open2(m_avcodec_context_ptr, m_avcodec_ptr,0) < 0)
		{
			break;
		}

		// �������֡
		m_avframe_decode = avcodec_alloc_frame();
		if (X_NULL == m_avframe_decode)
		{
			break;
		}
		//JPEGѹ����
		m_jpeg_enccode = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
		if (X_NULL == m_jpeg_enccode)
		{
			break;
		}
		//JPEGѹ�������Ļ���
		m_jpeg_encode_contex = avcodec_alloc_context3(m_jpeg_enccode);
		if (X_NULL == m_jpeg_encode_contex)
		{
			break;
		}	

		m_jpeg_encode_contex->codec_type = AVMEDIA_TYPE_VIDEO;//����������
		//m_jpeg_encode_contex->bit_rate = 4000000;
		m_jpeg_encode_contex->width = 1920;
		m_jpeg_encode_contex->height = 1088;
		m_jpeg_encode_contex->time_base.num = 1;
		m_jpeg_encode_contex->time_base.den = 25;
		//m_jpeg_encode_contex->gop_size = 75;
		m_jpeg_encode_contex->pix_fmt = PIX_FMT_YUVJ420P;

		if ( avcodec_open2(m_jpeg_encode_contex, m_jpeg_enccode, NULL) < 0)
		{
			break;
		}
		xt_err = X_ERR_OK;
	} while (0);

	return xt_err;
}

/**************************************************************************
* FunctionName:
*     release
* Description:
*     ���󷴳�ʼ��������
*/
void vxFFmpegDecode::release(void)
{
	av_free_packet(&m_avpacket_decode);

	if (X_NULL != m_avcodec_context_ptr)
	{
		avcodec_close(m_avcodec_context_ptr);
		av_free(m_avcodec_context_ptr);
		m_avcodec_context_ptr = X_NULL;
	}


	if (X_NULL != m_jpeg_encode_contex)
	{
		avcodec_close(m_jpeg_encode_contex);
		av_free(m_jpeg_encode_contex);
		m_jpeg_encode_contex = X_NULL;
	}

	m_jpeg_enccode= X_NULL;
	m_avcodec_ptr = X_NULL;
	if (X_NULL != m_avframe_decode)
	{
		av_free(m_avframe_decode);
		m_avframe_decode = X_NULL;
	}

	if (X_NULL != m_avframe_rgb_buf)
	{
		av_free(m_avframe_rgb_buf);
		m_avframe_rgb_buf = X_NULL;
	}
	if (X_NULL != m_sws_context_ptr)
	{
		sws_freeContext(m_sws_context_ptr);
		m_sws_context_ptr = X_NULL;
	}
	if (m_d3d.init_flag == 1)
	{
		d3d_cleanup(&m_d3d);	
		m_hwnd = NULL;
	}

}
/*
JPEGѹ��
*/
int vxFFmpegDecode::jpeg_endoce(int width,int height,x_uint8_t * xt_buf,AVFrame * yuv420_fram)
{

	AVPacket pkt;
	int got_output = 0;
	int jpeg_data_size = 0;
	//�����ߺ�֮ǰ��һ������Ҫ�رս��룬���´���������
	if(width != m_jpeg_encode_contex->width || height != m_jpeg_encode_contex->height)
	{
		if(m_jpeg_encode_contex != NULL)
		{

			avcodec_close(m_jpeg_encode_contex);
			av_free(m_jpeg_encode_contex);
			m_jpeg_encode_contex = X_NULL;
		}
		//JPEGѹ�������Ļ���
		m_jpeg_encode_contex = avcodec_alloc_context3(m_jpeg_enccode);
		if (X_NULL == m_jpeg_encode_contex)
		{
			return -1;
		}	
		m_jpeg_encode_contex->codec_type = AVMEDIA_TYPE_VIDEO;//����������	
		m_jpeg_encode_contex->width = width;
		m_jpeg_encode_contex->height = height;
		m_jpeg_encode_contex->time_base.num = 1;
		m_jpeg_encode_contex->time_base.den = 25;	
		m_jpeg_encode_contex->pix_fmt = PIX_FMT_YUVJ420P;

		if ( avcodec_open2(m_jpeg_encode_contex, m_jpeg_enccode, NULL) < 0)
		{
			av_free(m_jpeg_encode_contex);
			m_jpeg_encode_contex = X_NULL;
			return -1;
		}

	}
	//��ʼ����
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	//ѹ��
	avcodec_encode_video2(m_jpeg_encode_contex, &pkt, yuv420_fram, &got_output);
	//�ж��Ƿ��ȡ������
	if (got_output)
	{
		 memcpy(xt_buf,pkt.data, pkt.size);
		jpeg_data_size = pkt.size;
		av_free_packet(&pkt);
	     return jpeg_data_size;	 
	} 
	
	return -1;
}
/**************************************************************************
* FunctionName:
*     input_nalu_data
* Description:
*     ���뵥�� NALU ���ݰ�ִ�н��롣
* Parameter:
*     @[in ] xt_buf: ���ݰ����档
*     @[in ] xt_size: ���ݰ����泤�ȡ�
*     @[in ] xt_flip: ��ʶ�Ƿ�Խ���ͼ����д�ֱ��ת��
	  @[out] imageInfo:���������ݣ��ص�������Ҫ�ýṹ����ȡ����������
	  @[in]	 m_PixelFormat:������������� �ֱ�ΪYUV420P��RGB �������ö��
	  @[in]	jpeg_call_back_func ��ȡJPEGͼ��Ļص�����
	  @[in] decode_type:���������� 0 H.264������ 1 JPEG������
	  @[in] hwnd:��ʾ���ھ��
	  @[in] show_enable:�Ƿ������ʾ 0����ʾ 1 ��ʾ
* ReturnValue:
*     ���� x_true����ʾ���һ֡������ͼ����빤�������� X_FALSE, ��ʾδ��ɡ�
*/
x_bool_t vxFFmpegDecode::input_nalu_data(x_uint8_t * xt_buf, x_uint32_t xt_size, x_bool_t xt_flip,WT_ImageInfo_t	*imageInfo,enum WT_PixelFormat_t m_PixelFormat,WT_JPEGInfo_t *jpeg_image,int decode_type,HWND hwnd,int show_enable)
{
	m_avpacket_decode.data = xt_buf;
	m_avpacket_decode.size = (int)xt_size;
	int decoded_frame = 0;

	if(decode_type == 1)
	{
		//�����JPEG����Ļ�����Ҫ����JPEGѹ��ֱ�Ӿ��ܻ�ȡJPEG����
		if(jpeg_image->pBuffer != NULL)
		{	
			if(xt_buf[0] == 0x00 && xt_buf[1] == 0x00 && xt_buf[2] == 0x00&& xt_buf[3] == 0x01 && xt_buf[4] == 0xff && xt_buf[5] == 0xd8)
			{
				memcpy(jpeg_image->pBuffer,xt_buf+4,xt_size-4);
				jpeg_image->size = xt_size-4;
				jpeg_image->uWidth = m_avcodec_context_ptr->width;
				jpeg_image->uHeight = m_avcodec_context_ptr->height;
			}

		}	
		if (imageInfo == NULL && show_enable ==0)
		{
			return true;
		}
	}
	//����
	avcodec_decode_video2(m_avcodec_context_ptr, m_avframe_decode, &decoded_frame, &m_avpacket_decode);
	if (0 != decoded_frame)
	{
		if (m_PixelFormat == WT_PIX_FMT_YUV420P && imageInfo != NULL)
		{
			// ���������ͼ���Ⱥ͸߶�
			imageInfo->uWidth=m_avcodec_context_ptr->width;
			imageInfo->uHeight= m_avcodec_context_ptr->height;
			//����
			imageInfo->pBuffer[0] = m_avframe_decode->data[0];
			imageInfo->pBuffer[1] = m_avframe_decode->data[1];
			imageInfo->pBuffer[2] = m_avframe_decode->data[2];
			imageInfo->uPitch[0]= m_avframe_decode->linesize[0];
			imageInfo->uPitch[1]= m_avframe_decode->linesize[1];
			imageInfo->uPitch[2]= m_avframe_decode->linesize[2];
			imageInfo->uPixFmt = WT_PIX_FMT_YUV420P;
		}
		if(decode_type == 0)
		{
			//�����H.264��������ô��Ҫ��ȡJPRG��������Ҫ����ѹ��
			if(jpeg_image->pBuffer != NULL)
			{	
				jpeg_image->size = jpeg_endoce(m_avcodec_context_ptr->width,m_avcodec_context_ptr->height,jpeg_image->pBuffer,m_avframe_decode);
				jpeg_image->uWidth = m_avcodec_context_ptr->width;
				jpeg_image->uHeight = m_avcodec_context_ptr->height;
			}
		}

		//�жϿ�ߣ����ھ���Ƿ�����仯��������仯��ô�����ж��Ƿ���Ҫ�����ͷţ�Ȼ�����³�ʼ��
		if(m_xt_width != m_avcodec_context_ptr->width  || m_xt_height != m_avcodec_context_ptr->height || m_hwnd != hwnd)
		{
			if (m_d3d.init_flag == 1)
			{
				d3d_cleanup(&m_d3d);	
				m_hwnd = NULL;
			}
		}
		//�ж��Ƿ���Ҫ��d3d���г�ʼ��
		if(m_d3d.init_flag == 0 && show_enable)
		{
			m_hwnd = hwnd;
			d3d_init(&m_d3d,m_hwnd,D3D_PIX_FMT_YUV420P,m_avcodec_context_ptr->width,m_avcodec_context_ptr->height);
		}
		//�������Ϊ��Ҫ��ʾ����ô�ж��Ƿ��Ѿ���ʼ������ʼ����ɺ��ͼ��ʹ��d3d��ʾ
		if(m_d3d.init_flag && show_enable)
			ShowImageYUV420P(m_avframe_decode,m_avcodec_context_ptr->width,m_avcodec_context_ptr->height);

		if (m_PixelFormat == WT_PIX_FMT_BGR24 && imageInfo != NULL)
		{
				
			//�жϿ���Ƿ�����仯������仯��ô���ͷ�ͼ���ʽת�������Ļ��������������������
			if(m_xt_width != m_avcodec_context_ptr->width  || m_xt_height != m_avcodec_context_ptr->height)
			{

				if (X_NULL != m_sws_context_ptr)
				{
					sws_freeContext(m_sws_context_ptr);
					m_sws_context_ptr = X_NULL;

				}

				if (X_NULL != m_avframe_rgb_buf)
				{
					av_free(m_avframe_rgb_buf);
					m_avframe_rgb_buf = X_NULL;
				}
			}
			
			//��ȡRGBͼ��
			if (X_NULL == m_avframe_rgb_buf)
			{	//����RGB����֡������
				m_xmem_decode.auto_resize(4 * m_avcodec_context_ptr->width * m_avcodec_context_ptr->height);

				m_avframe_rgb_buf = avcodec_alloc_frame();
				if (X_NULL == m_avframe_rgb_buf)
				{
					return X_FALSE;
				}

				avpicture_fill((AVPicture *)m_avframe_rgb_buf, (uint8_t *)m_xmem_decode.data(), PIX_FMT_BGR24, m_avcodec_context_ptr->width, m_avcodec_context_ptr->height);
			}

			if (X_NULL == m_sws_context_ptr)
			{	//������ʽת�������Ļ���
				m_sws_context_ptr = sws_getContext(m_avcodec_context_ptr->width, m_avcodec_context_ptr->height, PIX_FMT_YUV420P,
											m_avcodec_context_ptr->width, m_avcodec_context_ptr->height, PIX_FMT_BGR24, SWS_BICUBIC, X_NULL, X_NULL, X_NULL);
				if (X_NULL == m_sws_context_ptr)
				{
					return X_FALSE;
				}
			}

			if (xt_flip)
			{
				// ����Ϊ��ֱ��ת��ʽת���� RGB ��ʽ����
				m_avframe_decode->data[0] = m_avframe_decode->data[0] + m_avframe_decode->linesize[0] * (m_avcodec_context_ptr->height - 1);
				m_avframe_decode->data[1] = m_avframe_decode->data[1] + m_avframe_decode->linesize[1] * (m_avcodec_context_ptr->height / 2 - 1);
				m_avframe_decode->data[2] = m_avframe_decode->data[2] + m_avframe_decode->linesize[2] * (m_avcodec_context_ptr->height / 2 - 1);
				m_avframe_decode->linesize[0] *= -1;
				m_avframe_decode->linesize[1] *= -1;
				m_avframe_decode->linesize[2] *= -1;
			}


			// �� YUV ���ݸ�ʽת���� RGB ����
			try
			{
				sws_scale(m_sws_context_ptr, m_avframe_decode->data, m_avframe_decode->linesize, 0, m_avcodec_context_ptr->height, m_avframe_rgb_buf->data, m_avframe_rgb_buf->linesize);
			}
			catch (CMemoryException* e)
			{
				log_write("input_nalu_data()::sws_scale error CMemoryException ");
				return X_FALSE;
			}
			catch (CFileException* e)
			{
					log_write("input_nalu_data()::sws_scale error CFileException ");
				return X_FALSE;
			}
			catch (CException* e)
			{
				log_write("input_nalu_data()::sws_scale error CException ");
				return X_FALSE;
			}	 

			// ���������ͼ���Ⱥ͸߶�
			imageInfo->uWidth=m_avcodec_context_ptr->width;
			imageInfo->uHeight= m_avcodec_context_ptr->height;

			imageInfo->pBuffer[0] =  (uint8_t *)m_xmem_decode.data();
			imageInfo->uPitch[0]= (int) m_avframe_rgb_buf->linesize;
			imageInfo->uPixFmt = WT_PIX_FMT_BGR24; 
		}	
		//������
		m_xt_width  = m_avcodec_context_ptr->width;
		m_xt_height = m_avcodec_context_ptr->height;
	}

	return (0 != decoded_frame);

}


void vxFFmpegDecode::ShowImageYUV420P(AVFrame * decode_frame,int width,int height)
{
	d3d_image_data_t imageInfo;

	memset(&imageInfo, 0, sizeof(imageInfo));


	imageInfo.pBuffer[0] = decode_frame->data[0];
	imageInfo.pBuffer[1] = decode_frame->data[1];
	imageInfo.pBuffer[2] = decode_frame->data[2];
	imageInfo.pitch[0] = decode_frame->linesize[0];
	imageInfo.pitch[1] = decode_frame->linesize[1];
	imageInfo.pitch[2] = decode_frame->linesize[2];
	imageInfo.width = width;
	imageInfo.height =height;
	d3d_render(&m_d3d, &imageInfo);	
}

///////////////////////////////////////////////////////////////////////////
// vxRTSPClient

//=========================================================================

// 
// vxRTSPClient static invoking methods
// 

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
x_int32_t vxRTSPClient::initial_lib(x_handle_t xt_pv_param)
{
	x_int32_t xt_err = X_ERR_UNKNOW;

	do 
	{
		av_register_all();

		xt_err = X_ERR_OK;
	} while (0);

	return xt_err;
}

/**************************************************************************
* FunctionName:
*     uninitial_lib
* Description:
*     �ⷴ��ʼ��������
*/
void vxRTSPClient::uninitial_lib(void)
{

}

/**************************************************************************
* FunctionName:
*     thread_work_recv
* Description:
*     ���ݲɼ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_recv(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->recv_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     thread_work_decode
* Description:
*     ���ݽ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_decode(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->decode_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     thread_work_decode
* Description:
*     ���ݽ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_ReConnect(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->reconnect_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     realframe_cbk_entry
* Description:
*     ʵʱ����֡�ص��ӿڡ�
*/
void vxRTSPClient::realframe_cbk_entry(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type, x_handle_t xt_user)
{
	vxRTSPClient * pv_this = (vxRTSPClient *)xt_user;
	pv_this->realframe_proc(xt_handle, xt_buf, xt_size, xt_type);
}

//=========================================================================

// 
// vxRTSPClient constructor/destructor
// 

vxRTSPClient::vxRTSPClient(void)
			: m_xt_hthread_recv(X_NULL)
			, m_xt_hthread_decode(X_NULL)
			, m_xt_bexitflag(X_TRUE)
			, m_xt_rtsp_client(X_NULL)
			, m_xt_realframe_queue(X_NULL)
			, m_xt_real_context_valid(X_FALSE)
			, m_xt_real_context_info(X_NULL)
			, m_xt_rtsp_url(X_NULL)
			, m_xt_width(0)
			, m_xt_height(0)
			, m_xt_flip(X_FALSE)
			, m_xfunc_realcbk(X_NULL)
			, m_jpeg_call_back_func(X_NULL)
{
	// ���������ڴ����
	m_xt_realframe_queue = (x_handle_t)(new xmemblock_cirqueue());
	if (X_NULL == m_xt_realframe_queue)
	{
		assert(false);
	}

	m_xt_real_context_info = (x_handle_t)(new xmemblock());
	if (X_NULL == m_xt_real_context_info)
	{
		assert(false);
	}

	x_uint32_t xt_tid;
	m_disconnect_times=0;
	m_getdata=0;
	memset(m_rtsp_url,0,200);
	m_WorkStatue = 0; 

	 m_decodeThreadStatue = 0; // 0ֹͣ�� 
	 m_ReciveThreadStatue = 0; // 0ֹͣ�� 


	// ���������߳�
	m_xt_hthread_reconnect = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_ReConnect, this, 0, &xt_tid);


	 m_hwnd = NULL;

	 m_Enable = 0;
	 m_RTSP_handle = 0;
	 memset(m_deviceip,0,20);
	 m_user_ptr = NULL;
}

vxRTSPClient::~vxRTSPClient(void)
{
	xmemblock * xt_mblk = (xmemblock *)m_xt_real_context_info;
	if (X_NULL != xt_mblk)
	{
		delete xt_mblk;
		xt_mblk = X_NULL;

		m_xt_real_context_info = X_NULL;
	}

	// �����ζ��л���
	xmemblock_cirqueue * xt_cirqueue = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL != xt_cirqueue)
	{
		xt_cirqueue->clear_cir_queue();
		delete xt_cirqueue;
		xt_cirqueue = X_NULL;

		m_xt_realframe_queue = X_NULL;
	}

	close();

	// ���������߳�
	if (X_NULL != m_xt_hthread_reconnect)
	{
		destroy_thread(m_xt_hthread_reconnect, INFINITE);
		m_xt_hthread_reconnect = X_NULL;
	}


}

//=========================================================================

// 
// vxRTSPClient public interfaces
// 

/**************************************************************************
* FunctionName:
*     open
* Description:
*     RTSP��ַ�Ĵ򿪲�����
* Parameter:
*     @[in ] xt_rtsp_url: RTSP URL ��ַ��
*     @[in ] xfunc_realcbk: �������ݻص������ӿڡ�

* ReturnValue:
*     �ɹ������� 0��ʧ�ܣ����� �����롣
*/
x_int8_t vxRTSPClient::open(const x_string_t xt_rtsp_url,HWND hwnd,enum WT_PixelFormat_t pixelFormat, int nEnable,WT_H264HANDLE handle,int decode_type)
{
	x_int8_t xt_err = X_ERR_UNKNOW;

	do 
	{
		// ������Ч�ж�
		if ((X_NULL != m_xt_hthread_recv)    ||
			(X_NULL != m_xt_hthread_decode)  ||
			(X_NULL != m_xt_rtsp_client))
		{
			break;
		}

		// �������
		m_xt_rtsp_url   = _strdup(xt_rtsp_url);
		memset(m_rtsp_url,0,200);
		strcpy(m_rtsp_url,m_xt_rtsp_url);	
		m_hwnd = hwnd;
		m_PixelFormat = pixelFormat;
		m_xt_bexitflag  = X_FALSE;
		m_Enable = nEnable;
		m_RTSP_handle = handle;
		m_decode_type = decode_type;

		// ���� RTSP ���ݽ��յĿͻ��˹�������
		m_xt_rtsp_client = (x_handle_t)vxRtspCliHandle::create(xt_rtsp_url);
		if (X_NULL == m_xt_rtsp_client)
		{
			break;
		}

		// �������ݻص��ӿ�
		((vxRtspCliHandle *)m_xt_rtsp_client)->set_recved_realframe_cbk((CLI_REALFRAME_CBK)&vxRTSPClient::realframe_cbk_entry, this);

		x_uint32_t xt_tid;

		// �������ݽ��յĹ����߳�
		m_xt_hthread_recv = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_work_recv, this, 0, &xt_tid);
		if (X_NULL == m_xt_hthread_recv)
		{
			break;
		}
		CloseHandle(m_xt_hthread_recv);
		m_xt_hthread_recv = X_NULL;
		// �������ݽ���Ĺ����߳�
		m_xt_hthread_decode = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_work_decode, this, 0, &xt_tid);
		if (X_NULL == m_xt_hthread_decode)
		{
			break;
		}
		CloseHandle(m_xt_hthread_decode);
		m_xt_hthread_decode = X_NULL;
		// ���� ��Ƶͷ��������Ϣ
		m_xt_real_context_valid = X_FALSE;
		if (X_NULL != m_xt_real_context_info)
		{
			((xmemblock *)m_xt_real_context_info)->reset();
		}
		xt_err = X_ERR_OK;
	} while (0);

	if (X_ERR_OK != xt_err)
	{
		DestroySource();
	}

	return xt_err;
}


void vxRTSPClient::DestroySource(void)
{
	// �����˳���ʶ
	m_xt_bexitflag = X_TRUE;
	int times = 0;
	//�ж��������߳��Ƿ��Ѿ��˳�
	while(m_decodeThreadStatue || m_ReciveThreadStatue)
	{
		Sleep(10);
		times++;
		if(times > 300) // ���3������ôǿ���˳�
			break;
	}
	try{

		// ɾ�� RTSP ���ݽ��յĿͻ��˶���
		if (X_NULL != m_xt_rtsp_client)
		{
			vxRtspCliHandle::destroy((vxRtspCliHandle *)m_xt_rtsp_client);
			m_xt_rtsp_client = X_NULL;
		}
		// ���� ��Ƶͷ��������Ϣ
		m_xt_real_context_valid = X_FALSE;
		if (X_NULL != m_xt_real_context_info)
		{
			((xmemblock *)m_xt_real_context_info)->reset();
		}

		if (X_NULL != m_xt_rtsp_url)
		{
			free(m_xt_rtsp_url);
			m_xt_rtsp_url = X_NULL;
		}

	}
	catch (CMemoryException* e)
	{
		log_write("DestroySource()::DestroySource error CMemoryException ");
		
	}
	catch (CFileException* e)
	{
		log_write("DestroySource()::DestroySource error CFileException ");
		
	}
	catch (CException* e)
	{
		log_write("DestroySource()::DestroySource error CException ");
		
	}
	m_xt_width  = 0;
	m_xt_height = 0;

}
/**************************************************************************
* FunctionName:
*     close
* Description:
*     ����رղ�����
*/ 
void vxRTSPClient::close(void)
{
 
	
	DestroySource();//����
	m_hwnd = NULL;
}

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
void vxRTSPClient::set_max_cached_block_nums(x_uint32_t xt_max_nums)
{
	if (X_NULL != m_xt_realframe_queue)
	{
		((xmemblock_cirqueue *)m_xt_realframe_queue)->resize_max_blocks(xt_max_nums);
	}
}

/**************************************************************************
* FunctionName:
*     get_max_cached_block_nums
* Description:
*     ��ȡ��󻺴��ڴ���������
*/
x_uint32_t vxRTSPClient::get_max_cached_block_nums(void) const
{
	if (X_NULL != m_xt_realframe_queue)
	{
		return (x_uint32_t)((xmemblock_cirqueue *)m_xt_realframe_queue)->max_blocks_size();
	}

	return 0;
}

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
x_bool_t vxRTSPClient::get_realframe_context(x_uint8_t * xt_buf, x_uint32_t & xt_size)
{
	xmemblock * xmblk = (xmemblock *)m_xt_real_context_info;
	if ((X_NULL == xmblk) || (X_TRUE != m_xt_real_context_valid))
	{
		return X_FALSE;
	}

	if ((X_NULL != xt_buf) && (xt_size >= xmblk->size()))
	{
		memcpy(xt_buf, xmblk->data(), xmblk->size());
	}

	xt_size = xmblk->size();

	return X_TRUE;
}

//=========================================================================

// 
// vxRTSPClient inner invoking methods
// 

/**************************************************************************
* FunctionName:
*     recv_loop
* Description:
*     ���ݽ��յ��¼��������̣����� thread_work_recv() �ӿڻص��ò�������
*/
void vxRTSPClient::recv_loop(void)
{
	
	vxRtspCliHandle * rtsp_client_handle = (vxRtspCliHandle *)m_xt_rtsp_client;
	if (X_NULL == rtsp_client_handle)
	{
		return;
	}
	m_ReciveThreadStatue = 1;
	rtsp_client_handle->do_event_loop(&m_xt_bexitflag);
	rtsp_client_handle->shutdown_stream();

	m_xt_bexitflag = X_TRUE;
	m_ReciveThreadStatue = 0;
}

/**************************************************************************
* FunctionName:
*     decode_loop
* Description:
*     ���ݽ�����¼��������̣����� thread_work_recv() �ӿڻص��ò�������
*/
void vxRTSPClient::decode_loop(void)
{
	unsigned char jpeg_data[1024*500] = {0};
	WT_H264Decode_t rtsp_decode;
	WT_JPEGInfo_t jpeg_image;
	xmemblock * x_block_ptr = X_NULL;

	memset(&rtsp_decode,0,sizeof(rtsp_decode));
	rtsp_decode.handle = m_RTSP_handle;
	memset(&jpeg_image,0,sizeof(WT_JPEGInfo_t));
	



	// ����֡���յĻ��λ������
	xmemblock_cirqueue * x_cirqueue_ptr = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL == x_cirqueue_ptr)
	{
		return;
	}

	// ffmpeg �����������
	vxFFmpegDecode ffdecode;

	// ��ʼ�� ffmpeg �������
	if(m_decode_type == 0)
	{
		if (X_ERR_OK != ffdecode.initial(AV_CODEC_ID_H264))
		{
			return;
		}
	}else
	{
		if (X_ERR_OK != ffdecode.initial(AV_CODEC_ID_MJPEG))
		{
			return;
		}
	}

	m_decodeThreadStatue = 1;
	// ѭ��������յ�������֡��ִ�н��롢�ص��ȹ���
	while (!is_exit_work())
	{
	
		if(m_jpeg_call_back_func || strlen(m_snap_image_path) > 0)
		{
			memset(jpeg_data,0,500*1024);
			jpeg_image.handle = m_RTSP_handle;
			jpeg_image.pBuffer = jpeg_data;
		}

		// ȡ֡����
		x_block_ptr = x_cirqueue_ptr->pop_front_from_saved_queue();
		if (X_NULL == x_block_ptr)
		{
			Sleep(5);
			
			continue;
		}

		m_getdata = 1;
		
		// ��������֡

		if (X_NULL != m_xfunc_realcbk)
		{//ע���˽�������ݻص���������Ҫ����rtsp_decode ���ڴ洢����������
			
			if (ffdecode.input_nalu_data((x_uint8_t *)x_block_ptr->data(), x_block_ptr->size(), m_xt_flip,&(rtsp_decode.imageInfo),m_PixelFormat,&jpeg_image,m_decode_type,m_hwnd,m_Enable))
			{
					//log_write("get frame");	
					m_xt_width  = ffdecode.decode_width();
					m_xt_height = ffdecode.decode_height();
					// ���ݻص�
					if (X_NULL != m_xfunc_realcbk)
					{

						m_xfunc_realcbk(&rtsp_decode);
					}
					
					if(m_jpeg_call_back_func && jpeg_image.size > 0 && jpeg_image.pBuffer) 
					{
						memset(jpeg_image.device_ip,0,20);
						strcpy(jpeg_image.device_ip,m_deviceip);
						jpeg_image.user_ptr = m_user_ptr;
						m_jpeg_call_back_func(&jpeg_image);
					}

					if (strlen(m_snap_image_path) > 0 &&  jpeg_image.size > 0 && jpeg_image.pBuffer)
					{
						save_snap_Jpeg(m_snap_image_path,&jpeg_image);
						memset(m_snap_image_path,0,sizeof(m_snap_image_path));
					}

			}
		}else
		{
			//û��ע���������ݻص����� ��ֻ�������
			if (ffdecode.input_nalu_data((x_uint8_t *)x_block_ptr->data(), x_block_ptr->size(), m_xt_flip,NULL,m_PixelFormat,&jpeg_image,m_decode_type,m_hwnd,m_Enable))
			{
				m_xt_width  = ffdecode.decode_width();
				m_xt_height = ffdecode.decode_height();

				if(m_jpeg_call_back_func && jpeg_image.size > 0 && jpeg_image.pBuffer) 
				{
					memset(jpeg_image.device_ip,0,20);
					strcpy(jpeg_image.device_ip,m_deviceip);
					jpeg_image.user_ptr = m_user_ptr;
					m_jpeg_call_back_func(&jpeg_image);
				}

				if (strlen(m_snap_image_path) > 0 &&  jpeg_image.size > 0 && jpeg_image.pBuffer)
				{
					save_snap_Jpeg(m_snap_image_path,&jpeg_image);
					memset(m_snap_image_path,0,sizeof(m_snap_image_path));
				}
				
			}

		}

		// ����֡�ڴ��
		x_cirqueue_ptr->recyc(x_block_ptr);
		x_block_ptr = X_NULL;


	}
	if (X_NULL != x_block_ptr)
	{
		x_cirqueue_ptr->recyc(x_block_ptr);
	}

	x_cirqueue_ptr->clear_cir_queue();
	m_xt_width  = 0;
	m_xt_height = 0;
	m_xt_bexitflag = X_TRUE;
	m_decodeThreadStatue = 0;

}

void vxRTSPClient::reconnect_loop(void)
{
	// ѭ��������յ�������֡��ִ�н��롢�ص��ȹ���
	while (1)
	{
		
		if ((m_WorkStatue == 1))//�жϸý������Ƿ��Ѿ����򿪣�
		{
			//���������ô��Ҫ���ж��߼��
			if(m_getdata == 0)//�ж��Ƿ��ȡ��������
			{
				m_disconnect_times++;//û��������ô�ۼ�
			}else{
				m_disconnect_times = 0;//��������ô����ۼ�
				m_getdata = 0;//��ձ�ʶ
			}
				
			
			if(m_disconnect_times > _RECONNECT_TIME_)//�ۼƳ�����һ��ʱ��
			{
				//��������
				DestroySource();//������Դ
				Sleep(100);
				real_decode_enable_flip(0);
				open(m_rtsp_url,m_hwnd,m_PixelFormat,m_Enable,m_RTSP_handle,m_decode_type);//���´���
				m_disconnect_times = 0;//���¼���
			} 
		}
		Sleep(500);
	}
}
/**************************************************************************
* FunctionName:
*     realframe_proc
* Description:
*     ʵʱ����֡�ص����մ������̣����� realframe_cbk_entry() �ӿڻص��ò�������
*/
void vxRTSPClient::realframe_proc(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type)
{
	if (m_xt_rtsp_client != xt_handle)
	{
		return;
	}

	xmemblock_cirqueue * x_cirqueue_ptr = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL == x_cirqueue_ptr)
	{
		return;
	}

	xmemblock * x_block_ptr = x_cirqueue_ptr->alloc();
	if (X_NULL == x_block_ptr)
	{
		return;
	}

	x_uint8_t xt_start_code[4] = { 0x00, 0x00, 0x00, 0x01 };
	x_block_ptr->write_block(xt_start_code, 4 * sizeof(x_uint8_t));
	x_block_ptr->append_data(xt_buf, xt_size);

	if (RTSP_FRAMETYPE_RCDPARAM == xt_type)
	{

		if ((X_NULL != m_xt_real_context_info) && (X_NULL != xt_buf))
		{
			((xmemblock *)m_xt_real_context_info)->append_data(x_block_ptr->data(), x_block_ptr->size());
		}

		m_xt_real_context_valid = (X_NULL == xt_buf);
	}

	x_cirqueue_ptr->push_back_to_saved_queue(x_block_ptr);
}
void vxRTSPClient::save_snap_Jpeg(char *file_path,WT_JPEGInfo_t *jpeg_image)
{ 	
		FILE* fp = fopen(file_path,"wb+");
		if(fp == NULL)
		{
			return ;
		}
		fwrite(jpeg_image->pBuffer,jpeg_image->size, 1, fp);
		fclose(fp);
		return ;
}
/*
void vxRTSPClient::ShowImage(unsigned char *pFrameRGB,UINT nSize, int nWidth, int nHeight)
{

	CRect rc;
	HDC hDC = GetDC(m_hwnd);
	CDC *pDC = NULL;
	BITMAPINFO m_bmphdr={0};
	DWORD dwBmpHdr = sizeof(BITMAPINFO);
	if(hDC !=NULL)
	{
		pDC = CDC::FromHandle(hDC);
		m_bmphdr.bmiHeader.biBitCount = 24;
		m_bmphdr.bmiHeader.biClrImportant = 0;
		m_bmphdr.bmiHeader.biSize = dwBmpHdr;
		m_bmphdr.bmiHeader.biSizeImage = 0;
		m_bmphdr.bmiHeader.biWidth = nWidth;
		m_bmphdr.bmiHeader.biHeight = -nHeight;
		m_bmphdr.bmiHeader.biXPelsPerMeter = 0;
		m_bmphdr.bmiHeader.biYPelsPerMeter = 0;
		m_bmphdr.bmiHeader.biClrUsed = 0;
		m_bmphdr.bmiHeader.biPlanes = 1;
		m_bmphdr.bmiHeader.biCompression = BI_RGB;
 
		if(hDC !=NULL)
		{
			GetWindowRect(m_hwnd,rc);
			pDC->SetStretchBltMode(HALFTONE);

			StretchDIBits(hDC,
				0,0,
				rc.Width(),rc.Height(),
				0, 0,
				nWidth, nHeight,
				pFrameRGB,
				&m_bmphdr,
				DIB_RGB_COLORS,
				SRCCOPY);
		}
		ReleaseDC(m_hwnd,hDC);		
	} 
}
*/

