#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "Simulator PrototypeDlg.h"
#include "CFabController.h"

#pragma region LPM

#pragma region 생성자/소멸자

int LPM::s_nTotalInitWafer = 0;
int LPM::s_nTotalSendWafer = 0;
bool LPM::s_bLPMWaferPickBlock = false;

LPM::LPM(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	s_nTotalInitWafer += m_nWaferCount;
	m_nOutputWaferCount = 0;
}

LPM::~LPM()
{
	m_th.~thread();
}

#pragma endregion	

bool LPM::SetOutputWaferCount(int _value)
{
	if (m_nOutputWaferCount > m_nWaferMax || _value > m_nWaferMax || _value < 0) // WaferMax 수치 제한
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
	// int To CString (나중에 Convert 메서드 추가 고려중)
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d\n"), m_nWaferMax);
	
	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_LPM"), strFilePath);				// 타입
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// 모듈명
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
	WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax

}

void LPM::Run() //LL <--> EFEM
{
	m_th = thread(&LPM::work, this);
}
void LPM::work()
{
	int const nMaxPMSlot = CFabController::s_pPM.size() * CFabController::s_pPM[0]->GetWaferMax();
	int const nMaxLLSlot = CFabController::s_pLL.size() * CFabController::s_pLL[0]->GetWaferMax();

	//DUMMYSTAGE인 경우
	if (m_strModuleName.Compare(_T("DUMMY\nSTAGE")) == 0)
	{
		m_nDummyWaferCount = 12;
		m_bIsWorking = true;
	}

	//LPM인 경우
	else
	{
		while (1)
		{
			//WaitForSingleObject(ATMRobot::s_hEventSendWaferChange, INFINITE);
			//ResetEvent(ATMRobot::s_hEventSendWaferChange);

			Sleep(1);

			if (m_nWaferCount == 0 && m_nOutputWaferCount == m_nWaferMax)
				//&& (LPM::s_nTotalSendWafer > 0 && (LPM::s_nTotalSendWafer) % s_nTotalInitWafer == 0))
			{
				m_nWaferCount = m_nWaferMax;
				m_nOutputWaferCount = 0;

			}

		}
	}
}
#pragma endregion

#pragma region ATMRobot
HANDLE ATMRobot::s_hEventOutputWaferChange = CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE ATMRobot::s_hEventSendWaferChange = CreateEvent(NULL, TRUE, TRUE, NULL);

int ATMRobot::s_nTotalWaferCntFromLPM;

#pragma region 생성자/소멸자
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

#pragma region Get/Set 메서드
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

#pragma region 메서드
void ATMRobot::SaveConfigModule(int nIdx, CString strFilePath)
{
	// 기본 ModuleBase의 필드
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// ATM Robot 추가 필드
	CString strPickTime, strPlaceTime, strRotateTime, strZRotatetime, strArmMode;
	strPickTime.Format(_T("%d"), m_nPickTime);
	strPlaceTime.Format(_T("%d"), m_nPlaceTime);
	strRotateTime.Format(_T("%d"), m_nRotateTime);
	strZRotatetime.Format(_T("%d\n"), m_nRotateZCoordinateTime);

	strArmMode.Format(_T("DualArm"));		// << 추후 개선할 수 있으면 개선 (Robot Arm 표현)

	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_ATMROBOT"), strFilePath);			// 타입
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// 모듈명
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
	WritePrivateProfileString(strIdx, _T("ArmMode"), strArmMode, strFilePath);						// ArmMode(WaferMax)
	WritePrivateProfileString(strIdx, _T("PickTime"), strPickTime, strFilePath);					// PickTime
	WritePrivateProfileString(strIdx, _T("PlaceTime"), strPlaceTime, strFilePath);					// PlaceTime
	WritePrivateProfileString(strIdx, _T("StationMoveTime"), strRotateTime, strFilePath);			// RotateTime
	WritePrivateProfileString(strIdx, _T("Z-MoveTime"), strZRotatetime, strFilePath);				// ZRotateTime
}
bool ATMRobot::PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl)
{
	if (pM->m_eModuleType == TYPE_LPM || pM->GetModuleName().Compare(_T("DUMMY\nSTAGE")) == 0)
	{
		if (LPM::s_bLPMWaferPickBlock == true)
			return false;
	}

	while (pM->GetIsWorking() == false
		&& pM->GetWaferCount() > 0
		&& m_nWaferCount < m_nWaferMax)
	{
		m_bIsWorking = true;
		WaitForSingleObject(pM->m_hMutex, INFINITE);

		Sleep(m_nRotateTime / SPEED);
		Sleep(m_nPickTime / SPEED);


		if (pM->SetWaferCount(pM->GetWaferCount() - 1) == true)
		{
			Sleep(m_nRotateTime / SPEED);
			Sleep(m_nPickTime / SPEED);

			SetWaferCount(m_nWaferCount + 1);

			if (pM->m_eModuleType == TYPE_LPM)
			{
				LPM* pLPM = (LPM*)pM;
				CString tmp = _T("");

				LPM::s_nTotalSendWafer++;
				SetEvent(ATMRobot::s_hEventSendWaferChange);

				tmp.Format(_T("Input:\n(%d)"), LPM::s_nTotalSendWafer);
				pClistCtrl->SetItemText(3, 1, tmp);

				//Aligner와 Exchange
				Sleep(m_nRotateTime / SPEED);

				//GUI에 찍어줌
				tmp = _T("");
				tmp.Format(_T("Aligner\n(ExChange)\n(%d)"), 1);
				pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount - 1);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);


				Sleep(max(m_nPickTime, m_nPlaceTime) / SPEED);

				tmp = _T("");
				tmp.Format(_T("Aligner\n(ExChange)\n(%d)"), 0);
				pClistCtrl->SetItemText(m_nRow - 1, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(ExChange)\n(%d)"), m_strModuleName, m_nWaferCount);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				//GUI에 찍어줌
				//

				tmp = _T("");
				tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount - 1);
				pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

				tmp = _T("");
				tmp.Format(_T("%s\n(전:%d)\n(후:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
				pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);

				//tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
				//pClistCtrl->SetItemText(3, m_nCol + 2, tmp);


				m_bIsWorking = false;
				ReleaseMutex(pM->m_hMutex);
				return true;
				//
			}

			//GUI에 찍어줌
			//{
				CString tmp;

				if (s_bDirect == false)
				{
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}

				else if (s_bDirect == true)
				{
					int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
				}
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
	if (pM->m_eModuleType == TYPE_LOADLOCK && pM->GetDoorValveOpen() == false)
	{
		return false;
	}

	if (pM->m_eModuleType == TYPE_LPM || m_strModuleName.Compare(_T("DUMMY\nSTAGE")) == 0)
	{
		LPM* pLPM = (LPM*)pM;

		if (pLPM->GetIsWorking() == false &&
			m_nWaferCount > 0 &&
			pLPM->GetOutputWaferCount() + pLPM->GetWaferCount() < pLPM->GetWaferMax())
		{
			m_bIsWorking = true;
			WaitForSingleObject(pM->m_hMutex, INFINITE);

			Sleep(m_nRotateTime / SPEED);
			Sleep(m_nPlaceTime / SPEED);

			SetWaferCount(m_nWaferCount - 1);
			pLPM->SetOutputWaferCount(pLPM->GetOutputWaferCount() + 1);

			LPM::s_nTotalOutputWafer++;
			SetEvent(ATMRobot::s_hEventOutputWaferChange);

			//GUI에 찍어줌
			CString tmp;

			int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

			tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
			pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

			tmp = _T("");
			tmp.Format(_T("%s\n(전:%d)\n(후:%d)"), pM->GetModuleName(), pM->GetWaferCount(), pLPM->GetOutputWaferCount());
			pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);

			tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
			pClistCtrl->SetItemText(3, 2 * axis - m_nCol + 2, tmp);

			m_bIsWorking = false;
			ReleaseMutex(pM->m_hMutex);

			return true;
		}

		else
			return false;
	}

	while (pM->GetIsWorking() == false &&
		pM->GetWaferCount() < pM->GetWaferMax() &&
		m_nWaferCount > 0)
	{
		m_bIsWorking = true;
		WaitForSingleObject(pM->m_hMutex, INFINITE);

		Sleep(m_nRotateTime / SPEED);
		Sleep(m_nPlaceTime / SPEED);
		//int InitialWaferYou = pM->GetWaferCount();
		//int InitialWaferMe = m_nWaferCount;

		if (pM->SetWaferCount(pM->GetWaferCount() + 1) == true)
		{
			SetWaferCount(m_nWaferCount - 1);

			//if (pM->m_eModuleType == TYPE_LPM)
			//{
			//	LPM::s_nTotalOutputWafer += pM->GetWaferCount() - InitialWaferYou;
			//	pM->SetWaferCount(InitialWaferYou);
			//	LPM* pLPM = (LPM*)pM;
			//	SetEvent(ATMRobot::s_hEventBlockATMRobot);

			//	CString tmp = _T("");
			//	tmp.Format(_T("Output\n(%d)"), LPM::s_nTotalOutputWafer);
			//	pClistCtrl->SetItemText(pM->m_nRow + 1, CFabController::GetInstance().m_pModule.back()->m_nCol * 2 - m_nCol + 1, tmp);
			//}
			// 
			//GUI에 찍어줌
			CString tmp;
			if (pM->m_eModuleType != TYPE_LPM)
			{
				if (s_bDirect == false)
				{
					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, pM->m_nCol, tmp);
				}

				else if (s_bDirect == true)
				{
					int axis = CFabController::GetInstance().m_pModule.back()->m_nCol;

					tmp.Format(_T("%s\n(%d)"), m_strModuleName, m_nWaferCount);
					pClistCtrl->SetItemText(m_nRow, 2 * axis - m_nCol, tmp);

					tmp = _T("");
					tmp.Format(_T("%s\n(%d)"), pM->GetModuleName(), pM->GetWaferCount());
					pClistCtrl->SetItemText(pM->m_nRow, 2 * axis - pM->m_nCol, tmp);
				}
			}
			/////////////////////////////////////////////////////////////////////////////
			if (pM->m_eModuleType == TYPE_LOADLOCK)
			{
				LoadLock* p = (LoadLock*)pM;
				SetEvent(p->m_hLLWaferCntChangeEvent);
			}

		}

		m_bIsWorking = false;
		ReleaseMutex(pM->m_hMutex);
		return true;
	}
}

void ATMRobot::work(Pick_PlaceM Pick_Place)
{
	vector<ModuleBase*> vLPMModules = Pick_Place.m_vPickModule;
	vector<ModuleBase*> vLLModules = Pick_Place.m_vPlaceModule;

	//ModuleBase* pSavePickModule = NULL;
	//ModuleBase* pSavePlaceModule = NULL;

	int k = 0;
	int l = 0;
	
	int m = 0;
	int n = 0;

	CListCtrl* pClistCtrl = Pick_Place.m_pClistCtrl;

	ModuleBase* pM = NULL;

	while (1)
	{
		Sleep(1);
		////PM이 다 차있는지 확인함
		//if (s_bDirect == false)
		////{
		//vLPMModules = Pick_Place.m_vPickModule;
		//vLLModules = Pick_Place.m_vPlaceModule;
		//}

		//else if (s_bDirect == true)
		//{
		//	vPickModules = Pick_Place.m_vPlaceModule;
		//	vPlaceModules = Pick_Place.m_vPickModule;
		//}


		//Wafer을 가져올 모듈을 모니터링함
		//for (int i = 0; i < vPickModules.size(); i++)
		//{
			//if (pSavePickModule != NULL && pSavePickModule->GetWaferCount() < pSavePickModule->GetWaferMax())
			//{
			//	PickWafer(pSavePickModule, pClistCtrl);
			//}

			//else
			////{
			//	pM = (ModuleBase*)vPickModules[i];

			//	if(PickWafer(pM, pClistCtrl)) break;

			//	pSavePickModule = pM;

			//	break;
			//}
		if (s_bDirect == false)
		{

			//Pick하는 경우
			pM = vLPMModules[k];
			bool bCheck = PickWafer(pM, pClistCtrl);

			if (bCheck == false && vLPMModules[k]->GetModuleName().Compare(_T("DUMMY\nSTAGE")) == 0 ||
				pM->GetWaferCount() == 0)
			{
				k++;
				if (k == vLPMModules.size())
					k = 0;
			}

			//Place하는 경우
			pM = vLLModules[l];
			PlaceWafer(pM, pClistCtrl);

			if (pM->GetWaferCount() == pM->GetWaferMax())
			{
				l++;
				if (l == vLLModules.size())
					l = 0;
			}
		}
			//if (pM->GetWaferCount() == 0)
			//{
			//	continue;
			//}

			//else if (pM->GetWaferCount() > 0)
			//{
			//	m++;
			//	if (m == vPickModules.size())
			//		m = 0;

			//	break;
			//}

		//}
		else
		{
			//Pick하는 경우
			pM = vLLModules[n];
			bool bCheck1 = PickWafer(pM, pClistCtrl);

			if (bCheck1 == false && vLLModules[n]->GetModuleName().Compare(_T("DUMMY\nSTAGE")) == 0 ||
				pM->GetWaferCount() == 0)
			{
				n++;
				if (n == vLLModules.size())
					n = 0;
			}
			//if (vPlaceModules[1]->m_eModuleType == TYPE_LPM)
			//{
				/*for (int i = m; i < vPlaceModules.size(); i++)
				{*/
				//LPM* pLPM = (LPM*)vPlaceModules[m];
				//bool bCheck = PlaceWafer(pLPM, pClistCtrl);

				//if (bCheck == false && vPlaceModules[m]->m_eModuleType == TYPE_DUMMYSTAGE ||
				//	pLPM->GetOutputWaferCount() == pLPM->GetWaferMax() ||
				//	pLPM->GetWaferCount() == pLPM->GetWaferMax())
				//{
				//	m++;

				//	if (m == vPlaceModules.size())
				//		m = 0;
				//}
				//break;

			//}
			//}

			//Place하는 경우
			LPM* pLPM = (LPM*)vLPMModules[m];
			bool bCheck2 = PlaceWafer(pLPM, pClistCtrl);

			if (bCheck2 == false && vLPMModules[m]->GetModuleName().Compare(_T("DUMMY\nSTAGE")) == 0 ||
				pLPM->GetOutputWaferCount() == pLPM->GetWaferMax() ||
				pLPM->GetWaferCount() == pLPM->GetWaferMax())
			{
				m++;

				if (m == vLPMModules.size())
					m = 0;
			}
			//if (vPlaceModules[1]->m_eModuleType != TYPE_LPM)
			//{
				//for (int i = n; i < vPlaceModules.size(); i++)
				//{
				//pM = vPlaceModules[n];
				//bool bCheck = PlaceWafer(pM, pClistCtrl);

				//if (pM->GetWaferCount() == pM->GetWaferMax())
				//{
				//	n++;
				//	if (n == vPlaceModules.size())
				//		n = 0;
				//}

				//break;
			//}
			//}
		}
	}
}

void ATMRobot::Run(vector<ModuleBase*> _vPickModules, vector<ModuleBase*> _vPlaceModules, CListCtrl* _pClistCtrl)
{
	Pick_PlaceM pick_place;
	pick_place.m_vPickModule = _vPickModules;
	pick_place.m_vPlaceModule = _vPlaceModules;
	pick_place.m_pClistCtrl = _pClistCtrl;

	m_th = thread(&ATMRobot::work, this, pick_place);
}
#pragma endregion

#pragma endregion