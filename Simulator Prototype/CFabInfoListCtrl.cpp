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

#pragma region ������/�Ҹ���
CFabInfoListCtrl::CFabInfoListCtrl()
{
	m_nCurRow = -1;
	m_nCurCol = -1;
	m_strModuleName = _T("");
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
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_SIMPLESELECT | LVS_EX_FULLROWSELECT);
	
	int nMaxCell = 21;
	
	// ����Ʈ��Ʈ�� Item �� ���� ����
	CImageList cImagelist;
	cImagelist.Create(1, 68, ILC_COLORDDB, 1, 0);
	SetImageList(&cImagelist, LVSIL_SMALL);

	CString strColumn;

	// �÷�
	for (int i = 0; i < nMaxCell; i++)
	{
		strColumn.Format(_T("%d"), i);
		InsertColumn(i+1, strColumn, LVCFMT_CENTER, 68, 0);
	}

	// �ο�
	for (int i = 1; i < 7; i++)
	{
		strColumn.Format(_T("%d"), i);
		InsertItem(i, strColumn, 0);
	}

	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("LPM01"), 25, 25, 2, 1));
	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("LPM02"), 25, 25, 3, 1));
	CFabController::GetInstance().m_pModule.push_back(new LPM(TYPE_LPM, _T("LPM03"), 25, 25, 4, 1));
	CFabController::GetInstance().m_pModule.push_back(new ATMRobot(TYPE_ATMROBOT, _T("ATMRobot"), 0, 2, 3, 2, 4, 4, 1, 1));
	CFabController::GetInstance().m_pModule.push_back(new LoadLock(TYPE_LOADLOCK, _T("LL1"), 0, 6, 2, 3, 15, 5, 15, 5, 2, 2, 2, 2));
	CFabController::GetInstance().m_pModule.push_back(new LoadLock(TYPE_LOADLOCK, _T("LL2"), 0, 6, 4, 3, 15, 5, 15, 5, 2, 2, 2, 2));
	CFabController::GetInstance().m_pModule.push_back(new VACRobot(TYPE_VACROBOT, _T("TM"), 0, 4, 3, 4, 4, 4, 4));
	CFabController::GetInstance().m_pModule.push_back(new ProcessChamber(TYPE_PROCESSCHAMBER, _T("PM01"), 0, 6, 2, 5, 600, 1200, 2, 2, 2));
	CFabController::GetInstance().m_pModule.push_back(new ProcessChamber(TYPE_PROCESSCHAMBER, _T("PM02"), 0, 6, 4, 5, 600, 1200, 2, 2, 2));

	for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
	{
		ModuleBase* pM = CFabController::GetInstance().m_pModule[i];

		if (pM->m_eModuleType == TYPE_LPM)
			CFabController::s_pLPM.push_back(pM);
		else if (pM->m_eModuleType == TYPE_ATMROBOT)
			CFabController::s_pATMRobot.push_back(pM);
		else if (pM->m_eModuleType == TYPE_LOADLOCK)
			CFabController::s_pLL.push_back(pM);
		else if (pM->m_eModuleType == TYPE_VACROBOT)
			CFabController::s_pVACRobot.push_back(pM);
		else if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
			CFabController::s_pPM.push_back(pM);
	}

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
		
		if ((m_nCurRow == pNMCD->dwItemSpec) && (m_nCurCol == pDraw->iSubItem) && (pDraw->iSubItem != 0))	// ���� Ŭ���� Sel�� �� �Ķ��� ǥ��
		{
			pDraw->clrText = RGB(255, 255, 255);
			pDraw->clrTextBk = RGB(0, 0, 200);
		}
		else if (!text.IsEmpty() && pDraw->iSubItem != 0)		// Sel�� ����� ������ ��
		{
			NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

			pDraw->clrText = RGB(255, 255, 255);
			pDraw->clrTextBk = RGB(200, 0, 0);
		}
		else
		{
			pDraw->clrText = 0x0;
			pDraw->clrTextBk = 0xffffff;
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

	if (m_nCurCol == 0)									// 1st �÷��� ��ȭ���� ó�� ���� ����
		return;

	if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty())	// �� ����ִ��� �Ǵ��ؼ� ���� �����Ͱ� ���� ���� (����� �� ��������) �ڵ� ����
		return;

	// ��� ���� ��� ����
	int nModuleIdx;
	CFabController::GetInstance().SelectModule(m_nCurRow, m_nCurCol, nModuleIdx);
	CFabController::GetInstance().PrintModule((CDialogEx*)this->GetParent(), nModuleIdx);


	*pResult = 0;
	//*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;
}

// ���콺 DB Ŭ��
void CFabInfoListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_nCurCol == 0)	return;									// 1st �÷��� ��ȭ���� ó�� ���� ����
	if (!(GetItemText(m_nCurRow, m_nCurCol).IsEmpty())) return;	// �� ����ִ��� �Ǵ��ؼ� ���� �����Ͱ� ���� ���� (����� �� ��������) �Լ�����

	// CreateDlg ����
	CSelCreateModuleDlg dlg(this);
	::GetCursorPos(&dlg.m_ptPos);

	int nSelOption = (int)dlg.DoModal();

	*pResult = 0;
}

// ���콺 R Ŭ��
void CFabInfoListCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// ����Ʈ��Ʈ�� Ŭ���� ��������� Row, Col idx��������
	
	m_nCurRow = pNMItemActivate->iItem;
	m_nCurCol = pNMItemActivate->iSubItem;

	SetItemState(m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	if (m_nCurCol == 0) return;
	if (GetItemText(m_nCurRow, m_nCurCol).IsEmpty()) return;

	// �˾��޴� ===================================================================	
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_MODULE);
	CMenu* pSubMenu = popMenu.GetSubMenu(0);
	CPoint pContext;
	GetCursorPos(&pContext);
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pContext.x, pContext.y, this);

	// ���� ���� ���û��¸� �����մϴ�
	//SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);

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
	if (IDYES == AfxMessageBox(_T("����� �����Ͻðڽ��ϱ�?"), MB_YESNO))
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