
/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */

#include "CoreBase.h"
#include "MathBase.h"
#include <cmath>
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"

#include "NavigationMesh.h"
#include "NavigationPath.h"

void NavigationMesh::CreateFromFile( const char* filename )
{
	unsigned int i=0;

	Clear();
	
	mdl::ModelHGM hgm;
	hgm.Initialise();
	if( hgm.Load( filename ) )
		return;

	for (i=0;i<hgm.GetOpaqueMeshListPtr()[0].nNumVertices;i+=3)
	{
		math::Vec3 posA = math::Vec3( hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i].v[0], hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i].v[1], hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i].v[2] );
		math::Vec3 posB = math::Vec3( hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i+1].v[0], hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i+1].v[1], hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i+1].v[2] );
		math::Vec3 posC = math::Vec3( hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i+2].v[0], hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i+2].v[1], hgm.GetOpaqueMeshListPtr()[0].pInterleavedData[i+2].v[2] );

		AddCell( posA, posB, posC );
	}

	LinkCells();

	hgm.Release();
}

//:	SnapPointToCell
//----------------------------------------------------------------------------------------
//
// Force a point to be inside the cell
//
//-------------------------------------------------------------------------------------://
math::Vec3 NavigationMesh::SnapPointToCell(NavigationCell* Cell, const math::Vec3& Point)
{
	math::Vec3 PointOut = Point;

	if (!Cell->IsPointInCellCollumn(PointOut))
	{
		Cell->ForcePointToCellCollumn(PointOut);
	}

	Cell->MapVectorHeightToCell(PointOut);
	return (PointOut);
}

//:	SnapPointToMesh
//----------------------------------------------------------------------------------------
//
// Force a point to be inside the nearest cell on the mesh
//
//-------------------------------------------------------------------------------------://
math::Vec3 NavigationMesh::SnapPointToMesh(NavigationCell** CellOut, const math::Vec3& Point)
{
	math::Vec3 PointOut = Point;

	*CellOut = FindClosestCell(PointOut);

	return (SnapPointToCell(*CellOut, PointOut));
}

//:	FindClosestCell
//----------------------------------------------------------------------------------------
//
// Find the closest cell on the mesh to the given point
//
//-------------------------------------------------------------------------------------://
NavigationCell* NavigationMesh::FindClosestCell(const math::Vec3& Point)const
{
	float ClosestDistance = 3.4E+38f;
	float ClosestHeight = 3.4E+38f;
	bool FoundHomeCell = false;
	float ThisDistance;
	NavigationCell* ClosestCell=0;

	CELL_ARRAY::const_iterator	CellIter = m_CellArray.begin();
	for(;CellIter != m_CellArray.end(); ++CellIter)
	{
		NavigationCell* pCell = *CellIter;

		if (pCell->IsPointInCellCollumn(Point))
		{
			math::Vec3 NewPosition(Point);
			pCell->MapVectorHeightToCell(NewPosition);

			ThisDistance = std::fabs(NewPosition.Y - Point.Y);

			if (FoundHomeCell)
			{
				if (ThisDistance < ClosestHeight)
				{
					ClosestCell = pCell;
					ClosestHeight = ThisDistance;
				}
			}
			else
			{
				ClosestCell = pCell;
				ClosestHeight = ThisDistance;
				FoundHomeCell = true;
			}
		}

		if (!FoundHomeCell)
		{
			math::Vec2 Start(pCell->CenterPoint().X, pCell->CenterPoint().Z);
			math::Vec2 End(Point.X, Point.Z);
			Line2D MotionPath(Start, End);
			NavigationCell* NextCell;
			NavigationCell::CELL_SIDE WallHit;
			math::Vec2 PointOfIntersection;

			NavigationCell::PATH_RESULT Result = pCell->ClassifyPathToCell(MotionPath, &NextCell, WallHit, &PointOfIntersection);

			if (Result == NavigationCell::EXITING_CELL)
			{
				math::Vec3 ClosestPoint3D(PointOfIntersection.X,0.0f,PointOfIntersection.Y);
				pCell->MapVectorHeightToCell(ClosestPoint3D);

				ClosestPoint3D -= Point;

				ThisDistance = ClosestPoint3D.length();

				if (ThisDistance<ClosestDistance)
				{
					ClosestDistance=ThisDistance;
					ClosestCell = pCell;
				}
			}
		}
	}

	return (ClosestCell);
}

//:	BuildNavigationPath
//----------------------------------------------------------------------------------------
//
// Build a navigation path using the provided points and the A* method
//
//-------------------------------------------------------------------------------------://
bool NavigationMesh::BuildNavigationPath(NavigationPath& NavPath, NavigationCell* StartCell, const math::Vec3& StartPos, NavigationCell* EndCell, const math::Vec3& EndPos)
{
	bool FoundPath = false;

	// Increment our path finding session ID
	// This Identifies each pathfinding session
	// so we do not need to clear out old data
	// in the cells from previous sessions.
	++m_PathSession;

	// load our data into the NavigationHeap object
	// to prepare it for use.
	m_NavHeap.Setup(m_PathSession, StartPos);

	// We are doing a reverse search, from EndCell to StartCell.
	// Push our EndCell onto the Heap at the first cell to be processed
	EndCell->QueryForPath(&m_NavHeap, 0, 0);

	// process the heap until empty, or a path is found
	while(m_NavHeap.NotEmpty() && !FoundPath)
	{
		NavigationNode ThisNode;

		// pop the top cell (the open cell with the lowest cost) off the Heap
		m_NavHeap.GetTop(ThisNode);

		// if this cell is our StartCell, we are done
		if(ThisNode.cell == StartCell)
		{
			FoundPath = true;
		}
		else
		{
			// Process the Cell, Adding it's neighbors to the Heap as needed
			ThisNode.cell->ProcessCell(&m_NavHeap);
		}
	}

	// if we found a path, build a waypoint list
	// out of the cells on the path
	if (FoundPath)
	{
		NavigationCell* TestCell = StartCell;
		math::Vec3 NewWayPoint;

		// Setup the Path object, clearing out any old data
		NavPath.Setup(this, StartPos, StartCell, EndPos, EndCell);

		// Step through each cell linked by our A* algorythm 
		// from StartCell to EndCell
		while (TestCell && TestCell != EndCell)
		{
			// add the link point of the cell as a way point (the exit wall's center)
			int LinkWall = TestCell->ArrivalWall();

			NewWayPoint = TestCell->WallMidpoint(LinkWall);
			NewWayPoint = SnapPointToCell(TestCell, NewWayPoint); // just to be sure

			NavPath.AddWayPoint(NewWayPoint, TestCell);

			// and on to the next cell
			TestCell = TestCell->Link(LinkWall);
		}

		// cap the end of the path.
		NavPath.EndPath();
		return(true);
	}
	return(false);
}

//:	ResolveMotionOnMesh
//----------------------------------------------------------------------------------------
//
// Resolve a movement vector on the mesh
//
//-------------------------------------------------------------------------------------://
void NavigationMesh::ResolveMotionOnMesh(const math::Vec3& StartPos, NavigationCell* StartCell, math::Vec3& EndPos, NavigationCell** EndCell)
{
	if( StartCell == 0 )
		return;

	// create a 2D motion path from our Start and End positions, tossing out their Y values to project them 
	// down to the XZ plane.
	Line2D MotionPath(math::Vec2(StartPos.X,StartPos.Z), math::Vec2(EndPos.X,EndPos.Z));

	// these three will hold the results of our tests against the cell walls
	NavigationCell::PATH_RESULT Result = NavigationCell::NO_RELATIONSHIP;
	NavigationCell::CELL_SIDE WallNumber;
	math::Vec2 PointOfIntersection;
	NavigationCell* NextCell = 0;

	// TestCell is the cell we are currently examining.
	NavigationCell* TestCell = StartCell;

	int numCalls = 0;

	//
	// Keep testing until we find our ending cell or stop moving due to friction
	//
	while ((Result != NavigationCell::ENDING_CELL) && (MotionPath.EndPointA() != MotionPath.EndPointB()))
	{
		// check if stuck
		numCalls++;
		if( numCalls > 10 )
			break;

		// use NavigationCell to determine how our path and cell interact
		Result = TestCell->ClassifyPathToCell(MotionPath, &NextCell, WallNumber, &PointOfIntersection);

		// if exiting the cell...
		if (Result == NavigationCell::EXITING_CELL)
		{
			// Set if we are moving to an adjacent cell or we have hit a solid (unlinked) edge
			if(NextCell)
			{
				// moving on. Set our motion origin to the point of intersection with this cell
				// and continue, using the new cell as our test cell.
				MotionPath.SetEndPointA(PointOfIntersection);
				TestCell = NextCell;
			}
			else
			{
				// we have hit a solid wall. Resolve the collision and correct our path.
				MotionPath.SetEndPointA(PointOfIntersection);
				TestCell->ProjectPathOnCellWall(WallNumber, MotionPath);

				// add some friction to the new MotionPath since we are scraping against a wall.
				// we do this by reducing the magnatude of our motion by 10%
				math::Vec2 Direction = MotionPath.EndPointB() - MotionPath.EndPointA();
				Direction *= 0.9f;
				MotionPath.SetEndPointB(MotionPath.EndPointA() + Direction);
			}
		}
		else if (Result == NavigationCell::NO_RELATIONSHIP)
		{
			// Although theoretically we should never encounter this case,
			// we do sometimes find ourselves standing directly on a vertex of the cell.
			// This can be viewed by some routines as being outside the cell.
			// To accomodate this rare case, we can force our starting point to be within
			// the current cell by nudging it back so we may continue.
			math::Vec2 NewOrigin = MotionPath.EndPointA();
			TestCell->ForcePointToCellCollumn(NewOrigin);
			MotionPath.SetEndPointA(NewOrigin);
		}
	}

	// we now have our new host cell
	*EndCell = TestCell;

	// Update the new control point position, 
	// solving for Y using the Plane member of the NavigationCell
	EndPos.X = MotionPath.EndPointB().X;
	EndPos.Z = MotionPath.EndPointB().Y;
	TestCell->MapVectorHeightToCell(EndPos);
}


//:	LineOfSightTest
//----------------------------------------------------------------------------------------
//
// Test to see if two points on the mesh can view each other
//
//-------------------------------------------------------------------------------------://
bool NavigationMesh::LineOfSightTest(NavigationCell* StartCell, const math::Vec3& StartPos, NavigationCell* EndCell, const math::Vec3& EndPos)
{
	Line2D MotionPath(math::Vec2(StartPos.X,StartPos.Z), math::Vec2(EndPos.X,EndPos.Z));
	NavigationCell* NextCell = StartCell;
	NavigationCell::CELL_SIDE WallNumber;
	NavigationCell::PATH_RESULT Result;

	while((Result = NextCell->ClassifyPathToCell(MotionPath, &NextCell, WallNumber, 0)) == NavigationCell::EXITING_CELL)
	{
		if (!NextCell) return(false);
	}

	return (Result == NavigationCell::ENDING_CELL);
}

//:	LinkCells
//----------------------------------------------------------------------------------------
//
// Link all the cells that are in our pool
//
//-------------------------------------------------------------------------------------://
void NavigationMesh::LinkCells()
{
	CELL_ARRAY::iterator IterA = m_CellArray.begin();

	while (IterA != m_CellArray.end())
	{
		NavigationCell* pCellA = *IterA;
		CELL_ARRAY::iterator IterB = m_CellArray.begin();

		while (IterB != m_CellArray.end())
		{
			NavigationCell* pCellB = *IterB;

			if (IterA != IterB)
			{
				if (!pCellA->Link(NavigationCell::SIDE_AB) && pCellB->RequestLink(pCellA->Vertex(0), pCellA->Vertex(1), pCellA))
				{
					pCellA->SetLink(NavigationCell::SIDE_AB, pCellB);
				}
				else if (!pCellA->Link(NavigationCell::SIDE_BC) && pCellB->RequestLink(pCellA->Vertex(1), pCellA->Vertex(2), pCellA))
				{
					pCellA->SetLink(NavigationCell::SIDE_BC, pCellB);
				}
				else if (!pCellA->Link(NavigationCell::SIDE_CA) && pCellB->RequestLink(pCellA->Vertex(2), pCellA->Vertex(0), pCellA))
				{
					pCellA->SetLink(NavigationCell::SIDE_CA, pCellB);
				}
			}

			++IterB;
		}

		++IterA;
	}
}

