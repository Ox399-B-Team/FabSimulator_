#pragma once
#include "pch.h"

enum ModuleType
{
	TYPE_LPM,
	TYPE_ATMROBOT,
	TYPE_LOADLOCK,
	TYPE_VACROBOT,
	TYPE_PROCESSCHAMBER
};

class ModuleBase
{
#pragma region 필드
protected:								// 자식 클래스들의 Get/Set 메서드를 통한 Wafer Max 수치 제한이 다를 수 있음을 생각, 접근이 허용되어야 함.. 그래서 protected
	int m_nWaferCount;					// 현재 모듈이 가지는 Wafer 개수s
	int m_nWaferMax;					// 현재 모듈이 가질 수 있는 Wafer Max 수치
	bool m_bIsWorking;					// 현재 모듈이 동작중임을 나타내는 필드
	CString m_strModuleName;			// 현재 모듈의 이름(기계명, 중복 x)

	bool m_bDoorValveOpen;
	bool m_bSlotValveOpen;

	class Pick_PlaceM
	{
	public:
		vector<ModuleBase*> m_vPickModule;
		vector<ModuleBase*> m_vPlaceModule;

		CListCtrl* m_pClistCtrl;
	};
public:
	int m_nRow;							// ListCtrl에 들어갈 Row값
	int m_nCol;							// ListCtrl에 들어갈 Col값
	ModuleType m_eModuleType;			// 모듈 타입(메타데이터)
	HANDLE m_hMutex;
	bool m_bExchangeOver;

	thread m_th;

	/////////////로직
	static bool s_bDirect;

#pragma endregion

#pragma region 생성자/소멸자
public:
	ModuleBase(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col);
	~ModuleBase();
#pragma endregion

#pragma region Get/Set 메서드
public:
	void SetModuleName(CString _value);
	CString GetModuleName() const;

	bool SetWaferCount(int _value);
	int GetWaferCount() const;

	void SetWaferMax(int _value);
	int GetWaferMax() const;

	bool GetIsWorking() const;
	bool SetIsWorking(bool _value);

	bool GetSlotValveOpen() const;
	bool GetDoorValveOpen() const;

#pragma endregion

#pragma region 메서드

public:
	void Resume();
	void Suspend();

	bool IsRunning();
	HANDLE hThread(); 

#pragma endregion
};