#pragma once
#include "BaseNode.h"
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

namespace GeneAssemble
{
	class CNode : public CBaseNode
	{
	public:
		CNode() : m_Coverage(0), m_Flag(2), m_Maker(0) {}
		CNode(unsigned short vFlag) : m_Coverage(0), m_Maker(0) {m_Flag = vFlag;}
		CNode(const CNode& vNode) {*this = vNode;}
		~CNode() {}

		void generateBinarySequence(const std::string& vStr);
		void setBinarySequence(const std::string& vStr);
		void setNodeCoverage(double vCoverage);
		void setNodeFlag(unsigned short vFlag);
		void setNodeMarker(unsigned short vMarker);
		void addReadID(long vReadID);
		void updateNodeCoverage();

		const double         getCoverage() const;
		const unsigned short getNodeFlag() const;
		const unsigned short getNodeMarker() const;
		const unsigned int   getNodeLength() const;
		const std::string    getNodeSequence() const;
		const std::string    getBinarySequence() const;	
		const boost::dynamic_bitset<> getBitSet() const;
		const std::vector<long> getReadIDSet() const;

		bool operator == (const CNode& vNode) const;

	protected:
		virtual void _compressedEncodingV(const std::string& vSourceStr, std::string& voCompressedStr) const override;
		virtual void _decompressedEncodingV(const std::string& vSourceStr, std::string& voDecompressedStr) const override;

	private:
		double m_Coverage;
		unsigned short m_Flag;  // 2:smaller 0:larger 1:rev-com equal to itself
		unsigned short m_Maker; //0:normal node 1:回文节点 2:要删除的节点
		std::vector<long> m_ReadIDSet;
		boost::dynamic_bitset<> m_BinarySequence;
	};
}