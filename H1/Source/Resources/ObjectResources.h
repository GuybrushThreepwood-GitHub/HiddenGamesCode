
/*===================================================================
	File: ObjectResources.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __OBJECTRESOURCES_H__
#define __OBJECTRESOURCES_H__

namespace res
{
	// sound info store
	struct ObjectResourceStore
	{
		int resId;
		const char* physicsFile;
		bool destroyFlag;
		int lowModelId;
		int mediumModelId;
		int highModelId;
		float disableDistance;
		float lowDistance;
		float mediumDistance;
		float highDistance;
	};

	void CreateObjectResourceMap();

	void ClearObjectResources();

	const ObjectResourceStore* GetObjectResource( int index );
}

#endif // __OBJECTRESOURCES_H__
