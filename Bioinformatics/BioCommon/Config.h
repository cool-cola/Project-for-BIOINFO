#pragma once
#pragma warning (disable: 4251 4819 4996 4244 4275)

#include "BioCommonExports.h"
#include <common/Singleton.h>
#include <vector>
#include <string>

namespace BIOCommon
{
	const int  g_SequenceLen = 700;
	const char g_PadChar     = 'O';

    //File
	class BIO_COMMON_DLL_EXPORT CConfig : public hiveCommon::CSingleton<CConfig>  //从hiveConfig::CHiveConfig进行派生
	{
	public:
		~CConfig();

		bool parseFastaFile(const std::string& vFileName, std::vector<std::string>& voSeqSet);
		bool parseIterationBestFile(const std::string& vFileName, std::vector<std::vector<int>>& voIB);
		bool parseFile(const std::string& vFileName, std::vector<std::string>& voSeqSet);

	private:
		CConfig(void);

		void __pruneSequences(std::string& voSequence) const;

	friend class hiveCommon::CSingleton<CConfig>;
	};
}
