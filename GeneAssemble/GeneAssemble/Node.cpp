#include "Node.h"
#include <boost/lexical_cast.hpp>

//*************************************************************************
//FUNCTION:
void GeneAssemble::CNode::generateBinarySequence(const std::string& vStr)
{
	_ASSERT(!vStr.empty());
	std::string BitStr;
	_compressedEncodingV(vStr, BitStr);
	m_BinarySequence = (m_Flag == 0) ? boost::dynamic_bitset<>(BitStr).flip() : boost::dynamic_bitset<>(BitStr);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CNode::setBinarySequence(const std::string& vStr)
{
	m_BinarySequence.clear();
	m_BinarySequence = boost::dynamic_bitset<>(vStr);
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CNode::setNodeCoverage(double vCoverage)
{
	m_Coverage = vCoverage;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CNode::addReadID(long vReadID)
{
	m_ReadIDSet.push_back(vReadID);
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CNode::updateNodeCoverage()
{
	++m_Coverage;	
}

//*************************************************************************
//FUNCTION:
const std::string GeneAssemble::CNode::getBinarySequence() const
{
	return boost::lexical_cast<std::string>(m_BinarySequence);
}

//*************************************************************************
//FUNCTION:
const unsigned short GeneAssemble::CNode::getNodeFlag() const
{
	return m_Flag;
}

//******************************************************************
//FUNCTION:
const std::string GeneAssemble::CNode::getNodeSequence() const
{
	std::string KmerSequence;
	_decompressedEncodingV(getBinarySequence(), KmerSequence);
	return KmerSequence;
}

//******************************************************************
//FUNCTION:
const double GeneAssemble::CNode::getCoverage() const
{
	return m_Coverage;
}

//*************************************************************************
//FUNCTION:
const std::vector<long> GeneAssemble::CNode::getReadIDSet() const
{
	return m_ReadIDSet;
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CNode::_compressedEncodingV(const std::string& vSourceStr, std::string& voCompressedStr) const
{
	//Transform GeneSequence to Binary Sequence
	_ASSERT(!vSourceStr.empty());

	for (unsigned int i=0; i<vSourceStr.size(); ++i)
	{
		if (vSourceStr[i] == 'A')	   voCompressedStr += "00";
		else if (vSourceStr[i] == 'C') voCompressedStr += "01";
		else if (vSourceStr[i] == 'G') voCompressedStr += "10";
		else if (vSourceStr[i] == 'T') voCompressedStr += "11";
	}
}

//*************************************************************************
//FUNCTION:
void GeneAssemble::CNode::_decompressedEncodingV(const std::string& vSourceStr, std::string& voDecompressedStr) const
{
	//Transform Binary Sequence to GeneSequence
	_ASSERT(!vSourceStr.empty());

	for (unsigned int i=0; i<vSourceStr.size(); i+=2)
	{
		std::string BinaryValue = vSourceStr.substr(i, 2);
		if (BinaryValue == "00")	  voDecompressedStr += "A";
		else if (BinaryValue == "01") voDecompressedStr += "C";
		else if (BinaryValue == "10") voDecompressedStr += "G";
		else if (BinaryValue == "11") voDecompressedStr += "T";
	}
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CNode::setNodeFlag(unsigned short vFlag)
{
	m_Flag = vFlag;
}

//******************************************************************
//FUNCTION:
void GeneAssemble::CNode::setNodeMarker(unsigned short vMarker)
{
	m_Maker = vMarker;
}

//******************************************************************
//FUNCTION:
const unsigned short GeneAssemble::CNode::getNodeMarker() const
{
	return m_Maker;
}

//******************************************************************
//FUNCTION:
const boost::dynamic_bitset<> GeneAssemble::CNode::getBitSet() const
{
	return m_BinarySequence;
}

//******************************************************************
//FUNCTION:
bool GeneAssemble::CNode::operator == (const CNode& vNode) const
{
	return m_BinarySequence == vNode.getBitSet();
}

//*************************************************************************
//FUNCTION:
const unsigned int GeneAssemble::CNode::getNodeLength() const
{
	return getNodeSequence().size();
}