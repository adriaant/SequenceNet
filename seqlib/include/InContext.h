/*
	Author:			Adriaan Tijsseling (AGT)
	Copyright: 		(c) Copyright 2002-3 Adriaan Tijsseling. All rights reserved.
	Description:	API interface for Sequence Learning Network
*/

// InContext layer class. Derived from Layer class
#ifndef __INCTXT__
#define __INCTXT__

#include "Layer.h"
#include "ContextUnit.h"

class InContext : public Layer
{

public:

	InContext(): Layer() {}
	InContext( int ctxtSize, int layerSize );
	~InContext();
	
	void				Initialize( int );
	void				Reset( SInt16 resetOption );
	void 				Update( Layer* layer );

	void 				SetInput( Layer * );
	void				SetInput( data_type * );	
	void				SetInput( data_type );	

	void				SetOutput( void );	
	inline data_type	GetOutput( int idx ) { return mUnit[idx].GetOutput(); }	

	inline void			SetOutWeight( int i, int j, data_type w ) { mOutWts[i][j] = w; }
	inline data_type	GetOutWeight( int i, int j ) { return mOutWts[i][j]; }
	inline void 		AdaptOutWt( int i, int j, data_type dw ) { mOutWts[i][j] += dw; };

	inline void			SetCtxtWeight( int i, int j, data_type w ) { mCtxtWts[i][j] = w; }
	inline data_type	GetCtxtWeight( int i, int j ) { return mCtxtWts[i][j]; }
	inline void 		AdaptCtxtWt( int i, int j, data_type dw ) { mCtxtWts[i][j] += dw; };

	int					LoadContext( char* filename );
	void				SaveContext( char* filename );
	
	void				Display( ostream* os );

	inline void			Data( data_type*, data_type scale ) { cerr << "Illegal function use: InContext::Data()" << endl; }
	void				Data( data_type**, int ident, data_type scale );

	inline int			GetLayerSize( void ) { return mContextSize; }
	
private:

	ContextUnit		*mUnit;
	data_type		**mOutWts;
	data_type		**mCtxtWts;
	int				mContextSize;
};

#endif
