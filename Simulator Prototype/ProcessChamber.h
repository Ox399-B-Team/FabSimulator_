#pragma once
#include "ModuleBase.h"

class ProcessChamber : public ModuleBase
{
#pragma region �ʵ�
private:
	int m_nProcessTime;					// Process ���� �ð�
	int m_nCleanTime;					// Clean ���� �ð�	
	int m_nSlotValveOpenTime;			// LL ���� VALVE ���� �ð�
	int m_nSlotValveCloseTime;			// LL ���� VALVE �ݴ� �ð�
	int m_nCleanCount;					// Process ��� ���� �� Clean ������ ������ ���� ��ġ
	int m_nProcessCount;

public:
	HANDLE m_hPmWaferCntChangeEvent;
	static int s_nCntPMWorkOver;

#pragma endregion

#pragma region ������/�Ҹ���
public:
	ProcessChamber(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col,
		int _ProcessTime, int _CleanTime, int _SlotOpenTime, int _SlotCloseTime, int _CleanCount);
	~ProcessChamber();
#pragma endregion

#pragma region Get/Set �޼���
public:
	void SetProcessTime(int _value);
	int GetProcessTime() const;
	void SetCleanTime(int _value);
	int GetCleanTime() const;
	void SetSlotValveOpenTime(int _value);
	int GetSlotValveOpenTime() const;
	void SetSlotValveCloseTime(int _value);
	int GetSlotValveCloseTime() const;
	void SetCleanCount(int _value);
	int GetCleanCount() const;
	int GetProcessCount() const;

#pragma endregion
	
#pragma region �޼���
public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	void work();
	void Run();
#pragma endregion
};