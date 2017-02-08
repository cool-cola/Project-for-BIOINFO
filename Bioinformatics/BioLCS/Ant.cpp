#include "Ant.h"
#include "Pheromone.h"
#include "BioUtility.h"
#include "BioCommonInterface.h"

using namespace BIOLCS;

CAnt::CAnt(void) : 
	m_SequenceNum(0), 
	m_SequenceLen(SEQUENCELEN), 
	m_CandidatesNum(CANDIDATESNUM), 
	m_ExploitProbability(EXPLOITPROBABILITY),
	m_ExcludeProbability(EXCLUDEPROBABILITY), 
	m_PheromoneInfluence(PHEROMONEINFLUENCE), 
	m_HeuristicInfluence(HEURISTICINFLUENCE), 
	m_pPheromone(nullptr), 
	m_pConfig(nullptr)
{
}

CAnt::~CAnt(void)
{
	delete m_pConfig;
}

//*************************************************************************
//FUNCTION:
void CAnt::init()
{
	m_CandidatesNum      = m_pConfig->getAttribute<int>("CANDIDATES_NUM");
	m_ExploitProbability = m_pConfig->getAttribute<float>("EXPLOIT_PROBABILITY");
	m_ExcludeProbability = m_pConfig->getAttribute<float>("EXCLUDE_PROBABILITY");
	m_PheromoneInfluence = m_pConfig->getAttribute<float>("PHEROMONE_INFLUENCE");
	m_HeuristicInfluence = m_pConfig->getAttribute<float>("HEURISTIC_INFLUENCE");

	initAntMemory(m_SequenceNum);
}

//*********************************************************************************
//FUNCTION:
void CAnt::constructAntSolution(const std::vector<std::string>& vSequenceSet) 
{
	int SelectedNum = BIOCommon::generateUniformIntDistribution(0, m_SequenceNum - 1);

	int CurPos = 0;
	while (CurPos < m_SequenceLen)
	{
		if (vSequenceSet[SelectedNum][CurPos] == g_PadChar) break;

		std::vector<std::vector<int>> Candidates;
		Candidates.resize(m_CandidatesNum);
	    std::vector<double> TransitionFactors;
	    TransitionFactors.resize(m_CandidatesNum);

		if (!__traverseCandidateWindow(vSequenceSet, SelectedNum, CurPos, Candidates, TransitionFactors)) continue;

		double RandomNum = BIOCommon::generateUniformRealDistribution(0, 1);
		if (RandomNum <= m_ExploitProbability)
		{
			int CandidateCharIndex = __findCandidateCharIndexWithMaxTransitionFactor(TransitionFactors);
			__addSolution(Candidates[CandidateCharIndex]);
			CurPos += CandidateCharIndex + 1;
		}
		else if (RandomNum > m_ExploitProbability && RandomNum <= m_ExcludeProbability)
		{
			int AddIndex = BIOCommon::generateUniformIntDistribution(0, m_CandidatesNum - 1);
			while (TransitionFactors[AddIndex] < 0.000001)
				AddIndex = (AddIndex + 1) % m_CandidatesNum;
			
			__addSolution(Candidates[AddIndex]);
			CurPos += AddIndex + 1;
		}
		else
			CurPos += m_CandidatesNum;
	}
}

//*************************************************************************
//FUNCTION:
bool CAnt::__traverseCandidateWindow(const std::vector<std::string>& vSequenceSet, const int vSequenceIndex, int& vioCurPos, std::vector<std::vector<int>>& vioCandidate, std::vector<double>& vioTransitionFactors)
{
	double SumTransitionFactor = 0.0;

	for (unsigned int i=0; i<vioCandidate.size() && vioCurPos+i<m_SequenceLen; ++i)
	{
		char CandidateChar = vSequenceSet[vSequenceIndex][vioCurPos+i];
		if (CandidateChar == g_PadChar)
			break;

		if (__calculateCorrespondPosition(CandidateChar, vSequenceSet, vioCandidate[i]))
			vioTransitionFactors[i] = __calculateTransitionFactor(vSequenceIndex, vioCandidate[i]);

		SumTransitionFactor += vioTransitionFactors[i];
	}

	if (__isDoubleEqualsZero(SumTransitionFactor))
	{
		vioCurPos += m_CandidatesNum;
		return false;
	}

	for (unsigned int i=0; i<m_CandidatesNum; ++i)
		vioTransitionFactors[i] /= SumTransitionFactor;

	return true;
}

//*********************************************************************************
//FUNCTION:
void CAnt::initAntMemory(const int vColumn)
{
	m_AntMemory.resize(1);

	for (unsigned int i=0; i<vColumn; ++i)
		m_AntMemory[0].push_back(-1);
}

//*********************************************************************************
//FUNCTION:
void CAnt::resetAntMemory()
{
	m_AntMemory.clear();
	initAntMemory(m_SequenceNum);
}

//*********************************************************************************
//FUNCTION:
unsigned int CAnt::__findCandidateCharIndexWithMaxTransitionFactor(const std::vector<double>& vTransitionFactors) const
{
	unsigned int MaxIndex = 0;
	for (unsigned int i=0; i<vTransitionFactors.size(); ++i)
	{
		if (vTransitionFactors[i] > vTransitionFactors[MaxIndex]) MaxIndex = i;
	}

	return MaxIndex;
}

//*********************************************************************************
//FUNCTION:
void CAnt::__addSolution(const std::vector<int>& vNextPosition)
{
	m_AntMemory.push_back(vNextPosition);
}

//*********************************************************************************
//FUNCTION:
bool CAnt::__calculateCorrespondPosition(const char vChar, const std::vector<std::string>& vSequenceSet, std::vector<int>& voNextPosition) const
{
	_ASSERT(m_AntMemory.size() > 0);

	voNextPosition.resize(vSequenceSet.size());

	for (unsigned int i=0; i<vSequenceSet.size(); ++i)
	{
		int CurPos = vSequenceSet[i].find_first_of(vChar, m_AntMemory[m_AntMemory.size()-1][i] + 1);

		if (CurPos == std::string::npos)
			return false;

		voNextPosition[i] = CurPos;
	}
	return true;
}

//*********************************************************************************
//FUNCTION:
bool CAnt::__isDoubleEqualsZero(const double vValue) const
{
	return vValue < 0.000000001 ? true : false;
}

//*********************************************************************************
//FUNCTION:
double CAnt::__calculateTransitionFactor(const int vStringIndex, const std::vector<int>& vNextPosition) const
{
	double PheromoneFactor = m_pPheromone->getPheromone(vStringIndex, vNextPosition[vStringIndex]);
	double HeuristicFactor = __calculateHeuristicFactor(vNextPosition);

	return pow(PheromoneFactor, m_PheromoneInfluence) * pow(HeuristicFactor, m_HeuristicInfluence);
}

//*************************************************************************
//FUNCTION:
double CAnt::__calculateHeuristicFactor(const std::vector<int>& vNextPosition) const
{
	_ASSERT(m_AntMemory.size() > 0);

	double Result = 0.0;
	int SolutionSize = m_AntMemory.size();

	for (unsigned int i=0; i<vNextPosition.size(); ++i)
	{
		int PrePosition = m_AntMemory[SolutionSize-1][i];
		int RemainNum   = m_SequenceLen - PrePosition - 1;
		int SkipNum     = vNextPosition[i] - PrePosition - 1;

		if (RemainNum == 0)
			Result += (double)SkipNum;
		else
			Result += (double)SkipNum / RemainNum; 
	}

	if (__isDoubleEqualsZero(Result))
		Result = 1.0;

	return 1.0 / Result;
}