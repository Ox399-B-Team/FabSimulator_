﻿
// Simulator PrototypeDlg.h: 헤더 파일

//Thread에서 updateData 호출 시 에러를 위해 작성
#define UPDATE_MSG  WM_USER + 1

#pragma once
#include "CFabInfoListCtrl.h"
#include "CPictureEx.h"
#include "CFormInfo.h"
#include "CFormInfoATM.h"
#include "CFormInfoLL.h"
#include "CFormInfoVAC.h"
#include "CFormInfoPM.h"
#include "OScopeCtrl.h"
#include "CRecordDlg.h"
#include "CRecordData.h"

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
	
	// FabInfo
	CTabCtrl m_ctrlInfoTab;
	CFormInfo* m_pFormInfo;
	CFormInfoATM* m_pFormTimeInfoATM;
	CFormInfoLL* m_pFormTimeInfoLL;
	CFormInfoVAC* m_pFormTimeInfoVAC;
	CFormInfoPM* m_pFormTimeInfoPM;

	// 버튼 토글
	BOOL m_bIsRunning;

	// Time Control
	CRect m_rtGraph;
	int m_nCurSpeed;

	// FabTime
	int m_nHour;
	int m_nMinute;
	int m_nSecond;

	// 그래프
	COScopeCtrl* m_ctrlGraph;
	COScopeCtrl* m_ctrlGraphLPM;
	COScopeCtrl* m_ctrlGraphROBOT;
	COScopeCtrl* m_ctrlGraphLL;
	COScopeCtrl* m_ctrlGraphPM;
	bool m_bIsFullGraph;

	// 사이즈 조정
	CRect m_rtList;
	CRect m_rtBtnSize;
	bool m_bIsFullList;

	//Thread에서 updateData 호출 시 에러를 위해 작성
	afx_msg LRESULT OnReceivedMsgFromThread(WPARAM w, LPARAM l);
	afx_msg void OnBnClickedButtonLinecontrolRun();
	afx_msg void OnBnClickedButtonLinecontrolClear();
	afx_msg void OnBnClickedButtonLoadConfig();
	afx_msg void OnBnClickedButtonSaveConfig();
	afx_msg void OnBnClickedButtonLoadCsv();
	afx_msg void OnBnClickedButtonSaveCsv();
	afx_msg void OnTcnSelchangeTabInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadioSpeed1();
	afx_msg void OnBnClickedRadioSpeed2();
	afx_msg void OnBnClickedRadioSpeed3();
	afx_msg void OnBnClickedRadioSpeed4();
	afx_msg void OnBnClickedButtonChangegraph();
	afx_msg void OnBnClickedButtonChangesize();
	//기록을 저장하기 위한 변수 / 함수

	vector<CRecordData> m_VRecordData;
	CListCtrl m_ctrlRecord;
	afx_msg void OnNMDblclkListRecord(NMHDR* pNMHDR, LRESULT* pResult);
};



