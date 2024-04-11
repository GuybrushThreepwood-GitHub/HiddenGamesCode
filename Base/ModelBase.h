

#ifndef __MODELBASE_H__
#define __MODELBASE_H__

#ifndef __MODELCOMMON_H__
	#include "Model/ModelCommon.h"
#endif // __MODELCOMMON_H__

#ifndef __MODEL_H__
	#include "Model/Model.h"
#endif // __MODEL_H__

#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
	#ifndef __MESH_H__
		#include "Model/FF/Mesh.h"
	#endif // __MESH_H__

	#ifndef __SKINMESH_H__
		#include "Model/FF/SkinMesh.h"
	#endif // __SKINMESH_H__

	#ifndef __MODELHGM_H__
		#include "Model/FF/ModelHGM.h"
	#endif // __MODELHGM_H__

	#ifndef __MODELHGA_H__
		#include "Model/FF/ModelHGA.h"
	#endif // __MODELHGA_H__
#endif // (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#ifdef BASE_SUPPORT_OPENGL_GLSL
	#ifndef __MESHGLSL_H__
		#include "Model/GLSL/MeshGLSL.h"
	#endif // __MESH_H__

	#ifndef __SKINMESHGLSL_H__
		#include "Model/GLSL/SkinMeshGLSL.h"
	#endif // __SKINMESH_H__

	#ifndef __MODELHGMGLSL_H__
		#include "Model/GLSL/ModelHGMGLSL.h"
	#endif // __MODELHGM_H__

	#ifndef __MODELHGAGLSL_H__
		#include "Model/GLSL/ModelHGAGLSL.h"
	#endif // __MODELHGA_H__
#endif // (BASE_SUPPORT_OPENGL_GLSL)

#endif // __MODELBASE_H__
