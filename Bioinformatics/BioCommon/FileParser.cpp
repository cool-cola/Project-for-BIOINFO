#include "FileParser.h"
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"

using namespace BIOCommon;

CFileParser::CFileParser()
{
}

CFileParser::~CFileParser()
{
}

//*******************************************************************************
//FUNCTION:
bool CFileParser::parseFastaFile(const std::string& vFileName, std::vector<std::string>& voSeqSet)
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
		else if (!FinalSequence.empty())
		{
			voSeqSet.push_back(FinalSequence);
			FinalSequence.clear();
		}			
	}
	voSeqSet.push_back(FinalSequence);
	return true;
}

//*******************************************************************************
//FUNCTION:
bool CFileParser::parseSolutionFile(const std::string& vFileName, std::vector<std::vector<int>>& voSolution)
{
	_ASSERT(!vFileName.empty());

	std::ifstream File(vFileName.c_str());
	_HIVE_EARLY_RETURN(!File, "Can not open file.", false);

	std::string TempSequence;
	std::vector<int> CurrentLine;

	while(!File.eof())
	{
		getline(File, TempSequence);

		std::vector<std::string> SplitString;
		boost::split(SplitString, TempSequence, boost::is_any_of(" "), boost::token_compress_on);

		for (auto Str : SplitString)
		{
			CurrentLine.push_back(std::atoi(Str.c_str()));
		}

		voSolution.push_back(CurrentLine);
		CurrentLine.clear();
		TempSequence.clear();
	}
	return true;
}