/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "SeqGlobal.h"	// SeqGlobal.h contains project wide definitions and the like
#include "Sequence.h"	// public include


// This is the inititialization routine. This has to be called first!
SequenceAPI::SequenceAPI( void )
{
	// store stream defaults, since we will be manipulating width and precision
	GetStreamDefaults();

	// initialize the random number generator for noise
	SetSeed( GetSeed() );
	
	// store the current working directory
	getcwd( mSequenceCurDir, FILENAME_MAX );
	strcpy( mSequenceLogDir, mSequenceCurDir );		// init log dir to same dir
	
	mPatterns = new Patterns();	// this just sets an empty pattern class
	
	// set defaults for the network specifications
	strcpy( mBasename, "seq" );
	strcpy( mDirname, "." );
	mNumFiles = 1;
	mFileIndex = 0;
	mRuns = 1;
	mEpochs = 1000;
	mIterations = 10;
	mRecallLen = 100;
	mNumLayers = 8;
	mLayerSize = 16;
	mContextSize = 16;
	mOrder = kPermutedOrder;
	mType = O_INF;
    mVerbosity = O_ERROR;
	mErrCrit = 0.1;
	mAlphaCrit = 0.01;
	mRecallCrit = 0.05;
	mTermNoise = 0.05;
	mError = -1.0;
	mOldInput   = NULL;
	mNewInput   = NULL;
	mInContext  = NULL;
	mTextDisplay = NULL;
	mNetwork = NULL;
	mNoise = NULL;
	mSequenceLog = &cout;
}


// To terminate usage of the API
SequenceAPI::~SequenceAPI( void )
{
	// Clean up!	
	if ( mPatterns	  != NULL ) delete   mPatterns;
	if ( mTextDisplay != NULL ) delete   mTextDisplay;
	if ( mNetwork	  != NULL ) delete   mNetwork;
	if ( mOldInput 	  != NULL ) delete[] mOldInput;
	if ( mNewInput 	  != NULL ) delete[] mNewInput;
	if ( mInContext   != NULL ) delete[] mInContext;
	if ( mNoise	  	  != NULL ) delete[] mNoise;

	chdir( mSequenceCurDir );
}


// logfiles: specify a new log name and a new directory (relative to the program dir)
// pass NULL as dirname if no directory has to be created
int SequenceAPI::OpenSequenceLog( ofstream* log, char* logname )
{
	if ( logname == NULL || log == NULL )
	{
		cerr << "you need to pass a valid file and file name!" << endl;
		return kSequenceFileError;
	}

	mSequenceLog = log;

	// open a new logfile into the log directory (which should have been set with 
	// SequenceSetWorkingDirectory)
	if ( ! log->is_open() )
	{
		log->open( logname );
		if ( log->fail() )
		{
			cerr << "cannot create log file!" << endl;
			return kSequenceFileError;
		}
	}
	return kNoErr;
}


int SequenceAPI::SequenceSetLogDirectory( char* dirname )
{
	// make sure we are in the right directory
	chdir( mSequenceCurDir );
	
	// create the new directory
	if ( dirname != NULL )
	{
		strcpy( mSequenceLogDir, dirname );
		mkdir( mSequenceLogDir, S_IRWXU | S_IRWXG );
		if ( chdir( mSequenceLogDir ) )
		{
			cerr << "cannot create or change to new directory!" << endl;
			return kSequenceFileError;
		}
	}
	else
		strcpy( mSequenceLogDir, mSequenceCurDir );
	
	return kNoErr;
}

// This routine is for changing directories temporarily back to original working directory
// and back again to the log directory. Use it if opening or saving files
int SequenceAPI::SequenceDirectory( bool originalDir )
{
	if ( originalDir == kOriginalDir )
	{
		if ( chdir( mSequenceCurDir ) )
		{
			cerr << "cannot change directories!" << endl;
			return kSequenceFileError;
		}
	}
	else
	{
		if ( chdir( mSequenceLogDir ) )
		{
			cerr << "cannot change directories!" << endl;
			return kSequenceFileError;
		}
	}
	return kNoErr;
}


// Display simulation environment
void SequenceAPI::SequenceShow( void )
{
	*mSequenceLog << "Simulation specifics:" << endl;
	*mSequenceLog << "    number of runs           : " << mRuns << endl;
	*mSequenceLog << "    number of epochs         : " << mEpochs << endl;
	*mSequenceLog << "    number of iterations     : " << mIterations << endl;
	*mSequenceLog << "    recall length            : " << mRecallLen << endl;
	*mSequenceLog << "    verbosity                : ";
	SequenceShowVerbosity( mVerbosity );
	*mSequenceLog << "    file name                : " << mBasename << endl;
	*mSequenceLog << "    directory                : " << mDirname << endl;
	*mSequenceLog << "network specifications:" << endl;
	
	*mSequenceLog << mNumLayers		<< "\tlayers" << endl;
	*mSequenceLog << mLayerSize		<< "\tnodes" << endl;
	*mSequenceLog << mContextSize	<< "\tcontext" << endl;
	*mSequenceLog << mVerbosity			<< "\tverbosity level" << endl;
	AdjustStream( *mSequenceLog, 3, 6, kLeft, true );
	*mSequenceLog << mPar.lrPar			<< "\tlearning rate" << endl;	
	*mSequenceLog << mPar.aInitPar		<< "\tmax leaky integration" << endl;
	*mSequenceLog << mPar.aStepPar		<< "\tstep leaky integration" << endl;
	*mSequenceLog << mPar.pInitPar		<< "\tsubtraction init" << endl;
	*mSequenceLog << mPar.pStepPar		<< "\tsubtraction step" << endl;
	*mSequenceLog << mPar.tInitPar		<< "\tmax threshold" << endl;
	*mSequenceLog << mPar.tStepPar		<< "\tstep threshold" << endl;
	*mSequenceLog << mPar.cPar			<< "\tsigmoid" << endl;
	*mSequenceLog << mPar.nPar			<< "\tsynaptic noise" << endl;
	*mSequenceLog << mPar.cdTPar		<< "\tcd threshold" << endl;
	AdjustStream( *mSequenceLog, 0, 1, kLeft, true );
	*mSequenceLog << mPar.cdDInitPar	<< "\tcd initial delay" << endl;
	*mSequenceLog << mPar.cdDStepPar	<< "\tcd delay increment" << endl;
	AdjustStream( *mSequenceLog, 3, 6, kLeft, true );
	*mSequenceLog << mPar.bPar			<< "\tbase rate learning" << endl;
	*mSequenceLog << mPar.lPar			<< "\tlearning change rate" << endl;
	*mSequenceLog << mPar.mPar			<< "\tcd momentum" << endl;
	*mSequenceLog << mPar.cdWtInitPar	<< "\tcd initial weight" << endl;
	*mSequenceLog << mPar.cdWtStepPar	<< "\tcd weight increment" << endl;
	*mSequenceLog << endl;
	AdjustStream( *mSequenceLog, 0, 1, kLeft, true );
	SetStreamDefaults( *mSequenceLog );
}


// Output the verbosity level in readable format
void SequenceAPI::SequenceShowVerbosity( int verbosity )
{
	if ( verbosity == O_NONE ) *mSequenceLog << "silent mode";
	if ( verbosity & O_ERROR ) *mSequenceLog << "progress ";
	if ( verbosity & O_OUTPUT ) *mSequenceLog << "act ";
	if ( verbosity & O_OUTPUTPLUS ) *mSequenceLog << "act+ ";
	if ( verbosity & O_CDOUT ) *mSequenceLog << "cd ";
	if ( verbosity & O_SAVEDELTA ) *mSequenceLog << "saving weight-changes ";
	if ( verbosity & O_SAVEORB ) *mSequenceLog << "saving total activations ";
	*mSequenceLog << endl;
}	


// Creates network either based on network specification file or by recycling the current
int SequenceAPI::SequenceSetupNetwork( bool recycle )
{
	// This will  initialize the relevant data
	// Delete old network and create new one
	if ( mTextDisplay != NULL ) delete mTextDisplay;
	if ( mNetwork 	  != NULL ) delete mNetwork;

	if ( recycle )
	{
		char filename[256];
		
		strcpy( filename, mDirname );
		strcat( filename, "/" );
		strcat( filename, mBasename );
		strcat( filename, ".net" );
	
		if ( chdir( mSequenceCurDir ) ) cerr << "cannot change directories!" << endl;
		// open the network specs file and read in the details

		if ( SequenceReadSpecs( filename ) ) return kSequenceFileError;
		if ( chdir( mSequenceLogDir ) ) cerr << "cannot change directories!" << endl;
	}

	// Create network
	mNetwork = new Network( mLayerSize, mContextSize, mNumLayers );
	mNetwork->SetParameters( &(mPar) );

	// Initialize display objects: all screen and file outputs go here
	mTextDisplay = new TextDisplay( mNetwork, mVerbosity );
	
	// Reset data buffers
	if ( mOldInput )   delete[] mOldInput;
	if ( mNewInput )   delete[] mNewInput;
	if ( mInContext )  delete[] mInContext;
	if ( mNoise )  	   delete[] mNoise;	

	mOldInput   = new data_type[mLayerSize];
	mNewInput   = new data_type[mLayerSize];
	mInContext  = new data_type[mContextSize];
	mNoise	    = new data_type[mLayerSize];	

	for ( int i = 0; i < mLayerSize  ; i++ ) mNewInput[i]  = 0.0;
	for ( int i = 0; i < mContextSize; i++ ) mInContext[i] = 0.0;
		
	// we set mOldInput to noise, since during training we use mNewInput
	// as a teaching signal and mOldInput as the current input. 
	// So, when learning starts, we have no input yet until the next 
	// input arrives. So the first input will be just plain noise. 
	SequenceNoise( mOldInput );
	SequenceNoise( mNoise );

	// initialize error value
	mError = -1.0;
	
	return kNoErr;
}


// Load a pattern file
// Pattern files are of the form "seq2.pat", where "seq" is the basename,
// "2" is the index, and ".pat" the suffix. 
int SequenceAPI::SequenceLoadPatterns( int idx )
{
	int 	err = kNoErr;
	char	filename[256];
	char	stridx[5];
	
	strcpy( filename, mDirname );
	strcat( filename, "/" );
	strcat( filename, mBasename );
	sprintf( stridx, "-%d", idx );
	strcat( filename, stridx );
	strcat( filename, ".pat" );

	if ( chdir( mSequenceCurDir ) )
	{
		cerr << "cannot change directories!" << endl;
		return kSequenceFileError;
	}
	mPatterns->DeletePatterns();
	bool fileOK = mPatterns->Initialize( filename, mLayerSize, mContextSize );
	if ( ! fileOK )
	{
		FileOpenError( filename );
		err = kSequenceFileError;
	}
	if ( chdir( mSequenceLogDir ) )
	{
		cerr << "cannot change directories!" << endl;
		return kSequenceFileError;
	}
	return err;
}


// Load a pattern file with irregular name (without suffix)
int SequenceAPI::SequenceLoadPatterns( char* patname )
{
	int 	err = kNoErr;
	char	filename[256];
	
	strcpy( filename, mDirname );
	strcat( filename, "/" );
	strcat( filename, patname );
	strcat( filename, ".pat" );

	if ( chdir( mSequenceCurDir ) )
	{
		cerr << "cannot change directories!" << endl;
		return kSequenceFileError;
	}
	mPatterns->DeletePatterns();
	bool fileOK = mPatterns->Initialize( filename, mLayerSize, 
												   mContextSize );
	if ( ! fileOK )
	{
		FileOpenError( filename );
		err = kSequenceFileError;
	}
	if ( chdir( mSequenceLogDir ) )
	{
		cerr << "cannot change directories!" << endl;
		return kSequenceFileError;
	}
	return err;
}


// Set the context from either online or offline
void SequenceAPI::SequenceSetContext( int mode )
{ 
	if ( mode == kOffline )
		mNetwork->SetContext( mPatterns->GetContext() );
	else
		mNetwork->SetContext( mInContext ); 
}


// Get input from pattern file
void SequenceAPI::SequenceGetInput( int idx )
{
	for ( int i = 0; i < mLayerSize; i++ )
		mOldInput[i] = mPatterns->GetPattern( idx, i );
}


// Set the noise signal
void SequenceAPI::SequenceNoise( void )
{
	for ( int i = 0; i < mLayerSize; i++ )
		mNoise[i] = PseudoRNG()*mTermNoise;
}

// Set the noise signal
void SequenceAPI::SequenceNoise( data_type* vec )
{
	for ( int i = 0; i < mLayerSize; i++ )
		vec[i] = PseudoRNG()*mTermNoise;
}

// swap output for input
void SequenceAPI::SequenceSwap( void )
{
	for ( int i = 0; i < mLayerSize; i++ ) mOldInput[i] = mNewInput[i];
}

// Train one single input. That's all.
// Input should be specified in the API class object, any output data is returned
// in the same data struct.
int SequenceAPI::SequenceTrainSingle( void )
{
	// set the context
	mNetwork->SetContext( mInContext );
	// the new provided input is used as a teaching signal for the previous input. 
	// The new input will be propagated when the next input becomes available	
	mNetwork->SetInput( mOldInput );
	mNetwork->LearnPropagate();
	mNetwork->Update();
	
	// adapt the weights from hidden to output, using the new input, store error value
	mError = mNetwork->Adaptation( mNewInput, mInContext );
	// swap the values
	SequenceSwap();

	// text output if necessary
	if ( mVerbosity & O_ERROR || mVerbosity & O_OUTPUT || mVerbosity & O_OUTPUTPLUS || mVerbosity & O_CDOUT )
	{
		PrintNext( mSequenceLog, kReturn );
		mTextDisplay->Print( mSequenceLog, mVerbosity, 0 );
	}

	// store new learning rate
	mAlpha = mNetwork->GetLR();
		
	// should return any errors...
	return 0;
}


// Train sequence from pattern file
int SequenceAPI::SequenceTrainFile( void )
{
	int numpat = mPatterns->GetNumPatterns();
	
	// set context
	SequenceSetContext( kOffline );
	
	// set error and lr to initial values
	mError = 0;
	mAlpha = 0;

	// set noise signal if required
	if ( mPatterns->GetType() == O_NOISE ) SequenceNoise();
	
	for ( int i = 0; i < numpat; i++ )
	{
		SequenceSetInput(i);				// set pattern 
		mNetwork->LearnPropagate();
		mNetwork->Update();
		// Adapt the weights from hidden to output, using the next input
		// the % operator assures that at i==numpat we use i=0
		if ( i == numpat-1 )
		{
			switch ( mPatterns->GetType() )
			{
				case O_NOISE:
					mError += mNetwork->Adaptation( mNoise, mPatterns->GetContext() );
					break;
				case O_END:
					mError += mNetwork->Adaptation( mPatterns->GetPattern(i), mPatterns->GetContext() );
					break;
				case O_INF:
					mError += mNetwork->Adaptation( mPatterns->GetPattern(0), mPatterns->GetContext() );
					break;
			}
		}
		else 
			mError += mNetwork->Adaptation( mPatterns->GetPattern(i+1), mPatterns->GetContext() );

		mAlpha += mNetwork->GetLR();
		
		// text output if necessary
		if ( mVerbosity & O_ERROR || mVerbosity & O_OUTPUT || mVerbosity & O_OUTPUTPLUS || mVerbosity & O_CDOUT )
		{
			PrintNext( mSequenceLog, kReturn );
			mTextDisplay->Print( mSequenceLog, mVerbosity, 0 );
		}
	}

	// show current error for sequence
	mError = mError / numpat;
	if ( mVerbosity & O_ERROR )
	{
		*mSequenceLog << ": ";
		AdjustStream( *mSequenceLog, 3, 6, kLeft, true );
		*mSequenceLog << mError << endl;
	}
		
	// store new learning rate
	mAlpha = mAlpha / numpat;
	
	// should return any errors...
	return 0;
}


// Recall sequence with pattern from file: before calling this function:
//		context should be set (use SequenceSetContext() if using pattern file)
//		pattern cue has been set (use SequenceGetInput(idx) if using pattern file)
int SequenceAPI::SequenceRecall( void )
{
	mNetwork->SetInput( mOldInput );
	mNetwork->RecallPropagate( kInput2Hidden );
	mNetwork->Update();

	// text output if necessary
	if ( mVerbosity & O_ERROR || mVerbosity & O_OUTPUT || mVerbosity & O_OUTPUTPLUS || mVerbosity & O_CDOUT )
	{
		PrintNext( mSequenceLog, kReturn );
		mTextDisplay->Print( mSequenceLog, mVerbosity, 0 );
	}
	
	// copy output to input so we are recalling
	for ( int i = 0; i < mLayerSize; i++ ) mOldInput[i] = mNetwork->GetOutput(i);
		
	// store new learning rate
	mAlpha = mNetwork->GetLR();
	
	return 0;
}


void SequenceAPI::SequenceCallAdaptation( int idx, int numpat )
{
	// Adapt the weights from hidden to output, using the next input
	// the % operator assures that at i==numpat we use i=0
	//err += mNetwork->Adaptation( mPatterns->GetPattern( (i+1)%numpat ), mTextDisplay );
	if ( idx == numpat-1 )
	{
		switch ( mPatterns->GetType() )
		{
			case O_NOISE:
				mError += mNetwork->Adaptation( mNoise, mPatterns->GetContext() );
				break;
			case O_END:
				mError += mNetwork->Adaptation( mPatterns->GetPattern(idx), mPatterns->GetContext() );
				break;
			case O_INF:
				mError += mNetwork->Adaptation( mPatterns->GetPattern(0), mPatterns->GetContext() );
				break;
		}
	}
	else 
		mError += mNetwork->Adaptation( mPatterns->GetPattern(idx+1), mPatterns->GetContext() );
}


data_type SequenceAPI::SequenceData( int identifier )
{
	switch ( identifier )
	{
		case kModuleHidden:
			return mNetwork->GetTotalAct();
			break;
	}
	return 0.0;
}

void SequenceAPI::SequenceData( int identifier, data_type* data, int idx = -1, data_type scale = 1.0 )
{
	switch ( identifier )
	{
		case kModuleHidden:
			if ( idx < 0 )
				mNetwork->DataActivation( data, scale );
			else
				mNetwork->DataActivation( data, idx, scale );
			break;
		case kModuleOutput:
			mNetwork->DataOutput( data, scale );
			break;
		case kModuleOutContext:
			mNetwork->DataContext( data, scale );
			break;
		default:
			break;
	}
}

void SequenceAPI::SequenceData( int from_ident, int to_ident, int idx, data_type** data, data_type scale = 1.0 )
{
	switch ( from_ident )
	{
		case kModuleHidden:
			mNetwork->DataWeight( data, to_ident, idx, scale );
			break;
		case kModuleOutContext:
			mNetwork->DataWeight( data, to_ident, scale );
			break;
		default:
			break;
	}
}


void SequenceAPI::SequenceData( int identifier, int type, int idx )
{
	switch ( identifier )
	{
		case kModuleHidden:
			if ( type == O_ACT )
				mNetwork->DisplayActivation( mSequenceLog );
			else
				mNetwork->DisplayWeight( mSequenceLog, idx );
			break;
		case kModuleOutput:
			mNetwork->DisplayOutput( mSequenceLog );
			break;
		case kModuleCD:
			mNetwork->DisplayCD( mSequenceLog );
			break;
		case kModuleOutContext:
			if ( type == O_ACT )
				mNetwork->DisplayContext( mSequenceLog );
			else
				mNetwork->DisplayWeight( mSequenceLog );
			break;
		default:
			break;
	}
}


// sets network sizes
void SequenceAPI::SequenceSetNumLayers( int val )
{ 
	mNumLayers = val;
	SequenceSetupNetwork( false );
}
void SequenceAPI::SequenceSetLayerSize( int val )
{
	mLayerSize = val;
	SequenceSetupNetwork( false );
}
void SequenceAPI::SequenceSetContextSize( int val )
{
	mContextSize = val;
	SequenceSetupNetwork( false );
}


void SequenceAPI::SequenceSetParameter( int identifier, data_type val )
{
	switch ( identifier )
	{
		case kParLr:
			mPar.lrPar = val;
			break;
		case kParAi:
			mPar.aInitPar = val;
			break;
		case kParAs:
			mPar.aStepPar = val;
			break;
		case kParPi:
			mPar.pInitPar = val;
			break;
		case kParPs:
			mPar.pStepPar = val;
			break;
		case kParTi:
			mPar.tInitPar = val;
			break;
		case kParTs:
			mPar.tStepPar = val;
			break;
		case kParC:
			mPar.cPar = val;
			break;
		case kParN:
			mPar.nPar = val;
			break;
		case kParCd:
			mPar.cdTPar = val;
			break;
		case kParCDi:
			mPar.cdDInitPar = val;
			break;
		case kParCDs:
			mPar.cdDStepPar = val;
			break;
		case kParB:
			mPar.bPar = val;
			break;
		case kParL:
			mPar.lPar = val;
			break;
		case kParM:
			mPar.mPar = val;
			break;
		case kParCDwi:
			mPar.cdWtInitPar = val;
			break;
		case kParCDws:
			mPar.cdWtStepPar = val;
			break;
	}
	mNetwork->SetParameters( &(mPar) );
}


data_type SequenceAPI::SequenceGetParameter( int identifier )
{
	switch ( identifier )
	{
		case kParLr:
			return mPar.lrPar;
			break;
		case kParAi:
			return mPar.aInitPar;
			break;
		case kParAs:
			return mPar.aStepPar;
			break;
		case kParPi:
			return mPar.pInitPar;
			break;
		case kParPs:
			return mPar.pStepPar;
			break;
		case kParTi:
			return mPar.tInitPar;
			break;
		case kParTs:
			return mPar.tStepPar;
			break;
		case kParC:
			return mPar.cPar;
			break;
		case kParN:
			return mPar.nPar;
			break;
		case kParCd:
			return mPar.cdTPar;
			break;
		case kParCDi:
			return mPar.cdDInitPar;
			break;
		case kParCDs:
			return mPar.cdDStepPar;
			break;
		case kParB:
			return mPar.bPar;
			break;
		case kParL:
			return mPar.lPar;
			break;
		case kParM:
			return mPar.mPar;
			break;
		case kParCDwi:
			return mPar.cdWtInitPar;
			break;
		case kParCDws:
			return mPar.cdWtStepPar;
			break;
	}
	return 0.0;
}


// Find similarity with patterns from file. Returns index of pattern and error
data_type SequenceAPI::SequenceCompareOutput( int* idx )
{
	data_type err;
	data_type min_error = 100.0;

	*idx = -1;
	for ( int k = 0; k < SequenceNumPatterns(); k ++ )
	{
		err = SequenceDistance( k );
		if ( err < min_error ) 
		{
			min_error = err;
			*idx = k+1;
		}
	}
	return min_error;
}

// Find similarity with patterns from provided matrix. Returns index of pattern and error
data_type SequenceAPI::SequenceCompareOutput( data_type** patterns, int numpat, int* idx )
{
	data_type	err;
	data_type	min_error = 100.0;

	*idx = -1;
	for ( int k = 0; k < numpat; k++ )
	{
		err = SequenceDistance( patterns[k] );
		if ( err < min_error ) 
		{
			min_error = err;
			*idx = k;
		}
	}
	return min_error;
}


int	SequenceAPI::SequenceReadSpecs( char* filename )
{
	ifstream	infile;

	// open the file
	infile.open( filename );
	if ( infile.fail() ) 
	{
		FileOpenError( filename );
		return kSequenceFileError;
	}
	
	// read the file contents
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mNumLayers;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mLayerSize;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mContextSize;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.lrPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.aInitPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.aStepPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.pInitPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.pStepPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.tInitPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.tStepPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.cPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.nPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.cdTPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.cdDInitPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.cdDStepPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.bPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.lPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.mPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.cdWtInitPar;
	SkipComments( &infile );	// ignore any strings starting with #
	infile >> mPar.cdWtStepPar;
	
	// close the file
	infile.close();
	
	return kNoErr;
}


// This will open the files for saving changes in weights and orbit data. 
// Ony when the corresponding options are set, though.
int SequenceAPI::SequenceOpenFiles( char* filename )
{
	int 	err = kNoErr;
	char	tmpname[256];
	
	strcpy( tmpname, mDirname );
	strcat( tmpname, "/" );
	strcat( tmpname, filename );

	if ( chdir( mSequenceCurDir ) )
	{
		cerr << "cannot change directories!" << endl;
		return kSequenceFileError;
	}

	if ( mVerbosity & O_SAVEDELTA )
	{
		mNetwork->SetSaveDelta( true );
		mNetwork->OpenDeltaFile( filename );
	}
	else
		mNetwork->SetSaveDelta( false );

	if ( chdir( mSequenceLogDir ) )
	{
		cerr << "cannot change directories!" << endl;
		return kSequenceFileError;
	}
	return err;
}


void SequenceAPI::SequenceSpeedTest( bool start )
{
	if ( start == kStart )
	{
		mSequenceStartTime = clock();
	}
	else
	{
		unsigned long tmpHrs, tmpMins, tmpSecs, tmpMicro;
		
		mSequenceEndTime = clock();
		mSequenceEndTime = mSequenceEndTime - mSequenceStartTime;
		tmpSecs = mSequenceEndTime / CLOCKS_PER_SEC;
		tmpMins = tmpSecs / 60;
		tmpHrs  = tmpMins / 60;
		tmpSecs = tmpSecs % 60;
		tmpMins = tmpMins % 60;
		tmpMicro = (long)mSequenceEndTime;
		tmpMicro = tmpMicro % 60;
		PrintNext( mSequenceLog, kReturn );
		PrintNext( mSequenceLog, kIntend );
		cerr << "simulation took ";
		if ( tmpHrs > 0 ) 
			cerr << tmpHrs << " hours, " << tmpMins << " minutes, " << tmpSecs << " seconds and " << tmpMicro << " microsecs\n";
		else if ( tmpMins > 0 )
			cerr << tmpMins << " minutes, " << tmpSecs << " seconds and " << tmpMicro << " microsecs\n";
		else if ( tmpSecs > 0 )
			cerr << tmpSecs << " seconds and " << tmpMicro << " microsecs\n";
		else
			cerr << tmpMicro << " microseconds\n";
	}
}

