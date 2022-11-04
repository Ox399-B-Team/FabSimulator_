#pragma once

#include "pch.h"
#include "CFabController.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "resource.h"

//Bottleneck ���� �ΰ���(1�� ������ �ΰ��ϰ� ������)
#define BOTTLENECK_SENSITIVE 1

#pragma region ������/�Ҹ���

CFabController::CFabController()
{
	m_pMainDlg = (CSimulatorPrototypeDlg*)AfxGetMainWnd();
	m_bRunning = FALSE;
	m_nCurModuleIdx = -1;

	for (int i = 0; i < 3; i++)
	{
		s_hMoniteringThread[i] = NULL;
	}
}

CFabController::CFabController(const CFabController& other)
{
	m_pMainDlg = (CSimulatorPrototypeDlg*)AfxGetMainWnd();
	m_bRunning = FALSE;
	m_nCurModuleIdx = -1;
}

CFabController::~CFabController()
{
}

#pragma endregion

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

int CFabController::SelectModuleCount(ModuleType eType)
{
	switch (eType)
	{
	case TYPE_LPM:
		return LPM::s_nCount;

	case TYPE_ATMROBOT:
		return ATMRobot::s_nCount;

	case TYPE_LOADLOCK:
		return LoadLock::s_nCount;

	case TYPE_VACROBOT:
		return VACRobot::s_nCount;

	case TYPE_PROCESSCHAMBER:
		return ProcessChamber::s_nCount;
	}
	return 0;
}

BOOL CFabController::CompareModuleName(CString strModuleName)
{
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		if (m_pModule[i]->GetModuleName().CompareNoCase(strModuleName) == 0)
			return true;
	}

	return false;
}

void CFabController::DrawModule(bool bEmptyFlag /*=false*/)
{
	if (bEmptyFlag == true)
	{
		for (int i = 1; i < 20; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				m_pMainDlg->m_ctrlListFabInfo.SetItemText(j, i, _T(""));
			}
		}

		return;
	}
	else
	{
		for (int i = 0; i < (int)m_pModule.size(); i++)
		{
			if (m_pModule[i]->m_nCol != 2 || m_pModule[i]->m_nRow != 0)
				m_pMainDlg->m_ctrlListFabInfo.SetItemText(m_pModule[i]->m_nRow, m_pModule[i]->m_nCol, m_pModule[i]->GetModuleName());
		}
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

DWORD WINAPI ClearAllModuleWorkThread(LPVOID p)
{
	CFabController* pCFabController = (CFabController*)p;

	if (pCFabController->m_pModule.size() == 1)
	{
		AfxMessageBox(_T("Clear�� ����� �������� �ʽ��ϴ�."));
		return false;
	}

	else
	{
		double dTmpSpeed = ModuleBase::s_dSpeed;

		ModuleBase::s_dSpeed = 1000;
		pCFabController->RunModules(true);
		//���ʷ� �����ϴ� ��� ����
		// �� ����� �����带 �����Ű�� �����ؾ��� (��� �ʵ� BOOL m_bRunning �߰� / �� �� ��� �������� While���� m_bRunning ���ǹ� �߰�)
		// �� ����� �۾��� ������ �Լ� ���� �����Ű�Ⱑ ��ǥ
		//Sleep(1);

		pCFabController->m_vPickModules.clear();
		pCFabController->m_vPlaceModules.clear();

		//1. ��ü �����ȣ ������
		//����
		for (int i = 0; i < pCFabController->m_pModule.size(); i++)
		{
			pCFabController->m_pModule[i]->m_bStopFlag = true;
		}

		//Thread
		pCFabController->m_bRunning = FALSE;

		//2. MonitoringThreads, LPM, ATM, VAC ����
		if (ATMRobot::s_pATMRobot.size() > 0)
		{
			SetEvent(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);
			SetEvent(ATMRobot::s_hEventSendWaferChange);
		}


		//����ȭ�� ������ ���ᰡ �����ϰԲ� ��ȣ ����(LL, PM)

		for (int i = 0; i < LoadLock::s_pLL.size(); i++)
		{
			SetEvent(LoadLock::s_pLL[i]->m_hLLWaferCntChangeEvent);
		}

		for (int i = 0; i < ProcessChamber::s_pPM.size(); i++)
		{
			SetEvent(ProcessChamber::s_pPM[i]->m_hPmWaferCntChangeEvent);
			SetEvent(ProcessChamber::s_pPM[i]->m_hInitWorkOver);
		}

		AfxMessageBox(_T("��� ������ �����ϰ� ����� ������ ����ϴ� ���Դϴ�.\n[����Ϸ�] �޼����� ��Ÿ�� ������ ��ø� ��ٷ� �ּ���."));
		//3. ��� ������ �����ȣ�� ���� ������ �����
		if (WaitForMultipleObjects(ModuleBase::s_vEventCloseThread.size(), ModuleBase::s_vEventCloseThread.data(), TRUE, 10000)
			!= WAIT_OBJECT_0)
			AfxMessageBox(_T("���ð��� ���� �ɸ� �����Դϴ�.\n���α׷��� �������� �����Ͻô� ���� ��õ�帳�ϴ�."));

		//AfxMessageBox(_T("��� Thread�� ���������� �����"));

		Sleep(1);

		//4. GUI�� ����� ������
		pCFabController->DrawModule(true);

		//5. static ������ �ʱ�ȭ
		ModuleBase::s_bDirect = false;
		ModuleBase::m_dTotalProcessTime = 0.0;
		ModuleBase::m_dTotalCleanTime = 0.0;
		ModuleBase::m_dTotalThroughput = 0.0;
		ModuleBase::s_dSpeed = dTmpSpeed;
		ModuleBase::s_nTotalOutputWafer = 0;
		ModuleBase::s_nTotalInputWafer = 0;

		if (LPM::s_pLPM.size() > 0)
		{
			LPM::s_nTotalSendWafer = 0;
			LPM::s_nTotalInitWafer = 0;
			LPM::s_nTotalUsedDummyWafer = 0;
			LPM::s_bLPMWaferPickBlock = false;
		}

		if (ATMRobot::s_pATMRobot.size() > 0)
		{
			CloseHandle(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);
			ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange = CreateEvent(NULL, FALSE, TRUE, NULL);

			CloseHandle(ATMRobot::s_hEventSendWaferChange);
			ATMRobot::s_hEventSendWaferChange = CreateEvent(NULL, TRUE, TRUE, NULL);

			ATMRobot::s_nTotalWaferCntFromLPM = 0;
			ATMRobot::s_nRequiredDummyWaferCntLpmToPM = 0;
			ATMRobot::s_nRequiredDummyWaferCntPMToLpm = 0;
		}

		if (LoadLock::s_pLL.size() > 0)
		{
			LoadLock::s_nTotalSendWaferFromLL = 0;
			LoadLock::s_nRequiredDummyWaferCntLpmToPM = 0;
			LoadLock::s_nRequiredDummyWaferCntPMToLpm = 0;
		}

		if (ProcessChamber::s_pPM.size() > 0)
		{
			ProcessChamber::s_nCntPMWorkOver = 0;
			ProcessChamber::s_vWorkOverHandle.clear();
		}

		for (int i = 0; i < 3; i++)
		{
			pCFabController->s_hMoniteringThread[i] = NULL;
		}

		//6. vector ���� �ʱ�ȭ �� ����
		pCFabController->m_vPickModules.clear();
		pCFabController->m_vPlaceModules.clear();

		//7. m_pModule ����
		for (int i = 0; i < pCFabController->m_pModule.size(); i++)
		{
			delete pCFabController->m_pModule[i];
		}
		pCFabController->m_pModule.clear();

		pCFabController->m_pMainDlg->SetWindowText(_T("�ּ� Fab Simulator"));

		//8. FormInfo �ʱ�ȭ
		pCFabController->m_pMainDlg->m_pFormInfo->InitializeFormInfo(false);	//
		pCFabController->m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
		pCFabController->m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
		pCFabController->m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
		pCFabController->m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
		pCFabController->m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
		pCFabController->m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);

		//9. Timer �ʱ�ȭ
		pCFabController->m_pMainDlg->m_nHour = 0;
		pCFabController->m_pMainDlg->m_nMinute = 0;
		pCFabController->m_pMainDlg->m_nSecond = 0;

		CString strTemp = _T("");
		strTemp.Format(_T("FAB Time %02d:%02d:%02d"), 0, 0, 0);
		pCFabController->m_pMainDlg->m_ctrlFabTime.SetWindowText(strTemp);

		//10. ���ο� DummyStage �߰�
		pCFabController->m_pModule.push_back(new LPM(TYPE_LPM, _T("DummyStage"), 0, 12, 0, 2));

		AfxMessageBox(_T("����Ϸ�"), MB_OK);

		return true;
	}
}

// ��� ��ü ����
thread CFabController::ClearAllModule()
{
	thread th = thread(ClearAllModuleWorkThread, this);
	return th;
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

		m_pMainDlg->m_pFormInfo->InitializeFormInfo();

		m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);
	}
	}
}

// ��� ��ü Info ����
void CFabController::SetFabInfo(int nHour, int nMin, int nSec)
{
	// Throughput�� ���ϱ� ���� �ð��� double�� ��ȯ (��, ���� �÷� ��ȯ)
	ModuleBase::m_dTotalProcessTime = (double)nHour + ((double)nMin / (double)60) + ((double)nSec / (double)3600);

	if (ModuleBase::s_bIsCleaning)
		ModuleBase::m_dTotalCleanTime += 0.000277778;

	//CString tmp = _T("");
	//tmp.Format(_T("CleanTime:%lf��"), (ModuleBase::m_dTotalCleanTime / 0.000277778) / 3600);
	//AfxGetMainWnd()->SetWindowText(tmp);

	ModuleBase::SetTotalThroughput();

	tmp.Format(_T("%.2lf"), ModuleBase::m_dTotalThroughput);
	
	m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_THROUGHPUT_VALUE)->SetWindowText(tmp);
}

// ��� ���� Info ����
void CFabController::SetUnitInfo(int nModuleIdx)
{
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pModule[i]->SetThroughput();
	}

	if (nModuleIdx != -1)
	{
		CString tmp;
		tmp.Format(_T("%.2lf"), m_pModule[nModuleIdx]->GetThroughput());
		m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_THRUPUT_VALUE)->SetWindowText(tmp);

		tmp.Format(_T("%d"), m_pModule[nModuleIdx]->m_nInputWafer);
		m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_INPUT_VALUE)->SetWindowText(tmp);

		tmp.Format(_T("%d"), m_pModule[nModuleIdx]->m_nOutputWafer);
		m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_UNIT_OUTPUT_VALUE)->SetWindowText(tmp);
	}
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

		if (strModuleType == _T("TYPE_LPM") && strModuleName.Compare(_T("DummyStage")) != 0) //DummyStage ��ø ������ ���� �ڵ� �߰�
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

	strTemp.Format(_T("Fab Throughput : %.2lf Wafer/Hr, \n\n"), ModuleBase::m_dTotalThroughput);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("/////////////////////////////////////////////////////////////////////////////////////////,\n\n\n"));
	cFile.WriteString(strTemp);

	strTemp.Format(_T("Idx, ModuleType, Name, WaferMax, TotalRunTime, InputCellCount, OutputCellCount, Thruput(Wafer/Hr),\n"));
	cFile.WriteString(strTemp);
	
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pModule[i]->SaveCSVModule(i, strFilePath, cFile, m_pMainDlg->m_nHour, m_pMainDlg->m_nMinute, m_pMainDlg->m_nSecond);
	}
	
	cFile.SeekToEnd();
	cFile.Close();
}

// Speed ����
void CFabController::ChangeTimeSpeed(int nCurSpeed)
{
	switch (nCurSpeed)
	{
	case 0:
		ModuleBase::s_dSpeed = 0.01;	// 0.001 == 1���
		break;

	case 1:
		ModuleBase::s_dSpeed = 0.02;
		break;

	case 2:
		ModuleBase::s_dSpeed = 0.06;
		break;

	case 3:
		ModuleBase::s_dSpeed = 0.1;
		break;

	default:
		break;
	}
}

// �׷��� ����
void CFabController::RunGraph()
{
	// ���� �����ֱ� ���� �׷��� ����
	delete m_pMainDlg->m_ctrlGraph;
	delete m_pMainDlg->m_ctrlGraphLPM;
	delete m_pMainDlg->m_ctrlGraphROBOT;
	delete m_pMainDlg->m_ctrlGraphLL;
	delete m_pMainDlg->m_ctrlGraphPM;

	InitGraph();
}

// �׷��� ����
void CFabController::DeleteGraph()
{
	delete m_pMainDlg->m_ctrlGraph;
	delete m_pMainDlg->m_ctrlGraphLPM;
	delete m_pMainDlg->m_ctrlGraphROBOT;
	delete m_pMainDlg->m_ctrlGraphLL;
	delete m_pMainDlg->m_ctrlGraphPM;

	m_pMainDlg->m_ctrlGraph = new COScopeCtrl();
	m_pMainDlg->m_ctrlGraph->Create(WS_VISIBLE | WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphLPM = new COScopeCtrl();
	m_pMainDlg->m_ctrlGraphLPM->Create(WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphROBOT = new COScopeCtrl();
	m_pMainDlg->m_ctrlGraphROBOT->Create(WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphLL = new COScopeCtrl();
	m_pMainDlg->m_ctrlGraphLL->Create(WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphPM = new COScopeCtrl();
	m_pMainDlg->m_ctrlGraphPM->Create(WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_bIsFullGraph = true;
}

// �׷��� �ʱ�ȭ
void CFabController::InitGraph()
{
	// ��ü �׷��� =====================================
	int size = (int)m_pModule.size() + 1;
	m_pMainDlg->m_ctrlGraph = new COScopeCtrl(0, size);
	m_pMainDlg->m_ctrlGraph->Create(WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraph->SetRange(0., 300., size);
	m_pMainDlg->m_ctrlGraph->autofitYscale = true;
	m_pMainDlg->m_ctrlGraph->SetYUnits(_T("Throughput"));
	m_pMainDlg->m_ctrlGraph->SetXUnits(_T("Time"));

	m_pMainDlg->m_ctrlGraph->SetLegendLabel(_T("Total"), 0);

	for (int i = 0; i < size - 1; i++)
	{
		m_pMainDlg->m_ctrlGraph->SetLegendLabel(m_pModule[i]->GetModuleName(), i + 1);
	}

	// LPM �׷��� =====================================
	int w = m_pMainDlg->m_rtGraph.Width() / 2;
	int h = m_pMainDlg->m_rtGraph.Height() / 2;

	size = (int)LPM::s_pLPM.size() + 1;

	m_pMainDlg->m_ctrlGraphLPM = new COScopeCtrl(1, size);
	CRect rt1(CPoint(m_pMainDlg->m_rtGraph.left, m_pMainDlg->m_rtGraph.top), CSize(w, h));
	m_pMainDlg->m_ctrlGraphLPM->Create(WS_CHILD, rt1, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphLPM->SetRange(0., 300., size);
	m_pMainDlg->m_ctrlGraphLPM->autofitYscale = true;
	m_pMainDlg->m_ctrlGraphLPM->SetYUnits(_T("Throughput"));
	m_pMainDlg->m_ctrlGraphLPM->SetXUnits(_T("Time"));

	m_pMainDlg->m_ctrlGraphLPM->SetLegendLabel(_T("Total"), 0);

	for (int i = 0; i < size - 1; i++)
	{
		m_pMainDlg->m_ctrlGraphLPM->SetLegendLabel(LPM::s_pLPM[i]->GetModuleName(), i + 1);
	}

	m_pMainDlg->m_ctrlGraphLPM->ShowWindow(SW_HIDE);


	// ROBOT �׷��� =====================================
	size = (int)ATMRobot::s_pATMRobot.size() + (int)VACRobot::s_pVACRobot.size() + 1;

	m_pMainDlg->m_ctrlGraphROBOT = new COScopeCtrl(2, size);
	CRect rt2(CPoint(m_pMainDlg->m_rtGraph.left + w, m_pMainDlg->m_rtGraph.top), CSize(w, h));
	m_pMainDlg->m_ctrlGraphROBOT->Create(WS_CHILD, rt2, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphROBOT->SetRange(0., 300., size);
	m_pMainDlg->m_ctrlGraphROBOT->autofitYscale = true;
	m_pMainDlg->m_ctrlGraphROBOT->SetYUnits(_T("Throughput"));
	m_pMainDlg->m_ctrlGraphROBOT->SetXUnits(_T("Time"));

	m_pMainDlg->m_ctrlGraphROBOT->SetLegendLabel(_T("Total"), 0);

	m_pMainDlg->m_ctrlGraphROBOT->SetLegendLabel(ATMRobot::s_pATMRobot[0]->GetModuleName(), 1);
	m_pMainDlg->m_ctrlGraphROBOT->SetLegendLabel(VACRobot::s_pVACRobot[0]->GetModuleName(), 2);

	m_pMainDlg->m_ctrlGraphROBOT->ShowWindow(SW_HIDE);

	// LL �׷��� =====================================
	size = (int)LoadLock::s_pLL.size() + 1;

	m_pMainDlg->m_ctrlGraphLL = new COScopeCtrl(3, size);
	CRect rt3(CPoint(m_pMainDlg->m_rtGraph.left, m_pMainDlg->m_rtGraph.top + h), CSize(w, h));
	m_pMainDlg->m_ctrlGraphLL->Create(WS_CHILD, rt3, m_pMainDlg, IDC_STATIC_RT_GRAPH);
	
	m_pMainDlg->m_ctrlGraphLL->SetRange(0., 150., size);
	m_pMainDlg->m_ctrlGraphLL->autofitYscale = true;
	m_pMainDlg->m_ctrlGraphLL->SetYUnits(_T("Throughput"));
	m_pMainDlg->m_ctrlGraphLL->SetXUnits(_T("Time"));

	m_pMainDlg->m_ctrlGraphLL->SetLegendLabel(_T("Total"), 0);

	for (int i = 0; i < size - 1; i++)
	{
		m_pMainDlg->m_ctrlGraphLL->SetLegendLabel(LoadLock::s_pLL[i]->GetModuleName(), i + 1);
	}

	m_pMainDlg->m_ctrlGraphLL->ShowWindow(SW_HIDE);


	// PM �׷��� =====================================
	size = (int)ProcessChamber::s_pPM.size() + 1;

	m_pMainDlg->m_ctrlGraphPM = new COScopeCtrl(4, size);
	CRect rt4(CPoint(m_pMainDlg->m_rtGraph.left + w, m_pMainDlg->m_rtGraph.top + h), CSize(w, h));
	m_pMainDlg->m_ctrlGraphPM->Create(WS_CHILD, rt4, m_pMainDlg, IDC_STATIC_RT_GRAPH);

	m_pMainDlg->m_ctrlGraphPM->SetRange(0., 150., size);
	m_pMainDlg->m_ctrlGraphPM->autofitYscale = true;
	m_pMainDlg->m_ctrlGraphPM->SetYUnits(_T("Throughput"));
	m_pMainDlg->m_ctrlGraphPM->SetXUnits(_T("Time"));

	m_pMainDlg->m_ctrlGraphPM->SetLegendLabel(_T("Total"), 0);

	for (int i = 0; i < size - 1; i++)
	{
		m_pMainDlg->m_ctrlGraphPM->SetLegendLabel(ProcessChamber::s_pPM[i]->GetModuleName(), i + 1);
	}

	m_pMainDlg->m_ctrlGraphPM->ShowWindow(SW_HIDE);

	ChangeGraph(m_pMainDlg->m_bIsFullGraph);
}

// �׷��� ����
void CFabController::ChangeGraph(bool m_bIsFullGraph)
{
	if (m_bIsFullGraph)
	{
		m_pMainDlg->m_ctrlGraph->ShowWindow(SW_SHOW);
		m_pMainDlg->m_ctrlGraphLPM->ShowWindow(SW_HIDE);
		m_pMainDlg->m_ctrlGraphROBOT->ShowWindow(SW_HIDE);
		m_pMainDlg->m_ctrlGraphLL->ShowWindow(SW_HIDE);
		m_pMainDlg->m_ctrlGraphPM->ShowWindow(SW_HIDE);
	}
	else
	{
		m_pMainDlg->m_ctrlGraph->ShowWindow(SW_HIDE);
		m_pMainDlg->m_ctrlGraphLPM->ShowWindow(SW_SHOW);
		m_pMainDlg->m_ctrlGraphROBOT->ShowWindow(SW_SHOW);
		m_pMainDlg->m_ctrlGraphLL->ShowWindow(SW_SHOW);
		m_pMainDlg->m_ctrlGraphPM->ShowWindow(SW_SHOW);
	}
}

//////////////////////////////////////////////////////////
//�߾Ӱ��� thread : ���۰����� ����͸��ϰ� �������ִ� thread
//////////////////////////////////////////////////////////
DWORD WINAPI MoniteringThread1(LPVOID p)
{
	CFabController* pController = (CFabController*)p;

	int const nMaxPMSlot = ProcessChamber::s_pPM.size() * ProcessChamber::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = LoadLock::s_pLL.size() * LoadLock::s_pLL[0]->GetWaferMax();

	int nCheckTotalOutputAndDummyWafer = 0;

	//����. ����� ��������� ��Ȳ�� ���߾� �ٲپ� ��
	while (pController->m_bRunning)
	{
		Sleep(1 / ModuleBase::s_dSpeed);

		int nCntTotalPmWafer = 0;
		int nCntTotalLLWafer = 0;

		//1. ����� ���⺯�� �� �ʱ�ȭ ó��
		if (ModuleBase::s_bDirect == true &&
			(LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer > nCheckTotalOutputAndDummyWafer)
			&& (LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer) % nMaxPMSlot == 0)
		{
			ModuleBase::s_bDirect = false;

			nCheckTotalOutputAndDummyWafer = LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer;

			for (int i = 0; i < LoadLock::s_pLL.size(); i++)
			{
				SetEvent(LoadLock::s_pLL[i]->m_hLLWaferCntChangeEvent);
			}

			//Exchange�� �����ٴ� ����
			for (int i = 0; i < CFabController::GetInstance().m_pModule.size(); i++)
			{
				CFabController::GetInstance().m_pModule[i]->m_bExchangeOver = false;
			}
		}
	}

	return 0;
}

DWORD WINAPI MoniteringThread2(LPVOID p)
{
	CFabController* pController = (CFabController*)p;

	int const nMaxPMSlot = ProcessChamber::s_pPM.size() * ProcessChamber::s_pPM[0]->GetWaferMax();
	int nCheckTotalOutputAndDummyWafer = 0;

	int nCnt = 0;

	while (pController->m_bRunning)
	{
		WaitForSingleObject(ATMRobot::s_hEventSendWaferChange, INFINITE);
		ResetEvent(ATMRobot::s_hEventSendWaferChange);
		if (LPM::s_nTotalSendWafer >= nMaxPMSlot && (LPM::s_nTotalSendWafer - nMaxPMSlot) % nMaxPMSlot == 0)
		{
			LPM::s_bLPMWaferPickBlock = true;

			while(pController->m_bRunning)
			{
				if (nCnt == 0)
				{
					WaitForMultipleObjects(ProcessChamber::s_vWorkOverHandle.size(), ProcessChamber::s_vWorkOverHandle.data(), TRUE, INFINITE);
					LPM::s_bLPMWaferPickBlock = false;
					nCnt++;
					break;
				}

				WaitForSingleObject(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange, INFINITE);
				if ((LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer > nCheckTotalOutputAndDummyWafer
					&& (LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer) % nMaxPMSlot == 0)
					)
				{					
					WaitForMultipleObjects(ProcessChamber::s_vWorkOverHandle.size(), ProcessChamber::s_vWorkOverHandle.data(), TRUE, INFINITE);
					ProcessChamber::s_nCntPMWorkOver = 0;

					LPM::s_bLPMWaferPickBlock = false;
					nCheckTotalOutputAndDummyWafer = LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer;

					//::OutputDebugString()

					break;
				}
			}
		}			
	}

	return 0;
}

DWORD WINAPI MoniteringThread3(LPVOID p)
{
	CString tmp = NULL;
	CFabController* pController = (CFabController*)p;

	while (pController->m_bRunning)
	{
		Sleep(600 / ModuleBase::s_dSpeed);
		ModuleBase* pBottleneckModule = NULL;
		ModuleBase* pPreModule = NULL;


		if (pController->m_pMainDlg->m_bIsRunning == TRUE)
		{

			LoadLock* pStandardLL = LoadLock::s_pLL[0];
			for (int i = 0; i < LoadLock::s_pLL.size(); i++)
			{
				if (pStandardLL->m_nInputWafer > pStandardLL->GetWaferMax()
					&& (pStandardLL->m_nInputWafer > LoadLock::s_pLL[i]->m_nInputWafer * BOTTLENECK_SENSITIVE
						|| pStandardLL->m_nInputWafer < LoadLock::s_pLL[i]->m_nInputWafer / BOTTLENECK_SENSITIVE))
				{
					pBottleneckModule = LoadLock::s_pLL[i];

					//////////////////

					if (pBottleneckModule != pPreModule)
					{
						pPreModule = LoadLock::s_pLL[i];

						pController->m_pMainDlg->OnBnClickedButtonLinecontrolRun();

						tmp.Format(_T("%s ��⿡�� Bottleneck�� �ǽɵ˴ϴ�. \n �� ������ �簳�մϴ�."), pBottleneckModule->GetModuleName());
						AfxMessageBox(tmp);

						Sleep(3000);
						
						pController->m_pMainDlg->OnBnClickedButtonLinecontrolRun();
					}
					//////////////////
				}
			}

			ProcessChamber* pStandardPM = ProcessChamber::s_pPM[0];
			for (int i = 0; i < ProcessChamber::s_pPM.size(); i++)
			{
				if (pStandardPM->m_nInputWafer > pStandardPM->GetWaferMax()
					&& (pStandardPM->m_nInputWafer > ProcessChamber::s_pPM[i]->m_nInputWafer * BOTTLENECK_SENSITIVE
						|| pStandardPM->m_nInputWafer < ProcessChamber::s_pPM[i]->m_nInputWafer / BOTTLENECK_SENSITIVE))
				{
					pBottleneckModule = ProcessChamber::s_pPM[i];

					//////////////////
					if (pBottleneckModule != pPreModule)
					{
						pPreModule = ProcessChamber::s_pPM[i];

						pController->m_pMainDlg->OnBnClickedButtonLinecontrolRun();
					
						tmp.Format(_T("%s ��⿡�� Bottleneck�� �ǽɵ˴ϴ�. \n �� ������ �簳�մϴ�."), pBottleneckModule->GetModuleName());
						AfxMessageBox(tmp);
						Sleep(3000);

						pController->m_pMainDlg->OnBnClickedButtonLinecontrolRun();
					}
					///////////////////
				}
			}
		}

		else
		{
			Sleep(1200 / ModuleBase::s_dSpeed);
		}
	}

	return 0;
}


bool CFabController::RunModules(bool bRunToClear)
{	
	//���� ó��
	if (m_pModule.size() == 0 && bRunToClear == false)
	{
		AfxMessageBox(_T("������ ����� �����ϴ�.\n"), MB_ICONSTOP);
		return false;
	}

	//����
	if (m_pModule[0]->IsRunning() == false)		// ����ڰ� LPM�� ������ �ٸ� ����� ���� ����� ���� ����?
	{
		//���� ó��(Clear�� ���� Run��Ų ��찡 �ƴ� ���)
		if (bRunToClear == false)
		{
			// ��� ����� ���� ���
			if (LPM::s_pLPM.size() < 3 || ATMRobot::s_pATMRobot.size() == 0 || LoadLock::s_pLL.size() == 0 || VACRobot::s_pVACRobot.size() == 0 || ProcessChamber::s_pPM.size() == 0)
			{
				if(LPM::s_pLPM.size() < 3)
					AfxMessageBox(_T("LPM ��� ���� ������� �ʽ��ϴ�.\n"));
				else if(ATMRobot::s_pATMRobot.size() == 0)
					AfxMessageBox(_T("ATMRobot ��� ���� ������� �ʽ��ϴ�.\n"));
				else if (LoadLock::s_pLL.size() == 0)
					AfxMessageBox(_T("LoadLock ��� ���� ������� �ʽ��ϴ�.\n"));
				else if (VACRobot::s_pVACRobot.size() == 0)
					AfxMessageBox(_T("VACRobot ��� ���� ������� �ʽ��ϴ�.\n"));
				else if (ProcessChamber::s_pPM.size() == 0)
					AfxMessageBox(_T("ProcessChamber ��� ���� ������� �ʽ��ϴ�.\n"));

				return false;
			}
			// LL �� WaferMax < PM �� WaferMax ����
			if (LoadLock::s_pLL.size() * LoadLock::s_pLL[0]->GetWaferMax() != ProcessChamber::s_pPM.size() * ProcessChamber::s_pPM[0]->GetWaferMax())
			{
				AfxMessageBox(_T("LL���� �� wafer ���� PM���� �� Wafer ���� ���ƾ� �մϴ�.\n"), MB_ICONSTOP);
				return false;
			}
			// LL �� WaferMax < PM �� WaferMax ����
			if (ProcessChamber::s_pPM.size() > 4 && ProcessChamber::s_pPM[0]->GetWaferMax() > 1)
			{
				AfxMessageBox(_T("Slot�� 2�� �̻��� PM���� TM�� 4������ ���� �� �ֽ��ϴ�.\n"), MB_ICONSTOP);
				return false;
			}
			// ��� PM�� Slot �� ���ƾ���
			bool bCheck = false;
			for (int i = 0; i < ProcessChamber::s_pPM.size(); i++)
			{
				if (ProcessChamber::s_pPM[0]->GetWaferMax() != ProcessChamber::s_pPM[i]->GetWaferMax())
				{
					AfxMessageBox(_T("��� PM���� Wafer Slot ���� ���ƾ� �մϴ�.\n"), MB_ICONSTOP);
					bCheck = true;
					break;
				}
			}
			if (bCheck == true)
				return false;

			// ������� ��, 
			if (ProcessChamber::s_pPM[0]->GetWaferMax() % 2 != 0 && VACRobot::s_pVACRobot[0]->GetWaferMax() == 4)
			{
				AfxMessageBox(_T("VACRobot�� Quad Arm�� ���, PM�� Slot���� ¦������ �մϴ�.\n"), MB_ICONSTOP);
				return false;
			}
		}

		CListCtrl* pListCtrl = (&(m_pMainDlg->m_ctrlListFabInfo));

		m_bRunning = TRUE;

		//�߾Ӱ��� thread ����
		if (bRunToClear == false)
		{
			s_hMoniteringThread[0] = CreateThread(NULL, NULL, MoniteringThread1, this, NULL, NULL);
			s_hMoniteringThread[1] = CreateThread(NULL, NULL, MoniteringThread2, this, NULL, NULL);
			s_hMoniteringThread[2] = CreateThread(NULL, NULL, MoniteringThread3, this, NULL, NULL);
		}

		//Process�� �̹� �������� �ƴ� �� ����
		for (int i = 0; i < m_pModule.size(); i++)
		{
			//Robot�� ��� ���� ����(TM, ATM) : Robot �յڷ� input, ��������
			if (m_pModule[i]->m_eModuleType == TYPE_ATMROBOT || m_pModule[i]->m_eModuleType == TYPE_VACROBOT)
			{
				for (int j = 0; j < m_pModule.size(); j++)
				{
					if (m_pModule[i]->m_nCol - 1 == m_pModule[j]->m_nCol)
						m_vPickModules.push_back(m_pModule[j]);

					else if (m_pModule[i]->m_nCol + 1 == m_pModule[j]->m_nCol)
						m_vPlaceModules.push_back(m_pModule[j]);
				}

				m_pModule[i]->Run(m_vPickModules, m_vPlaceModules, pListCtrl);

				m_vPickModules.clear();
				m_vPlaceModules.clear();
			}

			//LPM, LL, PM�� ��� ���� ����
			else
				m_pModule[i]->Run(m_vPickModules, m_vPlaceModules, pListCtrl);
		}

		// �׷��� ����
		if(bRunToClear == false) RunGraph();
		
		return true;
	}

	//�̹� ������ �����ϰ� �ִ� ��� �Ͻ�����
	else
	{
		for (int i = 0; i < m_pModule.size(); i++)
		{
			m_pModule[i]->Resume();
		}

		for (int i = 0; i < 3; i++)
		{
			ResumeThread(s_hMoniteringThread[i]);
		}
	}
}

void CFabController::SuspendModules(bool bExceptTh3)
{
	for (int i = 0; i < m_pModule.size(); i++)
	{
		m_pModule[i]->Suspend();
	}

	for (int i = 0; i < 3; i++)
	{
		if (bExceptTh3 == true && i == 2)
			break;

		SuspendThread(s_hMoniteringThread[i]);
	}
}

#pragma endregion