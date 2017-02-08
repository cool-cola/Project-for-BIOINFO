#pragma once
#include <vector>
#include <set>
#include <string>
#include "common/Singleton.h"

namespace BIOLCS
{
	class CExactLCS : public hiveCommon::CSingleton<CExactLCS>
	{
	public:
		~CExactLCS(void);

		bool findLCS(const std::vector<std::string>& vSeqSet, std::string& voLCS);
		void saveLCS(const std::string& vFileName) const;

	private:
		CExactLCS(void);

		void __fillMat(const std::string& vFirstSeq, const std::string& vSecondSeq);
		void __traceBack(const std::string& vSequence, int vRowIndex, int vColumnIndex, int vLCSIndex);
		void __outputEvent() const;

		bool __findLCSsOfTwoSeqs(const std::string& vFirstSeq, const std::string& vSecondSeq);
		bool __findLCSsOfSeqAndSeqSet(const std::string& vSequence, std::set<std::string>& vSeqSet);
		bool __findLCSsOfSeqSet(const std::vector<std::string>& vSeqSet);

		int m_LCS2Len;

		std::string m_LCS;
		std::set<std::string> m_LCSSet;
		std::vector<std::vector<int>> m_LCSMat;
		std::vector<std::vector<int>> m_DirMat;

	friend class hiveCommon::CSingleton<CExactLCS>;
	};
}