/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "CD_Layer.h"
#include <math.h>

CD_Layer::CD_Layer( int layerSize ) 
{
	mLayerSize = layerSize;
    // Create a layer with CD cells
    mCD_Unit = new CD_Unit[mLayerSize];   
}


CD_Layer::~CD_Layer() 
{
	delete[] mCD_Unit;
}


void CD_Layer::Initialize( int layerSize ) 
{
#pragma unused ( layerSize )
	cerr << "Illegal function use: CD_Layer::Initialize()" << endl;
}


void CD_Layer::Reset( SInt16 resetOption )
{
	mInput = 0.0;
    mOutput = 0.0;
    mLR = 0.1;
    mDelta = 0.0;
    mDeltaNew = 0.0;
    mDeltaDir = 0;
	for ( int i = 0; i < mLayerSize; i++ )
		mCD_Unit[i].Reset( resetOption );
}


void CD_Layer::SetParameters( ParBlock* par ) 
{
	data_type	delayPar = par->cdDInitPar;
	
    for ( int i = 0; i < mLayerSize; i++ )
    {
    	mCD_Unit[i].SetParameters( par->cdTPar, (int)delayPar );
    	delayPar += par->cdDStepPar;
    }
    
    mAlphaLR = par->lrPar;
    mBase = par->bPar;
    mLambda = par->lPar;
    mInitWt = par->cdWtInitPar;
    mStepWt = par->cdWtStepPar;
    mMomentum = par->mPar;
}


void CD_Layer::SetInput( Layer *fromLayer ) 
{
#pragma unused( fromLayer )
	cerr << "Illegal function use: CD_Layer::SetInput(Layer*)" << endl;
}


void CD_Layer::SetInput( data_type *an_input ) 
{
#pragma unused( an_input )
	cerr << "Illegal function use: CD_Layer::SetInput(data_type*)" << endl;
}


void CD_Layer::Update( Layer* layer ) 
{
	data_type	wt;
	int			i;
	
	// incoming activation is weighted per node, so that two patterns
	// 0 0 1 and 1 0 0 are treated differently
	// cf. dendritic connectivity
	// the value of the wt seems to be better in magnitudes of 1.0 
	// in order to sharpen the differences between the nodes
	wt = mInitWt;
	mInput = 0.0;
	for ( i = 0; i < mLayerSize; i++ )
	{
		mInput += wt * layer->GetOutput( i );
		wt += mStepWt;
	}

	mOutput = 0.0;
	for ( i = 0; i < mLayerSize; i++ ) 
	{
		mCD_Unit[i].SetInput( mInput );
		mCD_Unit[i].Update();
		mOutput += mCD_Unit[i].GetOutput();
	}
	
	if ( mOutput < 1.0 )
	{
		mDeltaNew = mDelta + mLambda;
		if ( mDeltaDir == 1 )
			mDeltaNew += mMomentum * mLambda;
		mDeltaDir = 1;
	}	
	else
	{
		mDeltaNew = mDelta - mLambda;
		if ( mDeltaDir == -1 )
			mDeltaNew -= mMomentum * mLambda;
		mDeltaDir = -1;
	}

	if ( mDeltaNew < 0.0 )
	{
		mDeltaNew = 0.0;
		mLR = 0.0;
		mDelta = mDeltaNew;
	}
	else if ( mDeltaNew > mAlphaLR ) 
	{
		mDeltaNew = mAlphaLR;
		mLR = mAlphaLR;
		mDelta = mDeltaNew;
	}
	else
	{
		mDelta = mDeltaNew;
		mLR = mAlphaLR - mAlphaLR / ( 1+exp( ( 12.0 / mAlphaLR ) * mDelta - 7 ) );
	}
	if ( mLR < mBase ) mLR = mBase; // base rate learning
}


void CD_Layer::Display( ostream* os )
{
	*os << "history from coincidence detection module\n";
    for ( int i = 0; i < mLayerSize; i++ )
    {
    	for ( int j = mCD_Unit[i].GetDelay()-1; j >= 0; j-- )
    	{
    		AdjustStream( *os, 2, 3, kLeft, true );
    		*os << mCD_Unit[i].GetHistory( j ) << " ";
    	}
    	*os << endl;
    }
    *os << "\tcurrent lr: ";
    *os << mLR << endl;
	SetStreamDefaults( *os );
}


