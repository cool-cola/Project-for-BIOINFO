#include "Scaffold.h"
#include "DeBruijnGraph.h"
#include "PairEndInfo.h"

GeneAssemble::CScaffold::CScaffold() : 
	m_pConfig(nullptr),
	m_LongNodeCutoff(LONG_NODE_CUTOFF),
	m_UnreliableConnectionCutoff(UNRELIABLE_CONNECTION_CUTOFF)
{
	m_pDeBruijnGraph = new CDeBruijnGraph();
}

GeneAssemble::CScaffold::~CScaffold()
{
	delete m_pDeBruijnGraph;
	delete m_pConfig;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CScaffold::setDeBruijnGraph(CDeBruijnGraph* vDeBruijnGraph)
{
	_ASSERT(vDeBruijnGraph);

	m_pDeBruijnGraph = vDeBruijnGraph;
}

//*************************************************************************
//FUNCTION:
std::vector<std::vector<unsigned int>> GeneAssemble::CScaffold::computeReadToNodeMappings(long vReadNum)
{
	std::vector<std::vector<unsigned int>> ReadNodes;
	ReadNodes.resize(vReadNum);

	CDeBruijnGraph::vertex_iterator VertexIter, VertexEnd;
	for (boost::tie(VertexIter, VertexEnd) = vertices(*m_pDeBruijnGraph); VertexIter!=VertexEnd; ++VertexIter)
	{
		std::vector<long> ReadIDSet = m_pDeBruijnGraph->getVertexProperty(*VertexIter).getReadIDSet();
		for (auto Iter : ReadIDSet)
		{
			ReadNodes[Iter].push_back(*VertexIter);
		}
	}

	return ReadNodes;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CScaffold::computeNodeToNodeMappings(std::vector<std::vector<unsigned int>> vReadNodes)
{
	m_LongNodeCutoff = m_pConfig->getAttribute<int>("LONG_NODE_CUTOFF");

	CDeBruijnGraph::vertex_iterator VertexIter, VertexEnd;
	for (boost::tie(VertexIter, VertexEnd) = vertices(*m_pDeBruijnGraph); VertexIter!=VertexEnd; ++VertexIter)
	{
		if(m_pDeBruijnGraph->getVertexProperty(*VertexIter).getNodeLength() > m_LongNodeCutoff)
		{
			std::vector<long> ReadIDSet = m_pDeBruijnGraph->getVertexProperty(*VertexIter).getReadIDSet();
			for (auto ReadID : ReadIDSet)
			{
				long PairReadID = CPairEndInfo::getInstance()->findPairedReadID(ReadID);
				
				if (PairReadID == -1) continue;
				for (auto NodeID : vReadNodes[PairReadID])
				{
					createConnection(*VertexIter, NodeID);//FIXME:未考虑互为双胞胎的情况
				}			
			}
		}
	}
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CScaffold::createConnection(unsigned int vSourceNode, unsigned int vTargetNode)
{
	for (auto &Iter : m_Scaffold[vSourceNode])
	{
		if (Iter.m_NodeID == vTargetNode)
		{
			Iter.m_Count += 1;
			return;
		}
	}

	m_Scaffold[vSourceNode].push_back(SConnect(vTargetNode, 1));
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CScaffold::markUnreliableConnections()
{
	for (auto &NodeConnections : m_Scaffold)
	{
		for (auto &Connection : NodeConnections.second)
		{
			if (Connection.m_Count <= m_UnreliableConnectionCutoff && (*m_pDeBruijnGraph)[Connection.m_NodeID].getNodeLength() > m_LongNodeCutoff)
			{
				Connection.m_IsReliable = false;
			}
		}
	}
}

//*************************************************************************
//FUNCTION:
unsigned int GeneAssemble::CScaffold::findOppositeLongNode(unsigned int vSourceNode)
{
	unsigned int Result;
	int Count = 0;
	for (auto Iter : m_Scaffold[vSourceNode])
	{
		if (Iter.m_IsReliable && (*m_pDeBruijnGraph)[Iter.m_NodeID].getNodeLength() > m_LongNodeCutoff)
		{
			++Count;
			Result = Iter.m_NodeID;
		}
	}

	if (Count != 1)
		return std::numeric_limits<unsigned int>::max();

	auto Iter = std::find(m_Scaffold[Result].begin(), m_Scaffold[Result].end(), SConnect(vSourceNode));
	if (Iter == m_Scaffold[Result].end())
		return std::numeric_limits<unsigned int>::max();

	return Result;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CScaffold::assembleConnections(long vReadNum)
{
	std::vector<std::vector<unsigned int>> ReadNodes = computeReadToNodeMappings(vReadNum);
	computeNodeToNodeMappings(ReadNodes);
	markUnreliableConnections();

	for (auto Iter : m_Scaffold)
	{
		unsigned int OppositeLongNodeIndex = findOppositeLongNode(Iter.first);
		if (OppositeLongNodeIndex != std::numeric_limits<unsigned int>::max())
		{
			//拼接长节点
		}
	}
}