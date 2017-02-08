#include "CommonInterface.h"
#include "GeneAssemble.h"
#include "PairEndInfo.h"
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"

using namespace GeneAssemble;

//*************************************************************************
//FUNCTION:
bool GENE_ASSEMBLE_DLL_EXPORT GeneAssemble::parseReadFile(const std::string& vFileName, std::vector<std::string>& voReadSet)
{
	_ASSERT(!vFileName.empty());

	std::ifstream File(vFileName.c_str());
	_HIVE_EARLY_RETURN(!File, "Can not open file.", false);

	std::string TempSequence, FinalSequence; 

	while (!File.eof())
	{
		getline(File, TempSequence);

		if (TempSequence[0] != '>')
			FinalSequence.append(TempSequence);			

		if (!FinalSequence.empty())
		{
			voReadSet.push_back(FinalSequence);
			FinalSequence.clear();
		}			
	}
	File.close();
	return true;
}

//*************************************************************************
//FUNCTION:
bool GENE_ASSEMBLE_DLL_EXPORT GeneAssemble::parsePairEndFile(const std::string& vFileName)
{
	_ASSERT(!vFileName.empty());

	std::ifstream File(vFileName.c_str());
	_HIVE_EARLY_RETURN(!File, "Can not open file.", false);

	std::string TempSequence;
	std::vector<std::string> SplitString;

	while(!File.eof())
	{
		getline(File, TempSequence);		
		boost::split(SplitString, TempSequence, boost::is_any_of(" "), boost::token_compress_on);
		CPairEndInfo::getInstance()->addPairEndInfo(std::atol(SplitString[0].c_str()), std::atol(SplitString[1].c_str()), std::atol(SplitString[2].c_str()));

		SplitString.clear();
		TempSequence.clear();
	}
	return true;
}

//*************************************************************************
//FUNCTION:
void GENE_ASSEMBLE_DLL_EXPORT GeneAssemble::geneAssemble(const std::vector<std::string>& vReadSet)
{
	CGeneAssemble::getInstance()->assembleGene(vReadSet);
}

//*************************************************************************
//FUNCTION:
void GENE_ASSEMBLE_DLL_EXPORT GeneAssemble::parseGeneConfig(const std::string& vConfigFile)
{
	_ASSERT(!vConfigFile.empty());

	hiveConfig::CHiveConfig* pConfig = new hiveConfig::CHiveConfig;
	pConfig->defineAttribute("DEBRUIJN_GRAPH",               hiveConfig::ATTRIBUTE_SUBCONFIG);
	pConfig->defineAttribute("REMOVE_ERROR",                 hiveConfig::ATTRIBUTE_SUBCONFIG);
	pConfig->defineAttribute("SCAFFOLD",                     hiveConfig::ATTRIBUTE_SUBCONFIG);
	pConfig->defineAttribute("KMER_LEN",                     hiveConfig::ATTRIBUTE_INT);
	pConfig->defineAttribute("TIP_LEN",                      hiveConfig::ATTRIBUTE_INT);
	pConfig->defineAttribute("SINGLE_BUBBLE_PATH_LEN",       hiveConfig::ATTRIBUTE_INT);
	pConfig->defineAttribute("MAX_BUBBLE_NODE_NUM",          hiveConfig::ATTRIBUTE_INT);
	pConfig->defineAttribute("MIN_PATH_SIMILARITY",          hiveConfig::ATTRIBUTE_FLOAT);
	pConfig->defineAttribute("LONG_NODE_CUTOFF",             hiveConfig::ATTRIBUTE_INT);
	pConfig->defineAttribute("UNRELIABLE_CONNECTION_CUTOFF", hiveConfig::ATTRIBUTE_INT);
	hiveConfig::hiveParseConfig(vConfigFile, hiveConfig::CONFIG_XML, pConfig);

	_ASSERT(pConfig);
	CGeneAssemble::getInstance()->setConfig(pConfig);
	delete pConfig;
}

//*************************************************************************
//FUNCTION:
bool GENE_ASSEMBLE_DLL_EXPORT GeneAssemble::splitGene2Reads(const std::string& vFileName, unsigned int vReadLength, std::string& vReadFileName, std::string& vPairEndFileName)
{
	_ASSERT(!vFileName.empty() && !vReadFileName.empty() && !vPairEndFileName.empty());
	
	std::ofstream OutPairEndFile(vPairEndFileName.c_str());
	std::ofstream OutReadFile(vReadFileName.c_str());
	std::ifstream File(vFileName.c_str());
	_HIVE_EARLY_RETURN(!File, "Can not open file.", false);	

	long ReadID = 0;
	std::string TempSequence, FinalSequence; 
	while (!File.eof())
	{
		getline(File, TempSequence);

		if (TempSequence[0] != '>')
			FinalSequence.append(TempSequence);			
		else if (!FinalSequence.empty())
		{
			OutPairEndFile << ReadID;

			long Count = 0;
			unsigned int Head = 0;
			while (1)
			{
				OutReadFile << ">Read_" << ReadID + Count << "_length_" << vReadLength << std::endl;

				if (FinalSequence.size() - Head < vReadLength)
				{
					OutReadFile << FinalSequence.substr(Head) << std::endl;
					break;
				}
				else
				{
					OutReadFile << FinalSequence.substr(Head, vReadLength) << std::endl;
					Head += (++Count) * vReadLength;
				}
			}
			OutPairEndFile << " " << ReadID + Count << " " << (Count - 1) * vReadLength << std::endl;
			ReadID += (Count + 1);
		}			
	}
	OutPairEndFile.close();
	OutReadFile.close();
	File.close();
	return true;
}