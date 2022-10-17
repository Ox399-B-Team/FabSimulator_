#pragma once
#include "ModuleBase.h"

class ProcessChamber : public ModuleBase
{
#pragma region 필드
private:
	int m_nProcessTime;					// Process 진행 시간
	int m_nCleanTime;					// Clean 진행 시간	
	int m_nSlotValveOpenTime;			// LL 연결 VALVE 여는 시간
	int m_nSlotValveCloseTime;			// LL 연결 VALVE 닫는 시간
	int m_nCleanCount;					// Process 몇번 진행 후 Clean 진행할 것인지 설정 수치
	int m_nProcessCount;

public:
	static int s_nCntPMWorkOver;
	static vector<HANDLE> s_vWorkOverHandle;

	HANDLE m_hPmWaferCntChangeEvent;
	HANDLE m_hPMWorkOver;
	HANDLE m_hInitWorkOver;

	int m_bNecessaryDummyWafer;

#pragma endregion

#pragma region 생성자/소멸자
public:
	ProcessChamber(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col,
		int _ProcessTime, int _CleanTime, int _SlotOpenTime, int _SlotCloseTime, int _CleanCount);
	virtual ~ProcessChamber();
#pragma endregion

#pragma region Get/Set 메서드
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
	
#pragma region 메서드
private:
	void WorkThread();

public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	void Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist);
#pragma endregion
};