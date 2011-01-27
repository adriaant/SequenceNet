/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Coincidence Detector layer class. Derived from Layer class. 

#ifndef __CDLAYER__
#define __CDLAYER__

#include "Layer.h"
#include "CD_Unit.h"
#include <iostream>
using namespace std;

class CD_Layer : public Layer
{

public:

	CD_Layer(): Layer() {}
	CD_Layer( int layerSize );
	~CD_Layer();
	
	void				Initialize( int );
	void				Reset( SInt16 resetOption );	
	void				SetParameters( ParBlock* par );
	void				SetInput( Layer* );
	void				SetInput( data_type* );

	void 				Update( Layer* layer );
	
	inline data_type	GetOutput( int idx ) { return mCD_Unit[idx].GetOutput(); }
	inline data_type	GetInput( void ) { return mInput; }
	inline data_type	GetOutput( void ) { return mOutput; }
	inline data_type	GetLR( void ) { return mLR; }
	inline void			SetLR( data_type lr ) { mLR = lr; }

	void				Display( ostream* os );
	inline void			Data( data_type*, data_type scale ) { cerr << "Illegal function use: CD_Layer::Data()" << endl; }
	
private:

	CD_Unit		*mCD_Unit;
	data_type	mInput;			// summed input from output layer
	data_type	mOutput;		// summed output from CD units
	data_type	mAlphaLR;		// initial learning rate
	data_type	mBase;			// base rate learning
	data_type	mInitWt;		// initial CD weight
	data_type	mStepWt;		// step CD weight
	data_type	mLR;			// modified learning rate
	data_type	mDelta;			// change in learning rate
	data_type	mDeltaNew;
	int			mDeltaDir;		// direction of change
	data_type	mLambda;		// learning rate's de/increment
	data_type	mMomentum;		// momentum term
};

#endif

