/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "OutContext.h"


OutContext::OutContext( int layerSize ) 
{
	mLayerSize = layerSize;
    
    // Create a layer with generic cells
    mUnit = new Unit[mLayerSize];
}


OutContext::~OutContext() 
{
	// Clean up dynamically allocated arrays
	delete[] mUnit;
}


void OutContext::Initialize( int layerSize ) 
{
	mLayerSize = layerSize;
    mUnit = new Unit[mLayerSize];
}


void OutContext::Reset( SInt16 resetOption )
{
	for ( int i = 0; i < mLayerSize; i++ )
		mUnit[i].Reset( resetOption );
}


void OutContext::SetInput( Layer *fromLayer ) 
{
#pragma unused( fromLayer )
	cerr << "Illegal function use: OutContext::SetInput(Layer*)" << endl;
}


void OutContext::SetInput( data_type *an_input ) 
{
#pragma unused( an_input )
	cerr << "Illegal function use: OutContext::SetInput(Layer*)" << endl;
}


void OutContext::Update( Hidden* layers, InContext* ctxLayer, int numLayers )
{
	data_type	netInput;
	int			i, j, k;

	for ( i = 0; i < mLayerSize; i++ ) 
	{
		netInput = 0.0;
		
		for ( j = 0; j < mLayerSize; j++ )
			netInput += ( ctxLayer->GetCtxtWeight(j,i) * ctxLayer->GetOutput(j) );

		for ( j = 0; j < numLayers; j++ )
		{
			for ( k = 0; k < layers[j].GetLayerSize(); k++ )
				netInput += layers[j].GetCtxtWt(k,i) * layers[j].GetOutput(k);
		}

		mUnit[i].SetInput( netInput );
		mUnit[i].Update();
	}
}

void OutContext::Display( ostream* os )
{
	*os << "activations of stored context module\n";
	for ( int i = 0; i < mLayerSize; i++ )
	{
		AdjustStream( *os, 2, 0, kLeft, true );
		*os << GetOutput(i) << " ";
	}
    *os << endl;
	SetStreamDefaults( *os );
}


void OutContext::Data( data_type* vec, data_type scale )
{
	for ( int i = 0; i < mLayerSize; i++ )
		vec[i] = scale * GetOutput( i );
}

