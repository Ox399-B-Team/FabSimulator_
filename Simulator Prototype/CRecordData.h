#pragma once
class CRecordDetail
{
public:
	CString m_strMType;
	CString m_strMName;
	CString m_strWaferMax;
	CString m_strRunTime;
	int m_nIWafer;
	int m_nOWafer;
	double m_dThroughput;
};

class CRecordData
{
public:
	CString m_strCurtime;
	int m_ihour;
	int m_imin;
	int m_isecond;
	int m_itotalout;
	int m_itotalin;
	int m_dtotalthroughput;

	////////////////////////////

	vector<CRecordDetail> m_VRecordDetail;
};

