#include "BioLCSInterface.h"
#include "common/ConfigInterface.h"
#include "HybridACOLS.h"
#include "ExactLCS.h"

using namespace BIOLCS;

//*************************************************************************
//FUNCTION:
void BIOLCS::initHybridACOLSConfig(hiveConfig::CHiveConfig* vioConfig)
{
	vioConfig->defineAttribute("SEQUENCE_LEN",   hiveConfig::ATTRIBUTE_INT);
	vioConfig->defineAttribute("ITERATION_TIME", hiveConfig::ATTRIBUTE_INT);
	vioConfig->defineAttribute("ANT_NUM",        hiveConfig::ATTRIBUTE_INT);
}

//*************************************************************************
//FUNCTION:
void BIOLCS::initAntConfig(hiveConfig::CHiveConfig* vioConfig)
{
	vioConfig->defineAttribute("ANT",                 hiveConfig::ATTRIBUTE_SUBCONFIG);
	vioConfig->defineAttribute("CANDIDATES_NUM",      hiveConfig::ATTRIBUTE_INT);
	vioConfig->defineAttribute("PHEROMONE_INFLUENCE", hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("HEURISTIC_INFLUENCE", hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("EXPLOIT_PROBABILITY", hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("EXCLUDE_PROBABILITY", hiveConfig::ATTRIBUTE_FLOAT);
}

//*************************************************************************
//FUNCTION:
void BIOLCS::initPheromoneConfig(hiveConfig::CHiveConfig* vioConfig)
{
	vioConfig->defineAttribute("PHEROMONE",        hiveConfig::ATTRIBUTE_SUBCONFIG);
	vioConfig->defineAttribute("EVAPORATION_RATE", hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("INIT_PHEROMONE",   hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("MIN_PHEROMONE",    hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("MAX_PHEROMONE",    hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("IB_PHEROMONE",     hiveConfig::ATTRIBUTE_FLOAT);
	vioConfig->defineAttribute("BS_PHEROMONE",     hiveConfig::ATTRIBUTE_FLOAT);
}

//*******************************************************************************
//FUNCTION:
void BIOLCS::parseBioLCSConfig(const std::string& vConfigFile)
{
	_ASSERT(!vConfigFile.empty());

	hiveConfig::CHiveConfig* pConfig = new hiveConfig::CHiveConfig;
	initHybridACOLSConfig(pConfig);
	initAntConfig(pConfig);
	initPheromoneConfig(pConfig);
	hiveConfig::hiveParseConfig(vConfigFile, hiveConfig::CONFIG_XML, pConfig);

	_ASSERT(pConfig);
	CHybridACOLS::getInstance()->setConfig(pConfig);
	delete pConfig;
}

//*******************************************************************************
//FUNCTION:
void BIOLCS::saveExactLCS(const std::string& vFileName)
{
	_ASSERT(vFileName.size() > 0);
	CExactLCS::getInstance()->saveLCS(vFileName);
}

//*******************************************************************************
//FUNCTION:
bool BIOLCS::findExactLCS(const std::vector<std::string>& vSequenceSet, std::string& voLCS)
{
	_ASSERT(vSequenceSet.size() > 0);
	return CExactLCS::getInstance()->findLCS(vSequenceSet, voLCS);
}

//*******************************************************************************
//FUNCTION:
bool BIOLCS::findHybridACOLCSWithLS(const std::vector<std::string>& vSequenceSet, std::string& voLCS)
{
	_ASSERT(vSequenceSet.size() > 0);
	return CHybridACOLS::getInstance()->findLCS(vSequenceSet, voLCS, true);
}

//*************************************************************************
//FUNCTION:
bool BIOLCS::findHybridACOLCS(const std::vector<std::string>& vSequenceSet, std::string& voLCS)
{
	_ASSERT(vSequenceSet.size() > 0);
	return CHybridACOLS::getInstance()->findLCS(vSequenceSet, voLCS);
}