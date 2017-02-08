#include "LocalSearch.h"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "BioCommonInterface.h"
#include "BioUtilityInterface.h"

using namespace BIOUtility;

CLocalSearch::CLocalSearch(void) : 
	m_RemovedCharacterNum(0),
	m_MaxInterval(MAXINTERVAL), 
	m_pConfig(nullptr)
{
}

CLocalSearch::~CLocalSearch(void)
{
	delete m_pConfig;
}

//*************************************************************************
//FUNCTION:
void CLocalSearch::init()
{
	m_RemovedCharacterNum = BIOCommon::generateUniformIntDistribution(2, 5);
	m_MaxInterval = m_pConfig->getAttribute<int>("MAX_INTERVAL");
}

//*********************************************************************************
//FUNCTION:
bool CLocalSearch::searchLocally(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution)
{
	_ASSERT(vSequenceSet.size() > 0 && vioSolution.size() > 0);

	init();

	bool IsImproved = true;

	unsigned int SolutionLen = vioSolution.size();
	if (SolutionLen < 3) return false;
	while (SolutionLen <= m_RemovedCharacterNum)
	{
		m_RemovedCharacterNum = BIOCommon::generateUniformIntDistribution(2, 5);
	}

	unsigned int IterationNum = 0;
	while (IsImproved)
	{
		IsImproved = false;
		unsigned int CurPos = 1;

		while (CurPos < (SolutionLen - m_RemovedCharacterNum))
		{
			IsImproved = IsImproved || __enumerate(vSequenceSet, vioSolution, CurPos); //更新一次
			CurPos++;
		}

		SolutionLen = vioSolution.size();
		++IterationNum;
	}

	return (IterationNum > 1) ? true : false;
}

//*********************************************************************************
//FUNCTION:
void CLocalSearch::__generateBitArray(const unsigned int vBitNum, std::vector<std::string>& voBitArray) const
{
	_ASSERT(vBitNum > 0);

	unsigned int BitArrayMax = static_cast<unsigned int>(pow(2, vBitNum));
	unsigned int BitArrayMin = static_cast<unsigned int>(pow(2, m_RemovedCharacterNum));

	for (auto i=BitArrayMin; i<BitArrayMax; ++i)
	{
		boost::dynamic_bitset<> BinarySwitch(vBitNum, i);
		if (BinarySwitch.count() <= m_RemovedCharacterNum) { continue; }

		std::stringstream BinaryString;
		BinaryString << BinarySwitch;

		voBitArray.push_back(BinaryString.str());
	}
}

//*********************************************************************************
//FUNCTION:
void CLocalSearch::__generateAllIntervals(const std::vector<std::string>& vSequenceSet, const std::vector<std::vector<int>>& vSolution, const unsigned int vPosition, std::vector<std::string>& voIntervals) const
{
	_ASSERT(vSequenceSet.size());
	_ASSERT(vPosition >= 1 && vPosition < (vSolution.size() - m_RemovedCharacterNum));

	for (unsigned int i=0; i<vSequenceSet.size(); ++i)
	{
		unsigned int Start = vSolution[vPosition-1][i] + 1;
		unsigned int MinIntervalLength = vSolution[m_RemovedCharacterNum+vPosition][i] - Start;

		std::string Interval = vSequenceSet[i].substr(Start, MinIntervalLength);
		voIntervals.push_back(Interval);
	}
}

//*********************************************************************************
//FUNCTION:
bool CLocalSearch::__updateBestSolution(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution, const std::string& vLocalBest, const unsigned int vPosition) const
{
	_ASSERT(vSequenceSet.size() > 0 && vLocalBest.size() > 0);
	_ASSERT(vPosition >= 1 && vPosition < (vioSolution.size() - m_RemovedCharacterNum));

	vioSolution.erase(vioSolution.begin()+vPosition, vioSolution.begin()+vPosition+m_RemovedCharacterNum);

	std::vector<int> TempVec;
	for (unsigned int i=0; i<vLocalBest.size(); i++)
	{
		for (unsigned int k=0; k<vSequenceSet.size(); ++k)
		{
			unsigned int IntervalStart = vioSolution[vPosition-1+i][k] + 1;
			unsigned int IntervalEnd   = vioSolution[vPosition+i][k];

			unsigned int Pos = vSequenceSet[k].find_first_of(vLocalBest[i], IntervalStart);
			if ((Pos == std::string::npos) || (Pos >= IntervalEnd))
			{
				hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Failed to update best solution.");
				return false;
			}	

			TempVec.push_back(Pos);
		}

		vioSolution.insert(vioSolution.begin() + vPosition + i, TempVec);
		TempVec.clear();
	}

	return true;
}

//*********************************************************************************
//FUNCTION:
bool CLocalSearch::__enumerate(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution, unsigned int vPosition) const
{
	_ASSERT(vSequenceSet.size() > 0);
	_ASSERT(vPosition >= 1 && vPosition < (vioSolution.size() - m_RemovedCharacterNum));

	int MinIntervalIndex           = __calculateMinIntervalIndex(vSequenceSet, vioSolution, vPosition).first;
	unsigned int MinIntervalLength = __calculateMinIntervalIndex(vSequenceSet, vioSolution, vPosition).second;

	if (MinIntervalIndex == -1)
		return false;

	std::vector<std::string> BitArray;
	__generateBitArray(MinIntervalLength, BitArray);
	std::vector<std::string> Intervals;
	__generateAllIntervals(vSequenceSet, vioSolution, vPosition, Intervals);

	bool IsImprove = false;

	std::string LocalBest;
	for (auto Iter : BitArray)
	{
		std::string SubSequence = __selectSubSequenceOfInterval(Iter, Intervals.at(MinIntervalIndex));
		if (BIOUtility::isCommonSequence(Intervals, SubSequence))
		{
			IsImprove = true;
			
			if (SubSequence.size() > LocalBest.size())
				LocalBest = SubSequence;	
		}		
	}

	if (IsImprove)
	{
		if (__updateBestSolution(vSequenceSet, vioSolution, LocalBest, vPosition))
			hiveCommon::hiveOutputEvent("Done local search.", hiveCommon::INFO_LEVEL);
		else
			IsImprove = false;
	}

	return IsImprove;
}

//*********************************************************************************
//FUNCTION:
std::pair<int, unsigned int> CLocalSearch::__calculateMinIntervalIndex(const std::vector<std::string>& vSequenceSet, const std::vector<std::vector<int>>& vSolution, unsigned int vPosition) const
{
	_ASSERT(vSequenceSet.size() > 0);
	_ASSERT(vPosition >= 1 && vPosition < (vSolution.size() - m_RemovedCharacterNum));

	unsigned int SeqNum = vSequenceSet.size();
	unsigned int MinIntervalLength = m_MaxInterval;
	int MinIntervalIndex  = -1;

	for (unsigned int SeqIndex=0; SeqIndex<SeqNum; SeqIndex++)
	{
		unsigned int StartPos = vSolution[vPosition-1][SeqIndex] + 1;
		unsigned int TempIntervalLength = vSolution[m_RemovedCharacterNum+vPosition][SeqIndex] - StartPos; //EndPos - StartPos;

		if (TempIntervalLength == m_RemovedCharacterNum)
			return std::make_pair(-1, m_RemovedCharacterNum); 

		if (TempIntervalLength < MinIntervalLength)
		{
			MinIntervalLength = TempIntervalLength;
			MinIntervalIndex  = SeqIndex;
		}
	}

	return std::make_pair(MinIntervalIndex, MinIntervalLength); 
}

//*********************************************************************************
//FUNCTION:
std::string CLocalSearch::__selectSubSequenceOfInterval(const std::string& vBinarySwitchStr, const std::string& vSourceStr) const
{
	_ASSERT(vBinarySwitchStr.size() == vSourceStr.size());

	std::string SubSequence;
	for (unsigned int i=0; i<vBinarySwitchStr.size(); ++i)
	{
		if (vBinarySwitchStr[i] == '1')
			SubSequence.push_back(vSourceStr[i]);
	}

	return SubSequence;
}