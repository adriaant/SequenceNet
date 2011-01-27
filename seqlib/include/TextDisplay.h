/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Generic class for data display. Use this to report any progress 
// of the current neural net. Derive appropriate subclasses, such 
// as GL based display, or plain text output. Also covers file saving

#ifndef __TEXTDISPLAY__
#define __TEXTDISPLAY__

#include <iostream>
using namespace std;
#include <math.h>

#include "SeqGlobal.h"
#include "Display.h"

enum 
{
	kReset		= 0x00
};


class TextDisplay : public Display
{

public:

	TextDisplay( Network* net, int dbg );
	~TextDisplay() {}

	int 	Print( ostream* os, int idx );
	int		Print( ostream* os, SInt16 infoType, int );
	int 	Print( ostream* os, SInt16 infoType, Patterns* patterns, int display );
	
	int		Save( int, int, void* ) { return 0; };
};

#endif
