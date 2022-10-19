// CFormInfoVAC.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormInfoVAC.h"


// CFormInfoVAC 대화 상자

IMPLEMENT_DYNAMIC(CFormInfoVAC, CDialogEx)

CFormInfoVAC::CFormInfoVAC(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TIMEINFO_VAC, pParent)
	, m_strPickTime(_T("-"))
	, m_strPlaceTime(_T("-"))
	, m_strRotateTime(_T("-"))
{

}

CFormInfoVAC::~CFormInfoVAC()
{
}

void CFormInfoVAC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UNIT_VAC_PICKTIME_VALUE, m_strPickTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_VAC_PLACETIME_VALUE, m_strPlaceTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_VAC_ROTATETIME_VALUE, m_strRotateTime);
}


BEGIN_MESSAGE_MAP(CFormInfoVAC, CDialogEx)
END_MESSAGE_MAP()


// CFormInfoVAC 메시지 처리기


BOOL CFormInfoVAC::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
