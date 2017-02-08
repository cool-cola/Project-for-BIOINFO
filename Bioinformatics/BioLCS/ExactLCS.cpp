#include "ExactLCS.h"
#include <fstream>
#include <sstream>
#include "common/CommonInterface.h"

using namespace BIOLCS;

CExactLCS::CExactLCS() : m_LCS2Len(0)
{
}

CExactLCS::~CExactLCS(void)
{
}

//*******************************************************************************
//FUNCTION:
bool CExactLCS::findLCS(const std::vector<std::string>& vSeqSet, std::string& voLCS)
{
	_ASSERT(vSeqSet.size() > 0);

	if (__findLCSsOfSeqSet(vSeqSet))
	{
		voLCS = m_LCS;
		__outputEvent();
		return true;
	}
	else
		return false;
}

//*******************************************************************************
//FUNCTION:
void CExactLCS::saveLCS(const std::string& vFileName) const
{
	_ASSERT(vFileName.size() > 0);

	std::ofstream File;
	File.open(vFileName);
	if (!File)
	{
		hiveCommon::hiveOutputEvent("Can not create file.", hiveCommon::INFO_LEVEL);
	}
	else
	{
		for (auto Iter : m_LCSSet)
		{
			File << ">LCS" << std::endl;
			File << Iter << std::endl;
		}
	}
}

//*******************************************************************************
//FUNCTION:
void CExactLCS::__outputEvent() const
{
	auto Iter = m_LCSSet.begin();

	std::stringstream OutputEvent;
	OutputEvent << "The length of LCS is : " << (*Iter).size() << std::endl;
	for (; Iter!=m_LCSSet.end(); Iter++)
	{
		OutputEvent << "  " << *Iter << std::endl;
	}

	hiveCommon::hiveOutputEvent(OutputEvent.str(), hiveCommon::INFO_LEVEL);
}

//*******************************************************************************
//FUNCTION:
void CExactLCS::__fillMat(const std::string& vFirstSeq, const std::string& vSecondSeq)
{
	_ASSERT(vFirstSeq.size() > 0 && vSecondSeq.size() > 0);

	unsigned int Row = vFirstSeq.size() + 1;
	unsigned int Column = vSecondSeq.size() + 1;

	m_LCSMat.resize(Row);
	m_DirMat.resize(Row);

	m_LCSMat[0].resize(Column);
	m_DirMat[0].resize(Column);

	for (unsigned int i=1; i<Row; ++i)
	{
		m_LCSMat[i].resize(Column);
		m_DirMat[i].resize(Column);
		for (unsigned int k=1; k<Column; ++k)
		{
			if (vFirstSeq[i-1] == vSecondSeq[k-1])
			{
				m_LCSMat[i][k] = m_LCSMat[i-1][k-1] + 1;
				m_DirMat[i][k] = 1;
			} 
			else if (m_LCSMat[i-1][k] > m_LCSMat[i][k-1])
			{
				m_LCSMat[i][k] = m_LCSMat[i-1][k];
				m_DirMat[i][k] = 2;
			}
			else if (m_LCSMat[i-1][k] < m_LCSMat[i][k-1])
			{
				m_LCSMat[i][k] = m_LCSMat[i][k-1];
				m_DirMat[i][k] = 3;
			}
			else
			{
				m_LCSMat[i][k] = m_LCSMat[i-1][k];
				m_DirMat[i][k] = 4;
			}
		}
	}
}

//*******************************************************************************
//FUNCTION:
void CExactLCS::__traceBack(const std::string& vSequence, int vRowIndex, int vColumnIndex, int vLCSIndex)
{
	_ASSERT(vSequence.size() > 0);
	_ASSERT(vRowIndex >= 0 && vColumnIndex >= 0 && vLCSIndex >= 0);

	if (vLCSIndex == m_LCS2Len)
	{
		std::string TempLCS = m_LCS;
		reverse(TempLCS.begin(), TempLCS.end());
		m_LCSSet.insert(TempLCS);
		TempLCS.clear();
	} 
	else
	{
		if (m_DirMat[vRowIndex][vColumnIndex] == 1)
		{
			m_LCS[vLCSIndex] = vSequence[vRowIndex-1];
			++vLCSIndex;
			__traceBack(vSequence, vRowIndex-1, vColumnIndex-1, vLCSIndex);
			--vLCSIndex;
			
			m_LCS.erase(vLCSIndex);
			m_LCS.resize(m_LCS2Len);
		} 
		else if (m_DirMat[vRowIndex][vColumnIndex] == 2)
		{
			__traceBack(vSequence, vRowIndex-1, vColumnIndex, vLCSIndex);
		}
		else if (m_DirMat[vRowIndex][vColumnIndex] == 3)
		{
			__traceBack(vSequence, vRowIndex, vColumnIndex-1, vLCSIndex);
		}
		else
		{
			__traceBack(vSequence, vRowIndex-1, vColumnIndex, vLCSIndex);
			__traceBack(vSequence, vRowIndex, vColumnIndex-1, vLCSIndex);
		}
	}
}

//*******************************************************************************
//FUNCTION:
bool CExactLCS::__findLCSsOfTwoSeqs(const std::string& vFirstSeq, const std::string& vSecondSeq)
{
	_ASSERT(vFirstSeq.size() > 0 && vSecondSeq.size() > 0);

	__fillMat(vFirstSeq, vSecondSeq);

	int Row = vFirstSeq.size();
	int Column = vSecondSeq.size();

	m_LCS2Len = m_LCSMat[Row][Column];
	m_LCS.resize(m_LCS2Len);
	__traceBack(vFirstSeq, Row, Column, 0);

	return (m_LCS2Len > 0) ? true : false;
}

//*******************************************************************************
//FUNCTION:
bool CExactLCS::__findLCSsOfSeqAndSeqSet(const std::string& vSequence, std::set<std::string>& vSeqSet)
{
	_ASSERT(vSequence.size() > 0 && vSeqSet.size() > 0);

	int TempLength = 0;
	std::set<std::string> TempLCSSet;

	std::set<std::string> TempSet = vSeqSet;
	for (auto SeqIter : TempSet)
	{
		m_LCSSet.clear();
		if (!__findLCSsOfTwoSeqs(vSequence, SeqIter))
			return false;

		int LCSLength = (*m_LCSSet.begin()).size();
		if (LCSLength > TempLength)
		{
			TempLCSSet.clear();
			TempLCSSet = m_LCSSet;
			TempLength = (*TempLCSSet.begin()).size();
		}
		else if (LCSLength == TempLength)
		{
			for (auto Iter : m_LCSSet)
				TempLCSSet.insert(Iter);
		}
	}
	m_LCSSet.clear();
	m_LCSSet = TempLCSSet;
	TempLCSSet.clear();

	return true;
}

//*******************************************************************************
//FUNCTION:
bool CExactLCS::__findLCSsOfSeqSet(const std::vector<std::string>& vSeqSet)
{
	_ASSERT(vSeqSet.size() > 0);

	if (__findLCSsOfTwoSeqs(vSeqSet.at(0), vSeqSet.at(1)))
	{
		for (unsigned int k=2; k<vSeqSet.size(); ++k) 
		{
			if (k == 16739)
			{
				if (!__findLCSsOfSeqAndSeqSet(vSeqSet.at(k), m_LCSSet))
					return false;
			}

			if (!__findLCSsOfSeqAndSeqSet(vSeqSet.at(k), m_LCSSet))
				return false;
		}
		return true;
	}

	return false;
}