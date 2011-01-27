/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Class for generic neural cell/unit.

#ifndef __UNIT__
#define __UNIT__

#include "SeqGlobal.h"
#include "Utilities.h"

class Unit 
{

public:

	Unit() { mInput = 0.0; mOutput = 0.0; mRule = Sigmoid; }
	~Unit() {}

	virtual void 		Reset( SInt16 ) { mInput = 0.0; mOutput = 0.0; }
	virtual void		Update() { mOutput = (*mRule)( mInput ); };
	inline void			SetInput( data_type newInput ) { mInput = newInput; }
	inline data_type 	GetOutput( void ) { return mOutput; }

protected:

	data_type	mInput;					// Input
	data_type	mOutput;				// Output at time t
	data_type	(*mRule)( data_type );	// squashing function to use
};

#endif
