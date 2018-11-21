
// H264ShowPlayDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CH264ShowPlayDlg 对话框
class CH264ShowPlayDlg : public CDialog
{
// 构造
public:
	CH264ShowPlayDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_H264SHOWPLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnH264End();
//	afx_msg void OnChangeH264ShowWindows();

	// 视频1的帧率
	CStatic m_video1_rate;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton11();
	int m_H264enable;
	int m_MJPEGenable;
	afx_msg void OnBnClickedButton12();
};
