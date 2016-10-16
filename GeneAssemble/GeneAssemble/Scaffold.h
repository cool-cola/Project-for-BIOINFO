#pragma once
#include <vector>
#include <map>
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"

namespace GeneAssemble
{
	const int LONG_NODE_CUTOFF = 250;
	const int UNRELIABLE_CONNECTION_CUTOFF = 5;

	class CDeBruijnGraph;
	class CPairEndInfo;

	struct SConnect 
	{
		unsigned int m_NodeID;
		unsigned int m_Count;
		bool m_IsReliable;

		SConnect() : m_NodeID(0), m_Count(0), m_IsReliable(true) {}
		SConnect(unsigned int vNodeID, unsigned int vCount = 0)
		{
			m_NodeID = vNodeID;
			m_Count  = vCount;
			m_IsReliable = true;
		}

		bool operator == (const SConnect& vConnect)
		{
			return m_NodeID == vConnect.m_NodeID;
		}
	};

	class CScaffold
	{
	public:
		CScaffold();
		~CScaffold();

		void setDeBruijnGraph(CDeBruijnGraph* vDeBruijnGraph);
		void setConfig(const hiveConfig::CHiveConfig* vConfig) {_ASSERT(vConfig); m_pConfig = vConfig->cloneConfigV();}

		void assembleConnections(long vReadNum); //FIXME:ÓÐ´ý²¹³ä
		void computeNodeToNodeMappings(std::vector<std::vector<unsigned int>> vReadNodes);
		void createConnection(unsigned int vSourceNode, unsigned int vTargetNode);
		void markUnreliableConnections();

		unsigned int findOppositeLongNode(unsigned int vSourceNode);
		std::vector<std::vector<unsigned int>> computeReadToNodeMappings(long vReadNum);

	private:
		int m_LongNodeCutoff;
		int m_UnreliableConnectionCutoff;

		CDeBruijnGraph* m_pDeBruijnGraph;
		const hiveConfig::CHiveConfig* m_pConfig;
		std::map<unsigned int, std::vector<SConnect>> m_Scaffold;
	};
}