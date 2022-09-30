﻿
// Simulator PrototypeDlg.h: 헤더 파일
//

#pragma once
#include "CFabInfoListCtrl.h"
#include "CPictureEx.h"
#include "CFormInfo.h"
#include "CFormInfoATM.h"
#include "CFormInfoLL.h"
#include "CFormInfoVAC.h"
#include "CFormInfoPM.h"

// CSimulatorPrototypeDlg 대화 상자

class CSimulatorPrototypeDlg : public CDialogEx
{
// 생성입니다.
public:
	CSimulatorPrototypeDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CSimulatorPrototypeDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMULATOR_PROTOTYPE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CFabInfoListCtrl m_ctrlListFabInfo;
	CStatic m_ctrlFabTime;
	CPictureEx m_picLogo;
	CPictureEx m_picFabLogo;
	
	// FabInfo 멤버
	CTabCtrl m_ctrlInfoTab;
	CFormInfo* m_pFormInfo;
	CFormInfoATM* m_pFormTimeInfoATM;
	CFormInfoLL* m_pFormTimeInfoLL;
	CFormInfoVAC* m_pFormTimeInfoVAC;
	CFormInfoPM* m_pFormTimeInfoPM;

	// 버튼 토글
	BOOL m_bIsRunning;
	
	// FabTime 멤버
	int m_nHour;
	int m_nMinute;
	int m_nSecond;
	int m_nDecisecond;

	afx_msg void OnBnClickedButtonLinecontrolRun();
	afx_msg void OnBnClickedButtonLinecontrolClear();
	afx_msg void OnBnClickedButtonSaveConfig();
	afx_msg void OnBnClickedButtonLoadConfig();
	afx_msg void OnBnClickedButtonSaveCsv();
	afx_msg void OnTcnSelchangeTabInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};