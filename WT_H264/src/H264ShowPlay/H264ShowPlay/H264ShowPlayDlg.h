
// H264ShowPlayDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CH264ShowPlayDlg �Ի���
class CH264ShowPlayDlg : public CDialog
{
// ����
public:
	CH264ShowPlayDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_H264SHOWPLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnH264End();
//	afx_msg void OnChangeH264ShowWindows();

	// ��Ƶ1��֡��
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
