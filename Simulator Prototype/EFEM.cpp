#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "Simulator PrototypeDlg.h"
#include "CFabController.h"
#include "resource.h"

#pragma region LPM

#pragma region ������/�Ҹ���

int LPM::s_nTotalInitWafer = 0;
int LPM::s_nTotalSendWafer = 0;
int LPM::s_nTotalUsedDummyWafer = 0;
bool LPM::s_bLPMWaferPickBlock = false;

LPM::LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	s_nTotalInitWafer += m_nWaferCount;
	m_nOutputWaferCount = 0;

	// �θ� ������ ���� ȣ�� �ǰ� ���߿� ȣ��ǹǷ� WaferMax�� ���Ե�
	m_nInputWafer = _WaferMax;
}

LPM::~LPM()
{
	//m_th.~thread();
}

#pragma endregion	

bool LPM::SetOutputWaferCount(int _value)
{
	if (m_nOutputWaferCount > m_nWaferMax || _value > m_nWaferMax || _value < 0) // WaferMax ��ġ ����
	{
		return false;
	}
	
	else
	{
		m_nOutputWaferCount = _value;
		return true;
	}
}

int LPM::GetOutputWaferCount() const
{
	return m_nOutputWaferCount;
}

void LPM::SaveConfigModule(int nIdx, CString strFilePath)
{
	// int To CString (���߿� Convert �޼��� �߰� �����)
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d\n"), m_nWaferMax);
	
	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_LPM"), strFilePath);				// Ÿ��
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// ����
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// �÷�
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// �ο�
	WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax
}

void LPM::Run() //LL <--> EFEM
{
	m_th = thread(&LPM::WorkThread, this);
}

void LPM::WorkThread()
{
	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();

	//DUMMYSTAGE�� ���
	if (m_strModuleName.Compare(_T("DummyStage")) == 0)
	{
		m_nWaferCount = 12;
		//while (m_bStopFlag == false)
		//{
		//	Sleep(1 / ModuleBase::s_dSpeed);
		//}
		SetEvent(m_hThreadCloseSignal);
	}

	//LPM�� ���
	else
	{
		while (m_bStopFlag == false)
		{
			//WaitForSingleObject(ATMRobot::s_hEventSendWaferChange, INFINITE);
			//ResetEvent(ATMRobot::s_hEventSendWaferChange);

			Sleep(1 / ModuleBase::s_dSpeed);

			if (m_nWaferCount == 0 && m_nOutputWaferCount == m_nWaferMax)
				//&& (LPM::s_nTotalSendWafer > 0 && (LPM::s_nTotalSendWafer) % s_nTotalInitWafer == 0))
			{
				m_nWaferCount = m_nWaferMax;
				m_nOutputWaferCount = 0;
				
				// CSV ���� ����, UI ��� �뵵 (������ �����, ���� �ʿ�) ������ - �����Ǿ����
				m_nInputWafer = 0;
			}
		}

		SetEvent(m_hThreadCloseSignal);
	}
}
#pragma endregion

#pragma region ATMRobot
HANDLE ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange = CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE ATMRobot::s_hEventSendWaferChange = CreateEvent(NULL, TRUE, TRUE, NULL);

int ATMRobot::s_nTotalWaferCntFromLPM = 0;
int ATMRobot::s_nRequiredDummyWaferCntLpmToPM = 0;
int ATMRobot::s_nRequiredDummyWaferCntPMToLpm = 0;

#pragma region ������/�Ҹ���
ATMRobot::ATMRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime, int RoteteZTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPickTime = _PickTime;
	m_nPlaceTime = _PlaceTime;
	m_nRotateTime = _RotateTime;
	m_nRotateZCoordinateTime = RoteteZTime;
	m_bIsInputWafer = true;

	SetEvent(ATMRobot::s_hEventSendWaferChange);
}

ATMRobot::~ATMRobot()
{
}
#pragma endregion

#pragma region Get/Set �޼���
void ATMRobot::SetPickTime(int _value)
{
	m_nPickTime = _value;
}
int ATMRobot::GetPickTime() const
{
	return m_nPickTime;
}
void ATMRobot::SetPlaceTime(int _value)
{
	m_nPlaceTime = _value;
}
int ATMRobot::GetPlaceTime() const
{
	return m_nPlaceTime;
}
void ATMRobot::SetRotateTime(int _value)
{
	m_nRotateTime = _value;
}
int ATMRobot::GetRotateTime() const
{
	return m_nRotateTime;
}
void ATMRobot::SetRotateZTime(int _value)
{
	m_nRotateZCoordinateTime = _value;
}
int ATMRobot::GetRotateZTime() const
{
	return m_nRotateZCoordinateTime;
}
void ATMRobot::SetIsInputWafer(bool _value)
{
	m_bIsInputWafer = _value;
}
bool ATMRobot::GetIsInputWafer() const
{
	return m_bIsInputWafer;
}
#pragma endregion

#pragma region �޼���
void ATMRobot::SaveConfigModule(int nIdx, CString strFilePath)
{
	// �⺻ ModuleBase�� �ʵ�
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// ATM Robot �߰� �ʵ�
	CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
	strPickTime.Format(_T("%d"), m_nPickTime);
	strPlaceTime.Format(_T("%d"), m_nPlaceTime);
	strRotateTime.Format(_T("%d"), m_nRotateTime);
	strZRotatetime.Format(_T("%d\n"), m_nRotateZCoordinateTime);

	strArmMode.Format(_T("DualArm"));		// << ���� ������ �� ������ ���� (Robot Arm ǥ��)

	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_ATMROBOT"), strFilePath);			// Ÿ��
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// ����
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// �÷�
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// �ο�
	WritePrivateProfileString(strIdx, _T("ArmMode"), strArmMode, strFilePath);						// ArmMode(WaferMax)
	WritePrivateProfileString(strIdx, _T("PickTime"), strPickTime, strFilePath);					// PickTime
	WritePrivateProfileString(strIdx, _T("PlaceTime"), strPlaceTime, strFilePath);					// PlaceTime
	WritePrivateProfileString(strIdx, _T("StationMoveTime"), strRotateTime, strFilePath);			// RotateTime
	WritePrivateProfileString(strIdx, _T("Z-MoveTime"), strZRotatetime, strFilePath);				// ZRotateTime
}

bool ATMRobot::PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	//1. Pick���� ���ϴ� ���� �� ����
	//�������� ���(Pick = LPM)
	if (ModuleBase::s_bDirect == false)
	{
		if (LPM::s_bLPMWaferPickBlock == true)
			return false;

		if (pM->GetModuleName().Compare(_T("DummyStage")) == 0
		&& ATMRobot::s_nRequiredDummyWaferCntLpmToPM == 0)
		{
			return false;
		}
	}

	//�������� ���(Pick = LL)
	else
	{

	}

	//2. Pick����
	while (pM->GetIsWorking() == false && pM->GetWaferCount() > 0 && m_nWaferCount < m_nWaferMax)
	{
		m_bIsWorking = true;
		WaitForSingleObject(pM->m_hMutex, INFINITE);

		Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
		Sleep(m_nPickTime / ModuleBase::s_dSpeed);


		if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
		{
			bool bCheck = SetWaferCount(m_nWaferCount + 1);

			if (bCheck == true && pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				//ModuleBase::m_dTotalCleanTime += (m_nRotateTime + m_nPlaceTime) / ModuleBase::s_dSpeed;
				ATMRobot::s_nRequiredDummyWaferCntLpmToPM--;
			}

			if (pM->m_eModuleType == TYPE_LPM || pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				LPM* pLPM = (LPM*)pM;
				CString tmp = _T("");

				// Throughtput ���ϱ� ���� �߰� (���� �����丵 �ʿ�) =====================================================
				CSimulatorPrototypeDlg* pMainDlg = ((CSimulatorPrototypeDlg*)AfxGetApp()->GetMainWnd());

				if (pM->GetModuleName().Compare(_T("DummyStage")) != 0)
				{
					s_nTotalInputWafer++;
					tmp.Format(_T("%d"), s_nTotalInputWafer);
					pMainDlg->m_pFormInfo->m_nFabInputCnt = s_nTotalInputWafer;
					pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_INPUT_VALUE)->SetWindowText(tmp);

					tmp.Format(_T("Input:\n(%d)"), LPM::s_nTotalInputWafer);
					pClistCtrl->SetItemText(2, 1, tmp);
				}
				// ��� ������ Thruput ���ϱ� ����
				pM->m_nOutputWafer++;				// LPM Output ++
				this->m_nInputWafer++;				// ATM input ++
				
				// ATM Aligner, Dummy ����� ��� x

				//tmp.Format(_T("%d"), m_nOutputWafer);
				//pMainDlg->m_pFormInfo->m_nFabInputCnt = s_nTotalInputWafer;
				//pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_INPUT_VALUE)->SetWindowText(tmp);
				// =====================================================
				

				LPM::s_nTotalSendWafer++;	// << ���� ModuleBase�� s_nTotalInputWafer�� ��ġ�� ����?
				SetEvent(ATMRobot::s_hEventSendWaferChange);
				
				//Aligner�� Exchange
				if (pM->m_eModuleType == TYPE_LPM)
				{
					Sleep(m_nRotateTime / ModuleBase::s_dSpeed);

					//GUI�� �����
					tmp = _T("");
					tmp.Format(_T("Aligner\n(ExChange)\n(%d)"), 1);
					pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount - 1);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);


					Sleep(max(m_nPickTime, m_nPlaceTime) / ModuleBase::s_dSpeed);

					tmp = _T("");
					tmp.Format(_T("Aligner\n(ExChange)\n(%d)"), 0);
					pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
				}

				//GUI�� �����

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount - 1);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(��:%d)\n(��:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
				pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);

				//if (pM->GetWaferCount() == 0)
				//{
				//	int axis = CFabController::s_pPM[0]->m_nCol;

				//	tmp = _T("");
				//	tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				//	pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
				//}

				m_bIsWorking = false;
				ReleaseMutex(pM->m_hMutex);
				return true;
			}

			//GUI�� �����
				CString tmp;

				//if (s_bDirect == false)
				{
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}

				//else if (s_bDirect == true)
				//{
				//	int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

				//	tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				//	pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

				//	tmp = _T("");
				//	tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				//	pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
				//}

				if (pM->m_eModuleType == TYPE_LOADLOCK)
				{
					LoadLock* p = (LoadLock*)pM;
					SetEvent(p->m_hLLWaferCntChangeEvent);
				}
			///////////////////////////////////////////////////////////////////////////////
			m_bIsWorking = false;
			ReleaseMutex(pM->m_hMutex);
			return true;
		}
	}
	return false;
}

bool ATMRobot::PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{	
	//1. Place���� ���ϴ� ���� �� ����
	//�������� ���(Place = LL)
	if (ModuleBase::s_bDirect == false)
	{
		if (pM->GetDoorValveOpen() == false)
			return false;
	}

	//�������� ���(Place = LPM)
	else if(ModuleBase::s_bDirect == true 
		&& pM->GetModuleName().Compare(_T("DummyStage")) != 0)
	{
		LPM* pLPM = (LPM*)pM;

		if (pLPM->GetIsWorking() == false && m_nWaferCount > 0 && pLPM->GetOutputWaferCount() + pLPM->GetWaferCount() < pLPM->GetWaferMax())
			{
				m_bIsWorking = true;
				WaitForSingleObject(pM->m_hMutex, INFINITE);

				Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
				Sleep(m_nPlaceTime / ModuleBase::s_dSpeed);

				SetWaferCount(m_nWaferCount - 1);
				pLPM->SetOutputWaferCount(pLPM->GetOutputWaferCount() + 1);

				// Throughtput ���ϱ� ���� �߰�==========================
				pM->m_nInputWafer++;
				this->m_nOutputWafer++;
				// =====================================================

				ATMRobot::s_nTotalOutputWafer++;
				SetEvent(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);

				//GUI�� �����
				CString tmp;

				int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(��:%d)\n(��:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
				pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);

				tmp.Format(_T("Output\n(%d)"), s_nTotalOutputWafer);
				pClistCtrl->SetItemText(4, 1, tmp);

				// Throughtput ���ϱ� ���� �߰� (���� �����丵 �ʿ�) =====================================================
				tmp.Format(_T("%d"), s_nTotalOutputWafer);
				((CSimulatorPrototypeDlg*)AfxGetApp()->GetMainWnd())->m_pFormInfo->m_nFabOutputCnt = s_nTotalOutputWafer;
				((CSimulatorPrototypeDlg*)AfxGetApp()->GetMainWnd())->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_OUTPUT_VALUE)->SetWindowText(tmp);
				// =====================================================

				m_bIsWorking = false;
				ReleaseMutex(pM->m_hMutex);

				return true;
			}

		else
			return false;
	
	}

	//2. place����
	while (pM->GetIsWorking() == false && pM->GetWaferCount() < pM->GetWaferMax() && m_nWaferCount > 0)
	{
		m_bIsWorking = true;
		WaitForSingleObject(pM->m_hMutex, INFINITE);

		Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
		Sleep(m_nPlaceTime / ModuleBase::s_dSpeed);

		if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
		{
			bool bCheck = SetWaferCount(m_nWaferCount - 1);

			// Throughtput ���ϱ� ���� �߰�==========================
			pM->m_nInputWafer++;
			this->m_nOutputWafer++;
			// =====================================================

			if (bCheck == true && pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				//ModuleBase::m_dTotalCleanTime += (m_nRotateTime + m_nPlaceTime) / ModuleBase::s_dSpeed;
				//ATMRobot::s_nRequiredDummyWaferCntPMToLpm--;
				LPM::s_nTotalUsedDummyWafer++;
				ATMRobot::s_nRequiredDummyWaferCntPMToLpm--;
				SetEvent(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);
			}

			//if (pM->m_eModuleType == TYPE_LOADLOCK 
			//	&& LoadLock::s_nRequiredDummyWaferCntLpmToPM > 0)
			//{
			//	LoadLock::s_nRequiredDummyWaferCntLpmToPM--;
			//	ModuleBase::m_dTotalCleanTime += (m_nRotateTime + m_nPlaceTime) / ModuleBase::s_dSpeed;
			//}

			//GUI�� �����
			CString tmp;
			if (pM->m_eModuleType != TYPE_LPM || pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				//if (s_bDirect == false)
				{
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);

					//if (pM->GetWaferCount() == pM->GetWaferMax())
					//{
					//	int axis = CFabController::s_pPM[0]->m_nCol;

					//	tmp = _T("");
					//	tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					//	pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
					//}
				}

				//else if (s_bDirect == true)
				//{
				//	int axis = CFabController::s_pPM[0]->m_nCol;

				//	tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				//	pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

				//	tmp = _T("");
				//	tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				//	pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);

				//	if (pM->GetWaferCount() == pM->GetWaferMax())
				//	{
				//		tmp = _T("");
				//		tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				//		pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				//	}
				//}
			}
			/////////////////////////////////////////////////////////////////////////////
			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;

				p->m_nInputWafer++;

				SetEvent(p->m_hLLWaferCntChangeEvent);
			}

		}

		m_bIsWorking = false;
		ReleaseMutex(pM->m_hMutex);
		return true;
	}
}

void ATMRobot::WorkThread(Pick_PlaceM Pick_Place)
{
	vector<ModuleBase*> vLPMModules = Pick_Place.m_vPickModule;
	vector<ModuleBase*> vLLModules = Pick_Place.m_vPlaceModule;

	int k = 0;
	int l = 0;
	
	int m = 0;
	int n = 0;

	CListCtrl* pClistCtrl = Pick_Place.m_pClistCtrl;

	ModuleBase* pM = NULL;

	while (m_bStopFlag == false)
	{
		Sleep(1);

		//�������� ���(Pick = LPM, Place = LL)
		if (s_bDirect == false)
		{

			//Pick�ϴ� ���
			//DummyWafer�� ������ �ϴ� ���
			if (ATMRobot::s_nRequiredDummyWaferCntLpmToPM > 0)
			{
				PickWafer(vLPMModules[0], pClistCtrl);
			}

			//DummyWafer�� ������ �ʾƵ� ������ ���
			else
			{
				pM = vLPMModules[k];
				bool bCheck = PickWafer(pM, pClistCtrl);

				if (bCheck == false && vLPMModules[k]->GetModuleName().Compare(_T("DummyStage")) == 0 ||
					pM->GetWaferCount() == 0)
				{
					k++;
					if (k == vLPMModules.size())
						k = 0;
				}
			}

			//Place�ϴ� ���
			pM = vLLModules[l];
			PlaceWafer(pM, pClistCtrl);

			if (pM->GetWaferCount() == pM->GetWaferMax())
			{
				l++;
				if (l == vLLModules.size())
					l = 0;
			}
		}

		//�������� ���(Pick - LL, Place = LPM)
		else
		{
			//Pick�ϴ� ���
			pM = vLLModules[n];
			bool bCheck1 = PickWafer(pM, pClistCtrl);

			if (bCheck1 == false && vLLModules[n]->GetModuleName().Compare(_T("DummyStage")) == 0 ||
				pM->GetWaferCount() == 0)
			{
				n++;
				if (n == vLLModules.size())
					n = 0;
			}

			//Place�ϴ� ���
			//DummyWafer�� �ڸ��� �ξ���ϴ� ���
			if (ATMRobot::s_nRequiredDummyWaferCntPMToLpm > 0)
			{
				PlaceWafer(vLPMModules[0], pClistCtrl);
			}

			//DummyWafer�� ���� �ʴ� ���
			else
			{
				LPM* pLPM = (LPM*)vLPMModules[m];
				bool bCheck2 = PlaceWafer(pLPM, pClistCtrl);

				if (bCheck2 == false && vLPMModules[m]->GetModuleName().Compare(_T("DummyStage")) == 0 ||
					pLPM->GetOutputWaferCount() == pLPM->GetWaferMax() ||
					pLPM->GetWaferCount() == pLPM->GetWaferMax())
				{
					m++;

					if (m == vLPMModules.size())
						m = 0;
				}
			}
		}
	}

	SetEvent(m_hThreadCloseSignal);
}

void ATMRobot::Run(vector<ModuleBase*> _vPickModules, vector<ModuleBase*> _vPlaceModules, CListCtrl* _pClistCtrl)
{
	Pick_PlaceM pick_place;
	pick_place.m_vPickModule = _vPickModules;
	pick_place.m_vPlaceModule = _vPlaceModules;
	pick_place.m_pClistCtrl = _pClistCtrl;

	m_th = thread(&ATMRobot::WorkThread, this, pick_place);
}
#pragma endregion

#pragma endregion