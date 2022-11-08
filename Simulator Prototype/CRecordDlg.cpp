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
	CFileDialog FileDlg(FALSE, _T("csv"), _T("Result"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("CSV FILES(*.csv)|*.csv|All Files(*.*)|*.*||"));

	// CFileDialog 시작 경로 변경 (현재 프로그램의 작업 경로로 변경)
	TCHAR Temp_Path[MAX_PATH];						// 현재 작업 경로 저장을 위한 배열 선언
	GetCurrentDirectory(MAX_PATH, Temp_Path);		// 현재 프로그램 작업경로 저장
	FileDlg.m_ofn.lpstrInitialDir = Temp_Path;		// CFileDlg 초기 작업경로 변경

	if (FileDlg.DoModal() == IDOK)
	{
		CStdioFile cfile;	// Text 모드 저장 시 편의성을 위해 CFile 자식 클래스인 CStdioFile 사용 결정
		CFileException cException;	// 실패한 작업의 상태를 수신할 기존 파일 예외 개체에 대한 포인터

		// CStdioFile Open 실패 시 예외처리 (ex에 오류 상태를 수신 후 출력)
		if (!cfile.Open(FileDlg.GetPathName(), CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate, &cException))
		{
			TCHAR szError[1024];
			
			cException.GetErrorMessage(szError, 1024);
			_tprintf_s(_T("파일 열기 불가 : %1024s"), szError);
			AfxMessageBox(szError);

			return;
		}

		CString strTemp;
		strTemp.Format(_T(",\n") + m_strCreatedDate + _T("JUSUNG FAB SIMULATOR - RESULT DATA,\n\n"));
		cfile.WriteString(strTemp);

		strTemp.Format(_T("/////////////////////////////////////////////////////////////////////////////////////////,\n\n"));
		cfile.WriteString(strTemp);

		strTemp.Format(_T("Fab Total Run Time : %s, \n"), m_strRuntime);
		cfile.WriteString(strTemp);

		strTemp.Format(_T("Fab Total Output Cell Count : %s Cells,\n"), m_strTotalOutput);
		cfile.WriteString(strTemp);

		strTemp.Format(_T("Fab Total Input Cell Count : %s Cells,\n"), m_strTotalInput);
		cfile.WriteString(strTemp);

		strTemp.Format(_T("Fab Throughput : %s Wafer/Hr, \n\n"), m_strThroughput);
		cfile.WriteString(strTemp);

		strTemp.Format(_T("/////////////////////////////////////////////////////////////////////////////////////////,\n\n\n"));
		cfile.WriteString(strTemp);

		strTemp.Format(_T("Idx, ModuleType, Name, WaferMax, TotalRunTime, InputCellCount, OutputCellCount, Thruput(Wafer/Hr),\n"));
		cfile.WriteString(strTemp);

		for (int i = 0; i < m_ctrlLayout.GetItemCount();i++)
		{
			int j = 0;
			
			strTemp.Format(_T("%d, "),i);
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j++) + _T(",  "));
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j++) + _T(",  "));
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j++) + _T(",  "));
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j++) + _T(",  "));
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j++) + _T(",  "));
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j++) + _T(",  "));
			cfile.WriteString(strTemp);
			strTemp.Format(m_ctrlLayout.GetItemText(i, j) + _T("\n"));
			cfile.WriteString(strTemp);
			
		}
		cfile.SeekToEnd();
		cfile.Close();

		AfxMessageBox(_T("저장이 완료되었습니다."));
	}
}
