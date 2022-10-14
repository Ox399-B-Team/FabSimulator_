#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"
#include "CFabController.h"


#pragma region ������/�Ҹ���
VACRobot::VACRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPickTime = _PickTime;
	m_nPlaceTime = _PlaceTime;
	m_nRotateTime = _RotateTime;
}
VACRobot::~VACRobot()
{
}
#pragma endregion

#pragma region Get/Set �޼���
void VACRobot::SetPickTime(int _value)
{
	m_nPickTime = _value;
}

int VACRobot::GetPickTime() const
{
	return m_nPickTime;
}

void VACRobot::SetPlaceTime(int _value)
{
	m_nPlaceTime = _value;
}

int VACRobot::GetPlaceTime() const
{
	return m_nPlaceTime;
}

void VACRobot::SetRotateTime(int _value)
{
	m_nRotateTime = _value;
}

int VACRobot::GetRotateTime() const
{
	return m_nRotateTime;
}
#pragma endregion

#pragma region �޼���
void VACRobot::SaveConfigModule(int nIdx, CString strFilePath)
{
	// �⺻ ModuleBase�� �ʵ�
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// VACRobot �߰� �ʵ�
	CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
	strArmMode.Format(m_nWaferMax == 4 ? _T("QuadArm") : _T("DualArm"));					// WaferMax == 4 �� �� QuadArm | �ƴ� �� DualArm
	strPickTime.Format(_T("%d"), m_nPickTime);
	strPlaceTime.Format(_T("%d"), m_nPlaceTime);
	strRotateTime.Format(_T("%d\n"), m_nRotateTime);


	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_VACROBOT"), strFilePath);			// Ÿ��
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// ����
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// �÷�
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// �ο�
	WritePrivateProfileString(strIdx, _T("ArmMode"), strArmMode, strFilePath);						// ArmMode(WaferMax)
	WritePrivateProfileString(strIdx, _T("PickTime"), strPickTime, strFilePath);					// PickTime
	WritePrivateProfileString(strIdx, _T("PlaceTime"), strPlaceTime, strFilePath);					// PlaceTime
	WritePrivateProfileString(strIdx, _T("RotateTime"), strRotateTime, strFilePath);				// RotateTime
}

bool VACRobot::PickWafer(ModuleBase* pM)
{
	m_bIsWorking = true;
	WaitForSingleObject(pM->m_hMutex, INFINITE);

	Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
	Sleep(m_nPickTime / ModuleBase::s_dSpeed);


	while (pM->GetIsWorking() == false)
	{
		int InitialWafer = pM->GetWaferCount();
		if (pM->SetWaferCount(pM->GetWaferCount() - m_nWaferMax / 2) == true)
		{
			SetWaferCount(m_nWaferCount + m_nWaferMax / 2);

			// Throughtput ���ϱ� ���� �߰�==========================
			pM->m_nOutputWafer += m_nWaferMax / 2;
			this->m_nInputWafer += m_nWaferMax / 2;
			// =====================================================
			
			//GUI�� �����
			CString tmp;

			//if (s_bDirect == false)
			{
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
			}

			//else if (s_bDirect == true)
			//{
			//	int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

			//	tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
			//	pClistCtrl->SetItemText(m_nRow, axis*2 - m_nCol, tmp);

			//	tmp = _T("");
			//	tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
			//	pClistCtrl->SetItemText(pM->m_nRow, axis*2 - pM->m_nCol, tmp);
			//}

			if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
			{
				ProcessChamber* p = (ProcessChamber*)pM;
				SetEvent(p->m_hPmWaferCntChangeEvent);
			}

			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				LoadLock::s_nTotalSendWaferFromLL += InitialWafer - pM->GetWaferCount();;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}
			break;
		}
	}
	m_bIsWorking = false;
	ReleaseMutex(pM->m_hMutex);
	return true;
}

bool VACRobot::PlaceWafer(ModuleBase* pM)
{
	m_bIsWorking = true;
	WaitForSingleObject(pM->m_hMutex, INFINITE);

	Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
	Sleep(m_nPlaceTime / ModuleBase::s_dSpeed);     

	while (1)
	{
		if (pM->SetWaferCount(pM->GetWaferCount() + m_nWaferMax / 2) == true)
		{
			SetWaferCount(m_nWaferCount - m_nWaferMax / 2);

			// Throughtput ���ϱ� ���� �߰�==========================
			pM->m_nInputWafer += m_nWaferMax / 2;
			this->m_nOutputWafer += m_nWaferMax / 2;
			// =====================================================


			//GUI�� �����
			CString tmp;

			//if (s_bDirect == false)
			{
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
			}

			//else if (s_bDirect == true)
			//{
			//	int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

			//	tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
			//	pClistCtrl->SetItemText(m_nRow, axis*2 - m_nCol, tmp);

			//	tmp = _T("");
			//	tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
			//	pClistCtrl->SetItemText(pM->m_nRow, axis*2 - pM->m_nCol, tmp);
			//}

			if (pM->m_eModuleType == TYPE_PROCESSCHAMBER)
			{
				ProcessChamber* p = (ProcessChamber*)pM;
				SetEvent(p->m_hPmWaferCntChangeEvent);
			}
			break;
		}
	}

	m_bIsWorking = false;
	ReleaseMutex(pM->m_hMutex);
	return true;
}

void VACRobot::WorkThread()
{
	ModuleBase* pM = NULL;

	
	int nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();
	int nCntNeedToExchangeWafer = min(nMaxLLSlot, nMaxPMSlot) * 2;

	//1.
	while (m_bStopFlag == false)
	{
		Sleep(1);
		//�̹� Exchange�� �����ٴ� ���� Ȯ��
		bool bIsThereTrue = false;
		for (int i = 0; i < m_vPickModule.size(); i++)
		{
			if (m_vPickModule[i]->m_bExchangeOver == true)
				bIsThereTrue = true;
		}

		for (int j = 0; j < m_vPlaceModule.size(); j++)
		{
			if (m_vPlaceModule[j]->m_bExchangeOver == true)
				bIsThereTrue = true;
		}

		bool bCheckATMRobotEmpty = true;
		for (int i = 0; i < CFabController::s_pATMRobot.size(); i++)
		{
			if (CFabController::s_pATMRobot[i]->GetWaferCount() > 0)
				bCheckATMRobotEmpty = false;
		}

		bool bCheckLLFull = true;
		for (int i = 0; i < CFabController::s_pLL.size(); i++)
		{
			if (CFabController::s_pLL[i]->GetWaferCount() != CFabController::s_pLL[i]->GetWaferMax())
				bCheckLLFull = false;
		}

		int nCntCheckExchange = 0;
		//1.1. Wafer Exchange�ϴ� ���
		if (LPM::s_nTotalSendWafer > nMaxLLSlot && LPM::s_nTotalSendWafer % nMaxLLSlot == 0
			&& s_bDirect == false
			&& bIsThereTrue == false
			&& bCheckATMRobotEmpty == true
			&& bCheckLLFull == true)
		{	

			//Exchange�� �ʿ��� ���ڵ�
			int nCntExchangedWaferPickModule = 0;
			int nCntExchangedWaferPlaceModule = 0;

			int nCntNeedToExchangeWaferPickModule = 0;
			int nCntNeedToExchangeWaferPlaceModule = 0;

			//�۾� ����

			for (int i = 0; i < m_vPickModule.size(); i++)
			{
				if (m_vPickModule[i]->m_bExchangeOver == true)
					continue;

				nCntNeedToExchangeWaferPickModule = m_vPickModule[i]->GetWaferMax();

				for (int j = 0; j < m_vPlaceModule.size(); j++)
				{
					if (m_vPlaceModule[j]->m_bExchangeOver == true)
						continue;

					nCntNeedToExchangeWaferPlaceModule = m_vPlaceModule[j]->GetWaferMax();

					ProcessChamber* pPM = (ProcessChamber*)m_vPlaceModule[j];


					//1. LL�� ���� wafer�� Pick

					if (SetWaferCount(m_nWaferCount + m_nWaferMax / 2) == false)
						continue;
					m_vPickModule[i]->SetWaferCount(m_vPickModule[i]->GetWaferCount() - m_nWaferMax / 2);

					// Throughtput ���ϱ� ���� �߰�==========================
					m_vPickModule[i]->m_nOutputWafer += m_nWaferMax / 2;
					this->m_nInputWafer += m_nWaferMax / 2;
					// =====================================================
					
					//ȭ�鿡 ���
					CString tmp = _T("");
					tmp.Format(_T("%s\n(Pick)\n(%d)"), m_strModuleName, m_nWaferCount);
					m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(Pick)\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
					m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

					Sleep(m_nPickTime / ModuleBase::s_dSpeed);

					//2. Exchange ����
					for (int k = 0; k < max(nCntNeedToExchangeWaferPickModule, nCntNeedToExchangeWaferPlaceModule) * 2; k++)
					{
						if (pPM->m_nNecessaryDummyWafer > 0)
							pPM->m_nNecessaryDummyWafer -= m_nWaferMax / 2;;

						//PM�� Exchange
						if(k % 2 == 0)
						{	//ȭ�鿡 ���
							tmp = _T("");
							tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
							m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

							tmp = _T("");
							tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_vPlaceModule[j]->GetModuleName(), m_vPlaceModule[j]->GetWaferCount());
							m_pClistCtrl->SetItemText(m_vPlaceModule[j]->m_nRow, m_vPlaceModule[j]->m_nCol, tmp);

							// Throughtput ���ϱ� ���� �߰�==========================
							pPM->m_nInputWafer += m_nWaferMax / 2;
							pPM->m_nOutputWafer += m_nWaferMax / 2;
							this->m_nInputWafer += m_nWaferMax / 2;
							this->m_nOutputWafer += m_nWaferMax / 2;
							// =====================================================

							nCntExchangedWaferPlaceModule += m_nWaferMax / 2;
							if (nCntExchangedWaferPlaceModule == nCntNeedToExchangeWaferPlaceModule)
								break;
						}

						//LL�� Exchange
						else 
						{
							//ȭ�鿡 ���
							tmp = _T("");
							tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
							m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

							tmp = _T("");
							tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
							m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

							// Throughtput ���ϱ� ���� �߰�==========================
							m_vPickModule[i]->m_nInputWafer += m_nWaferMax / 2;
							m_vPickModule[i]->m_nOutputWafer += m_nWaferMax / 2;
							this->m_nInputWafer += m_nWaferMax / 2;
							this->m_nOutputWafer += m_nWaferMax / 2;
							// =====================================================

							nCntExchangedWaferPickModule += m_nWaferMax / 2;
							if (nCntExchangedWaferPickModule == nCntNeedToExchangeWaferPickModule)
								break;
						}


						//2. PM�� ���� Rotate
						Sleep(m_nRotateTime / ModuleBase::s_dSpeed);

						//3. PM�� wafer�� Exchange
						Sleep(max(m_nPlaceTime, m_nPickTime) / ModuleBase::s_dSpeed);

						//vPlaceModules[i]->SetWaferCount(vPlaceModules[i]->GetWaferCount() - 1);
						//m_nWaferCount++;

						//ȭ�鿡 ���
						tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
						m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
						m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), m_vPlaceModule[j]->GetModuleName(), m_vPlaceModule[j]->GetWaferCount());
						m_pClistCtrl->SetItemText(m_vPlaceModule[j]->m_nRow, m_vPlaceModule[j]->m_nCol, tmp);
					}

					//3. LL�� wafer �ٽ� Place
					//ȭ�鿡 ���
					tmp = _T("");
					tmp.Format(_T("%s\n(Place)\n(%d)"), m_strModuleName, m_nWaferCount);
					m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(Place)\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
					m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

					Sleep(m_nRotateTime / ModuleBase::s_dSpeed);
					if (SetWaferCount(m_nWaferCount - m_nWaferMax/2) == false)
						continue;
					m_vPickModule[i]->SetWaferCount(m_vPickModule[i]->GetWaferCount() + m_nWaferMax / 2);
					nCntExchangedWaferPickModule += m_nWaferMax / 2;

					// Throughtput ���ϱ� ���� �߰�==========================
					pM->m_nInputWafer += m_nWaferMax / 2;
					this->m_nOutputWafer += m_nWaferMax / 2;
					// =====================================================

					if(pPM->m_nNecessaryDummyWafer > 0)
						pPM->m_nNecessaryDummyWafer -= m_nWaferMax / 2;;

					Sleep(m_nPlaceTime / ModuleBase::s_dSpeed);
					//nCntExchangedWaferPickModule++;


					//ȭ�鿡 ���
					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
					m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), m_vPlaceModule[j]->GetModuleName(), m_vPlaceModule[j]->GetWaferCount());
					m_pClistCtrl->SetItemText(m_vPlaceModule[j]->m_nRow, m_vPlaceModule[j]->m_nCol, tmp);

					if (nCntExchangedWaferPlaceModule >= nCntNeedToExchangeWaferPlaceModule)
					{
						nCntExchangedWaferPlaceModule = 0;

						//ProcessChamber* pPM = (ProcessChamber*)(vPlaceModules[j]);
						pPM->m_bExchangeOver = true;
						SetEvent(pPM->m_hPmWaferCntChangeEvent);
					}	

					if (nCntExchangedWaferPickModule >= nCntNeedToExchangeWaferPickModule)
					{
						nCntExchangedWaferPickModule = 0;

						LoadLock* pLL = (LoadLock*)(m_vPickModule[i]);
						pLL->m_bExchangeOver = true;

						break;
					}		
				}
			}

			LoadLock::s_nTotalSendWaferFromLL = 0;
			ModuleBase::s_bDirect = true;

			ATMRobot::s_nRequiredDummyWaferCntLpmToPM;
			ATMRobot::s_nRequiredDummyWaferCntPMToLpm;

			for (int i = 0; i < m_vPickModule.size(); i++)
			{
				LoadLock* pLL = (LoadLock*)m_vPickModule[i];
				SetEvent(pLL->m_hLLWaferCntChangeEvent);
			}
		}

		//1.2. Wafer Move�ϴ� ���
		else if(LoadLock::s_nTotalSendWaferFromLL < nMaxPMSlot)
		{
			int nCntTotalPMWafer = 0;
			for (int i = 0; i < m_vPlaceModule.size(); i++)
			{
				nCntTotalPMWafer += m_vPlaceModule[i]->GetWaferCount();
			}

			if (nCntTotalPMWafer == nMaxPMSlot)
				continue;

			for (int i = 0; i < m_vPickModule.size(); i++)
			{
				pM = (ModuleBase*)m_vPickModule[i];

				if (pM->m_eModuleType == TYPE_LOADLOCK &&
					pM->GetSlotValveOpen() == false)
				{
				}
				else if (pM->GetIsWorking() == false && pM->GetWaferCount() >= m_nWaferMax / 2 && m_nWaferCount < m_nWaferMax)
				{
					//!!!!!!!!!!!!!!!!//

					PickWafer(pM);

					//!!!!!!!!!!!!!!!!//
				}
			}

			//Wafer�� ���� ����� ����͸���
			for (int i = 0; i < m_vPlaceModule.size(); i++)
			{
				pM = (ModuleBase*)m_vPlaceModule[i];

				if (pM->GetIsWorking() == false &&
					m_nWaferCount >= m_nWaferMax / 2 &&
					pM->GetWaferCount() <= pM->GetWaferMax() - m_nWaferMax / 2)
				{
					//!!!!!!!!!!!!!!!!//

					PlaceWafer(pM);

					//!!!!!!!!!!!!!!!!//
				}
			}
		}
	}
	
	SetEvent(m_hThreadCloseSignal);
}

void VACRobot::Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist)
{
	m_vPickModule = vPickModules;
	m_vPlaceModule = vPlaceModules;
	m_pClistCtrl = pClist;

	m_th = thread(&VACRobot::WorkThread, this);
}
#pragma endregion
