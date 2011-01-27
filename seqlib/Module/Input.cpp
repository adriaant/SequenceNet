/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "Input.h"


Input::Input( int layerSize ) 
{
	mLayerSize = layerSize;
    
    // Create a layer with generic Units/Cells
    mInputUnit = new InputUnit[mLayerSize];
}


Input::~Input() 
{
	// Clean up dynamically allocated array
	delete[] mInputUnit;
}


void Input::Initialize( int layerSize ) 
{
	mLayerSize = layerSize;
    
    // Create a layer with Input cells
    mInputUnit = new InputUnit[mLayerSize];
}


void Input::Reset( SInt16 resetOption )
{
   	int	i;
  
	for ( i = 0; i < mLayerSize; i++ )
		mInputUnit[i].Reset( resetOption );
}


// Layer is polymorphic virtual class, so correct class is automatically invoked
void Input::SetInput( Layer *fromLayer ) 
{
	for ( int i = 0; i < mLayerSize; i++ )
		mInputUnit[i].SetInput( fromLayer->GetOutput(i) );
}


void Input::SetInput( data_type *an_input ) 
{
	for ( int i = 0; i < mLayerSize; i++ )
		mInputUnit[i].SetInput( an_input[i] ); 
}


void Input::SetInput( data_type an_input ) 
{
	for ( int i = 0; i < mLayerSize; i++ )
		mInputUnit[i].SetInput( an_input ); 
}
