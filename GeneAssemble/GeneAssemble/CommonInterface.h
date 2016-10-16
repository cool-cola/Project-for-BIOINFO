#pragma once
#include <string>
#include <vector>
#include "GeneAssembleExports.h"

namespace GeneAssemble
{
	bool GENE_ASSEMBLE_DLL_EXPORT splitGene2Reads(const std::string& vFileName, unsigned int vReadLength, std::string& vReadFileName, std::string& vPairEndFileName);
	bool GENE_ASSEMBLE_DLL_EXPORT parseReadFile(const std::string& vFileName, std::vector<std::string>& voReadSet);
	bool GENE_ASSEMBLE_DLL_EXPORT parsePairEndFile(const std::string& vFileName);

	void GENE_ASSEMBLE_DLL_EXPORT parseGeneConfig(const std::string& vConfigFile);
	void GENE_ASSEMBLE_DLL_EXPORT geneAssemble(const std::vector<std::string>& vReadSet);
}