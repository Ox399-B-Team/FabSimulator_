#pragma once
#include "pch.h"

class ModuleBase;

class Pick_PlaceM
{
public:
	vector<ModuleBase*> m_vPickModule;
	vector<ModuleBase*> m_vPlaceModule;

	CListCtrl* m_pClistCtrl;
};

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

	double m_dThroughput;				// �� ��� �� Throughput
	//double m_dCleanTime;				// �� ��� �� Clean Process ����ð� (Hour)
	int m_nDummyWaferCount;				// ���� ���� �� ��뿹��?

	bool m_bDoorValveOpen;
	bool m_bSlotValveOpen;

public:
	static double m_dTotalProcessTime;	// ��ü Process ����ð� (Hour)			<< ��� ��� �������
	static double m_dTotalCleanTime;	// ��ü Clean Process ����ð� (Hour)		<< ��� ��� �������
	static double m_dTotalThroughput;	// ��ü Process Throughput				<< ��� ��� �������
	static double s_dSpeed;
	static int s_nTotalInputWafer;
	static int s_nTotalOutputWafer;

	ModuleType m_eModuleType;			// ��� Ÿ�� (��Ÿ������)
	int m_nRow;							// ListCtrl�� �� Row��
	int m_nCol;							// ListCtrl�� �� Col��
	int m_nInputWafer;					// ��� �ν��Ͻ� �� InputWafer
	int m_nOutputWafer;					// ��� �ν��Ͻ� �� OutputWafer
	HANDLE m_hMutex;
	bool m_bExchangeOver;
	bool m_bStopFlag;
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

	bool SetDummyWaferCount(int _value);
	int GetDummyWaferCount() const;

	void SetWaferMax(int _value);
	int GetWaferMax() const;

	bool GetIsWorking() const;
	bool SetIsWorking(bool _value);

	bool GetSlotValveOpen() const;
	bool GetDoorValveOpen() const;

	double GetThroughput() const;
	void SetThroughput();

	static void SetTotalThroughput();
#pragma endregion

#pragma region �޼���

public:
	void Resume();
	void Suspend();

	bool IsRunning();
	HANDLE hThread(); 

	virtual void SaveConfigModule(int nIdx, CString strFilePath) = 0;
	virtual void SaveCSVModule(int nIdx, CString strFilePath, CStdioFile& cFile,int nHour, int nMin, int nSec);
	CString ConvertModuleType();
	CString ConvertWaferMax();
#pragma endregion
};