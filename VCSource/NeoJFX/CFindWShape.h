#pragma once

#include "Inc.h"


class CFind_W_Shape
{
public:
	CFind_W_Shape();
	~CFind_W_Shape();

	BOOL	FindShape(const TMabValues *pMabValues, int nMabCnt);

	double	ElbowR()	{ return m_elboR.value; }
	double	ElbowL()	{ return m_elboL.value; }
	double	Nose()		{ return m_nose.value; }

	int		Idx_ElbowR(){ return m_elboR.idx; }
	int		Idx_ElbowL(){ return m_elboL.idx; }
	int		Idx_Nose()	{ return m_nose.idx; }

private:
	//EN_FIND_SHAPE	FindElbowsNose(const int nStartIdx, const SIG_FACTORS_LIST* plstSigFactors);
	VOID			Reset();

private:
	
	TPoints		m_elboR;
	TPoints		m_elboL;
	TPoints		m_nose;

	
};

