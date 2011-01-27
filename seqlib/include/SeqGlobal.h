/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

#ifndef __SEQGLOBAL__
#define __SEQGLOBAL__

#include <iostream>
using namespace std;
#include	<string.h>


// define the data type to use for calculations. Note that most compilers
// will implicitly convert float to double. If you use float, you will need
// to explicitly suppress these automatic conversions or revert to using
// double. As I prefer double precision, I'll define double as the datatype to use

#define		data_type		float

// already defined on MacOS, so will be defined as zero on other platforms
#ifndef	TARGET_OS_MAC
	#define 	TARGET_OS_MAC   0
#endif

#if !TARGET_OS_MAC
	#define         SInt16                  int
	#define         SInt32                  long
	#define         nil                     NULL
#endif

// Error codes
enum
{
	kNoErr = 0,
	kSequenceFileError = 1
};

// pattern presentation
enum
{
	kLinearOrder = 0,
	kPermutedOrder = 1
};

// miscellany
enum
{
	kLogDir = 0,
	kOriginalDir = 1,
	kOffline = 2,
	kOnline = 3
};


// Reset Options
enum 
{
	O_ACT	= 0x01,		// reset activations
	O_WT	= 0x02,		// reset weights
	O_CD	= 0x04		// reset CD layer
};

// Sequence types (in Pattern class)
enum
{
	O_INF			= 0,	// cyclic
	O_END			= 1,	// last pattern points to last pattern
	O_NOISE			= 2		// last pattern points to noise
};

// Switch types
enum
{
	kOff			= 0,		// turn off
	kOn				= 1,		// turn on
	kFix			= 2			// third option: e.g. no learn in context
};

// Intelligible macro to pass to the SpeedTest function. This calculates
// the amount of time a simulation took between, of course, kStart and kEnd
enum
{
	kStart = 0,
	kEnd = 1
};

// Identifiers
enum
{
	kModuleHidden = 0,
	kModuleOutput,
	kModuleCD,
	kModuleOutContext,
	
	kParLr,				// learning rate/alpha
	kParAi,				// max leaky integration 
	kParAs,				// leaky integration decrement
	kParPi,				// max subtraction
	kParPs,				// subtraction decrement
	kParTi,				// max threshold
	kParTs,				// threshold decrement
	kParC,				// sigmoid constant
	kParN,				// synaptic noise value
	kParCd,				// coincidence detector threshold
	kParCDi,			// initial time delay CD
	kParCDs,			// time delay increment
	kParB,				// base rate learning
	kParL,				// learning rate change
	kParM,				// learning rate momentum
	kParCDwi,			// initial weight value CD to output
	kParCDws,			// increment weight value CD to output
};

// parameters
typedef struct _par_Block
{
	data_type	lrPar;			// learning rate
	data_type	aInitPar;		// maximum leaky integration
	data_type	aStepPar;		// leaky integration decrement
	data_type	pInitPar;		// maximum subtraction constant
	data_type	pStepPar;		// subtraction constant decrement
	data_type	tInitPar;		// maximum threshold
	data_type	tStepPar;		// threshold decrement
	data_type	cPar;			// sigmoid
	data_type	nPar;			// synaptic noise
	data_type	cdTPar;			// threshold coincidence detector
	data_type	cdDInitPar;		// initial delay coincidence detector
	data_type	cdDStepPar;		// delay increment coincidence detector
	data_type	bPar;			// base rate learning
	data_type	lPar;			// cd de/increment value: lambda
	data_type	mPar;			// cd momentum
	data_type	cdWtInitPar;	// start weight coincidence detector
	data_type	cdWtStepPar;	// step weight coincidence detector
}
ParBlock, *ParBlockPtr;

// For cout
enum
{
	kTab,
	kReturn,
	kIntend,
	kSpace
};

// Verbosity Options
enum 
{
	O_NONE 			= 0x00,
	O_OUTPUT 		= 0x01,		// output as is
	O_OUTPUTPLUS 	= 0x02,		// formatted output
	O_ERROR 		= 0x04,		// detailed learning progress
	O_CDOUT			= 0x08,		// coincidence detection output
	O_MULTI			= 0x10,		// show detailed progress in multi sequence learning
	O_SAVEDELTA 	= 0x20,		// save changes in weights to file
	O_SAVEORB 		= 0x40		// save total activation over time to file
};


#endif

