#include "CFindWShape.h"
#include <Windows.h>

CFind_W_Shape::CFind_W_Shape()
{
	Reset();
}


CFind_W_Shape::~CFind_W_Shape()
{

}

VOID CFind_W_Shape::Reset()
{
	m_elboR.idx		= -1;
	m_elboR.value	= 0;
	m_elboR.time	= "";

	m_elboL.idx		= -1;
	m_elboL.value	= 0;
	m_elboL.time	= "";

	m_nose.idx		= -1;
	m_nose.value	= 0;
	m_nose.time		= "";
}


// find only one W shape
// plstSigFactors has a ascending order
// [0]:oldest, [last]:latest

BOOL CFind_W_Shape::FindShape(const TMabValues* pMabValues, int nMabCnt)
{
	Reset();

	BOOL bFind = FALSE;
	EN_FIND_SHAPE	ret = FIND_NONE;

	// find right-elbo from the latest ( [lastidx] ) in reverse direction
	for (int i = nMabCnt -1; i > 1; i--)
	{
		double right	= pMabValues[i-0].mab;
		double mid		= pMabValues[i-1].mab;
		double left		= pMabValues[i-2].mab;
		
		if (left > mid && mid < right)
		{
			m_elboR.idx		= i-1;
			m_elboR.value	= mid;
			m_elboR.time = pMabValues[i - 1].CandleTime;

			ret = FIND_ELBOW_R;

			break;
		}
	}

	// If the right elbow is not founded, return FALSE;
	if (ret != FIND_ELBOW_R)
		return FALSE;


	// find left-elbo from next to the right elbow in reverse direction
	for (int k = m_elboR.idx -1; k > 1; k--)
	{
		double right	= pMabValues[k - 0].mab;
		double mid		= pMabValues[k - 1].mab;
		double left		= pMabValues[k - 2].mab;

		if (left > mid && mid < right)
		{
			m_elboL.idx		= k - 1;
			m_elboL.value	= mid;
			m_elboL.time	= pMabValues[k-1].CandleTime;

			ret = FIND_ELBOW_L;
			break;
		}
	}

	if (ret != FIND_ELBOW_L)
		return FALSE;


	// find nose between r-elbow and l-elbow
	for (int j = m_elboR.idx - 1; j > m_elboL.idx; j--)
	{
		double newNose = pMabValues[j].mab;
		if (m_nose.value < newNose)
		{
			m_nose.idx		= j;
			m_nose.value	= newNose;
			m_nose.time		= pMabValues[j].CandleTime;

			ret = FIND_NOSE_HIGH;
		}
	}

	return (ret==FIND_NOSE_HIGH);
}