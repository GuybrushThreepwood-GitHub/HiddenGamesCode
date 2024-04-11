
#ifndef __MODELRESOURCES_H__
#define __MODELRESOURCES_H__

// forward declare

namespace res
{
	// model info store
	struct ModelResourceStore
	{
		const char* modelName;
		const char* textureAnimFile;
		GLenum magFilter;
		GLenum minFilter;
		int textureStartIndex;
		int textureCount;
		bool hasDamageTexture;
	};

	void CreateResourceMap();

	void ClearModelMap();

	const ModelResourceStore* GetModelResource( int index );

	mdl::ModelHGM* LoadModel( const char* model, GLenum magFilter, GLenum minFilter, bool allowSmoothing=false );

	mdl::ModelHGM* LoadModel( int index, bool allowSmoothing=false );

	void RemoveReference( mdl::ModelHGM* pModel );

	void RemoveModel( mdl::ModelHGM* pModel=0 );
}

#endif // __MODELRESOURCES_H__

