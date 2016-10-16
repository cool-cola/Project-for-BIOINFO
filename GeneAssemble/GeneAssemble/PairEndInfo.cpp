#include "PairEndInfo.h"

GeneAssemble::CPairEndInfo::CPairEndInfo()
{
}

GeneAssemble::CPairEndInfo::~CPairEndInfo()
{
}

//*************************************************************************
//FUNCTION:
long GeneAssemble::CPairEndInfo::findPairedReadID(long vCurReadID)
{
	auto Iter = std::find(m_HeadReadIDSet.begin(), m_HeadReadIDSet.end(), vCurReadID);
	if (Iter != m_HeadReadIDSet.end())
	{
		return m_TailReadIDSet[Iter-m_HeadReadIDSet.begin()];
	}
	
	Iter = std::find(m_TailReadIDSet.begin(), m_TailReadIDSet.end(), vCurReadID);
	if (Iter != m_TailReadIDSet.end())
	{
		return m_HeadReadIDSet[Iter-m_TailReadIDSet.begin()];
	}

	return -1;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CPairEndInfo::addHeadReadID(long vHeadReadID)
{
	m_HeadReadIDSet.push_back(vHeadReadID);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CPairEndInfo::addTailReadID(long vTailReadID)
{
	m_TailReadIDSet.push_back(vTailReadID);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CPairEndInfo::addInsertLength(long vInsertLength)
{
	m_InsertLengthSet.push_back(vInsertLength);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CPairEndInfo::addPairEndInfo(long vHeadReadID, long vTailReadID, long vInsertLength)
{
	m_HeadReadIDSet.push_back(vHeadReadID);
	m_TailReadIDSet.push_back(vTailReadID);
	m_InsertLengthSet.push_back(vInsertLength);
}