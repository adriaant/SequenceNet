/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Weight class. Every layer should have a matrix of Weights, 
// if necessary. For specific weight types, derive a subclass. 

#ifndef __WEIGHT__
#define __WEIGHT__

#include "SeqGlobal.h"

class Weight
{

public:

	Weight() { mWeightValue = 0.0; mClamped = false; }
	Weight( data_type resetValue ) { mWeightValue = resetValue; mClamped = false; }
	~Weight() {}
	
	// Reset function. Either default or with supplied reset value
	inline void			Reset( void ) { mWeightValue = 0.0; }
	inline void			Reset( data_type resetValue ) { mWeightValue = resetValue; }

	// Weight adaptation function
	inline void 		AdaptWeight( data_type deltaWeight ) { mWeightValue += deltaWeight; }
	
	// Accessor functions
	inline data_type	GetWeight( void ) { return mWeightValue; }
	inline void			SetWeight( data_type newWeight ) { mWeightValue = newWeight; }
	inline bool			IsClamped( void ) { return mClamped; }
	
private:
	
	// well, duh, the value of the weight...
	data_type	mWeightValue;
	// for future use: clamping weight values for analyses
	bool		mClamped;
};

#endif
