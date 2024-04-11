
/*===================================================================
	File: UIFileLoader.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __UIFILELOADER_H__
#define __UIFILELOADER_H__

#include "ScriptAccess/ScriptDataHolder.h"

const int BUTTON_TYPE = 0;
const int TEXT_TYPE = 1;
const int UI_MAXSTATICTEXTBUFFER_SIZE	= 256;

class UIFileLoader
{
	public:
		enum TextAlignment
		{
			TextAlignment_Left=0,
			TextAlignment_Center,
			TextAlignment_Right,
		};

		enum AnchorHorizontal
		{
			AnchorHorizontal_None=0,
			AnchorHorizontal_Center,
			AnchorHorizontal_Left,
			AnchorHorizontal_Right,
			AnchorHorizontal_CenterOffset,
		};

		enum AnchorVertical
		{
			AnchorVertical_None=0,
			AnchorVertical_Center,
			AnchorVertical_Top,
			AnchorVertical_Bottom,
			AnchorVertical_CenterOffset,
		};

		enum EventCode
		{
			EVENT_PRESS=0,
			EVENT_RELEASE=1,
			EVENT_HELD=2,

			EVENT_HOVER=3
		};

	public:
		/// default constructor
		UIFileLoader();
		/// default destructor
		~UIFileLoader();
		
		int Load( const char *szFilename, const math::Vec2& srcAssetDims, mdl::ModelHGM* model=0 );
		int LoadFromScript( const char *szFilename, const math::Vec2& srcAssetDims, mdl::ModelHGM* model );

		void Update( float touchSize, int numTouchTests, float deltaTime );

		void Draw();

		bool CheckElementForTouch( int elementId, EventCode eventCode=EVENT_RELEASE );
		bool CheckElementForSingleTouch( int elementId );

		void ChangeElementText( int elementId, const char* text, ... );
		void ChangeElementTextBoxState( int elementId, bool state );
		void ChangeElementTextBoxColour( int elementId, const math::Vec4Lite& col );

		void ChangeElementDrawState( int elementId, bool state );
		void ChangeElementDrawState( bool state );

		void ChangeElementPosition( int elementId, const math::Vec3& pos );

		const math::Vec3& GetElementPosition( int elementId );
		const math::Vec3& GetLastTouchPosition( int elementId );
		const int GetLastTouchIndex( int elementId );
		const int GetElementTouchCount( int elementId );

	public:
		struct SimpleButton
		{
			int elementId;

			int meshAttach1Id;
			int meshAttach2Id;
			int meshAttach3Id;
			int meshAttach4Id;
			int meshAttach5Id;

			int useNormalScale;
			int anchorHorizontal;
			int anchorVertical;

			int scaleDimsX;
			int scaleDimsY;

			int areaType;

			math::Vec3 origPos;
			math::Vec3 pos;
			math::Vec3 dims;

			math::Vec3 anchoredPos;

			bool touched;
			bool singleTouch;
			int touchIndex;
			math::Vec3 touchPos;
			int touchCount;
			int eventCode; 
			int hover;

			collision::AABB aabb;
			collision::Sphere sphere;
		};

		struct SimpleText
		{
			bool show;

			int doNotScale;
			int elementId;
			int dropShadow;
			int fontId;
			int fontSize;
			int fontR;
			int fontG;
			int fontB;
			int meshAttach1Id;
			int meshAttach2Id;
			int meshAttach3Id;
			int meshAttach4Id;
			int meshAttach5Id;

			int useNormalScale;
			int anchorHorizontal;
			int anchorVertical;

			int scaleDimsX;
			int scaleDimsY;

			math::Vec3 origPos;
			math::Vec3 screenPos;
			math::Vec3 pos;
			//math::Vec3 dims;

			math::Vec3 anchoredPos;

			bool touched;
			bool singleTouch;
			int touchIndex;
			math::Vec3 touchPos;
			int touchCount;
			int eventCode; 
			int hover;

			int fixedWidth;
			bool drawBox;
			math::Vec4Lite boxColour;

			renderer::FreetypeFont* pFontfreetype;
		
			/// current string holder
			char characterBuffer[UI_MAXSTATICTEXTBUFFER_SIZE];
			/// current number of characters in the string buffer
			unsigned int currentCharCount;

			/// alignment
			UIFileLoader::TextAlignment alignment;

			collision::AABB aabb;
		};

	private:
		bool m_Loaded;

		mdl::ModelHGM* m_Model;

		std::vector<SimpleButton> m_Buttons;
		std::vector<SimpleText> m_Text;

		ScriptDataHolder::DevScriptData m_DevData;
};

#endif // __UIFILELOADER_H__


