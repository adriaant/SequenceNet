/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Generic class for data display. Use this to report any progress 
// of the current neural net. Derive appropriate subclasses, such 
// as GL based display, or plain text output. Also covers file saving

#ifndef __DISPLAY__
#define __DISPLAY__

#include <iostream>
using namespace std;
#include <math.h>

#include "SeqGlobal.h"
#include "Patterns.h"
#include "Network.h"


// constants to define types of info to display
enum 
{ 
	kDisplayWeight = 0x01, 
	kDisplayOutput = 0x02, 
	kDisplayError  = 0x04 
};


class Display 
{

public:

	Display() { mDisplayLevel = 1; }
	~Display() {}

	virtual int		Print( ostream* os, SInt16 infoType, int timeStamp ) = 0;
	virtual int 	Print( ostream* os, SInt16 infoType, Patterns* patterns, int display ) = 0;

	virtual	int		Save( int saveType, int counter, void* saveData ) = 0;
	
	inline	void	SetDisplayLevel( SInt16 display ) { mDisplayLevel = display; }
	
protected:

	SInt16		mDisplayLevel;			// Level of detail of progress
	Network*	mNetwork;
};

#endif
