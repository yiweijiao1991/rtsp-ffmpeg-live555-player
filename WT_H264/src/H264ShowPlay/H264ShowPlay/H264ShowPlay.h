
// H264ShowPlay.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CH264ShowPlayApp:
// �йش����ʵ�֣������ H264ShowPlay.cpp
//

class CH264ShowPlayApp : public CWinAppEx
{
public:
	CH264ShowPlayApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CH264ShowPlayApp theApp;