#include "GeneAssemble.h"
#include "DeBruijnGraph.h"
#include "RemoveErrors.h"
#include <ctime>//for test
#include <iostream>//for test

GeneAssemble::CGeneAssemble::CGeneAssemble() : m_pConfig(nullptr)
{
	m_pDeBruijnGraph = new CDeBruijnGraph();
	m_pRemoveErrors  = new CRemoveErrors();
}

GeneAssemble::CGeneAssemble::~CGeneAssemble()
{
	delete m_pDeBruijnGraph;
	delete m_pRemoveErrors;
	delete m_pConfig;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CGeneAssemble::assembleGene(const std::vector<std::string>& vReadSet)
{
	m_pDeBruijnGraph->setConfig(m_pConfig->findSubConfigByName("DeBruijnGraph"));
	m_pRemoveErrors->setConfig(m_pConfig->findSubConfigByName("RemoveError"));
	m_pRemoveErrors->setDeBruijnGraph(m_pDeBruijnGraph);
		
	time_t Start = clock();//for test
	m_pDeBruijnGraph->constructDeBruijnGraph(vReadSet);
	time_t End = clock();//for test
	std::cout << "construct Graph : " << End - Start << std::endl;//for test

	m_pDeBruijnGraph->exportDeBruijnGraph();

	Start = clock();//for test
	m_pDeBruijnGraph->simplifyDeBruijnGraph();
	End = clock();//for test
	std::cout << "simplify Graph : " << End - Start << std::endl;//for test

	m_pDeBruijnGraph->exportDeBruijnGraph("Simplify");
	
	Start = clock();//for test
 	m_pRemoveErrors->removeTips();
	End = clock();//for test
	std::cout << "remove tip : " << End - Start << std::endl;//for test

	m_pDeBruijnGraph->exportDeBruijnGraph("Remove Tips");

 	Start = clock();//for test
	m_pRemoveErrors->removeBubbles();
	End = clock();//for test
	std::cout << "remove bubble : " << End - Start << std::endl;//for test
 	
	m_pDeBruijnGraph->exportDeBruijnGraph("Remove Bubbles");

	Start = clock();//for test
	m_pRemoveErrors->removeErroneousConnections();
	End = clock();//for test
	std::cout << "remove erroneous connections : " << End - Start << std::endl;//for test

	m_pDeBruijnGraph->exportDeBruijnGraph("Remove Erroneous Connections");

	Start = clock();//for test
	m_pRemoveErrors->removeRepeats();
	End = clock();//for test
	std::cout << "remove repeats : " << End - Start << std::endl;//for test

	m_pDeBruijnGraph->exportDeBruijnGraph("Remove Repeats");
}