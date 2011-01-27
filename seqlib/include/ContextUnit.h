/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Class for context unit. Same as plain unit, but uses simple SetOutput function
// instead of the Update function

#ifndef __CTXTUNIT__
#define __CTXTUNIT__

#include "Unit.h"

class ContextUnit : public Unit
{

public:

	ContextUnit() { mInput = 0.0; mOutput = 0.0; }
	~ContextUnit() {}

	inline void			SetOutput( data_type value ) { mOutput = value; };
	inline data_type	GetInput( void ) { return mInput; };

};

#endif
