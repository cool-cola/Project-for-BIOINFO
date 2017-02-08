#pragma once
#include <string>
#include <vector>
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace BIOLCS
{
	const int    CANDIDATESNUM      = 10; /*different with DNA-10 and Protein-24*/
	const double PHEROMONEINFLUENCE = 1.0;
	const double HEURISTICINFLUENCE = 2.0;
	const double EXPLOITPROBABILITY = 0.9;
	const double EXCLUDEPROBABILITY = 0.95;

	class CPheromone;

	class CAnt
	{
	public:
		CAnt(void);
		~CAnt(void);

		void fetchSolution(std::vector<std::vector<int>>& voSolution) const {voSolution = m_AntMemory;}
		const std::vector<std::vector<int>>& getSolution() const {return m_AntMemory;}
		const int getSolutionSize() const {return m_AntMemory.size();}

		void init();
		void resetAntMemory();
		void initAntMemory(const int vColumn);				
		void constructAntSolution(const std::vector<std::string>& vSequenceSet);

		void setPheromone(CPheromone* vPheromone)              {_ASSERT(vPheromone); m_pPheromone = vPheromone;}
		void setSequenceNum(const int vSequenceNum)            {_ASSERT(vSequenceNum); m_SequenceNum = vSequenceNum;}
		void setSequenceLen(const int vSequenceLen)            {_ASSERT(vSequenceLen); m_SequenceLen = vSequenceLen;}
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

	private:
		int                            m_SequenceNum;
		int                            m_SequenceLen;
		int                            m_CandidatesNum;
		double                         m_PheromoneInfluence;
		double                         m_HeuristicInfluence;
		double                         m_ExploitProbability;
		double                         m_ExcludeProbability;		
		std::vector<std::vector<int>>  m_AntMemory;

		const CPheromone*	           m_pPheromone;    
		const hiveConfig::CHiveConfig* m_pConfig;      

		unsigned int __findCandidateCharIndexWithMaxTransitionFactor(const std::vector<double>& vTransitionFactors) const;

		void __addSolution(const std::vector<int>& vNextPosition);
		
		bool __traverseCandidateWindow(const std::vector<std::string>& vSequenceSet, const int vSequenceIndex, int& vioCurPos, std::vector<std::vector<int>>& vioCandidate, std::vector<double>& vioTransitionFactors);
		bool __calculateCorrespondPosition(const char vChar, const std::vector<std::string>& vSequenceSet, std::vector<int>& voNextPosition) const;
		bool __isDoubleEqualsZero(const double vValue) const;

		double __calculateTransitionFactor(const int vStringIndex, const std::vector<int>& vNextPosition) const;
		double __calculateHeuristicFactor(const std::vector<int>& vNextPosition) const;
	};
}