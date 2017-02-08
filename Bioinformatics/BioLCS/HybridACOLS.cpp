#include "HybridACOLS.h"
#include "Ant.h"
#include "Pheromone.h"
#include "BioUtility.h"
#include "BioCommonInterface.h"
#include "BioUtilityInterface.h"
#include "BioLCSInterface.h"

using namespace BIOLCS;

CHybridACOLS::CHybridACOLS(void) : 
	m_UpperBound(0), 
	m_SequenceNum(0), 
	m_AntNum(ANTNUM), 
	m_SequenceLen(SEQUENCELEN), 
	m_IterationTime(ITERATIONTIME), 
	m_pPheromone(nullptr),
	m_pConfig(nullptr)
{
}

CHybridACOLS::~CHybridACOLS(void)
{
	delete m_pPheromone;
	delete m_pConfig;

	for (auto Ant : m_Ants)
		delete Ant;
}

//*********************************************************************************
//FUNCTION:
bool CHybridACOLS::findLCS(const std::vector<std::string>& vSequenceSet, std::string& voLCS, const bool vIfLocalSearch)
{
	_ASSERT(vSequenceSet.size() > 0);

	m_SequenceSet = vSequenceSet;
	__init();
	
	for (unsigned int k=0; k<m_IterationTime; ++k) 
	{
		for (auto& Ant : m_Ants)
		{
			Ant->resetAntMemory();
			Ant->constructAntSolution(m_SequenceSet);
		}

		unsigned int IterBestLen = 0;
		int IterBestIndex = -1;
		for (unsigned int i=0; i<m_Ants.size(); ++i)
		{
			if (m_Ants[i]->getSolutionSize() > IterBestLen)
			{
				IterBestLen = m_Ants[i]->getSolutionSize();
				IterBestIndex = i;
			}
		}

		if (IterBestIndex == -1)
			continue;
		else
			m_IterBestSolution = m_Ants[IterBestIndex]->getSolution();

		if (vIfLocalSearch)
			BIOUtility::optimizeSolutionByLocalSearch(m_SequenceSet, m_IterBestSolution);

		m_pPheromone->update(m_BestSolution, m_IterBestSolution, m_UpperBound);

		if (m_BestSolution.size() < m_IterBestSolution.size())
			m_BestSolution = m_IterBestSolution;
	}

	voLCS = __calculateSolution();
	return (voLCS.size() > 0);
}

//*********************************************************************************
//FUNCTION:
int CHybridACOLS::__calculateUpperBound() const
{
	unsigned int IndexA = BIOCommon::generateUniformIntDistribution(0, m_SequenceNum-1);
	unsigned int IndexB = BIOCommon::generateUniformIntDistribution(0, m_SequenceNum-1);
	while (IndexA == IndexB)
	{
		IndexB = BIOCommon::generateUniformIntDistribution(0, m_SequenceNum-1);
	}

	std::vector<std::vector<int>> Matrix;
	Matrix.resize(2);
	for (auto& Iter : Matrix)
	{
		Iter.resize(m_SequenceLen + 1);
	}

	unsigned int CurRow = 1;
	for (unsigned int i=0; i<m_SequenceLen; ++i)
	{
		for (unsigned int k=1; k<=m_SequenceLen; ++k)
		{
			if (m_SequenceSet[IndexA][i] == m_SequenceSet[IndexB][k-1])
				Matrix[CurRow][k] = Matrix[!CurRow][k-1] + 1;
			else 
				Matrix[CurRow][k] = std::max(Matrix[!CurRow][k], Matrix[CurRow][k-1]);
		}
		CurRow = !CurRow;
	}

	return Matrix[!CurRow][m_SequenceLen];
}

//*********************************************************************************
//FUNCTION:
void CHybridACOLS::__init()
{	
	m_SequenceLen = m_pConfig->getAttribute<int>("SEQUENCE_LEN");
	m_IterationTime = m_pConfig->getAttribute<int>("ITERATION_TIME");
	m_AntNum = m_pConfig->getAttribute<int>("ANT_NUM");

	for (auto& Sequence : m_SequenceSet)
		__pruneSequences(Sequence);

	m_SequenceNum = m_SequenceSet.size();
	m_UpperBound  = __calculateUpperBound();

	m_pPheromone = new CPheromone(m_SequenceNum);   
	m_pPheromone->setConfig(m_pConfig->findSubConfigByName("PHEROMONE"));
	m_pPheromone->setSequenceLen(m_SequenceLen); 
	m_pPheromone->init();

	for (unsigned int i=0; i<m_AntNum; ++i)
	{
		CAnt* pAnt = new CAnt;
		pAnt->setSequenceLen(m_SequenceLen);
		pAnt->setSequenceNum(m_SequenceNum);
		pAnt->setPheromone(m_pPheromone);
		pAnt->setConfig(m_pConfig->findSubConfigByName("ANT"));
		pAnt->init();

		m_Ants.push_back(pAnt);
	}
}

//*********************************************************************************
//FUNCTION:
void CHybridACOLS::__pruneSequences(std::string& vioSequence) const
{
	_ASSERT(vioSequence.size() > 0);

	if (vioSequence.size() > m_SequenceLen)  
		vioSequence = vioSequence.substr(0, m_SequenceLen); 
	else if (vioSequence.size() < m_SequenceLen) 
		vioSequence.append(m_SequenceLen-vioSequence.size(), g_PadChar); 
}

//*********************************************************************************
//FUNCTION:
std::string CHybridACOLS::__calculateSolution() const
{
	std::string Result;
	for (unsigned int i=1; i<m_BestSolution.size(); ++i)
		Result.push_back(m_SequenceSet[0][m_BestSolution[i][0]]);

	return Result;
}