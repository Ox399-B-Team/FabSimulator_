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
#pragma region �ʵ�
protected:								// �ڽ� Ŭ�������� Get/Set �޼��带 ���� Wafer Max ��ġ ������ �ٸ� �� ������ ����, ������ ���Ǿ�� ��.. �׷��� protected
	int m_nWaferCount;					// ���� ����� ������ Wafer ����s
	int m_nWaferMax;					// ���� ����� ���� �� �ִ� Wafer Max ��ġ
	bool m_bIsWorking;					// ���� ����� ���������� ��Ÿ���� �ʵ�
	CString m_strModuleName;			// ���� ����� �̸�(����, �ߺ� x)

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
	int m_nRow;							// ListCtrl�� �� Row��
	int m_nCol;							// ListCtrl�� �� Col��
	ModuleType m_eModuleType;			// ��� Ÿ��(��Ÿ������)
	HANDLE m_hMutex;
	bool m_bExchangeOver;

	thread m_th;

	/////////////����
	static bool s_bDirect;

#pragma endregion

#pragma region ������/�Ҹ���
public:
	ModuleBase(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col);
	~ModuleBase();
#pragma endregion

#pragma region Get/Set �޼���
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

#pragma region �޼���

public:
	void Resume();
	void Suspend();

	bool IsRunning();
	HANDLE hThread(); 

#pragma endregion
};