/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Output layer class. Derived from Layer class
#ifndef __OUTPUT__
#define __OUTPUT__

#include "Hidden.h"
#include "Weight.h"
#include "Unit.h"
#include "InContext.h"

class Output : public Layer
{

public:

	Output(): Layer() {}
	Output( int layerSize );
	~Output();
	
	void				Initialize( int );
	void				Reset( SInt16 resetOption );
	void 				SetInput( Layer * );
	void				SetInput( data_type * );	
	void 				Update( Hidden* layers, InContext* ctxLayer, int numLayers );
	void				Display( ostream* os );
	void				Data( data_type*, data_type scale );
		
	inline data_type	GetOutput( int idx ) { return mUnit[idx].GetOutput(); }
	
private:

	Unit *mUnit;
};

#endif
