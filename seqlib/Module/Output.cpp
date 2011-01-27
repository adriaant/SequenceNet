/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "Output.h"


Output::Output( int layerSize ) 
{
	mLayerSize = layerSize;
    
    // Create a layer with generic cells
    mUnit = new Unit[mLayerSize];
}


Output::~Output() 
{
	// Clean up dynamically allocated arrays
	delete[] mUnit;
}


void Output::Initialize( int layerSize ) 
{
	mLayerSize = layerSize;
    
    // Create a layer with SAM cells
    mUnit = new Unit[mLayerSize];
}


void Output::Reset( SInt16 resetOption )
{
	for ( int i = 0; i < mLayerSize; i++ )
		mUnit[i].Reset( resetOption );
}


void Output::SetInput( Layer *fromLayer ) 
{
#pragma unused( fromLayer )
	cerr << "Illegal function use: Output::SetInput(Layer*)" << endl;
}


void Output::SetInput( data_type *an_input ) 
{
#pragma unused( an_input )
	cerr << "Illegal function use: Output::SetInput(Layer*)" << endl;
}


void Output::Update( Hidden* layers, InContext* ctxLayer, int numLayers )
{
	data_type	netInput;
	int			i, j, k;
	
	for ( i = 0; i < mLayerSize; i++ ) 
	{
		netInput = 0.0;

		for ( j = 0; j < ctxLayer->GetLayerSize(); j++ )
			netInput += ( ctxLayer->GetOutWeight(j,i) * ctxLayer->GetOutput(j) );

		for ( j = 0; j < numLayers; j++ )
		{
			for ( k = 0; k < mLayerSize; k++ )
				netInput += layers[j].GetOutWt(k,i) * layers[j].GetOutput(k);
		}
		
		mUnit[i].SetInput( netInput );
		mUnit[i].Update();
	}
}


void Output::Display( ostream* os )
{
	*os << "activations of output module\n";
	for ( int i = 0; i < mLayerSize; i++ )
	{
		AdjustStream( *os, 2, 0, kLeft, true );
		*os << GetOutput(i) << " ";
	}
    *os << endl;
	SetStreamDefaults( *os );
}


void Output::Data( data_type* vec, data_type scale )
{
	for ( int i = 0; i < mLayerSize; i++ )
		vec[i] = scale * GetOutput( i );
}
