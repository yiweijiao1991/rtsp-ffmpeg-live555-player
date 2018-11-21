#ifndef _D3D_SHOW_H
#define _D3D_SHOW_H

#include "d3d9.h"
#include "d3dx9.h"

// ���ظ�ʽ
enum D3D_PixelFormat_t{
	D3D_PIX_FMT_BGR24 = 0,
	D3D_PIX_FMT_YUV420P,
};

typedef struct d3d_handle_t{
	IDirect3D9			*pDirect3D9;
	IDirect3DDevice9	*pDirect3DDevice;
	IDirect3DSurface9	*pDirect3DSurfaceRender;
	RECT				m_rtViewport;
	int					pixel_format;
	int					width;
	int					height;

	int init_flag;    //0 û�г�ʼ��		1�Ѿ���ʼ��

}d3d_handle_t;

typedef struct d3d_image_data_t{
	int				width;			/**<���*/
	int				height;			/**<�߶�*/
	int				pitch[6];		/**<ͼ���ȵ�һ��������ռ�ڴ��ֽ���
										BGR24:
										uPitch[0]: BGRһ��������ռ�ڴ��ֽ���
										
										YUV420P��
										uPitch[0]: Yһ��������ռ�ڴ��ֽ���
										uPitch[1]: Uһ��������ռ�ڴ��ֽ���
										uPitch[2]: Vһ��������ռ�ڴ��ֽ���
									*/
	unsigned char	*pBuffer[6];	/**<ͼ���ڴ���׵�ַ
										BGR24:
										pBuffer[0]: BGR�׵�ַ
										
										YUV420P��
										pBuffer[0]: Y�׵�ַ
										pBuffer[1]: U�׵�ַ
										pBuffer[2]: V�׵�ַ
									*/
}d3d_image_data_t;


extern int __stdcall d3d_init(d3d_handle_t *handle, HWND hwnd, int nPixelFormat, int width, int height);
extern int __stdcall d3d_render(d3d_handle_t *handle, d3d_image_data_t *imageInfo);
extern void __stdcall d3d_cleanup(d3d_handle_t *handle);

#endif