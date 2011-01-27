/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#ifndef __SEQUENCE__
#define __SEQUENCE__

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "Utilities.h"	// Access to useful functions
#include "Network.h"
#include "Rnd.h"
#include "Patterns.h"
#include "TextDisplay.h"


class SequenceAPI
{
public:
	
	SequenceAPI();
	~SequenceAPI();

		// create a log file
	int 				OpenSequenceLog( ofstream* log, char* logname );
	inline void 		SetSequenceLog( ofstream* log ) { mSequenceLog = log; }
	inline void 		SetSequenceLog( ostream* log ) { mSequenceLog = log; }
	inline ostream*		GetSequenceLog( void ) { return mSequenceLog; }
	inline void			CloseSequenceLog( ofstream* log ) { if ( log->is_open() ) log->close(); }	
	// maintain different directories
	int					SequenceSetLogDirectory( char* dirname );
	int 				SequenceDirectory( bool originalDir );
	// show the simulation parameters
	void				SequenceShow( void );
	void				SequenceShowVerbosity( int verbosity );
	// creates Sequence network based on network files passed to program
	int					SequenceSetupNetwork( bool recycle );
	// loads pattern file based on idx
	int					SequenceLoadPatterns( int idx );
	// loads pattern file based on filename without suffix
	int 				SequenceLoadPatterns( char* patname );
	// train in online mode
	int 				SequenceTrainSingle( void );
	// train a pattern file
	int 				SequenceTrainFile( void );
	// recall sequence
	int					SequenceRecall( void );
	// accessor routine
	void 				SequenceCallAdaptation( int idx, int numpat );
	// swap output to input
	void				SequenceSwap( void );

// INLINES
	// resets network: activations, weights, and/or winning nodes
	inline void		SequenceReset( int type ) { mNetwork->Reset( type ); }
	// prints loaded patterns to stderr
	inline void 	SequenceShowPatterns( void ) { mPatterns->PrintPatterns( mSequenceLog ); }
	// routines for determining recall error and recall frequencies
	// reset the mErrs and mFreq array
	inline void 	SequenceClearErr( void ) { mPatterns->Reset(); }
	// accessor routines
	inline void 	SequenceCallPropagate( void ) { mNetwork->LearnPropagate(); }
	inline void 	SequenceCallUpdate( void ) { mNetwork->Update(); }
	inline void		SequenceCallAdaptation( void ) { mNetwork->Adaptation( mNewInput, mInContext ); }
	// To record the duration of a simulation, call this function with 
	// argument "kStart" to start recording time and "kEnd" to output
	// a formatted string displaying the duration of a simulation
	inline void		SequenceDuration( int start ) { SequenceSpeedTest( start ); }


// GETTERS
	// Get type of sequence: cyclic, end terminating, noise terminating
	inline int 			SequenceGetType( void ) { return mPatterns->GetType(); }
	inline int 			SequenceGetOverrideType( void ) { return mType; }
	// get number of patterns loaded
	inline int			SequenceNumPatterns( void ) { return mPatterns->GetNumPatterns(); }
	// Get error for pattern idx
	inline data_type 	SequenceGetErr( int idx ) { return mPatterns->GetErrs( idx ); }
	// Get frequency for pattern
	inline int			SequenceGetFreq( int idx ) { return mPatterns->GetFreq( idx ); }
	// Get the current value of the dynamic learning rate
	inline data_type 	SequenceGetLR( void ) { return mNetwork->GetLR(); }
	inline data_type	SequenceGetAlpha( void ) { return mAlpha; }
	// Get current error value
	inline data_type	SequenceGetError( void ) { return mError; }
	// Get input from pattern file
	void				SequenceGetInput( int idx );
	// retrieves parameter and network settings
	data_type			SequenceGetParameter( int identifier );
	inline int			SequenceGetNumLayers	( void )	{ return mNumLayers;   }
	inline int			SequenceGetLayerSize	( void )	{ return mLayerSize;   }
	inline int			SequenceGetContextSize	( void )	{ return mContextSize; }
	inline int			SequenceGetRuns			( void ) 	{ return mRuns;	 	   }
	inline int			SequenceGetEpochs		( void ) 	{ return mEpochs;      }
	inline int			SequenceGetIterations	( void ) 	{ return mIterations;  }
	inline int			SequenceGetRecallLen	( void )	{ return mRecallLen;   }
	inline int			SequenceGetNumFiles		( void )	{ return mNumFiles;	   }
	inline int			SequenceGetFileIndex	( void )	{ return mFileIndex;   }
	inline int			SequenceGetOrder		( void ) 	{ return mOrder; 	   }
	inline int			SequenceGetVerbosity	( void )	{ return mVerbosity;   }
	inline data_type	SequenceGetErrCrit		( void )	{ return mErrCrit;	   }
	inline data_type	SequenceGetAlphaCrit	( void )	{ return mAlphaCrit;   }
	inline data_type	SequenceGetRecallCrit	( void )	{ return mRecallCrit;  }
	inline char*		SequenceGetDirectory	( void ) 	{ return mDirname;	   }
	inline char*		SequenceGetBasename		( void )	{ return mBasename;	   }

// SETTERS
	// Set type of sequence: cyclic, end terminating, noise terminating
	inline void 		SequenceSetOverrideType( int seqType ) { mType = seqType; }
	inline void 		SequenceSetType( void ) { mPatterns->SetType( mType ); }
	// Set frequency for pattern
	inline void			SequenceSetFreq( int idx ) { mPatterns->SetFreq( idx, mPatterns->GetFreq( idx ) + 1 ); }
	// sets parameter and network settings
	inline void			SequenceSetRuns		   ( int val ) 		{ mRuns = val; 				}
	inline void			SequenceSetEpochs	   ( int val ) 		{ mEpochs = val; 			}
	inline void			SequenceSetIterations  ( int val )		{ mIterations = val; 		}
	inline void			SequenceSetRecallLen   ( int val ) 		{ mRecallLen = val; 		}
	inline void			SequenceSetNumFiles	   ( int val ) 		{ mNumFiles = val; 			}
	inline void			SequenceSetFileIndex   ( int val ) 		{ mFileIndex = val; 		}
	inline void			SequenceSetOrder	   ( int val ) 		{ mOrder = val; 			}
	inline void			SequenceSetVerbosity   ( int val ) 		{ mVerbosity = val; 		}
	inline void			SequenceSetErrCrit	   ( data_type val ){ mErrCrit = val; 			}
	inline void			SequenceSetAlphaCrit   ( data_type val ){ mAlphaCrit = val; 		}
	inline void			SequenceSetRecallCrit  ( data_type val ){ mRecallCrit = val; 		}
	inline void			SequenceSetBasename	   ( char* str ) 	{ strcpy( mBasename, str ); }
	inline void			SequenceSetDirectory   ( char* str ) 	{ strcpy( mDirname, str );  }
	// set network sizes
	void				SequenceSetNumLayers( int val );
	void				SequenceSetLayerSize( int val );
	void				SequenceSetContextSize( int val );
	// sets online values
	inline void			SequenceSetContext( int idx, data_type val ) { mInContext[idx] = val; }
	inline void			SequenceSetInput( int idx, data_type val ) { mOldInput[idx] = val; }
	inline void			SequenceSetNewInput( int idx, data_type val ) { mNewInput[idx] = val; }
	// reset alpha value
	inline void			SequenceSetAlpha( data_type val ) { mAlpha = val; }
	// reset error value
	inline void			SequenceSetError( data_type val ) { mError = val; }
	// Set error for pattern idx
	inline void 		SequenceSetErr(int idx) {mPatterns->SetErrs(idx, mNetwork->QuadraticError(mPatterns->GetPattern(idx)));}
	// Set the context from either online or offline
	void				SequenceSetContext( int mode );
	// Set the input from pattern file
	inline void			SequenceSetInput( int idx ) { mNetwork->SetInput( mPatterns->GetPattern( idx ) ); }
	// set noise for sequences terminating in noise signal
	void				SequenceNoise( void );
	void				SequenceNoise( data_type* vec );
	// sets various nework parameters
	void 				SequenceSetParameter( int identifier, data_type val );
	
// FILE HANDLING
	// routines for saving, loading, and displaying data
	inline void			SequenceSaveWeights( char* filename ) { mNetwork->SaveWeights( filename ); }
	inline void			SequenceLoadWeights( char* filename ) { mNetwork->LoadWeights( filename ); }
	inline void			SequenceSaveContext( char* filename ) { mNetwork->SaveContext( filename ); }
	inline void			SequenceLoadContext( char* filename ) { mNetwork->LoadContext( filename ); }
	// manage files for storing weight changes
	int 				SequenceOpenFiles( char* filename );
	inline void 		SequenceCloseFiles( void ) { mNetwork->CloseDeltaFile(); }

// DATA HANDLING
	data_type 	SequenceData( int identifier );
	void 		SequenceData( int identifier, data_type* data, int idx, data_type scale );
	void 		SequenceData( int from_ident, int to_ident, int idx, data_type** data, data_type scale );
	void		SequenceData( int identifier, int type, int idx );

// UTILITIES
	// Find similarity with patterns from file. Returns index of pattern and error
	data_type			SequenceCompareOutput( int* idx );
	// Find similarity with patterns from memory. Returns index of pattern and error
	data_type 			SequenceCompareOutput( data_type** patterns, int numpat, int* idx );
	// calculates Euclidean distance between current output and a given pattern
	inline data_type	SequenceDistance( data_type* pat ){ return mNetwork->QuadraticError( pat ); }
	inline data_type	SequenceDistance( int idx ){ return mNetwork->QuadraticError(mPatterns->GetPattern(idx)); }

private:

	int		SequenceReadSpecs( char* newfilename );
	void	SequenceSpeedTest( bool start );

	Network*		mNetwork;						// network class is the meat of the matter
	Patterns*		mPatterns;						// pattern class to read and store patterns
	TextDisplay*	mTextDisplay;					// displays any info during training & recall
	ostream*   		mSequenceLog;					// redirected cout
	char			mSequenceCurDir[FILENAME_MAX];	// global to store the main directory
	char			mSequenceLogDir[FILENAME_MAX];	// global to store log file directory
	clock_t			mSequenceStartTime;				// to report duration of sims
	clock_t			mSequenceEndTime;

	char		mBasename[32];	// the base name of the file, without suffix
	char		mDirname[255];	// directory with network files
	int			mNumFiles;		// number of files used in multi-sequence training
	int			mFileIndex;		// index of file to be trained first
	int			mRuns;			// number of runs
	int			mEpochs;		// number of epochs for multi-sequence training
	int			mIterations;	// number of iterations for recall loop
	int			mRecallLen;		// length of recall
	int			mNumLayers;		// number of intermediate layers used
	int			mLayerSize;		// size of layer
	int			mContextSize;	// size of context layer
	int			mVerbosity;		// Indicates the details to show in the onscreen text report
	int			mOrder;			// order of sequence presentation
	int			mType;			// type of sequence
	data_type	mErrCrit;		// criterion for training error
	data_type	mAlphaCrit;		// criterion for base rate learning
	data_type	mRecallCrit;	// criterion for correct recall
	data_type	mAlpha;			// current learning rate
	data_type	mError;			// error for current training item
	data_type	mTermNoise;		// noise range for sequences terminating in noise
	data_type*	mOldInput;		// pointer to old input value array
	data_type*	mNewInput;		// pointer to new input value array
	data_type*	mInContext;		// pointer to input context array
	data_type*	mNoise;			// pointer to noisy signal array (terminator)
	ParBlock	mPar;			// struct for SAM parameters, defined in SeqGlobal.h
};


#endif

