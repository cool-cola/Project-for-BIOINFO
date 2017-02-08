#include "Pheromone.h"

using namespace BIOLCS;

CPheromone::CPheromone(void) : 
	m_SequenceNum(0),
	m_SequenceLen(SEQUENCELEN), 
	m_EvaporationRate(EVAPORATIONRATE), 
	m_IBPheromone(IBPHEROMONE),
	m_BSPheromone(BSPHEROMONE), 
	m_MinPheromone(MINPHEROMONE), 
	m_MaxPheromone(MAXPHEROMONE), 
	m_InitPheromone(INITPHEROMONE), 
	m_pConfig(nullptr)
{
}

CPheromone::CPheromone(int vSequenceNum) : 
	m_SequenceNum(vSequenceNum), 
	m_SequenceLen(SEQUENCELEN), 
	m_EvaporationRate(EVAPORATIONRATE), 
	m_IBPheromone(IBPHEROMONE),
	m_BSPheromone(BSPHEROMONE), 
	m_MinPheromone(MINPHEROMONE), 
	m_MaxPheromone(MAXPHEROMONE), 
	m_InitPheromone(INITPHEROMONE), 
	m_pConfig(nullptr)
{
}

CPheromone::~CPheromone(void)
{
	delete m_pConfig;
}

//*********************************************************************************
//FUNCTION:
void CPheromone::init()
{
	m_EvaporationRate = m_pConfig->getAttribute<float>("EVAPORATION_RATE");
	m_InitPheromone = m_pConfig->getAttribute<float>("INIT_PHEROMONE");
	m_MinPheromone = m_pConfig->getAttribute<float>("MIN_PHEROMONE");
	m_MaxPheromone = m_pConfig->getAttribute<float>("MAX_PHEROMONE");
	m_IBPheromone = m_pConfig->getAttribute<float>("IB_PHEROMONE");
	m_BSPheromone = m_pConfig->getAttribute<float>("BS_PHEROMONE");

	m_PheromoneMatrix.resize(m_SequenceNum);
	for (auto& PheromoneRow : m_PheromoneMatrix)           
	{
		PheromoneRow.resize(m_SequenceLen);
		for (auto& PheromoneValue : PheromoneRow)
			PheromoneValue = m_InitPheromone;
	}
}

//*********************************************************************************
//FUNCTION:
void CPheromone::update(const std::vector<std::vector<int>>& vBestSolution, const std::vector<std::vector<int>>& vIterBestSolution, const int vUpperBound)
{
	_ASSERT(vBestSolution.size() >= 0 && vIterBestSolution.size() > 0);
	_ASSERT(vUpperBound > 0);

	__evaporate();
	__deposit(vBestSolution, vIterBestSolution, vUpperBound);
}

//*********************************************************************************
//FUNCTION:
const double CPheromone::getPheromone(const int vIndexX, const int vIndexY) const
{
	_ASSERT(vIndexX >= 0 && vIndexX < m_SequenceNum);
	_ASSERT(vIndexY >= 0 && vIndexY < m_SequenceLen); 

	return m_PheromoneMatrix[vIndexX][vIndexY]; 
}

//*********************************************************************************
//FUNCTION:
void CPheromone::__evaporate()
{
	for (int i=0; i<m_SequenceNum; ++i)   
	{
		for (int k=0; k<m_SequenceLen; ++k) 
		{
			double PheromoneValue = (1 - m_EvaporationRate) * m_PheromoneMatrix[i][k]; 
			m_PheromoneMatrix[i][k] = PheromoneValue > m_MinPheromone ? PheromoneValue : m_MinPheromone; 
		}
	}
}

//*********************************************************************************
//FUNCTION:
void CPheromone::__deposit(const std::vector<std::vector<int>>& vBestSolution, const std::vector<std::vector<int>>& vIterBestSolution, const int vUpperBound)
{
	_ASSERT(vBestSolution.size() >= 0 && vIterBestSolution.size() > 0);
	_ASSERT(vUpperBound > 0);

	double DeltaBSSolution = __calculateDeltaPheromone(vBestSolution, vUpperBound);
	double DeltaIBSolution = __calculateDeltaPheromone(vIterBestSolution, vUpperBound);

	__updateSolutionPheromone(vBestSolution, DeltaBSSolution, m_BSPheromone); 
	__updateSolutionPheromone(vIterBestSolution, DeltaIBSolution, m_IBPheromone); 
}

//*********************************************************************************
//FUNCTION:
void CPheromone::__updateSolutionPheromone(const std::vector<std::vector<int>>& vSolution, const double vDeltaPheromone, const double vPheromoneFactor)
{
	_ASSERT(vSolution.size() >= 0);

	for (int i=1; i<vSolution.size(); ++i)
	{
		for(int k=0; k<vSolution[i].size(); ++k)
		{
			double PheromoneValue = m_PheromoneMatrix[k][vSolution[i][k]] + vDeltaPheromone * vPheromoneFactor;
			m_PheromoneMatrix[k][vSolution[i][k]] = PheromoneValue > m_MaxPheromone ? m_MaxPheromone : PheromoneValue;
		}
	}
}

//*********************************************************************************
//FUNCTION:
double CPheromone::__calculateDeltaPheromone(const std::vector<std::vector<int>>& vSolution, const int vUpperBound)
{
	_ASSERT(vSolution.size() >= 0 && vUpperBound > 0);
	return (vSolution.size() == 0) ? 0.0 : (double)(vSolution.size() - 1) / vUpperBound;
}