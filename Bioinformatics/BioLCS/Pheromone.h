#pragma once
#include <vector>
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace BIOLCS
{
	const int    SEQUENCELEN     = 700;
	const double EVAPORATIONRATE = 0.004;
	const double INITPHEROMONE   = 0.5;
	const double MINPHEROMONE    = 0.01;
	const double MAXPHEROMONE    = 0.99;
	const double IBPHEROMONE     = 0.1;
	const double BSPHEROMONE     = 0.1;

	class CPheromone
	{
	public:
		CPheromone(void);
		CPheromone(int vSequenceNum);
		~CPheromone(void);

		void init();
		void update(const std::vector<std::vector<int>>& vBestSolution, const std::vector<std::vector<int>>& vIterBestSolution, const int vUpperBound);
		
		void setSequenceLen(const int vSequenceLen)            {_ASSERT(vSequenceLen); m_SequenceLen = vSequenceLen;}
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

		const double getPheromone(const int vIndexX, const int vIndexY) const;

	private:
		int                              m_SequenceNum;
		int                              m_SequenceLen;
		double                           m_EvaporationRate;
		double                           m_InitPheromone;
		double                           m_MinPheromone;
		double                           m_MaxPheromone;
		double                           m_IBPheromone;
		double                           m_BSPheromone;
		const hiveConfig::CHiveConfig*   m_pConfig;
		std::vector<std::vector<double>> m_PheromoneMatrix;

		void __evaporate();
		void __deposit(const std::vector<std::vector<int>>& vBestSolution, const std::vector<std::vector<int>>& vIterBestSolution, const int vUpperBound);
		void __updateSolutionPheromone(const std::vector<std::vector<int>>& vSolution, const double vDeltaPheromone, const double vPheromoneFactor);

		double __calculateDeltaPheromone(const std::vector<std::vector<int>>& vSolution, const int vUpperBound);
	};
}