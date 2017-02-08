#pragma once
#pragma warning (disable: 4251 4819 4996 4244 4275)
#include <string>
#include <vector>
#include "BioCommonExports.h"

namespace BIOCommon
{
	BIO_COMMON_DLL_EXPORT bool parseFastaFile(const std::string& vFileName, std::vector<std::string>& voSequenceSet);
	BIO_COMMON_DLL_EXPORT bool parseSolutionFile(const std::string& vFileName, std::vector<std::vector<int>>& voSolution);
	
	BIO_COMMON_DLL_EXPORT int    generateUniformIntDistribution(int vMin, int vMax);
	BIO_COMMON_DLL_EXPORT double generateUniformRealDistribution(double vMin, double vMax);
}