#pragma once
#include <string>
#include <vector>
#include "common/Singleton.h"
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace GeneAssemble
{
	class CDeBruijnGraph;
	class CRemoveErrors;
	class CGeneConfig;

	class CGeneAssemble : public hiveCommon::CSingleton<CGeneAssemble>
	{
	public:
		~CGeneAssemble();

		void assembleGene(const std::vector<std::string>& vReadSet);
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

	private:
		CGeneAssemble();

		CDeBruijnGraph*          m_pDeBruijnGraph;
		CRemoveErrors*           m_pRemoveErrors;
		std::vector<std::string> m_ReadSet;
		const hiveConfig::CHiveConfig* m_pConfig;

	friend class hiveCommon::CSingleton<CGeneAssemble>;
	};
};