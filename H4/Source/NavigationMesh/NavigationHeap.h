/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef __NAVIGATIONHEAP_H__
#define __NAVIGATIONHEAP_H__

#ifndef __NAVIGATIONCELL_H__
	#include "NavigationCell.h"
#endif

#pragma warning(disable : 4786)
#include <vector>

#include <functional>
#pragma warning(disable : 4786)
#include <list>

#pragma warning(disable : 4786)
#include <algorithm>


/*	NavigationNode
------------------------------------------------------------------------------------------
	
	A NavigationNode represents an entry in the NavigationHeap. It provides some simple
	operators to classify it against other NavigationNodes when the heap is sorted.
	
------------------------------------------------------------------------------------------
*/
class NavigationNode
{
	public:
		NavigationNode(): cell(0), cost(0)	{}
		~NavigationNode()					{}

		NavigationCell* cell; // pointer to the cell in question
		float cost;        // (g + h) in A* represents the cost of traveling through this cell

		inline bool operator < (const NavigationNode& b )
		{
			// To compare two nodes, we compare the cost or `f' value, which is the
			// sum of the g and h values defined by A*.
			return (cost < (b.cost));
		}

		inline bool operator > (const NavigationNode& b )
		{
			// To compare two nodes, we compare the cost or `f' value, which is the
			// sum of the g and h values defined by A*.
			return (cost > (b.cost));
		}

		inline bool operator == (const NavigationNode& b )
		{
			// Two nodes are equal if their components are equal
			return ((cell == b.cell) && (cost == b.cost));
		}
};

//
// The STL requires some external operators as well
//
inline bool operator < ( const NavigationNode& a, const NavigationNode& b )
{
	return (a.cost < b.cost);
}

inline bool operator > ( const NavigationNode& a, const NavigationNode& b )
{
	return (a.cost > b.cost);
}

inline bool operator == ( const NavigationNode& a, const NavigationNode& b )
{
	return ((a.cell == b.cell) && (a.cost == b.cost));
}


/*	NavigationHeap
------------------------------------------------------------------------------------------
	
	A NavigationHeap is a priority-ordered list facilitated by the STL heap functions.
	This class is also used to hold the current path finding session ID and the desired 
	goal point for NavigationCells to query. Thanks to Amit J. Patel for detailing the use
	of STL heaps in this way. It's much faster than a linked list or multimap approach.
	
------------------------------------------------------------------------------------------
*/
class NavigationHeap
{
	public:

		// ----- ENUMERATIONS & CONSTANTS -----
		// greater<NavigationNode> is an STL thing to create a 'comparison' object out of
		// the greater-than operator, and call it comp.
		typedef std::vector<NavigationNode> Container;
		std::greater<NavigationNode> comp;


		NavigationHeap();
		~NavigationHeap();

		void Setup(int SessionID, const math::Vec3& Goal);
		void AddCell(NavigationCell* pCell);
		void AdjustCell(NavigationCell* pCell);

		bool NotEmpty()const;
		void GetTop(NavigationNode& n);
		int SessionID()const;
		const math::Vec3& Goal()const;

	private:

		Container m_Nodes;
		int m_SessionID;
		math::Vec3 m_Goal;

		Container::iterator FindNodeInterator(NavigationCell* pCell);

		NavigationHeap( const NavigationHeap& Src);
		NavigationHeap& operator=( const NavigationHeap& Src);
};

//- Inline Functions ---------------------------------------------------------------------

/*	NavigationHeap
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationHeap::NavigationHeap()
{

}

/*	~NavigationHeap
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationHeap::~NavigationHeap()
{
}

//:	Setup
//	Initialize the heap and record our new session info 
inline void NavigationHeap::Setup(int SessionID, const math::Vec3& Goal)
{
	m_Goal = Goal;
	m_SessionID = SessionID;
	m_Nodes.clear();
}

//:	AddCell
//	Add a cell to the heap, sorting it in by it's cost value 
inline void NavigationHeap::AddCell(NavigationCell* pCell)
{
	NavigationNode NewNode;

	NewNode.cell = pCell;
	NewNode.cost = pCell->PathfindingCost();

	m_Nodes.push_back(NewNode);
	std::push_heap( m_Nodes.begin(), m_Nodes.end(), comp );
}

//:	AdjustCell
//	Adjust a cell in the heap to reflect it's updated cost value. NOTE: Cells may only 
//	sort up in the heap. 
inline void NavigationHeap::AdjustCell(NavigationCell* pCell)
{
	Container::iterator iter = FindNodeInterator(pCell);

	if (iter!=m_Nodes.end())
	{
		// update the node data
		(*iter).cell = pCell;
		(*iter).cost = pCell->PathfindingCost();
	
		// reorder the heap
		std::push_heap( m_Nodes.begin(), iter+1, comp );
	}
}

//:	NotEmpty
//	Returns true if the heap is not empty 
inline bool NavigationHeap::NotEmpty()const
{
	return(m_Nodes.size() ? true:false);
}

//:	GetTop
//	Pop the top off the heap and remove the best value for processing. 
inline void NavigationHeap::GetTop(NavigationNode& n)
{
	n = m_Nodes.front();
	std::pop_heap( m_Nodes.begin(), m_Nodes.end(), comp );
	m_Nodes.pop_back();
}

//:	FindNodeInterator
//	Search the container for a given cell. May be slow, so don't do this unless nessesary. 
inline NavigationHeap::Container::iterator NavigationHeap::FindNodeInterator(NavigationCell* pCell)
{
	for( Container::iterator i = m_Nodes.begin(); i != m_Nodes.end(); ++i )
	{
	  if( (*i).cell == pCell )
		  return i;
	}
	return m_Nodes.end();
}

inline int NavigationHeap::SessionID()const
{
	return(m_SessionID);
}

inline 	const math::Vec3& NavigationHeap::Goal()const
{
	return(m_Goal);
}

#endif  // __NAVIGATIONHEAP_H__

