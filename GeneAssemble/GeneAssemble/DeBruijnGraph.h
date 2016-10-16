#pragma once
#include "Node.h"
#include <map>
#include <boost/dynamic_bitset.hpp>
#include "common/WeightedDirectedGraph.h"
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace GeneAssemble
{
	const int KMER_LEN = 3;

	class CDeBruijnGraph : public hiveCommon::CWeightedDirectedGraph<CNode>
	{
	public:
		CDeBruijnGraph();
		CDeBruijnGraph(const CDeBruijnGraph& vGraph) : m_pConfig(vGraph.m_pConfig->cloneConfigV()) {}
		~CDeBruijnGraph();

		void constructDeBruijnGraph(const std::vector<std::string>& vReadSet);
		void simplifyDeBruijnGraph(); 
		void exportDeBruijnGraph(std::string vStage = "Origin");
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}		
		void generatePathSequence(const std::vector<unsigned int>& vPath, std::string& voPathSequence) const;
		void transferInEdges(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex);
		void transferOutEdges(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex);
		void deleteCoupleNodes(unsigned int vNodeIndex);
		void deleteEdge(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex);//FIXME:��Ҫ�鵽CWeightedDirectedGraph
		void dumpIncomingVertexSet(GraphVertex vNodeIndex, std::vector<GraphVertex>& voOutput) const; //FIXME:��Ҫ�鵽CWeightedDirectedGraph
		void resetNodeMarker();

		bool isPathSequenceSimilar(const std::string& vFirstPathSeq, const std::string& vSecondPathSeq, float vMinPathSimilarity) const;
		bool isConnectedByRead(unsigned int vLeftNodeIndex, unsigned int vMidNodeIndex, unsigned int vRightNodeIndex) const;  //FIXME���Ƿ����ͼ���У����д�����

		double calculateNodeAverageCoverage() const;          
		double calculateNodeCoverageStandardDeviation() const;
		double calculateEdgeAverageMultiplicity();            //FIXME:�д����죬�Ƿ�ɾ��
		double calculateEdgeMultiplicityStandardDeviation();  //FIXME:�д����죬�Ƿ�ɾ��

		const int getKmerLen() const;
		unsigned int getNumInEdges(unsigned int vNodeIndex) const;  //FIXME:��Ҫ�鵽CWeightedDirectedGraph  
		unsigned int getNumOutEdges(unsigned int vNodeIndex) const; //FIXME:��Ҫ�鵽CWeightedDirectedGraph 
		unsigned int getEdgeWeight(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex);
		const unsigned int getTwinNodeIndex(unsigned int vNodeIndex) const;

	private:
		int m_KmerLen;
		std::map<boost::dynamic_bitset<>, unsigned int> m_NodeKeyMap;
		const hiveConfig::CHiveConfig* m_pConfig;

		bool __isNodeExist(const CNode& vNode, unsigned int& voNodeIndex);
		bool __isEdgeExist(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex, CDeBruijnGraph::edge_descriptor& voEdge) const;
		bool __isNextNode(unsigned int vCurrentNodeIndex, unsigned int vNextNodeIndex) const;
		bool __isTwinNode(unsigned int vCurrentNodeIndex, unsigned int vTwinNodeIndex) const;
		bool __isReadOnNode(long vReadID, unsigned int vNodeIndex) const; //FIXME���Ƿ����ͼ���У����д�����
		
		void __mergeNode(unsigned int vKeepNodeIndex, unsigned int vDeleNodeIndex);
		void __updateEdgeMultiplicity(const CDeBruijnGraph::edge_descriptor vEdge);
		void __constructEdge(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex, CDeBruijnGraph::edge_descriptor vEdge);
	};
};