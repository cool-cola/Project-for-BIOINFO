#include "BioCommonInterface.h"
#include "FileParser.h"
#include "RandomNum.h"

using namespace BIOCommon;

//*******************************************************************************
//FUNCTION:
bool BIOCommon::parseFastaFile(const std::string& vFileName, std::vector<std::string>& voSequenceSet)
{
	_ASSERT(!vFileName.empty());

	return CFileParser::getInstance()->parseFastaFile(vFileName, voSequenceSet);
}

//*******************************************************************************
//FUNCTION:
bool BIOCommon::parseSolutionFile(const std::string& vFileName, std::vector<std::vector<int>>& voSolution)
{
	_ASSERT(!vFileName.empty());

	return CFileParser::getInstance()->parseSolutionFile(vFileName, voSolution);
}

//*******************************************************************************
//FUNCTION:
int BIOCommon::generateUniformIntDistribution(int vMin, int vMax)
{
	_ASSERT(vMin >= 0 && vMin < vMax);

	return CRandomNumber::getInstance()->generateUniformRealDistribution(vMin, vMax);
}

//*******************************************************************************
//FUNCTION:
double BIOCommon::generateUniformRealDistribution(double vMin, double vMax)
{
	_ASSERT(vMin >= 0 && vMin < vMax);

	return CRandomNumber::getInstance()->generateUniformRealDistribution(vMin, vMax);
}