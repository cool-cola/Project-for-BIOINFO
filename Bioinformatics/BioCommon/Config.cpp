#include "Config.h"
#include <fstream>
#include "common/CommonInterface.h"

using namespace BIOCommon;

CConfig::CConfig()
{

}

CConfig::~CConfig()
{

}

//*******************************************************************************
//FUNCTION:
bool CConfig::parseFastaFile(const std::string& vFileName, std::vector<std::string>& voSeqSet)
{
	_ASSERT(!vFileName.empty());

	std::ifstream File(vFileName.c_str());
	if (!File)
	{
		hiveCommon::hiveOutputEvent("Can not open file.", hiveCommon::INFO_LEVEL);
		return false;
	}

	int SequenceNum = 0;
	std::string TempSequence, FinalSequence; 

	while (!File.eof())
	{
		getline(File, TempSequence);

		if (TempSequence[0] == '>')
			SequenceNum++;
		else
			FinalSequence.append(TempSequence);			

		if (SequenceNum > 1 && !FinalSequence.empty())
		{
			voSeqSet.push_back(FinalSequence);
			FinalSequence.clear();
		}			
	}
	return true;
}

//*******************************************************************************
//FUNCTION:
bool CConfig::parseIterationBestFile(const std::string& vFileName, std::vector<std::vector<int>>& voIB)
{
	_ASSERT(!vFileName.empty());

	std::ifstream File(vFileName.c_str());
	if (!File)
	{
		return false;
	}

	std::string TempSequence;
	std::vector<int> CurrentLine;

	while(!File.eof())
	{
		getline(File, TempSequence);

		for(int i = 0; i < TempSequence.size();)
		{
			if(TempSequence[i] == ' ')
			{
				i++;
				continue;
			}
			else
			{
				std::vector<int> CurrentNum;
				while(i < TempSequence.size() && TempSequence[i] != ' ')
				{
					CurrentNum.push_back(TempSequence[i] - 48);
					i++;
				}

				int sum = 0;
				for(int j = CurrentNum.size() - 1, k = 0; j >= 0; j--, k++)
				{
					sum = sum + CurrentNum[j] * (int)pow(10, k);
				}
				CurrentLine.push_back(sum);
			}
		}
		voIB.push_back(CurrentLine);
		CurrentLine.clear();
		TempSequence.clear();
	}
	return true;
}

//*******************************************************************************
//FUNCTION:
bool BIOCommon::CConfig::parseFile(const std::string& vFileName, std::vector<std::string>& voSeqSet)
{
	_ASSERT(vFileName.size()>0);

	std::fstream File(vFileName.c_str());
	if (!File)
	{
		hiveCommon::hiveOutputEvent("Can not open file.", hiveCommon::INFO_LEVEL);
		return false;
	}

	unsigned int Count = 0;
	std::string TempSeq;
	while (!File.eof())
	{
		getline(File, TempSeq);

		if (!TempSeq.empty() && TempSeq[0]!='>')
		{
			__pruneSequences(TempSeq);
			voSeqSet.push_back(TempSeq);
		}
	}

	return true;
}

//*******************************************************************************
//FUNCTION:
void BIOCommon::CConfig::__pruneSequences(std::string& voSequence) const
{
	_ASSERT(voSequence.size()>0);

	int SeqLen = voSequence.size();

	if (SeqLen>g_SequenceLen)
		voSequence = voSequence.substr(0, g_SequenceLen);
	else if (SeqLen<g_SequenceLen)
		voSequence.append(g_SequenceLen-SeqLen, g_PadChar);
}
