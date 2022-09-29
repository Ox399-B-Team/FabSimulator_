#pragma once

#include "pch.h"
#include "CFabController.h"

vector<ModuleBase*> CFabController::s_pLPM;
vector<ModuleBase*> CFabController::s_pATMRobot;
vector<ModuleBase*> CFabController::s_pLL;
vector<ModuleBase*> CFabController::s_pVACRobot;
vector<ModuleBase*> CFabController::s_pPM;

bool CFabController::s_bAllWorkOver;

HANDLE CFabController::s_hMoniteringThread1;
HANDLE CFabController::s_hMoniteringThread2;


#pragma region ������/�Ҹ���

CFabController::CFabController()
{
	m_pMainDlg = (CSimulatorPrototypeDlg*)AfxGetMainWnd();
	m_bRunning = FALSE;
	m_nCurModuleIdx = -1;
}

CFabController::CFabController(const CFabController& other)
{
	m_pMainDlg = (CSimulatorPrototypeDlg*)AfxGetMainWnd();
	m_bRunning = FALSE;
}

CFabController::~CFabController()
{
}

#pragma endregion

#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "resource.h"

#pragma region �޼���

int CFabController::SelectModule(int nRow, int nCol, int& pModuleIdx)
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		if (m_pModule[i]->m_nCol == nCol && m_pModule[i]->m_nRow == nRow)
		{
			pModuleIdx = i;
			switch (m_pModule[i]->m_eModuleType)
			{
			case TYPE_LPM:
				return (int)TYPE_LPM;

			case TYPE_ATMROBOT:
				return (int)TYPE_ATMROBOT;

			case TYPE_LOADLOCK:
				return (int)TYPE_LOADLOCK;

			case TYPE_VACROBOT:
				return (int)TYPE_VACROBOT;

			case TYPE_PROCESSCHAMBER:
				return (int)TYPE_PROCESSCHAMBER;
			}
		}
	}

	pModuleIdx = -1;
	return -1;
}

void CFabController::DrawModule()
{
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pMainDlg->m_ctrlListFabInfo.SetItemText(m_pModule[i]->m_nRow, m_pModule[i]->m_nCol, m_pModule[i]->GetModuleName());
	}
}

BOOL CFabController::InitUpdateDlg(CDialogEx* pDlg, int nModuleIdx)
{
	switch (m_pModule[nModuleIdx]->m_eModuleType)
	{
	case TYPE_LPM:
	{
		LPM* pModule = (LPM*)m_pModule[nModuleIdx];
		CFormLPM* pForm = (CFormLPM*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();
		pForm->m_nWaferMax = pModule->GetWaferMax();

		return TRUE;
	}
	case TYPE_ATMROBOT:
	{
		ATMRobot* pModule = (ATMRobot*)m_pModule[nModuleIdx];
		CFormATM* pForm = (CFormATM*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();

		if (pModule->GetWaferMax() == 2) pForm->m_ctrlArm.SetCurSel(0);
		else pForm->m_ctrlArm.SetCurSel(1);

		pForm->m_nPickTime = pModule->GetPickTime();
		pForm->m_nPlaceTime = pModule->GetPlaceTime();
		pForm->m_nRotateTime = pModule->GetRotateTime();
		pForm->m_nZRotateTime = pModule->GetRotateZTime();

		return TRUE;
	}
	case TYPE_LOADLOCK:
	{
		LoadLock* pModule = (LoadLock*)m_pModule[nModuleIdx];
		CFormLL* pForm = (CFormLL*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();
		pForm->m_nWaferMax = pModule->GetWaferMax();

		pForm->m_nPumpTime = pModule->GetPumpTime();
		pForm->m_nPumpStable = pModule->GetPumpStableTime();
		pForm->m_nVentTime = pModule->GetVentTime();
		pForm->m_nVentStable = pModule->GetVentStableTime();
		pForm->m_nDoorOpen = pModule->GetDoorValveOpenTime();
		pForm->m_nDoorClose = pModule->GetDoorValveCloseTime();
		pForm->m_nSlotOpen = pModule->GetSlotValveOpenTime();
		pForm->m_nSlotClose = pModule->GetSlotValveCloseTime();

		return TRUE;
	}
	case TYPE_VACROBOT:
	{
		VACRobot* pModule = (VACRobot*)m_pModule[nModuleIdx];
		CFormVAC* pForm = (CFormVAC*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();

		if (pModule->GetWaferMax() == 2) pForm->m_ctrlArm.SetCurSel(0);
		else pForm->m_ctrlArm.SetCurSel(1);

		pForm->m_nPickTime = pModule->GetPickTime();
		pForm->m_nPlaceTime = pModule->GetPlaceTime();
		pForm->m_nRotateTime = pModule->GetRotateTime();

		return TRUE;
	}
	case TYPE_PROCESSCHAMBER:
	{
		ProcessChamber* pModule = (ProcessChamber*)m_pModule[nModuleIdx];
		CFormPM* pForm = (CFormPM*)pDlg;

		pForm->m_strObjName = pModule->GetModuleName();
		pForm->m_nWaferMax = pModule->GetWaferMax();
		pForm->m_nProcessTime = pModule->GetProcessTime();
		pForm->m_nCleanTime = pModule->GetCleanTime();
		pForm->m_nCleanCount = pModule->GetCleanCount();
		pForm->m_nSlotOpen = pModule->GetSlotValveOpenTime();
		pForm->m_nSlotClose = pModule->GetSlotValveCloseTime();

		return TRUE;
	}
	default:
		return FALSE;
	}

}

void CFabController::CreateModule(CDialogEx* pDlg, int nModuleIdx)
{
	// Type
	ModuleType eType = (ModuleType)nModuleIdx;

	// ��� Row, Col ��
	int nCol = m_pMainDlg->m_ctrlListFabInfo.m_nCurCol;
	int nRow = m_pMainDlg->m_ctrlListFabInfo.m_nCurRow;

	switch (eType)
	{
	case TYPE_LPM:
	{
		CFormLPM* pForm = (CFormLPM*)pDlg;

		m_pModule.push_back(new LPM(eType, pForm->m_strObjName, pForm->m_nWaferMax, pForm->m_nWaferMax, nRow, nCol));

		break;
	}
	case TYPE_ATMROBOT:
	{
		CFormATM* pForm = (CFormATM*)pDlg;
		
		int nPickTime = pForm->m_nPickTime;
		int nPlaceTime = pForm->m_nPlaceTime;
		int nRotateTime = pForm->m_nRotateTime;
		int nZRotateTime = pForm->m_nZRotateTime;

		m_pModule.push_back(new ATMRobot(
			eType, pForm->m_strObjName, 0, 2, nRow, nCol, 
			nPickTime, nPlaceTime, nRotateTime, nZRotateTime));

		break;
	}
	case TYPE_LOADLOCK:
	{
		CFormLL* pForm = (CFormLL*)pDlg;

		int nWaferMax = pForm->m_nWaferMax;

		// �����ð� ��
		int nPumpTime = pForm->m_nPumpTime;
		int nPumpStable = pForm->m_nPumpStable;
		int nVentTime = pForm->m_nVentTime;
		int nVentStable = pForm->m_nVentStable;
		int nSlotOpen = pForm->m_nSlotOpen;
		int nSlotClose = pForm->m_nSlotClose;
		int nDoorOpen = pForm->m_nDoorOpen;
		int nDoorClose = pForm->m_nDoorClose;

		m_pModule.push_back(new LoadLock(
			eType, pForm->m_strObjName, 0, nWaferMax, nRow, nCol,
			nPumpTime, nPumpStable, nVentTime, nVentStable, nSlotOpen, 
			nSlotClose, nDoorOpen, nDoorClose));

		break;
	}
	case TYPE_VACROBOT:
	{
		CFormVAC* pForm = (CFormVAC*)pDlg;

		// Arm
		int nWaferMax = pForm->m_ctrlArm.GetCurSel() == 0 ? 4 : 2;
		
		// ���� �ð� ��
		int nPickTime = pForm->m_nPickTime;
		int nPlaceTime = pForm->m_nPlaceTime;
		int nRotateTime = pForm->m_nRotateTime;

		m_pModule.push_back(new VACRobot(eType, pForm->m_strObjName, 0, nWaferMax, nRow, nCol, 
			nPickTime, nPlaceTime, nRotateTime));
		
		break;
	}
	case TYPE_PROCESSCHAMBER:
	{
		CFormPM* pForm = (CFormPM*)pDlg;

		// �����ð� ��
		int nProcessTime = pForm->m_nProcessTime;
		int nCleanTime = pForm->m_nCleanTime;
		int nSlotOpen = pForm->m_nSlotOpen;
		int nSlotClose = pForm->m_nSlotClose;
		int nCleanCount = pForm->m_nCleanCount;
		
		m_pModule.push_back(new ProcessChamber(eType, pForm->m_strObjName, 0, pForm->m_nWaferMax, nRow, nCol, 
			nProcessTime, nCleanTime, nSlotOpen, nSlotClose, nCleanCount));

		break;
	}
	}

	DrawModule();
}

void CFabController::UpdateModule(CDialogEx* pDlg, int nModuleIdx)
{
	switch (m_pModule[nModuleIdx]->m_eModuleType)
	{
	case TYPE_LPM:
	{
		LPM* pModule = (LPM*)m_pModule[nModuleIdx];
		CFormLPM* pForm = (CFormLPM*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_nWaferMax);
		
		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_ATMROBOT:
	{
		ATMRobot* pModule = (ATMRobot*)m_pModule[nModuleIdx];
		CFormATM* pForm = (CFormATM*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_ctrlArm.GetCurSel() == 0? 2 : 4);
		pModule->SetPickTime(pForm->m_nPickTime);
		pModule->SetPlaceTime(pForm->m_nPlaceTime);
		pModule->SetRotateTime(pForm->m_nRotateTime);
		pModule->SetRotateZTime(pForm->m_nZRotateTime);
		
		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_LOADLOCK:
	{
		LoadLock* pModule = (LoadLock*)m_pModule[nModuleIdx];
		CFormLL* pForm = (CFormLL*)pDlg;
		
		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_nWaferMax);
		pModule->SetPumpTime(pForm->m_nPumpTime);
		pModule->SetPumpStableTime(pForm->m_nPumpStable);
		pModule->SetVentTime(pForm->m_nVentTime);
		pModule->SetVentStableTime(pForm->m_nVentStable);
		pModule->SetDoorValveOpenTime(pForm->m_nDoorOpen);
		pModule->SetDoorValveCloseTime(pForm->m_nDoorClose);
		pModule->SetSlotValveOpenTime(pForm->m_nSlotOpen);
		pModule->SetSlotValveCloseTime(pForm->m_nSlotClose);

		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_VACROBOT:
	{
		VACRobot* pModule = (VACRobot*)m_pModule[nModuleIdx];
		CFormVAC* pForm = (CFormVAC*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_ctrlArm.GetCurSel() == 0 ? 2 : 4);
		pModule->SetPickTime(pForm->m_nPickTime);
		pModule->SetPlaceTime(pForm->m_nPlaceTime);
		pModule->SetRotateTime(pForm->m_nRotateTime);

		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());

		return;
	}
	case TYPE_PROCESSCHAMBER:
	{
		ProcessChamber* pModule = (ProcessChamber*)m_pModule[nModuleIdx];
		CFormPM* pForm = (CFormPM*)pDlg;

		pModule->SetModuleName(pForm->m_strObjName);
		pModule->SetWaferMax(pForm->m_nWaferMax);
		pModule->SetProcessTime(pForm->m_nProcessTime);
		pModule->SetCleanTime(pForm->m_nCleanTime);
		pModule->SetCleanCount(pForm->m_nCleanCount);
		pModule->SetSlotValveOpenTime(pForm->m_nSlotOpen);
		pModule->SetSlotValveCloseTime(pForm->m_nSlotClose);

		m_pMainDlg->m_ctrlListFabInfo.SetItemText(pModule->m_nRow, pModule->m_nCol, pModule->GetModuleName());
		
		return;
	}
	default:
		return;
	}
}

void CFabController::DeleteModule(CFabInfoListCtrl* pCtrl, int nModuleIdx)
{
	delete m_pModule[nModuleIdx];

	m_pModule.erase(m_pModule.begin() + nModuleIdx);
	pCtrl->SetItemText(pCtrl->m_nCurRow, pCtrl->m_nCurCol, _T(""));

	// �߰� ���� �ʿ� (����͸� �����忡�� ���Ǵ�? >> ���� �ʿ�)
}

// ��� ��ü ����
void CFabController::ClearAllModule()
{
	m_bRunning = FALSE;

	s_pLPM.clear();
	s_pATMRobot.clear();
	s_pLL.clear();
	s_pVACRobot.clear();
	s_pPM.clear();

	m_vPickModules.clear();
	m_vPlaceModules.clear();

	s_hMoniteringThread1 = NULL;
	s_hMoniteringThread2 = NULL;

	s_bAllWorkOver = FALSE;

	// �� ����� �����带 �����Ű�� �����ؾ��� (��� �ʵ� BOOL m_bRunning �߰� / �� �� ��� �������� While���� m_bRunning ���ǹ� �߰�)
	// �� ����� �۾��� ������ �Լ� ���� �����Ű�Ⱑ ��ǥ..
	for (int i = 0; i < m_pModule.size(); i++)
	{
		//m_pModule[i]->m_th
		delete m_pModule[i];
	}

	m_pModule.clear();

	DrawModule();

	m_pMainDlg->SetWindowText(_T("�ּ� Fab Simulator"));

	// �߰� ���� �ʿ� (����͸� �����忡�� ���Ǵ�? >> ���� �ʿ�)
}

// Info�� ��� ���� ���
void CFabController::PrintModuleInfo(int nModuleIdx, int nModuleType, int nCurSel)
{
	CString strModuleType;
	CString strWaferMax;

	switch ((ModuleType)nModuleType)
	{
	case TYPE_LPM:
	{
		if (nCurSel != 0) // MainDlg�� InfoTab�� Index�� 0�� �� (Informations�� ����� ���� ��)
		{
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
			
			m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);
		}
		
		strModuleType.Format(_T("TYPE_LPM"));
		strWaferMax.Format(_T("%d"), m_pModule[nModuleIdx]->GetWaferMax());

		m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_TYPE_VALUE)->SetWindowText(strModuleType);
		m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_NAME_VALUE)->SetWindowText(m_pModule[nModuleIdx]->GetModuleName());
		m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_WAFERMAX_VALUE)->SetWindowText(strWaferMax);
		
		//m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
		//m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
		//m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
		//m_pMainDlg->m_pFormInfo->UpdateData(0);

		break;
	}

	case TYPE_ATMROBOT:
	{
		ATMRobot* pModule = (ATMRobot*)m_pModule[nModuleIdx];

		if (nCurSel == 0) // MainDlg�� InfoTab�� Index�� 0�� �� (Informations�� ����� ���� ��)
		{
			strModuleType.Format(_T("TYPE_ATMROBOT"));
			strWaferMax.Format(_T("2 (Dual Arm)"));

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_TYPE_VALUE)->SetWindowText(strModuleType);
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_NAME_VALUE)->SetWindowText(m_pModule[nModuleIdx]->GetModuleName());
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_WAFERMAX_VALUE)->SetWindowText(strWaferMax);

			//m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			//m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
			//m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			//m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1) // MainDlg�� InfoTab�� Index�� 1�� �� (Time Info�� ����� ���� ��)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPickTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strPickTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPlaceTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strPlaceTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetRotateTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strRotateTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetRotateZTime());
			m_pMainDlg->m_pFormTimeInfoATM->m_strZRotateTime = strTime;

			m_pMainDlg->m_pFormTimeInfoATM->UpdateData(0);
			
			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}
		
		break;
	}

	case TYPE_LOADLOCK:
	{
		LoadLock* pModule = (LoadLock*)m_pModule[nModuleIdx];

		if (nCurSel == 0) // MainDlg�� InfoTab�� Index�� 0�� �� (Informations�� ����� ���� ��)
		{
			strModuleType.Format(_T("TYPE_LOADLOCK"));
			strWaferMax.Format(_T("%d"), m_pModule[nModuleIdx]->GetWaferMax());

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_TYPE_VALUE)->SetWindowText(strModuleType);
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_NAME_VALUE)->SetWindowText(m_pModule[nModuleIdx]->GetModuleName());
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_WAFERMAX_VALUE)->SetWindowText(strWaferMax);

			//m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			//m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
			//m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			//m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1) // MainDlg�� InfoTab�� Index�� 1�� �� (Time Info�� ����� ���� ��)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPumpTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strPumpTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPumpStableTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strPumpStableTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetVentTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strVentTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetVentStableTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strVentStableTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetDoorValveOpenTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strDoorOpenTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetDoorValveCloseTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strDoorCloseTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveOpenTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strSlotOpenTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveCloseTime());
			m_pMainDlg->m_pFormTimeInfoLL->m_strSlotCloseTime = strTime;

			m_pMainDlg->m_pFormTimeInfoLL->UpdateData(0);


			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}

		break;
	}

	case TYPE_VACROBOT:
	{
		VACRobot* pModule = (VACRobot*)m_pModule[nModuleIdx];

		if (nCurSel == 0) // MainDlg�� InfoTab�� Index�� 0�� �� (Informations�� ����� ���� ��)
		{
			strModuleType.Format(_T("TYPE_VACROBOT"));

			if ((m_pModule[nModuleIdx])->GetWaferMax() == 4)
				strWaferMax.Format(_T("4 (Quad Arm)"));
			else
				strWaferMax.Format(_T("2 (Dual Arm)"));
			
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_TYPE_VALUE)->SetWindowText(strModuleType);
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_NAME_VALUE)->SetWindowText(m_pModule[nModuleIdx]->GetModuleName());
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_WAFERMAX_VALUE)->SetWindowText(strWaferMax);

			//m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			//m_pMainDlg->m_pFormInfo->m_strModuleName = m_pModule[nModuleIdx]->GetModuleName();
			//m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			//m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1) // MainDlg�� InfoTab�� Index�� 1�� �� (Time Info�� ����� ���� ��)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPickTime());
			m_pMainDlg->m_pFormTimeInfoVAC->m_strPickTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetPlaceTime());
			m_pMainDlg->m_pFormTimeInfoVAC->m_strPlaceTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetRotateTime());
			m_pMainDlg->m_pFormTimeInfoVAC->m_strRotateTime = strTime;

			m_pMainDlg->m_pFormTimeInfoVAC->UpdateData(0);
			
			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		}

		break;
	}

	case TYPE_PROCESSCHAMBER:
	{
		ProcessChamber* pModule = (ProcessChamber*)m_pModule[nModuleIdx];
		
		if (nCurSel == 0) // MainDlg�� InfoTab�� Index�� 0�� �� (Informations�� ����� ���� ��)
		{
			strModuleType.Format(_T("TYPE_PROCESSCHAMBER"));
			strWaferMax.Format(_T("%d"), pModule->GetWaferMax());

			m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_TYPE_VALUE)->SetWindowText(strModuleType);
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_NAME_VALUE)->SetWindowText(m_pModule[nModuleIdx]->GetModuleName());
			m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_WAFERMAX_VALUE)->SetWindowText(strWaferMax);

			//m_pMainDlg->m_pFormInfo->m_strModuleType = strModuleType;
			//m_pMainDlg->m_pFormInfo->m_strModuleName = pModule->GetModuleName();
			//m_pMainDlg->m_pFormInfo->m_strWaferMax = strWaferMax;
			//m_pMainDlg->m_pFormInfo->UpdateData(0);
		}

		else if (nCurSel == 1)
		{
			CString strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetProcessTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strProcessTime = strTime;

			strTime.Format(_T("%d (cnt)"), pModule->GetProcessCount());
			m_pMainDlg->m_pFormTimeInfoPM->m_strProcessCount = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetCleanTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strCleanTime = strTime;

			strTime.Format(_T("%d (cnt)"), pModule->GetCleanCount());
			m_pMainDlg->m_pFormTimeInfoPM->m_strCleanCount = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveOpenTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strSlotOpenTime = strTime;

			strTime.Format(_T("%d (sec)"), pModule->GetSlotValveCloseTime());
			m_pMainDlg->m_pFormTimeInfoPM->m_strSlotCloseTime = strTime;

			m_pMainDlg->m_pFormTimeInfoPM->UpdateData(0);

			// Form SHOW / HIDE ===================================================
			m_pMainDlg->m_pFormInfo->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
			m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_SHOW);
		}

		break;
	}

	default:
	{
		m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
		m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
		m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
		m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
		m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);

		m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);
	}
	}
}

void CFabController::SetFabInfo(int nHour, int nMin, int nSec)
{
	// Throughput�� ���ϱ� ���� �ð��� double�� ��ȯ (��, ���� �÷� ��ȯ)
	ModuleBase::m_dTotalProcessTime = (double)nHour + ((double)nMin / (double)60) + ((double)nSec / (double)3600);

	ModuleBase::SetTotalThroughput();
	
	CString tmp;
	tmp.Format(_T("%.2lf"), ModuleBase::m_dTotalThroughput);

	m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_THROUGHPUT_VALUE)->SetWindowText(tmp);
}

void CFabController::SetUnitInfo(int nModuleIdx)
{
	m_pModule[nModuleIdx]->SetThroughput();

	CString tmp;
	tmp.Format(_T("%.2lf"), m_pModule[nModuleIdx]->GetThroughput());
	m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_THRUPUT_VALUE)->SetWindowText(tmp);

	tmp.Format(_T("%d"), m_pModule[nModuleIdx]->m_nInputWafer);
	m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_INPUT_VALUE)->SetWindowText(tmp);

	tmp.Format(_T("%d"), m_pModule[nModuleIdx]->m_nOutputWafer);
	m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_OUTPUT_VALUE)->SetWindowText(tmp);
}

// ConfigFile ����
void CFabController::SaveConfigFile(CString strFilePath)
{
	//INI ���Ͽ� ���� ����
	int nModuleSize = (int)m_pModule.size();
	CString strSize;
	strSize.Format(_T("%d\n"), nModuleSize);
	
	WritePrivateProfileString(_T("Common"),//���Ǹ� 
		_T("ModuleCount"), //Ű��
		strSize, //Ű ��
		strFilePath);

	for (int i = 0; i < nModuleSize; i++)
	{	
		m_pModule[i]->SaveConfigModule(i, strFilePath);
	}
}

// ConfigFile �ҷ�����
void CFabController::LoadConfigFile(CString strFilePath)
{
	// Common ���� ���� �б� (ModuleCount)
	int nModuleSize = GetPrivateProfileInt(_T("Common"), _T("ModuleCount"), -1, strFilePath);
	
	for (int i = 0; i < nModuleSize; i++)
	{
		TCHAR szType[100] = { NULL };
		TCHAR szName[100] = { NULL };

		CString strIdx, strModuleType, strModuleName;
		strIdx.Format(_T("%d"), i);

		GetPrivateProfileString(strIdx, _T("ModuleType"), NULL, szType, sizeof(szType), strFilePath);

		strModuleType.Format(_T("%s"), szType);
		
		GetPrivateProfileString(strIdx, _T("ModuleName"), NULL, szName, sizeof(szName), strFilePath);
		strModuleName.Format(_T("%s"), szName);

		int nRow = GetPrivateProfileInt(strIdx, _T("PosY"), -1, strFilePath);
		int nCol = GetPrivateProfileInt(strIdx, _T("PosX"), -1, strFilePath);

		if (strModuleType == _T("TYPE_LPM"))
		{
			ModuleType eType = TYPE_LPM;
			int nWaferMax = GetPrivateProfileInt(strIdx, _T("WaferMax"), -1, strFilePath);

			m_pModule.push_back(new LPM(eType, strModuleName, nWaferMax, nWaferMax, nRow, nCol));
		}
		else if (strModuleType == _T("TYPE_ATMROBOT"))
		{
			ModuleType eType = TYPE_ATMROBOT;

			TCHAR szArmMode[100] = { NULL };
			GetPrivateProfileString(strIdx, _T("ArmMode"), NULL, szArmMode, sizeof(szArmMode), strFilePath);

			CString strArmMode;
			strArmMode.Format(_T("%s"), szArmMode);
			int nWaferMax = strArmMode == _T("DualArm") ? 2 : 4;

			int nPickTime = GetPrivateProfileInt(strIdx, _T("PickTime"), -1, strFilePath);
			int nPlaceTime = GetPrivateProfileInt(strIdx, _T("PlaceTime"), -1, strFilePath);
			int nRotateTime = GetPrivateProfileInt(strIdx, _T("StationMoveTime"), -1, strFilePath);
			int nZRotateTime = GetPrivateProfileInt(strIdx, _T("Z-MoveTime"), -1, strFilePath);
			
			m_pModule.push_back(new ATMRobot(eType, strModuleName, 0, nWaferMax, nRow, nCol, nPickTime, nPlaceTime, nRotateTime, nZRotateTime));
		}
		else if (strModuleType == _T("TYPE_LOADLOCK"))
		{
			ModuleType eType = TYPE_LOADLOCK;
			int nWaferMax = GetPrivateProfileInt(strIdx, _T("WaferMax"), -1, strFilePath);
			int nPumpTime = GetPrivateProfileInt(strIdx, _T("PumpTime"), -1, strFilePath);
			int nPumpStableTime = GetPrivateProfileInt(strIdx, _T("PumpStableTime"), -1, strFilePath);
			int nVentTime = GetPrivateProfileInt(strIdx, _T("VentTime"), -1, strFilePath);
			int nVentStableTime = GetPrivateProfileInt(strIdx, _T("VentStableTime"), -1, strFilePath);
			int nSlotOpenTime = GetPrivateProfileInt(strIdx, _T("SlotValveOpenTime"), -1, strFilePath);
			int nSlotCloseTime = GetPrivateProfileInt(strIdx, _T("SlotValveCloseTime"), -1, strFilePath);
			int nDoorOpenTime = GetPrivateProfileInt(strIdx, _T("DoorValveOpenTime"), -1, strFilePath);
			int nDoorCloseTime = GetPrivateProfileInt(strIdx, _T("DoorValveCloseTime"), -1, strFilePath);

			m_pModule.push_back(new LoadLock(eType, strModuleName, 0, nWaferMax, nRow, nCol, 
				nPumpTime, nPumpStableTime, nVentTime, nVentStableTime, nSlotOpenTime, nSlotCloseTime, nDoorOpenTime, nDoorCloseTime));
		}
		else if (strModuleType == _T("TYPE_VACROBOT"))
		{
			ModuleType eType = TYPE_VACROBOT;

			TCHAR szArmMode[100] = { NULL };
			GetPrivateProfileString(strIdx, _T("ArmMode"), NULL, szArmMode, sizeof(szArmMode), strFilePath);
			
			CString strArmMode;
			strArmMode.Format(_T("%s"), szArmMode);
			int nWaferMax = strArmMode == _T("DualArm") ? 2 : 4;

			int nPickTime = GetPrivateProfileInt(strIdx, _T("PickTime"), -1, strFilePath);
			int nPlaceTime = GetPrivateProfileInt(strIdx, _T("PlaceTime"), -1, strFilePath);
			int nRotateTime = GetPrivateProfileInt(strIdx, _T("RotateTime"), -1, strFilePath);

			m_pModule.push_back(new VACRobot(eType, strModuleName, 0, nWaferMax, nRow, nCol, nPickTime, nPlaceTime, nRotateTime));
		}
		else if (strModuleType == _T("TYPE_PROCESSCHAMBER"))
		{
			ModuleType eType = TYPE_PROCESSCHAMBER;
			int nWaferMax = GetPrivateProfileInt(strIdx, _T("WaferMax"), -1, strFilePath);
			int nProcessTime = GetPrivateProfileInt(strIdx, _T("ProcessTime"), -1, strFilePath);
			int nCleanTime = GetPrivateProfileInt(strIdx, _T("CleanTime"), -1, strFilePath);
			int nCleanCount = GetPrivateProfileInt(strIdx, _T("CleanCount"), -1, strFilePath);
			int nSlotOpenTime = GetPrivateProfileInt(strIdx, _T("SlotValveOpenTime"), -1, strFilePath);
			int nSlotCloseTime = GetPrivateProfileInt(strIdx, _T("SlotValveCloseTime"), -1, strFilePath);
			
			m_pModule.push_back(new ProcessChamber(eType, strModuleName, 0, nWaferMax, nRow, nCol, nProcessTime, nCleanTime, nSlotOpenTime, nSlotCloseTime, nCleanCount));
		}

	}
	
	DrawModule();
}

// CSVFile ����
void CFabController::SaveCSVFile(CString strFilePath)
{
	CStdioFile cFile;	// Text ��� ���� �� ���Ǽ��� ���� CFile �ڽ� Ŭ������ CStdioFile ��� ����
	CFileException cException;	// ������ �۾��� ���¸� ������ ���� ���� ���� ��ü�� ���� ������

	// CStdioFile Open ���� �� ����ó�� (ex�� ���� ���¸� ���� �� ���)
	if (!cFile.Open(strFilePath, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate, &cException))
	{
		TCHAR szError[1024];
		cException.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("���� ���� �Ұ� : %1024s"), szError);

		return;
	}
	
	CTime cTime = CTime::GetCurrentTime();	// ���� �ð� ������
	CString strCurTime = cTime.Format(_T("%Y-%m-%d   %H:%M:%S "));
	
	CString strTemp;
	strTemp.Format(_T(",\n") + strCurTime + _T("JUSUNG FAB SIMULATOR - RESULT DATA,\n\n"));
	cFile.WriteString(strTemp);
	
	strTemp.Format(_T("/////////////////////////////////////////////////////////////////////////////////////////,\n\n"));
	cFile.WriteString(strTemp);
	
	strTemp.Format(_T("Fab Total Run Time : %02d:%02d:%02d,\n"), m_pMainDlg->m_nHour, m_pMainDlg->m_nMinute, m_pMainDlg->m_nSecond);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("Fab Total Output Cell Count : %d Cells, \n"), ModuleBase::s_nTotalOutputWafer);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("Fab Total Input Cell Count : %d Cells, \n"), ModuleBase::s_nTotalInputWafer);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("Fab Throughput : %.2lf Cell/Hr, \n\n"), ModuleBase::m_dTotalThroughput);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("/////////////////////////////////////////////////////////////////////////////////////////,\n\n\n"));
	cFile.WriteString(strTemp);

	strTemp.Format(_T("Idx, Module Type, Name, WaferMax, Total Run Time, Input Cell Count, OutputCellCount, Thruput(Cell/Hr),\n"));
	cFile.WriteString(strTemp);
	
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pModule[i]->SaveCSVModule(i, strFilePath, cFile, m_pMainDlg->m_nHour, m_pMainDlg->m_nMinute, m_pMainDlg->m_nSecond);
	}
	
	cFile.SeekToEnd();
	cFile.Close();
}

//////////////////////////////////////////////////////////
//�߾Ӱ��� thread : ���۰����� ����͸��ϰ� �������ִ� thread
//////////////////////////////////////////////////////////
DWORD WINAPI MoniteringThread1(LPVOID p)
{
	CFabController* pController = (CFabController*)p;

	SetEvent(VACRobot::s_hVACRobotExchangeOver);

	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();

	int nCheckTotalOutputWafer = 0;

	//����. ����� ��������� ��Ȳ�� ���߾� �ٲپ� ��
	while (pController->m_bRunning)
	{
		int nCntTotalPmWafer = 0;
		int nCntTotalLLWafer = 0;
		CFabController::s_bAllWorkOver = true;

		//bool bModuleEmpty = true;

		//for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
		//{
		//	/*if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_LPM &&
		//		CFabController::GetInstance().m_pModule[i]->GetWaferCount() != 0)
		//		bModuleEmpty = false;*/

		//	if (i < CFabController::s_pPM.size())
		//		nCntTotalPmWafer += CFabController::s_pPM[i]->GetWaferCount();

		//	if (i < CFabController::s_pLL.size())
		//		nCntTotalLLWafer += CFabController::s_pLL[i]->GetWaferCount();

		//	//PM�� ������ ��� ������ ���� �ϰ� ���� ���� ���
		//	//if (CFabController::GetInstance().m_pModule[i]->m_eModuleType != TYPE_PROCESSCHAMBER &&
		//	//	CFabController::GetInstance().m_pModule[i]->GetIsWorking() == true)
		//	//	CFabController::s_bAllWorkOver = false;
		//}

		//������ ��������� �ٲپ� ��
		//if (ModuleBase::s_bDirect == false &&
		//	nCntTotalLLWafer == nMaxPMSlot &&
		//	nCntTotalPmWafer == nMaxPMSlot &&
		//	LoadLock::s_nTotalSendWaferFromLL == 0)
		//{
		//	//ModuleBase::s_bDirect = true;
		//}
		//WaitForSingleObject(VACRobot::s_hVACRobotExchangeOver, INFINITE);

		if (ModuleBase::s_bDirect == true &&
			LPM::s_nTotalOutputWafer > nCheckTotalOutputWafer && LPM::s_nTotalOutputWafer % nMaxPMSlot == 0)
		{
			ModuleBase::s_bDirect = false;

			nCheckTotalOutputWafer = LPM::s_nTotalOutputWafer;

			for (int i = 0; i < CFabController::s_pLL.size(); i++)
			{
				LoadLock* pLL = (LoadLock*)CFabController::s_pLL[i];
				SetEvent(pLL->m_hLLWaferCntChangeEvent);
			}

			//Exchange�� �����ٴ� ����
		
			for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
			{
				CFabController::GetInstance().m_pModule[i]->m_bExchangeOver = false;
			}
			//ResetEvent(VACRobot::s_hVACRobotExchangeOver);
		}
	}

	return 0;
}

DWORD WINAPI MoniteringThread2(LPVOID p)
{
	CFabController* pController = (CFabController*)p;

	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();

	int nCheckTotalOutputWafer = 0;

	while (pController->m_bRunning)
	{
		WaitForSingleObject(ATMRobot::s_hEventSendWaferChange, INFINITE);
		ResetEvent(ATMRobot::s_hEventSendWaferChange);
		if (LPM::s_nTotalSendWafer == nMaxPMSlot + nMaxLLSlot ||
			(LPM::s_nTotalSendWafer > nMaxPMSlot + nMaxLLSlot && (LPM::s_nTotalSendWafer - (nMaxPMSlot + nMaxLLSlot)) % nMaxLLSlot == 0))
		{
			LPM::s_bLPMWaferPickBlock = true;


			while(1)
			{
				WaitForSingleObject(ATMRobot::s_hEventOutputWaferChange, INFINITE);
				if (LPM::s_nTotalOutputWafer > nCheckTotalOutputWafer && LPM::s_nTotalOutputWafer % nMaxPMSlot == 0)
				{
					LPM::s_bLPMWaferPickBlock = false;

					nCheckTotalOutputWafer = LPM::s_nTotalOutputWafer;

					break;
				}
			}
		}			
	}

	return 0;
}

void CFabController::RunModules()
{	
	//���ʷ� �����ϴ� ��� ����
	if (m_pModule[1]->IsRunning() == false)		// ����ڰ� LPM�� ������ �ٸ� ����� ���� ����� ���� ����?
	{
		CListCtrl* pListCtrl = (&(m_pMainDlg->m_ctrlListFabInfo));
		
		for (int i = 0; i < m_pModule.size(); i++)		// ��� Ÿ�Ժ� ����
		{
			ModuleBase* pM = m_pModule[i];

			if (pM->m_eModuleType == TYPE_LPM)
				s_pLPM.push_back(pM);
			else if (pM->m_eModuleType == TYPE_ATMROBOT)
				s_pATMRobot.push_back(pM);
			else if (pM->m_eModuleType == TYPE_LOADLOCK)
				s_pLL.push_back(pM);
			else if (pM->m_eModuleType == TYPE_VACROBOT)
				s_pVACRobot.push_back(pM);
			else if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
				s_pPM.push_back(pM);
		}

		// LL �� WaferMax < PM �� WaferMax ����
		if (s_pLL.size() * s_pLL[0]->GetWaferMax() > s_pPM.size() * s_pPM[0]->GetWaferMax())
		{
			AfxMessageBox(_T("LL���� �� wafer ���� PM���� �� Wafer ������ ���� �� �����ϴ�.\n"));
		}

		else
		{
			m_bRunning = TRUE;

			//�߾Ӱ��� thread ����
			s_hMoniteringThread1 = CreateThread(NULL, NULL, MoniteringThread1, this, NULL, NULL);
			s_hMoniteringThread2 = CreateThread(NULL, NULL, MoniteringThread2, this, NULL, NULL);

			//Process�� �̹� �������� �ƴ� �� ����
			for (int i = 0; i < m_pModule.size(); i++)
			{
				//Robot�� ��� ���� ����(TM, ATM) : Robot �յڷ� input, ��������
				if (m_pModule[i]->m_eModuleType == TYPE_ATMROBOT || m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
				{
					for (int j = 0; j < m_pModule.size(); j++)
					{
						{
							if (m_pModule[i]->m_nCol - 1 == m_pModule[j]->m_nCol) 
								m_vPickModules.push_back(m_pModule[j]);

							else if (m_pModule[i]->m_nCol + 1 == m_pModule[j]->m_nCol)
								m_vPlaceModules.push_back(m_pModule[j]);
						}
					}

					if (m_pModule[i]->m_eModuleType == TYPE_ATMROBOT)
					{
						ATMRobot* p = (ATMRobot*)m_pModule[i];
						p->Run(m_vPickModules, m_vPlaceModules, pListCtrl);
					}	

					else if (m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
					{
						VACRobot* p = (VACRobot*)m_pModule[i];
						p->Run(m_vPickModules, m_vPlaceModules, pListCtrl);
					}

					m_vPickModules.clear();
					m_vPlaceModules.clear();
				}

				//LPM, LL, PM�� ��� ���� ����
				else if (m_pModule[i]->m_eModuleType == TYPE_LPM)
				{
					LPM* p;
					p = (LPM*)m_pModule[i];
					p->Run();
				}

				else if (m_pModule[i]->m_eModuleType == TYPE_LOADLOCK)
				{
					LoadLock* p;
					p = (LoadLock*)m_pModule[i];
					p->Run();
				}


				else if (m_pModule[i]->m_eModuleType == TYPE_PROCESSCHAMBER)
				{
					ProcessChamber* p;
					p = (ProcessChamber*)m_pModule[i];
					p->Run();
				}
			}
		}
	}

	//�̹� ������ �����ϰ� �ִ� ��� �Ͻ�����
	else
	{
		for (int i = 0; i < m_pModule.size(); i++)
		{
			m_pModule[i]->Resume();
		}

		ResumeThread(s_hMoniteringThread1);
		ResumeThread(s_hMoniteringThread2);
	}
}

void CFabController::SuspendModules()
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		m_pModule[i]->Suspend();
	}

	SuspendThread(s_hMoniteringThread1);
	SuspendThread(s_hMoniteringThread2);
}

#pragma endregion