/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Main file for using the Sequence API
					This code creates an executable that uses the SequenceNet API. 
					The user has to provide the functions InitNetwork() and DoSimulation().
*/

#include <stdlib.h>
#include "SeqGlobal.h"		// SeqGlobal.h contains project wide definitions and the like
#include "Sequence.h"		// the interface file to the Seq API Library

// defined in user-provided simulation file
extern bool	InitNetwork( void );
extern void DoSimulation( void );

// PROTOTYPE
void Usage( void );

// GLOBALS
SequenceAPI* gSequenceAPI;	// pointer to API interface


int main( int argc, char *argv[] )
{
	int arg;

// initialize the API instance. This MUST be called at the start of the program!
	gSequenceAPI = new SequenceAPI;

// set default parameters for the SequenceNet API
	gSequenceAPI->SequenceSetRuns( 1 );
	gSequenceAPI->SequenceSetEpochs( 500 );
	gSequenceAPI->SequenceSetIterations( 10 );
	gSequenceAPI->SequenceSetRecallLen( 100 );
	gSequenceAPI->SequenceSetNumFiles( 1 );
	gSequenceAPI->SequenceSetFileIndex( 0 );
	gSequenceAPI->SequenceSetVerbosity( 0x00 );
	gSequenceAPI->SequenceSetErrCrit( 0.05 );
	gSequenceAPI->SequenceSetAlphaCrit( 0.01 );
	gSequenceAPI->SequenceSetRecallCrit( 0.05 );

// process command-line arguments to get custom values for the above parameters 	
	if( argc > 1 )
	{
		// run down each argument
		arg = 1;
		while( arg < argc )
		{
			// check if -h is called
			if( strcmp( argv[arg], "-h") == 0 )
			{
				arg++;
				Usage();
			}
			// perhaps -r : number of runs	
			if( strcmp( argv[arg], "-r") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetRuns( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -e : number of epochs
			if( strcmp( argv[arg], "-e") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetEpochs( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -i : number of iterations
			if( strcmp( argv[arg], "-i") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetIterations( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -l : number of iterations for recall
			if( strcmp( argv[arg], "-l") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetRecallLen( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -f : number of files (i.e. sequences) to train 
			// (only for offline training method, see MultiSequence.h and .cpp)
			if( strcmp( argv[arg], "-f") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetNumFiles( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -F : index of first file to train
			// (only for offline training method, see MultiSequence.h and .cpp)
			if( strcmp( argv[arg], "-f") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetFileIndex( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -p : linear or permuted presentation	of sequences
			if( strcmp( argv[arg], "-p") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetOrder( atoi( argv[arg] ) );
				goto loop;
			}
			// perhaps -t : sequence type (cyclic or terminated)
			if( strcmp( argv[arg], "-t") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetOverrideType( atoi( argv[arg] ) );
				goto loop;
			}
			// verbosity -v		
			if( strcmp( argv[arg], "-v") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetVerbosity( atoi( argv[arg] ) );
				goto loop;
			}
			// error criteria : training error
			if( strcmp( argv[arg], "-E") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetErrCrit( atof( argv[arg] ) );
				goto loop;
			}
			// error criteria : learning rate
			if( strcmp( argv[arg], "-a") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetAlphaCrit( atof( argv[arg] ) );
				goto loop;
			}
			// error criteria for correct recall
			if( strcmp( argv[arg], "-R") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetRecallCrit( atof( argv[arg] ) );
				goto loop;
			}
			// base file name -b
			if( strcmp( argv[arg], "-b") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetBasename( argv[arg] );
				goto loop;
			}
			// directory -d for files
			if( strcmp( argv[arg], "-d") == 0 )
			{
				arg++;
				if( argv[arg] == nil ) Usage();
				gSequenceAPI->SequenceSetDirectory( argv[arg] );
				goto loop;
			}
loop:
			arg++;
		}
	}

// initialize network
	if ( InitNetwork() )
	{
	// run simulation (defined in simulation code file)
		DoSimulation();
	}
	
// clean the API instance
	delete gSequenceAPI;
	
	return 0;
}

// show the command line options if incorrectly engaged or if the user uses the -h flag
void Usage( void )
{
    cerr << "Usage: seqnet (-OPTIONS) [default]" << endl;
    cerr << "    -h        = display this help and exit" << endl;
    cerr << "    -r [1]    = number of simulations to run" << endl;
    cerr << "    -e [5000] = maximum number of epochs to present each set of sequences" << endl;
    cerr << "    -i [10]   = number of iterations to train a single pattern" << endl;
    cerr << "    -l [100]  = number of iterations for single recall" << endl;
    cerr << "    -p [1]    = presentation order: 0 (linear) or 1 (permuted)" << endl;
    cerr << "    -t [0]    = sequence type: 0 (cyclic), 1 (end-terminating), 2 (noise terminating)" << endl;
    cerr << "    -E [0.1]  = error criterium" << endl;
    cerr << "    -a [0.01] = learning rate criterium" << endl;
    cerr << "    -R [0.05] = criterium for correct recall" << endl;
    cerr << "    -b [seq]  = base file name for network files. Files should be suffixed as:" << endl;
    cerr << "                .net : network definition" << endl;
    cerr << "                -x.pat : pattern file with index x (online for offline multi-sequence training)" << endl;
    cerr << "    -d [.]    = directory containing the network files" << endl;
    cerr << "    -f [1]    = number of pattern files to train" << endl;
    cerr << "    -F [0]    = index of first pattern file to train" << endl;
    cerr << "    -v [1]    = verbosity level, defined as a bitwise operation of:" << endl;
    cerr << "                0  : silent mode" << endl;
    cerr << "                1  : activations" << endl;
    cerr << "                2  : formatted activations" << endl;
    cerr << "                4  : detailed learning progress" << endl;
    cerr << "                8  : coincidence detection layer output" << endl;
    cerr << "                16 : display detailed progress multi sequence learning" << endl;
    cerr << "                32 : save weight-changes" << endl;
    cerr << "                64 : save total activation-changes" << endl;
	exit(0);
}
