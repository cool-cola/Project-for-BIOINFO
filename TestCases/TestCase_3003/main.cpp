#include <iostream>
#include <memory>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "BioCommon/BioCommonInterface.h"

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
	//_crtBreakAlloc = 23270;
#endif
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		std::string FileName = "../../Dataset/DNA/alu.n.fasta";
		std::vector<std::string> SeqSet;
		bool Res = BIOCommon::parseFastaFile(FileName, SeqSet);

		_ASSERT(SeqSet.size() > 0);
		std::cout << "First sequence : " << SeqSet.at(0) << std::endl;
		std::cout << "Last  sequence : " << SeqSet.at(SeqSet.size() - 1) << std::endl;
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}