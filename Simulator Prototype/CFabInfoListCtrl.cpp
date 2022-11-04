#pragma once
#include "pch.h"
#include "resource.h"
#include "CFabInfoListCtrl.h"
#include "CSelUpdateModuleDlg.h"
#include "CSelCreateModuleDlg.h"
#include "Simulator PrototypeDlg.h"

//////////�ӽ� ���///////////
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "CFabController.h"
/////////////////////////////

#define GREEN RGB(127, 185, 2)
#define ORANGE RGB(255, 152, 49) //(242, 79, 36)
#define BLUE RGB(0, 180, 242)
#define YELLOW RGB(255, 255, 0) //(255, 195, 0)
#define WHITE RGB(255, 255, 255)
#define BLACK RGB(0, 0, 0)
#define GRAY RGB(240, 240, 240)

#pragma region ������/�Ҹ���
CFabInfoListCtrl::CFabInfoListCtrl()
{
	m_nCurRow = -1;
	m_nCurCol = -1;

	for (int i = 0; i < 14; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			m_arrIsAvilable[i][j] = true;
}
	}

	// ���� ����/�Ұ��� �߰� (������ ����)
	//for (int nRow = 0; nRow < 6; nRow++)
	//{
	//	for (int nCol = 0; nCol < 2; nCol++)
	//	{
	//		m_arrIsAvilable[nRow][nCol] = false;
	//	}
	//}
	
	// ��� �ִ� ���� ���� =============================================
	
	//m_arrIsAvilable[0][2] = false;
	//m_arrIsAvilable[0][3] = false;
	//m_arrIsAvilable[0][4] = false;
	//m_arrIsAvilable[5][2] = false;

	//m_arrIsAvilable[0][3] = false;
	//m_arrIsAvilable[1][3] = false;
	//m_arrIsAvilable[2][3] = false;
	//m_arrIsAvilable[4][3] = false;
	//m_arrIsAvilable[5][3] = false;

	//m_arrIsAvilable[0][4] = false;
	//m_arrIsAvilable[5][4] = false;

	//m_arrIsAvilable[0][5] = false;
	//m_arrIsAvilable[1][5] = false;
	//m_arrIsAvilable[2][5] = false;
	//m_arrIsAvilable[4][5] = false;
	//m_arrIsAvilable[5][5] = false;

	// ================================================================

}

CFabInfoListCtrl::~CFabInfoListCtrl()
{

}
#pragma endregion

#pragma region �޼���
BOOL CFabInfoListCtrl::InitListCtrl()
{
	ModifyStyle(LVS_OWNERDRAWFIXED, 0, 0);
	SetExtendedStyle(LVS_EX_GRIDLINES);
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_SIMPLESELECT | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	
	//int nMaxCell = 7;
	
	// ����Ʈ��Ʈ�� Item �� ���� ����
	CImageList cImagelist;
	cImagelist.Create(1, 70, ILC_COLORDDB, 1, 0);
	SetImageList(&cImagelist, LVSIL_SMALL);

	CString strColumn;

	// �÷�
	InsertColumn(0, _T("-"), LVCFMT_CENTER, 52, 0);
	InsertColumn(1, _T("IN"), LVCFMT_CENTER, 83, 0);
	InsertColumn(2, _T("LPM"), LVCFMT_CENTER, 83, 0);
	InsertColumn(3, _T("ATMRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(4, _T("LoadLock"), LVCFMT_CENTER, 83, 0);
	InsertColumn(5, _T("VACRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(6, _T("PM"), LVCFMT_CENTER, 83, 0);
	InsertColumn(7, _T("VACRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(8, _T("LoadLock"), LVCFMT_CENTER, 83, 0);
	InsertColumn(9, _T("ATMRobot"), LVCFMT_CENTER, 83, 0);
	InsertColumn(10, _T("LPM"), LVCFMT_CENTER, 83, 0);
	InsertColumn(11, _T("OUT"), LVCFMT_CENTER, 83, 0);
	
	// �ο�
	for (int i = 1; i < 7; i++)
	{
		strColumn.Format(_T("%d"), i);
		InsertItem(i, strColumn, 0);
	}

	SetBkColor(GRAY);

	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("DummyStage"), 0, 12, 0, 2));
	
	CFabController::GetInstance().DrawModule();

	return TRUE;
}

#pragma endregion

BEGIN_MESSAGE_MAP(CFabInfoListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CFabInfoListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CFabInfoListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CFabInfoListCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CFabInfoListCtrl::OnNMRClick)
	ON_COMMAND(ID_MENU_CHANGEMODULE, &CFabInfoListCtrl::OnMenuChangemodule)
	ON_COMMAND(ID_MENU_DELETEMODULE, &CFabInfoListCtrl::OnMenuDeletemodule)
END_MESSAGE_MAP()

#pragma region ListCtrl

/* ���� ����
* pDraw->clrTextBk = RGB(127, 185, 2); ����
* pDraw->clrTextBk = RGB(242, 79, 36); ��Ȳ
* pDraw->clrTextBk = RGB(0, 180, 242); �Ķ�
* pDraw->clrTextBk = RGB(255, 195, 0); ���
*/

// ����Ʈ��Ʈ�� CustomDraw
void CFabInfoListCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
	
	if (pNMCD->dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // ���⼭ �Լ��� ���� ������ *pResult ���� �����ȴ�.
	}
	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;
		return;
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT))
	{
		// sub-item�� �׷����� ���� ������ *pResult �� CDRF_NOTIFYSUBITEMDRAW �� �ؼ� ����� ��
		CString text = GetItemText(pNMCD->dwItemSpec, pDraw->iSubItem);

		if (m_arrIsAvilable[pNMCD->dwItemSpec][pDraw->iSubItem] == false)
		{
			pDraw->clrText = BLACK;
			pDraw->clrTextBk = GRAY;
		}
		else if ((m_nCurRow == pNMCD->dwItemSpec) && (m_nCurCol == pDraw->iSubItem) && 
			m_arrIsAvilable[pNMCD->dwItemSpec][pDraw->iSubItem] == true)	// ���� Ŭ���� Sel�� �� �� ����
		{
			pDraw->clrText = BLACK;
			pDraw->clrTextBk = YELLOW;
		}
		else if (!text.IsEmpty() && pDraw->iSubItem > 1)		// Sel�� ����� ������ ��
		{
			pDraw->clrText = WHITE;
			pDraw->clrTextBk = GREEN;

			//-----------------------------
			vector<ModuleBase*> pModule = CFabController::GetInstance().m_pModule;

			if (ModuleBase::s_bDirect == true)
				pDraw->clrTextBk = ORANGE;//��Ȳ��-������
			else
				pDraw->clrTextBk = GREEN;//���λ�-������
		}
		else
		{
			pDraw->clrText = BLACK;
			pDraw->clrTextBk = WHITE;
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // �̷��� �ؾ� ������ ��� �׷�����.
		return;
	}

	*pResult = 0;
}

// ���콺 L Ŭ��
void CFabInfoListCtrl::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{	
	// ���� Row�� SetItemState ȣ�� (Row�� �ٽ� NMCustomDraw ȣ��?)
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// ���� Row, Col ����
	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	// ����Ʈ ��Ʈ�� ��ü�� NMCustomDraw ȣ��?
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_arrIsAvilable[m_nCurRow][m_nCurCol] == false) return;
	//if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty()) return;
	
	// ListCtrl���� Select �� ��� ���� �ҷ�����
	int nModuleIdx;
	int nModuleType = CFabController::GetInstance().SelectModule(m_nCurRow, m_nCurCol, nModuleIdx);

	// Type �� UpdateData ȣ��
	int nCurSel = ((CSimulatorPrototypeDlg*)AfxGetMainWnd())->m_ctrlInfoTab.GetCurSel();
	CFabController::GetInstance().PrintModuleInfo(nModuleIdx, nModuleType, nCurSel);

	*pResult = 0;
}

// ���콺 DB Ŭ��
void CFabInfoListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Run �� DBClick ����
	if (((CSimulatorPrototypeDlg*)GetParent())->m_bIsRunning == true) return;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_arrIsAvilable[m_nCurRow][m_nCurCol] == false)	return;
	if (!(GetItemText(m_nCurRow, m_nCurCol).IsEmpty())) return;	// �� ����ִ��� �Ǵ��ؼ� ���� �����Ͱ� ���� ���� (����� �� ��������) �Լ�����
	if (CFabController::GetInstance().m_pModule[0]->IsRunning()) return;
	
	// CreateDlg ����
	CSelCreateModuleDlg dlg(this, m_nCurRow, m_nCurCol);
	::GetCursorPos(&dlg.m_ptPos);

	int nSelOption = (int)dlg.DoModal();
	*pResult = 0;
}

// ���콺 R Ŭ��
void CFabInfoListCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Run �� RClick ����
	if (((CSimulatorPrototypeDlg*)GetParent())->m_bIsRunning == true) return;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// ����Ʈ��Ʈ�� Ŭ���� ��������� Row, Col idx��������
	
	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_arrIsAvilable[m_nCurRow][m_nCurCol] == false) return;		
	if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty()) return;
	if (CFabController::GetInstance().m_pModule[0]->IsRunning()) return;


	// �˾��޴� ===================================================================	
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_MODULE);
	CMenu* pSubMenu = popMenu.GetSubMenu(0);
	CPoint pContext;
	GetCursorPos(&pContext);
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pContext.x, pContext.y, this);

	*pResult = 0;
}

#pragma endregion

#pragma region Menu

// ��� �Ӽ����� ����޴� ����
void CFabInfoListCtrl::OnMenuChangemodule()
{
	CSelUpdateModuleDlg dlg(this, m_nCurRow, m_nCurCol);
	::GetCursorPos(&dlg.m_ptPos);
	
	int nSelOption = (int)dlg.DoModal();
}

// ��� ���� ����޴� ����
void CFabInfoListCtrl::OnMenuDeletemodule()
{
	// ���� ��Ȯ�� Dlg ĸ�� ������ ���� �Ͻ������� App�� m_pszAppName ����
	LPCTSTR pAppNameTemp = AfxGetApp()->m_pszAppName;
	AfxGetApp()->m_pszAppName = _T("��� ����");

	// ���� ��Ȯ�� Dlg ȣ��..
	if (IDYES == MessageBox(_T("����� �����Ͻðڽ��ϱ�?"), _T("��� ����"), MB_YESNO))
	{
		// ��Ʈ�ѷ��� vector<>�� ���� �� �ν��Ͻ� ����..
		int nModuleIdx;
		CFabController::GetInstance().SelectModule(m_nCurRow, m_nCurCol, nModuleIdx);
		CFabController::GetInstance().DeleteModule(this, nModuleIdx);
	}

	// m_pszAppName �ǵ�������
	AfxGetApp()->m_pszAppName = pAppNameTemp;
}

#pragma endregion