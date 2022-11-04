// CFormInfoPM.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CFormInfoPM.h"


// CFormInfoPM 대화 상자

IMPLEMENT_DYNAMIC(CFormInfoPM, CDialogEx)

CFormInfoPM::CFormInfoPM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TIMEINFO_PM, pParent)
	, m_strProcessTime(_T(""))
	, m_strProcessCount(_T(""))
	, m_strCleanTime(_T(""))
	, m_strCleanCount(_T(""))
	, m_strSlotOpenTime(_T(""))
	, m_strSlotCloseTime(_T(""))
{

}

CFormInfoPM::~CFormInfoPM()
{
}

void CFormInfoPM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_UNIT_PM_PROCESSTIME_VALUE, m_strProcessTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_LL_PUMPSTABLETIME_VALUE, m_strProcessCount);
	DDX_Text(pDX, IDC_STATIC_UNIT_PM_CLEANTIME_VALUE, m_strCleanTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_PM_CLEANCOUNT_VALUE, m_strCleanCount);
	DDX_Text(pDX, IDC_STATIC_UNIT_PM_SLOTOPENTIME_VALUE, m_strSlotOpenTime);
	DDX_Text(pDX, IDC_STATIC_UNIT_PM_SLOTCLOSETIME_VALUE, m_strSlotCloseTime);
}


BEGIN_MESSAGE_MAP(CFormInfoPM, CDialogEx)
END_MESSAGE_MAP()


// CFormInfoPM 메시지 처리기


BOOL CFormInfoPM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
