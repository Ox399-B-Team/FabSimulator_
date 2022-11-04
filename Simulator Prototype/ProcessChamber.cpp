#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "ProcessChamber.h"

int ProcessChamber::s_nCntPMWorkOver = 0;
vector<HANDLE> ProcessChamber::s_vWorkOverHandle;
int ProcessChamber::s_nCount = 0;

vector<ProcessChamber*> ProcessChamber::s_pPM;

#pragma region 생성자/소멸자

ProcessChamber::ProcessChamber(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, 
	int _ProcessTime, int _CleanTime, int _SlotOpenTime, int _SlotCloseTime, const int _CleanCount)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nProcessTime = _ProcessTime;
	m_nCleanTime = _CleanTime;
	m_nSlotValveOpenTime = _SlotOpenTime;
	m_nSlotValveCloseTime = _SlotCloseTime;
	m_nCleanCount = _CleanCount;

	m_nProcessCount = 0;
	s_nCount++;
	m_bNecessaryDummyWafer = false;
	m_bRequiredDummyWaferCntPMToLpm = false;

	m_hPMWorkOver = CreateEvent(NULL, FALSE, FALSE, NULL);
	s_vWorkOverHandle.push_back(m_hPMWorkOver);

	m_hPmWaferCntChangeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	s_pPM.push_back(this);
}

ProcessChamber::~ProcessChamber()
{

	for (int i = 0; i < s_vWorkOverHandle.size(); i++)
	{
		if(s_vWorkOverHandle[i] == m_hPMWorkOver)
		{
			s_vWorkOverHandle.erase(s_vWorkOverHandle.begin() + i);
			break;
		}
	}

	for (int i = 0; i < s_pPM.size(); i++)
	{
		if (s_pPM[i] == this)
		{
			s_pPM.erase(s_pPM.begin() + i);
			break;
		}
	}

	s_nCount--;

	CloseHandle(m_hPmWaferCntChangeEvent);
	CloseHandle(m_hPMWorkOver);
}
#pragma endregion

#pragma region Get/Set 메서드
void ProcessChamber::SetProcessTime(int _value)
{
	m_nProcessTime = _value;
}

int ProcessChamber::GetProcessTime() const
{
	return m_nProcessTime;
}

void ProcessChamber::SetCleanTime(int _value)
{
	m_nCleanTime = _value;
}

int ProcessChamber::GetCleanTime() const
{
	return m_nCleanTime;
}

void ProcessChamber::SetCleanCount(int _value)
{
	m_nCleanCount = _value;
}

int ProcessChamber::GetCleanCount() const
{
	return m_nCleanCount;
}

int ProcessChamber::GetProcessCount() const
{
	return m_nProcessCount;
}

void ProcessChamber::SetSlotValveOpenTime(int _value)
{
	m_nSlotValveOpenTime = _value;
}

int ProcessChamber::GetSlotValveOpenTime() const
{
	return m_nSlotValveOpenTime;
}

void ProcessChamber::SetSlotValveCloseTime(int _value)
{
	m_nSlotValveCloseTime = _value;
}

int ProcessChamber::GetSlotValveCloseTime() const
{
	return m_nSlotValveCloseTime;
}
#pragma endregion

#pragma region 메서드
void ProcessChamber::SaveConfigModule(int nIdx, CString strFilePath)
{
	// 기본 ModuleBase의 필드
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// ProcessChamber 추가 필드
	CString strProcessTime, strCleanTime, strCleanCount, strSlotOpenTime, strSlotCloseTime;

	strProcessTime.Format(_T("%d"), m_nProcessTime);
	strCleanTime.Format(_T("%d"), m_nCleanTime);
	strCleanCount.Format(_T("%d"), m_nCleanCount);
	strSlotOpenTime.Format(_T("%d"), m_nSlotValveOpenTime);
	strSlotCloseTime.Format(_T("%d\n"), m_nSlotValveCloseTime);

	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_PROCESSCHAMBER"), strFilePath);	// 타입
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// 모듈명
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// 컬럼
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// 로우
	WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax
	WritePrivateProfileString(strIdx, _T("ProcessTime"), strProcessTime, strFilePath);				// ProcessTime
	WritePrivateProfileString(strIdx, _T("CleanTime"), strCleanTime, strFilePath);					// CleanTime
	WritePrivateProfileString(strIdx, _T("CleanCount"), strCleanCount, strFilePath);				// CleanCount
	WritePrivateProfileString(strIdx, _T("SlotValveOpenTime"), strSlotOpenTime, strFilePath);		// SlotOpenTime
	WritePrivateProfileString(strIdx, _T("SlotValveCloseTime"), strSlotCloseTime, strFilePath);		// SlotCloseTime
}

void ProcessChamber::WorkThread()
{
	int nCheckFirstProcess = 0;
	bool bCheck = false;
	while (m_bStopFlag == false)
	{
		WaitForSingleObject(m_hPmWaferCntChangeEvent, INFINITE);
		//1. Clean
		if (bCheck == false
			&& m_nProcessCount > 0 && m_nProcessCount % m_nCleanCount == 0
			&& m_nWaferCount == m_nWaferMax)
		{
			m_bIsWorking = true;

			Sleep(m_nSlotValveCloseTime / ModuleBase::s_dSpeed);

			CString tmp = _T("");
			for (int i = 1; i < 21; i++)
			{
				Sleep(m_nCleanTime / (ModuleBase::s_dSpeed * 20));
				tmp.Format(_T("%s\n<Clean>\n[%d%%]"), m_strModuleName, i * 5);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
			}

			Sleep(m_nSlotValveOpenTime / ModuleBase::s_dSpeed);

			//OutputDebugString(_T("dd\n"));
			m_bRequiredDummyWaferCntPMToLpm = true;

			SetEvent(m_hPMWorkOver);
			m_bIsWorking = false;

			bCheck = true;
		}
		
		//2. Process
		else if (m_nWaferCount == m_nWaferMax)
		{
			m_bIsWorking = true;

			m_nProcessCount++;

			Sleep(m_nSlotValveCloseTime / ModuleBase::s_dSpeed);

			CString tmp = _T("");
			for (int i = 1; i < 21; i++)
			{
				Sleep(m_nProcessTime / (ModuleBase::s_dSpeed * 20));
				tmp.Format(_T("%s\n<Process>\n[%d%%]"), m_strModuleName, i * 5);
				m_pClistCtrl->SetItemText(m_nRow, m_nCol, tmp);
			}

			Sleep(m_nSlotValveOpenTime / ModuleBase::s_dSpeed);

			if (m_nProcessCount > 0 && m_nProcessCount % m_nCleanCount == 0)
			{
				m_bNecessaryDummyWafer = true;
				ATMRobot::s_nRequiredDummyWaferCntLpmToPM += m_nWaferMax;
				LoadLock::s_nRequiredDummyWaferCntLpmToPM += m_nWaferMax;
			}

			SetEvent(m_hPMWorkOver);
			m_bIsWorking = false;

			bCheck = false;
		}
		ResetEvent(m_hPmWaferCntChangeEvent);
	}
	SetEvent(m_hThreadCloseSignal);
}

void ProcessChamber::Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist)
{
	m_pClistCtrl = pClist;
	m_th = thread(&ProcessChamber::WorkThread, this);
}
#pragma endregion