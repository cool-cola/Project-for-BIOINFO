#include "CommonSequenceJudger.h"
#include "common/CommonInterface.h"

using namespace BIOUtility;

CCommonSequenceJudger::CCommonSequenceJudger() : 
	m_ThreadNum(THREADNUM), 
	m_TemplateSequence(TEMPLATESEQUENCE),
	m_pConfig(nullptr)
{	
}

CCommonSequenceJudger::~CCommonSequenceJudger()
{
	delete m_pConfig;
}

//*************************************************************************
//FUNCTION:
void CCommonSequenceJudger::init()
{
	m_ThreadNum = m_pConfig->getAttribute<int>("THREAD_NUM");
	m_TemplateSequence = m_pConfig->getAttribute<std::string>("TEMPLATE_SEQUENCE");
}

//*******************************************************************************
//FUNCTION:
bool CCommonSequenceJudger::isCommonSequence(const std::vector<std::string>& vSeqSet, const std::string& vStr)
{
	_ASSERT(vSeqSet.size() > 0 && vStr.size() > 0);

	init();

	for (unsigned int i=0; i<vSeqSet.size(); ++i)
	{
		unsigned int k = 0;
		for (unsigned int m=0; m<vSeqSet[i].length() && k<vStr.size(); ++m)
		{
			if (vSeqSet[i][m] == vStr[k])
				++k;
		}
		if (k < vStr.size())
		{
			hiveCommon::hiveOutputEvent(vStr + " is not the common sequence!", hiveCommon::INFO_LEVEL);
			return false;
		}
	}

	hiveCommon::hiveOutputEvent(vStr + " is a common sequence!", hiveCommon::INFO_LEVEL);
	return true;
}

//*******************************************************************************
//FUNCTION:
bool CCommonSequenceJudger::isLongestCommonSequence(const std::vector<std::string>& vSeqSet, const std::string& vStr)
{
	_ASSERT(vSeqSet.size() > 0 && vStr.size() > 0);

	init();

	std::string TempString = vStr;
	if (isCommonSequence(vSeqSet, TempString))
	{
		for (unsigned int i=0; i<TempString.size(); ++i)
		{
			for (auto ch : m_TemplateSequence)
			{
				TempString.insert(i, 1, ch);
				if (isCommonSequence(vSeqSet, TempString))
				{
					hiveCommon::hiveOutputEvent(vStr + " is not the longest common sequence!", hiveCommon::INFO_LEVEL);
					return false;
				}
				TempString.erase(i, 1);
			}
		}
	}
	else
	{
		hiveCommon::hiveOutputEvent(vStr + " is not the common sequence!", hiveCommon::INFO_LEVEL);
		return false;
	}

	hiveCommon::hiveOutputEvent(vStr + " is the longest common sequence!", hiveCommon::INFO_LEVEL);
	return true;
}