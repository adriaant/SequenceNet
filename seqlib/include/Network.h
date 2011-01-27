/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#ifndef __NET__
#define __NET__

#include <iostream>
using namespace std;
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "SeqGlobal.h"
#include "Utilities.h"

#include "Input.h"
#include "InContext.h"
#include "Hidden.h"
#include "Output.h"
#include "OutContext.h"
#include "CD_Layer.h"
#include "Patterns.h"

enum { kInput2Hidden, kHidden2Output, kOutput2Input };


class Network 
{   
	// give access to private data to our Display classes
	friend class TextDisplay;
	friend class GLDisplay;
	
public:

	Network( int mLayerSize, int mCtxtSize, int mTotalHidden );
	~Network();

	void			Reset( SInt16 resetOption );
	void			SetParameters( ParBlock* par );
	
	inline void		SetInput( data_type *an_input ) { mInput->SetInput( an_input ); }
	inline void		SetInput( data_type an_input, int idx ) { mInput->SetInput( an_input, idx ); }
	inline void		SetContext( data_type *an_input ) { mInContext->SetInput( an_input ); }
	inline void		SetContext( void ) { for ( int i=0; i<mCtxtSize; i++ ) mInContext->SetInput( 0.0 ); }
	
// SAVERS
	void			SaveWeights( char* filename );
	void			LoadWeights( char* filename );
	void			SaveContext( char* filename );
	void			LoadContext( char* filename );	
	void			OpenDeltaFile( char* filename );
	void			CloseDeltaFile( void );
	inline void		SetSaveDelta( bool val ) { mSaveDelta = val; }
	int				Save();
	int				Save( void* saveData );

// MISC			
	void 			InitializeRecall( Patterns* patterns, int startPosition, int initLen );
	data_type		QuadraticError( data_type * );
	data_type		GetTotalAct( void );
	void			AddNoise( data_type noiseVal );

// GETTERS	
	inline int			GetNumLayer( void ) { return mTotalHidden; }
	inline int			GetLayerSize( void ) { return mLayerSize; }
	inline int			GetContextSize( void ) { return mCtxtSize; }
	inline Hidden*		GetLayerArray( void ) { return mHidden; }
	inline Hidden*		GetLayerPtr( int idx ) { return &mHidden[idx]; }
	inline Output*		GetOutput() { return mOutput; }
	inline data_type	GetOutput( int idx ) { return mOutput->GetOutput( idx ); }
	inline Input*		GetInput() { return mInput; }
	inline CD_Layer*	GetCD() { return mCDLayer; }
	inline data_type	GetCDOutput( int idx ) { return mCDLayer->GetOutput( idx ); }
	inline data_type	GetLR() { return mCDLayer->GetLR(); }

// DISPLAY FUNCTIONS
	inline void		DisplayWeight( ostream* os ) 	  		{ mInContext->Display( os );       }
	inline void		DisplayWeight( ostream* os, int idx )   { mHidden[idx].Display( os, idx ); }
	inline void		DisplayCD( ostream* os )		 	 	{ mCDLayer->Display( os );         }
	inline void		DisplayContext( ostream* os )	  		{ mOutContext->Display( os );	   }
	inline void		DisplayOutput( ostream* os )	  		{ mOutput->Display( os );		   }
		   void		DisplayActivation( ostream* os );

// DATA FUNCTIONS
	inline void		DataWeight( data_type** data, int ident, data_type scale ){ mInContext->Data( data, ident, scale );  }
	inline void		DataWeight( data_type** data, int ident, int idx, data_type scale ) { mHidden[idx].Data( data, ident, scale ); }
	inline void		DataContext( data_type* data, data_type scale )	{ mOutContext->Data( data, scale ); }
	inline void		DataOutput( data_type* data, data_type scale ) { mOutput->Data( data, scale );	 }
	inline void		DataActivation( data_type* data, int idx, data_type scale )  { mHidden[idx].Data( data, scale ); }
	void			DataActivation( data_type* data, data_type scale  );

// TRAINING AND RECALL
	void			LearnPropagate( void );
	void			RecallPropagate( SInt16 direction );
	void 			Update( void );
	data_type 		Adaptation( data_type* target, data_type* ctxt );

private:

	int   			mLayerSize;		// layer size
	int				mCtxtSize;		// context size
	int   			mTotalHidden;	// number of hidden layers

	Input 			*mInput;		// input layer ( no function )
	InContext		*mInContext;	// context input layer
	Hidden  		*mHidden;		// SAM neurons
	Output			*mOutput;		// activations of output layer, sigmoid activated
	OutContext		*mOutContext;	// context output layer
	CD_Layer		*mCDLayer;		// coincidence detector layer
	
	data_type		*mTotalDeltaWt;	// global weight adaptation for a layer to M
	data_type		*mA;			// parameter array for 'a'
	data_type		*mP;			// parameter array for 'p'
	data_type		*mT;			// parameter array for 'T'
	data_type		mAlphaLR;		// learning rate
	data_type		mNoise;			// synaptic noise value
	
	bool			mSaveDelta;		// saving weight changes
	ofstream		*mFile;			// file for weight changes
};


#endif

