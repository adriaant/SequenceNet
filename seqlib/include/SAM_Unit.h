/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Class for neural cell/unit. Derived subclass from abstract Unit class. 

#ifndef __SAM__
#define __SAM__

#include "Unit.h"

class SAM_Unit : public Unit
{

public:

	SAM_Unit();
	~SAM_Unit() {};

	void	SetParameters( data_type p_a, data_type p_p, data_type a_T, data_type c_p );
	void 	SetDefaults();
	void 	Reset( SInt16 resetOption );
	void	Update();

	data_type	FiringProbability( data_type pot, data_type thresh );
	int			IsSpiking();

	inline data_type	GetActivation() { return U; }
	inline data_type 	GetHistory() { return H; }

protected:

	data_type U, V, T;			// accumulated potential, internal potential, threshold
	data_type H, H1, H2;		// Synaptic history, thresholds (depression/potentiation)
	data_type q, a;				// Decay rates
	data_type p;				// Subtraction constant at pulse firing
	data_type c;				// Sigmoid elasticity constant
	data_type mMU;				// gate
//	data_type c1, c2;			// Learning rate
};

#endif
