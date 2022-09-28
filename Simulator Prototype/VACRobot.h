#pragma once
#include "ModuleBase.h"

class VACRobot : public ModuleBase
{
#pragma region �ʵ�
private:
	int m_nPickTime;					// RobotArm���� Wafer�� �������� �ð�
	int m_nPlaceTime;					// RobotArm�� Wafer�� ��� Module�� ���� �ð�
	int m_nRotateTime;					// Robot�� ȸ���ð�

public:
	static HANDLE s_hVACRobotExchangeOver;
#pragma endregion

#pragma region ������/�Ҹ���
public:
	VACRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime);
	~VACRobot();
#pragma endregion

#pragma region Get/Set �޼���
public:
	void SetPickTime(int _value);
	int GetPickTime() const;
	void SetPlaceTime(int _value);
	int GetPlaceTime() const;
	void SetRotateTime(int _value);
	int GetRotateTime() const;
#pragma endregion

#pragma region �޼���
public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	bool PickWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	bool PlaceWafer(ModuleBase* pM, CListCtrl* pClistCtrl);
	void work(Pick_PlaceM Pick_Place);
	void Run(vector<ModuleBase*> m_vPickModules, vector<ModuleBase*> m_vPlaceModules, CListCtrl* pClist);

#pragma endregion
};