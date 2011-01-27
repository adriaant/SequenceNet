/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Sample simulation file for using the Sequence API
					This particular code trains a sequential network with a set of offline sequences
					using the MultiSequence class. 
	Invoke with:
					
	./seqnet -F 0 -f 10 -b king -d simulations/tekken -R 0.05 -p 0 -e 1000 > simulations/tekken/log.txt
*/

#include "MultiSequence.h"

// GLOBALS
extern SequenceAPI*	gSequenceAPI;	// the interface file to the API Library

// PROTOTYPES
bool InitNetwork( void );
void DoSimulation( void );


// initializes the network
bool InitNetwork( void )
{
	int seqErr;

// every library output should go to cout
	gSequenceAPI->SetSequenceLog( &cout ); 
	
// Create the network
	seqErr = gSequenceAPI->SequenceSetupNetwork( true );
	if ( seqErr != kNoErr ) return false;
		
// record current network settings to log file
	gSequenceAPI->SequenceShow();

// Set the step parameters (subtraction and leaky-integration) for k layers
	int	k = gSequenceAPI->SequenceGetNumLayers();
	gSequenceAPI->SequenceSetParameter( kParAs, gSequenceAPI->SequenceGetParameter( kParAi ) / (k+1) );
	gSequenceAPI->SequenceSetParameter( kParPs, gSequenceAPI->SequenceGetParameter( kParPi ) / (k+1) );

	return true;
}


// the meat of the simulation
void DoSimulation( void )
{
	char filename[256];
	bool retVal;
	
// create the multi sequence training instance
	MultiSequence* multiSeq = new MultiSequence();
// We may want to test recalls for partial context and pattern cues. 
	// For this we load three different files: 
	// a contexts file, a cues file and a pattern list file
	// Set names for test files. Directory names are internally suppended
	multiSeq->SetPatternsFile( "patterns.txt" );
	multiSeq->SetContextsFile( "contexts.txt" );
	multiSeq->SetCuesFile( "cues.txt" );

// note that the patterns, cues and contexts files have labels!
	if ( !multiSeq->LoadTestFiles() ) goto bail;

// record duration of simulation
	gSequenceAPI->SequenceDuration( kStart );

// perform simulation for user-specified number of runs (usually just one)
	for ( int run = 0; run < gSequenceAPI->SequenceGetRuns(); run++ )
	{
		*(gSequenceAPI->GetSequenceLog()) << "\nRUN " << run << endl;

	// start clean: reset learning weights, activations 
	// (resetting weights also resets coincidence detector)
		gSequenceAPI->SequenceReset( O_WT | O_ACT | O_CD );

	// run the multi-sequence learning procedure
		retVal = multiSeq->RunMultiSequenceSimulation();

	// make sure we store the final values of the weights and context weights
		strcpy( filename, gSequenceAPI->SequenceGetDirectory());
		strcat( filename, "/seq" );	
		gSequenceAPI->SequenceSaveWeights( filename );
		strcpy( filename, gSequenceAPI->SequenceGetDirectory());
		strcat( filename, "/seqc" );	
		gSequenceAPI->SequenceSaveContext( filename );

		if ( retVal == false ) goto bail;
		
	// do big recall test
		multiSeq->TestRecall( run );
	}

// output duration of simulation
	gSequenceAPI->SequenceDuration( kEnd );
	
bail:

	delete multiSeq;
}

