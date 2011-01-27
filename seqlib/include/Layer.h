/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// This is the abstract class for any layer of neurons. 
// Generic functions are defined here, 
// specific layers need to be created
// by subclassing from this layer. 


#ifndef __LAYER__
#define __LAYER__

#include <iostream>
using namespace std;
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "SeqGlobal.h"
#include "Utilities.h"


class Layer
{

public:

	Layer() { mLayerSize = 0; }
	Layer( int newLayerSize ) { mLayerSize = newLayerSize; }
	~Layer() {}
	
	virtual void 		Initialize( int ) = 0;
	virtual void		Reset( SInt16 ) = 0;
	virtual void 		SetInput( Layer * ) = 0;
	virtual void 		SetInput( data_type * ) = 0;
	virtual data_type	GetOutput( int idx ) = 0;
	virtual void 		Display( ostream* os ) = 0;
	virtual void		Data( data_type*, data_type scale ) = 0;
	
	inline int			GetLayerSize( void ) { return mLayerSize; }

protected:

	int mLayerSize;
};

#endif
