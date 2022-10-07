#include "pch.h"
#include "EFEM.h"
#include "LoadLock.h"
#include "ProcessChamber.h"

#pragma region ������/�Ҹ���

ProcessChamber::ProcessChamber(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, 
	int _ProcessTime, int _CleanTime, int _SlotOpenTime, int _SlotCloseTime, int _CleanCount)
	: ModuleBase(_Type, _Name, _WaferCount, _WaferMax, _Row, _Col)
{
	m_nProcessTime = _ProcessTime;
	m_nCleanTime = _CleanTime;
	m_nSlotValveOpenTime = _SlotOpenTime;
	m_nSlotValveCloseTime = _SlotCloseTime;
	m_nCleanCount = _CleanCount;

	m_nProcessCount = 0;

	m_nNecessaryDummyWafer = 0;

	m_hPmWaferCntChangeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

ProcessChamber::~ProcessChamber()
{
	CloseHandle(m_hPmWaferCntChangeEvent);
}
#pragma endregion

#pragma region Get/Set �޼���
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

#pragma region �޼���
void ProcessChamber::SaveConfigModule(int nIdx, CString strFilePath)
{
	// �⺻ ModuleBase�� �ʵ�
	CString strIdx, strRow, strCol, strWaferMax;
	strIdx.Format(_T("%d"), nIdx);
	strRow.Format(_T("%d"), m_nRow);
	strCol.Format(_T("%d"), m_nCol);
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	// ProcessChamber �߰� �ʵ�
	CString strProcessTime, strCleanTime, strCleanCount, strSlotOpenTime, strSlotCloseTime;

	strProcessTime.Format(_T("%d"), m_nProcessTime);
	strCleanTime.Format(_T("%d"), m_nCleanTime);
	strCleanCount.Format(_T("%d"), m_nCleanCount);
	strSlotOpenTime.Format(_T("%d"), m_nSlotValveOpenTime);
	strSlotCloseTime.Format(_T("%d\n"), m_nSlotValveCloseTime);

	WritePrivateProfileString(strIdx, _T("ModuleType"), _T("TYPE_PROCESSCHAMBER"), strFilePath);	// Ÿ��
	WritePrivateProfileString(strIdx, _T("ModuleName"), m_strModuleName, strFilePath);				// ����
	WritePrivateProfileString(strIdx, _T("PosX"), strCol, strFilePath);								// �÷�
	WritePrivateProfileString(strIdx, _T("PosY"), strRow, strFilePath);								// �ο�
	WritePrivateProfileString(strIdx, _T("WaferMax"), strWaferMax, strFilePath);					// WaferMax
	WritePrivateProfileString(strIdx, _T("ProcessTime"), strProcessTime, strFilePath);				// ProcessTime
	WritePrivateProfileString(strIdx, _T("CleanTime"), strCleanTime, strFilePath);					// CleanTime
	WritePrivateProfileString(strIdx, _T("CleanCount"), strCleanCount, strFilePath);				// CleanCount
	WritePrivateProfileString(strIdx, _T("SlotValveOpenTime"), strSlotOpenTime, strFilePath);		// SlotOpenTime
	WritePrivateProfileString(strIdx, _T("SlotValveCloseTime"), strSlotCloseTime, strFilePath);		// SlotCloseTime
}

void ProcessChamber::work()
{
	while (m_bStopFlag == false)
	{
		WaitForSingleObject(m_hPmWaferCntChangeEvent, INFINITE);
		if (m_nProcessCount > 0 && m_nProcessCount % m_nCleanCount == 0 
			&& m_nWaferCount == m_nWaferMax//) 
			&& m_nNecessaryDummyWafer == 0)
		{
			m_bIsWorking = true;
			
			Sleep(m_nSlotValveCloseTime / ModuleBase::s_dSpeed);

			Sleep(m_nCleanTime / ModuleBase::s_dSpeed);

			Sleep(m_nSlotValveOpenTime / ModuleBase::s_dSpeed);

			ATMRobot::s_nRequiredDummyWaferCntPMToLpm += m_nWaferMax;
		}
		//ResetEvent(m_hPmWaferCntMinusEvent);

		//WaitForSingleObject(m_hPmWaferCntPlusEvent, INFINITE);
		if (m_nWaferCount == m_nWaferMax)
		{
			m_bIsWorking = true;

			Sleep(m_nSlotValveCloseTime / ModuleBase::s_dSpeed);

			Sleep(m_nProcessTime / ModuleBase::s_dSpeed);

			Sleep(m_nSlotValveOpenTime / ModuleBase::s_dSpeed);

			m_nProcessCount++;
			if (m_nProcessCount > 0 && m_nProcessCount % m_nCleanCount == 0)
			{
				m_nNecessaryDummyWafer = m_nWaferMax;
				ATMRobot::s_nRequiredDummyWaferCntLpmToPM += m_nWaferMax;
				//LoadLock::s_nRequiredDummyWaferCntLpmToPM += m_nWaferMax;
			}
		}
		m_bIsWorking = false;
		ResetEvent(m_hPmWaferCntChangeEvent);
	}
	SetEvent(m_hThreadCloseSignal);
}

void ProcessChamber::Run()
{
	m_th = thread(&ProcessChamber::work, this);
}
#pragma endregion