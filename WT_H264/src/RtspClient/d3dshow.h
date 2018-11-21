#ifndef _D3D_SHOW_H
#define _D3D_SHOW_H

#include "d3d9.h"
#include "d3dx9.h"

// 像素格式
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

	int init_flag;    //0 没有初始化		1已经初始化

}d3d_handle_t;

typedef struct d3d_image_data_t{
	int				width;			/**<宽度*/
	int				height;			/**<高度*/
	int				pitch[6];		/**<图像宽度的一行像素所占内存字节数
										BGR24:
										uPitch[0]: BGR一行像素所占内存字节数
										
										YUV420P：
										uPitch[0]: Y一行像素所占内存字节数
										uPitch[1]: U一行像素所占内存字节数
										uPitch[2]: V一行像素所占内存字节数
									*/
	unsigned char	*pBuffer[6];	/**<图像内存的首地址
										BGR24:
										pBuffer[0]: BGR首地址
										
										YUV420P：
										pBuffer[0]: Y首地址
										pBuffer[1]: U首地址
										pBuffer[2]: V首地址
									*/
}d3d_image_data_t;


extern int __stdcall d3d_init(d3d_handle_t *handle, HWND hwnd, int nPixelFormat, int width, int height);
extern int __stdcall d3d_render(d3d_handle_t *handle, d3d_image_data_t *imageInfo);
extern void __stdcall d3d_cleanup(d3d_handle_t *handle);

#endif