#pragma once
#include <vector>
#include <string>
#include "common/Singleton.h"
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace BIOLCS
{
	const int ITERATIONTIME = 5;
	const int ANTNUM        = 16;

	class CPheromone;
	class CAnt;

	class CHybridACOLS : public hiveCommon::CSingleton<CHybridACOLS>   
	{
	public:
		~CHybridACOLS(void);

		bool findLCS(const std::vector<std::string>& vSequenceSet, std::string& voLCS, const bool vIfLocalSearch = false);

		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

	private:
		CHybridACOLS(void);

		int	m_UpperBound;
		int m_SequenceNum;
		int m_SequenceLen;
		int m_AntNum;
		int m_IterationTime;

		CPheromone*			           m_pPheromone;
		const hiveConfig::CHiveConfig* m_pConfig;
		
		std::vector<CAnt*>	          m_Ants;
		std::vector<std::string>      m_SequenceSet;
		std::vector<std::vector<int>> m_BestSolution;
		std::vector<std::vector<int>> m_IterBestSolution;

		void __init();
		void __pruneSequences(std::string& vioSequence) const;

		int         __calculateUpperBound() const;
		std::string __calculateSolution() const;

	friend class hiveCommon::CSingleton<CHybridACOLS>;
	};
}