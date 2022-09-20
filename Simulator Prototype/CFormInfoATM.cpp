// CFormInfoATM.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormInfoATM.h"


// CFormInfoATM 대화 상자

IMPLEMENT_DYNAMIC(CFormInfoATM, CDialogEx)

CFormInfoATM::CFormInfoATM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TIMEINFO_ATM, pParent)
	, m_strPickTime(_T("-"))
	, m_strPlaceTime(_T("-"))
	, m_strRotateTime(_T("-"))
	, m_strZRotateTime(_T("-"))
{

}

CFormInfoATM::~CFormInfoATM()
{
}

void CFormInfoATM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UNIT_PICKTIME_VALUE, m_strPickTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_PLACETIME_VALUE, m_strPlaceTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_ATM_STATIONMOVE_VALUE, m_strRotateTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_ATM_ZMOVE_VALUE, m_strZRotateTime);
}


BEGIN_MESSAGE_MAP(CFormInfoATM, CDialogEx)
END_MESSAGE_MAP()


// CFormInfoATM 메시지 처리기


BOOL CFormInfoATM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
