
#ifndef __PHYSICSCONSTS_H__
#define __PHYSICSCONSTS_H__

namespace
{
	// exported types from maya
	const int B2DTYPE_BOX				=	0;
	const int B2DTYPE_CIRCLE			=	1;
	const int B2DTYPE_MESH				=	2;
	const int B2DTYPE_DISTANCEJOINT		=	3;
	const int B2DTYPE_GEARJOINT			=	4;
	const int B2DTYPE_MOUSEJOINT		=	5;
	const int B2DTYPE_PRISMATICJOINT	=	6;
	const int B2DTYPE_PULLEYJOINT		=	7;
	const int B2DTYPE_REVOLUTEJOINT		=	8;

	const int ODETYPE_BOX			=	0;
	const int ODETYPE_SPHERE		=	1;
	const int ODETYPE_CAPSULE		=	2;
	const int ODETYPE_CYLINDER		=	3;
	const int ODETYPE_TRIMESH		=	4;
	const int ODETYPE_CONVEX		=	5;
}

#endif // __PHYSICSCONSTS_H__