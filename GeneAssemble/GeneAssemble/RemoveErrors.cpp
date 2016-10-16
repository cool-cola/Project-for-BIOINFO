#include "RemoveErrors.h"
#include <algorithm>
#include "DeBruijnGraph.h"

GeneAssemble::CRemoveErrors::CRemoveErrors() :  
	m_SingleBubblePathLen(SINGLE_BUBBLE_PATH_LEN), 
	m_MaxBubbleNodeNum(MAX_BUBBLE_NODE_NUM), 
	m_MinPathSimilarity(MIN_PATH_SIMILARITY), 
	m_pConfig(nullptr),
	m_BubbleProgress(0)
{
	m_pDeBruijnGraph = new CDeBruijnGraph();
}

GeneAssemble::CRemoveErrors::~CRemoveErrors()
{
	delete m_pDeBruijnGraph;
	delete m_pConfig;

	__clearBubbleMap();
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::removeTips()
{
	_ASSERT(m_pDeBruijnGraph->getNumVertices() > 2);

	CDeBruijnGraph::vertex_iterator VertexIter = vertices(*m_pDeBruijnGraph).first;
	while (VertexIter != vertices(*m_pDeBruijnGraph).second)
	{
		if (m_pDeBruijnGraph->getNumOutEdges(*VertexIter) <= 1) 
		{
			++VertexIter;
			continue;
		}

		std::vector<CDeBruijnGraph::GraphVertex> AdjacentVertexSet;
		m_pDeBruijnGraph->dumpAdjacentVertexSet(*VertexIter, AdjacentVertexSet);
		unsigned int MaxMultiplicity = 0;
		for (auto Iter : AdjacentVertexSet)
		{
			unsigned int EdgeMultiplicity = m_pDeBruijnGraph->getEdgeWeight(*VertexIter, Iter);
			_ASSERT(EdgeMultiplicity >= 0);
			MaxMultiplicity = (EdgeMultiplicity < MaxMultiplicity) ? MaxMultiplicity : EdgeMultiplicity;
		}

		for (auto Iter : AdjacentVertexSet)
		{
			if (m_pDeBruijnGraph->getNumOutEdges(Iter) != 0) continue;

			CNode AdjacentNode = m_pDeBruijnGraph->getVertexProperty(Iter);
			unsigned int AdjacentNodeSeqLen = (AdjacentNode.getBinarySequence()).size();
			unsigned int EdgeMultiplicity = m_pDeBruijnGraph->getEdgeWeight(*VertexIter, Iter);
			if ((AdjacentNodeSeqLen < static_cast<unsigned int>(m_pDeBruijnGraph->getKmerLen()*2)) && (EdgeMultiplicity < MaxMultiplicity))
			{
				m_pDeBruijnGraph->deleteCoupleNodes(Iter);
			}
		}
		++VertexIter;
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::removeErroneousConnections()
{
	double Average = m_pDeBruijnGraph->calculateNodeAverageCoverage();
	double StandardDeviation = m_pDeBruijnGraph->calculateNodeCoverageStandardDeviation();
	
	CDeBruijnGraph::vertex_iterator VertexIter = vertices(*m_pDeBruijnGraph).first;
	for (; VertexIter!=vertices(*m_pDeBruijnGraph).second; ++VertexIter)
	{
		if ((*m_pDeBruijnGraph)[*VertexIter].getCoverage() < Average - StandardDeviation)
		{
			m_pDeBruijnGraph->deleteCoupleNodes(*VertexIter);
		}
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::removeRepeats()
{
	CDeBruijnGraph::vertex_iterator VertexIter = vertices(*m_pDeBruijnGraph).first;
	for (; VertexIter!=vertices(*m_pDeBruijnGraph).second; ++VertexIter)
	{
		if (!__isRepeat(*VertexIter))
		{
			++VertexIter;
			continue;
		}

		if (!__isRepeatSolvable(*VertexIter))
		{
			++VertexIter;
			continue;
		}

		__splitRepeatNode(*VertexIter);

		if ((m_pDeBruijnGraph->getVertexProperty(*VertexIter)).getNodeFlag() > static_cast<unsigned short>(1))
			++VertexIter;
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::setDeBruijnGraph(CDeBruijnGraph* vDeBruijnGraph)
{
	_ASSERT(vDeBruijnGraph);

	m_pDeBruijnGraph = vDeBruijnGraph;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::__clearBubbleMap()
{
	for (auto Iter : m_BubbleMap)
	{
		delete Iter.second;
	}
	m_BubbleMap.clear();
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__isAllBubbleNodeVisited() const
{
	for (auto Iter : m_BubbleMap)
	{
		if (!Iter.second->m_IsVisited)
			return false;
	}
	return true;
}

//*************************************************************************
//FUNCTION:
unsigned int GeneAssemble::CRemoveErrors::__selectUnvisitedBubbleNode() const
{
	for (auto Iter : m_BubbleMap)
	{
		if (!Iter.second->m_IsVisited)
		{
			return Iter.first;
		}
	}
	return m_pDeBruijnGraph->getNumVertices() + 1;
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__tracebackBubblePath(unsigned int vEndNodeIndex, std::vector<unsigned int>& voPath)
{
	_ASSERT(vEndNodeIndex < m_pDeBruijnGraph->getNumVertices());

	voPath.push_back(vEndNodeIndex);

	SBubbleNode* pTempNode = m_BubbleMap[vEndNodeIndex];
	if (pTempNode->m_PreNodeID == -1)
		return true;
	unsigned int PreIndex = pTempNode->m_PreNodeID;

	std::vector<unsigned int>::iterator Iter;
	while (true)
	{
		voPath.push_back(PreIndex);
		pTempNode = m_BubbleMap[PreIndex];
		if (pTempNode->m_PreNodeID == -1)
			break;
		PreIndex  = pTempNode->m_PreNodeID;

		Iter = std::find(voPath.begin(), voPath.end(), PreIndex);
		if (Iter != voPath.end()) 
			return false;
	}

	std::reverse(voPath.begin(), voPath.end());
	return true;
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__tracebackBubblePath(unsigned int vFastNodeIndex, unsigned int vSlowNodeIndex, std::vector<unsigned int>& voFastPath, std::vector<unsigned int>& voSlowPath)
{
	unsigned int TempFastNode = vFastNodeIndex;
	unsigned int TempSlowNode = vSlowNodeIndex;
	voSlowPath.push_back(vFastNodeIndex);

	while (TempFastNode != TempSlowNode)
	{
		if (m_BubbleMap[TempFastNode]->m_Cost > m_BubbleMap[TempSlowNode]->m_Cost)
		{
			voFastPath.push_back(TempFastNode);
			TempFastNode = m_BubbleMap[TempFastNode]->m_PreNodeID;
		}
		else if (m_BubbleMap[TempFastNode]->m_Cost < m_BubbleMap[TempSlowNode]->m_Cost)
		{
			voSlowPath.push_back(TempSlowNode);
			TempSlowNode = m_BubbleMap[TempSlowNode]->m_PreNodeID;
		}
		else if (__isPreviousToNode(TempFastNode, TempSlowNode)) // FIXME：调试下具体出现的情况
		{
			while (TempFastNode != TempSlowNode)
			{
				voSlowPath.push_back(TempSlowNode);
				TempSlowNode = m_BubbleMap[TempSlowNode]->m_PreNodeID;
			}
		}
		else if (__isPreviousToNode(TempSlowNode, TempFastNode)) // FIXME：调试下具体出现的情况
		{
			while (TempFastNode != TempSlowNode)
			{
				voFastPath.push_back(TempFastNode);
				TempFastNode = m_BubbleMap[TempFastNode]->m_PreNodeID;
			}
		}
		else
		{
			voFastPath.push_back(TempFastNode);
			TempFastNode = m_BubbleMap[TempFastNode]->m_PreNodeID;
			voSlowPath.push_back(TempSlowNode);
			TempSlowNode = m_BubbleMap[TempSlowNode]->m_PreNodeID;
		}
	}

	if (voFastPath.size()<=1 || voSlowPath.size()==1) // FIXME:考虑是否删除路径大小为1的边
		return false;

	voFastPath.push_back(TempFastNode);
	voSlowPath.push_back(TempFastNode);
	std::reverse(voFastPath.begin(), voFastPath.end());
	std::reverse(voSlowPath.begin(), voSlowPath.end());
	return true;
}

//******************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__isRepeat(unsigned int vNodeIndex) const
{
	_ASSERT(vNodeIndex < m_pDeBruijnGraph->getNumVertices());

	if (m_pDeBruijnGraph->getNumInEdges(vNodeIndex) != m_pDeBruijnGraph->getNumOutEdges(vNodeIndex)
		|| m_pDeBruijnGraph->getNumInEdges(vNodeIndex) < 2)
		return false;

	std::vector<CDeBruijnGraph::GraphVertex> AdjacentVertexSet;
	m_pDeBruijnGraph->dumpAdjacentVertexSet(vNodeIndex, AdjacentVertexSet);
	std::vector<CDeBruijnGraph::GraphVertex> InComingVertexSet;
	m_pDeBruijnGraph->dumpIncomingVertexSet(vNodeIndex, InComingVertexSet);
	for (unsigned int i=0; i<AdjacentVertexSet.size(); ++i)
		if (m_pDeBruijnGraph->getNumInEdges(AdjacentVertexSet[i])!=1 || m_pDeBruijnGraph->getNumOutEdges(InComingVertexSet[i])!=1)
			return false;

	int **GoThrough;
	GoThrough = new int *[InComingVertexSet.size()];
	for (unsigned int i=0; i<InComingVertexSet.size(); ++i)
		GoThrough[i] = new int [AdjacentVertexSet.size()];

	for (unsigned int i=0; i<InComingVertexSet.size(); ++i)
	{
		int Counter = 0;
		for (unsigned int k=0; k<AdjacentVertexSet.size(); ++k)
		{
			GoThrough[i][k] = (m_pDeBruijnGraph->isConnectedByRead(InComingVertexSet[i], vNodeIndex, AdjacentVertexSet[k])) ? 1 : 0;
			Counter        += GoThrough[i][k];
			if (Counter > 1) return false;
		}
		if (Counter != 1) return false;
	}

	for (unsigned int i=0; i<AdjacentVertexSet.size(); ++i)
	{
		int Counter = 0;
		for (unsigned int k=0; k<InComingVertexSet.size(); ++k)
		{
			Counter += GoThrough[k][i];
			if (Counter > 1) return false;
		}
		if (Counter != 1) return false;
	}

	for (unsigned int i=0; i<InComingVertexSet.size(); ++i)
			delete []GoThrough[i];
	delete []GoThrough;

	return true;
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__isRepeatSolvable(unsigned int vNodeIndex) const
{
	std::vector<CDeBruijnGraph::GraphVertex> AdjacentVertexSet;
	m_pDeBruijnGraph->dumpAdjacentVertexSet(vNodeIndex, AdjacentVertexSet);

	std::vector<CDeBruijnGraph::GraphVertex> InComingVertexSet;
	m_pDeBruijnGraph->dumpIncomingVertexSet(vNodeIndex, InComingVertexSet);

	std::vector<CDeBruijnGraph::GraphVertex>::iterator Iter;
	for (auto NodeIndex : AdjacentVertexSet)
	{
		Iter = std::find(InComingVertexSet.begin(), InComingVertexSet.end(), NodeIndex);
		if (Iter != InComingVertexSet.end()) return false;
	}

	return true;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::__splitRepeatNode(unsigned int vNodeIndex) //FIXME:暂时没有处理TwinNode
{
	std::vector<unsigned int> AdjacentVertexSet;
	m_pDeBruijnGraph->dumpAdjacentVertexSet(vNodeIndex, AdjacentVertexSet);

	std::vector<unsigned int> InComingVertexSet;
	m_pDeBruijnGraph->dumpIncomingVertexSet(vNodeIndex, InComingVertexSet);

	for (unsigned int i=0; i<InComingVertexSet.size(); ++i)
	{
		for (unsigned int k=0; k<AdjacentVertexSet.size(); ++k)
		{
			if (m_pDeBruijnGraph->isConnectedByRead(InComingVertexSet[i], vNodeIndex, AdjacentVertexSet[k]))
			{
				CNode RepeatNode(m_pDeBruijnGraph->getVertexProperty(vNodeIndex));
		
				unsigned int CurrNodeIndex;
				CurrNodeIndex = m_pDeBruijnGraph->addVertex(RepeatNode);
				m_pDeBruijnGraph->addEdge(InComingVertexSet[i], CurrNodeIndex, m_pDeBruijnGraph->getEdgeWeight(InComingVertexSet[i], CurrNodeIndex));
				m_pDeBruijnGraph->addEdge(CurrNodeIndex, AdjacentVertexSet[k], m_pDeBruijnGraph->getEdgeWeight(CurrNodeIndex, AdjacentVertexSet[k]));
				
				m_pDeBruijnGraph->deleteEdge(InComingVertexSet[i], vNodeIndex);
				m_pDeBruijnGraph->deleteEdge(vNodeIndex, AdjacentVertexSet[k]);

				AdjacentVertexSet.erase(AdjacentVertexSet.begin() + k);
				
				break;
			}
		}
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::__tourBus(unsigned int vNodeIndex)
{
	unsigned int CurrentNode = vNodeIndex;
	__clearBubbleMap(); // FIXME:确定下清理函数的位置
	m_BubbleMap[CurrentNode] = new SBubbleNode(-1, false, 0.0);

	while (!__isAllBubbleNodeVisited())
	{
		__tourBusNode(CurrentNode);
		CurrentNode = __selectMinCostUnvisitedBubbleNode();
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::__tourBusNode(unsigned int vNodeIndex)
{
	unsigned int OldPreviousNodeIndex = m_pDeBruijnGraph->getNumVertices();
	m_BubbleMap[vNodeIndex]->m_IsVisited = true;

	std::vector<unsigned int> AdjacentVertexSet;
	m_pDeBruijnGraph->dumpAdjacentVertexSet(vNodeIndex, AdjacentVertexSet);

	for (auto Iter : AdjacentVertexSet)
	{
		if ((*m_pDeBruijnGraph)[Iter].getNodeMarker() != 2)
			__tourBusArc(vNodeIndex, Iter);
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::__tourBusArc(unsigned int vOriginalNodeIndex, unsigned int vDestinationNodeIndex)
{
	unsigned int OldPreviousNodeIndex = m_pDeBruijnGraph->getNumVertices();
	double DestinationNodeCost = 0.0;

	if (m_BubbleMap.find(vDestinationNodeIndex) != m_BubbleMap.end())
	{
		OldPreviousNodeIndex = m_BubbleMap[vDestinationNodeIndex]->m_PreNodeID;
		DestinationNodeCost  = m_BubbleMap[vDestinationNodeIndex]->m_Cost;
	}
	
	// FIXME：调试下具体出现的情况
	if (OldPreviousNodeIndex == vOriginalNodeIndex || (*m_pDeBruijnGraph)[vDestinationNodeIndex].getNodeMarker() == 1)
		return;
	
	unsigned int EdgeMulti = m_pDeBruijnGraph->getEdgeWeight(vOriginalNodeIndex, vDestinationNodeIndex);
	double OriginNodeCost  = m_BubbleMap[vOriginalNodeIndex]->m_Cost;
	double TotalCost       = OriginNodeCost;
	if (EdgeMulti > 0)
		TotalCost += static_cast<double>((*m_pDeBruijnGraph)[vOriginalNodeIndex].getNodeSequence().size()) / EdgeMulti;

	if (OldPreviousNodeIndex == m_pDeBruijnGraph->getNumVertices())
	{
		m_BubbleMap[vDestinationNodeIndex] = new SBubbleNode(vOriginalNodeIndex, false, TotalCost);
		return;
	}
	else if (DestinationNodeCost > TotalCost)
	{
		m_BubbleMap[vDestinationNodeIndex]->m_Cost      = TotalCost;
		m_BubbleMap[vDestinationNodeIndex]->m_PreNodeID = vOriginalNodeIndex;
		__comparePath(vDestinationNodeIndex, OldPreviousNodeIndex);
	}
	else
	{
		// FIXME：调试下具体出现的情况
		if (DestinationNodeCost == OriginNodeCost && __isPreviousToNode(vDestinationNodeIndex, vOriginalNodeIndex))
			return;

		__comparePath(vDestinationNodeIndex, vOriginalNodeIndex);
	}
}

//*************************************************************************
//FUNCTION:
unsigned int GeneAssemble::CRemoveErrors::__selectMinCostUnvisitedBubbleNode() const
{
	unsigned int NodeIndex = m_pDeBruijnGraph->getNumVertices();
	double MinCost = std::numeric_limits<double>::max();

	for (auto Iter : m_BubbleMap)
	{
		if (Iter.second->m_Cost < MinCost && !Iter.second->m_IsVisited)
		{
			MinCost = Iter.second->m_Cost;
			NodeIndex = Iter.first;
		}
	}

	return NodeIndex;
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::__comparePath(unsigned int vFastNodeIndex, unsigned int vSlowNodeIndex)
{
	std::vector<unsigned int> FastPath, SlowPath;
	if (!__tracebackBubblePath(vFastNodeIndex, vSlowNodeIndex, FastPath, SlowPath))
		return;

	if (__isBubbleSolvable(FastPath, SlowPath))
	{
		m_pDeBruijnGraph->deleteEdge(SlowPath[0], SlowPath[1]);
		m_pDeBruijnGraph->deleteEdge(SlowPath[SlowPath.size()-2], SlowPath[SlowPath.size()-1]);
		m_pDeBruijnGraph->transferInEdges(SlowPath[1], FastPath[1]);
		m_pDeBruijnGraph->transferOutEdges(SlowPath[SlowPath.size()-2], FastPath[FastPath.size()-2]);

		SlowPath.erase(SlowPath.begin());
		SlowPath.pop_back();

		for (auto NodeIter : SlowPath)
		{
			(*m_pDeBruijnGraph)[NodeIter].setNodeMarker(2);
			(*m_pDeBruijnGraph)[m_pDeBruijnGraph->getTwinNodeIndex(NodeIter)].setNodeMarker(2);
		}
	}
}

//******************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__isPreviousToNode(unsigned int vPreviousNodeIndex, unsigned int vTargetNodeIndex) 
{
	unsigned int CurrentNodeIndex = vTargetNodeIndex;
	unsigned int TempPreviousNodeIndex = 0;
	double TargetCost = m_BubbleMap[vTargetNodeIndex]->m_Cost;

	while (CurrentNodeIndex)
	{
		if (CurrentNodeIndex == vPreviousNodeIndex)
			return true;

		if (CurrentNodeIndex == TempPreviousNodeIndex)
			return false;

		if (m_BubbleMap[vTargetNodeIndex]->m_Cost != TargetCost) //FIXME：调试下具体出现的情况  
			return false;

		TempPreviousNodeIndex = CurrentNodeIndex;
		if (m_BubbleMap[CurrentNodeIndex]->m_PreNodeID == -1)
			break;
		CurrentNodeIndex = m_BubbleMap[CurrentNodeIndex]->m_PreNodeID;
	}
	return false;
}

//******************************************************************
//FUNCTION:
unsigned int GeneAssemble::CRemoveErrors::__getNextStartNode()
{
	if (m_BubbleProgress == m_pDeBruijnGraph->getNumVertices())
		return m_pDeBruijnGraph->getNumVertices();

	unsigned int Index = m_BubbleProgress;
	for (; Index<m_pDeBruijnGraph->getNumVertices(); ++Index)
	{
		if ((*m_pDeBruijnGraph)[Index].getNodeMarker() == 0)
		{
			m_BubbleProgress = Index + 1;
			break;
		}
	}
	
	return Index;
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CRemoveErrors::removeBubbles()
{
	m_pDeBruijnGraph->resetNodeMarker();

	unsigned int StartNode;
	while ((StartNode=__getNextStartNode()) != m_pDeBruijnGraph->getNumVertices())
	{
		__tourBus(StartNode);

		for (auto Iter : m_BubbleMap)
		{
			if ((*m_pDeBruijnGraph)[Iter.first].getNodeMarker() != 2)
			{
				(*m_pDeBruijnGraph)[Iter.first].setNodeMarker(1);
				(*m_pDeBruijnGraph)[m_pDeBruijnGraph->getTwinNodeIndex(Iter.first)].setNodeMarker(1);
			}
		}
	}

	CDeBruijnGraph::vertex_iterator VertexIter = vertices(*m_pDeBruijnGraph).first;
	while (VertexIter!=vertices(*m_pDeBruijnGraph).second)
	{
		if ((*m_pDeBruijnGraph)[*VertexIter].getNodeMarker() == 2)
		{
			m_pDeBruijnGraph->deleteVertex(*VertexIter);
			continue;
		}
		++VertexIter;
	}

	m_pDeBruijnGraph->resetNodeMarker();
}

//******************************************************************
//FUNCTION:
bool GeneAssemble::CRemoveErrors::__isBubbleSolvable(const std::vector<unsigned int>& vFastPath, const std::vector<unsigned int>& vSlowPath)
{
	m_MaxBubbleNodeNum = m_pConfig->getAttribute<int>("MAX_BUBBLE_NODE_NUM");
	if (static_cast<int>(vFastPath.size() + vSlowPath.size() - 2) > m_MaxBubbleNodeNum)
		return false;

	std::string FastPathSequence, SlowPathSequence;
	m_pDeBruijnGraph->generatePathSequence(vFastPath, FastPathSequence);
	m_pDeBruijnGraph->generatePathSequence(vSlowPath, SlowPathSequence);

	m_SingleBubblePathLen = m_pConfig->getAttribute<int>("SINGLE_BUBBLE_PATH_LEN");
	if (FastPathSequence.size() > m_SingleBubblePathLen || SlowPathSequence.size() > m_SingleBubblePathLen)
		return false;

	m_MinPathSimilarity = m_pConfig->getAttribute<float>("MIN_PATH_SIMILARITY");
	return m_pDeBruijnGraph->isPathSequenceSimilar(FastPathSequence, SlowPathSequence, m_MinPathSimilarity);
}