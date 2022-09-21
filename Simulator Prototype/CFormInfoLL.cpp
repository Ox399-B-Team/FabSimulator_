// CFormInfoLL.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormInfoLL.h"


// CFormInfoLL 대화 상자

IMPLEMENT_DYNAMIC(CFormInfoLL, CDialogEx)

CFormInfoLL::CFormInfoLL(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TIMEINFO_LL, pParent)
	, m_strPumpTime(_T("-"))
	, m_strVentTime(_T("-"))
	, m_strSlotOpenTime(_T("-"))
	, m_strDoorOpenTime(_T("-"))
	, m_strPumpStableTime(_T("-"))
	, m_strVentStableTime(_T("-"))
	, m_strSlotCloseTime(_T("-"))
	, m_strDoorCloseTime(_T("-"))
{
	
}

CFormInfoLL::~CFormInfoLL()
{
}

void CFormInfoLL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_PUMPTIME_VALUE, m_strPumpTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_VENTTIME_VALUE, m_strVentTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_SLOTOPENTIME_VALUE, m_strSlotOpenTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_VALVEOPENTIME_VALUE, m_strDoorOpenTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_PUMPSTABLETIME_VALUE, m_strPumpStableTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_VENTSTABLETIME_VALUE, m_strVentStableTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_SLOTCLOSETIME_VALUE, m_strSlotCloseTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_VALVECLOSETIME_VALUE, m_strDoorCloseTime);
}


BEGIN_MESSAGE_MAP(CFormInfoLL, CDialogEx)
END_MESSAGE_MAP()


// CFormInfoLL 메시지 처리기


BOOL CFormInfoLL::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
