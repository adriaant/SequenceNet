#include "TextDisplay.h"


// create the deltaweight files and store a reference to the network object
TextDisplay::TextDisplay( Network* net, int dbg )
{
	mNetwork = net;
	mDisplayLevel = dbg;
}


// Prints time stamp
int TextDisplay::Print( ostream* os, int idx )
{
	AdjustStream( *os, 0, 4, kLeft, true );
	*os << idx+1 << ":";
	SetStreamDefaults( *os );
	return -1;
}


// Output display in a formatted structure
int	TextDisplay::Print( ostream* os, SInt16 infoType, int )
{
	int i;
	
	if ( infoType & O_OUTPUT )
	{
		*os << "  ";
		AdjustStream( *os, 3, 6, kLeft, true );
		for ( i = 0; i < mNetwork->mLayerSize; i++ )
			*os << mNetwork->mOutput->GetOutput(i) << " ";
		SetStreamDefaults( *os );
	}
	if ( infoType & O_OUTPUTPLUS )
	{
		*os << "  ";
		for ( i = 0; i < mNetwork->mLayerSize; i++ ) 
		{
			if ( mNetwork->mOutput->GetOutput(i) < 0.4 )
				*os << "0";
			else if ( mNetwork->mOutput->GetOutput(i) > 0.6 )
				*os << "1";
			else
				*os << "-";
		}
		*os << "  ";
		for ( i = 0; i < mNetwork->mCtxtSize; i++ ) 
		{
			if ( mNetwork->mOutContext->GetOutput(i) < 0.4 )
				*os << "0";
			else if ( mNetwork->mOutContext->GetOutput(i) > 0.6 )
				*os << "1";
			else
				*os << "-";
		}
	}
	if ( infoType & O_ERROR )		// display learning progress
	{
		*os << "  ";
		AdjustStream( *os, 2, 6, kLeft, true );
		*os << mNetwork->mCDLayer->GetLR();
		*os << "  ";
		AdjustStream( *os, 2, 5, kLeft, true );
		*os << mNetwork->GetTotalAct();		
		SetStreamDefaults( *os );
	}
	if ( infoType & O_CD )		// display coincidence layer output
	{
		*os << "  ";
		SetStreamDefaults( *os );
		for ( i = 0; i < mNetwork->mLayerSize; i++ ) 
			*os << mNetwork->mCDLayer->GetOutput(i);
	}
	return -1;
}


// display recall frequency distribution
int TextDisplay::Print( ostream* os, SInt16 infoType, Patterns* patterns, int display )
{
	static int	first_state = 0;
	static int	prev_state = 0;
	
	if ( infoType == kReset )
	{
		first_state = 0;
		prev_state = 0;
		return 0;
	}
		
	int	idx = 0;	
	int	numpat = patterns->GetNumPatterns();
	
	for ( int i = 0; i < numpat; i++ ) 
	{
		patterns->SetErrs( i, mNetwork->QuadraticError( patterns->GetPattern(i) ) );
		if ( patterns->GetErrs(i) <= patterns->GetErrs(idx) ) 
			idx = i;
	}

	if ( patterns->GetErrs(idx) < 0.4 ) 
	{
		prev_state = idx;
		if ( display )
		{
			AdjustStream( *os, 0, 3, kLeft, true );
			*os << idx+1 << " ";
			AdjustStream( *os, 0, 5, kLeft, true );
			*os << patterns->GetErrs(idx);
			SetStreamDefaults( *os );
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
				AdjustStream( *os, 0, 3, kLeft, true );
				*os << prev_state+1 << " ";
				AdjustStream( *os, 0, 5, kLeft, true );
				*os << patterns->GetErrs(idx);
				SetStreamDefaults( *os );
			}
			return prev_state;
		}
		else
		{
			if ( display ) 
			{
				AdjustStream( *os, 0, 3, kLeft, true );
				*os << -1 << " " << 100.0;
				SetStreamDefaults( *os );
			}
			return -1;
		}
	}

	return idx;
}

