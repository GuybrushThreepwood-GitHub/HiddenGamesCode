
#ifndef __NAVOBJECT_H__
#define __NAVOBJECT_H__

#include "NavigationCell.h"
#include "NavigationMesh.h"
#include "NavigationPath.h"

class NavObject
{
	public:
		NavObject( NavigationMesh* pNavmesh )
		{
			m_Parent = pNavmesh;
			m_CurrentCell = 0;
			m_PathActive = false;
		}
		virtual ~NavObject(){}

		void CalculateStartingCell( math::Vec3& pos )
		{
			if (m_Parent)
			{
				// if no starting cell is provided, find one by searching the mesh
				//if (!m_CurrentCell)
				{
					m_CurrentCell = m_Parent->FindClosestCell(pos);
				}

				//if( m_CurrentCell )
				//{
					// make sure our position is within the current cell
				//	pos =  m_Parent->SnapPointToCell(m_CurrentCell, pos);
				//}
			}
		}

		NavigationCell* GetCurrentCell()	{ return m_CurrentCell; }

	public:
		NavigationMesh* m_Parent;		// the mesh we are sitting on
		NavigationCell* m_CurrentCell;	// our current cell on the mesh

		bool m_PathActive;				// true when we are using a path to navigate
		NavigationPath m_Path;			// our path object
		NavigationPath::WayPointID m_NextWaypoint; // ID of the next waypoint we will move to
};

#endif // __NAVOBJECT_H__

