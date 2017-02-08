#include <iostream>
#include "common/CommonInterface.h"
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
		for (unsigned int i=0; i<100; ++i)
		{
			std::cout << BIOCommon::generateUniformIntDistribution(1, 100) << "," << BIOCommon::generateUniformRealDistribution(0, 1) << std::endl;
		}
	}
	catch (...)
	{
		hiveCommon::hiveOutputEvent("There is a error!", hiveCommon::INFO_LEVEL);
	}

	return 0;
}