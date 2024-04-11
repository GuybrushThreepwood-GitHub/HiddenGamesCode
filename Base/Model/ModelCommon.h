
#ifndef __MODELCOMMON_H__
#define __MODELCOMMON_H__

// forward declare
namespace math { class Vec3; }
namespace math { class Vec4; }
namespace math { class Quaternion; }
namespace mdl { class Mesh; }
namespace renderer { enum ETextureFormat; }

namespace mdl
{
	namespace transform
	{
		struct TransformBlock
		{
			math::Vec3 translate;
			math::Vec3 eulerRotation;
			math::Quaternion quatRotation;
			math::Vec3 scale;
		};

	} // namespace transform

	namespace mat
	{
		enum EMaterialType
		{
			MATTYPE_UNKNOWN=0,
			MATTYPE_LAMBERT=1,
			MATTYPE_BLINN=2,
			MATTYPE_PHONG=3,

			MATTYPE_SURFACESHADER=4,

			MATTYPE_UNSUPPORTED=9999
		};

		enum EEnvmapType
		{
			ENVMAP_UNKNOWN=0,
			ENVMAP_SPHERE=1,
			ENVMAP_CUBE=2,

			ENVMAP_UNSUPPORTED=9999,
		};

		struct EnvmapBlock
		{
			bool hasEnvMap;
			EEnvmapType envType;
			int envIdx[6];
		};

		struct BumpmapBlock
		{
			bool hasBump;
			float bumpDepth;
			int bumpIdx;
		};

		struct ReflectShaderBlock
		{
			math::Vec4 specularColour;
			int specularColourIdx;
			float reflectivity;
			math::Vec4 reflectedColour;
			EnvmapBlock envmapBlock;
		};

		struct LambertShaderBlock
		{
			float diffuseCoeff;
			math::Vec4 colour;
			int colourIdx;
			math::Vec4 transparency; 
			int transparencyIdx;
			math::Vec4 ambientColour;
			int ambientColourIdx;
			math::Vec4 incandescence;
			int incandescenceIdx;
			float translucenceCoeff;
			float glowIntensity;

			BumpmapBlock bump;
		};

		struct PhongShaderBlock
		{
			// lambert
			LambertShaderBlock lambert;

			// reflect shader inherit
			ReflectShaderBlock reflect;

			// phong
			float cosPower;
		};

		struct BlinnShaderBlock
		{
			// lambert
			LambertShaderBlock lambert;

			// reflect shader inherit
			ReflectShaderBlock reflect;

			// blinn
			float eccentricity;
			float specularRollOff; 
		};

	} // namespace mat

	struct TextureHeader
	{
		unsigned int nTextureCount;
		unsigned int nInternal;

		unsigned int nFilesStart;
		unsigned int nFilesEnd;

		unsigned int nTotalBlockSize;
	};

	struct TextureFormat
	{
		renderer::ETextureFormat eTexFormat;
	};

	struct MaterialHeader
	{
		unsigned int nMaterialCount;

		unsigned int nMaterialsStart;
		unsigned int nMaterialsEnd;

		unsigned int nTotalBlockSize;
	};

	struct MaterialFormat
	{
		mdl::mat::EMaterialType eMatFormat;
	};

	// FIXME: this is a total waste of memory
	struct Material
	{
		mdl::mat::EMaterialType eMatFormat;
		mdl::mat::LambertShaderBlock lambertBlock;
		mdl::mat::BlinnShaderBlock blinnBlock;
		mdl::mat::PhongShaderBlock phongBlock;
	};

	struct InterleavedData
	{
		float v[3];		//	12 bytes
		float uv[2];	//	8 bytes
		float n[3];		//	12 bytes
		// ------------------------
		// total			32 bytes
		float t[4];		//  16 bytes
		// total			48 bytes
		float pad[4];
	}; 

	struct TextureAnimationBlock
	{
		struct MeshData
		{
			int meshId;
			mdl::Mesh* pMesh;
		};

		struct AnimationData
		{
			float time;
			int textureLookup;
			GLuint textureId;
			GLuint originalTextureId;
		};

		int numSubmeshIds;
		MeshData* pSubMeshList;

		int numAnimationFrames;
		AnimationData* animationData;
		float animationSwapTime;
	};
}

#endif // __MODELCOMMON_H__

