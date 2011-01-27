/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Sample simulation file for using the Sequence API
					This particular code trains a sequential network with a set of online sequences
					(that are conveniently loaded from file for the purpose of this example). 
					The current example loads 4 sequences of length 4 from file. Contexts are defined
					in the code. The network is trained with the 4 sequences (incrementally) and then tested.
	Invoke with:	
					./seqnet -b seq -d simulations/multionline -R 0.05
*/

#include "SeqGlobal.h"		// SeqGlobal.h contains project wide definitions and the like
#include "Sequence.h"

#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) (srand(x))

#define kMargin	10
#define kSeqLen 4
#define kNumSeqs 4

// GLOBALS
extern SequenceAPI*	gSequenceAPI;		// the interface file to the API Library
data_type** 		gOnlinePatterns;	// storage matrix for patters from file
int					kNumPats = 16;		// total number of patterns (over all 4 sequences)
float				gContexts[4][16] =	// static storage of contexts
	{
		{ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 } 
	};

// PROTOTYPES
bool	InitNetwork( void );
void	DoSimulation( void );
void	KillNetwork( void );
void	MySetContext( int idx );
void	MySetInput( int set, int idx );
void	MySetCue( int set, int idx );
bool	ReadPatterns( void );


// initializes the network and reads in patterns
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

// Create pattern storage
	ReadPatterns();

	return true;
}


void DoSimulation( void )
{
	int			i, k, ite, idx, epoch, numSeqs;
	int			ok = 0;
	float 		tot_a = 0.0;
	int			recallIndices[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	bool		even;
	double		sumVal, alpha, err;
	
// start clean: reset learning weights
	gSequenceAPI->SequenceReset( O_WT );

// TRAIN SEQUENCES
	
// record duration of simulation
	gSequenceAPI->SequenceDuration( kStart );
	
// start with learning one sequence, incrementing if successful
	numSeqs = 1; 

// run for user-specified number of epochs (one epoch being a run through the current set of sequences)
	for ( epoch = 0; epoch < gSequenceAPI->SequenceGetEpochs(); epoch++ ) 
	{
	// AdjustStream formats the cout text output
		AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 1, kLeft, false );
		*(gSequenceAPI->GetSequenceLog()) << "\nepoch: " << epoch << endl;

	// in this example, sequences are presented in successive order
		for ( k = 0; k < numSeqs; k++ )
		{
		// reset coincidence detector and activations when presenting a new sequence
			gSequenceAPI->SequenceReset( O_CD | O_ACT );

		// set associated context
			*(gSequenceAPI->GetSequenceLog()) << "seq: " << k << endl;
			MySetContext(k);
	
// TEST RECALL for current sequence to check if it needs to be trained

		// first cue with first pattern of sequence
			MySetCue( k, 0 );
			
		// propagate activation for user-defined number of iterations 
			sumVal = 0.0;
			for ( i = 0; i < gSequenceAPI->SequenceGetRecallLen(); i++ )
			{
				gSequenceAPI->SequenceRecall();

			// compare output of network with complete pattern set to find the closest match
			// error is returned as well as the index of the closest match
				sumVal += gSequenceAPI->SequenceCompareOutput( gOnlinePatterns, kNumPats, &idx );
				recallIndices[idx] += 1;
			// show recall data
				*(gSequenceAPI->GetSequenceLog()) << idx+1 << " ";
				if ( (i+1) % 25 == 0 ) *(gSequenceAPI->GetSequenceLog()) << endl;
			}
			*(gSequenceAPI->GetSequenceLog()) << endl;
		
		// average error over the number of iterations
			sumVal = sumVal/gSequenceAPI->SequenceGetRecallLen();
			*(gSequenceAPI->GetSequenceLog()) << "recall: " << sumVal << " ";
		
		// check if this recall was correct. We do this the crude way by checking if each pattern in the
		// sequence has been recalled roughly equally (give and take a margin kMargin)
			for ( i = k*kSeqLen; i < k*kSeqLen+kSeqLen; i++ )
			{
				if ( recallIndices[i] < (gSequenceAPI->SequenceGetRecallLen()/kSeqLen) - kMargin )
					even = false;
				else if ( recallIndices[i] > (gSequenceAPI->SequenceGetRecallLen()/kSeqLen) + kMargin )
					even = false;
				else
					even = true;
				if ( even == false ) break;
			}
		
		// output recall frequencies for each pattern during this recall
			for ( i = 0; i < kNumPats; i++ )
			{
				*(gSequenceAPI->GetSequenceLog()) << recallIndices[i] << " ";
				recallIndices[i] = 0;
			}
			*(gSequenceAPI->GetSequenceLog()) << ": ";

		// if a set of sequences is recalled correctly, add a new sequence. 
		// If all sequences were recalled correctly, training is done and we go to full recall of all sequences
			if ( sumVal <= gSequenceAPI->SequenceGetRecallCrit() && even == true )
			{
				ok += 1;
				*(gSequenceAPI->GetSequenceLog()) << ok << endl;
				if ( ok == kNumSeqs )
				{
					*(gSequenceAPI->GetSequenceLog()) << ok << endl;
					goto recall;
				}
				else if ( ok == numSeqs )
				{
					ok = 0;
					numSeqs += 1;
				}
				continue;
			}
			else
			{
				if ( ok > 0 ) ok -= 1;
				*(gSequenceAPI->GetSequenceLog()) << ok << endl;
			}

// TRAIN CURRENT SEQUENCE

		// reset activations and coincidence detector
			gSequenceAPI->SequenceReset( O_CD | O_ACT );
		
		// loop through user specified number of iterations
			for ( ite = 0; ite < gSequenceAPI->SequenceGetIterations(); ite++ )
			{
				AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 1, kLeft, false );
				*(gSequenceAPI->GetSequenceLog()) << "  iteration: " << ite << endl;
	
				alpha = 0.0;
				err = 0.0;
			// set corresponding context
				MySetContext( k );
			// present each pattern in the sequence
				for ( i = 0; i < kSeqLen; i++ )
				{
				// set input to network
					MySetInput( k, i );
				// train the current pattern
					gSequenceAPI->SequenceTrainSingle();
				// obtain performance indicator and print it to console
					*(gSequenceAPI->GetSequenceLog()) << "-> ";
					AdjustStream( *(gSequenceAPI->GetSequenceLog()), 3, 6, kLeft, true );
					*(gSequenceAPI->GetSequenceLog()) << gSequenceAPI->SequenceGetError();
					AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 1, kLeft, false );
					*(gSequenceAPI->GetSequenceLog()) << " | ";
					AdjustStream( *(gSequenceAPI->GetSequenceLog()), 3, 6, kLeft, true );
					*(gSequenceAPI->GetSequenceLog()) << gSequenceAPI->SequenceGetAlpha() << endl;
				// the dynamic learning rate is the indicator of a sequence being learned. We therefore
				// use a more flexible error criterium
					alpha += gSequenceAPI->SequenceGetAlpha();
					err += gSequenceAPI->SequenceGetError();
				}
				if ( (alpha/(double)kSeqLen) <= (gSequenceAPI->SequenceGetAlphaCrit() + 0.001 ) && 
					(err/(double)kSeqLen) <= gSequenceAPI->SequenceGetErrCrit() ) break;
			} // ite
		} // k
	} // epochs

// RECALL SEQUENCES
	// if all sequences have been trained successfully or if the maximum number of epochs was exceeded, 
	// we proceed checking the recall of the full sequence set
recall:

	*(gSequenceAPI->GetSequenceLog()) << endl;
	gSequenceAPI->SequenceDuration( kEnd );	// end time recording, display duration

	for ( k = 0; k < numSeqs; k++ )
	{
		*(gSequenceAPI->GetSequenceLog()) << "\nrecall: " << k << endl;
		gSequenceAPI->SequenceReset( O_CD | O_ACT );
			
	// set context
		MySetContext( k );
	// set input to network
		MySetCue( k, 0 );
	// we loop for a large number of iterations to check stability
		for ( i = 0; i < 2500/*gSequenceAPI->SequenceGetRecallLen()*/; i++ )
		{
			*(gSequenceAPI->GetSequenceLog()) << endl;
			// error testing not yet implemented
			gSequenceAPI->SequenceRecall();  
	
			*(gSequenceAPI->GetSequenceLog()) << " ";
			// compare output of network with pattern set to find the closest match
			err = gSequenceAPI->SequenceCompareOutput( gOnlinePatterns, kNumPats, &idx );
			AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 4, kLeft, true );
			*(gSequenceAPI->GetSequenceLog()) << idx+1;
			AdjustStream( *(gSequenceAPI->GetSequenceLog()), 2, 0, kLeft, true ); 
			*(gSequenceAPI->GetSequenceLog()) << err;
			AdjustStream( *(gSequenceAPI->GetSequenceLog()), 0, 1, kLeft, false );
		}
		*(gSequenceAPI->GetSequenceLog()) << endl;
	}

// make sure we store the final values of the weights and context weights
	char filename[256];
	strcpy( filename, gSequenceAPI->SequenceGetDirectory());
	strcat( filename, "/seq" );	
	gSequenceAPI->SequenceSaveWeights( filename );
	strcpy( filename, gSequenceAPI->SequenceGetDirectory());
	strcat( filename, "/seqc" );	
	gSequenceAPI->SequenceSaveContext( filename );
}


// dispose of the network
void KillNetwork( void )
{	
	DisposeMatrix( gOnlinePatterns, kNumPats );
}


// set the context
void MySetContext( int idx )
{
	for ( int i = 0; i < gSequenceAPI->SequenceGetContextSize(); i++ ) 
		gSequenceAPI->SequenceSetContext( i, gContexts[idx][i] );
// this function must be called to call the network to update its input values
	gSequenceAPI->SequenceSetContext( kOnline );
}


// sets the first pattern of a given sequence as cue for recall
void MySetCue( int seq, int idx )
{
	for ( int i = 0; i < gSequenceAPI->SequenceGetLayerSize(); i++ )
		gSequenceAPI->SequenceSetInput( i, gOnlinePatterns[seq*kSeqLen+idx][i] );
}


// sets given pattern from given sequence
void MySetInput( int seq, int idx )
{
	for ( int i = 0; i < gSequenceAPI->SequenceGetLayerSize(); i++ )
		gSequenceAPI->SequenceSetNewInput( i, gOnlinePatterns[seq*kSeqLen+idx][i] );
}


// read in patterns from file
bool ReadPatterns( void ) 
{
	ifstream infile;
	int 	 dummyInt;
	float	 dummyFloat;
	char	 filename[256];
	
// open the file
	strcpy( filename, gSequenceAPI->SequenceGetDirectory());
	strcat( filename, "/pat.txt" );	
	infile.open( filename );
	if ( infile.fail() )
	{		
		FileOpenError( filename );
		return false;
	}

// read the number of patterns
	SkipComments( &infile );
	infile >> kNumPats;

// read the sequence type, but ignored here as by default we use cyclic sequences
	SkipComments( &infile );
	infile >> dummyInt;
	
// create the storage matrix
	gOnlinePatterns = CreateMatrix( 0.0, kNumPats, gSequenceAPI->SequenceGetLayerSize() );

// read in the pattern values
	for ( int i = 0; i < kNumPats; i++ )
	{
		for ( int j = 0; j < gSequenceAPI->SequenceGetLayerSize(); j++ )
		{
			SkipComments( &infile );
			infile >> gOnlinePatterns[i][j];
		}
	}
	
// close and return
	infile.close();
	return true;
}


