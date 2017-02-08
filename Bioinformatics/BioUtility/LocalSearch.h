#pragma once
#include <vector>
#include <string>
#include "common/Singleton.h"
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace BIOUtility
{
	const int MAXINTERVAL = 25;

	class CLocalSearch : public hiveCommon::CSingleton<CLocalSearch>
	{
	public:
		~CLocalSearch(void);

		void init();
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

		bool searchLocally(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution);
	
	private:
		CLocalSearch(void);
		 
		int			 m_MaxInterval;
		unsigned int m_RemovedCharacterNum;
		const hiveConfig::CHiveConfig* m_pConfig;

		void __generateBitArray(const unsigned int vBitNum, std::vector<std::string>& voBitArray) const;
		void __generateAllIntervals(const std::vector<std::string>& vSequenceSet, const std::vector<std::vector<int>>& vSolution, const unsigned int vPosition, std::vector<std::string>& voIntervals) const;
		
		bool __enumerate(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution, unsigned int vPosition) const;
		bool __updateBestSolution(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution, const std::string& vLocalBest, const unsigned int vPosition) const;
		
		std::string __selectSubSequenceOfInterval(const std::string& vBinarySwitchStr, const std::string& vSourceStr) const;
		std::pair<int, unsigned int> __calculateMinIntervalIndex(const std::vector<std::string>& vSequenceSet, const std::vector<std::vector<int>>& vSolution, const unsigned int vPosition) const;
	
	friend class hiveCommon::CSingleton<CLocalSearch>;
	};
}