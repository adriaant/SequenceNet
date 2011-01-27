/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Sample simulation file for using the Sequence API
					This particular code trains a sequential network with one single online sequence. 
	Invoke with:
					./seqnet -e 2000 -b online -d simulations/singleonline -v 12					
*/

#include "SeqGlobal.h"		// SeqGlobal.h contains project wide definitions and the like
#include "Sequence.h"

#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) (srand(x))

#define kNumPats	4

// GLOBALS
extern SequenceAPI	*gSequenceAPI;		// the interface file to the API Library
data_type			**gOnlinePatterns;	// data_type is defined in SeqGlobal.h

// PROTOTYPES
bool	InitNetwork( void );
void	DoSimulation( void );
void	MySetContext( void );
void	MySetInput( void );
void	MySetInput( int idx );


// initializes the network and creates the online pattern storage
bool InitNetwork( void )
{
	int seqErr;

// Create the network
	seqErr = gSequenceAPI->SequenceSetupNetwork( true );
	if ( seqErr != kNoErr ) return false;
		
// record current network settings to log file
	gSequenceAPI->SequenceShow();

// Set the step parameters (subtraction and leaky-integration) for k layers
	int	k = gSequenceAPI->SequenceGetNumLayers();
	gSequenceAPI->SequenceSetParameter( kParAs, gSequenceAPI->SequenceGetParameter( kParAi ) / (k+1) );
	gSequenceAPI->SequenceSetParameter( kParPs, gSequenceAPI->SequenceGetParameter( kParPi ) / (k+1) );

// start clean: reset learning weights and activations (resetting weights also resets the coincidence detector)
	gSequenceAPI->SequenceReset( O_WT | O_ACT );

// Create pattern storage
	gOnlinePatterns = CreateMatrix( 0.0, kNumPats, gSequenceAPI->SequenceGetLayerSize() );

// Initialize patterns
	for ( int i = 0; i < kNumPats; i++ )
	{
		*(gSequenceAPI->GetSequenceLog()) << endl;
		for ( int j = 0; j < gSequenceAPI->SequenceGetLayerSize(); j++ )
		{
			gOnlinePatterns[i][j] = drand48();

			AdjustStream( *(gSequenceAPI->GetSequenceLog()), 3, 6, kLeft, false );
			*(gSequenceAPI->GetSequenceLog()) << gOnlinePatterns[i][j] << " ";
		}
	}
	*(gSequenceAPI->GetSequenceLog()) << endl;
	
	return true;
}


// the actual training regiment
void DoSimulation( void )
{
	int			idx, epoch;
	data_type	err;

// set context
	MySetContext();
	
// train the sequence
	
// record duration of simulation
	gSequenceAPI->SequenceDuration( kStart );			

	for ( epoch = 0; epoch < kNumPats * gSequenceAPI->SequenceGetEpochs(); epoch++ ) 
	{
	// set the current pattern in the sequence
		MySetInput();

	// train this pattern
		gSequenceAPI->SequenceTrainSingle();

	// need to output a line break in quiet mode
		if ( gSequenceAPI->SequenceGetVerbosity() == 0 ) *(gSequenceAPI->GetSequenceLog()) << endl;
			
	// show the current learning error
		*(gSequenceAPI->GetSequenceLog()) << " " << gSequenceAPI->SequenceGetError();

	// terminate training if the learning rate drops to baseline and the error is below criterion
		if ( gSequenceAPI->SequenceGetAlpha() <= gSequenceAPI->SequenceGetAlphaCrit() && 
			 gSequenceAPI->SequenceGetError() <= gSequenceAPI->SequenceGetErrCrit() ) break;
	}

// end time recording, display duration
	gSequenceAPI->SequenceDuration( kEnd );	

// learning is done, proceed with recall
	*(gSequenceAPI->GetSequenceLog()) << "\n\n#Recall after " << epoch << " epochs" << endl;

// cue the recall with the first pattern in the sequence
	MySetInput( 0 );

// loop for a while...
	for ( int i = 0; i < 1000; i++ )
	{
	// do the recall
		gSequenceAPI->SequenceRecall();  

	// need to output a line break in quiet mode
		if ( gSequenceAPI->SequenceGetVerbosity() == 0 ) *(gSequenceAPI->GetSequenceLog()) << endl;

	// compare output of network with pattern set to find the closest match and display
		*(gSequenceAPI->GetSequenceLog()) << " ";		
		err = gSequenceAPI->SequenceCompareOutput( gOnlinePatterns, kNumPats, &idx );
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 4, kLeft, true );
		*(gSequenceAPI->GetSequenceLog()) << idx+1;
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 2, 0, kLeft, true ); 
		*(gSequenceAPI->GetSequenceLog()) << err;
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 1, kLeft, false );
	}

// send a line break to console
	*(gSequenceAPI->GetSequenceLog()) << endl;

// clean up	
	DisposeMatrix( gOnlinePatterns, kNumPats );
}


// set the context to a random vector. Since this is a single sequence learning thing, 
// it would actually be sufficient to set it to zero.
void MySetContext( void )
{
	for ( int i = 0; i < gSequenceAPI->SequenceGetContextSize(); i++ ) 
	{
		if ( drand48() > 0.6 )
			gSequenceAPI->SequenceSetContext( i, 1.0 );
		else
			gSequenceAPI->SequenceSetContext( i, 0.0 );
	}
	gSequenceAPI->SequenceSetContext( kOnline );
}


// create a sequence of kNumPats random patterns
void MySetInput( void )
{
	static int pattern_idx = 0;

	for ( int i = 0; i < gSequenceAPI->SequenceGetLayerSize(); i++ )
		gSequenceAPI->SequenceSetNewInput( i, gOnlinePatterns[pattern_idx][i] );
	
	pattern_idx = ( pattern_idx + 1 ) % kNumPats;
}


// stimulate the network with the idx-th pattern of the learned sequence
void MySetInput( int idx )
{
	for ( int i = 0; i < gSequenceAPI->SequenceGetLayerSize(); i++ )
		gSequenceAPI->SequenceSetInput( i, gOnlinePatterns[idx][i] );
}


