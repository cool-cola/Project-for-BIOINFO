#include "DeBruijnGraph.h"
#include <algorithm>
#include <string>
#include <sstream>
#include "common/CommonInterface.h"
 
GeneAssemble::CDeBruijnGraph::CDeBruijnGraph() : m_KmerLen(KMER_LEN), m_pConfig(nullptr)
{
}

GeneAssemble::CDeBruijnGraph::~CDeBruijnGraph()
{
	delete m_pConfig;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::constructDeBruijnGraph(const std::vector<std::string>& vReadSet)
{
	if (m_pConfig->getAttribute<int>("KMER_LEN") % 2) // k must be odd
		m_KmerLen = m_pConfig->getAttribute<int>("KMER_LEN");

	for (unsigned int ReadIndex=0; ReadIndex<vReadSet.size(); ++ReadIndex)
	{
		unsigned int PrevNodeIndex = std::numeric_limits<unsigned int>::max();
		unsigned int CurrNodeIndex = std::numeric_limits<unsigned int>::max();

		for (unsigned int Index=0; Index<=vReadSet[ReadIndex].size()-m_KmerLen; ++Index)
		{
			CNode Node(2), TwinNode(0);
			std::string Kmer = vReadSet[ReadIndex].substr(Index, m_KmerLen);
			Node.generateBinarySequence(Kmer);
			std::reverse(Kmer.begin(), Kmer.end());
			TwinNode.generateBinarySequence(Kmer);
			
			if (Node.getBinarySequence() == TwinNode.getBinarySequence()) 
				Node.setNodeFlag(1);

			// add nodes
			if (!__isNodeExist(Node, CurrNodeIndex))
			{
				CurrNodeIndex = addVertex(Node);
				m_NodeKeyMap.insert(std::pair<boost::dynamic_bitset<>, unsigned int>(Node.getBitSet(), CurrNodeIndex));
				if ((*this)[CurrNodeIndex].getNodeFlag() != static_cast<unsigned short>(1))
				{
					addVertex(TwinNode);	
					m_NodeKeyMap.insert(std::pair<boost::dynamic_bitset<>, unsigned int>(TwinNode.getBitSet(), CurrNodeIndex+1));
				}
			}
			(*this)[CurrNodeIndex].addReadID(ReadIndex);//FIXME：暂时没有给twinNode对应添加readID信息
			(*this)[CurrNodeIndex].updateNodeCoverage();
			if ((*this)[CurrNodeIndex].getNodeFlag() != static_cast<unsigned short>(1)) 
				(*this)[getTwinNodeIndex(CurrNodeIndex)].updateNodeCoverage();

			// add edges
			if (getNumVertices() > 1 && PrevNodeIndex != std::numeric_limits<unsigned int>::max())
			{
				CDeBruijnGraph::edge_descriptor Edge, TwinEdge;
				__constructEdge(PrevNodeIndex, CurrNodeIndex, Edge);
				
				if (!__isTwinNode(PrevNodeIndex, CurrNodeIndex)) 	
					__constructEdge(getTwinNodeIndex(CurrNodeIndex), getTwinNodeIndex(PrevNodeIndex), TwinEdge);
			}

			PrevNodeIndex = CurrNodeIndex;
			CurrNodeIndex = std::numeric_limits<unsigned int>::max();
		}
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::simplifyDeBruijnGraph()
{
	CDeBruijnGraph::vertex_iterator VertexIter = vertices(*this).first;
	while  (VertexIter != vertices(*this).second)
	{
		if (getNumOutEdges(*VertexIter) != 1) 
		{
			VertexIter += 2;
			continue;
		}

		std::vector<CDeBruijnGraph::GraphVertex> AdjacentNodeIndexSet;
		dumpAdjacentVertexSet(*VertexIter, AdjacentNodeIndexSet);
		_ASSERT(AdjacentNodeIndexSet.size() == 1);
		
		if (getNumInEdges(AdjacentNodeIndexSet[0]) == 1 
			&& !__isNextNode(AdjacentNodeIndexSet[0], *VertexIter) 
			&& !__isTwinNode(*VertexIter, AdjacentNodeIndexSet[0]))
		{	
			__mergeNode(*VertexIter, AdjacentNodeIndexSet[0]);
			__mergeNode(getTwinNodeIndex(AdjacentNodeIndexSet[0]), getTwinNodeIndex(*VertexIter));

			deleteVertex(AdjacentNodeIndexSet[0]);
			deleteVertex(getTwinNodeIndex(*VertexIter));
		}
		else
			VertexIter += 2;
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::exportDeBruijnGraph(std::string vStage)
{
	std::stringstream Info;
	Info << "Stage : " << vStage << std::endl
		 << "================================" << std::endl;

	Info << "Num of edges : " << getNumEdges() << std::endl;
	boost::property_map<CDeBruijnGraph, boost::edge_weight_t>::type EdgeMultiplicity = get(boost::edge_weight, *this);
	CDeBruijnGraph::edge_iterator EdgeIter, EdgeEnd;
	for (boost::tie(EdgeIter, EdgeEnd) = edges(*this); EdgeIter!=EdgeEnd; ++EdgeIter)
	{
		Info << "EdgeWeight["
			 << "(" << boost::source(*EdgeIter, *this)  << "," << boost::target(*EdgeIter, *this) << ")]" 
			 << " = " << EdgeMultiplicity[*EdgeIter] << std::endl
		     << getVertexProperty(boost::source(*EdgeIter, *this)).getNodeSequence() << " - "
			 << getVertexProperty(boost::target(*EdgeIter, *this)).getNodeSequence() << std::endl;
	}

	Info << std::endl 
		 << "Num of nodes : " << getNumVertices() << std::endl;
	CDeBruijnGraph::vertex_iterator VertexIter, VertexEnd;
	for (boost::tie(VertexIter, VertexEnd) = vertices(*this); VertexIter!=VertexEnd; ++VertexIter)
	{
		Info << "NodeCoverage["
			 << *VertexIter << std::setiosflags(std::ios::left) << std::setw(3) << "]"
			 << " = " << getVertexProperty(*VertexIter).getCoverage() << std::endl
			 << getVertexProperty(*VertexIter).getNodeSequence() << std::endl;
	}	

	hiveCommon::hiveOutputEvent(Info.str(), hiveCommon::INFO_LEVEL);
}

//*************************************************************************
//FUNCTION:
unsigned int GeneAssemble::CDeBruijnGraph::getNumInEdges(unsigned int vNodeIndex) const
{
	_ASSERT(vNodeIndex != CDeBruijnGraph::null_vertex());
	return in_degree(vNodeIndex, *this);
}

//*************************************************************************
//FUNCTION:
unsigned int GeneAssemble::CDeBruijnGraph::getNumOutEdges(unsigned int vNodeIndex) const
{
	return getNumAdjacentVertex(vNodeIndex);
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::deleteCoupleNodes(unsigned int vNodeIndex)
{
	if ((*this)[vNodeIndex].getNodeFlag() == static_cast<unsigned short>(0))
	{
		deleteVertex(vNodeIndex);
		deleteVertex(getTwinNodeIndex(vNodeIndex));
	}
	else if ((*this)[vNodeIndex].getNodeFlag() == static_cast<unsigned short>(2))
	{
		deleteVertex(getTwinNodeIndex(vNodeIndex));
		deleteVertex(vNodeIndex);
	}
	else
	{
		deleteVertex(vNodeIndex);
	}
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::__isNodeExist(const CNode& vNode, unsigned int& voNodeIndex) //FIXME:每次都要遍历整个图去判断一个节点是否存在(考虑优化)
{
	std::map<boost::dynamic_bitset<>, unsigned int>::const_iterator Iter = m_NodeKeyMap.find(vNode.getBitSet());
	
	if (Iter != m_NodeKeyMap.end())
		voNodeIndex = Iter->second;

	return Iter != m_NodeKeyMap.end();
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::__isEdgeExist(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex, CDeBruijnGraph::edge_descriptor& voEdge) const
{
	bool Found = false;
	boost::tie(voEdge, Found) = boost::edge(vSourceNodeIndex, vTargetNodeIndex, *this);

	return Found;
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::__isNextNode(unsigned int vCurrentNodeIndex, unsigned int vNextNodeIndex) const
{
	_ASSERT(vCurrentNodeIndex != CDeBruijnGraph::null_vertex() && vNextNodeIndex != CDeBruijnGraph::null_vertex());
	std::vector<CDeBruijnGraph::GraphVertex> AdjacentVertexSet;
	dumpAdjacentVertexSet(vCurrentNodeIndex, AdjacentVertexSet);
	auto Iter = std::find(AdjacentVertexSet.begin(), AdjacentVertexSet.end(), vNextNodeIndex);
	return !(Iter == AdjacentVertexSet.end());
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::__isTwinNode(unsigned int vCurrentNodeIndex, unsigned int vTwinNodeIndex) const
{
	_ASSERT(vCurrentNodeIndex != CDeBruijnGraph::null_vertex() && vTwinNodeIndex != CDeBruijnGraph::null_vertex());
	
	return vCurrentNodeIndex == getTwinNodeIndex(vTwinNodeIndex);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::__mergeNode(unsigned int vKeepNodeIndex, unsigned int vDeleNodeIndex)
{
	CNode& KeepNode = (*this)[vKeepNodeIndex];
	CNode& DeleNode = (*this)[vDeleNodeIndex];

	std::string KeepNodeBinarySeq = KeepNode.getBinarySequence();
	std::string DeleNodeBinarySeq = DeleNode.getBinarySequence();
	std::string MergedBinarySeq = KeepNodeBinarySeq + DeleNodeBinarySeq.substr((m_KmerLen-1)*2, *(DeleNodeBinarySeq.end()-1));
	KeepNode.setBinarySequence(MergedBinarySeq);

	if (getNumOutEdges(vDeleNodeIndex))
		transferOutEdges(vDeleNodeIndex, vKeepNodeIndex);
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::isPathSequenceSimilar(const std::string& vFirstPathSeq, const std::string& vSecondPathSeq, float vMinPathSimilarity) const
{
	if (vFirstPathSeq.empty() || vSecondPathSeq.empty()) return false;

	unsigned int Row = vFirstPathSeq.size();
	unsigned int Column = vSecondPathSeq.size();

	std::vector<std::vector<float>> ScoreMatrix;
	ScoreMatrix.resize(Row + 1);
	for (auto &Iter : ScoreMatrix)
		Iter.resize(Column + 1);

	for (auto &Iter : ScoreMatrix[0])
		Iter = 0.0;
	for (unsigned int i=0; i<=Row; ++i)
		ScoreMatrix[i][0] = 0.0;

	for (unsigned int i=1; i<=Row; ++i)
	{
		for (unsigned int k=1; k<=Column; ++k)
		{
			float DiagonalScore = ScoreMatrix[i-1][k-1] + ((vFirstPathSeq[i-1] == vSecondPathSeq[k-1]) ? 1.0 : 0.0);
			float LeftScore     = ScoreMatrix[i][k-1];
			float UpScore       = ScoreMatrix[i-1][k];

			ScoreMatrix[i][k] = std::max<float>(std::max<float>(DiagonalScore, LeftScore), UpScore);
		}
	}
	return (ScoreMatrix[Row][Column] / static_cast<float>((Row > Column) ? Row : Column) > vMinPathSimilarity);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::__updateEdgeMultiplicity(const CDeBruijnGraph::edge_descriptor vEdge)
{
	boost::property_map<CDeBruijnGraph, boost::edge_weight_t>::type EdgeMultiplicity = get(boost::edge_weight, *this);
	++EdgeMultiplicity[vEdge];
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::generatePathSequence(const std::vector<unsigned int>& vPath, std::string& voPathSequence) const
{
	_ASSERT(vPath.size() > 0);
	
	CNode TempNode = (*this)[vPath[0]];
	voPathSequence = TempNode.getNodeSequence();
	for (unsigned int i=1; i<vPath.size(); ++i)
	{
		TempNode = (*this)[vPath[i]];
		voPathSequence += TempNode.getNodeSequence().back();
	}
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::__constructEdge(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex, CDeBruijnGraph::edge_descriptor vEdge)
{
	if (!__isEdgeExist(vSourceNodeIndex, vTargetNodeIndex, vEdge)) 
		addEdge(vSourceNodeIndex, vTargetNodeIndex, 1.0);
	else
		__updateEdgeMultiplicity(vEdge);
}

//******************************************************************
//FUNCTION:
double GeneAssemble::CDeBruijnGraph::calculateNodeAverageCoverage() const
{
	double TotalNodeCoverage = 0.0;
	CDeBruijnGraph::vertex_iterator VertexIter, VertexEnd;
	for (boost::tie(VertexIter, VertexEnd) = vertices(*this); VertexIter!=VertexEnd; ++VertexIter)
	{
		TotalNodeCoverage += (*this)[*VertexIter].getCoverage();
	}

	return TotalNodeCoverage / getNumVertices();
}

//******************************************************************
//FUNCTION:
double GeneAssemble::CDeBruijnGraph::calculateNodeCoverageStandardDeviation() const
{
	double Variance = 0.0;
	double Average = calculateNodeAverageCoverage();
	CDeBruijnGraph::vertex_iterator VertexIter, VertexEnd;
	for (boost::tie(VertexIter, VertexEnd) = vertices(*this); VertexIter!=VertexEnd; ++VertexIter)
	{
		Variance += pow(((*this)[*VertexIter].getCoverage() - Average), 2);
	}

	return sqrt(Variance / getNumVertices());
}

//******************************************************************
//FUNCTION:
double GeneAssemble::CDeBruijnGraph::calculateEdgeAverageMultiplicity()
{
	double TotalEdgeMultiplicity = 0.0;
	boost::property_map<CDeBruijnGraph, boost::edge_weight_t>::type EdgeMultiplicity = get(boost::edge_weight, *this);

	CDeBruijnGraph::edge_iterator EdgeIter, EdgeEnd;
	for (boost::tie(EdgeIter, EdgeEnd) = edges(*this); EdgeIter!=EdgeEnd; ++EdgeIter)
	{
		TotalEdgeMultiplicity += EdgeMultiplicity[*EdgeIter];
	}
	
	return TotalEdgeMultiplicity / getNumEdges();
}

//******************************************************************
//FUNCTION:
double GeneAssemble::CDeBruijnGraph::calculateEdgeMultiplicityStandardDeviation() 
{
	double Variance = 0.0;
	double Average = calculateEdgeAverageMultiplicity();

	boost::property_map<CDeBruijnGraph, boost::edge_weight_t>::type EdgeMultiplicity = get(boost::edge_weight, *this);

	CDeBruijnGraph::edge_iterator EdgeIter, EdgeEnd;
	for (boost::tie(EdgeIter, EdgeEnd) = edges(*this); EdgeIter!=EdgeEnd; ++EdgeIter)
	{
		Variance += pow((EdgeMultiplicity[*EdgeIter] - Average), 2);
	}

	return sqrt(Variance / getNumEdges());
}

//*************************************************************************
//FUNCTION:
const unsigned int GeneAssemble::CDeBruijnGraph::getTwinNodeIndex(unsigned int vNodeIndex) const
{
	_ASSERT(vNodeIndex >= 0 && vNodeIndex < getNumVertices());
	return vNodeIndex + getVertexProperty(vNodeIndex).getNodeFlag() - 1;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::dumpIncomingVertexSet(GraphVertex vNodeIndex, std::vector<GraphVertex>& voOutput) const
{
	_ASSERT((vNodeIndex != CDeBruijnGraph::null_vertex()) && voOutput.empty());
	CDeBruijnGraph::in_edge_iterator InEdge, LastInEdge;
	for (boost::tie(InEdge, LastInEdge)=boost::in_edges(vNodeIndex, *this); InEdge!=LastInEdge; InEdge++)
	{
		voOutput.push_back(boost::source(*InEdge, *this));
	}
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::transferOutEdges(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex)
{
	std::vector<CDeBruijnGraph::GraphVertex> AdjacentNodeIndexSet;
	dumpAdjacentVertexSet(vSourceNodeIndex, AdjacentNodeIndexSet);
	for (auto NodeIndex : AdjacentNodeIndexSet)
		addEdge(vTargetNodeIndex, NodeIndex, getEdgeWeight(vSourceNodeIndex, NodeIndex));
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::transferInEdges(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex)
{
	std::vector<CDeBruijnGraph::GraphVertex> IncomingNodeIndexSet;
	dumpIncomingVertexSet(vSourceNodeIndex, IncomingNodeIndexSet);
	for (auto NodeIndex : IncomingNodeIndexSet)
		addEdge(NodeIndex, vTargetNodeIndex, getEdgeWeight(NodeIndex, vSourceNodeIndex));
}

//*************************************************************************
//FUNCTION:
const int GeneAssemble::CDeBruijnGraph::getKmerLen() const
{
	return m_KmerLen;
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::isConnectedByRead(unsigned int vLeftNodeIndex, unsigned int vMidNodeIndex, unsigned int vRightNodeIndex) const
{
	for (auto Iter : getVertexProperty(vLeftNodeIndex).getReadIDSet())
	{
		return (__isReadOnNode(Iter, vMidNodeIndex) && __isReadOnNode(Iter, vRightNodeIndex));
	}
	return false;
}

//*************************************************************************
//FUNCTION:
bool GeneAssemble::CDeBruijnGraph::__isReadOnNode(long vReadID, unsigned int vNodeIndex) const
{
	std::vector<long> ReadIDSet = getVertexProperty(vNodeIndex).getReadIDSet();
	auto Iter = std::find(ReadIDSet.begin(), ReadIDSet.end(), vReadID);
	return (Iter != ReadIDSet.end());
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::deleteEdge(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex)
{
	boost::remove_edge(vSourceNodeIndex, vTargetNodeIndex, *this);
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CDeBruijnGraph::resetNodeMarker()
{
	CDeBruijnGraph::vertex_iterator VertexIter, VertexEnd;
	for (boost::tie(VertexIter, VertexEnd) = vertices(*this); VertexIter!=VertexEnd; ++VertexIter)
	{
		if (getTwinNodeIndex(*VertexIter) == *VertexIter)
		{
			(*this)[*VertexIter].setNodeMarker(1);
			continue;
		}

		std::vector<unsigned int> AdjacentVertexSet;
		dumpAdjacentVertexSet(*VertexIter, AdjacentVertexSet);
		if (std::find(AdjacentVertexSet.begin(), AdjacentVertexSet.end(), getTwinNodeIndex(*VertexIter)) != AdjacentVertexSet.end())
		{
			(*this)[*VertexIter].setNodeMarker(1);
			(*this)[getTwinNodeIndex(*VertexIter)].setNodeMarker(1);
			++VertexIter;
			continue;
		}

		AdjacentVertexSet.clear();
		dumpAdjacentVertexSet(getTwinNodeIndex(*VertexIter), AdjacentVertexSet);
		if (std::find(AdjacentVertexSet.begin(), AdjacentVertexSet.end(), *VertexIter) != AdjacentVertexSet.end())
		{
			(*this)[*VertexIter].setNodeMarker(1);
			(*this)[getTwinNodeIndex(*VertexIter)].setNodeMarker(1);
			++VertexIter;
			continue;
		}

		(*this)[*VertexIter].setNodeMarker(0);
		(*this)[getTwinNodeIndex(*VertexIter)].setNodeMarker(0);
		++VertexIter;
	}	
}

//*************************************************************************
//FUNCTION:
unsigned int GeneAssemble::CDeBruijnGraph::getEdgeWeight(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex)
{
	bool Found = false;
	CDeBruijnGraph::edge_descriptor Edge;
	boost::tie(Edge, Found) = boost::edge(vSourceNodeIndex, vTargetNodeIndex, *this);

	if (Found)
	{
		boost::property_map<CDeBruijnGraph, boost::edge_weight_t>::type EdgeMultiplicity = get(boost::edge_weight, *this);
		return static_cast<unsigned int>(EdgeMultiplicity[Edge]);
	}
	else
		return 0;
}