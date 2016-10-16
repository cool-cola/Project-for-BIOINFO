#pragma once
#include <vector>
#include "common/Singleton.h"

namespace GeneAssemble
{
	class CPairEndInfo : public hiveCommon::CSingleton<CPairEndInfo>
	{
	public:
		~CPairEndInfo();
		
		void addHeadReadID(long vHeadReadID);
		void addTailReadID(long vTailReadID);
		void addInsertLength(long vInsertLength);
		void addPairEndInfo(long vHeadReadID, long vTailReadID, long vInsertLength);
		
		long findPairedReadID(long vCurReadID);

	private:
		CPairEndInfo();

		std::vector<long> m_HeadReadIDSet;
		std::vector<long> m_TailReadIDSet;
		std::vector<long> m_InsertLengthSet;

	friend class hiveCommon::CSingleton<CPairEndInfo>;
	};
}