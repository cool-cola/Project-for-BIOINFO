#include "BioUtilityInterface.h"
#include "CommonSequenceJudger.h"
#include "LocalSearch.h"

using namespace BIOUtility;

//*******************************************************************************
//FUNCTION:
void BIOUtility::parseBioUtilityConfig(const std::string& vConfigFile)
{
	_ASSERT(!vConfigFile.empty());

	hiveConfig::CHiveConfig* pConfig = new hiveConfig::CHiveConfig;
	pConfig->defineAttribute("COMMON_SEQUENCE_JUDGER", hiveConfig::ATTRIBUTE_SUBCONFIG);
	pConfig->defineAttribute("LOCAL_SEARCH",           hiveConfig::ATTRIBUTE_SUBCONFIG);
	pConfig->defineAttribute("TEMPLATE_SEQUENCE",      hiveConfig::ATTRIBUTE_STRING);
	pConfig->defineAttribute("MAX_INTERVAL",           hiveConfig::ATTRIBUTE_INT);
	pConfig->defineAttribute("THREAD_NUM",             hiveConfig::ATTRIBUTE_INT);
	hiveConfig::hiveParseConfig(vConfigFile, hiveConfig::CONFIG_XML, pConfig);

	_ASSERT(pConfig);
	CCommonSequenceJudger::getInstance()->setConfig(pConfig->findSubConfigByName("JUDGER"));
 	CLocalSearch::getInstance()->setConfig(pConfig->findSubConfigByName("LOCALSEARCH"));
	delete pConfig;
}

//*******************************************************************************
//FUNCTION:
bool BIOUtility::isCommonSequence(const std::vector<std::string>& vSequenceSet, const std::string& vJudgedStr)
{
	_ASSERT(vSequenceSet.size() > 0 && !vJudgedStr.empty());

	return CCommonSequenceJudger::getInstance()->isCommonSequence(vSequenceSet, vJudgedStr);
}

//*******************************************************************************
//FUNCTION:
bool BIOUtility::isLongestCommonSequence(const std::vector<std::string>& vSequenceSet, const std::string& vJudgedStr)
{
	_ASSERT(vSequenceSet.size() > 0 && !vJudgedStr.empty());

	return CCommonSequenceJudger::getInstance()->isLongestCommonSequence(vSequenceSet, vJudgedStr);
}

//*******************************************************************************
//FUNCTION:
bool BIOUtility::optimizeSolutionByLocalSearch(const std::vector<std::string>& vSequenceSet, std::vector<std::vector<int>>& vioSolution)
{
	_ASSERT(vSequenceSet.size() > 0 && vioSolution.size() > 0);

	return CLocalSearch::getInstance()->searchLocally(vSequenceSet, vioSolution);
}