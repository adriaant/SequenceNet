/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Class for coincidence detector unit. Derived subclass from abstract Unit class. 

#ifndef __CDUNIT__
#define __CDUNIT__

#include "Unit.h"
#include <stdlib.h>


// This neuron functions as a coincidence detector.
// The idea is that the neuron gets two inputs, an input at time t and an
// input at time t-delay. If both t and t-delay are the same amplitude, the
// neuron will fire. This is kinda different from standard fire if both are
// on as I wanted to capture the fact that two nonactive inputs are also
// conveying information on the temporal structure of the sequence.

class CD_Unit : public Unit
{

public:

	CD_Unit();
	~CD_Unit();

	void	SetParameters( data_type p_T, int p_d );
	void 	SetDefaults();
	void 	Reset( SInt16 resetOption );
	void	Update();

	inline int		 GetDelay( void ) { return mDelay; }
	inline data_type GetHistory( int i ) { return mDatedInput[i]; }
	
protected:

	data_type*	mDatedInput;	// Array of inputs at time t-delay
	data_type	mT;				// Threshold
	int			mDelay;			// number of iterations before update
};

#endif
