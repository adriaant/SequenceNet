/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#include "Network.h"
#include "TextDisplay.h"
#include "Rnd.h"


// custom Network constructor. Do NOT use default constructor
Network::Network( int mLayerSize, int mCtxtSize, int mTotalHidden ) 
{
	int	i;
	
	this->mLayerSize 	= mLayerSize;
	this->mCtxtSize     = mCtxtSize;
	this->mTotalHidden	= mTotalHidden;
	
// initialize the various components of the network
	
	// input layer initialized to given size 
	mInput	= new Input( mLayerSize );
	
	// context layers (input&output) initialized to given size 
	mInContext  = new InContext( mCtxtSize, mLayerSize );
	mOutContext = new OutContext( mCtxtSize );
	
	// create array of hidden layers of given number
	mHidden = new Hidden[mTotalHidden];
	
	// initialize each of the hidden layer in the new array
	for ( i = 0; i < mTotalHidden; i++ ) mHidden[i].Initialize( mLayerSize, mCtxtSize );
	
	// create output layer by initalizing an array of activation values
	mOutput = new Output( mLayerSize );

	// create coincidence detector layer
	mCDLayer = new CD_Layer( mLayerSize );
	
	// create data storage for record of global weight change 
	// for each of the hidden layers
	mTotalDeltaWt = new data_type[mTotalHidden];
	
	// initialize parameter arrays
	mA = new data_type[mTotalHidden];
	mT = new data_type[mTotalHidden];
	mP = new data_type[mTotalHidden];
	
	// set to default values
	data_type initVal = 0.50;
	for ( i = 0; i < mTotalHidden; i++ )
	{
		mA[i] = 0.97;
		mT[i] = 0.90;
		mP[i] = initVal;
		initVal -= 0.03;
	}

	mNoise		= 0.00001;
	mAlphaLR 	= 0.1;
	
	// create file streams to record weight updates
	mSaveDelta = false;
	mFile = new ofstream[mTotalHidden];
}


// destructor: Free up allocated memory
Network::~Network() 
{
	if ( mA ) delete[] mA;
	if ( mT ) delete[] mT;
	if ( mP ) delete[] mP;
	if ( mTotalDeltaWt ) delete[] mTotalDeltaWt;
	
	if ( mHidden )	   delete[] mHidden;
	if ( mInput )	   delete mInput;
	if ( mInContext )  delete mInContext;
	if ( mOutput ) 	   delete mOutput;
	if ( mOutContext ) delete mOutContext;
	if ( mCDLayer )    delete mCDLayer;
	
	// close filestreams if opened
	for ( int i = 0; i < mTotalHidden; i++ )
	{
		if ( mFile[i].is_open() ) 
			mFile[i].close();
	}
	delete[] mFile;
}


/*--------------------------------------*
 *		LOAD/SAVE FUNCTIONS				*
 *--------------------------------------*/

void Network::SaveWeights( char* filename )
{
	for ( int i = 0; i < mTotalHidden; i++ )
		mHidden[i].SaveWeights( filename, i );
}

void Network::LoadWeights( char* filename )
{
	for ( int i = 0; i < mTotalHidden; i++ )
	{
		if ( ! mHidden[i].LoadWeights( filename, i ) )
			return;
	}
	cerr << '\t' << "weights are loaded.\n";
}


void Network::SaveContext( char* filename )
{
	mInContext->SaveContext( filename );
}

void Network::LoadContext( char* filename )
{
	mInContext->LoadContext( filename );
	cerr << '\t' << "context weights are loaded.\n";
}


void Network::OpenDeltaFile( char* filename )
{
    char	tmpname[255];
    char	stridx[5];
    
	// create file streams to record weight updates
	for ( int i = 0; i < mTotalHidden; i++ )
	{
		if ( mFile[i].is_open() ) mFile[i].close();

	    strcpy( tmpname, filename );
	    sprintf( stridx, "%d", i );
	    strcat( tmpname, stridx );
	    strcat( tmpname, "-dw" );

		mFile[i].open( tmpname );
	}
}

void Network::CloseDeltaFile( void )
{
	for ( int i = 0; i < mTotalHidden; i++ )
		if ( mFile[i].is_open() ) mFile[i].close();
	SetSaveDelta( false );
}

int	Network::Save( void* saveData )
{
	if ( mSaveDelta )
	{
		data_type*	dataCast = static_cast<data_type*>( saveData );
			
		for ( int i = 0; i < 1; i++ )
			mFile[i] << dataCast[i] << endl;
	}
	return 0;
}

// write a break to file
int	Network::Save( void )
{
	if ( mSaveDelta )
	{
		for ( int i = 0; i < mTotalHidden; i++ ) mFile[i] << "\n";
	}
	return 0;
}


/*--------------------------------------*
 *		      RESET FUNCTION			*
 *--------------------------------------*/

void Network::Reset( SInt16 resetOption )
{
	if ( resetOption & O_CD || resetOption & O_WT ) mCDLayer->Reset( resetOption );
	mOutput->Reset( resetOption );
	mInput->Reset( resetOption );
	mInContext->Reset( resetOption );
	mOutContext->Reset( resetOption );
	for ( int i = 0; i < mTotalHidden; i++ ) mHidden[i].Reset( resetOption );
	for ( int i = 0; i < mTotalHidden; i++ ) mTotalDeltaWt[i] = 0.0;
}


/*--------------------------------------*
 *		  SETTINGS FUNCTIONS 			*
 *--------------------------------------*/

void Network::SetParameters( ParBlock* par )
{
	data_type	aVal = par->aInitPar;
	data_type	tVal = par->tInitPar;
	data_type	pVal = par->pInitPar;
	int			i;

	mAlphaLR = par->lrPar;
	mNoise	 = par->nPar;
	
	for ( i = 0; i < mTotalHidden; i++ )
	{
		mA[i] = aVal;
		mT[i] = tVal;
		mP[i] = pVal;
		aVal -= par->aStepPar;
		tVal -= par->tStepPar;
		pVal -= par->pStepPar;
	}

	for ( i = 0; i < mTotalHidden; i++ ) mHidden[i].SetParameters( mA[i], mP[i], mT[i], par->cPar );
	
	mCDLayer->SetParameters( par ); 
}


/*--------------------------------------*
 *		  PROPAGATION FUNCTIONS 		*
 *--------------------------------------*/

// for training only and propagates from Input to Hidden
void Network::LearnPropagate( void )
{
	// propagate from Input to Hidden:
	mInContext->SetOutput();
	for ( int i = 0; i < mTotalHidden; i++ ) mHidden[i].SetInput( mInput );
}


// This version of propagate is only called from within Recall functions
void Network::RecallPropagate( SInt16 direction )
{
	switch ( direction )
	{
			// Transfer with one-to-one mapping input to hidden layers
		case kInput2Hidden:
		{
			// update context layer first with activation from output context layer
			mInContext->Update( mOutContext );
			mInContext->SetOutput();
			for ( int i = 0; i < mTotalHidden; i++ ) mHidden[i].SetInput( mInput );
		}
			break;			
		case kHidden2Output: // not used: see Update()
			break;
			// Transfer with one-to-one mapping Output layer to Input layer
		case kOutput2Input:
			mInput->SetInput( mOutput );
			break;
	}
}


/*--------------------------------------*
 *		  ACT UPDATE FUNCTIONS 			*
 *--------------------------------------*/

// Update layers
void Network::Update( void )
{
	// Update hidden layers. 
	for ( int i = 0; i < mTotalHidden; i++ ) mHidden[i].Update();

	// Update output context layer. Gets input from input context layer
	mOutContext->Update( mHidden, mInContext, mTotalHidden );
	
	// Update output layer. Gets input from all hidden layers plus context layer
	mOutput->Update( mHidden, mInContext, mTotalHidden );
	
	// Update coincidence detector layer
	mCDLayer->Update( mOutput );	
}


/*--------------------------------------*
 *		  WEIGHT UPDATE FUNCTIONS 		*
 *--------------------------------------*/

// Update weights from hidden layers to output
// This step assumes that input layer is fed with the "next" pattern
data_type Network::Adaptation( data_type* target, data_type* ctxt ) 
{
	data_type	dw, err, total_err = 0;
	int			i, j, k;
	
	// coincidence detection modification:
	// if response from CD is positive, then learning should go down
	data_type	currentLR = mCDLayer->GetLR();

// adapt weights to output layer	
	for ( k = 0; k < mTotalHidden; k++ ) 
	{		
		mTotalDeltaWt[k] = 0.0;
		
		for ( i = 0; i < mLayerSize; i++ )
		{
			err = target[i] - mOutput->GetOutput(i);
			if ( err < 0 ) total_err -= err;
			else total_err += err;
				
			for ( j = 0; j < mLayerSize; j++ ) 
			{
				dw = currentLR * err * mHidden[k].GetOutput(j);
				dw += PseudoRNG( mNoise );
				mHidden[k].AdaptOutWt( j, i, dw );
			//	mTotalDeltaWt[k] += dw * dw;
				if ( j == 0 ) mTotalDeltaWt[k] += dw;
			}			
		}
	//	mTotalDeltaWt[k] = ( data_type )sqrt( mTotalDeltaWt[k] );
	}

	for ( i = 0; i < mLayerSize; i++ )
	{
		err = target[i] - mOutput->GetOutput(i);
		for ( j = 0; j < mCtxtSize; j++ ) 
		{
			dw = currentLR * err * mInContext->GetOutput(j);
			mInContext->AdaptOutWt( j, i, dw );
		}			
	}

// adapt weights to out-context layer
	for ( k = 0; k < mTotalHidden; k++ ) 
	{		
		mTotalDeltaWt[k] = 0.0;
		
		for ( i = 0; i < mCtxtSize; i++ )
		{
			err = ctxt[i] - mOutContext->GetOutput(i);
				
			for ( j = 0; j < mLayerSize; j++ ) 
			{
				dw = currentLR * err * mHidden[k].GetOutput(j);
			//	dw += PseudoRNG( mNoise );
				mHidden[k].AdaptCtxtWt( j, i, dw );
				if ( j == 0 ) mTotalDeltaWt[k] += dw;
			}			
		}
	}

	for ( i = 0; i < mCtxtSize; i++ )
	{
		err = ctxt[i] - mOutContext->GetOutput(i);
		for ( j = 0; j < mCtxtSize; j++ ) 
		{
			dw = currentLR * err * mInContext->GetOutput(j);
			mInContext->AdaptCtxtWt( j, i, dw );
		}			
	}

	// save changes in weights, but only if user wants us to
	Save( mTotalDeltaWt );
	
	return ( total_err / ( mLayerSize ) );
}


/*--------------------------------------*
 *		      RECALL FUNCTIONS 	   	    *
 *--------------------------------------*/

void Network::InitializeRecall( Patterns* patterns, int startPos, int initLen ) 
{
	// set the context
	SetContext( patterns->GetContext() );
		
	// present the pattern at the desired position in the sequence
	SetInput( patterns->GetPattern(startPos) );
	if ( initLen > 1 )
	{
		// propagate towards the hidden layer
		RecallPropagate( kInput2Hidden );
		// update activations throughout the net
		Update();
		// set the next pattern in the sequence
		if ( startPos == patterns->GetNumPatterns()-1 )
			startPos = 0;
		else
			startPos += 1;
		SetInput( patterns->GetPattern( startPos ) );
	}
}


/*--------------------------------------*
 *		     DISTANCE FUNCTIONS 	    *
 *--------------------------------------*/

data_type Network::QuadraticError( data_type *pat ) 
{
	data_type err = 0.0;
	data_type diff;

	for ( int i = 0; i < mLayerSize; i++ )
	{
		diff = pat[i] - mOutput->GetOutput(i);
		err += ( diff * diff );
	}
	err = err / (data_type)mLayerSize;
	
	return (data_type)sqrt( err );
}


/*--------------------------------------*
 *		   MISCELLANY FUNCTIONS 	    *
 *--------------------------------------*/

void Network::DataActivation( data_type* vec, data_type scale )
{
	int k = 0;
	for ( int i = 0; i < mTotalHidden; i++ )
		for ( int j = 0; j < mLayerSize; j++ )
		{
			vec[k] = scale * mHidden[i].GetOutput(j);
			k++;
		}
}
 
data_type Network::GetTotalAct( void )
{
	data_type tot_a = 0.0;
	
//	for ( int i = 0; i < mLayerSize; i++ )
//		tot_a += mOutput->GetOutput(i);

	for ( int k = 0; k < mTotalHidden; k++ ) 
		for ( int j = 0; j < mLayerSize; j++ ) 
			tot_a += mHidden[k].GetOutput(j);
	return tot_a;
}


// adds noise to weights in range [-x,x]
void Network::AddNoise( data_type noiseVal )
{
	for ( int k = 0; k < mTotalHidden; k++ )
	{
		for ( int i = 0; i < mLayerSize; i++ )
			for ( int j = 0; j < mLayerSize; j++ ) 
				mHidden[k].AdaptOutWt( i, j, PseudoRNG( noiseVal ) );
		for ( int i = 0; i < mCtxtSize; i++ )
			for ( int j = 0; j < mCtxtSize; j++ ) 
				mInContext->AdaptCtxtWt( j, i, PseudoRNG( noiseVal ) );
	}
}


void Network::DisplayActivation( ostream* os ) 
{
	*os << "printout of activations of hidden modules\n";
	for ( int i = 0; i < mTotalHidden; i++ )
		mHidden[i].Display( os );
}

