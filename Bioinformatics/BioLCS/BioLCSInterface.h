#pragma once
#include <string>
#include <vector>
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"
#include "BioLCSExports.h"

namespace BIOLCS
{
	void initHybridACOLSConfig(hiveConfig::CHiveConfig* vioConfig);
	void initAntConfig(hiveConfig::CHiveConfig* vioConfig);
	void initPheromoneConfig(hiveConfig::CHiveConfig* vioConfig);

	BIO_LCS_DLL_EXPORT void parseBioLCSConfig(const std::string& vConfigFile);
	BIO_LCS_DLL_EXPORT void saveExactLCS(const std::string& vFileName);
	
	BIO_LCS_DLL_EXPORT bool findExactLCS(const std::vector<std::string>& vSequenceSet, std::string& voLCS);
	BIO_LCS_DLL_EXPORT bool findHybridACOLCSWithLS(const std::vector<std::string>& vSequenceSet, std::string& voLCS);
	BIO_LCS_DLL_EXPORT bool findHybridACOLCS(const std::vector<std::string>& vSequenceSet, std::string& voLCS);
}