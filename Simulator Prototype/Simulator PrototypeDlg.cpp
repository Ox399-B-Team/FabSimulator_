﻿// Simulator PrototypeDlg.cpp: 구현 파일

#pragma once
#include "pch.h"
#include "framework.h"
#include "Simulator Prototype.h"
#include "Simulator PrototypeDlg.h"
#include "afxdialogex.h"
#include "CSelCreateModuleDlg.h"
#include "CFabController.h"
#include "CSelUpdateModuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ***************************************** 내가 추가한 헤더파일 *****************************************

#pragma region AboutDlg 코드
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

#pragma endregion

#define TIMER_CLOCK 1
#define TIMER_GRAPH 2

// CSimulatorPrototypeDlg 대화 상자

CSimulatorPrototypeDlg::CSimulatorPrototypeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMULATOR_PROTOTYPE_DIALOG, pParent)
	, m_nCurSpeed(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_JUSUNG);
	m_nHour = 0;
	m_nMinute = 0;
	m_nSecond = 0;
	m_pFormInfo = NULL;
	m_pFormTimeInfoATM = NULL;
	m_pFormTimeInfoLL = NULL;
	m_pFormTimeInfoVAC = NULL;
	m_pFormTimeInfoPM = NULL;
	
	m_ctrlGraph = NULL;
	m_ctrlGraphLPM = NULL;
	m_ctrlGraphROBOT = NULL;
	m_ctrlGraphLL = NULL;
	m_ctrlGraphPM = NULL;

	m_bIsFullGraph = true;
	m_bIsFullList = false;

	m_bIsRunning = FALSE;
}

CSimulatorPrototypeDlg::~CSimulatorPrototypeDlg()
{
	delete m_pFormInfo;
	delete m_pFormTimeInfoATM;
	delete m_pFormTimeInfoLL;
	delete m_pFormTimeInfoVAC;
	delete m_pFormTimeInfoPM;
	delete m_ctrlGraphLPM;
	delete m_ctrlGraphROBOT;
	delete m_ctrlGraphLL;
	delete m_ctrlGraphPM;
	delete m_ctrlGraph;
}

void CSimulatorPrototypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FAB_INFO, m_ctrlListFabInfo);
	DDX_Control(pDX, IDC_STATIC_FABTIME, m_ctrlFabTime);
	DDX_Control(pDX, IDC_PIC_JUSUNG, m_picLogo);
	DDX_Control(pDX, IDC_PIC_FAB, m_picFabLogo);
	DDX_Control(pDX, IDC_TAB_INFO, m_ctrlInfoTab);
	DDX_Radio(pDX, IDC_RADIO_SPEED1, m_nCurSpeed);
	DDX_Control(pDX, IDC_LIST_RECORD, m_ctrlRecord);
}

BEGIN_MESSAGE_MAP(CSimulatorPrototypeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LINECONTROL_RUN, &CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolRun)
	ON_BN_CLICKED(IDC_BUTTON_LINECONTROL_CLEAR, &CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolClear)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CONFIG, &CSimulatorPrototypeDlg::OnBnClickedButtonSaveConfig)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CONFIG, &CSimulatorPrototypeDlg::OnBnClickedButtonLoadConfig)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CSV, &CSimulatorPrototypeDlg::OnBnClickedButtonSaveCsv)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_INFO, &CSimulatorPrototypeDlg::OnTcnSelchangeTabInfo)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_SPEED1, &CSimulatorPrototypeDlg::OnBnClickedRadioSpeed1)
	ON_BN_CLICKED(IDC_RADIO_SPEED2, &CSimulatorPrototypeDlg::OnBnClickedRadioSpeed2)
	ON_BN_CLICKED(IDC_RADIO_SPEED3, &CSimulatorPrototypeDlg::OnBnClickedRadioSpeed3)
	ON_BN_CLICKED(IDC_RADIO_SPEED4, &CSimulatorPrototypeDlg::OnBnClickedRadioSpeed4)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CSV, &CSimulatorPrototypeDlg::OnBnClickedButtonLoadCsv)
	ON_MESSAGE(UPDATE_MSG, &CSimulatorPrototypeDlg::OnReceivedMsgFromThread)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEGRAPH, &CSimulatorPrototypeDlg::OnBnClickedButtonChangegraph)
	ON_BN_CLICKED(IDC_BUTTON_CHANGESIZE, &CSimulatorPrototypeDlg::OnBnClickedButtonChangesize)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RECORD, &CSimulatorPrototypeDlg::OnNMDblclkListRecord)
END_MESSAGE_MAP()

// CSimulatorPrototypeDlg 메시지 처리기

// Thread에서 updateData 호출 시 에러를 위해 작성
LRESULT CSimulatorPrototypeDlg::OnReceivedMsgFromThread(WPARAM w, LPARAM l)
{
	UpdateData(FALSE);

	return 0;
}

BOOL CSimulatorPrototypeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// InfoTabCtrl에서 SHOW 될 Form생성 =======================================
	CRect rect;
	m_ctrlInfoTab.GetWindowRect(rect);
	
	m_ctrlInfoTab.InsertItem(0, _T("Informations"));
	m_ctrlInfoTab.InsertItem(1, _T("Time Parameters"));

	m_pFormInfo = new CFormInfo();		// 추후 delete 필요
	m_pFormInfo->Create(IDD_DIALOG_INFO, &m_ctrlInfoTab);
	m_pFormInfo->MoveWindow(1, 21, rect.Width(), rect.Height());
	m_pFormInfo->ShowWindow(SW_SHOW);

	m_pFormTimeInfoATM = new CFormInfoATM();
	m_pFormTimeInfoATM->Create(IDD_DIALOG_TIMEINFO_ATM, &m_ctrlInfoTab);
	m_pFormTimeInfoATM->MoveWindow(1, 21, rect.Width(), rect.Height());
	m_pFormTimeInfoATM->ShowWindow(SW_HIDE);

	m_pFormTimeInfoLL = new CFormInfoLL();
	m_pFormTimeInfoLL->Create(IDD_DIALOG_TIMEINFO_LL, &m_ctrlInfoTab);
	m_pFormTimeInfoLL->MoveWindow(1, 21, rect.Width(), rect.Height());
	m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
	
	m_pFormTimeInfoVAC = new CFormInfoVAC();
	m_pFormTimeInfoVAC->Create(IDD_DIALOG_TIMEINFO_VAC, &m_ctrlInfoTab);
	m_pFormTimeInfoVAC->MoveWindow(1, 21, rect.Width(), rect.Height());
	m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);

	m_pFormTimeInfoPM = new CFormInfoPM();
	m_pFormTimeInfoPM->Create(IDD_DIALOG_TIMEINFO_PM, &m_ctrlInfoTab);
	m_pFormTimeInfoPM->MoveWindow(1, 21, rect.Width(), rect.Height());
	m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

	m_ctrlInfoTab.SetCurSel(0);

	// 그래프 ================================================================
	GetDlgItem(IDC_STATIC_RT_GRAPH)->GetWindowRect(m_rtGraph);
	ScreenToClient(m_rtGraph);

	m_ctrlGraph = new COScopeCtrl();
	m_ctrlGraph->Create(WS_VISIBLE | WS_CHILD, m_rtGraph, this, IDC_STATIC_RT_GRAPH);

	int w = m_rtGraph.Width() / 2;
	int h = m_rtGraph.Height() / 2;

	m_ctrlGraphLPM = new COScopeCtrl;
	CRect rt1(CPoint(m_rtGraph.left, m_rtGraph.top), CSize(w, h));
	m_ctrlGraphLPM->Create(WS_CHILD, rt1, this, IDC_STATIC_RT_GRAPH);
	m_ctrlGraphLPM->ShowWindow(SW_HIDE);

	m_ctrlGraphROBOT = new COScopeCtrl;
	CRect rt2(CPoint(m_rtGraph.left + w, m_rtGraph.top), CSize(w, h));
	m_ctrlGraphROBOT->Create(WS_CHILD, rt2, this, IDC_STATIC_RT_GRAPH);
	m_ctrlGraphROBOT->ShowWindow(SW_HIDE);

	m_ctrlGraphLL = new COScopeCtrl;
	CRect rt3(CPoint(m_rtGraph.left, m_rtGraph.top + h), CSize(w, h));
	m_ctrlGraphLL->Create(WS_CHILD, rt3, this, IDC_STATIC_RT_GRAPH);
	m_ctrlGraphLL->ShowWindow(SW_HIDE);

	m_ctrlGraphPM = new COScopeCtrl;
	CRect rt4(CPoint(m_rtGraph.left + w, m_rtGraph.top + h), CSize(w, h));
	m_ctrlGraphPM->Create(WS_CHILD, rt4, this, IDC_STATIC_RT_GRAPH);
	m_ctrlGraphPM->ShowWindow(SW_HIDE);

	// 사이즈 조절 ===========================================================
	m_ctrlListFabInfo.GetWindowRect(m_rtList);
	ScreenToClient(m_rtList);

	GetDlgItem(IDC_BUTTON_CHANGESIZE)->GetWindowRect(m_rtBtnSize);
	ScreenToClient(m_rtBtnSize);

	// Timer 폰트 ================================================================
	static CFont font;
	font.CreateFont(
		35, // 글자높이
		0, // 글자너비
		0, // 출력각도
		0, // 기준 선에서의각도
		FW_HEAVY, // 글자굵기
		FALSE, // Italic 적용여부
		FALSE, // 밑줄적용여부
		FALSE, // 취소선적용여부
		DEFAULT_CHARSET, // 문자셋종류
		OUT_DEFAULT_PRECIS, // 출력정밀도
		CLIP_CHARACTER_PRECIS, // 클리핑정밀도
		PROOF_QUALITY, // 출력문자품질
		DEFAULT_PITCH, // 글꼴Pitch
		_T("Arial") // 글꼴
	);
	GetDlgItem(IDC_STATIC_FABTIME)->SetFont(&font, TRUE);

	// 기록 ListControl Record

	m_ctrlRecord.SetExtendedStyle(m_ctrlRecord.GetExtendedStyle() |
		LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ctrlRecord.ModifyStyle(0, LVS_SHOWSELALWAYS);

	m_ctrlRecord.InsertColumn(0, _T("RunTime"), LVCFMT_RIGHT, 100);
	m_ctrlRecord.InsertColumn(1, _T("Throughput"), LVCFMT_LEFT, 100);
	m_ctrlRecord.InsertColumn(2, _T("Total Modules Count"), LVCFMT_LEFT, 100);

	// ListControl 초기화
	m_ctrlListFabInfo.InitListCtrl();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CSimulatorPrototypeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
// 아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CSimulatorPrototypeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서 이 함수를 호출합니다.
HCURSOR CSimulatorPrototypeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Run 버튼 클릭 이벤트처리기
void CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolRun()
{
	CButton* pBtnRun = (CButton*)GetDlgItem(IDC_BUTTON_LINECONTROL_RUN);
	
	if (m_bIsRunning)
	{
		m_bIsRunning = FALSE;

		CFabController::GetInstance().SuspendModules();
		KillTimer(TIMER_CLOCK);
		KillTimer(TIMER_GRAPH);
		
		pBtnRun->SetWindowText(_T("RUN"));

		GetDlgItem(IDC_BUTTON_LOAD_CONFIG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVE_CONFIG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LOAD_CSV)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVE_CSV)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LINECONTROL_CLEAR)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SPEED1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SPEED2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SPEED3)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SPEED4)->EnableWindow(TRUE);
	}
	else
	{
		m_bIsRunning = TRUE;

		if (CFabController::GetInstance().RunModules() == false)
		{
			m_bIsRunning = FALSE;
			return;
		}
		
		SetTimer(TIMER_CLOCK, (1/ModuleBase::s_dSpeed), NULL);	// 타이머
		SetTimer(TIMER_GRAPH, 50, NULL);						// 그래프

		pBtnRun->SetWindowText(_T("STOP"));

		GetDlgItem(IDC_BUTTON_LOAD_CONFIG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVE_CONFIG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOAD_CSV)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVE_CSV)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LINECONTROL_CLEAR)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPEED1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPEED2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPEED3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPEED4)->EnableWindow(FALSE);
	}
}

// Clear 스레드
DWORD WINAPI OnBnClickedButtonLinecontrolClearWorkThread(LPVOID p)
{
	CFabController::GetInstance().ClearAllModule().join();

	return true;
}

// Clear 버튼 클릭 이벤트처리기
void CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolClear()
{
	if (IDYES == MessageBox(_T("모듈을 초기화하시겠습니까?"), _T("모듈 초기화"), MB_YESNO))
	{
		
		CloseHandle(CreateThread(NULL, NULL, OnBnClickedButtonLinecontrolClearWorkThread, this, NULL, NULL));

		CFabController::GetInstance().DeleteGraph();

		if ((int)CFabController::GetInstance().m_pModule.size() != 1)
		{
			int nRow = m_ctrlRecord.GetItemCount();
			CString strTemp = _T("");
			strTemp.Format(_T("%02d:%02d:%02d"), m_nHour, m_nMinute, m_nSecond);
			m_ctrlRecord.InsertItem(nRow, strTemp);

			strTemp.Format(_T("%.2lf"), ModuleBase::m_dTotalThroughput);
			m_ctrlRecord.SetItemText(nRow, 1, strTemp);
			strTemp.Format(_T("%d"), (int)CFabController::GetInstance().m_pModule.size());
			m_ctrlRecord.SetItemText(nRow, 2, strTemp);

			CTime cTime = CTime::GetCurrentTime();	// 현재 시간 가져옴
			CString strCurTime = cTime.Format(_T("%Y-%m-%d   %H:%M:%S "));

			strTemp.Format(_T("%02d:%02d:%02d"), m_nHour, m_nMinute, m_nSecond);

			CRecordData cRecordData;
			cRecordData.m_strCurtime = strCurTime;
			cRecordData.m_ihour = m_nHour;
			cRecordData.m_imin = m_nMinute;
			cRecordData.m_isecond = m_nSecond;
			cRecordData.m_itotalout = ModuleBase::s_nTotalOutputWafer;
			cRecordData.m_itotalin = ModuleBase::s_nTotalInputWafer;
			cRecordData.m_dtotalthroughput = ModuleBase::m_dTotalThroughput;

			int n = CFabController::GetInstance().m_pModule.size();
			for (int i = 0; i < n;i++)
			{
				CRecordDetail cRecordDetail;
				cRecordDetail.m_strMType = CFabController::GetInstance().m_pModule[i]->ConvertModuleType();
				cRecordDetail.m_strMName = CFabController::GetInstance().m_pModule[i]->GetModuleName();
				cRecordDetail.m_strWaferMax = CFabController::GetInstance().m_pModule[i]->ConvertWaferMax();
				CString temp;
				temp.Format(_T("%02d:%02d:%02d"), m_nHour, m_nMinute, m_nSecond);
				cRecordDetail.m_strRunTime = temp;
				cRecordDetail.m_nIWafer = CFabController::GetInstance().m_pModule[i]->m_nInputWafer;
				cRecordDetail.m_nOWafer = CFabController::GetInstance().m_pModule[i]->m_nOutputWafer;
				cRecordDetail.m_dThroughput = CFabController::GetInstance().m_pModule[i]->GetThroughput();

				cRecordData.m_VRecordDetail.push_back(cRecordDetail);
			}
			m_VRecordData.push_back(cRecordData);
		}
	}
}

// ConfigLoad 스레드
DWORD WINAPI OnBnClickedButtonLoadConfigWorkThread(LPVOID p)
{
	BOOL* pFlag = (BOOL*)p;

	if(*pFlag == TRUE)
		CFabController::GetInstance().ClearAllModule().join();

	CFileDialog fileDlg(TRUE, _T("cfg"), NULL, OFN_FILEMUSTEXIST, _T("CFG FILES(*.cfg)|*.cfg|All Files(*.*)|*.*||"));

	// CFileDialog 시작 경로 변경 (현재 프로그램의 작업 경로로 변경)
	TCHAR temp_path[MAX_PATH];						// 현재 작업 경로 저장을 위한 배열 선언
	GetCurrentDirectory(MAX_PATH, temp_path);		// 현재 프로그램 작업경로 저장
	fileDlg.m_ofn.lpstrInitialDir = temp_path;		// CFileDlg 초기 작업경로 변경

	if (fileDlg.DoModal() == IDOK)
	{
		CFabController::GetInstance().LoadConfigFile(fileDlg.GetPathName());
		MessageBox(NULL, _T("Config파일 Load 완료"), _T("Cfg Load"), MB_OK);
	}

	return true;
}

// ConfigLoad 버튼 클릭 이벤트처리기
void CSimulatorPrototypeDlg::OnBnClickedButtonLoadConfig()
{
	// Load 재확인 Dlg 호출
	if (CFabController::GetInstance().m_pModule.size() > 1)
	{
		if (IDYES == MessageBox(_T("Load 시 기존에 생성한 모듈 정보가 전부 삭제됩니다.\n진행하시겠습니까?"), _T("불러오기"), MB_YESNO))
		{
			CFabController::GetInstance().DeleteGraph();

			BOOL bFlag = true;
			CloseHandle(CreateThread(NULL, NULL, OnBnClickedButtonLoadConfigWorkThread, &bFlag, NULL, NULL));
		}
	}
	else
	{
		BOOL bFlag = false;
		CloseHandle(CreateThread(NULL, NULL, OnBnClickedButtonLoadConfigWorkThread, &bFlag, NULL, NULL));
	}
}

// ConfigSave 버튼 클릭 이벤트처리기
void CSimulatorPrototypeDlg::OnBnClickedButtonSaveConfig()
{
	CFileDialog fileDlg(FALSE, _T("cfg"), _T("Simulation"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("CFG FILES(*.cfg)|*.cfg|All Files(*.*)|*.*||"));

	// CFileDialog 시작 경로 변경 (현재 프로그램의 작업 경로로 변경)
	TCHAR temp_path[MAX_PATH];						// 현재 작업 경로 저장을 위한 배열 선언
	GetCurrentDirectory(MAX_PATH, temp_path);		// 현재 프로그램 작업경로 저장
	fileDlg.m_ofn.lpstrInitialDir = temp_path;		// CFileDlg 초기 작업경로 변경

	if (fileDlg.DoModal() == IDOK)
	{
		CFabController::GetInstance().SaveConfigFile(fileDlg.GetPathName());
	}
}

// #include <shellapi.h>
// Load CSV 버튼 클릭 이벤트처리기
void CSimulatorPrototypeDlg::OnBnClickedButtonLoadCsv()
{
	CFileDialog fileDlg(TRUE, _T("csv"), NULL, OFN_FILEMUSTEXIST, _T("CSV FILES(*.csv)|*.csv|All Files(*.*)|*.*||"));
	
	// CFileDialog 시작 경로 변경 (현재 프로그램의 작업 경로로 변경)
	TCHAR temp_path[MAX_PATH];						// 현재 작업 경로 저장을 위한 배열 선언
	GetCurrentDirectory(MAX_PATH, temp_path);		// 현재 프로그램 작업경로 저장
	fileDlg.m_ofn.lpstrInitialDir = temp_path;		// CFileDlg 초기 작업경로 변경

	if (fileDlg.DoModal() == IDOK)
	{
		ShellExecute(NULL, _T("open"), fileDlg.GetPathName(), NULL, NULL, SW_SHOW);
	}
}

// SaveCSV 버튼 클릭 이벤트처리기
void CSimulatorPrototypeDlg::OnBnClickedButtonSaveCsv()
{
	CFileDialog fileDlg(FALSE, _T("csv"), _T("Result"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("CSV FILES(*.csv)|*.csv|All Files(*.*)|*.*||"));

	// CFileDialog 시작 경로 변경 (현재 프로그램의 작업 경로로 변경)
	TCHAR temp_path[MAX_PATH];						// 현재 작업 경로 저장을 위한 배열 선언
	GetCurrentDirectory(MAX_PATH, temp_path);		// 현재 프로그램 작업경로 저장
	fileDlg.m_ofn.lpstrInitialDir = temp_path;		// CFileDlg 초기 작업경로 변경

	if (fileDlg.DoModal() == IDOK)
	{
		CFabController::GetInstance().SaveCSVFile(fileDlg.GetPathName());
	}
}

// InfoTab Index 변경 이벤트처리기
void CSimulatorPrototypeDlg::OnTcnSelchangeTabInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nModuleIdx;
	int nModuleType = CFabController::GetInstance().SelectModule(m_ctrlListFabInfo.m_nCurRow, m_ctrlListFabInfo.m_nCurCol, nModuleIdx);
	CFabController::GetInstance().PrintModuleInfo(nModuleIdx, nModuleType, m_ctrlInfoTab.GetCurSel());

	*pResult = 0;
}

// OnTimer 이벤트처리기
void CSimulatorPrototypeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_CLOCK)
	{
		m_nSecond++;
		if (m_nMinute >= 60)
		{
			m_nHour = m_nHour + (m_nMinute / 60);
			m_nMinute = m_nMinute % 60;
		}
		if (m_nSecond >= 60)
		{
			m_nMinute = m_nMinute + (m_nSecond / 60);
			m_nSecond = m_nSecond % 60;
		}

		CString strTemp;
		strTemp.Format(_T("FAB Time %02d:%02d:%02d"), m_nHour, m_nMinute, m_nSecond);
		m_ctrlFabTime.SetWindowText(strTemp);

		CFabController::GetInstance().SetFabInfo(m_nHour, m_nMinute, m_nSecond);

		int nCurModuleIdx;
		CFabController::GetInstance().SelectModule(m_ctrlListFabInfo.m_nCurRow, m_ctrlListFabInfo.m_nCurCol, nCurModuleIdx);
		CFabController::GetInstance().SetUnitInfo(nCurModuleIdx);
	}
	else if (nIDEvent == TIMER_GRAPH)
	{	
		// Graph 표현
		int size = (int)CFabController::GetInstance().m_pModule.size() + 1;
		double* pValue = new double[size];

		pValue[0] = ModuleBase::m_dTotalThroughput;

		for (int i = 1; i < size; i++)
		{
			pValue[i] = CFabController::GetInstance().m_pModule[i - 1]->GetThroughput();
		}
		m_ctrlGraph->AppendPoints(pValue);

		delete[] pValue;

		// LPM
		size = (int)LPM::s_pLPM.size() + 1;
		pValue = new double[size];

		pValue[0] = ModuleBase::m_dTotalThroughput;

		for (int i = 1; i < size; i++)
		{
			pValue[i] = LPM::s_pLPM[i - 1]->GetThroughput();
		}

		m_ctrlGraphLPM->AppendPoints(pValue);
		delete[] pValue;

		// ROBOT
		size = 3;
		pValue = new double[size];

		pValue[0] = ModuleBase::m_dTotalThroughput;
		pValue[1] = ATMRobot::s_pATMRobot[0]->GetThroughput();
		pValue[2] = VACRobot::s_pVACRobot[0]->GetThroughput();

		m_ctrlGraphROBOT->AppendPoints(pValue);
		delete[] pValue;

		// LL
		size = (int)LoadLock::s_pLL.size() + 1;
		pValue = new double[size];

		pValue[0] = ModuleBase::m_dTotalThroughput;

		for (int i = 1; i < size; i++)
		{
			pValue[i] = LoadLock::s_pLL[i - 1]->GetThroughput();
		}

		m_ctrlGraphLL->AppendPoints(pValue);
		delete[] pValue;

		// PM
		size = (int)ProcessChamber::s_pPM.size() + 1;
		pValue = new double[size];

		pValue[0] = ModuleBase::m_dTotalThroughput;

		for (int i = 1; i < size; i++)
		{
			pValue[i] = ProcessChamber::s_pPM[i - 1]->GetThroughput();
		}

		m_ctrlGraphPM->AppendPoints(pValue);
		delete[] pValue;
	}

	// 시간 가속 기능 구현 시 추가?

	CDialogEx::OnTimer(nIDEvent);
}

// 배경색 (추후 UI 꾸밀 시 작업)
BOOL CSimulatorPrototypeDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//CRect rect;
	//GetClientRect(&rect);
	//
	//CBrush myBrush(RGB(245, 245, 245));
	//CBrush* pOld = pDC->SelectObject(&myBrush);
	//BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	//pDC->SelectObject(pOld);
	//pDC->FillSolidRect(rect, RGB(0, 0, 0));
	//return bRes;

	// 기본 색상
	return CDialogEx::OnEraseBkgnd(pDC);
}

// 폰트
HBRUSH CSimulatorPrototypeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_FABTIME)
	{
		pDC->SetTextColor(RGB(107, 116, 125));
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

// Speed 설정
void CSimulatorPrototypeDlg::OnBnClickedRadioSpeed1()
{
	UpdateData(1);

	CFabController::GetInstance().ChangeTimeSpeed(m_nCurSpeed);
}
void CSimulatorPrototypeDlg::OnBnClickedRadioSpeed2()
{
	UpdateData(1);

	CFabController::GetInstance().ChangeTimeSpeed(m_nCurSpeed);
}
void CSimulatorPrototypeDlg::OnBnClickedRadioSpeed3()
{
	UpdateData(1);

	CFabController::GetInstance().ChangeTimeSpeed(m_nCurSpeed);
}
void CSimulatorPrototypeDlg::OnBnClickedRadioSpeed4()
{
	UpdateData(1);

	CFabController::GetInstance().ChangeTimeSpeed(m_nCurSpeed);
}

// 그래프 변경
void CSimulatorPrototypeDlg::OnBnClickedButtonChangegraph()
{
	m_bIsFullGraph = !m_bIsFullGraph;

	CFabController::GetInstance().ChangeGraph(m_bIsFullGraph);
}

// 리스트컨트롤 / 그래프 사이즈 변경 버튼
void CSimulatorPrototypeDlg::OnBnClickedButtonChangesize()
{
	CRect rtCurRect;
	if (m_bIsFullList)
	{
		rtCurRect = m_rtList;
		rtCurRect.right = m_rtList.right;

		GetDlgItem(IDC_BUTTON_CHANGESIZE)->MoveWindow(m_rtList.right + 1, m_rtBtnSize.top, m_rtBtnSize.right - m_rtBtnSize.left, m_rtBtnSize.bottom - m_rtBtnSize.top);
		CFabController::GetInstance().ChangeGraph(m_bIsFullGraph);
		m_ctrlListFabInfo.MoveWindow(rtCurRect);

		GetDlgItem(IDC_BUTTON_CHANGEGRAPH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CHANGESIZE)->SetWindowText(_T("▶"));

		m_bIsFullList = !m_bIsFullList;
	}
	else
	{
		rtCurRect = m_rtList;
		rtCurRect.right = m_rtGraph.right;

		GetDlgItem(IDC_BUTTON_CHANGESIZE)->MoveWindow(m_rtGraph.right + 1, m_rtBtnSize.top, m_rtBtnSize.right - m_rtBtnSize.left, m_rtBtnSize.bottom - m_rtBtnSize.top);

		m_ctrlGraph->ShowWindow(HIDE_WINDOW);
		m_ctrlGraphLPM->ShowWindow(HIDE_WINDOW);
		m_ctrlGraphROBOT->ShowWindow(HIDE_WINDOW);
		m_ctrlGraphLL->ShowWindow(HIDE_WINDOW);
		m_ctrlGraphPM->ShowWindow(HIDE_WINDOW);
		GetDlgItem(IDC_STATIC_RT_GRAPH)->ShowWindow(HIDE_WINDOW);

		GetDlgItem(IDC_BUTTON_CHANGEGRAPH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CHANGESIZE)->SetWindowText(_T("◀"));

		m_ctrlListFabInfo.MoveWindow(rtCurRect);

		m_bIsFullList = !m_bIsFullList;
	}	
}


void CSimulatorPrototypeDlg::OnNMDblclkListRecord(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int idx = pNMItemActivate->iItem;
	if (idx != -1)
	{
		CString strTemp1, strTemp2, strTemp3, strTemp4;
		strTemp1.Format(_T("%02d:%02d:%02d"), m_VRecordData.at(idx).m_ihour,
												 m_VRecordData.at(idx).m_imin,
												 m_VRecordData.at(idx).m_isecond);
		strTemp2.Format(_T("%d"), m_VRecordData.at(idx).m_itotalout);
		strTemp3.Format(_T("%d"), m_VRecordData.at(idx).m_itotalin);
		strTemp4.Format(_T("%d"), m_VRecordData.at(idx).m_dtotalthroughput);
		CRecordDlg* dlg = new CRecordDlg(m_VRecordData.at(idx).m_strCurtime,
										strTemp1,
										strTemp2,
										strTemp3,
										strTemp4);
		
		dlg->Create(IDD_DIALOG_RECORD);
		
		for (int j = 0; j < m_VRecordData.at(idx).m_VRecordDetail.size(); j++)
		{
			int nRow = dlg->m_ctrlLayout.GetItemCount();
			int nCol = 1;
			
			dlg->m_ctrlLayout.InsertItem(nRow, m_VRecordData.at(idx).m_VRecordDetail.at(j).m_strMType);
			dlg->m_ctrlLayout.SetItemText(nRow, nCol++, m_VRecordData.at(idx).m_VRecordDetail.at(j).m_strMName);
			dlg->m_ctrlLayout.SetItemText(nRow, nCol++, m_VRecordData.at(idx).m_VRecordDetail.at(j).m_strWaferMax);
			dlg->m_ctrlLayout.SetItemText(nRow, nCol++, m_VRecordData.at(idx).m_VRecordDetail.at(j).m_strRunTime);

			CString temp = _T("");
			temp.Format(_T("%d"), m_VRecordData.at(idx).m_VRecordDetail.at(j).m_nIWafer);
			dlg->m_ctrlLayout.SetItemText(nRow, nCol++, temp);

			temp.Format(_T("%d"), m_VRecordData.at(idx).m_VRecordDetail.at(j).m_nOWafer);
			dlg->m_ctrlLayout.SetItemText(nRow, nCol++, temp);

			temp.Format(_T("%.2lf"), m_VRecordData.at(idx).m_VRecordDetail.at(j).m_dThroughput);
			dlg->m_ctrlLayout.SetItemText(nRow, nCol, temp);
		}
		dlg->ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}

