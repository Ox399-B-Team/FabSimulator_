#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "VACRobot.h"
#include "ProcessChamber.h"

int VACRobot::s_nCount = 0;
vector<VACRobot*> VACRobot::s_pVACRobot;

#pragma region 생성자/소멸자
VACRobot::VACRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nPickTime = _PickTime;
	m_nPlaceTime = _PlaceTime;
	m_nRotateTime = _RotateTime;
	s_nCount++;

	s_pVACRobot.push_back(this);
}
VACRobot::~VACRobot()
{
	s_nCount--;

	for (int i = 0; i < s_pVACRobot.size(); i++)
	{
		if (s_pVACRobot[i] == this)
		{
			s_pVACRobot.erase(s_pVACRobot.begin() + i);
			break;
		}
	}
}
#pragma endregion

#pragma region Get/Set 메서드
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

#pragma region 메서드
void VACRobot::SaveConfigModule(int nIdx, CString strFilePath)
{
	// 기본 ModuleBase의 필드
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// VACRobot 추가 필드
	CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
	strArmMode.Format(m_nWaferMax == 4 ? _T("QuadArm") : _T("DualArm"));					// WaferMax == 4 일 시 QuadArm | 아닐 시 DualArm
	strPickTime.Format(_T("%d"), m_nPickTime);
	strPlaceTime.Format(_T("%d"), m_nPlaceTime);
	strRotateTime.Format(_T("%d\n"), m_nRotateTime);


	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_VACROBOT"), strFilePath);			// 타입
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// 모듈명
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
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


	while (pM->GetIsWorking() == false && m_bStopFlag == false)
	{
		int InitialWafer = pM->GetWaferCount();
		if (pM->SetWaferCount(pM->GetWaferCount() - m_nWaferMax / 2) == true)
		{
			SetWaferCount(m_nWaferCount + m_nWaferMax / 2);

			// Throughtput 구하기 위해 추가==========================
			pM->m_nOutputWafer += m_nWaferMax / 2;
			this->m_nInputWafer += m_nWaferMax / 2;
			// =====================================================
			
			//GUI에 찍어줌
			CString tmp;

			tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
			m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

			tmp = _T("");
			tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
			m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);


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

	while (m_bStopFlag == false)
	{
		if (pM->SetWaferCount(pM->GetWaferCount() + m_nWaferMax / 2) == true)
		{
			SetWaferCount(m_nWaferCount - m_nWaferMax / 2);

			// Throughtput 구하기 위해 추가==========================
			pM->m_nInputWafer += m_nWaferMax / 2;
			this->m_nOutputWafer += m_nWaferMax / 2;
			// =====================================================


			//GUI에 찍어줌
			CString tmp;

			//if (s_bDirect == false)
			{
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
				m_pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
			}

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

	int nMaxPMSlot = 0;
	int nMaxLLSlot = 0;
	int nCntNeedToExchangeWafer = 0;

	if (ProcessChamber::s_pPM.size() > 0 && LoadLock::s_pLL.size() > 0)
	{
		nMaxPMSlot = ProcessChamber::s_pPM.size() * ProcessChamber::s_pPM[0]->GetWaferMax();
		nMaxLLSlot = LoadLock::s_pLL.size() * LoadLock::s_pLL[0]->GetWaferMax();
		nCntNeedToExchangeWafer = min(nMaxLLSlot, nMaxPMSlot) * 2;
	}

	while (m_bStopFlag == false)
	{
		Sleep(1 / s_dSpeed);

		//이미 Exchange가 끝났다는 조건 확인
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
		for (int i = 0; i < ATMRobot::s_pATMRobot.size(); i++)
		{
			if (ATMRobot::s_pATMRobot[i]->GetWaferCount() > 0)
				bCheckATMRobotEmpty = false;
		}

		bool bCheckLLFull = true;
		for (int i = 0; i < LoadLock::s_pLL.size(); i++)
		{
			if (LoadLock::s_pLL[i]->GetWaferCount() != LoadLock::s_pLL[i]->GetWaferMax())
				bCheckLLFull = false;
		}

		//1.1. Wafer Exchange하는 경우
		if (LPM::s_nTotalSendWafer > nMaxLLSlot && LPM::s_nTotalSendWafer % nMaxLLSlot == 0
			&& s_bDirect == false
			&& bIsThereTrue == false
			&& bCheckATMRobotEmpty == true
			&& bCheckLLFull == true)
		{
			//Exchange에 필요한 인자들
			int nCntExchangedWaferPickModule = 0;
			int nCntExchangedWaferPlaceModule = 0;

			int nCntNeedToExchangeWaferPickModule = 0;
			int nCntNeedToExchangeWaferPlaceModule = 0;

			//작업 시작
			for (int m = 0; m < 2; m++)
			{
				for (int i = 0; i < m_vPickModule.size(); i++)
				{
					LoadLock* pLL = (LoadLock*)m_vPickModule[i];
					if (pLL->m_bExchangeOver == true)
						continue;

					nCntNeedToExchangeWaferPickModule = m_vPickModule[i]->GetWaferMax();

					for (int j = 0; j < m_vPlaceModule.size(); j++)
					{
						ProcessChamber* pPM = (ProcessChamber*)m_vPlaceModule[j];
						if (pPM->m_bExchangeOver == true || (m == 0 && pPM->m_bNecessaryDummyWafer == false))
							continue;

						if (pPM->m_bNecessaryDummyWafer == true)
							s_bIsCleaning = true;

						else
							s_bIsCleaning = false;

						nCntNeedToExchangeWaferPlaceModule = pPM->GetWaferMax();

						//1. LL로 부터 wafer를 Pick

						if (SetWaferCount(m_nWaferCount + m_nWaferMax / 2) == false)
							continue;
						m_vPickModule[i]->SetWaferCount(m_vPickModule[i]->GetWaferCount() - m_nWaferMax / 2);

						// Throughtput 구하기 위해 추가==========================
						m_vPickModule[i]->m_nOutputWafer += m_nWaferMax / 2;
						this->m_nInputWafer += m_nWaferMax / 2;
						// =====================================================

						//화면에 출력
						CString tmp = _T("");
						tmp.Format(_T("%s\n<Pick>\n(%d)"), m_strModuleName, m_nWaferCount);
						m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n<Pick>\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
						m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

						Sleep(m_nPickTime / s_dSpeed);

						//2. Exchange 시작
						for (int k = 0; k < max(nCntNeedToExchangeWaferPickModule, nCntNeedToExchangeWaferPlaceModule) * 2; k++)
						{
							//PM과 Exchange
							if (k % 2 == 0)
							{	//화면에 출력
								tmp = _T("");
								tmp.Format(_T("%s\n<Exchange>\n(%d)"), m_strModuleName, m_nWaferCount);
								m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

								tmp = _T("");
								tmp.Format(_T("%s\n<Exchange>\n(%d)"), m_vPlaceModule[j]->GetModuleName(), m_vPlaceModule[j]->GetWaferCount());
								m_pClistCtrl->SetItemText(m_vPlaceModule[j]->m_nRow, m_vPlaceModule[j]->m_nCol, tmp);

								// Throughtput 구하기 위해 추가==========================
								pPM->m_nInputWafer += m_nWaferMax / 2;
								pPM->m_nOutputWafer += m_nWaferMax / 2;
								this->m_nInputWafer += m_nWaferMax / 2;
								this->m_nOutputWafer += m_nWaferMax / 2;
								// =====================================================

								nCntExchangedWaferPlaceModule += m_nWaferMax / 2;
								if (nCntExchangedWaferPlaceModule == nCntNeedToExchangeWaferPlaceModule)
									break;
							}

							//LL과 Exchange
							else
							{
								//화면에 출력
								tmp = _T("");
								tmp.Format(_T("%s\n<Exchange>\n(%d)"), m_strModuleName, m_nWaferCount);
								m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

								tmp = _T("");
								tmp.Format(_T("%s\n<Exchange>\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
								m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

								// Throughtput 구하기 위해 추가==========================
								m_vPickModule[i]->m_nInputWafer += m_nWaferMax / 2;
								m_vPickModule[i]->m_nOutputWafer += m_nWaferMax / 2;
								this->m_nInputWafer += m_nWaferMax / 2;
								this->m_nOutputWafer += m_nWaferMax / 2;
								// =====================================================

								nCntExchangedWaferPickModule += m_nWaferMax / 2;
								if (nCntExchangedWaferPickModule == nCntNeedToExchangeWaferPickModule)
									break;
							}


							//2. 모듈을 향해 Rotate
							Sleep(m_nRotateTime / s_dSpeed);

							//3. 모듈의 wafer와 Exchange
							Sleep(max(m_nPlaceTime, m_nPickTime) / s_dSpeed);

							//화면에 출력
							tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
							m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

							tmp = _T("");
							tmp.Format(_T("%s\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
							m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

							tmp = _T("");
							tmp.Format(_T("%s\n(%d)"), m_vPlaceModule[j]->GetModuleName(), m_vPlaceModule[j]->GetWaferCount());
							m_pClistCtrl->SetItemText(m_vPlaceModule[j]->m_nRow, m_vPlaceModule[j]->m_nCol, tmp);
						}

						//3. LL에 wafer 다시 Place
						//화면에 출력
						tmp = _T("");
						tmp.Format(_T("%s\n<Place>\n(%d)"), m_strModuleName, m_nWaferCount);
						m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n<Place>\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
						m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

						Sleep(m_nRotateTime / s_dSpeed);
						if (SetWaferCount(m_nWaferCount - m_nWaferMax / 2) == false)
							continue;
						m_vPickModule[i]->SetWaferCount(m_vPickModule[i]->GetWaferCount() + m_nWaferMax / 2);
						nCntExchangedWaferPickModule += m_nWaferMax / 2;

						// Throughtput 구하기 위해 추가==========================
						pM->m_nInputWafer += m_nWaferMax / 2;
						this->m_nOutputWafer += m_nWaferMax / 2;
						// =====================================================

						Sleep(m_nPlaceTime / s_dSpeed);


						//화면에 출력
						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
						m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), m_vPickModule[i]->GetModuleName(), m_vPickModule[i]->GetWaferCount());
						m_pClistCtrl->SetItemText(m_vPickModule[i]->m_nRow, m_vPickModule[i]->m_nCol, tmp);

						tmp = _T("");
						tmp.Format(_T("%s\n(%d)"), m_vPlaceModule[j]->GetModuleName(), m_vPlaceModule[j]->GetWaferCount());
						m_pClistCtrl->SetItemText(m_vPlaceModule[j]->m_nRow, m_vPlaceModule[j]->m_nCol, tmp);
						
						if (pPM->m_bRequiredDummyWaferCntPMToLpm == true)
						{
							LoadLock::s_nRequiredDummyWaferCntPMToLpm += pPM->GetWaferMax();
							ATMRobot::s_nRequiredDummyWaferCntPMToLpm += pPM->GetWaferMax();
							pPM->m_bRequiredDummyWaferCntPMToLpm = false;
						}

						if (nCntExchangedWaferPlaceModule >= nCntNeedToExchangeWaferPlaceModule)
						{
							nCntExchangedWaferPlaceModule = 0;

							//ProcessChamber* pPM = (ProcessChamber*)(vPlaceModules[j]);
							pPM->m_bExchangeOver = true;
							if (pPM->m_bNecessaryDummyWafer == true)
								pPM->m_bNecessaryDummyWafer = false;

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

		//1.2. Wafer Move하는 경우
		else
		{
			if (LoadLock::s_nTotalSendWaferFromLL <= nMaxPMSlot)
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

				//Wafer을 보낼 모듈을 모니터링함
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
