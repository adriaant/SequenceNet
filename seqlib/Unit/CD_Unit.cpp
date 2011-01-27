/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "CD_Unit.h"


// Initialization
CD_Unit::CD_Unit() 
{
	mT = 0.1;
	mDelay = 1;
	mInput = 0.0;
	mOutput = 0.0;
	mDatedInput = nil;
}


CD_Unit::~CD_Unit()
{
	delete[] mDatedInput;
}


// Set operating factors of neuron
void CD_Unit::SetParameters( data_type p_T, int p_d ) 
{
	mT = p_T;
	mDelay = p_d;
	if ( mDatedInput != nil )
		delete[] mDatedInput;
    // Create a storage for dated inputs
    mDatedInput = new data_type[mDelay+1];
	for ( int i = 0; i < mDelay+1; i++ )
		mDatedInput[i] = -1.0;
}


// Default settings
void CD_Unit::SetDefaults() 
{
	mT = 0.1;
	mDelay = 1;
	if ( mDatedInput != nil )
		delete[] mDatedInput;
    // Create a storage for dated inputs
    mDatedInput = new data_type[mDelay+1];
	for ( int i = 0; i < mDelay+1; i++ )
		mDatedInput[i] = -1.0;
}


void CD_Unit::Reset( SInt16 resetOption ) 
{
	if ( resetOption & O_ACT )
	{
		mInput = 0.0;
		mOutput = 0.0;
		for ( int i = 0; i < mDelay+1; i++ )
			mDatedInput[i] = -1.0;
	}
}


void CD_Unit::Update() 
{
	// update the cell given the current input and the dated input.
	
	// move old inputs backwards in time...
	for ( int i = 0; i < mDelay; i++ )
		mDatedInput[i] = mDatedInput[i+1];
	// store the current input
	mDatedInput[mDelay] = mInput;
	
	// process input and timed input
	if ( SafeAbs( mDatedInput[mDelay], mDatedInput[0] )  < mT )
		mOutput = 1.0;
	else
		mOutput = 0.0;
}


