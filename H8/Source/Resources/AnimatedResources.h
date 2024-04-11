
#ifndef __ANIMATEDRESOURCES_H__
#define __ANIMATEDRESOURCES_H__

namespace res
{
	void CreateAnimatedResourceMap();

	void ClearAnimatedModelMap();

	mdl::ModelHGA* LoadAnimatedModel( int index, bool allowSmoothing=false );

	void RemoveAnimatedReference( mdl::ModelHGA* pModel );

	void RemoveAnimatedModel( mdl::ModelHGA* pModel=0 );
}

#endif // __ANIMATEDRESOURCES_H__

