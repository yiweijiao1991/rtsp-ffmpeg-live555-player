#include "WT_VideoDecode.h"
#include "vxRTSPClient.h"


/* ����H264����

	return��
		�ɹ���0
		ʧ�ܣ�<0

	notes��
		�˽ӿڽ������һ��
*/
vxRTSPClient       m_rtspClient[16];
int Find_Decode()
{
	int i = 0;
	for (i = 0; i< 16;i++)
	{
		if(!m_rtspClient[i].is_working())
			return i;
	} 
	return -1;
}
int __stdcall WT_H264Init()
{
	vxRTSPClient::initial_lib(0);
	return 0;
}
/*  
	H264���٣��ͷ���Դ

	notes:
		������Դ����������ؼ���ʾ�ģ���Ҫ����
		����ShowWindow(TRUE)������ʹ�ؼ�������
*/
void __stdcall WT_H264Destory()
{
	int i = 0;
	for(i = 0; i< 16;i++)
	{
		m_rtspClient[i].m_WorkStatue = 0;
		if(m_rtspClient[i].is_working())
			m_rtspClient[i].close();
		m_rtspClient[i].m_xfunc_realcbk = NULL;
		m_rtspClient[i].m_jpeg_call_back_func = NULL;
	}

	vxRTSPClient::uninitial_lib();
}

/* ����H264����
	param:
		chIp:			Ҫ���ӵ��豸IP��ַ
		hwnd:			Ҫ��ʾ�Ŀؼ���� ��ʵ�õ�ʱ���� NULL
		pixelFormat:	����H264���������ظ�ʽ
		nEnable:		���ý�����Ƿ�������ʾ���ؼ���:nEnable:1��ʾ;0����ʾ
						ע������pixelFormat==WT_PIX_FMT_BGR24ʱ���Ż���������ʾ
	
	return��
		�ɹ�������H264���
		ʧ�ܣ�0 ��ʧ��
*/
WT_H264HANDLE __stdcall WT_H264Start(char *chIp, HWND hwnd, enum WT_PixelFormat_t pixelFormat, int nEnable) 
{
	char str[200] = {0};
	int i = 0;
	WT_H264HANDLE handle = 0;

	if (nEnable)
	{
		for (i = 0;i<16;i++)
		{
			if(m_rtspClient[i].m_hwnd == hwnd)
				return 0;
		}
	}	
	sprintf(str,"rtsp://%s/stream1",chIp);
	WT_H264HANDLE ID = Find_Decode();
	if (ID < 0 || ID > 15)
	{
		return 0;
	}
	if(m_rtspClient[ID].is_working())
		return 0;
	handle = ID+1;
	m_rtspClient[ID].real_decode_enable_flip(0);
	if( 0 != m_rtspClient[ID].open(str,hwnd,pixelFormat, nEnable,handle,0))
		return 0;
	m_rtspClient[ID].m_WorkStatue = 1;
	return handle;
}

/* ��������
	param:
		chIp:			Ҫ���ӵ��豸IP��ַ
		hwnd:			Ҫ��ʾ�Ŀؼ���� ��ʵ�õ�ʱ���� NULL
		pixelFormat:	���ý��������ظ�ʽ
		nEnable:		���ý�����Ƿ�������ʾ���ؼ���:nEnable:1��ʾ;0����ʾ
						ע������pixelFormat==WT_PIX_FMT_BGR24ʱ���Ż���������ʾ
		decode_type		�������� 0H.264  1JPEG
		user_prt		�û��Զ���ָ��
	return��
		�ɹ�������H264���
		ʧ�ܣ�0 ��ʧ��
*/
WT_H264HANDLE __stdcall WT_Stream_Start(char *chIp, HWND hwnd, enum WT_PixelFormat_t pixelFormat, int nEnable,int decode_type,void *user_prt) 
{
	char str[200] = {0};
	int i = 0;
	WT_H264HANDLE handle = 0;

	if (nEnable)
	{
		for (i = 0;i<16;i++)
		{
			if(m_rtspClient[i].m_hwnd == hwnd)
				return 0;
		}
	}
	
	if(decode_type == 0)
	{
		sprintf(str,"rtsp://%s/stream1",chIp);
	}
	else if(decode_type == 1)
	{
		sprintf(str,"rtsp://%s/mjpegstream",chIp);
	}else
	{
		return 0;
	}
	WT_H264HANDLE ID = Find_Decode();
	if (ID < 0 || ID > 15)
	{
		return 0;
	}
	if(m_rtspClient[ID].is_working())
		return 0;
	handle = ID+1;
	m_rtspClient[ID].real_decode_enable_flip(0);
	if( 0 != m_rtspClient[ID].open(str,hwnd,pixelFormat, nEnable,handle,decode_type))
		return 0;
	m_rtspClient[ID].m_WorkStatue = 1;
	memset(m_rtspClient[ID].m_deviceip,0,20);
	strcpy(m_rtspClient[ID].m_deviceip,chIp);
	m_rtspClient[ID].m_user_ptr = user_prt;
	return handle;
}


/*  ���������豸��H264������
	param:
		handle:			H264���

	return��
		�ɹ���0
		ʧ�ܣ�<0

	notes:
		�Ͽ�H.264���պ���������ؼ���ʾ�ģ���Ҫ����
		����ShowWindow(TRUE)������ʹ�ؼ�������
*/
int __stdcall WT_H264End(WT_H264HANDLE handle)
{
	int ID = handle -1;
	if(ID < 0 || ID >15)
		return -1;
	m_rtspClient[ID].m_WorkStatue = 0; 
	m_rtspClient[ID].close();
	m_rtspClient[ID].m_xfunc_realcbk = NULL;
	m_rtspClient[ID].m_jpeg_call_back_func = NULL;
	return 0;

}

/*  ����H264��ʾ�Ĵ���
	param:
		handle:			H264���
		hwnd:			����ؼ����

	return��
		�ɹ���0
		ʧ�ܣ�<0
*/
int __stdcall WT_ChangeH264ShowWindows(WT_H264HANDLE handle, HWND hwnd)
{
	int ID = handle -1;
	if(ID < 0 || ID >15)
		return -1;
	m_rtspClient[ID].m_hwnd = hwnd;
	return 0;
}
	
/*  
	ͨ���ص��ķ�ʽ����ȡ���ա�����������
	notes��
		����Ĳ�������WT_H264Decode_t�ṹ��
*/
void __stdcall WT_RegH264DecodeEvent(WT_H264DecodeCallback h264Decode)
{
	int i = 0;
	for (i =0 ;i<16;i++)
	{
		m_rtspClient[i].m_xfunc_realcbk = h264Decode;
	}
	
}

/*Ϊ�Ѿ��򿪵ĵ�·����ⵥ������ص�����
param��
handle:			H264���
h264Decode �ص�����
*/
int __stdcall WT_RegH264DecodeEvent_EX(WT_H264DecodeCallback h264Decode,WT_H264HANDLE handle)
{
	int ID = handle -1;
	if(ID < 0 || ID >15)
		return -1;
	m_rtspClient[ID].m_xfunc_realcbk = h264Decode;
	return 1;

}


/*ע��JPEG�ص�����
param��
handle:			H264���
h264Decode �ص�����
*/
int __stdcall WT_RegJPEGEvent(WT_JPEGCallback jpeg_call_back)
{
	int i = 0;
	for (i =0 ;i<16;i++)
	{
		m_rtspClient[i].m_jpeg_call_back_func = jpeg_call_back;
	}
	return 1;
}

int __stdcall WT_Snap_JPEG(char *image_path,WT_H264HANDLE handle)
{
	int ID = handle -1;

	if(ID < 0 || ID >15)
	{
		return -1;
	}	

	if (!image_path || strlen(image_path) <= 0)
	{
		return -2;
	}
	
	if( ! m_rtspClient[ID].is_working())
	{
		return -3;
	}	
	strcpy(m_rtspClient[ID].m_snap_image_path,image_path);
	Sleep(10);
	return 1;
}

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
int __stdcall WT_RegJPEGEvent_EX(WT_JPEGCallback jpeg_call_back,WT_H264HANDLE decode_handle)
{
	int ID = decode_handle -1;
	if(ID < 0 || ID >15)
		return -1;
	m_rtspClient[ID].m_jpeg_call_back_func = jpeg_call_back;
	return 1;
}