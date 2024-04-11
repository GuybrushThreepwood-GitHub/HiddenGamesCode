
#ifndef __MODELRESOURCES_H__
#define __MODELRESOURCES_H__

namespace res
{
	void CreateResourceMap();

	void ClearModelMap();

	mdl::ModelHGM* LoadModel( const char* model, GLenum magFilter, GLenum minFilter, bool allowSmoothing=false );

	mdl::ModelHGM* LoadModel( int index, bool allowSmoothing=false );

	void RemoveReference( mdl::ModelHGM* pModel );

	void RemoveModel( mdl::ModelHGM* pModel=0 );
}

#endif // __MODELRESOURCES_H__

