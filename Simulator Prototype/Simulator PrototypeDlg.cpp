// Simulator PrototypeDlg.cpp: 구현 파일

#pragma once
#include "pch.h"
#include "framework.h"
#include "Simulator Prototype.h"
#include "Simulator PrototypeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ***************************************** 내가 추가한 헤더파일 *****************************************
#include "CSelCreateModuleDlg.h"
#include "CFabController.h"
#include "CSelUpdateModuleDlg.h"

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
// CSimulatorPrototypeDlg 대화 상자

CSimulatorPrototypeDlg::CSimulatorPrototypeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMULATOR_PROTOTYPE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimulatorPrototypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FAB_INFO, m_ctrlListFabInfo);
	DDX_Control(pDX, IDC_STATIC_FABTIME, m_ctrlFabTime);
	DDX_Control(pDX, IDC_PIC_JUSUNG, m_picLogo);
	DDX_Control(pDX, IDC_PIC_FAB, m_picFabLogo);
}

BEGIN_MESSAGE_MAP(CSimulatorPrototypeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LINECONTROL_RUN, &CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolRun)
	ON_BN_CLICKED(IDC_BUTTON_LINECONTROL_STOP, &CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolStop)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CSimulatorPrototypeDlg 메시지 처리기

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
	SetTimer(TIMER_CLOCK, 1000, NULL);
	
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

// Run 버튼
void CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolRun()
{
	CFabController::GetInstance().RunModules();
}

// Stop 버튼
void CSimulatorPrototypeDlg::OnBnClickedButtonLinecontrolStop()
{
	CFabController::GetInstance().SuspendModules();
}

// 배경색
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
		pDC->SetTextColor(RGB(0, 0, 0));

		CFont font;

		font.CreateFontW(
			40, // 글자높이
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
		
		CFont* Oldfont = (CFont*)pDC->SelectObject(&font);
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.


	return hbr;
}


void CSimulatorPrototypeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == TIMER_CLOCK)
	{
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);

		CString strTemp;
		strTemp.Format(_T("FAB Time : %02d:%02d:%02d"), curTime.wHour, curTime.wMinute, curTime.wSecond);
		m_ctrlFabTime.SetWindowText(strTemp);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CSimulatorPrototypeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//AfxMessageBox(_T("here"));
}
