#pragma once

#include "pch.h"
#include "CFabController.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "resource.h"

vector<LPM*> CFabController::s_pLPM;
vector<ATMRobot*> CFabController::s_pATMRobot;
vector<LoadLock*> CFabController::s_pLL;
vector<VACRobot*> CFabController::s_pVACRobot;
vector<ProcessChamber*> CFabController::s_pPM;

HANDLE CFabController::s_hMoniteringThread1;
HANDLE CFabController::s_hMoniteringThread2;


#pragma region 생성자/소멸자

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
	m_nCurModuleIdx = -1;
}

CFabController::~CFabController()
{
}

#pragma endregion

#pragma region 메서드

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

void CFabController::DrawModule(bool bEmptyFlag /*=false*/)
{
	if (bEmptyFlag == true)
	{
		//for (int i = 0; i < (int)m_pModule.size(); i++)
		//{
		//	m_pMainDlg->m_ctrlListFabInfo.SetItemText(m_pModule[i]->m_nRow, m_pModule[i]->m_nCol, _T(""));
		//}

		//UI가 확정되지 않았기에, 일단 확정될 때까지는 일단 모두 날려줌
		for (int i = 0; i < 20; i++)
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

	// 모듈 Row, Col 값
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

		// 설정시간 값
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
		
		// 설정 시간 값
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

		// 설정시간 값
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

	// 추가 삭제 필요 (모니터링 스레드에서 사용되는? >> 협의 필요)
}

DWORD WINAPI ClearAllModuleWorkThread(LPVOID p)
{
	CFabController* pCFabController = (CFabController*)p;

	int nTmpSpeed = ModuleBase::s_dSpeed;

	ModuleBase::s_dSpeed = 1000;
	pCFabController->RunModules();
	//최초로 동작하는 경우 시작
// 각 모듈의 스레드를 종료시키고 해제해야함 (모듈 필드 BOOL m_bRunning 추가 / 그 후 모듈 스레드의 While문에 m_bRunning 조건문 추가)
// 각 모듈의 작업자 스레드 함수 정상 종료시키기가 목표
	//Sleep(1);

	//1. 전체 종료신호 보내기
	//모듈들
	for (int i = 0; i < pCFabController->m_pModule.size(); i++)
	{
		pCFabController->m_pModule[i]->m_bStopFlag = true;
	}

	//Thread
	pCFabController->m_bRunning = FALSE;

	//2. MonitoringThreads, LPM, ATM, VAC 종료
	SetEvent(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);
	SetEvent(ATMRobot::s_hEventSendWaferChange);


	//2. 동기화된 모듈들은 종료가 가능하게끔 신호 보냄(LL, PM)

	for (int i = 0; i < pCFabController->s_pLL.size(); i++)
	{
		SetEvent(pCFabController->s_pLL[i]->m_hLLWaferCntChangeEvent);
	}

	for (int i = 0; i < pCFabController->s_pPM.size(); i++)
	{
		SetEvent(pCFabController->s_pPM[i]->m_hPmWaferCntChangeEvent);
		SetEvent(pCFabController->s_pPM[i]->m_hInitWorkOver);
	}

	AfxMessageBox(_T("모든 모듈들이 안전하게 종료될 때까지 대기하는 중입니다.\n[종료완료] 메세지가 나타날 때까지 잠시만 기다려 주세요."));
	//3. 모든 모듈들의 종료신호를 받을 때까지 대기함
	if (WaitForMultipleObjects(ModuleBase::s_vEventCloseThread.size(), ModuleBase::s_vEventCloseThread.data(), TRUE, 10000)
		!= WAIT_OBJECT_0)
		AfxMessageBox(_T("대기시간이 오래 걸릴 예정입니다.\n프로그램을 수동으로 종료하시는 것을 추천드립니다."));

		//AfxMessageBox(_T("모든 Thread가 정상적으로 종료됨"));
	
	Sleep(1);

	//4. GUI상 모듈을 지워줌
	pCFabController->DrawModule(true);

	for (int i = 0; i < pCFabController->m_pModule.size(); i++)
	{
		delete pCFabController->m_pModule[i];
	}

	//5. static 변수들 초기화
	ModuleBase::s_bDirect = false;
	ModuleBase::m_dTotalProcessTime = 0.0;
	ModuleBase::m_dTotalCleanTime = 0.0;
	ModuleBase::m_dTotalThroughput = 0.0;
	ModuleBase::s_dSpeed = nTmpSpeed;
	ModuleBase::s_nTotalOutputWafer = 0;
	ModuleBase::s_nTotalInputWafer = 0;

	LPM::s_nTotalSendWafer = 0;
	LPM::s_nTotalInitWafer = 0;
	LPM::s_nTotalUsedDummyWafer = 0;
	LPM::s_bLPMWaferPickBlock = false;

	CloseHandle(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);
	ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange = CreateEvent(NULL, FALSE, TRUE, NULL);

	CloseHandle(ATMRobot::s_hEventSendWaferChange);
	ATMRobot::s_hEventSendWaferChange = CreateEvent(NULL, TRUE, TRUE, NULL);

	ATMRobot::s_nTotalWaferCntFromLPM = 0;
	ATMRobot::s_nRequiredDummyWaferCntLpmToPM = 0;
	ATMRobot::s_nRequiredDummyWaferCntPMToLpm = 0;

	LoadLock::s_nTotalSendWaferFromLL = 0;
	LoadLock::s_nRequiredDummyWaferCntLpmToPM = 0;
	LoadLock::s_nRequiredDummyWaferCntPMToLpm = 0;
	
	ProcessChamber::s_nCntPMWorkOver = 0;
	ProcessChamber::s_vWorkOverHandle.clear();
	
	CFabController::s_hMoniteringThread1 = NULL;
	CFabController::s_hMoniteringThread2 = NULL;

	//5. vector 변수 초기화 및 비우기

	pCFabController->s_pLPM.clear();
	pCFabController->s_pLL.clear();
	pCFabController->s_pATMRobot.clear();
	pCFabController->s_pVACRobot.clear();
	pCFabController->s_pPM.clear();

	pCFabController->m_vPickModules.clear();
	pCFabController->m_vPlaceModules.clear();

	pCFabController->s_hMoniteringThread1 = NULL;
	pCFabController->s_hMoniteringThread2 = NULL;

	pCFabController->m_pModule.clear();

	pCFabController->m_pMainDlg->SetWindowText(_T("주성 Fab Simulator"));

	// FormInfo 초기화
	pCFabController->m_pMainDlg->m_pFormInfo->InitializeFormInfo();
	pCFabController->m_pMainDlg->m_pFormInfo->ShowWindow(SW_SHOW);
	pCFabController->m_pMainDlg->m_pFormTimeInfoATM->ShowWindow(SW_HIDE);
	pCFabController->m_pMainDlg->m_pFormTimeInfoLL->ShowWindow(SW_HIDE);
	pCFabController->m_pMainDlg->m_pFormTimeInfoVAC->ShowWindow(SW_HIDE);
	pCFabController->m_pMainDlg->m_pFormTimeInfoPM->ShowWindow(SW_HIDE);
	pCFabController->m_pMainDlg->m_ctrlInfoTab.SetCurSel(0);

	AfxMessageBox(_T("종료완료"));

	return true;
}

// 모듈 전체 삭제
thread CFabController::ClearAllModule()
{
	thread th = thread(ClearAllModuleWorkThread, this);

	return th;
}

// Info에 모듈 정보 출력
void CFabController::PrintModuleInfo(int nModuleIdx, int nModuleType, int nCurSel)
{
	CString strModuleType;
	CString strWaferMax;

	switch ((ModuleType)nModuleType)
	{
	case TYPE_LPM:
	{
		if (nCurSel != 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
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

		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
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

		else if (nCurSel == 1) // MainDlg의 InfoTab의 Index가 1일 시 (Time Info가 골라져 있을 시)
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

		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
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

		else if (nCurSel == 1) // MainDlg의 InfoTab의 Index가 1일 시 (Time Info가 골라져 있을 시)
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

		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
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

		else if (nCurSel == 1) // MainDlg의 InfoTab의 Index가 1일 시 (Time Info가 골라져 있을 시)
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
		
		if (nCurSel == 0) // MainDlg의 InfoTab의 Index가 0일 시 (Informations가 골라져 있을 시)
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

// 모듈 전체 Info 설정
void CFabController::SetFabInfo(int nHour, int nMin, int nSec)
{
	// Throughput을 구하기 위해 시간을 double로 변환 (초, 분을 시로 변환)
	ModuleBase::m_dTotalProcessTime = (double)nHour + ((double)nMin / (double)60) + ((double)nSec / (double)3600);

	ModuleBase::SetTotalThroughput();
	
	CString tmp;
	tmp.Format(_T("%.2lf"), ModuleBase::m_dTotalThroughput);

	m_pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_THROUGHPUT_VALUE)->SetWindowText(tmp);
}

// 모듈 유닛 Info 설정
void CFabController::SetUnitInfo(int nModuleIdx)
{
	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pModule[i]->SetThroughput();
	}

	//m_pModule[nModuleIdx]->SetThroughput();

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

// ConfigFile 저장
void CFabController::SaveConfigFile(CString strFilePath)
{
	//INI 파일에 정보 쓰기
	int nModuleSize = (int)m_pModule.size();
	CString strSize;
	strSize.Format(_T("%d\n"), nModuleSize);
	
	WritePrivateProfileString(_T("Common"),//섹션명 
		_T("ModuleCount"), //키명
		strSize, //키 값
		strFilePath);

	for (int i = 0; i < nModuleSize; i++)
	{	
		m_pModule[i]->SaveConfigModule(i, strFilePath);
	}
}

// ConfigFile 불러오기
void CFabController::LoadConfigFile(CString strFilePath)
{
	// Common 섹션 먼저 읽기 (ModuleCount)
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

// CSVFile 저장
void CFabController::SaveCSVFile(CString strFilePath)
{
	CStdioFile cFile;	// Text 모드 저장 시 편의성을 위해 CFile 자식 클래스인 CStdioFile 사용 결정
	CFileException cException;	// 실패한 작업의 상태를 수신할 기존 파일 예외 개체에 대한 포인터

	// CStdioFile Open 실패 시 예외처리 (ex에 오류 상태를 수신 후 출력)
	if (!cFile.Open(strFilePath, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate, &cException))
	{
		TCHAR szError[1024];
		cException.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("파일 열기 불가 : %1024s"), szError);

		return;
	}
	
	CTime cTime = CTime::GetCurrentTime();	// 현재 시간 가져옴
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

// Speed 변경
void CFabController::ChangeTimeSpeed(int nCurSpeed)
{
	switch (nCurSpeed)
	{
	case 0:
		ModuleBase::s_dSpeed = 0.01;	// 0.001 == 1배속
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

// 그래프 시작
void CFabController::RunGraph()
{
	// 기존 보여주기 위한 그래프 삭제
	delete m_pMainDlg->m_ctrlGraph;

	// 그래프 ================================================================
	m_pMainDlg->GetDlgItem(IDC_STATIC_RT_GRAPH)->GetWindowRect(m_pMainDlg->m_rtGraph);

	m_pMainDlg->ScreenToClient(m_pMainDlg->m_rtGraph);

	m_pMainDlg->m_ctrlGraph = new COScopeCtrl(((int)m_pModule.size() + 1));
	m_pMainDlg->m_ctrlGraph->Create(WS_VISIBLE | WS_CHILD, m_pMainDlg->m_rtGraph, m_pMainDlg, IDC_STATIC_RT_GRAPH);
	
	m_pMainDlg->m_ctrlGraph->SetRange(0., 300., (int)m_pModule.size()+1);
	m_pMainDlg->m_ctrlGraph->autofitYscale = true;
	m_pMainDlg->m_ctrlGraph->SetYUnits(_T("Throughput"));
	m_pMainDlg->m_ctrlGraph->SetXUnits(_T("Time"));

	m_pMainDlg->m_ctrlGraph->SetLegendLabel(_T("Total"), 0);

	for (int i = 0; i < (int)m_pModule.size(); i++)
	{
		m_pMainDlg->m_ctrlGraph->SetLegendLabel(m_pModule[i]->GetModuleName(), i+1);
	}	
	//m_pMainDlg->m_ctrlGraph->InvalidateCtrl();
}

//////////////////////////////////////////////////////////
//중앙감시 thread : 동작과정을 모니터링하고 제어해주는 thread
//////////////////////////////////////////////////////////
DWORD WINAPI MoniteringThread1(LPVOID p)
{
	CFabController* pController = (CFabController*)p;

	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();

	int nCheckTotalOutputAndDummyWafer = 0;

	//목적. 모듈의 진행방향을 상황에 맞추어 바꾸어 줌
	while (pController->m_bRunning)
	{
		int nCntTotalPmWafer = 0;
		int nCntTotalLLWafer = 0;

		if (ModuleBase::s_bDirect == true &&
			(LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer > nCheckTotalOutputAndDummyWafer)
			&& (LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer) % nMaxPMSlot == 0)
		{
			ModuleBase::s_bDirect = false;

			nCheckTotalOutputAndDummyWafer = LPM::s_nTotalOutputWafer + LPM::s_nTotalUsedDummyWafer;

			for (int i = 0; i < CFabController::s_pLL.size(); i++)
			{
				LoadLock* pLL = (LoadLock*)CFabController::s_pLL[i];
				SetEvent(pLL->m_hLLWaferCntChangeEvent);
			}

			//Exchange가 끝났다는 조건
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

	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
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

bool CFabController::RunModules()
{	
	bool bError = false;
	
	if (m_pModule.size() == 0)
	{
		AfxMessageBox(_T("설정된 모듈이 없습니다.\n"));
		return false;
	}

	if (m_pModule[1]->IsRunning() == false)		// 사용자가 LPM을 제외한 다른 모듈을 먼저 만들고 실행 가능?
	{
		s_pLPM.clear();
		s_pPM.clear();
		s_pLL.clear();
		s_pVACRobot.clear();
		s_pPM.clear();

		for (int i = 0; i < m_pModule.size(); i++)		// 모듈 타입별 구분
		{
			ModuleBase* pM = m_pModule[i];

			if (pM->m_eModuleType == TYPE_LPM)
				s_pLPM.push_back((LPM*)pM);
			else if (pM->m_eModuleType == TYPE_ATMROBOT)
				s_pATMRobot.push_back((ATMRobot*)pM);
			else if (pM->m_eModuleType == TYPE_LOADLOCK)
				s_pLL.push_back((LoadLock*)pM);
			else if (pM->m_eModuleType == TYPE_VACROBOT)
				s_pVACRobot.push_back((VACRobot*)pM);
			else if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
				s_pPM.push_back((ProcessChamber*)pM);
		}
		// 모든 모듈이 없을 경우
		if (s_pLPM.size() == 1 || s_pATMRobot.size() == 0 || s_pLL.size() == 0 || s_pVACRobot.size() == 0 || s_pPM.size() == 0)
		{
			AfxMessageBox(_T("작동하기 위해 설정된 모듈들이 충분하지 않습니다.\n"));
			return false;
		}
		// LL 총 WaferMax < PM 총 WaferMax 제한
		if (s_pLL.size() * s_pLL[0]->GetWaferMax() != s_pPM.size() * s_pPM[0]->GetWaferMax())
		{
			AfxMessageBox(_T("LL들의 총 wafer 수가 PM들의 총 Wafer 수와 같아야 합니다.\n"));
			return false;
		}
		// LL 총 WaferMax < PM 총 WaferMax 제한
		if (s_pPM.size() > 4 && s_pPM[0]->GetWaferMax() < 2)
		{
			AfxMessageBox(_T("LL들의 총 wafer 수가 PM들의 총 Wafer 수와 같아야 합니다.\n"));
			return false;
		}

		else
		{
			CListCtrl* pListCtrl = (&(m_pMainDlg->m_ctrlListFabInfo));

			m_bRunning = TRUE;

			//중앙감시 thread 생성
			s_hMoniteringThread1 = CreateThread(NULL, NULL, MoniteringThread1, this, NULL, NULL);
			s_hMoniteringThread2 = CreateThread(NULL, NULL, MoniteringThread2, this, NULL, NULL);

			//Process가 이미 진행중이 아닐 때 로직
			for (int i = 0; i < m_pModule.size(); i++)
			{
				//Robot일 경우 동작 로직(TM, ATM) : Robot 앞뒤로 input, 설정해줌
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

				//LPM, LL, PM일 경우 동작 로직
				else
					m_pModule[i]->Run(m_vPickModules, m_vPlaceModules, pListCtrl);
					
			}

			return true;
		}
	}

	//이미 모듈들이 동작하고 있는 경우 일시정지
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