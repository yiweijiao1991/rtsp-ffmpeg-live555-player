#include "stdafx.h"
#include "d3dshow.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

void __stdcall d3d_cleanup(d3d_handle_t *handle)
{
	if(!handle)
		return ;

	if(handle->pDirect3DSurfaceRender)
		handle->pDirect3DSurfaceRender->Release();
	if(handle->pDirect3DDevice)
		handle->pDirect3DDevice->Release();
	if(handle->pDirect3D9)
		handle->pDirect3D9->Release();

	handle->pixel_format = -1;
	handle->width = 0;
	handle->height = 0;
	handle->init_flag = 0;
}


int __stdcall d3d_init(d3d_handle_t *handle, HWND hwnd, int nPixelFormat, int width, int height)
{
	if(!handle 
		|| !hwnd 
		|| (nPixelFormat!=D3D_PIX_FMT_BGR24 && nPixelFormat!=D3D_PIX_FMT_YUV420P)
		|| width<=0
		|| height <= 0)
		return -1;

	HRESULT lRet;
 
	handle->pDirect3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( handle->pDirect3D9 == NULL )
		goto end;
 
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
 
	GetClientRect(hwnd,&handle->m_rtViewport);
 
	lRet=handle->pDirect3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &handle->pDirect3DDevice );
	if(FAILED(lRet))
		goto end;
 
	if(nPixelFormat == D3D_PIX_FMT_BGR24)
	{
		lRet=handle->pDirect3DDevice->CreateOffscreenPlainSurface(
			width,height,
			D3DFMT_R8G8B8,
			D3DPOOL_DEFAULT,
			&handle->pDirect3DSurfaceRender,
			NULL);
	}else if(nPixelFormat == D3D_PIX_FMT_YUV420P){
		lRet=handle->pDirect3DDevice->CreateOffscreenPlainSurface(
			width,height,
			(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
			D3DPOOL_DEFAULT,
			&handle->pDirect3DSurfaceRender,
			NULL);
	}


	if(FAILED(lRet))
		goto end;

	handle->pixel_format = nPixelFormat;
	handle->width = width;
	handle->height = height;

	handle->init_flag = 1;

	return 0;

end:
	d3d_cleanup(handle);
	return -1;
}

	D3DXMATRIX WorldMatrix;
int __stdcall d3d_render(d3d_handle_t *handle, d3d_image_data_t *imageInfo)
{
	if(!handle || !imageInfo)
		return -1;
	
	if(handle->pDirect3DSurfaceRender == NULL)
		return -1;
	
	HRESULT			lRet;
	D3DLOCKED_RECT	d3d_rect;
	lRet=handle->pDirect3DSurfaceRender->LockRect(&d3d_rect,NULL,D3DLOCK_DONOTWAIT);
	if(FAILED(lRet))
		return -1;

	byte *pSrc = imageInfo->pBuffer[0];
	byte * pDest = (BYTE *)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;
	unsigned long i = 0;

	//Copy Data
	if(handle->pixel_format == D3D_PIX_FMT_BGR24)
	{
		int width_size= imageInfo->width*3;
		for(i=0; i< handle->height; i++){
			memcpy( pDest, pSrc, width_size );
			pDest += stride;
			pSrc += width_size;
		}
	}else if(handle->pixel_format == D3D_PIX_FMT_YUV420P){
		// Y
		for(i = 0;i < handle->height;i ++){
			memcpy(pDest + i * stride, imageInfo->pBuffer[0]+imageInfo->pitch[0]*i, handle->width);
		}
		// V
		for(i = 0;i < handle->height/2;i ++){
			memcpy(pDest + stride * handle->height + i * stride / 2, imageInfo->pBuffer[2]+imageInfo->pitch[2]*i, handle->width / 2);
		}
		// U
		for(i = 0;i < handle->height/2;i ++){
			memcpy(pDest + stride * handle->height + stride * handle->height / 4 + i * stride / 2, imageInfo->pBuffer[1]+imageInfo->pitch[1]*i, handle->width / 2);
		}
	}

	lRet=handle->pDirect3DSurfaceRender->UnlockRect();
	if(FAILED(lRet))
		return -1;

	if (handle->pDirect3DDevice == NULL)
		return -1;

	handle->pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	handle->pDirect3DDevice->BeginScene();
	IDirect3DSurface9 * pBackBuffer = NULL;
	handle->pDirect3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	handle->pDirect3DDevice->StretchRect(handle->pDirect3DSurfaceRender,NULL,pBackBuffer,&handle->m_rtViewport,D3DTEXF_LINEAR);
	//handle->pDirect3DDevice->EndScene();
#if 0	
	//handle->pDirect3DDevice->BeginScene();
    //绘制线
	handle->g_pLine->SetWidth(3.f);
	handle->g_pLine->SetAntialias(TRUE);
	handle->g_pLine->Draw(handle->g_pLineArr, 5, 0xffffffff);
	//handle->pDirect3DDevice->EndScene();

	//handle->pDirect3DDevice->BeginScene();
    //绘制线
	handle->g_pLine->SetWidth(3.f);
	handle->g_pLine->SetAntialias(TRUE);
	handle->g_pLine->Draw(handle->g_pLineArr2, 5, 0xffffffff);
#endif
	handle->pDirect3DDevice->EndScene();
	handle->pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
	

	return 0;
}
