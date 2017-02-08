#pragma once
#pragma warning (disable: 4251 4819 4996 4244 4275)
#include <vector>
#include <string>
#include "common/Singleton.h"
#include "BioCommonExports.h"

namespace BIOCommon
{
	class CFileParser : public hiveCommon::CSingleton<CFileParser> 
	{
	public:
		~CFileParser();

		bool parseFastaFile(const std::string& vFileName, std::vector<std::string>& voSeqSet);
		bool parseSolutionFile(const std::string& vFileName, std::vector<std::vector<int>>& voSolution);

	private:
		CFileParser(void);

	friend class hiveCommon::CSingleton<CFileParser>;
	};
}