#pragma once
#include <string>
#include <vector>
#include "BioUtilityExports.h"

namespace BIOUtility
{
	BIO_UTILITY_DLL_EXPORT void parseBioUtilityConfig(const std::string& vConfigFile);
	BIO_UTILITY_DLL_EXPORT bool isCommonSequence(const std::vector<std::string>& vSequenceSet, const std::string& vJudgedStr);
	BIO_UTILITY_DLL_EXPORT bool isLongestCommonSequence(const std::vector<std::string>& vSequenceSet, const std::string& vJudgedStr);
	BIO_UTILITY_DLL_EXPORT bool optimizeSolutionByLocalSearch(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution);
}