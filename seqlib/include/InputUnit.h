/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Class for input unit. Derived subclass from abstract Unit class. 
// Only holds input values. Nothing else.

#ifndef __INPUTUNIT__
#define __INPUTUNIT__

#include "SeqGlobal.h"
#include "Utilities.h"
#include "Unit.h"

class InputUnit : public Unit
{

public:

	// obviously mOutput and mRule are obsolete here, but initialize anyway
	InputUnit() { mInput = 0.0; mOutput = 0.0; mRule = Sigmoid; }
	~InputUnit() {}

	void 				Reset( SInt16 ) { mInput = 0.0; }
	void				Update() {};
	inline void			SetInput( data_type newInput ) { mInput = newInput; }
	inline data_type 	GetOutput( void ) { return mInput; }

};

#endif    
