/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "InContext.h"

InContext::InContext( int ctxtSize, int layerSize ) 
{
	int	i, j;
	
	mLayerSize = layerSize;
    mContextSize = ctxtSize;
    
    // Create a layer with generic cells
    mUnit = new ContextUnit[mContextSize];
    
    // weights from incontext to output layer
    mOutWts = new data_type*[mContextSize];
    for ( i = 0; i < mContextSize; i++ )
    {
    	mOutWts[i] = new data_type[mLayerSize];    	
    	for ( j = 0; j < mLayerSize; j++ ) mOutWts[i][j] = 0.0;
    }

    // weights from in-context to out-context
    mCtxtWts = new data_type*[mContextSize];
    for ( i = 0; i < mContextSize; i++ )
    {
    	mCtxtWts[i] = new data_type[mContextSize];    	
    	for ( j = 0; j < mContextSize; j++ ) mCtxtWts[i][j] = 0.0;
    }
}


InContext::~InContext() 
{
	// Clean up dynamically allocated arrays
	delete[] mUnit;
	for ( int i = 0; i < mContextSize; i++ ) delete[] mOutWts[i];
	delete[] mOutWts;
	for ( int i = 0; i < mContextSize; i++ ) delete[] mCtxtWts[i];
	delete[] mCtxtWts;
}


void InContext::Initialize( int ctxtSize ) 
{
#pragma unused( layerSize )
	cerr << "Illegal function use: InContext::Initialize()" << endl;
}


int InContext::LoadContext( char *filename )
{
	ifstream	infile;
	char		tmpname[255];
	data_type	dummy1, dummy2, w;
	int			i, j;
	
	strcpy( tmpname, filename );
	strcat( tmpname, ".wts" );
	
	infile.open( tmpname );
	if ( infile.fail() )
	{
		FileOpenError( tmpname );
		return 0;
	}
	for ( i = 0; i < mContextSize; i++ )
	{
		for ( j = 0; j < mLayerSize; j++ )
		{
			infile >> dummy1 >> dummy2 >> w;
			mOutWts[i][j] = w;
		}
	}
	for ( i = 0; i < mContextSize; i++ )
	{
		for ( j = 0; j < mContextSize; j++ )
		{
			infile >> dummy1 >> dummy2 >> w;
			mCtxtWts[i][j] = w;
		}
	}
	infile.close();
	
	return 1;
}


void InContext::SaveContext( char* filename )
{
    ofstream	outfile;
    char		tmpname[255];
    int			i, j;
    
    strcpy( tmpname, filename );
    strcat( (char*)tmpname, ".wts" );
    
	outfile.open( tmpname );

	for ( i = 0; i < mContextSize; i++ )
	{
		for ( j = 0; j < mLayerSize; j++ ) 
			outfile << i << " " << j << " " << mOutWts[i][j] << "\n";
		outfile << "\n";
	}
	for ( i = 0; i < mContextSize; i++ )
	{
		for ( j = 0; j < mContextSize; j++ ) 
			outfile << i << " " << j << " " << mCtxtWts[i][j] << "\n";
		outfile << "\n";
	}

	outfile.close();
}


void InContext::Reset( SInt16 resetOption )
{
	if ( resetOption & O_ACT )
	{
		for ( int i = 0; i < mContextSize; i++ )
			mUnit[i].Reset( resetOption );
	}
	if ( resetOption & O_WT )
	{
	    // reinit weights from context to output
	    for ( int i = 0; i < mContextSize; i++ )
		{
	    	for ( int j = 0; j < mLayerSize; j++ ) mOutWts[i][j] = 0.0;
			for ( int j = 0; j < mContextSize; j++ ) mCtxtWts[i][j] = 0.0;
		}
	}
}


void InContext::SetInput( Layer *fromLayer ) 
{
#pragma unused( fromLayer )
	cerr << "Illegal function use: InContext::SetInput(Layer*)" << endl;
}


void InContext::SetInput( data_type *an_input ) 
{
	for ( int i = 0; i < mContextSize; i++ )
		mUnit[i].SetInput( an_input[i] ); 
}


void InContext::SetInput( data_type an_input ) 
{
	for ( int i = 0; i < mContextSize; i++ )
		mUnit[i].SetInput( an_input ); 
}


void InContext::SetOutput( void )
{ 
	for ( int i = 0; i < mContextSize; i++ )
		mUnit[i].SetOutput( mUnit[i].GetInput() );
}


void InContext::Update( Layer* ctxLayer ) 
{
	data_type	sumAct, difAct;

	for ( int i = 0; i < mContextSize; i++ )
	{
		difAct = SafeAbs( mUnit[i].GetInput() - ctxLayer->GetOutput(i) );
		sumAct = ( difAct * mUnit[i].GetInput() + ctxLayer->GetOutput(i) ) / 
					( 1.0 + difAct );

		mUnit[i].SetOutput( sumAct );
	}
}


void InContext::Display( ostream* os )
{
	*os << "weights from context to output\n";
    for ( int i = 0; i < mContextSize; i++ )
    {
    	for ( int j = 0; j < mLayerSize; j++ )
    	{
    		AdjustStream( *os, 2, 4, kLeft, true );
    		*os << GetOutWeight( i, j ) << " ";
    	}
    	*os << endl;
    }
	*os << "weights from context to context\n";
    for ( int i = 0; i < mContextSize; i++ )
    {
    	for ( int j = 0; j < mContextSize; j++ )
    	{
    		AdjustStream( *os, 2, 4, kLeft, true );
    		*os << GetCtxtWeight( i, j ) << " ";
    	}
    	*os << endl;
    }
	SetStreamDefaults( *os );
}


void InContext::Data( data_type** vec, int ident, data_type scale )
{
	int i, j;
	
	if ( ident == kModuleOutput )
	{
		for ( i = 0; i < mContextSize; i++ )
			for ( j = 0; j < mLayerSize; j++ )
				vec[i][j] = scale * GetOutWeight( i, j );
	}
	else if ( ident == kModuleOutContext )
	{
		for ( i = 0; i < mContextSize; i++ )
			for ( j = 0; j < mContextSize; j++ )
				vec[i][j] = scale * GetCtxtWeight( i, j );
	}
}

