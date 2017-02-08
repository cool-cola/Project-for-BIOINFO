#include "RandomNum.h"
#include <ctime>

using namespace BIOCommon;

CRandomNumber::CRandomNumber()
{
}

CRandomNumber::~CRandomNumber()
{
}

//*******************************************************************************
//FUNCTION:
int CRandomNumber::generateUniformIntDistribution(int vMin, int vMax)
{
	boost::mt19937_64 Producer(__setSeed());
	boost::uniform_int<int> Distributer(vMin, vMax);

	return Distributer(Producer);
}

//*******************************************************************************
//FUNCTION:
double CRandomNumber::generateUniformRealDistribution(double vMin, double vMax) 
{
	boost::mt19937_64 Producer(__setSeed());   
	boost::uniform_real<double> Distributer(vMin, vMax);

	return Distributer(Producer);
}

//*******************************************************************************
//FUNCTION:
time_t CRandomNumber::__setSeed()
{
	static time_t Seed = time(0);
	return ++Seed;
}