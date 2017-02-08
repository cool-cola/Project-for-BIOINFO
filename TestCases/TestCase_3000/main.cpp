#include <iostream>
#include <memory>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "BioCommon/BioCommonInterface.h"
#include "BioUtility/BioUtilityInterface.h"
#include "BioLCS/BioLCSInterface.h"

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 1045;
#endif
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		std::vector<std::string> SeqSet;
		BIOCommon::parseFastaFile("../../DataSet/TestData/test03.fasta", SeqSet);

		BIOUtility::parseBioUtilityConfig("ConfigUtility.xml");

		std::string LCS;
		if (BIOLCS::findExactLCS(SeqSet, LCS))
		{
			std::string OutFileName = "../../DataSet/Result03.fasta";
			BIOLCS::saveExactLCS(OutFileName);

			std::vector<std::string> ResultSet;
			BIOCommon::parseFastaFile(OutFileName, ResultSet);

			for (auto Iter : ResultSet)
			{
				if (BIOUtility::isLongestCommonSequence(SeqSet, Iter)) continue;
				else
				{
					std::cout << "Test case failed." << std::endl;
					return 1;
				}
			}
			std::cout << "Test case success." << std::endl;
		}
		else 
		{
			std::cout << "Test case failed." << std::endl;
			return 1;
		}
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}