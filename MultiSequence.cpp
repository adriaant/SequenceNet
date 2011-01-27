/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Class implementing a training procedure for incremental learning of
					multiple offline sequences. See MultiOffline.cpp for usage.
*/

#include "MultiSequence.h"

extern SequenceAPI*	gSequenceAPI;	


MultiSequence::MultiSequence( void )
{
	mPatterns 		= NULL;
	mContexts 		= NULL;
	mCues 			= NULL;
	mPatLabels 	 	= NULL;
	mContextLabels  = NULL;
	mCueLabels 		= NULL;
	mNumContexts	= NULL;
	mStats = new StatSpec[gSequenceAPI->SequenceGetNumFiles()];
}


MultiSequence::~MultiSequence()
{
	if ( mPatterns		!= NULL ) DisposeMatrix( mPatterns, mNumPatterns );
	if ( mContexts		!= NULL ) DisposeMatrix( mContexts, mTotalContexts );
	if ( mCues			!= NULL ) DisposeMatrix( mCues, mNumCues );
	if ( mPatLabels		!= NULL ) delete[] mPatLabels;
	if ( mContextLabels != NULL ) delete[] mContextLabels;
	if ( mCueLabels		!= NULL ) delete[] mCueLabels;
	if ( mNumContexts	!= NULL ) delete[] mNumContexts;
	if ( mStats			!= NULL ) delete[] mStats;
}


void MultiSequence::SetPatternsFile( char* filename )
{
	strcpy( mPatternsFile, gSequenceAPI->SequenceGetDirectory() );
	strcat( mPatternsFile, "/" );
	strcat( mPatternsFile, filename );
}


void MultiSequence::SetContextsFile( char* filename )
{
	strcpy( mContextsFile, gSequenceAPI->SequenceGetDirectory() );
	strcat( mContextsFile, "/" );
	strcat( mContextsFile, filename );
}


void MultiSequence::SetCuesFile( char* filename )
{
	strcpy( mCuesFile, gSequenceAPI->SequenceGetDirectory() );
	strcat( mCuesFile, "/" );
	strcat( mCuesFile, filename );
}


// We may want to test recalls for partial context and pattern cues. 
// For this we load three different files: 
// a contexts file, a cues file and a pattern list file
// Note that the patterns, cues and contexts files have labels!
bool MultiSequence::LoadTestFiles( void )
{
	ifstream 	infile;
	int			i;
	int			layerSize = gSequenceAPI->SequenceGetLayerSize();
	
	// make sure working directory is correct
	gSequenceAPI->SequenceDirectory( kOriginalDir );
	
	// open the patterns file
	infile.open( mPatternsFile );
	if ( infile.fail() )
	{
		FileOpenError( mPatternsFile );
		return false;
	}
	// read the number of patterns
	SkipComments( &infile );
	infile >> mNumPatterns;
	// create the labels storage
	mPatLabels = new char[mNumPatterns];
	// create the storage matrix
	mPatterns = CreateMatrix( 0.0, mNumPatterns, layerSize );
	for ( i = 0; i < mNumPatterns; i++ )
	{
		SkipComments( &infile );
		infile >> mPatLabels[i];
		for ( int j = 0; j < layerSize; j++ )
		{
			SkipComments( &infile );
			infile >> mPatterns[i][j];
		}
	}	
	infile.close();

	// open the contexts file
	infile.open( mContextsFile );
	if ( infile.fail() )
	{
		FileOpenError( mContextsFile );
		return false;
	}
	// read the number of patterns
	SkipComments( &infile );
	infile >> mTotalContexts;	
	// create the labels storage
	mContextLabels = new char[mTotalContexts];
	// create the storage matrix
	mContexts = CreateMatrix( 0.0, mTotalContexts, layerSize );
	for ( i = 0; i < mTotalContexts; i++ )
	{
		SkipComments( &infile );
		infile >> mContextLabels[i];
		for ( int j = 0; j < layerSize; j++ )
		{
			SkipComments( &infile );
			infile >> mContexts[i][j];
		}
	}	
	infile.close();

	// open the cues file
	infile.open( mCuesFile );
	if ( infile.fail() )
	{
		FileOpenError( mCuesFile );
		return false;
	}
	// read the number of patterns
	SkipComments( &infile );
	infile >> mNumCues;	
	// create the labels storage
	mCueLabels = new char[mNumCues];
	// create the number of contexts indicator for each cue
	mNumContexts = new int[mNumCues];
	// create the storage matrix
	mCues = CreateMatrix( 0.0, mNumCues, layerSize );
	for ( i = 0; i < mNumCues; i++ )
	{
		SkipComments( &infile );
		infile >> mCueLabels[i];
		SkipComments( &infile );
		infile >> mNumContexts[i];
		for ( int j = 0; j < layerSize; j++ )
		{
			SkipComments( &infile );
			infile >> mCues[i][j];
		}
	}
	infile.close();


	// move working directory back to log dir
	gSequenceAPI->SequenceDirectory( kLogDir );
	return true;
}


// Core routine for training multiple sequences
bool MultiSequence::RunMultiSequenceSimulation( void )
{		
	int     		seqErr;		// any errors passed from API (currently nothing...)
	data_type* 		errorDB;
	int*			array;
	data_type		recallError = 1.0, tmpErr = 1.0;
	int				i, j, ite, idx, epochCtr = 0;
	bool			done = false;
	bool			retVal = false;
	int				currentFileIdx = gSequenceAPI->SequenceGetFileIndex();
	
	// buffer for recall errors
	errorDB = new data_type[gSequenceAPI->SequenceGetNumFiles()];
	for ( i = 0; i < gSequenceAPI->SequenceGetNumFiles(); i++ ) errorDB[i] = 1.0;
	
	if ( gSequenceAPI->SequenceGetOrder() == kPermutedOrder ) // permuted order
	{
		array = new int[gSequenceAPI->SequenceGetNumFiles()];			// array for permuted patterns
		for ( i = 0; i < gSequenceAPI->SequenceGetNumFiles(); i++ ) array[i] = i;
		Permute( array, gSequenceAPI->SequenceGetNumFiles() );		// permute indices
			
		cout << "order of sequence presentation will be:\t";
		for ( int i = 0; i < gSequenceAPI->SequenceGetNumFiles(); i++ ) cout << array[i] << " ";
		cout << endl;
	}
	
	// make sure stats data are initialized
	ClearStats( gSequenceAPI->SequenceGetNumFiles() );
	
	// specify simulation
	cout << "\nTraining multiple sequences from patterns " << gSequenceAPI->SequenceGetBasename() << endl;

	// set formatting for cout
	AdjustStream( cout, 3, 7, kLeft, true );
	
	while ( true )
	{
		// start training a set containing the first sequence, and then increment
		for ( i = 0; i <= currentFileIdx; i++ )
		{
			// the sequence added is indicated differently 
			// based on permuted or linear presentation
			if ( gSequenceAPI->SequenceGetOrder() == kPermutedOrder )
				idx = array[i];
			else
				idx = i;
				
			if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI ) cout << "\n\nSEQUENCE:\t" << idx; // update to log file
			
			// load the patterns
		    seqErr = gSequenceAPI->SequenceLoadPatterns( idx );
		    if ( seqErr != kNoErr ) goto bail;
			// set indicated sequence type. To use sequence type indicated in 
			// pattern file, pass '-1' to this function
			gSequenceAPI->SequenceSetType();
				
			// check if recall is ok
			if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI ) cout << "\nRECALL: ";
		
			switch ( gSequenceAPI->SequenceGetType() )
			{
				case O_NOISE:
				case O_END:
					// we recall twice to check for transients
					RecallSequences( gSequenceAPI->SequenceNumPatterns()-1, &recallError );
					if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI ) cout << endl;
					RecallSequences( gSequenceAPI->SequenceNumPatterns()-1, &tmpErr );
					if ( tmpErr < recallError ) recallError = tmpErr;
					break;
				case O_INF:
					RecallSequences( gSequenceAPI->SequenceGetRecallLen(), &recallError );
					break;
			}
			// add error to db
			errorDB[i] = recallError;
			mStats[i].sRecallCounter += 1;
			mStats[i].sErrAverage += recallError;			
			
			// if the error is bad, train this pattern, otherwise check all recall errors
			if ( errorDB[i] > gSequenceAPI->SequenceGetRecallCrit() )
			{
				if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI ) cout << "\nTRAINING: ";
			
				TrainCurrentSequence( gSequenceAPI->SequenceGetIterations(), &ite );
				mStats[i].sTrainCounter += 1;
				mStats[i].sLRAverage += gSequenceAPI->SequenceGetAlpha();
				mStats[i].sIteAverage += ite;		
			}
			else
			{
				// check if all patterns have correct recall		
				done = true;
				for ( j = 0; j <= currentFileIdx; j++ )
				{
					if ( errorDB[j] > gSequenceAPI->SequenceGetRecallCrit() )
					{
						done = false;
						break;
					}
				}
				
				// when done, add a new sequence or terminate. Else: just repeat
				// also stop when #epochs > some criterion (set to 500). Latter value
				// observed to be the number of epochs after which termination is unlikely
				// to occur. We check if adding a new sequence will help learning
				if ( done )
				{
					// prepare to move to the next sequence, if present
					currentFileIdx += 1;

					// write current stats
					cout << endl;
					PrintStats( currentFileIdx, epochCtr );
					// if all sequences trained, bail out
					if ( currentFileIdx >= gSequenceAPI->SequenceGetNumFiles() )
					{
						retVal = true;
						goto bail;
					}
					// otherwise add a new sequence
					cout << "\n___ ADDING SEQUENCE ";
					if ( gSequenceAPI->SequenceGetOrder() == kPermutedOrder )
						cout << array[currentFileIdx]+1 << " ___";
					else
						cout << currentFileIdx << " ___";
					epochCtr = 0;
					ClearStats( gSequenceAPI->SequenceGetNumFiles() );
				}
			}			

			// each time we're back at the initial sequence, it is recorded as an epoch
			if ( i == 0 ) epochCtr += 1;			
		}
		if ( epochCtr > gSequenceAPI->SequenceGetEpochs() )
		{		
			cout << endl;
			PrintStats( currentFileIdx, epochCtr ); // write current stats
			retVal = true;
			goto bail;
		}
	}
	
bail:
	// clean up the mess
	delete[] errorDB;
	if ( gSequenceAPI->SequenceGetOrder() == kPermutedOrder ) delete[] array;
	
	if ( epochCtr > gSequenceAPI->SequenceGetEpochs() )
	{
		cout << "\nNumber of epochs exceeded\n" << endl;
		return false;
	}
	else return retVal;
}


void MultiSequence::TrainCurrentSequence( int num_iterations, int *final_ite )
{
	int ite;
	// Train the sequence
	for ( ite = 0; ite < num_iterations; ite++ ) 
	{
		// train single pass through sequence
		gSequenceAPI->SequenceTrainFile();  
		// check on lr and err
		// the lr is the average over all patterns (see SequenceTrainFile)
		if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI )
		{
			AdjustStream( cout, 2, 5, kLeft, true ); 
			cout << gSequenceAPI->SequenceGetAlpha() << " "; 
			AdjustStream( cout, 0, 1, kLeft, true );
		}
		if ( gSequenceAPI->SequenceGetAlpha() <= gSequenceAPI->SequenceGetAlphaCrit() ) /* && gSequenceAPI->SequenceGetError() <= gSequenceAPI->SequenceGetErrCrit() */
			break;
	}

	if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI ) cout << "\nfinished at " << ite << " iterations" << endl;

	*final_ite = ite;
}


void MultiSequence::RecallSequences( int num_iterations, data_type *recall_error )
{
	int			i;
	int			newState = -1;	
	data_type	err = 0.0;

	gSequenceAPI->SequenceClearErr();	// clear mErrs and mFreq from gPatterns
	
	// obtain the first pattern in the sequence
	gSequenceAPI->SequenceGetInput( 0 );

	// Set the context
	gSequenceAPI->SequenceSetContext( kOffline );
					
	// Observe the frequency distribution of correctly recalled sequences
	SequenceErrors( 0 );	// reset state information

	// loop for a while...
	for ( i = 0; i < num_iterations; i++ )
	{
		gSequenceAPI->SequenceRecall();  

		if ( gSequenceAPI->SequenceGetType() == O_INF )
		{
			newState = SequenceErrors( 1 );
			if ( newState != -1 ) gSequenceAPI->SequenceSetFreq( newState );
		}
		else
		{
			gSequenceAPI->SequenceSetErr( i+1 );
			if ( gSequenceAPI->SequenceGetErr( i+1 ) < 0.2 ) gSequenceAPI->SequenceSetFreq( i+1 );
		}
	}

	// average and scale errors

	if ( gSequenceAPI->SequenceGetType() == O_INF )
	{
		data_type	avg = 0.0;
		data_type	scale = (data_type)num_iterations;
		
		scale = scale / (data_type)(gSequenceAPI->SequenceNumPatterns());
		
		for ( i = 0; i < gSequenceAPI->SequenceNumPatterns(); i++ )
		{
			avg = (data_type)(gSequenceAPI->SequenceGetFreq(i))/scale - 1.0;
			err += ( avg * avg );
		}
		err = sqrt( err )/(data_type)(gSequenceAPI->SequenceNumPatterns());
	}
	else
	{
		for ( i = 1; i < gSequenceAPI->SequenceNumPatterns(); i++ )
		{
			err += gSequenceAPI->SequenceGetErr(i);
			if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI ) cout << " " << gSequenceAPI->SequenceGetErr(i) << " ";
		}
		err = err/(data_type)(gSequenceAPI->SequenceNumPatterns()-1);
	}	
	if ( gSequenceAPI->SequenceGetVerbosity() & O_MULTI )
	{
		AdjustStream( cout, 4, 6, kLeft, true ); 
		cout << err << " "; 
		AdjustStream( cout, 0, 1, kLeft, true );	
		for ( i = 0; i < gSequenceAPI->SequenceNumPatterns(); i++ ) cout << gSequenceAPI->SequenceGetFreq(i) << " ";
	}
	*recall_error = err;
}


// display recall frequency distribution
int MultiSequence::SequenceErrors( int noReset )
{
	static int	first_state = 0;
	static int	prev_state = 0;
	
	if ( noReset == 0 )
	{
		first_state = 0;
		prev_state = 0;
		return 0;
	}
		
	int		idx = 0;	
	bool	display = 0;
	
	if ( display ) cout << endl;
	
	for ( int i = 0; i < gSequenceAPI->SequenceNumPatterns(); i++ ) 
	{
		gSequenceAPI->SequenceSetErr( i );
		if ( gSequenceAPI->SequenceGetErr(i) <= gSequenceAPI->SequenceGetErr(idx) )  idx = i;
	}

	if ( gSequenceAPI->SequenceGetErr( idx ) < 0.4 ) // 0.4 criterion arbitrary. 
	{
		prev_state = idx;
			if ( display )
			{
				AdjustStream( cout, 0, 3, kLeft, true );
				cout << idx+1 << " ";
				AdjustStream( cout, 0, 5, kLeft, true );
				cout << gSequenceAPI->SequenceGetErr(idx);
				SetStreamDefaults( cout );
			}

		if ( first_state == 0 ) first_state = 1;
		return idx;
	} 
	else 
	{
		if ( first_state == 1 ) 
		{
				if ( display ) 
				{
					AdjustStream( cout, 0, 3, kLeft, true );
					cout << prev_state+1 << " ";
					AdjustStream( cout, 0, 5, kLeft, true );
					cout << gSequenceAPI->SequenceGetErr(idx);
					SetStreamDefaults( cout );
				}
			return prev_state;
		}
		else
		{
				if ( display ) 
				{
					AdjustStream( cout, 0, 3, kLeft, true );
					cout << -1 << " " << 100.0;
					SetStreamDefaults( cout );
				}
			return -1;
		}
	}

	return idx;
}


void MultiSequence::ClearStats( int num )
{
	for ( int i = 0; i < num; i++ )
	{
		mStats[i].sTrainCounter = 0;
		mStats[i].sRecallCounter = 0;
		mStats[i].sIteAverage = 0.0;
		mStats[i].sErrAverage = 0.0;
		mStats[i].sLRAverage = 0.0;
	}
}


void MultiSequence::PrintStats( int patIdx, int epochCtr )
{
	cout << "\ncurrent stats up to " << patIdx << " sequence";
	if ( patIdx > 1 ) cout << "s";
	cout << endl;
	cout << "number of epochs: " << epochCtr << endl;

	// % max epoch calculation
	cerr << epochCtr << "\t" << patIdx << "\t";

	for ( int i = 0; i < patIdx; i++ )
	{
		// print the % epochs
		if ( mStats[i].sTrainCounter != 0 )
			cerr << (((data_type)mStats[i].sTrainCounter/(data_type)epochCtr) * 100.0) << "\t";
		else
			cerr << 0 << "\t";
		
		// log entries
		cout << "sequence: " << i << endl;
		cout << "number of recall trials:\t";
		AdjustStream( cout, 0, 3, kLeft, true );
		cout << mStats[i].sRecallCounter << endl; 
		SetStreamDefaults( cout );
		cout << "average recall error:\t";
		AdjustStream( cout, 3, 6, kLeft, true );
		cout << ( mStats[i].sErrAverage / mStats[i].sRecallCounter ) << endl; 
		SetStreamDefaults( cout );
		cout << "number of training trials:\t";
		AdjustStream( cout, 0, 3, kLeft, true );
		cout << mStats[i].sTrainCounter << endl;
		SetStreamDefaults( cout );
		cout << "average iterations trained:\t";
		AdjustStream( cout, 3, 6, kLeft, true );
		cout << ( mStats[i].sIteAverage / mStats[i].sTrainCounter ) << endl;
		SetStreamDefaults( cout );
		cout << "average learning rate:\t";
		AdjustStream( cout, 3, 6, kLeft, true );
		cout << ( mStats[i].sLRAverage / mStats[i].sTrainCounter ) << endl;
		SetStreamDefaults( cout );
	}
	cout << endl;
	cerr << endl;
}



// test recalls for each cue in mCues and each context in mContexts
// compare output with patterns specified in mPatterns
void MultiSequence::TestRecall( int cur_run )
{
	data_type	sum, err, act1, act2;
	int			i, j, k, idx, run, layerSize = gSequenceAPI->SequenceGetLayerSize();
	int			startCtxt, endCtxt;
	ofstream	orbfile;
	char		orbname[256];
	char		basename[256];
    char		stridx[5];

    sprintf( stridx, "%d", cur_run );
	strcpy( basename, gSequenceAPI->SequenceGetDirectory() );
	strcat( basename, "/" );
    strcat( basename, "orb" );
    strcat( basename, stridx );
    strcat( basename, "-ctx" );

	// loop for each context and each cue
	for ( k = 0; k < mNumCues; k++ )
	{
		if ( k == 0 ) 
			startCtxt = 0;
		else
			startCtxt = mNumContexts[k-1];
		endCtxt = startCtxt + mNumContexts[k];
		
		for ( j = startCtxt; j < endCtxt; j++ )
		{
			// clean activations and coincidence detector activations
	//		gSequenceAPI->SequenceReset( O_CD | O_ACT );
			
			cout << "\nRecall with context \'" << mContextLabels[j];
			cout << "\' and cue \'" << mCueLabels[k] << "\'\n";

			strcpy( orbname, basename );
		    sprintf( (char*)stridx, "%d", j+1 );
		    strcat( (char*)orbname, stridx );
		    strcat( (char*)orbname, "cue" );
		    sprintf( (char*)stridx, "%d", k+1 );
		    strcat( (char*)orbname, stridx );
			orbfile.open( orbname );
			
			run = 0;
redo:
			run++;
			
			// set context and cue
			for ( i = 0; i < layerSize; i++ )
				gSequenceAPI->SequenceSetContext( i, mContexts[j][i] );
			gSequenceAPI->SequenceSetContext( kOnline );
			for ( i = 0; i < layerSize; i++ )
				gSequenceAPI->SequenceSetInput( i, mCues[k][i] );
			
			// recall for a while...
			
			sum = 0.0;
			AdjustStream( cout, 3, 6, kLeft, true );
			
			for ( i = 0; i < gSequenceAPI->SequenceGetRecallLen(); i++ )
			{
				gSequenceAPI->SequenceRecall(); 
				err = gSequenceAPI->SequenceCompareOutput( mPatterns, mNumPatterns, &idx );
				sum += err;

				if ( idx != -1 )
					cout << " " << mPatLabels[idx] << " ";
				else
					cout << " - ";				
				cout << err << endl;

				if ( i == 0 )
					act2 = gSequenceAPI->SequenceData( kModuleHidden );
				else
				{
					act1 = act2;
					act2 = gSequenceAPI->SequenceData( kModuleHidden );
					orbfile << act1 << "\t" << act2 << endl;
				}
			}

			cout << "Average error: " << sum/gSequenceAPI->SequenceGetRecallLen() << endl;

			orbfile << endl;
			
			// for terminating sequences we run this a second time to get rid
			// of residual activations from past sequences
			if ( run == 1 && gSequenceAPI->SequenceGetOverrideType() != O_INF ) goto redo;
			
			orbfile.close();
		}
		cout << endl;
	}
}



