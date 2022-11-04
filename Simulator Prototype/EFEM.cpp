#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "Simulator PrototypeDlg.h"
#include "resource.h"

#pragma region LPM

#pragma region ������/�Ҹ���

int LPM::s_nTotalInitWafer = 0;
int LPM::s_nTotalSendWafer = 0;
int LPM::s_nTotalUsedDummyWafer = 0;
bool LPM::s_bLPMWaferPickBlock = false;
int LPM::s_nCount = 0;

vector<LPM*> LPM::s_pLPM;

LPM::LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	s_nTotalInitWafer += m_nWaferCount;
	m_nOutputWaferCount = 0;

	// �θ� ������ ���� ȣ�� �ǰ� ���߿� ȣ��ǹǷ� WaferMax�� ���Ե�
	m_nInputWafer = _WaferMax;

	if(m_strModuleName != _T("DummyStage"))
		s_nCount++;

	s_pLPM.push_back(this);
}

LPM::~LPM()
{
	if (m_strModuleName != _T("DummyStage"))
		s_nCount--;
	//m_th.~thread();

	for (int i = 0; i < s_pLPM.size(); i++)
	{
		if (s_pLPM[i] == this)
		{
			s_pLPM.erase(s_pLPM.begin() + i);
			break;
		}
	}
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

void LPM::Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist) //LL <--> EFEM
{
	m_pClistCtrl = pClist;
	m_th = thread(&LPM::WorkThread, this);
}

void LPM::WorkThread()
{
	CString tmp = _T("");

	//DUMMYSTAGE�� ���
	if (m_strModuleName.Compare(_T("DummyStage")) == 0)
	{
		m_nWaferCount = 12;
		tmp.Format(_T("%s\n(12)"), m_strModuleName);
		m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

		while (m_bStopFlag == false)
		{
			Sleep(1 / s_dSpeed);

			if (m_nWaferCount == 0 && ATMRobot::s_nRequiredDummyWaferCntLpmToPM > 0)
			{
				m_nWaferCount = 12;
				tmp.Format(_T("%s\n<Reload>"), m_strModuleName);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
			}

			else if(m_nWaferCount == 12 && ATMRobot::s_nRequiredDummyWaferCntPMToLpm > 0)
			{
				m_nWaferCount = 0;
				tmp.Format(_T("%s\n(12)"), m_strModuleName);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
			}
		}
	}

	//LPM�� ���
	else
	{
		while (m_bStopFlag == false)
		{
			Sleep(1 / s_dSpeed);

			if (m_nWaferCount == 0 && m_nOutputWaferCount == m_nWaferMax)
			{
				tmp.Format(_T("%s\n<Reload>"), m_strModuleName);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				m_nWaferCount = m_nWaferMax;
				m_nOutputWaferCount = 0;
				
				// CSV ���� ����, UI ��� �뵵 (������ �����, ���� �ʿ�) ������ - �����Ǿ����
				m_nInputWafer = 0;
			}
		}
	}
	
	SetEvent(m_hThreadCloseSignal);
}
#pragma endregion

#pragma region ATMRobot
HANDLE ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange = CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE ATMRobot::s_hEventSendWaferChange = CreateEvent(NULL, TRUE, TRUE, NULL);
vector<ATMRobot*> ATMRobot::s_pATMRobot;

int ATMRobot::s_nTotalWaferCntFromLPM = 0;
int ATMRobot::s_nRequiredDummyWaferCntLpmToPM = 0;
int ATMRobot::s_nRequiredDummyWaferCntPMToLpm = 0;
int ATMRobot::s_nCount = 0;

#pragma region ������/�Ҹ���
ATMRobot::ATMRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime, int RoteteZTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPickTime = _PickTime;
	m_nPlaceTime = _PlaceTime;
	m_nRotateTime = _RotateTime;
	m_nRotateZCoordinateTime = RoteteZTime;
	m_bIsInputWafer = true;

	m_nDummyWaferReminder = 0;
	s_nCount++;
	SetEvent(ATMRobot::s_hEventSendWaferChange);

	s_pATMRobot.push_back(this);
}

ATMRobot::~ATMRobot()
{
	s_nCount--;

	for (int i = 0; i < s_pATMRobot.size(); i++)
	{
		if (s_pATMRobot[i] == this)
		{
			s_pATMRobot.erase(s_pATMRobot.begin() + i);
			break;
		}
	}
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

bool ATMRobot::PickWafer(ModuleBase* pM)
{
	CString tmp = _T("");

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
		//Clean Time ���� �ۼ�
		if (pM->GetModuleName().Compare(_T("DummyStage")) == 0 
			|| (pM->m_eModuleType == TYPE_LOADLOCK && LoadLock::s_nRequiredDummyWaferCntPMToLpm > 0))
			s_bIsCleaning = true;
		else if(pM->m_eModuleType == TYPE_LPM) 
			s_bIsCleaning = false;
		//
		m_bIsWorking = true;
		WaitForSingleObject(pM->m_hMutex, INFINITE);

		Sleep(m_nRotateZCoordinateTime / s_dSpeed);
		Sleep(m_nRotateTime / s_dSpeed);
		Sleep(m_nPickTime / s_dSpeed);


		if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
		{
			bool bCheck = SetWaferCount(m_nWaferCount + 1);

			if (bCheck == true && pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				ATMRobot::s_nRequiredDummyWaferCntLpmToPM--;
			}

			if (pM->m_eModuleType == TYPE_LPM || pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				LPM* pLPM = (LPM*)pM;

				// Throughtput ���ϱ� ���� �߰� (���� �����丵 �ʿ�) =====================================================
				CSimulatorPrototypeDlg* pMainDlg = ((CSimulatorPrototypeDlg*)AfxGetApp()->GetMainWnd());

				if (pM->GetModuleName().Compare(_T("DummyStage")) != 0)
				{
					s_nTotalInputWafer++;
					tmp.Format(_T("%d"), s_nTotalInputWafer);
					pMainDlg->m_pFormInfo->m_nFabInputCnt = s_nTotalInputWafer;
					pMainDlg->m_pFormInfo->GetDlgItem(IDC_STATIC_FAB_INPUT_VALUE)->SetWindowText(tmp);

					tmp.Format(_T("Input\n(%d)"), LPM::s_nTotalInputWafer);
					m_pClistCtrl->SetItemText(2, 1, tmp);
				}

				// ��� ������ Thruput ���ϱ� ����
				pM->m_nOutputWafer++;				// DummyStage or LPM Output ++
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
					tmp.Format(_T("Aligner\n<Exchange>\n(%d)"), 1);
					m_pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n<Exchange>\n(%d)"), m_strModuleName, m_nWaferCount - 1);
					m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);


					Sleep(max(m_nPickTime, m_nPlaceTime) / ModuleBase::s_dSpeed);

					tmp = _T("");
					tmp.Format(_T("Aligner\n(%d)"), 0);
					m_pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n<Exchange>\n(%d)"), m_strModuleName, m_nWaferCount);
					m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
				}

				//GUI�� �����

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount - 1);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(��:%d)\n(��:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
				m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);

				m_bIsWorking = false;
				ReleaseMutex(pM->m_hMutex);
				return true;
			}

			//GUI�� �����
			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* pLL = (LoadLock*)pM;

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				m_pClistCtrl->SetItemText(m_nRow, AXIS - m_nCol, tmp);


				if (LoadLock::s_nRequiredDummyWaferCntPMToLpm > 0)
				{
					LoadLock::s_nRequiredDummyWaferCntPMToLpm--;
					tmp = _T("");
					tmp.Format(_T("%s\n(0)\n(Dum:%d)"),pM->GetModuleName(),pM->GetWaferCount());
				}

				else
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());

				m_pClistCtrl->SetItemText(pM->m_nRow, AXIS - pM->m_nCol, tmp);

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

bool ATMRobot::PlaceWafer(ModuleBase* pM)
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

				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				m_pClistCtrl->SetItemText(m_nRow, AXIS - m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(��:%d)\n(��:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
				m_pClistCtrl->SetItemText(pM->m_nRow, AXIS - pM->m_nCol, tmp);

				tmp.Format(_T("Output\n(%d)"), s_nTotalOutputWafer);
				m_pClistCtrl->SetItemText(2, 11, tmp);

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

		Sleep(m_nRotateZCoordinateTime / s_dSpeed);
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
				LPM::s_nTotalUsedDummyWafer++;
				ATMRobot::s_nRequiredDummyWaferCntPMToLpm--;
				
				if (ATMRobot::s_nRequiredDummyWaferCntPMToLpm < 0)
					int a = 0;

				SetEvent(ATMRobot::s_hEventOutputWaferAndUsedDummyWaferChange);

				s_bIsCleaning = false;
			}

			//GUI�� �����
			CString tmp;
			if (pM->m_eModuleType != TYPE_LPM || pM->GetModuleName().Compare(_T("DummyStage")) == 0)
			{
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
			
				if (pM->m_eModuleType == TYPE_LOADLOCK)
				{
					if (LoadLock::s_nRequiredDummyWaferCntLpmToPM > 0)
					{

						tmp.Format(_T("%s\n(-)\n(Dum:%d)"), pM->GetModuleName(), pM->GetWaferCount());
						LoadLock::s_nRequiredDummyWaferCntLpmToPM--;

						//if (m_nDummyWaferReminder > 0 && pM->GetWaferCount() > m_nDummyWaferReminder)
						//{
						//	tmp = _T("");
						//	tmp.Format(_T("%s\n(%d)\n(Dum:%d)"), pM->GetModuleName(), pM->GetWaferCount() - m_nDummyWaferReminder, m_nDummyWaferReminder);
						//}
						//else
						//{
						//	tmp = _T("");
						//	tmp.Format(_T("%s\n(0)\n(Dum:%d)"), pM->GetModuleName(), pM->GetWaferCount());
						//}
					}
					else
					{
						//tmp = _T("");
						//tmp = m_pClistCtrl->GetItemText(pM->m_nRow, pM->m_nCol);

						//if (int nIndex = tmp.Find(_T("\n(-)\n(Dum:")) > 0)
						//{
						//	CString tmp2 = tmp.Right(2);
						//	tmp2.Trim(_T(")"));

						//	if (_ttoi(tmp2) != 0)
						//	{
						//		m_nDummyWaferReminder = _ttoi(tmp2);
						//		
						// (tmp2);
						//	}

						//	tmp.Format(_T("%s\n(%d)\n(Dum:%d)"), pM->GetModuleName(), pM->GetWaferCount() - m_nDummyWaferReminder, m_nDummyWaferReminder);
						//}

						//else
						//{
							tmp = _T("");
							tmp.Format(_T("%s\n(%d)\n(Dum:-)"), pM->GetModuleName(), pM->GetWaferCount());
						//}
					}

					m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}

				else
				{
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());

					m_pClistCtrl->SetItemText(pM->m_nRow, AXIS - pM->m_nCol, tmp);
				}

			}

			else
			{
				if (s_bDirect == false)
				{
					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}
				else
				{

					m_pClistCtrl->SetItemText(pM->m_nRow, AXIS - pM->m_nCol, tmp);
				}
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

void ATMRobot::WorkThread()
{
	int k = 1;
	int l = 0;
	
	int m = 1;
	int n = 0;

	ModuleBase* pM = NULL;

	while (m_bStopFlag == false)
	{
		Sleep(1 / s_dSpeed);

		//�������� ���(Pick = LPM, Place = LL)
		if (s_bDirect == false)
		{
			//Pick�ϴ� ���
			//DummyWafer�� ������ �ϴ� ���
			if (ATMRobot::s_nRequiredDummyWaferCntLpmToPM > 0)
			{
				PickWafer(m_vLPMModule[0]);
			}

			//DummyWafer�� ������ �ʾƵ� ������ ���
			else
			{
				pM = m_vLPMModule[k];
				bool bCheck = PickWafer(pM);

				if (bCheck == false && m_vLPMModule[k]->GetModuleName().Compare(_T("DummyStage")) == 0 ||
					pM->GetWaferCount() == 0)
				{
					k++;
					if (k == m_vLPMModule.size())
						k = 1;
				}
			}

			//Place�ϴ� ���
			pM = m_vLLModule[l];

			PlaceWafer(pM);

			if (pM->GetWaferCount() == pM->GetWaferMax())
			{
				l++;
				if (l == m_vLLModule.size())
					l = 0;
			}
		}

		//�������� ���(Pick - LL, Place = LPM)
		else
		{
			//Pick�ϴ� ���
			pM = m_vLLModule[n];

			PickWafer(pM);

			if (pM->GetWaferCount() == 0)
			{
				n++;
				if (n == m_vLLModule.size())
					n = 0;
			}

			//Place�ϴ� ���
			//DummyWafer�� �ڸ��� �ξ���ϴ� ���
			if (ATMRobot::s_nRequiredDummyWaferCntPMToLpm > 0)
			{
				PlaceWafer(m_vLPMModule[0]);
			}

			//DummyWafer�� ���� �ʴ� ���
			else
			{
				LPM* pLPM = (LPM*)m_vLPMModule[m];
				bool bCheck = PlaceWafer(pLPM);

				if (bCheck == false && m_vLPMModule[m]->GetModuleName().Compare(_T("DummyStage")) == 0 ||
					pLPM->GetOutputWaferCount() == pLPM->GetWaferMax() ||
					pLPM->GetWaferCount() == pLPM->GetWaferMax())
				{
					m++;

					if (m == m_vLPMModule.size())
						m = 1;
				}
			}
		}
	}

	SetEvent(m_hThreadCloseSignal);
}

void ATMRobot::Run(vector<ModuleBase*> _vPickModules, vector<ModuleBase*> _vPlaceModules, CListCtrl* _pClistCtrl)
{
	m_vLPMModule = _vPickModules;
	m_vLLModule = _vPlaceModules;
	m_pClistCtrl = _pClistCtrl;

	m_th = thread(&ATMRobot::WorkThread, this);
}
#pragma endregion

#pragma endregion