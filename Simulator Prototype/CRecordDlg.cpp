// CRecordDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Simulator Prototype.h"
#include "afxdialogex.h"
#include "CRecordDlg.h"


// CRecordDlg 대화 상자

IMPLEMENT_DYNAMIC(CRecordDlg, CDialogEx)

CRecordDlg::CRecordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RECORD, pParent)
	, m_strCreatedDate(_T(""))
	, m_strTotalOutput(_T(""))
	, m_strRuntime(_T(""))
	, m_strTotalInput(_T(""))
	, m_strThroughput(_T(""))
{

}

CRecordDlg::CRecordDlg(CString Created_date, CString Total_Runtime, CString Total_Output, CString Total_Input, CString Total_Throughput)
	: m_strCreatedDate(Created_date)
	, m_strRuntime(Total_Runtime)
	, m_strTotalOutput(Total_Output)
	, m_strTotalInput(Total_Input)
	, m_strThroughput(Total_Throughput)
{

}

CRecordDlg::~CRecordDlg()
{
	
}

void CRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_CREATED_DATE, m_strCreatedDate);
	DDX_Text(pDX, IDC_STATIC_TOTAL_OUTPUT, m_strTotalOutput);
	DDX_Text(pDX, IDC_STATIC_TOTAL_RUNTIME, m_strRuntime);
	DDX_Text(pDX, IDC_STATIC_TOTAL_INPUT, m_strTotalInput);
	DDX_Control(pDX, IDC_LIST_LAYOUT, m_ctrlLayout);
	DDX_Text(pDX, IDC_STATIC_THROUGHPUT, m_strThroughput);
}


BEGIN_MESSAGE_MAP(CRecordDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRecordDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SAVERESULT, &CRecordDlg::OnBnClickedButtonSaveresult)
END_MESSAGE_MAP()


// CRecordDlg 메시지 처리기


BOOL CRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlLayout.SetExtendedStyle(m_ctrlLayout.GetExtendedStyle() |
		LVS_EX_GRIDLINES| LVS_EX_FULLROWSELECT);
	m_ctrlLayout.ModifyStyle(0, LVS_SHOWSELALWAYS);

	m_ctrlLayout.InsertColumn(0, _T("ModuleType"), LVCFMT_LEFT, 100);
	m_ctrlLayout.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 70);
	m_ctrlLayout.InsertColumn(2, _T("WaferMax"), LVCFMT_LEFT, 65);
	m_ctrlLayout.InsertColumn(3, _T("TotalRunTime"), LVCFMT_LEFT, 100);
	m_ctrlLayout.InsertColumn(4, _T("InputWaferCount"), LVCFMT_LEFT, 105);
	m_ctrlLayout.InsertColumn(5, _T("OutputWaferCount"), LVCFMT_LEFT, 105);
	m_ctrlLayout.InsertColumn(6, _T("Throughput(Wafer/Hr)"), LVCFMT_LEFT, 130);

	UpdateData(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CRecordDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDialogEx::DestroyWindow();
}


void CRecordDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CRecordDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialogEx::OnClose();
}


void CRecordDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}


void CRecordDlg::OnBnClickedButtonSaveresult()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}
