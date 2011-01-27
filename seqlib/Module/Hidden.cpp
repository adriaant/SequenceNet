/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "Hidden.h"


Hidden::~Hidden() 
{
	// Clean up dynamically allocated arrays
	delete[] mSAM_Unit;
	
	for ( int i = 0; i < mLayerSize; i++ ) 
		delete[] mOutWts[i];
	delete[] mOutWts;
	for ( int i = 0; i < mLayerSize; i++ ) 
		delete[] mCtxtWts[i];
	delete[] mCtxtWts;
}


void Hidden::Initialize( int layerSize ) 
{
#pragma unused ( layerSize )
	cerr << "Illegal function use: Hidden::Initialize()" << endl;
}


void Hidden::Initialize( int layerSize, int ctxtSize ) 
{
   	int	i, j;
	
	mLayerSize = layerSize;
    mContextSize = ctxtSize;
	
// Create a layer with SAM cells
    mSAM_Unit = new SAM_Unit[mLayerSize];
    
// Create and initialize the weight matrix
	// to the output layer
    mOutWts = new Weight*[mLayerSize];
    for ( i = 0; i < mLayerSize; i++ )
    {
    	mOutWts[i] = new Weight[mLayerSize];
    	for ( j = 0; j < mLayerSize; j++ ) mOutWts[i][j].Reset();
    }
	// to the predicted context layer
    mCtxtWts = new Weight*[mLayerSize];
    for ( i = 0; i < mLayerSize; i++ )
    {
    	mCtxtWts[i] = new Weight[mContextSize];    	
    	for ( j = 0; j < mContextSize; j++ ) mCtxtWts[i][j].Reset();
    }
}


void Hidden::SaveWeights( char* filename, int idx )
{
    ofstream	outfile;
    char		tmpname[255];
    char		stridx[5];
    int			i, j;
    
    strcpy( tmpname, filename );
    sprintf( (char*)stridx, "%d", idx );
    strcat( (char*)tmpname, stridx );
    strcat( (char*)tmpname, ".wts" );
    
	outfile.open( tmpname );

	for ( i = 0; i < mLayerSize; i++ )
	{
		for ( j = 0; j < mLayerSize; j++ ) 
			outfile << i << " " << j << " " << GetOutWt( i, j ) << "\n";
		outfile << "\n";
	}
	for ( i = 0; i < mLayerSize; i++ )
	{
		for ( j = 0; j < mContextSize; j++ ) 
			outfile << i << " " << j << " " << GetCtxtWt( i, j ) << "\n";
		outfile << "\n";
	}

	outfile.close();
}


int Hidden::LoadWeights( char* filename, int idx )
{
	ifstream	infile;
	char		tmpname[255];
	char		stridx[5];
	data_type	dummy1, dummy2, w;
	int			i, j;
	
	strcpy( tmpname, filename );
	sprintf( stridx, "%d", idx );
	strcat( tmpname, stridx );
	strcat( tmpname, ".wts" );
	
	infile.open( tmpname );
	if ( infile.fail() )
	{
		FileOpenError( tmpname );
		return 0;
	}
	for ( i = 0; i < mLayerSize; i++ )
	{
		for ( j = 0; j < mLayerSize; j++ )
		{
			infile >> dummy1 >> dummy2 >> w;
			SetOutWt( i, j, w );
		}
	}
	for ( i = 0; i < mLayerSize; i++ )
	{
		for ( j = 0; j < mContextSize; j++ )
		{
			infile >> dummy1 >> dummy2 >> w;
			SetCtxtWt( i, j, w );
		}
	}

	infile.close();
	
	return 1;
}


void Hidden::Reset( SInt16 resetOption )
{
   	int	i, j;
  
	if ( resetOption & O_WT )
	{
		for ( i = 0; i < mLayerSize; i++ )
			for ( j = 0; j < mLayerSize; j++ ) mOutWts[i][j].Reset();

		for ( i = 0; i < mLayerSize; i++ )
			for ( j = 0; j < mContextSize; j++ ) mCtxtWts[i][j].Reset();
	}
	for ( i = 0; i < mLayerSize; i++ ) mSAM_Unit[i].Reset( resetOption );
}


void Hidden::SetParameters( data_type a, data_type p, data_type T, data_type c ) 
{
    for ( int i = 0; i < mLayerSize; i++ )
    	mSAM_Unit[i].SetParameters( a, p, T, c );
}

void Hidden::SetInput( Layer *fromLayer ) 
{
	for ( int i = 0; i < mLayerSize; i++ )
		mSAM_Unit[i].SetInput( fromLayer->GetOutput(i) );
}


void Hidden::SetInput( data_type *an_input ) 
{
	for ( int i = 0; i < mLayerSize; i++ )
		mSAM_Unit[i].SetInput( an_input[i] ); 
}


void Hidden::Update() 
{
	for ( int i = 0; i < mLayerSize; i++ )
		mSAM_Unit[i].Update();
}


void Hidden::Display( ostream* os )
{
	for ( int i = 0; i < mLayerSize; i++ )
	{
		AdjustStream( *os, 2, 7, kLeft, true );
		*os << GetOutput(i) << " ";
	}
    *os << endl;
	SetStreamDefaults( *os );
}

void Hidden::Display( ostream* os, int idx )
{
	*os << "weights from intermediate layer " << idx << " to output\n";
    for ( int i = 0; i < mLayerSize; i++ )
    {
    	for ( int j = 0; j < mLayerSize; j++ )
    	{
    		AdjustStream( *os, 2, 7, kLeft, true );
    		*os << GetOutWt( i, j ) << " ";
    	}
    	*os << endl;
    }
	*os << "weights from intermediate layer " << idx << " to context\n";
    for ( int i = 0; i < mLayerSize; i++ )
    {
    	for ( int j = 0; j < mContextSize; j++ )
    	{
    		AdjustStream( *os, 2, 7, kLeft, true );
    		*os << GetCtxtWt( i, j ) << " ";
    	}
    	*os << endl;
    }
	SetStreamDefaults( *os );
}


void Hidden::Data( data_type* vec, data_type scale )
{
	if ( vec == nil ) vec = new data_type[mLayerSize];
	for ( int i = 0; i < mLayerSize; i++ )
		vec[i] = scale * GetOutput( i );
}


void Hidden::Data( data_type** vec, int ident, data_type scale )
{
	int i, j;
	
	if ( ident == kModuleOutput )
	{
		for ( i = 0; i < mLayerSize; i++ )
			for ( j = 0; j < mLayerSize; j++ )
				vec[i][j] = scale * GetOutWt( i, j );
	}
	else if ( ident == kModuleOutContext )
	{
		for ( i = 0; i < mLayerSize; i++ )
			for ( j = 0; j < mContextSize; j++ )
				vec[i][j] = scale * GetCtxtWt( i, j );
	}
}


