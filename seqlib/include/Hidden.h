/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// Hidden layer class. Derived from Layer class. 

#ifndef __HIDDEN__
#define __HIDDEN__

#include "Layer.h"
#include "Input.h"
#include "Weight.h"
#include "SAM_Unit.h"
#include "Rnd.h"

class Hidden : public Layer
{

public:

	Hidden(): Layer() {}
	~Hidden();
	
	void				Initialize( int );
	void				Initialize( int layerSize, int contextSize );
	void				Reset( SInt16 resetOption );	
	void				SetParameters( data_type, data_type, data_type, data_type );
	void				SetInput( Layer * );
	void				SetInput( data_type * );

	void 				Update();
	inline void 		AdaptOutWt( int i, int j, data_type dw ) { mOutWts[i][j].AdaptWeight( dw ); };
	inline void 		AdaptCtxtWt( int i, int j, data_type dw ) { mCtxtWts[i][j].AdaptWeight( dw ); };
	
	inline data_type	GetOutput( int idx ) { return mSAM_Unit[idx].GetOutput(); }
	inline data_type	GetHistory( int idx ) { return mSAM_Unit[idx].GetHistory(); }

	inline data_type	GetOutWt( int i, int j ) { return mOutWts[i][j].GetWeight(); }
	inline void			SetOutWt( int i, int j, data_type w ) { mOutWts[i][j].SetWeight(w); }
	inline data_type	GetCtxtWt( int i, int j ) { return mCtxtWts[i][j].GetWeight(); }
	inline void			SetCtxtWt( int i, int j, data_type w ) { mCtxtWts[i][j].SetWeight(w); }

	void				SaveWeights( char* filename, int idx );
	int					LoadWeights( char* filename, int idx );

	inline data_type	GetWeight( int, int ) { cerr << "Illegal function use: Hidden::GetWeight()" << endl; return 0.0; }
	
	void				Display( ostream* os );
	void				Display( ostream* os, int );
	
	void				Data( data_type*, data_type scale );
	void				Data( data_type**, int ident, data_type scale );
	
private:

	int			mContextSize;
	SAM_Unit	*mSAM_Unit;
	Weight		**mOutWts;
	Weight		**mCtxtWts;
};

#endif
