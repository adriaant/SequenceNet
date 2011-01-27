/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	Abstract class definition for multiple sequence learning
*/


#ifndef __MULTISEQ__
#define __MULTISEQ__

#include <stdio.h>
#include "SeqGlobal.h"
#include "Utilities.h"
#include "Sequence.h"		// the interface file to the Seq API Library

// stats for batch training, applies to single epoch
typedef struct _batch_stats
{
	int				sTrainCounter;		// counter for number of training sessions
	int				sRecallCounter;		// counter for number of recalls
	data_type		sIteAverage;		// average training iterations
	data_type		sErrAverage;		// average recall error
	data_type		sLRAverage;			// average learning rate values
}
StatSpec, *StatSpecPtr;


class MultiSequence
{
public:

	MultiSequence();
	~MultiSequence();
	
	bool 		LoadTestFiles( void );
	bool		RunMultiSequenceSimulation( void );
	void		TrainCurrentSequence( int num_ite, int* final_ite );
	void		RecallSequences( int num_ite, data_type* recall_err );
	int			SequenceErrors( int no_reset );
	void		ClearStats( int num );
	void		PrintStats( int patIdx, int epoch );
	void 		TestRecall( int cur_run );
	
	void		SetPatternsFile( char* filename );
	void		SetContextsFile( char* filename );
	void		SetCuesFile( char* filename );
	

protected:

	char			mPatternsFile[128];	// name for file with all patterns used
	char			mContextsFile[128];	// name for file with contexts for testing
	char			mCuesFile[128];		// name for file with pattern cues for recall
	int				mNumPatterns;		// number of patterns in patternsfile
	char*			mPatLabels;			// labels for patterns
	data_type**		mPatterns;			// data storage for patterns from patterns file
	int				mTotalContexts;		// number of contexts in contexts file
	int*			mNumContexts;		// number of contexts for each cue
	char*			mContextLabels;		// labels for contexts
	data_type**		mContexts;			// data storage for contexts from contexts file
	int				mNumCues;			// number of cues in cues file
	char*			mCueLabels;			// labels for cues
	data_type**		mCues;				// data storage for patterns from cues file
	StatSpec*		mStats;				// for tracking stats
};

#endif
