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
		void deleteEdge(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex);//FIXME:需要归到CWeightedDirectedGraph
		void dumpIncomingVertexSet(GraphVertex vNodeIndex, std::vector<GraphVertex>& voOutput) const; //FIXME:需要归到CWeightedDirectedGraph
		void resetNodeMarker();

		bool isPathSequenceSimilar(const std::string& vFirstPathSeq, const std::string& vSecondPathSeq, float vMinPathSimilarity) const;
		bool isConnectedByRead(unsigned int vLeftNodeIndex, unsigned int vMidNodeIndex, unsigned int vRightNodeIndex) const;  //FIXME：是否放在图类中，还有待考量

		double calculateNodeAverageCoverage() const;          
		double calculateNodeCoverageStandardDeviation() const;
		double calculateEdgeAverageMultiplicity();            //FIXME:有待考察，是否删除
		double calculateEdgeMultiplicityStandardDeviation();  //FIXME:有待考察，是否删除

		const int getKmerLen() const;
		unsigned int getNumInEdges(unsigned int vNodeIndex) const;  //FIXME:需要归到CWeightedDirectedGraph  
		unsigned int getNumOutEdges(unsigned int vNodeIndex) const; //FIXME:需要归到CWeightedDirectedGraph 
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
		bool __isReadOnNode(long vReadID, unsigned int vNodeIndex) const; //FIXME：是否放在图类中，还有待考量
		
		void __mergeNode(unsigned int vKeepNodeIndex, unsigned int vDeleNodeIndex);
		void __updateEdgeMultiplicity(const CDeBruijnGraph::edge_descriptor vEdge);
		void __constructEdge(unsigned int vSourceNodeIndex, unsigned int vTargetNodeIndex, CDeBruijnGraph::edge_descriptor vEdge);
	};
};