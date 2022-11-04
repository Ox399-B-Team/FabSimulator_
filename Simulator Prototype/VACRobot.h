#pragma once
#include "ModuleBase.h"

class VACRobot : public ModuleBase
{
#pragma region �ʵ�
private:
	int m_nPickTime;					// RobotArm���� Wafer�� �������� �ð�
	int m_nPlaceTime;					// RobotArm�� Wafer�� ��� Module�� ���� �ð�
	int m_nRotateTime;					// Robot�� ȸ���ð�

	vector<ModuleBase*> m_vPickModule;
	vector<ModuleBase*> m_vPlaceModule;

public:
	static int s_nCount;
	static vector<VACRobot*> s_pVACRobot;

#pragma endregion

#pragma region ������/�Ҹ���
public:
	VACRobot(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col, int _PickTime, int _PlaceTime, int _RotateTime);
	virtual ~VACRobot();
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
private:
	void WorkThread();
	bool PickWafer(ModuleBase* pM);
	bool PlaceWafer(ModuleBase* pM);

public:
	virtual void SaveConfigModule(int nIdx, CString strFilePath);
	void Run(vector<ModuleBase*> _vPickModules, vector<ModuleBase*> _vPlaceModules, CListCtrl* _pClist);

#pragma endregion
};