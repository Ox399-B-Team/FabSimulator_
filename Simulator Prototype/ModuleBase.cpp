#include "pch.h"
#include "ModuleBase.h"
#include "CFabController.h"

bool ModuleBase::s_bDirect = false;
double ModuleBase::m_dTotalProcessTime = 0.0;
double ModuleBase::m_dTotalCleanTime = 0.0;
double ModuleBase::m_dTotalThroughput = 0.0;
int ModuleBase::s_nTotalOutputWafer = 0;
int ModuleBase::s_nTotalInputWafer = 0;

#pragma region ������/�Ҹ���

ModuleBase::ModuleBase(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col)
{
	m_eModuleType = _Type;
	m_strModuleName = _Name;
	m_nRow = _Row;
	m_nCol = _Col;
	m_nWaferCount = _WaferCount;
	m_nWaferMax = _WaferMax;

	m_nDummyWaferCount = 0;

	m_dThroughput = 0;
	m_nInputWaferCount = 0;
	m_nOutputWaferCount = 0;

	m_hMutex = CreateMutex(NULL, FALSE, NULL);
	m_bIsWorking = false;

	m_bExchangeOver = false;
}

ModuleBase::~ModuleBase()
{
	CloseHandle(m_hMutex);

	// ����ó�� �ʿ� **
	if (m_th.joinable())
	{
		m_th.join();
	}
	else
	{

	}
}

#pragma endregion

#pragma region Get/Set �޼���

void ModuleBase::SetModuleName(CString _value)
{
	m_strModuleName = _value;
}

CString ModuleBase::GetModuleName() const
{
	return m_strModuleName;
}

bool ModuleBase::SetWaferCount(int _value)
{
	if (m_nWaferCount + m_nDummyWaferCount > m_nWaferMax || _value > m_nWaferMax - m_nDummyWaferCount || _value < 0) // WaferMax ��ġ ����
	{
		return false;
	}

	else
	{
		m_nWaferCount = _value;
		return true;
	}
}

int ModuleBase::GetWaferCount() const
{
	return m_nWaferCount;
}

bool ModuleBase::SetDummyWaferCount(int _value)
{
	if (m_nWaferCount + m_nDummyWaferCount > m_nWaferMax || _value > m_nWaferMax - m_nWaferCount || _value < 0) // WaferMax ��ġ ����
	{
		return false;
	}

	else
	{
		m_nWaferCount = _value;
		return true;
	}
}

int ModuleBase::GetDummyWaferCount() const
{
	return m_nWaferCount;
}

void ModuleBase::SetWaferMax(int _value)
{
	m_nWaferMax = _value;
}

int ModuleBase::GetWaferMax() const
{
	return m_nWaferMax;
}

bool ModuleBase::GetIsWorking() const
{
	return m_bIsWorking;
}

bool ModuleBase::SetIsWorking(bool _value)
{
	// bool ��ȯ �ʿ�? == ���߿� ���� �ʿ�
	m_bIsWorking = _value;
	return true;
}

bool ModuleBase::GetSlotValveOpen() const
{
	return m_bSlotValveOpen;
}

bool ModuleBase::GetDoorValveOpen() const
{
	return m_bDoorValveOpen;
}

double ModuleBase::GetThroughput() const
{
	return m_dThroughput;
}

void ModuleBase::SetThroughput()
{
	// Throughput ���� ��� �ʿ� (��� �ν��Ͻ� �� Throughput)
	// ����� ��ü CleanTime���� ��� �� << ���� ���� �ʿ��� �� ����..
	m_dThroughput = m_nOutputWaferCount / (m_dTotalProcessTime - m_dTotalCleanTime);	
}

void ModuleBase::SetTotalThroughput()
{
	// Throughput = Total Time - Total Clean Time
	// ����� Total Clean Time ��� x
	m_dTotalThroughput = s_nTotalOutputWafer / (m_dTotalProcessTime - m_dTotalCleanTime);
}
#pragma endregion

#pragma region ������ �޼���

void ModuleBase::Resume()
{
	if (m_th.joinable())
	{
		ResumeThread(m_th.native_handle());
	}
}

void ModuleBase::Suspend()
{
	if (m_th.joinable())
	{
		SuspendThread(m_th.native_handle());
	}
}

bool ModuleBase::IsRunning()
{
	if (m_th.joinable())
	{
		return true;
	}
	else
	{
		return false;
	}
}

HANDLE ModuleBase::hThread()
{
	if (m_th.joinable())
		return m_th.native_handle();
}

#pragma endregion

#pragma region Convert �޼���



void ModuleBase::SaveCSVModule(int nIdx, CString strFilePath, CStdioFile& cFile, int nHour, int nMin, int nSec)
{
	CString strTemp;
	strTemp.Format(_T("%d,  "), nIdx);
	cFile.WriteString(strTemp);

	strTemp.Format(ConvertModuleType() + _T(",  "));
	cFile.WriteString(strTemp);

	strTemp.Format(GetModuleName() + _T(",  "));
	cFile.WriteString(strTemp);

	strTemp.Format(ConvertWaferMax() + _T(",  "));
	cFile.WriteString(strTemp);

	strTemp.Format(_T("%02d:%02d:%02d,  "), nHour, nMin, nSec);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("%d,  "), m_nInputWaferCount);
	cFile.WriteString(strTemp);

	strTemp.Format(_T("%d,  "), m_nOutputWaferCount);
	cFile.WriteString(strTemp);

	// Thruput �����;��� (CleanProcessTime ���� �� ��� �ʿ�)
	strTemp.Format(_T("%.2lf,  \n"), GetThroughput());
	cFile.WriteString(strTemp);
}

CString ModuleBase::ConvertModuleType()
{
	CString strType;

	switch (m_eModuleType)
	{
	case TYPE_LPM:
	{
		strType = _T("TYPE_LPM");
		break;
	}
	case TYPE_ATMROBOT:
	{
		strType = _T("TYPE_ATMROBOT");
		break;
	}
	case TYPE_LOADLOCK:
	{
		strType = _T("TYPE_LOADLOCK");
		break;
	}
	case TYPE_VACROBOT:
	{
		strType = _T("TYPE_VACROBOT");
		break;
	}
	case TYPE_PROCESSCHAMBER:
	{
		strType = _T("TYPE_PROCESSCHAMBER");
		break;
	}
	}

	return strType;
}

CString ModuleBase::ConvertWaferMax()
{
	CString strWaferMax;
	strWaferMax.Format(_T("%d"), m_nWaferMax);

	return strWaferMax;
}

#pragma endregion
