/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Input layer class. Simple derived Layer class. Only holds input values.

#ifndef __INPUT__
#define __INPUT__

#include "Layer.h"
#include "InputUnit.h"
#include <iostream>
using namespace std;

class Input : public Layer
{

public:

	Input(): Layer() {}
	Input( int layerSize );
	~Input();
	
	void		Initialize( int );
	void		Reset( SInt16 resetOption );
	void 		SetInput( Layer * );
	void		SetInput( data_type * );
	void		SetInput( data_type );
	inline void	SetInput( data_type an_input, int idx ) { mInputUnit[idx].SetInput( an_input ); }
	
	inline void Display( ostream* ) { cerr << "Illegal function use: Input::Display()" << endl; }
	inline void	Data( data_type*, data_type scale ) { cerr << "Illegal function use: Input::Data()" << endl; }
	
	// this just returns the input value of course
	inline data_type	GetOutput( int idx ) { return mInputUnit[idx].GetOutput(); }
	inline data_type	GetWeight( int, int ) { cerr << "Illegal function use: Input::GetWeight()" << endl; return 0.0; }
	
private:

	InputUnit	*mInputUnit;
};

#endif
