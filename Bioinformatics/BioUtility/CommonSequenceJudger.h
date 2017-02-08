#pragma once
#include <string>
#include <vector>
#include "common/Singleton.h"
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace BIOUtility
{
	const std::string TEMPLATESEQUENCE = "ACGT";
	const int THREADNUM = 1;

	class CCommonSequenceJudger : public hiveCommon::CSingleton<CCommonSequenceJudger>  //FIXME：非常糟糕的类命名，这个简单的动词能表达你类的功能？
	{
	public:
		~CCommonSequenceJudger();

		bool isCommonSequence(const std::vector<std::string>& vSeqSet, const std::string& vStr);
		bool isLongestCommonSequence(const std::vector<std::string>& vSeqSet, const std::string& vStr);
		
		void init();
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

	private:
		CCommonSequenceJudger(void);

		int                            m_ThreadNum;
		std::string                    m_TemplateSequence;
		const hiveConfig::CHiveConfig* m_pConfig;

	friend class hiveCommon::CSingleton<CCommonSequenceJudger>;
	};
}