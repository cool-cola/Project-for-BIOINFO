#pragma once
#include <map>
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace GeneAssemble
{
	const int   SINGLE_BUBBLE_PATH_LEN = 100;
	const int   MAX_BUBBLE_NODE_NUM    = 200;
	const float MIN_PATH_SIMILARITY    = 0.8f;

	class CDeBruijnGraph;

	struct SBubbleNode
	{
		int    m_PreNodeID;
		bool   m_IsVisited;
		double m_Cost;
		
		SBubbleNode() : m_PreNodeID(-1), m_IsVisited(false), m_Cost(0.0) {}
		SBubbleNode(int vPreNodeID, bool vIsVisited, double vCost)
		{
			m_PreNodeID = vPreNodeID;
			m_IsVisited = vIsVisited;
			m_Cost      = vCost;
		}
	};

	class CRemoveErrors
	{
	public:
		CRemoveErrors();
		~CRemoveErrors();

		void removeTips();
		void removeBubbles();
		void removeErroneousConnections(); //FIXME:SOAPdenovo删除覆盖度为1的点
		void removeRepeats();

		void setDeBruijnGraph(CDeBruijnGraph* vDeBruijnGraph);
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

	private:
		void __clearBubbleMap();
		void __splitRepeatNode(unsigned int vNodeIndex); 
		void __tourBus(unsigned int vNodeIndex);
		void __tourBusNode(unsigned int vNodeIndex);
		void __tourBusArc(unsigned int vOriginalNodeIndex, unsigned int vDestinationNodeIndex);	
		void __comparePath(unsigned int vFastNodeIndex, unsigned int vSlowNodeIndex); //FIXME:命名有待改善

		bool __isRepeat(unsigned int vNodeIndex) const;
		bool __isRepeatSolvable(unsigned int vNodeIndex) const;
		bool __isBubbleSolvable(const std::vector<unsigned int>& vFastPath, const std::vector<unsigned int>& vSlowPath);
		bool __isPreviousToNode(unsigned int vPreviousNodeIndex, unsigned int vTargetNodeIndex);
		bool __isAllBubbleNodeVisited() const;
		bool __tracebackBubblePath(unsigned int vEndNodeIndex, std::vector<unsigned int>& voPath); 
		bool __tracebackBubblePath(unsigned int vFastNodeIndex, unsigned int vSlowNodeIndex, std::vector<unsigned int>& voFastPath, std::vector<unsigned int>& voSlowPath);
		
		unsigned int __getNextStartNode();
		unsigned int __selectUnvisitedBubbleNode() const;
		unsigned int __selectMinCostUnvisitedBubbleNode() const; 

		int   m_SingleBubblePathLen;
		int   m_MaxBubbleNodeNum;
		float m_MinPathSimilarity;
		unsigned int m_BubbleProgress;

		CDeBruijnGraph* m_pDeBruijnGraph;
		const hiveConfig::CHiveConfig* m_pConfig;
		std::map<unsigned int, SBubbleNode*> m_BubbleMap;
	};
};