/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Pattern class. Contains everything related to reaading in,
// storing, and accessing patterns. 
// A subclass can be derived if you want to add preprocessing functions

#ifndef __PATTERNS__
#define __PATTERNS__

#include <iostream>
using namespace std;
#include <stdio.h>
#include <fstream>
#include <string.h>

#include "SeqGlobal.h"
#include "Utilities.h"


class Patterns
{

public:

	Patterns();
	Patterns( char* filename, int layerSize, int ctxtSize );
	~Patterns();
	
	// Initiator function
	bool			Initialize( char* filename, int layerSize, int ctxtSize );
	bool			ReadPatterns( void );
	void			DeletePatterns( void );
	void			Reset( void );
	
	// Display function
	void			PrintPatterns( ostream* os );
	
	// Accessor functions
	inline data_type*	GetPattern( int i ) { return (data_type*)mPatterns[i]; }
	inline data_type	GetPattern( int i, int j ) { return mPatterns[i][j]; }	

	inline data_type*	GetContext( void ) { return (data_type*)mContext; }
	inline data_type	GetContext( int i ) { return mContext[i]; }

	inline int			GetNumPatterns( void ) { return mNumPatterns; }

	inline int*			GetFreqArray( void ) { return mFreq; }
	inline int			GetFreq( int idx ) { return mFreq[idx]; };

	inline data_type*	GetErrsArray( void ) { return mErrs; }
	inline data_type	GetErrs( int idx ) { return mErrs[idx]; };

	inline int			GetType( void ) { return mSequenceType; };
	inline void			SetType( int seqType ) { mSequenceType = seqType; }
	
	// Setters
	inline void		SetFreq( int idx, int newVal ) { mFreq[idx] = newVal; };
	inline void		SetErrs( int idx, data_type newVal ) { mErrs[idx] = newVal; };

protected:

	data_type**		mPatterns;
	data_type*		mContext;
	data_type*		mErrs;				// errors in recall
	int*			mFreq;				// frequency distribution
	char			mFileName[256];
	int				mNumPatterns;
	int				mLayerSize;
	int				mCtxtSize;
	int				mSequenceType;		// unterminated or terminated 
										// (untermed, noise, last)
};

#endif
