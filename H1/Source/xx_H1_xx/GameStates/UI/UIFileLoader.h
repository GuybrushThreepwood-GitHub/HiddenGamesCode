
/*===================================================================
	File: UIFileLoader.h
	Game: H1

	(C)Hidden Games
=====================================================================*/

#ifndef __UIFILELOADER_H__
#define __UIFILELOADER_H__

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


		struct UIElement
		{
			int huiType;
			int elementId;
			bool show;

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

			int selectable;
			int hightlightOnSelect;

			int numeric1;
			int numeric2;
			int numeric3;
			int numeric4;
			int numeric5;

			int areaType;

			math::Vec3 origPos;
			math::Vec3 pos;
			math::Vec3 screenPos; // text only
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

			// button only
			collision::Sphere sphere;

			// text only
			int doNotScale;
			int dropShadow;
			int fontId;
			int fontSize;
			int fontR;
			int fontG;
			int fontB;

			int fixedWidth;
			bool drawBox;
			math::Vec4Lite boxColour;
			math::Vec4Lite textColour;
			renderer::FreetypeFont* pFontfreetype;
			char characterBuffer[UI_MAXSTATICTEXTBUFFER_SIZE];
			unsigned int currentCharCount;
			UIFileLoader::TextAlignment alignment;
		};

		/*struct SimpleButton
		{
			bool show;
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

			int selectable;
			int hightlightOnSelect;

			int numeric1;
			int numeric2;
			int numeric3;
			int numeric4;
			int numeric5;

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

			int selectable;
			int hightlightOnSelect;

			int numeric1;
			int numeric2;
			int numeric3;
			int numeric4;
			int numeric5;

			math::Vec3 origPos;
			math::Vec3 screenPos;
			math::Vec3 pos;

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
			math::Vec4Lite textColour;

			renderer::FreetypeFont* pFontfreetype;
		
			/// current string holder
			char characterBuffer[UI_MAXSTATICTEXTBUFFER_SIZE];
			/// current number of characters in the string buffer
			unsigned int currentCharCount;

			/// alignment
			UIFileLoader::TextAlignment alignment;

			collision::AABB aabb;
		};*/

	public:
		/// default constructor
		UIFileLoader();
		/// default destructor
		~UIFileLoader();
		void Release();
		
		int Load( const char *szFilename, const math::Vec2& srcAssetDims, mdl::ModelHGM* model=0 );
		int LoadFromScript( const char *szFilename, const math::Vec2& srcAssetDims, mdl::ModelHGM* model );

		bool HasSelectableElements()	{ return m_HasSelectableElements; }

		void Update( float touchSize, int numTouchTests, float deltaTime );

		void Draw();
		void DrawSelected();

		bool CheckElementForTouch( int elementId, EventCode eventCode=EVENT_RELEASE );
		bool CheckElementForSingleTouch( int elementId );

		void ChangeElementText( int elementId, const char* text, ... );
		void ChangeElementTextBoxState( int elementId, bool state );
		void ChangeElementTextColour( int elementId, const math::Vec4Lite& col );
		void ChangeElementTextBoxColour( int elementId, const math::Vec4Lite& col );
		void ChangeElementTextShadowState(int elementId, bool state);
        void ChangeElementTextAlignment(int elementId, TextAlignment alignment );
    
		void ChangeElementDrawState( int elementId, bool state );
		void ChangeElementDrawState( bool state );

		void ChangeElementPosition( int elementId, const math::Vec3& pos );

		const math::Vec3& GetElementPosition( int elementId );
		const math::Vec3& GetElementPositionNonAABB( int elementId );
		const math::Vec3& GetLastTouchPosition( int elementId );
		const int GetLastTouchIndex( int elementId );
		const int GetElementTouchCount( int elementId );
		const UIElement* GetElement( int elementId );
		const char* GetElementText( int elementId );
		math::Vec4Lite& GetElementTextColour( int elementId );

		const collision::AABB& GetElementAABB(int elementId);

		void RefreshSelected(int selectElement=-1);

		bool SelectNextColumn();
		bool SelectPreviousColumn();

		bool SelectNextRow();
		bool SelectPreviousRow();

		void SelectionPress(bool state);
		
	private:
		UIElement* FindNearestElementToSelection(int selectionIndex, int minValue, int maxValue, bool nextInc);

	private:
		bool m_Loaded;

		mdl::ModelHGM* m_Model;

		//std::vector<SimpleText> m_Text;
		//std::vector<SimpleButton> m_Buttons;
		std::vector<UIElement> m_UIElements;

		bool m_HasSelectableElements;
		UIElement* m_SelectedElement;

		math::Vec4Lite m_SelectionColour;
		float m_SelectionAlpha;
		bool m_SelectionAnimInc;

		ScriptDataHolder::DevScriptData m_DevData;
};

#endif // __UIFILELOADER_H__


