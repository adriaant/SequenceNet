/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include <unistd.h>
#include "Patterns.h"


Patterns::Patterns(  )
{
	strcpy( mFileName, "patterns" );	// default file name
										// need to add test to see if file exists
	mLayerSize = 0;
	mCtxtSize = 0;
	mNumPatterns = 0;
	mSequenceType = O_INF;
}


Patterns::Patterns( char* filename, int layerSize, int ctxtSize )
{
	strcpy( mFileName, filename );
	mLayerSize = layerSize;
	mCtxtSize = ctxtSize;
	
	ReadPatterns();
}


Patterns::~Patterns()
{
	// clean up the patterns storage matrix
	DeletePatterns();
}


bool Patterns::Initialize( char* filename, int layerSize, int ctxtSize )
{
	strcpy( mFileName, filename );
	mLayerSize = layerSize;
	mCtxtSize = ctxtSize;
	
	return ReadPatterns();
}


void Patterns::Reset( void )
{
	for ( int i = 0; i < mNumPatterns; i++ )
	{
		mFreq[i] = 0;
		mErrs[i] = 0.0;
	}
}


void Patterns::DeletePatterns( void )
{
	// clean up the patterns storage matrix
	if ( mNumPatterns != 0 )
	{
		DisposeMatrix( mPatterns, mNumPatterns );
		delete[] mContext;
		delete[] mFreq;
		delete[] mErrs;
	}
	mNumPatterns = 0;
}


bool Patterns::ReadPatterns( void ) 
{
	ifstream infile;

	// open the file
	infile.open( mFileName );
	if ( infile.fail() )
	{		
		FileOpenError( mFileName );
		return false;
	}

	// read the number of patterns
	SkipComments( &infile );
	infile >> mNumPatterns;

	// read the sequence type
	SkipComments( &infile );
	infile >> mSequenceType;
	
	// create the storage matrix
	mPatterns = CreateMatrix( 0.0, mNumPatterns, mLayerSize );
	mContext = new data_type[mCtxtSize];

	for ( int i = 0; i < mCtxtSize; i++ )
	{
		SkipComments( &infile );
		infile >> mContext[i];
	// test
	//	mContext[i] = mContext[i]/10.0;
	}
	for ( int i = 0; i < mNumPatterns; i++ )
	{
		for ( int j = 0; j < mLayerSize; j++ )
		{
			SkipComments( &infile );
			infile >> mPatterns[i][j];
		}
	}
	
	// close and return
	infile.close();

	mFreq = new int[mNumPatterns];
	mErrs = new data_type[mNumPatterns];
	
	return true;
}


// useless function, but let's keep it in
void Patterns::PrintPatterns( ostream* os ) 
{
	AdjustStream( *os, 2, 4, kLeft, false );
	*os << "    loaded patterns:" << endl;
	for ( int i = 0; i < mNumPatterns; i++ ) 
	{
		for ( int j = 0; j < mLayerSize; j++ ) 
			*os << mPatterns[i][j] << " ";
		*os << "\n";
	}
	*os << "\n";
}

