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

	double m_dThroughput;				// 각 모듈 별 Throughput
	//double m_dCleanTime;				// 각 모듈 별 Clean Process 진행시간 (Hour)

	bool m_bDoorValveOpen;
	bool m_bSlotValveOpen;

	HANDLE m_hThreadCloseSignal;

public:
	static double m_dTotalProcessTime;	// 전체 Process 진행시간 (Hour)			<< 모든 모듈 공통사항
	static double m_dTotalCleanTime;	// 전체 Clean Process 진행시간 (Hour)		<< 모든 모듈 공통사항
	static double m_dTotalThroughput;	// 전체 Process Throughput				<< 모든 모듈 공통사항
	static double s_dSpeed;
	static int s_nTotalInputWafer;
	static int s_nTotalOutputWafer;

	ModuleType m_eModuleType;			// 모듈 타입 (메타데이터)
	int m_nRow;							// ListCtrl에 들어갈 Row값
	int m_nCol;							// ListCtrl에 들어갈 Col값
	int m_nInputWafer;					// 모듈 인스턴스 별 InputWafer
	int m_nOutputWafer;					// 모듈 인스턴스 별 OutputWafer
	HANDLE m_hMutex;
	bool m_bExchangeOver;
	bool m_bStopFlag;
	thread m_th;

	/////////////로직
	static bool s_bDirect;

	//thread가 안전하게 종료할 때까지 기다리기 위한 Event
	static vector<HANDLE> s_vEventCloseThread;

#pragma endregion

#pragma region 생성자/소멸자
public:
	ModuleBase(ModuleType _Type, CString _Name, int _WaferCount, int _WaferMax, int _Row, int _Col);
	virtual ~ModuleBase();
#pragma endregion

#pragma region Get/Set 메서드
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

#pragma region 메서드

public:
	void Resume();
	void Suspend();

	bool IsRunning();
	HANDLE hThread(); 

	virtual void SaveConfigModule(int nIdx, CString strFilePath) = 0;
	virtual void SaveCSVModule(int nIdx, CString strFilePath, CStdioFile& cFile,int nHour, int nMin, int nSec);
	CString ConvertModuleType();
	CString ConvertWaferMax();

	virtual void Run(vector<ModuleBase*> vPickModules, vector<ModuleBase*> vPlaceModules, CListCtrl* pClist) = 0;
#pragma endregion
};