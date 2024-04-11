
#ifndef __PHYSICSBASE_H__
#define __PHYSICSBASE_H__

#ifndef __DEBUGBASE_H__
	#include "DebugBase.h"
#endif // __DEBUGBASE_H__

#ifndef __PHYSICSCONSTS_H__
	#include "Physics/PhysicsConsts.h"
#endif // __PHYSICSCONSTS_H__

#ifndef __PHYSICSIDENTIFIER_H__
	#include "Physics/PhysicsIdentifier.h"
#endif // __PHYSICSIDENTIFIER_H__

/// BOX2D SUPPORT
#ifdef BASE_SUPPORT_BOX2D
	#ifndef BOX2D_H
		#include <box2d.h>
	#endif // BOX2D_H

	#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
	#ifndef __BOX2DDEBUGDRAW_H__
		#include "Physics/box2d/FF/Box2DDebugDraw.h"
	#endif // __BOX2DDEBUGDRAW_H__
	#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

	#ifdef BASE_SUPPORT_OPENGL_GLSL
	#ifndef __BOX2DDEBUGDRAWGLSL_H__
		#include "Physics/box2d/GLSL/Box2DDebugDrawGLSL.h"
	#endif // __BOX2DDEBUGDRAWGLSL_H__
	#endif // BASE_SUPPORT_OPENGL_GLSL

	#ifndef __PHYSICSWORLDB2D_H__
		#include "Physics/box2d/PhysicsWorldB2D.h"
	#endif // __PHYSICSWORLDB2D_H__
#endif // BASE_SUPPORT_BOX2D

/// ODE SUPPORT
#ifdef BASE_SUPPORT_ODE
	#ifndef _ODE_ODE_H_
		#include <ode/ode.h>
	#endif // _ODE_ODE_H_

	#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
	#ifndef __ODEPHYSICSPRIMITIVES_H__
		#include "Physics/ode/FF/ODEPhysicsPrimitives.h"
	#endif // __ODEPHYSICSPRIMITIVES_H__
	#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

	#ifdef BASE_SUPPORT_OPENGL_GLSL
	#ifndef __ODEPHYSICSPRIMITIVESGLSL_H__
		#include "Physics/ode/GLSL/ODEPhysicsPrimitivesGLSL.h"
	#endif // __ODEPHYSICSPRIMITIVESGLSL_H__
	#endif // BASE_SUPPORT_OPENGL_GLSL

	#ifndef __PHYSICSWORLDODE_H__
		#include "Physics/ode/PhysicsWorldODE.h"
	#endif // __PHYSICSWORLDODE_H__
#endif // BASE_SUPPORT_ODE

/// BULLET SUPPORT
#ifdef BASE_SUPPORT_BULLET
	#ifdef _DEBUG
		// bullet overloads the new operator
		#undef new
	#endif // _DEBUG

	#ifndef BULLET_DYNAMICS_COMMON_H
		#include "btBulletDynamicsCommon.h"
	#endif // BULLET_DYNAMICS_COMMON_H

#endif // BASE_SUPPORT_BULLET

#endif // __PHYSICSBASE_H__