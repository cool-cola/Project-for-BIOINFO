#pragma once
#pragma warning (disable: 4251 4819 4996 4244 4275)
#include <boost/random.hpp>
#include "common/Singleton.h"
#include "BioCommonExports.h"

namespace BIOCommon
{
	class CRandomNumber : public hiveCommon::CSingleton<CRandomNumber>
	{
	public:
		~CRandomNumber();

		int    generateUniformIntDistribution(int vMin, int vMax);
		double generateUniformRealDistribution(double vMin, double vMax);

	private:
		CRandomNumber(void);

		time_t __setSeed();

	friend class hiveCommon::CSingleton<CRandomNumber>;
	};
}