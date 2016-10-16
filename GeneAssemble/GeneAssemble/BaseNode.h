#pragma once
#include <string>

namespace GeneAssemble
{
	class CBaseNode
	{
	public:
		CBaseNode() {}
		virtual ~CBaseNode() {}

	protected:
		virtual void _compressedEncodingV(const std::string& vSourceStr, std::string& voCompressedStr) const = 0;
		virtual void _decompressedEncodingV(const std::string& vSourceStr, std::string& voDecompressedStr) const = 0;
	};
}