/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "SAM_Unit.h"
#include "Rnd.h"


// Initialization
SAM_Unit::SAM_Unit() 
{
	a = 0.9;
	q = 0.10;
	T = 0.3;
	p = 0.1;
	c = -20.0;
/*	c1 = 0.1;
	c2 = 0.04;
	H1 = 1.5;
	H2 = 0.2;
*/
	H = 0.0;
	mInput = 0.0;
	U = 0.0;
	V = 0.0;
	mOutput = 0.0;
	mMU = 1.0;
}


// Set operating factors of neuron
void SAM_Unit::SetParameters( data_type p_a, data_type p_p, data_type a_T, data_type c_p ) 
{
	a = p_a;
	T = a_T;
	p = p_p;
	c = c_p;
}


// Default settings
void SAM_Unit::SetDefaults() 
{
	a = 0.5;
	q = 0.10;
	T = 0.45;
	p = 0.1;
	c = -20.0;
	mMU = 1.0;
}


// Reset neuron: History can be reset separately, but while I'm typing this
// comment I really wonder wether it's useful
void SAM_Unit::Reset( SInt16 resetOption ) 
{
	if ( resetOption & O_ACT )
	{
		mInput = 0.0;
		mOutput = 0.0;
		U = 0.0;
		V = 0.0;
	}
//	if ( resetOption & O_H )
//		H = 0.0;
}


// Return 1 if neuron is spiking. Is not used anywhere. What's the purpose?
// Btw, renamed function from IsSpinking to IsSpiking...
int SAM_Unit::IsSpiking() 
{
	return( ( mOutput > 0.5 ) ? 1 : 0 );
}


// Firing law: probability 1. Standard neurodynamics of the cell
// Lots to explore here. 
data_type SAM_Unit::FiringProbability( data_type pot, data_type thresh ) 
{
#if 0
	return( ( pot > thresh ) ? 1.0 : 0.0 );
#else
	return Sigmoid( c, pot - thresh );
#endif
}


void SAM_Unit::Update() 
{
	// compute accumulated potential

	// seems important to give a greater range of response.
	// that coefficient, mMu, should be investigated
	// at the moment, mMu is set to 1.0
	U = mMU * mInput + a * V;

	mOutput = FiringProbability( U, T );
	V = U - p * mOutput;

	// compute synaptic history
//	H = mOutput + q * H;
}


