/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// OutContext layer class. Derived from Layer class
#ifndef __OUTCTXT__
#define __OUTCTXT__

#include "InContext.h"
#include "Hidden.h"
#include "Weight.h"
#include "Unit.h"
#include "InContext.h"

class OutContext : public Layer
{

public:

	OutContext(): Layer() {}
	OutContext( int layerSize );
	~OutContext();
	
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
