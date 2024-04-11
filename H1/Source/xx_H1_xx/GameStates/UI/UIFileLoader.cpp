
/*===================================================================
	File: UIFileLoader.cpp
	Game: H1

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "InputBase.h"

#include <vector>
#include <ctype.h>

#include "Resources/FontResources.h"
#include "Resources/StringResources.h"

#include "GameSystems.h"
#include "UIFileLoader.h"

namespace
{
	math::Vec3 BadPos( -100.0f, -100.0f, 0.0f );
	math::Vec4Lite defaultColour( 255,255,255,255 );
	collision::AABB BadAABB;
}

/////////////////////////////////////////////////////
/// Function: RoundUp
/// Params: [in]value
///
/////////////////////////////////////////////////////
int RoundUp(int value)
{
	return 10 * ((value + 9) / 10);
}

/////////////////////////////////////////////////////
/// Function: RoundDown
/// Params: [in]value
///
/////////////////////////////////////////////////////
int RoundDown(int value)
{
	return 10 * (value / 10);
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
UIFileLoader::UIFileLoader()
{
	m_Loaded = false;
	m_HasSelectableElements = false;
	m_SelectedElement = 0;

	m_SelectionAlpha = 1.0f;
	m_SelectionColour = math::Vec4Lite(0, 255, 0, 0);
	m_SelectionAnimInc = false;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
UIFileLoader::~UIFileLoader()
{
	Release();
	
	//m_Buttons.clear();
	//m_Text.clear();
	m_UIElements.clear();
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void UIFileLoader::Release()
{
	std::vector<UIElement>::iterator textIt;
	
	// get touch flag
	textIt = m_UIElements.begin();
	while (textIt != m_UIElements.end())
	{
		if( textIt->pFontfreetype != 0 )
			res::RemoveFontResource( textIt->pFontfreetype );
		
		// next
		textIt++;
	}

}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int UIFileLoader::Load( const char *szFilename, const math::Vec2& srcAssetDims, mdl::ModelHGM* model )
{
	m_Loaded = false;
	m_HasSelectableElements = false;
	m_SelectedElement = 0;
	m_Model = model;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	return LoadFromScript( szFilename, srcAssetDims, model );
}

/////////////////////////////////////////////////////
/// Method: LoadFromScript
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int UIFileLoader::LoadFromScript( const char *szFilename, const math::Vec2& srcAssetDims, mdl::ModelHGM* model )
{
	if( core::IsEmptyString( szFilename ) )
		return 1;

	lua_State* pState = 0;
	int tableIndex = 1;
	int i=0;
	math::Vec2 scale = renderer::GetAssetScale( static_cast<int>(srcAssetDims.X), static_cast<int>(srcAssetDims.Y) );

	// reading from a saved file
	pState = script::LuaScripting::GetState();

	if( !script::LoadScript( szFilename ) )
	{
		lua_getglobal( pState, "hui_data" );
		tableIndex = -1;

		if( lua_istable( pState, tableIndex ) )
		{
			// grab what levels were generated for this career
			lua_pushstring( script::LuaScripting::GetState(), "hui_types" );
			lua_gettable( script::LuaScripting::GetState(), -2 );

			if( lua_istable( script::LuaScripting::GetState(), -1 ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = -2;
						
						int huiType = static_cast<int>( script::LuaGetNumberFromTableItem( "huiType", paramIndex, 0.0 ) );
						int huiId = static_cast<int>( script::LuaGetNumberFromTableItem( "huiId", paramIndex, 0.0 ) );
						int meshAttachId1 = static_cast<int>( script::LuaGetNumberFromTableItem( "meshAttachId1", paramIndex, -1.0 ) );
						int meshAttachId2 = static_cast<int>( script::LuaGetNumberFromTableItem( "meshAttachId2", paramIndex, -1.0 ) );
						int meshAttachId3 = static_cast<int>( script::LuaGetNumberFromTableItem( "meshAttachId3", paramIndex, -1.0 ) );
						int meshAttachId4 = static_cast<int>( script::LuaGetNumberFromTableItem( "meshAttachId4", paramIndex, -1.0 ) );
						int meshAttachId5 = static_cast<int>( script::LuaGetNumberFromTableItem( "meshAttachId5", paramIndex, -1.0 ) );

						int useScale = static_cast<int>( script::LuaGetNumberFromTableItem( "useScale", paramIndex, 0.0 ) );
						int anchorHoriz = static_cast<int>( script::LuaGetNumberFromTableItem( "anchorHoriz", paramIndex, 0.0 ) );
						int anchorVert = static_cast<int>( script::LuaGetNumberFromTableItem( "anchorVert", paramIndex, 0.0 ) );
						int scaleDimsX = static_cast<int>( script::LuaGetNumberFromTableItem( "scaleDimsX", paramIndex, 1.0 ) );
						int scaleDimsY = static_cast<int>( script::LuaGetNumberFromTableItem( "scaleDimsY", paramIndex, 1.0 ) );

						int selectable = static_cast<int>(script::LuaGetNumberFromTableItem("selectable", paramIndex, 0.0));
						int highlightOnSelect = static_cast<int>(script::LuaGetNumberFromTableItem("highlightOnSelect", paramIndex, 0.0));

						float posX = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
						float posY = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
						float posZ = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

						int numeric1 = static_cast<int>(script::LuaGetNumberFromTableItem("numeric1", paramIndex, -9999.0));
						int numeric2 = static_cast<int>(script::LuaGetNumberFromTableItem("numeric2", paramIndex, -9999.0));
						int numeric3 = static_cast<int>(script::LuaGetNumberFromTableItem("numeric3", paramIndex, -9999.0));
						int numeric4 = static_cast<int>(script::LuaGetNumberFromTableItem("numeric4", paramIndex, -9999.0));
						int numeric5 = static_cast<int>(script::LuaGetNumberFromTableItem("numeric5", paramIndex, -9999.0));

						if (!m_HasSelectableElements)
						{
							if (selectable != 0 &&
								numeric1 != -9999)
							{
								m_HasSelectableElements = true;
							}
						}

						switch( huiType )
						{
							case BUTTON_TYPE:
							{
								UIElement newElement;
								//SimpleButton button;
								math::Vec3 pos( posX, posY, posZ );
								
								newElement.huiType = huiType;

								int buttonType = static_cast<int>( script::LuaGetNumberFromTableItem( "buttonType", paramIndex, 0.0 ) );
								float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
								float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
								float r = static_cast<float>( script::LuaGetNumberFromTableItem( "r", paramIndex, 1.0 ) );

								newElement.elementId = huiId;
								newElement.areaType = buttonType;
								newElement.meshAttach1Id = meshAttachId1;
								newElement.meshAttach2Id = meshAttachId2;
								newElement.meshAttach3Id = meshAttachId3;
								newElement.meshAttach4Id = meshAttachId4;
								newElement.meshAttach5Id = meshAttachId5;

								newElement.selectable = selectable;
								newElement.hightlightOnSelect = highlightOnSelect;

								newElement.numeric1 = numeric1;
								newElement.numeric2 = numeric2;
								newElement.numeric3 = numeric3;
								newElement.numeric4 = numeric4;
								newElement.numeric5 = numeric5;

								newElement.useNormalScale = useScale;
								newElement.anchorHorizontal = anchorHoriz;
								newElement.anchorVertical = anchorVert;
								newElement.scaleDimsX = scaleDimsX;
								newElement.scaleDimsY = scaleDimsY;
								newElement.dims.Width = w*0.5f;
								newElement.dims.Height = h*0.5f;

								// always keep unchanged position
								newElement.origPos = newElement.pos = pos;

								// scale the types
								if (newElement.useNormalScale)
								{
									newElement.pos.X *= scale.X;
									newElement.pos.Y *= scale.Y;
								}
								else
								{
									// check alignments
									if (newElement.anchorHorizontal == AnchorHorizontal_Center)
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.pos.X = core::app::GetOrientationHeight() * 0.5f;
										else
											newElement.pos.X = core::app::GetOrientationWidth() * 0.5f;
									}
									else
									if (newElement.anchorHorizontal == AnchorHorizontal_Left)
									{
										newElement.anchoredPos.X = newElement.pos.X;
										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
										newElement.pos.X = newElement.anchoredPos.X;
									}
									else
									if (newElement.anchorHorizontal == AnchorHorizontal_Right)
									{
										newElement.anchoredPos.X = (srcAssetDims.Height - newElement.pos.X);
										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.pos.X = core::app::GetOrientationHeight() - newElement.anchoredPos.X;
										else								
											newElement.pos.X = core::app::GetOrientationWidth() - newElement.anchoredPos.X;
									}
									else
									if (newElement.anchorHorizontal == AnchorHorizontal_CenterOffset)
									{
										float origHalfWidth = 0.0f;
										float halfWidth = 0.0f;

										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
										{
											halfWidth = core::app::GetOrientationHeight() * 0.5f;
											origHalfWidth = srcAssetDims.Y * 0.5f;
										}
										else
										{
											halfWidth = core::app::GetOrientationWidth() * 0.5f;
											origHalfWidth = srcAssetDims.Y * 0.5f;
										}	

										if (newElement.pos.X < origHalfWidth)
										{
											newElement.anchoredPos.X = halfWidth - (origHalfWidth - newElement.pos.X);
										}
										else
										if (newElement.pos.X > origHalfWidth)
										{
											newElement.anchoredPos.X = halfWidth + (newElement.pos.X - origHalfWidth);
										}
										else
										if (newElement.pos.X == origHalfWidth)
										{
											newElement.pos.X = halfWidth;
										}

										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
										newElement.pos.X = newElement.anchoredPos.X;
									}

									// vertical
									if (newElement.anchorVertical == AnchorVertical_Center)
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.pos.Y = core::app::GetOrientationWidth() * 0.5f;
										else
											newElement.pos.Y = core::app::GetOrientationHeight() * 0.5f;
									}
									else
									if (newElement.anchorVertical == AnchorVertical_Top)
									{
										newElement.anchoredPos.Y = (srcAssetDims.Width - newElement.pos.Y);
										if (newElement.scaleDimsY)
											newElement.anchoredPos.Y *= scale.Y;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.pos.Y = core::app::GetOrientationWidth() - newElement.anchoredPos.Y;
										else
											newElement.pos.Y = core::app::GetOrientationHeight() - newElement.anchoredPos.Y;
									}
									else
									if (newElement.anchorVertical == AnchorVertical_Bottom)
									{
										newElement.anchoredPos.Y = newElement.pos.Y;
										if (newElement.scaleDimsY)
											newElement.anchoredPos.Y *= scale.Y;
										newElement.pos.Y = newElement.anchoredPos.Y;
									}
									else
										if (newElement.anchorVertical == AnchorVertical_CenterOffset)
									{
										float origHalfHeight = 0.0f;
										float halfHeight= 0.0f;

										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
										{							
											halfHeight = core::app::GetOrientationWidth() * 0.5f;
											origHalfHeight = srcAssetDims.X * 0.5f;
										}
										else
										{
											halfHeight = core::app::GetOrientationHeight() * 0.5f;
											origHalfHeight = srcAssetDims.X * 0.5f;
										}	

										if (newElement.pos.Y < origHalfHeight)
										{
											newElement.anchoredPos.Y = halfHeight - (origHalfHeight - newElement.pos.Y);
										}
										else
										if (newElement.pos.Y > origHalfHeight)
										{
											newElement.anchoredPos.Y = halfHeight + (newElement.pos.Y - origHalfHeight);
										}
										else
										if (newElement.pos.Y == origHalfHeight)
										{
											newElement.pos.Y = halfHeight;
										}

										if (newElement.scaleDimsY)
											newElement.anchoredPos.Y *= scale.Y;
										newElement.pos.Y = newElement.anchoredPos.Y;
									}
								}

								// scale dimensions
								if (newElement.scaleDimsX || newElement.useNormalScale)
								{
									//button.pos.X *= scale.X;
									newElement.dims.Width *= scale.X;
								}
								if (newElement.scaleDimsY || newElement.useNormalScale)
								{
									//button.pos.Y *= scale.Y;
									newElement.dims.Height *= scale.Y;
								}

								// create the aabb
								newElement.aabb.vCenter = newElement.pos;
								newElement.aabb.vBoxMin = newElement.pos - newElement.dims;
								newElement.aabb.vBoxMax = newElement.pos + newElement.dims;

								if( m_Model != 0 )
								{
									math::Vec3 modelScale( 1.0f, 1.0f, 1.0f );

									if (newElement.scaleDimsX || newElement.useNormalScale)
										modelScale.X *= scale.X;
									if (newElement.scaleDimsY || newElement.useNormalScale)
										modelScale.Y *= scale.Y;

									if (newElement.meshAttach1Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach1Id, newElement.pos);
										m_Model->SetMeshScale(newElement.meshAttach1Id, modelScale);
									}

									if (newElement.meshAttach2Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach2Id, newElement.pos);
										m_Model->SetMeshScale(newElement.meshAttach2Id, modelScale);
									}

									if (newElement.meshAttach3Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach3Id, newElement.pos);
										m_Model->SetMeshScale(newElement.meshAttach3Id, modelScale);
									}
									
									if (newElement.meshAttach4Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach4Id, newElement.pos);
										m_Model->SetMeshScale(newElement.meshAttach4Id, modelScale);
									}
									
									if (newElement.meshAttach5Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach5Id, newElement.pos);
										m_Model->SetMeshScale(newElement.meshAttach5Id, modelScale);
									}									
								}

								newElement.sphere.fRadius = r;

								// scale the types
								if( scale.X > scale.Y )
								{
									if (newElement.scaleDimsX || newElement.useNormalScale)
										newElement.sphere.fRadius *= scale.X;
								}
								else
								{
									if (newElement.scaleDimsY || newElement.useNormalScale)
										newElement.sphere.fRadius *= scale.Y;
								}

								newElement.sphere.vCenterPoint = newElement.pos;

								// text only init 
								newElement.drawBox = false;
								newElement.fontId = -1;
								newElement.doNotScale = false;
								newElement.dropShadow = false;
								newElement.fontSize = -1;
								newElement.fontR = 0;
								newElement.fontG = 0;
								newElement.fontB = 0;
								newElement.textColour.R = 0;
								newElement.textColour.G = 0;
								newElement.textColour.B = 0;
								newElement.alignment = TextAlignment_Left;
								newElement.fixedWidth = false;

								newElement.show = true;
								newElement.touched = false;
								newElement.singleTouch = false;

								m_UIElements.push_back(newElement);

							}break;
							case TEXT_TYPE:
							{
								UIElement newElement;
								//SimpleText text;
								math::Vec3 pos( posX, posY, posZ );

								newElement.huiType = huiType;

								int fontId = static_cast<int>( script::LuaGetNumberFromTableItem( "fontId", paramIndex, 0.0 ) );
								int noScale = static_cast<int>( script::LuaGetNumberFromTableItem( "noScale", paramIndex, 0.0 ) );
								int shadow = static_cast<int>( script::LuaGetNumberFromTableItem( "shadow", paramIndex, 0.0 ) );
								int size = static_cast<int>( script::LuaGetNumberFromTableItem( "size", paramIndex, 1.0 ) );
								int r = static_cast<int>( script::LuaGetNumberFromTableItem( "r", paramIndex, 255.0 ) );
								int g = static_cast<int>( script::LuaGetNumberFromTableItem( "g", paramIndex, 255.0 ) );
								int b = static_cast<int>( script::LuaGetNumberFromTableItem( "b", paramIndex, 255.0 ) );
								int align = static_cast<int>( script::LuaGetNumberFromTableItem( "align", paramIndex, 0.0 ) );
								int stringId = static_cast<int>( script::LuaGetNumberFromTableItem( "stringId", paramIndex, -1.0 ) );
								int fixedWidth = static_cast<int>( script::LuaGetNumberFromTableItem( "fixedWidth", paramIndex, 0.0 ) );

								newElement.elementId = huiId;

								newElement.meshAttach1Id = meshAttachId1;
								newElement.meshAttach2Id = meshAttachId2;
								newElement.meshAttach3Id = meshAttachId3;
								newElement.meshAttach4Id = meshAttachId4;
								newElement.meshAttach5Id = meshAttachId5;
								newElement.useNormalScale = useScale;
								newElement.anchorHorizontal = anchorHoriz;
								newElement.anchorVertical = anchorVert;
								newElement.scaleDimsX = scaleDimsX;
								newElement.scaleDimsY = scaleDimsY;

								newElement.selectable = selectable;
								newElement.hightlightOnSelect = highlightOnSelect;

								newElement.numeric1 = numeric1;
								newElement.numeric2 = numeric2;
								newElement.numeric3 = numeric3;
								newElement.numeric4 = numeric4;
								newElement.numeric5 = numeric5;

								newElement.fontId = fontId;
								newElement.doNotScale = noScale;
								newElement.dropShadow = shadow;
								newElement.fontSize = size;
								newElement.fontR = r;
								newElement.fontG = g;
								newElement.fontB = b;
								newElement.textColour.R = r;
								newElement.textColour.G = g;
								newElement.textColour.B = b;
								newElement.alignment = static_cast<TextAlignment>(align);
								newElement.fixedWidth = fixedWidth;

								newElement.areaType = 0;

								// always keep unchanged position
								newElement.origPos = newElement.pos = newElement.screenPos = pos;

								if (newElement.useNormalScale)
								{
									newElement.screenPos.X *= scale.X;
									newElement.screenPos.Y *= scale.Y;
								}
								else
								{
									if (newElement.anchorHorizontal == AnchorHorizontal_Center)
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.screenPos.X = core::app::GetOrientationHeight() * 0.5f;
										else
											newElement.screenPos.X = core::app::GetOrientationWidth() * 0.5f;
									}
									else
									if (newElement.anchorHorizontal == AnchorHorizontal_Left)
									{
										newElement.anchoredPos.X = newElement.screenPos.X;
										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
										newElement.screenPos.X = newElement.anchoredPos.X;
									}
									else
									if (newElement.anchorHorizontal == AnchorHorizontal_Right)
									{
										newElement.anchoredPos.X = (srcAssetDims.Height - newElement.screenPos.X);
										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.screenPos.X = core::app::GetOrientationHeight() - newElement.anchoredPos.X;
										else
											newElement.screenPos.X = core::app::GetOrientationWidth() - newElement.anchoredPos.X;
									}
									else
										if (newElement.anchorHorizontal == AnchorHorizontal_CenterOffset)
									{
										float origHalfWidth = 0.0f;
										float halfWidth = 0.0f;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
										{
											halfWidth = core::app::GetOrientationHeight() * 0.5f;
											origHalfWidth = srcAssetDims.Y * 0.5f;
										}
										else
										{
											halfWidth = core::app::GetOrientationWidth() * 0.5f;
											origHalfWidth = srcAssetDims.Y * 0.5f;
										}

										if (newElement.screenPos.X < origHalfWidth)
										{
											newElement.anchoredPos.X = halfWidth - (origHalfWidth - newElement.screenPos.X);
										}
										else
										if (newElement.screenPos.X > origHalfWidth)
										{
											newElement.anchoredPos.X = halfWidth + (newElement.screenPos.X - origHalfWidth);
										}
										else
										if (newElement.screenPos.X == origHalfWidth)
										{
											newElement.anchoredPos.X = halfWidth;
										}

										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
										newElement.screenPos.X = newElement.anchoredPos.X;
									}

									// vetical
									if (newElement.anchorVertical == AnchorVertical_Center)
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.screenPos.Y = core::app::GetOrientationWidth() * 0.5f;
										else
											newElement.screenPos.Y = core::app::GetOrientationHeight() * 0.5f;
									}
									else
									if (newElement.anchorVertical == AnchorVertical_Top)
									{
										newElement.anchoredPos.Y = (srcAssetDims.Width - newElement.screenPos.Y);
										if (newElement.scaleDimsY)
											newElement.anchoredPos.Y *= scale.Y;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											newElement.screenPos.Y = core::app::GetOrientationWidth() - newElement.anchoredPos.Y;
										else
											newElement.screenPos.Y = core::app::GetOrientationHeight() - newElement.anchoredPos.Y;
									}
									else
									if (newElement.anchorVertical == AnchorVertical_Bottom)
									{
										newElement.anchoredPos.Y = newElement.screenPos.Y;
										if (newElement.scaleDimsY)
											newElement.anchoredPos.Y *= scale.Y;
										newElement.screenPos.Y = newElement.anchoredPos.Y;
									}
									else
									if (newElement.anchorVertical == AnchorVertical_CenterOffset)
									{
										float origHalfHeight = 0.0f;
										float halfHeight = 0.0f;

										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
										{							
											halfHeight = core::app::GetOrientationWidth() * 0.5f;
											origHalfHeight = srcAssetDims.X * 0.5f;
										}
										else
										{
											halfHeight = core::app::GetOrientationHeight() * 0.5f;
											origHalfHeight = srcAssetDims.X * 0.5f;
										}

										if (newElement.screenPos.X < origHalfHeight)
										{
											newElement.anchoredPos.X = halfHeight - (origHalfHeight - newElement.screenPos.X);
										}
										else
										if (newElement.screenPos.X > origHalfHeight)
										{
											newElement.anchoredPos.X = halfHeight + (newElement.screenPos.X - origHalfHeight);
										}
										else
										if (newElement.pos.X == origHalfHeight)
										{
											newElement.anchoredPos.X = halfHeight;
										}

										if (newElement.scaleDimsX)
											newElement.anchoredPos.X *= scale.X;
										newElement.screenPos.X = newElement.anchoredPos.X;
									}

								}

								if( m_Model != 0 )
								{
									math::Vec3 modelScale( 1.0f, 1.0f, 1.0f );

									if (newElement.scaleDimsX || newElement.useNormalScale)
										modelScale.X *= scale.X;
									if (newElement.scaleDimsY || newElement.useNormalScale)
										modelScale.Y *= scale.Y;

									if (newElement.meshAttach1Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach1Id, newElement.screenPos);
										m_Model->SetMeshScale(newElement.meshAttach1Id, modelScale);
									}

									if (newElement.meshAttach2Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach2Id, newElement.screenPos);
										m_Model->SetMeshScale(newElement.meshAttach2Id, modelScale);
									}

									if (newElement.meshAttach3Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach3Id, newElement.screenPos);
										m_Model->SetMeshScale(newElement.meshAttach3Id, modelScale);
									}
									
									if (newElement.meshAttach4Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach4Id, newElement.screenPos);
										m_Model->SetMeshScale(newElement.meshAttach4Id, modelScale);
									}
									
									if (newElement.meshAttach5Id != -1)
									{
										m_Model->SetMeshTranslation(newElement.meshAttach5Id, newElement.screenPos);
										m_Model->SetMeshScale(newElement.meshAttach5Id, modelScale);
									}									
								}

								DBG_ASSERT_MSG((newElement.fontId != -1), "*ERROR* fontId in ui file '%s' is -1", szFilename);

								if (!newElement.doNotScale)
								{
									float currentSize = static_cast<float>(newElement.fontSize);

									if( scale.X > scale.Y )
										currentSize *= scale.Y;
									else
										currentSize *= scale.X;
						
									if( currentSize > 0.5f )
										newElement.fontSize = static_cast<int>(currentSize);
								}
								math::Vec4Lite col(newElement.fontR, newElement.fontG, newElement.fontB, 255);
								const res::FontResourceStore* fontRes = res::GetFontResource(newElement.fontId, newElement.fontSize, (newElement.dropShadow != 0), col, (newElement.fixedWidth != 0));
								newElement.pFontfreetype = fontRes->fontRender;

								// grab the string
								if( stringId != -1 )
								{
									snprintf(newElement.characterBuffer, UI_MAXSTATICTEXTBUFFER_SIZE, "%s", res::GetScriptString(stringId));
								}
								else
									std::memset(newElement.characterBuffer, 0, UI_MAXSTATICTEXTBUFFER_SIZE);

								std::size_t len = std::strlen(newElement.characterBuffer);

								if( (int)len < UI_MAXSTATICTEXTBUFFER_SIZE && (int)len > 0 )
								{
									newElement.currentCharCount = static_cast<unsigned int>(len);

									float xLength = 0.0f;
									int fontSpacing = 0;
									int fontWidth = 0;
									int fontHeight = 0;

									math::Vec2 vDims;
									vDims = newElement.pFontfreetype->GetDimensions();
									fontWidth = static_cast<int>(vDims.Width);
									fontHeight = static_cast<int>(vDims.Height);
									fontSpacing = fontWidth;

									if (newElement.currentCharCount >= 1)
									{
										unsigned int i = 0;
										for (i = 0; i < newElement.currentCharCount; i++)
										{
											// valid buffer
											if (newElement.characterBuffer)
											{
												// check if character is upper or lower case
												if (isupper((int)newElement.characterBuffer[i]))
												{
													// upper case, increase full width
													int nCharWidth = 0;
													newElement.pFontfreetype->GetCharacterWidth(newElement.characterBuffer[i], &nCharWidth);
													xLength += nCharWidth;
												}
												else
												{
													// lower case, increase only by font spacing
													int nCharWidth = 0;
													newElement.pFontfreetype->GetCharacterWidth(newElement.characterBuffer[i], &nCharWidth);
													xLength += nCharWidth;
												}
											}
										}
									}

									if (newElement.alignment == TextAlignment_Left) // left
									{
										newElement.pos.X = newElement.screenPos.X;
										newElement.pos.Y = newElement.screenPos.Y - (vDims.Height*0.25f);

										newElement.aabb.vCenter.X = (newElement.pos.X + xLength*0.5f);
										newElement.aabb.vCenter.Y = newElement.pos.Y;
										newElement.aabb.vCenter.Z = 0.0f;

										newElement.aabb.vBoxMin.X = newElement.pos.X;
										newElement.aabb.vBoxMin.Y = newElement.pos.Y - (vDims.Height*0.25f);
										newElement.aabb.vBoxMin.Z = 0.0f;

										newElement.aabb.vBoxMax.X = newElement.pos.X + xLength;
										newElement.aabb.vBoxMax.Y = newElement.pos.Y + (vDims.Height*0.75f);
										newElement.aabb.vBoxMax.Z = 0.0f;
									}
									else
									if (newElement.alignment == TextAlignment_Center) // center
									{
										newElement.pos.X = newElement.screenPos.X - xLength*0.5f;
										newElement.pos.Y = newElement.screenPos.Y - (vDims.Height*0.25f);

										newElement.aabb.vCenter.X = newElement.screenPos.X;
										newElement.aabb.vCenter.Y = newElement.screenPos.Y;
										newElement.aabb.vCenter.Z = 0.0f;

										newElement.aabb.vBoxMin.X = newElement.screenPos.X - xLength*0.5f;
										newElement.aabb.vBoxMin.Y = newElement.screenPos.Y - (vDims.Height*0.5f);
										newElement.aabb.vBoxMin.Z = 0.0f;

										newElement.aabb.vBoxMax.X = newElement.screenPos.X + xLength*0.5f;
										newElement.aabb.vBoxMax.Y = newElement.screenPos.Y + (vDims.Height*0.5f);
										newElement.aabb.vBoxMax.Z = 0.0f;
									}
									else 
										if (newElement.alignment == TextAlignment_Right) // right
									{
										newElement.pos.X = newElement.screenPos.X - xLength;
										newElement.pos.Y = newElement.screenPos.Y - (vDims.Height*0.25f);

										newElement.aabb.vCenter.X = (newElement.pos.X + xLength*0.5f);
										newElement.aabb.vCenter.Y = newElement.pos.Y;
										newElement.aabb.vCenter.Z = 0.0f;

										newElement.aabb.vBoxMin.X = newElement.pos.X;
										newElement.aabb.vBoxMin.Y = newElement.pos.Y - (vDims.Height*0.25f);
										newElement.aabb.vBoxMin.Z = 0.0f;

										newElement.aabb.vBoxMax.X = newElement.pos.X + xLength;
										newElement.aabb.vBoxMax.Y = newElement.pos.Y + (vDims.Height*0.75f);
										newElement.aabb.vBoxMax.Z = 0.0f;
									}
								}

								newElement.show = true;
								newElement.drawBox = false;
								newElement.boxColour = math::Vec4Lite(0, 0, 0, 255);

								m_UIElements.push_back(newElement);

							}break;

							default:
								DBG_ASSERT(0);
							break;
						} // end switch

					} // end istable

					lua_pop( script::LuaScripting::GetState(), 1 );
				}
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}

		lua_pop( script::LuaScripting::GetState(), 1 );
	} // if loaded

	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	m_DevData = pScriptData->GetDevData();
	m_Loaded = true;

	// apply data
	m_SelectionColour.R = m_DevData.selectionBoxR;
	m_SelectionColour.G = m_DevData.selectionBoxG;
	m_SelectionColour.B = m_DevData.selectionBoxB;

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void UIFileLoader::Update( float touchSize, int numTouchTests, float deltaTime )
{
	if( !m_Loaded )
		return;

	//std::vector<SimpleButton>::iterator buttonIt;
	//std::vector<SimpleText>::iterator textIt;
	std::vector<UIElement>::iterator elementIt;

	// clear touch flag
	elementIt = m_UIElements.begin();
	while (elementIt != m_UIElements.end())
	{
		elementIt->touched = false;
		elementIt->singleTouch = false;
		elementIt->touchIndex = -1;
		elementIt->touchPos = math::Vec3(0.0f, 0.0f, 0.0f);

		elementIt->touchCount = 0;
		elementIt->eventCode = -1;
		elementIt->hover = 0;

		// next
		elementIt++;
	}

	int i=0;
	for( i=0; i < numTouchTests; ++i )
	{		
		const input::TInputState::TouchData* pData = 0;
		pData = input::GetTouch(i);
		
		if( pData->bActive == false )			
			continue;

		math::Vec3 pos = math::Vec3( (float)pData->nTouchX, (float)pData->nTouchY, 0.0f );	
		collision::Sphere touch( pos, touchSize );		

		// check touch flag
		elementIt = m_UIElements.begin();
		while (elementIt != m_UIElements.end())
		{
			if (elementIt->huiType == TEXT_TYPE ||
				elementIt->areaType == 0)
			{
				// test against aabb
				if (touch.SphereCollidesWithAABB(elementIt->aabb))
				{
					if( (pData->bPress) || (pData->bRelease) || (pData->bHeld) )
					{
						elementIt->touched = true;
						elementIt->touchIndex = i;
						elementIt->touchPos = pos;

						if( (pData->bPress) || (pData->bHeld) )
						{
							if(pData->Ticks < input::HOLD_TIME )
							{
								elementIt->singleTouch = true;
								elementIt->eventCode = EVENT_PRESS;
							}
							else
							{
								elementIt->singleTouch = false;
								elementIt->eventCode = EVENT_HELD;
							}
						}
						else if( (pData->bRelease) )
						{
							elementIt->singleTouch = true;
							elementIt->eventCode = EVENT_RELEASE;
						}

						elementIt->touchCount++;
					}

					elementIt->hover = 1;
				}

			}
			else
			{
				// test against sphere
				if (touch.SphereCollidesWithSphere(elementIt->sphere))
				{
					if( (pData->bPress) || (pData->bRelease) || (pData->bHeld) )
					{
						elementIt->touched = true;
						elementIt->touchIndex = i;
						elementIt->touchPos = pos;

						if( (pData->bPress) || (pData->bHeld) )
						{
							if(pData->Ticks <= 0.5f )
							{
								elementIt->singleTouch = true;
								elementIt->eventCode = EVENT_PRESS;
							}
							else
							{
								elementIt->singleTouch = false;
								elementIt->eventCode = EVENT_HELD;
							}
						}
						else if( (pData->bRelease) )
						{
							elementIt->singleTouch = true;
							elementIt->eventCode = EVENT_RELEASE;
						}

						elementIt->touchCount++;
					}

					elementIt->hover = 1;
				}
			}

			// next
			elementIt++;
		}
	}

	if (m_SelectionAnimInc)
	{
		m_SelectionAlpha += 1.0f*deltaTime;
		if (m_SelectionAlpha >= 1.0f)
		{
			m_SelectionAlpha = 1.0f;
			m_SelectionAnimInc = false;
		}
	}
	else
	{
		m_SelectionAlpha -= 1.0f*deltaTime;
		if (m_SelectionAlpha <= 0.0f)
		{
			m_SelectionAlpha = 0.0f;
			m_SelectionAnimInc = true;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void UIFileLoader::Draw()
{
	if( !m_Loaded )
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		// draw BG box
		if (elementIt->drawBox &&
			!core::IsEmptyString(elementIt->characterBuffer))
		{
			math::Vec4Lite origCol = renderer::OpenGL::GetInstance()->GetColour4ub();
			renderer::OpenGL::GetInstance()->SetColour4ub(elementIt->boxColour.R, elementIt->boxColour.G, elementIt->boxColour.B, origCol.A);

			glPushMatrix();
				glTranslatef(elementIt->aabb.vCenter.X, elementIt->aabb.vCenter.Y, elementIt->aabb.vCenter.Z);
				renderer::DrawAABB(elementIt->aabb.vBoxMin, elementIt->aabb.vBoxMax, true);
			glPopMatrix();
		}


		if (elementIt->huiType == TEXT_TYPE &&
			elementIt->show)
		{
			math::Vec4Lite newColour;
			math::Vec4Lite origCol = renderer::OpenGL::GetInstance()->GetColour4ub();

			if ((elementIt->textColour.R != elementIt->fontR) ||
				(elementIt->textColour.G != elementIt->fontG) ||
				(elementIt->textColour.B != elementIt->fontB) ||
				(elementIt->textColour.A != origCol.A))
			{
				newColour.R = elementIt->textColour.R;
				newColour.G = elementIt->textColour.G;
				newColour.B = elementIt->textColour.B;
				newColour.A = elementIt->textColour.A;
			}
			else
			{
				newColour.R = elementIt->fontR;
				newColour.G = elementIt->fontG;
				newColour.B = elementIt->fontB;
				newColour.A = origCol.A;
			}

			elementIt->pFontfreetype->SetBlockFillColour(newColour);
			elementIt->pFontfreetype->Print(static_cast<int>(elementIt->pos.X), static_cast<int>(elementIt->pos.Y), (elementIt->dropShadow != 0), GL_LINEAR, elementIt->characterBuffer);
		}

		if( m_DevData.enableDebugUIDraw )
		{
			// change colour
			const math::Vec4Lite origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
			renderer::OpenGL::GetInstance()->SetColour4ub( 0, 255, 0, 255 );

			if (elementIt->huiType == TEXT_TYPE ||
				elementIt->areaType == 0 )
			{
				renderer::DrawAABB(elementIt->aabb.vBoxMin, elementIt->aabb.vBoxMax);
			}
			else
			{
				glPushMatrix();
					glTranslatef(elementIt->sphere.vCenterPoint.X, elementIt->sphere.vCenterPoint.Y, elementIt->sphere.vCenterPoint.Z);
					renderer::DrawSphere(elementIt->sphere.fRadius);
				glPopMatrix();
			}

			// reset colour
			renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
		}

		// next
		elementIt++;
	}

}

void UIFileLoader::DrawSelected()
{
	if (!core::app::IstvOS())
		return;

	if (m_SelectedElement != 0 &&
		m_SelectedElement->show &&
		m_SelectedElement->hightlightOnSelect)
	{
		glLineWidth(m_DevData.selectionBoxThickness);

		// change colour
		const math::Vec4Lite origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
		renderer::OpenGL::GetInstance()->SetColour4ub(m_SelectionColour.R, m_SelectionColour.G, m_SelectionColour.B, static_cast<GLubyte>(m_SelectionAlpha * 255.0f));
		collision::AABB aabb;
		aabb.vBoxMin = m_SelectedElement->aabb.vBoxMin;
		aabb.vBoxMax = m_SelectedElement->aabb.vBoxMax;

		aabb.vBoxMin.X -= m_DevData.selectionBoxExpand;
		aabb.vBoxMin.Y -= m_DevData.selectionBoxExpand;
		aabb.vBoxMax.X += m_DevData.selectionBoxExpand;
		aabb.vBoxMax.Y += m_DevData.selectionBoxExpand;

		renderer::DrawAABB(aabb.vBoxMin, aabb.vBoxMax);

		// reset colour
		renderer::OpenGL::GetInstance()->SetColour4ub(origColour.R, origColour.G, origColour.B, origColour.A);

		glLineWidth(1.0f);
	}
}

/////////////////////////////////////////////////////
/// Method: CheckElementForTouch
/// Params: [in]elementId, [in]eventCode
///
/////////////////////////////////////////////////////
bool UIFileLoader::CheckElementForTouch( int elementId, EventCode eventCode )
{
	if( !m_Loaded )
		return false;

	std::vector<UIElement>::iterator elementIt;
	
	// get touch flag
	elementIt = m_UIElements.begin();
	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId &&
			elementIt->show)
		{
			if( eventCode == EVENT_HOVER )
				return (elementIt->hover != 0);
			else
				if ((elementIt->eventCode == eventCode) &&
					elementIt->touched)
				return(true);
			else
				return(false);
		}

		// next
		elementIt++;
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: CheckElementForSingleTouch
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
bool UIFileLoader::CheckElementForSingleTouch( int elementId )
{
	if( !m_Loaded )
		return false;

	std::vector<UIElement>::iterator elementIt;

	// get touch flag
	elementIt = m_UIElements.begin();
	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId &&
			elementIt->show)
		{
			if (elementIt->eventCode == EVENT_RELEASE)
				return(elementIt->singleTouch);
		}

		elementIt++;
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: ChangeElementText
/// Params: None
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementText( int elementId, const char* text, ... )
{
	if( !m_Loaded )
		return;

	char szText[UI_MAXSTATICTEXTBUFFER_SIZE];
	std::va_list ap;				

	if( text == 0 )		
		return;				

	va_start( ap, text );					
		std::vsprintf( szText, text, ap );			
	va_end( ap );

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementId == elementIt->elementId)
		{
			std::size_t len = std::strlen( szText );

			if( (int)len < UI_MAXSTATICTEXTBUFFER_SIZE )
			{
				elementIt->currentCharCount = static_cast<unsigned int>(len);
				snprintf(elementIt->characterBuffer, UI_MAXSTATICTEXTBUFFER_SIZE, "%s", szText);

				float xLength = 0.0f;
				int fontSpacing = 0;
				int fontWidth = 0;
				int fontHeight = 0;

				math::Vec2 vDims;
				vDims = elementIt->pFontfreetype->GetDimensions();
				fontWidth = static_cast<int>(vDims.Width);
				fontHeight = static_cast<int>(vDims.Height);
				fontSpacing = fontWidth;

				if (elementIt->currentCharCount >= 1)
				{
					unsigned int i = 0;
					for (i = 0; i < elementIt->currentCharCount; i++)
					{
						// valid buffer
						if (elementIt->characterBuffer)
						{
							// lower case, increase only by font spacing
							int nCharWidth = 0;
							elementIt->pFontfreetype->GetCharacterWidth(elementIt->characterBuffer[i], &nCharWidth);
							xLength += nCharWidth;					
						}
					}
				}

				if (elementIt->alignment == TextAlignment_Left) // left
				{
					elementIt->pos.X = elementIt->screenPos.X;
					elementIt->pos.Y = elementIt->screenPos.Y - (vDims.Height*0.25f);

					//textIt->aabb.vCenter.X = (textIt->pos.X + xLength*0.5f); 
					//textIt->aabb.vCenter.Y = textIt->pos.Y; 
					//textIt->aabb.vCenter.Z = 0.0f; 

					elementIt->aabb.vBoxMin.X = elementIt->pos.X;
					elementIt->aabb.vBoxMin.Y = elementIt->pos.Y - (vDims.Height*0.25f);
					elementIt->aabb.vBoxMin.Z = 0.0f;

					elementIt->aabb.vBoxMax.X = elementIt->pos.X + xLength;
					elementIt->aabb.vBoxMax.Y = elementIt->pos.Y + (vDims.Height*0.75f);
					elementIt->aabb.vBoxMax.Z = 0.0f;
					elementIt->aabb.vCenter = elementIt->aabb.vBoxMax - ((elementIt->aabb.vBoxMax - elementIt->aabb.vBoxMin)*0.5f);
				}
				else
				if (elementIt->alignment == TextAlignment_Center) // center
				{
					elementIt->pos.X = elementIt->screenPos.X - xLength*0.5f;
					elementIt->pos.Y = elementIt->screenPos.Y - (vDims.Height*0.25f);

					//textIt->aabb.vCenter.X = textIt->screenPos.X; 
					//textIt->aabb.vCenter.Y = textIt->screenPos.Y; 
					//textIt->aabb.vCenter.Z = 0.0f; 

					elementIt->aabb.vBoxMin.X = elementIt->screenPos.X - xLength*0.5f;
					elementIt->aabb.vBoxMin.Y = elementIt->screenPos.Y - (vDims.Height*0.5f);
					elementIt->aabb.vBoxMin.Z = 0.0f;

					elementIt->aabb.vBoxMax.X = elementIt->screenPos.X + xLength*0.5f;
					elementIt->aabb.vBoxMax.Y = elementIt->screenPos.Y + (vDims.Height*0.5f);
					elementIt->aabb.vBoxMax.Z = 0.0f;
					elementIt->aabb.vCenter = elementIt->aabb.vBoxMax - ((elementIt->aabb.vBoxMax - elementIt->aabb.vBoxMin)*0.5f);
				}
				else
				if (elementIt->alignment == TextAlignment_Right) // right
				{
					elementIt->pos.X = elementIt->screenPos.X - xLength;
					elementIt->pos.Y = elementIt->screenPos.Y - (vDims.Height*0.25f);

					//textIt->aabb.vCenter.X = (textIt->pos.X + xLength*0.5f); 
					//textIt->aabb.vCenter.Y = textIt->pos.Y; 
					//textIt->aabb.vCenter.Z = 0.0f; 

					elementIt->aabb.vBoxMin.X = elementIt->pos.X;
					elementIt->aabb.vBoxMin.Y = elementIt->pos.Y - (vDims.Height*0.25f);
					elementIt->aabb.vBoxMin.Z = 0.0f;

					elementIt->aabb.vBoxMax.X = elementIt->pos.X + xLength;
					elementIt->aabb.vBoxMax.Y = elementIt->pos.Y + (vDims.Height*0.75f);
					elementIt->aabb.vBoxMax.Z = 0.0f;
					elementIt->aabb.vCenter = elementIt->aabb.vBoxMax - ((elementIt->aabb.vBoxMax - elementIt->aabb.vBoxMin)*0.5f);
				}
			}
			return;
		}

		// next
		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementTextBoxState
/// Params: [in]elementId, [in]state
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementTextBoxState( int elementId, bool state )
{
	if( !m_Loaded )
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementId == elementIt->elementId)
		{
			elementIt->drawBox = state;
			return;
		}

		// next
		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementTextColour
/// Params: [in]elementId, [in]col
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementTextColour( int elementId, const math::Vec4Lite& col )
{
	if( !m_Loaded )
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementId == elementIt->elementId)
		{
			elementIt->textColour = col;
			return;
		}

		// next
		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementTextBoxColour
/// Params: [in]elementId, [in]col
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementTextBoxColour( int elementId, const math::Vec4Lite& col )
{
	if( !m_Loaded )
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementId == elementIt->elementId)
		{
			elementIt->boxColour = col;
			return;
		}

		// next
		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementTextShadowState
/// Params: [in]elementId, [in]state
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementTextShadowState(int elementId, bool state)
{
	if (!m_Loaded)
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementId == elementIt->elementId)
		{
			elementIt->dropShadow = state;
			return;
		}

		// next
		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementTextAlignment
/// Params: [in]elementId, [in]alignment
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementTextAlignment(int elementId, UIFileLoader::TextAlignment alignment )
{
    if (!m_Loaded)
        return;
    
    std::vector<UIElement>::iterator elementIt;
    elementIt = m_UIElements.begin();
    
    while (elementIt != m_UIElements.end())
    {
        if (elementIt->huiType == TEXT_TYPE &&
            elementId == elementIt->elementId)
        {
            elementIt->alignment = alignment;
            ChangeElementText(elementId, elementIt->characterBuffer);
            return;
        }
        
        // next
        elementIt++;
    }
}

/////////////////////////////////////////////////////
/// Method: ChangeElementDrawState
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementDrawState( int elementId, bool state )
{
	if (!m_Loaded)
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	elementIt = m_UIElements.begin();
	while (elementIt != m_UIElements.end())
	{
		if (elementId == elementIt->elementId)
		{
			elementIt->show = state;
			return;
		}
		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementDrawState( bool state )
{
	if (!m_Loaded)
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	elementIt = m_UIElements.begin();
	while (elementIt != m_UIElements.end())
	{
		elementIt->show = state;

		elementIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementPosition
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementPosition( int elementId, const math::Vec3& pos )
{
	if( !m_Loaded )
		return;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{

			if (elementIt->huiType == BUTTON_TYPE)
			{
				if (elementIt->areaType == 0)
				{
					elementIt->pos = pos;

					elementIt->aabb.vCenter = pos;
					elementIt->aabb.vBoxMin = pos - elementIt->dims;
					elementIt->aabb.vBoxMax = pos + elementIt->dims;
				}
				else
				{
					elementIt->sphere.vCenterPoint = pos;
				}
			}
			else
			{
				elementIt->screenPos.X = pos.X;
				elementIt->screenPos.Y = pos.Y;

				ChangeElementText(elementId, elementIt->characterBuffer);
			}

			if( m_Model != 0 )
			{
				if (elementIt->meshAttach1Id != -1)
					m_Model->SetMeshTranslation(elementIt->meshAttach1Id, pos);

				if (elementIt->meshAttach2Id != -1)
					m_Model->SetMeshTranslation(elementIt->meshAttach2Id, pos);

				if (elementIt->meshAttach3Id != -1)
					m_Model->SetMeshTranslation(elementIt->meshAttach3Id, pos);

				if (elementIt->meshAttach4Id != -1)
					m_Model->SetMeshTranslation(elementIt->meshAttach4Id, pos);
			
				if (elementIt->meshAttach5Id != -1)
					m_Model->SetMeshTranslation(elementIt->meshAttach5Id, pos);
			}
			return;
		}

		// next
		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );
}

/////////////////////////////////////////////////////
/// Method: GetElementPosition
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const math::Vec3& UIFileLoader::GetElementPosition( int elementId )
{
	if( !m_Loaded )
		return BadPos;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			if (elementIt->areaType == 0 ||
				elementIt->huiType == TEXT_TYPE)
			{
				return(elementIt->aabb.vCenter);
			}
			else
			{
				return(elementIt->sphere.vCenterPoint);
			}
		}

		// next
		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(BadPos);
}

/////////////////////////////////////////////////////
/// Method: GetElementPositionNonAABB
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const math::Vec3& UIFileLoader::GetElementPositionNonAABB( int elementId )
{
	if( !m_Loaded )
		return BadPos;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			if (elementIt->huiType == BUTTON_TYPE)
			{
				if (elementIt->areaType == 0)
				{
					return(elementIt->pos);
				}
				else
				{
					return(elementIt->pos);
				}
			}
			else
			{
				return(elementIt->screenPos);
			}
		}

		// next
		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(BadPos);
}

/////////////////////////////////////////////////////
/// Method: GetLastTouchPosition
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const math::Vec3& UIFileLoader::GetLastTouchPosition( int elementId )
{
	if( !m_Loaded )
		return BadPos;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			return(elementIt->touchPos);
		}

		// next
		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(BadPos);
}

/////////////////////////////////////////////////////
/// Method: GetLastTouchIndex
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const int UIFileLoader::GetLastTouchIndex( int elementId )
{
	if( !m_Loaded )
		return -1;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			return(elementIt->touchIndex);
		}

		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(-1);
}

/////////////////////////////////////////////////////
/// Method: GetElementTouchCount
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const int UIFileLoader::GetElementTouchCount( int elementId )
{
	if( !m_Loaded )
		return -1;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			return(elementIt->touchCount);
		}

		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(-1);
}

/////////////////////////////////////////////////////
/// Method: GetElement
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const UIFileLoader::UIElement* UIFileLoader::GetElement( int elementId )
{
	if( !m_Loaded )
		return 0;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			return(&(*elementIt));
		}

		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: GetElementText
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const char* UIFileLoader::GetElementText( int elementId )
{
	if( !m_Loaded )
		return 0;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementIt->elementId == elementId)
		{
			return(&elementIt->characterBuffer[0]);
		}

		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: GetElementTextColour
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
math::Vec4Lite& UIFileLoader::GetElementTextColour( int elementId )
{
	if( !m_Loaded )
		return defaultColour;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->huiType == TEXT_TYPE &&
			elementIt->elementId == elementId)
		{
			return elementIt->textColour;
		}

		elementIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return defaultColour;
}

/////////////////////////////////////////////////////
/// Method: GetElementAABB
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
const collision::AABB& UIFileLoader::GetElementAABB(int elementId)
{
	math::Vec3 aabbMin(-1.0f, -1.0f, 0.0f);
	math::Vec3 aabbMax(1.0f, 1.0f, 0.0f);

	BadAABB.vBoxMin = aabbMin;
	BadAABB.vBoxMax = aabbMax;

	if (!m_Loaded)
		return BadAABB;

	std::vector<UIElement>::iterator elementIt;
	elementIt = m_UIElements.begin();

	while (elementIt != m_UIElements.end())
	{
		if (elementIt->elementId == elementId)
		{
			return(elementIt->aabb);
		}

		// next
		elementIt++;
	}

	DBG_ASSERT_MSG(0, "*ERROR* Element id does not exist in UI");

	return(BadAABB);
}

/////////////////////////////////////////////////////
/// Method: RefreshSelected
/// Params: None
///
/////////////////////////////////////////////////////
void UIFileLoader::RefreshSelected(int selectElement)
{
	if (!m_Loaded)
		return;

	if (!core::app::IstvOS())
		return;

	int lastSelected = 9999;
	m_SelectedElement = 0;

	std::size_t i = 0;
	bool foundSelected = false;

	for (i = 0; i != m_UIElements.size(); ++i)
	{
		if (m_UIElements[i].numeric1 != -9999)
		{
			// want the lowest selectable
			if (m_UIElements[i].show &&
				m_UIElements[i].selectable)
			{
				if (selectElement != -1)
				{
					if (m_UIElements[i].elementId == selectElement)
					{
						m_SelectedElement = &m_UIElements[i];
						foundSelected = true;
					}
				}

				if (!foundSelected)
				{
					if (m_UIElements[i].numeric1 < lastSelected)
					{
						lastSelected = m_UIElements[i].numeric1;
						m_SelectedElement = &m_UIElements[i];
					}
				}
			}
		}
	}

	if (m_SelectedElement)
	{
		input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedElement->pos.X);
		input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedElement->pos.Y);
	}
}

/////////////////////////////////////////////////////
/// Method: FindNearestElementToSelection
/// Params: None
///
/////////////////////////////////////////////////////
UIFileLoader::UIElement* UIFileLoader::FindNearestElementToSelection(int selectionIndex, int minValue, int maxValue, bool nextInc)
{
	std::size_t i = 0;
	std::vector<UIElement*> rangeList;

	// find the buttons within the range
	for (i = 0; i != m_UIElements.size(); ++i)
	{
		if (m_UIElements[i].numeric1 != -9999)
		{
			if (m_UIElements[i].show &&
				m_UIElements[i].selectable &&
				m_SelectedElement != &m_UIElements[i])
			{
				if (m_UIElements[i].numeric1 >= minValue &&
					m_UIElements[i].numeric1 < maxValue)
				{
					rangeList.push_back(&m_UIElements[i]);
				}
			}
		}
	}

	// nothing in range
	if (rangeList.size() == 0)
		return 0;

	// something in range
	for (i = 0; i != rangeList.size(); ++i)
	{
		// exact match
		if (rangeList[i]->numeric1 == selectionIndex)
			return rangeList[i];
	}

	// get a larger or smaller
	std::size_t bestIndex = -1;
	int smallestDifference = 9999;

	if (nextInc)
	{
		for (i = 0; i != rangeList.size(); ++i)
		{
			// prefer bigger
			if (rangeList[i]->numeric1 > selectionIndex)
			{
				if ((rangeList[i]->numeric1 - selectionIndex) < smallestDifference)
				{
					smallestDifference = (rangeList[i]->numeric1 - selectionIndex);
					bestIndex = i;
				}
			}
		}
	}
	else
	{

		for (i = 0; i != rangeList.size(); ++i)
		{
			// prefer smaller
			if (rangeList[i]->numeric1 < selectionIndex)
			{
				if ((selectionIndex - rangeList[i]->numeric1) < smallestDifference)
				{
					smallestDifference = (selectionIndex - rangeList[i]->numeric1);
					bestIndex = i;
				}
			}
		}
	}

	if (bestIndex != -1)
		return rangeList[bestIndex];

	return 0;
}

/////////////////////////////////////////////////////
/// Method: SelectNextColumn
/// Params: None
///
/////////////////////////////////////////////////////
bool UIFileLoader::SelectNextColumn()
{
	if (!core::app::IstvOS())
		return false;

	if (m_SelectedElement != 0)
	{
		bool notFound = true;

		// what value was the current column
		int originalColumn = m_SelectedElement->numeric1;
		int currentColumn = originalColumn;

		// need to calculate the min/max between a range of 10
		if (originalColumn == 0 ||
			((originalColumn / 10) != 0))
			originalColumn += 1;

		int minValue = RoundDown(originalColumn);
		int maxValue = RoundUp(originalColumn);

		if (originalColumn == 0)
			maxValue = 10;

		while (notFound)
		{
			currentColumn += 1;

			UIElement* nextElement = FindNearestElementToSelection(currentColumn, minValue, maxValue, true);

			if (nextElement != 0)
			{
				// something in range
				m_SelectedElement = nextElement;

				if (m_SelectedElement)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedElement->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedElement->pos.Y);
				}
				return true;
			}

			if (currentColumn >= maxValue)
				notFound = false;
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: SelectPreviousColumn
/// Params: None
///
/////////////////////////////////////////////////////
bool UIFileLoader::SelectPreviousColumn()
{
	if (!core::app::IstvOS())
		return false;

	if (m_SelectedElement != 0)
	{
		bool notFound = true;

		// what value was the current column
		int originalColumn = m_SelectedElement->numeric1;
		int currentColumn = originalColumn;

		// need to calculate the min/max between a range of 10
		if (originalColumn == 0 ||
			((originalColumn / 10) != 0))
			originalColumn += 1;

		int minValue = RoundDown(originalColumn);
		int maxValue = RoundUp(originalColumn);

		while (notFound)
		{
			currentColumn -= 1;

			UIElement* prevElement = FindNearestElementToSelection(currentColumn, minValue, maxValue, false);

			if (prevElement != 0)
			{
				// something in range
				m_SelectedElement = prevElement;

				if (m_SelectedElement)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedElement->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedElement->pos.Y);
				}
				return true;
			}

			if (currentColumn <= minValue)
				notFound = false;
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: RefreshSelected
/// Params: None
///
/////////////////////////////////////////////////////
bool UIFileLoader::SelectNextRow()
{
	if (!core::app::IstvOS())
		return false;

	if (m_SelectedElement != 0)
	{
		bool notFound = true;

		// what value was the current row
		int originalRow = m_SelectedElement->numeric1;
		int currentRow = originalRow;

		// need to calculate the min/max between a range of 10
		if (originalRow == 0 ||
			((originalRow / 10) != 0))
			originalRow += 1;

		int minValue = RoundDown(originalRow + 10);
		int maxValue = RoundUp(originalRow + 10);

		while (notFound)
		{
			currentRow += 10;

			UIElement* nextElement = FindNearestElementToSelection(currentRow, minValue, maxValue, true);

			if (nextElement != 0)
			{
				// something in range
				m_SelectedElement = nextElement;

				if (m_SelectedElement)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedElement->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedElement->pos.Y);
				}
				return true;
			}



			minValue += 10;
			maxValue += 10;

			if (currentRow >= 1000)
				notFound = false;
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: SelectPreviousRow
/// Params: None
///
/////////////////////////////////////////////////////
bool UIFileLoader::SelectPreviousRow()
{
	if (!core::app::IstvOS())
		return false;

	if (m_SelectedElement != 0)
	{
		bool notFound = true;

		// what value was the current row
		int originalRow = m_SelectedElement->numeric1;
		int currentRow = originalRow;

		// need to calculate the min/max between a range of 10
		if (originalRow == 0 ||
			((originalRow / 10) != 0))
			originalRow += 1;

		int minValue = RoundDown(std::abs(originalRow - 10));
		int maxValue = RoundUp(std::abs(originalRow - 10));

		if ((originalRow - 10) <= 0)
			maxValue = 10;

		while (notFound)
		{
			currentRow -= 10;

			UIElement* nextElement = FindNearestElementToSelection(currentRow, minValue, maxValue, false);

			if (nextElement != 0)
			{
				// something in range
				m_SelectedElement = nextElement;

				if (m_SelectedElement)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedElement->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedElement->pos.Y);
				}
				return true;
			}

			minValue -= 10;
			maxValue -= 10;

			if (currentRow <= 0)
				notFound = false;
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: SelectionPress
/// Params: None
///
/////////////////////////////////////////////////////
void UIFileLoader::SelectionPress(bool state)
{
	if (!core::app::IstvOS())
		return;

	if (state &&
		!input::gInputState.TouchesData[input::SECOND_TOUCH].bActive)
	{
		input::gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
		input::gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2(0.0f, 0.0f);
	}

	if (!state &&
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress)
	{
		input::gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].Ticks = 0.0f;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress = false;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bHeld = false;
	}
}
