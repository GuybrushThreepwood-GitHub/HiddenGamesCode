

#ifndef __MICROGAMEPICKUPITEM_H__
#define __MICROGAMEPICKUPITEM_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGamePickupItem : public MicroGame
{
	public:
		MicroGamePickupItem( const math::Vec3& camPos, const math::Vec3& camLookAt, float maxYRotation, float maxXRotation, float minZoom, float maxZoom, int objectId, int uiModelId, int description1Index, int description2Index );
		virtual ~MicroGamePickupItem();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_ItemModel;

		mdl::ModelHGM* m_PickUIModel;
		UIFileLoader m_PickupUI;

		math::Vec3 m_CamPos;
		math::Vec3 m_CamLookAt;

		int m_ItemModelId;
		int m_UIModelId;

		math::Vec3 m_Rot;

		float m_MaxYRot;
		float m_MaxXRot;

		float m_CurrentZoom;
		float m_MinZoom;
		float m_MaxZoom;

		const char* m_Description1;
		const char* m_Description2;
};

#endif // __MICROGAMEPICKUPITEM_H__
