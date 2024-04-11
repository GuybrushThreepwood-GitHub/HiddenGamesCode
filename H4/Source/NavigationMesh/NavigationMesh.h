/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef __NAVIGATIONMESH_H__
#define __NAVIGATIONMESH_H__

//#ifndef _MTXLIB_H
//	#include "mtxlib.h"
//#endif

#ifndef __NAVIGATIONCELL_H__
	#include "NavigationCell.h"
#endif

#ifndef __NAVIGATIONHEAP_H__
	#include "NavigationHeap.h"
#endif

#ifndef STD_VECTOR_H
#define STD_VECTOR_H
	#pragma warning(disable : 4786)
	#include <vector>
#endif

// forward declaration required
class NavigationPath;

/*	NavigationMesh
------------------------------------------------------------------------------------------
	
	A NavigationMesh is a collecion of NavigationCells used to control object movement while
	also providing path finding line-of-sight testing. It serves as a parent to all the
	Actor objects which exist upon it.
	
------------------------------------------------------------------------------------------
*/
class NavigationMesh
{
	public:
		typedef	std::vector<NavigationCell*> CELL_ARRAY;

		NavigationMesh();
		~NavigationMesh();

		void CreateFromFile( const char* filename );
		void Clear();
		void AddCell(const math::Vec3& PointA, const math::Vec3& PointB, const math::Vec3& PointC);
		void LinkCells();

		math::Vec3 SnapPointToCell(NavigationCell* Cell, const math::Vec3& Point);
		math::Vec3 SnapPointToMesh(NavigationCell** CellOut, const math::Vec3& Point);
		NavigationCell* FindClosestCell(const math::Vec3& Point)const;

		bool LineOfSightTest(NavigationCell* StartCell, const math::Vec3& StartPos, NavigationCell* EndCell, const math::Vec3& EndPos);
		bool BuildNavigationPath(NavigationPath& NavPath, NavigationCell* StartCell, const math::Vec3& StartPos, NavigationCell* EndCell, const math::Vec3& EndPos);

		void ResolveMotionOnMesh(const math::Vec3& StartPos, NavigationCell* StartCell, math::Vec3& EndPos, NavigationCell** EndCell);

		void Update(float elapsedTime = 1.0f);
		void Render();

		int TotalCells()const;
		NavigationCell* Cell(int index);

	private:
		CELL_ARRAY m_CellArray; // the cells that make up this mesh

		// path finding data...
		int m_PathSession;
		NavigationHeap m_NavHeap;

		NavigationMesh( const NavigationMesh& Src);
		NavigationMesh& operator=( const NavigationMesh& Src);

};

//- Inline Functions ---------------------------------------------------------------------

/*	NavigationMesh
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationMesh::NavigationMesh()
:m_PathSession(0)
{
	m_CellArray.clear();
}

/*	~NavigationMesh
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationMesh::~NavigationMesh()
{
	Clear();
}

//= OPERATORS ============================================================================

//= MUTATORS =============================================================================

//:	Clear
//----------------------------------------------------------------------------------------
//
//	Delete all cells associated with this mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Clear()
{
	CELL_ARRAY::iterator	CellIter = m_CellArray.begin();
	for(;CellIter != m_CellArray.end(); ++CellIter)
	{
		NavigationCell* Cell = *CellIter;
		delete Cell;
	}

	m_CellArray.clear();
}

//:	AddCell
//----------------------------------------------------------------------------------------
//
//	Add a new cell, defined by the three vertices in clockwise order, to this mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::AddCell(const math::Vec3& PointA, const math::Vec3& PointB, const math::Vec3& PointC)
{
	NavigationCell* NewCell = new NavigationCell;
	NewCell->Initialize(PointA, PointB, PointC);
	m_CellArray.push_back(NewCell);
}

//:	Render
//----------------------------------------------------------------------------------------
//
//	Render the mesh geometry to screen. The mesh is assumed to exist in world corrdinates 
//	for the purpose of this demo 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Render()
{
#ifdef BASE_PLATFORM_WINDOWS
    glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	// render each cell triangle
	CELL_ARRAY::const_iterator	iter = m_CellArray.begin();
	for(;iter != m_CellArray.end(); ++iter)
	{
		const NavigationCell* Cell = *iter;
		int i;

		for (i=0;i<3;++i)
		{
			char shade = (char)Cell->Vertex(i).Y;
			shade = 128+shade;

			renderer::OpenGL::GetInstance()->SetColour4ub(shade, shade, shade, 0xff);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(Cell->Vertex(i).X, Cell->Vertex(i).Y, Cell->Vertex(i).Z);
		}
	}

	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);

	// render cell edges as wireframe for added visibility
	iter = m_CellArray.begin();
	for(;iter != m_CellArray.end(); ++iter)
	{
		const NavigationCell* Cell = *iter;
		int i;

		for (i=0;i<3;++i)
		{
			char shade = (char)Cell->Vertex(i).Y;
			shade = 64+shade;

			renderer::OpenGL::GetInstance()->SetColour4ub(shade, shade, shade, 0x80);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(Cell->Vertex(i).X, Cell->Vertex(i).Y + 0.2f, Cell->Vertex(i).Z);
		}
	}

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
	glPopMatrix();
#endif // BASE_PLATFORM_WINDOWS
}

//:	Update
//----------------------------------------------------------------------------------------
//
//	Does noting at this point. Stubbed for future use in animating the mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Update(float elapsedTime)
{
}

inline int NavigationMesh::TotalCells()const
{
	return((int)m_CellArray.size());
}

inline NavigationCell* NavigationMesh::Cell(int index)
{
	return(m_CellArray.at(index));
}

#endif  // __NAVIGATIONMESH_H__

