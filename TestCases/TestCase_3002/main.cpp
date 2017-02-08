#include <iostream>
#include <ctime>
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
	//_crtBreakAlloc = 1291;
#endif
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		std::string FileName = "../../Dataset/TestData/pdbnt-40.fasta";
		std::vector<std::string> SeqSet;
		BIOCommon::parseFastaFile(FileName, SeqSet);

		BIOLCS::parseBioLCSConfig("ConfigLCS.xml");
		BIOUtility::parseBioUtilityConfig("ConfigUtility.xml");

 		std::string Result;
		time_t StartTime = clock();
		BIOLCS::findHybridACOLCS(SeqSet, Result);
		time_t EndTime = clock();

		std::cout << "Result : " << Result.size() << ":" << Result << std::endl;
		std::cout << "Time : " << EndTime - StartTime << std::endl;
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}