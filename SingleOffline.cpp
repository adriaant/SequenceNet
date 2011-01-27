/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Sample simulation file for using the Sequence API
					This particular code trains a sequential network with one single offline sequence. 
	Invoke with:
					./seqnet -e 100 -b sample -d simulations/singleoffline -v 12					
*/

#include "SeqGlobal.h" // SeqGlobal.h contains project wide definitions and the like
#include "Sequence.h"

// GLOBALS
extern SequenceAPI*	gSequenceAPI;	// the interface file to the API Library

// PROTOTYPES
bool	InitNetwork( void );
void	DoSimulation( void );

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

// load pattern file
	seqErr = gSequenceAPI->SequenceLoadPatterns( gSequenceAPI->SequenceGetBasename() );
	if ( seqErr != kNoErr ) return false;
	gSequenceAPI->SequenceShowPatterns();
	
	return true;
}


// the actual training regiment
void DoSimulation( void )
{
	int epoch;
// override sequence type assigned in pattern file
	// this stores the new type in the API
	gSequenceAPI->SequenceSetOverrideType( O_NOISE ); // terminate sequence in random noise vector
													  // alternatively use O_END (end in last pattern)
	// this tells the pattern object to use the new type
	gSequenceAPI->SequenceSetType();
	
// train the sequence
    
// record duration of simulation
	gSequenceAPI->SequenceDuration( kStart );

	for ( epoch = 0; epoch < gSequenceAPI->SequenceGetEpochs(); epoch++ ) 
	{
	// print current epoch
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 6, kLeft, false );
		*(gSequenceAPI->GetSequenceLog()) << epoch+1 << " ";

	// train single pass through sequence (we just call this epoch here, but "iteration" fits the bill as well)
		gSequenceAPI->SequenceTrainFile();  

	// terminate training if the learning rate drops to baseline
		/*
			Note that we do not use the error criterion, because it is not reliable with non-cyclic 
			sequences. The error returned when training the terminator is skewed as we use a noisy 
			terminator. Likewise, using the last pattern as a terminator does not produce a reliable
			error indicator. 
		*/
		if ( gSequenceAPI->SequenceGetAlpha() <= gSequenceAPI->SequenceGetAlphaCrit() ) break;
	}
	
// end time recording, display duration
	gSequenceAPI->SequenceDuration( kEnd );	

// learning is done, proceed with recall
	*(gSequenceAPI->GetSequenceLog()) << "\n\n#Recall after " << epoch << " iterations\n\n";

// cue the recall with the first pattern in the sequence
	gSequenceAPI->SequenceGetInput( 0 );

// loop for a while...
	data_type	min_error = 100.0;
	int			min_error_idx = -1;
	for ( int i = 0; i < gSequenceAPI->SequenceGetRecallLen(); i++ )
	{
	// do the recall
		gSequenceAPI->SequenceRecall(); 
	// compare output of network with pattern set to find the closest match and display
		min_error = gSequenceAPI->SequenceCompareOutput( &min_error_idx );
		*(gSequenceAPI->GetSequenceLog()) << " ";
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 3, 3, kLeft, false );
		*(gSequenceAPI->GetSequenceLog()) << min_error_idx << " ";
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 3, 6, kLeft, false );
		*(gSequenceAPI->GetSequenceLog()) << min_error;
	}

// send a line break to console
	*(gSequenceAPI->GetSequenceLog()) << endl;
}


