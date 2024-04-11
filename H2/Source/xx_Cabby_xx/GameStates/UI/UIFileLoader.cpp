
/*===================================================================
	File: UIFileLoader.cpp
	Game: Cabby

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
	collision::AABB BadAABB;
}

int RoundUp(int value)
{
	return 10 * ((value + 9) / 10);
}

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
	m_SelectedButton = 0;

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
	m_Buttons.clear();
	m_Text.clear();
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
	m_SelectedButton = 0;
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
								numeric1 != -9999 )
							{
								m_HasSelectableElements = true;
							}
						}

						switch( huiType )
						{
							case BUTTON_TYPE:
							{
								SimpleButton button;
								math::Vec3 pos( posX, posY, posZ );
								
								int buttonType = static_cast<int>( script::LuaGetNumberFromTableItem( "buttonType", paramIndex, 0.0 ) );
								float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
								float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
								float r = static_cast<float>( script::LuaGetNumberFromTableItem( "r", paramIndex, 1.0 ) );

								button.elementId = huiId;
								button.areaType = buttonType;
								button.meshAttach1Id = meshAttachId1;
								button.meshAttach2Id = meshAttachId2;
								button.meshAttach3Id = meshAttachId3;
								button.meshAttach4Id = meshAttachId4;
								button.meshAttach5Id = meshAttachId5;

								button.selectable = selectable;
								button.hightlightOnSelect = highlightOnSelect;

								button.numeric1 = numeric1;
								button.numeric2 = numeric2;
								button.numeric3 = numeric3;
								button.numeric4 = numeric4;
								button.numeric5 = numeric5;

								button.useNormalScale = useScale;
								button.anchorHorizontal = anchorHoriz;
								button.anchorVertical = anchorVert;
								button.scaleDimsX = scaleDimsX;
								button.scaleDimsY = scaleDimsY;
								button.dims.Width = w*0.5f;
								button.dims.Height = h*0.5f;

								// always keep unchanged position
								button.origPos = button.pos = pos;

								// scale the types
								if( button.useNormalScale )
								{
									button.pos.X *= scale.X;
									button.pos.Y *= scale.Y;
								}
								else
								{
									// check alignments
									if( button.anchorHorizontal == AnchorHorizontal_Center )
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											button.pos.X = core::app::GetOrientationHeight() * 0.5f;							
										else
											button.pos.X = core::app::GetOrientationWidth() * 0.5f;
									}
									else
									if( button.anchorHorizontal == AnchorHorizontal_Left )
									{
										button.anchoredPos.X = button.pos.X;
										if( button.scaleDimsX )
											button.anchoredPos.X *= scale.X;
										button.pos.X = button.anchoredPos.X;
									}
									else
									if( button.anchorHorizontal == AnchorHorizontal_Right )
									{
										button.anchoredPos.X = (srcAssetDims.Height - button.pos.X);
										if( button.scaleDimsX )
											button.anchoredPos.X *= scale.X;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											button.pos.X = core::app::GetOrientationHeight() - button.anchoredPos.X;
										else								
											button.pos.X = core::app::GetOrientationWidth() - button.anchoredPos.X;
									}
									else
									if( button.anchorHorizontal == AnchorHorizontal_CenterOffset )
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

										if( button.pos.X < origHalfWidth )
										{
											button.anchoredPos.X = halfWidth - (origHalfWidth - button.pos.X);
										}
										else
										if( button.pos.X > origHalfWidth )
										{
											button.anchoredPos.X = halfWidth + (button.pos.X - origHalfWidth);
										}
										else
										if( button.pos.X == origHalfWidth )
										{
											button.pos.X = halfWidth;
										}

										if( button.scaleDimsX )
											button.anchoredPos.X *= scale.X;
										button.pos.X = button.anchoredPos.X;
									}

									// vertical
									if( button.anchorVertical == AnchorVertical_Center )
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											button.pos.Y = core::app::GetOrientationWidth() * 0.5f;
										else
											button.pos.Y = core::app::GetOrientationHeight() * 0.5f;
									}
									else
									if( button.anchorVertical == AnchorVertical_Top )
									{
										button.anchoredPos.Y = (srcAssetDims.Width - button.pos.Y);
										if( button.scaleDimsY )
											button.anchoredPos.Y *= scale.Y;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											button.pos.Y = core::app::GetOrientationWidth() - button.anchoredPos.Y;
										else
											button.pos.Y = core::app::GetOrientationHeight() - button.anchoredPos.Y;
									}
									else
									if( button.anchorVertical == AnchorVertical_Bottom )
									{
										button.anchoredPos.Y = button.pos.Y;
										if( button.scaleDimsY )
											button.anchoredPos.Y *= scale.Y;
										button.pos.Y = button.anchoredPos.Y;
									}
									else
									if( button.anchorVertical == AnchorVertical_CenterOffset )
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

										if( button.pos.Y < origHalfHeight )
										{
											button.anchoredPos.Y = halfHeight - (origHalfHeight - button.pos.Y);
										}
										else
										if( button.pos.Y > origHalfHeight )
										{
											button.anchoredPos.Y = halfHeight + (button.pos.Y - origHalfHeight);
										}
										else
										if( button.pos.Y == origHalfHeight )
										{
											button.pos.Y = halfHeight;
										}

										if( button.scaleDimsY )
											button.anchoredPos.Y *= scale.Y;
										button.pos.Y = button.anchoredPos.Y;
									}
								}

								// scale dimensions
								if( button.scaleDimsX || button.useNormalScale )
								{
									//button.pos.X *= scale.X;
									button.dims.Width *= scale.X;
								}
								if( button.scaleDimsY || button.useNormalScale )
								{
									//button.pos.Y *= scale.Y;
									button.dims.Height *= scale.Y;
								}

								// create the aabb
								button.aabb.vCenter = button.pos;
								button.aabb.vBoxMin = button.pos - button.dims;
								button.aabb.vBoxMax = button.pos + button.dims;

								if( m_Model != 0 )
								{
									math::Vec3 modelScale( 1.0f, 1.0f, 1.0f );

									if( button.scaleDimsX || button.useNormalScale )
										modelScale.X *= scale.X;
									if( button.scaleDimsY || button.useNormalScale )
										modelScale.Y *= scale.Y;

									if( button.meshAttach1Id != -1 )
									{
										m_Model->SetMeshTranslation( button.meshAttach1Id, button.pos );
										m_Model->SetMeshScale( button.meshAttach1Id, modelScale );
									}

									if( button.meshAttach2Id != -1 )
									{
										m_Model->SetMeshTranslation( button.meshAttach2Id, button.pos );
										m_Model->SetMeshScale( button.meshAttach2Id, modelScale );
									}

									if( button.meshAttach3Id != -1 )
									{
										m_Model->SetMeshTranslation( button.meshAttach3Id, button.pos );
										m_Model->SetMeshScale( button.meshAttach3Id, modelScale );
									}
									
									if( button.meshAttach4Id != -1 )
									{
										m_Model->SetMeshTranslation( button.meshAttach4Id, button.pos );
										m_Model->SetMeshScale( button.meshAttach4Id, modelScale );
									}
									
									if( button.meshAttach5Id != -1 )
									{
										m_Model->SetMeshTranslation( button.meshAttach5Id, button.pos );
										m_Model->SetMeshScale( button.meshAttach5Id, modelScale );
									}									
								}

								button.sphere.fRadius = r;

								// scale the types
								if( scale.X > scale.Y )
								{
									if( button.scaleDimsX || button.useNormalScale)
										button.sphere.fRadius *= scale.X;
								}
								else
								{
									if( button.scaleDimsY || button.useNormalScale)
										button.sphere.fRadius *= scale.Y;
								}

								button.sphere.vCenterPoint = button.pos;

								button.show = true;
								button.touched = false;
								button.singleTouch = false;

								m_Buttons.push_back(button);

							}break;
							case TEXT_TYPE:
							{
								SimpleText text;
								math::Vec3 pos( posX, posY, posZ );

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

								text.elementId = huiId;

								text.meshAttach1Id = meshAttachId1;
								text.meshAttach2Id = meshAttachId2;
								text.meshAttach3Id = meshAttachId3;
								text.meshAttach4Id = meshAttachId4;
								text.meshAttach5Id = meshAttachId5;
								text.useNormalScale = useScale;
								text.anchorHorizontal = anchorHoriz;
								text.anchorVertical = anchorVert;
								text.scaleDimsX = scaleDimsX;
								text.scaleDimsY = scaleDimsY;

								text.selectable = selectable;
								text.hightlightOnSelect = highlightOnSelect;

								text.numeric1 = numeric1;
								text.numeric2 = numeric2;
								text.numeric3 = numeric3;
								text.numeric4 = numeric4;
								text.numeric5 = numeric5;

								text.fontId = fontId;
								text.doNotScale = noScale;
								text.dropShadow = shadow;
								text.fontSize = size;
								text.fontR = r;
								text.fontG = g;
								text.fontB = b;
								text.alignment = static_cast<TextAlignment>(align);
								text.fixedWidth = fixedWidth;

								// always keep unchanged position
								text.origPos = text.screenPos = pos;

								if( text.useNormalScale )
								{
									text.screenPos.X *= scale.X;
									text.screenPos.Y *= scale.Y;
								}
								else
								{
									if( text.anchorHorizontal == AnchorHorizontal_Center )
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											text.screenPos.X = core::app::GetOrientationHeight() * 0.5f;
										else
											text.screenPos.X = core::app::GetOrientationWidth() * 0.5f;
									}
									else
									if( text.anchorHorizontal == AnchorHorizontal_Left )
									{
										text.anchoredPos.X = text.screenPos.X;
										if( text.scaleDimsX )
											text.anchoredPos.X *= scale.X;
										text.screenPos.X = text.anchoredPos.X;
									}
									else
									if( text.anchorHorizontal == AnchorHorizontal_Right )
									{
										text.anchoredPos.X = (srcAssetDims.Height - text.screenPos.X);
										if( text.scaleDimsX )
											text.anchoredPos.X *= scale.X;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											text.screenPos.X = core::app::GetOrientationHeight() - text.anchoredPos.X;
										else
											text.screenPos.X = core::app::GetOrientationWidth() - text.anchoredPos.X;
									}
									else
									if( text.anchorHorizontal == AnchorHorizontal_CenterOffset )
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

										if( text.screenPos.X < origHalfWidth )
										{
											text.anchoredPos.X = halfWidth - (origHalfWidth - text.screenPos.X);
										}
										else
										if( text.screenPos.X > origHalfWidth )
										{
											text.anchoredPos.X = halfWidth + (text.screenPos.X - origHalfWidth);
										}
										else
										if( text.screenPos.X == origHalfWidth )
										{
											text.anchoredPos.X = halfWidth;
										}

										if( text.scaleDimsX )
											text.anchoredPos.X *= scale.X;
										text.screenPos.X = text.anchoredPos.X;
									}

									// vetical
									if( text.anchorVertical == AnchorVertical_Center )
									{
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											text.screenPos.Y = core::app::GetOrientationWidth() * 0.5f;
										else
											text.screenPos.Y = core::app::GetOrientationHeight() * 0.5f;
									}
									else
									if( text.anchorVertical == AnchorVertical_Top )
									{
										text.anchoredPos.Y = (srcAssetDims.Width - text.screenPos.Y);
										if( text.scaleDimsY )
											text.anchoredPos.Y *= scale.Y;
						
										if( renderer::OpenGL::GetInstance()->GetIsRotated() )
											text.screenPos.Y = core::app::GetOrientationWidth() - text.anchoredPos.Y;
										else
											text.screenPos.Y = core::app::GetOrientationHeight() - text.anchoredPos.Y;
									}
									else
									if( text.anchorVertical == AnchorVertical_Bottom )
									{
										text.anchoredPos.Y = text.screenPos.Y;
										if( text.scaleDimsY )
											text.anchoredPos.Y *= scale.Y;
										text.screenPos.Y = text.anchoredPos.Y;
									}
									else
									if( text.anchorVertical == AnchorVertical_CenterOffset )
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

										if( text.screenPos.X < origHalfHeight )
										{
											text.anchoredPos.X = halfHeight - (origHalfHeight - text.screenPos.X);
										}
										else
										if( text.screenPos.X > origHalfHeight )
										{
											text.anchoredPos.X = halfHeight + (text.screenPos.X - origHalfHeight);
										}
										else
										if( text.pos.X == origHalfHeight )
										{
											text.anchoredPos.X = halfHeight;
										}

										if( text.scaleDimsX )
											text.anchoredPos.X *= scale.X;
										text.screenPos.X = text.anchoredPos.X;
									}

								}

								if( m_Model != 0 )
								{
									math::Vec3 modelScale( 1.0f, 1.0f, 1.0f );

									if( text.scaleDimsX || text.useNormalScale )
										modelScale.X *= scale.X;
									if( text.scaleDimsY || text.useNormalScale )
										modelScale.Y *= scale.Y;

									if( text.meshAttach1Id != -1 )
									{
										m_Model->SetMeshTranslation( text.meshAttach1Id, text.screenPos );
										m_Model->SetMeshScale( text.meshAttach1Id, modelScale );
									}

									if( text.meshAttach2Id != -1 )
									{
										m_Model->SetMeshTranslation( text.meshAttach2Id, text.screenPos );
										m_Model->SetMeshScale( text.meshAttach2Id, modelScale );
									}

									if( text.meshAttach3Id != -1 )
									{
										m_Model->SetMeshTranslation( text.meshAttach3Id, text.screenPos );
										m_Model->SetMeshScale( text.meshAttach3Id, modelScale );
									}
									
									if( text.meshAttach4Id != -1 )
									{
										m_Model->SetMeshTranslation( text.meshAttach4Id, text.screenPos );
										m_Model->SetMeshScale( text.meshAttach4Id, modelScale );
									}
									
									if( text.meshAttach5Id != -1 )
									{
										m_Model->SetMeshTranslation( text.meshAttach5Id, text.screenPos );
										m_Model->SetMeshScale( text.meshAttach5Id, modelScale );
									}									
								}

								DBG_ASSERT_MSG( (text.fontId != -1), "*ERROR* fontId in ui file '%s' is -1", szFilename );

								if( !text.doNotScale )
								{
									float currentSize = static_cast<float>(text.fontSize);

									if( scale.X > scale.Y )
										currentSize *= scale.Y;
									else
										currentSize *= scale.X;
						
									if( currentSize > 0.5f )
										text.fontSize = static_cast<int>(currentSize);
								}
								math::Vec4Lite col( text.fontR, text.fontG, text.fontB, 255 );
								const res::FontResourceStore* fontRes = res::GetFontResource( text.fontId, text.fontSize, (text.dropShadow != 0), col, (text.fixedWidth!=0) );
								text.pFontfreetype = fontRes->fontRender;

								// grab the string
								if( stringId != -1 )
								{
									snprintf( text.characterBuffer, UI_MAXSTATICTEXTBUFFER_SIZE, "%s", res::GetScriptString( stringId ) );
								}
								else
									std::memset( text.characterBuffer, 0, UI_MAXSTATICTEXTBUFFER_SIZE );

								std::size_t len = std::strlen( text.characterBuffer );

								if( (int)len < UI_MAXSTATICTEXTBUFFER_SIZE && (int)len > 0 )
								{
									text.currentCharCount = static_cast<unsigned int>(len);

									float xLength = 0.0f;
									int fontSpacing = 0;
									int fontWidth = 0;
									int fontHeight = 0;

									math::Vec2 vDims;
									vDims = text.pFontfreetype->GetDimensions();
									fontWidth = static_cast<int>(vDims.Width);
									fontHeight = static_cast<int>(vDims.Height);
									fontSpacing = fontWidth;

									if( text.currentCharCount >= 1 )
									{
										unsigned int i = 0;
										for( i = 0; i < text.currentCharCount; i++ )
										{
											// valid buffer
											if( text.characterBuffer )
											{
												// check if character is upper or lower case
												if( isupper( (int)text.characterBuffer[i] ) )
												{
													// upper case, increase full width
													int nCharWidth = 0;
													text.pFontfreetype->GetCharacterWidth( text.characterBuffer[i], &nCharWidth );
													xLength += nCharWidth;
												}
												else
												{
													// lower case, increase only by font spacing
													int nCharWidth = 0;
													text.pFontfreetype->GetCharacterWidth( text.characterBuffer[i], &nCharWidth );
													xLength += nCharWidth;
												}
											}
										}
									}

									if( text.alignment == TextAlignment_Left ) // left
									{
										text.pos.X = text.screenPos.X;
										text.pos.Y = text.screenPos.Y - (vDims.Height*0.25f);

										text.aabb.vCenter.X = (text.pos.X + xLength*0.5f); 
										text.aabb.vCenter.Y = text.pos.Y; 
										text.aabb.vCenter.Z = 0.0f; 

										text.aabb.vBoxMin.X = text.pos.X;
										text.aabb.vBoxMin.Y = text.pos.Y - (vDims.Height*0.25f);
										text.aabb.vBoxMin.Z = 0.0f;

										text.aabb.vBoxMax.X = text.pos.X + xLength;
										text.aabb.vBoxMax.Y = text.pos.Y + (vDims.Height*0.75f);
										text.aabb.vBoxMax.Z = 0.0f;
									}
									else
									if( text.alignment == TextAlignment_Center ) // center
									{
										text.pos.X = text.screenPos.X - xLength*0.5f;
										text.pos.Y = text.screenPos.Y - (vDims.Height*0.25f);

										text.aabb.vCenter.X = text.screenPos.X; 
										text.aabb.vCenter.Y = text.screenPos.Y; 
										text.aabb.vCenter.Z = 0.0f; 

										text.aabb.vBoxMin.X = text.screenPos.X - xLength*0.5f;
										text.aabb.vBoxMin.Y = text.screenPos.Y - (vDims.Height*0.5f);
										text.aabb.vBoxMin.Z = 0.0f;

										text.aabb.vBoxMax.X = text.screenPos.X + xLength*0.5f;
										text.aabb.vBoxMax.Y = text.screenPos.Y + (vDims.Height*0.5f);
										text.aabb.vBoxMax.Z = 0.0f;
									}
									else 
									if( text.alignment == TextAlignment_Right ) // right
									{
										text.pos.X = text.screenPos.X - xLength;
										text.pos.Y = text.screenPos.Y - (vDims.Height*0.25f);

										text.aabb.vCenter.X = (text.pos.X + xLength*0.5f); 
										text.aabb.vCenter.Y = text.pos.Y; 
										text.aabb.vCenter.Z = 0.0f; 

										text.aabb.vBoxMin.X = text.pos.X;
										text.aabb.vBoxMin.Y = text.pos.Y - (vDims.Height*0.25f);
										text.aabb.vBoxMin.Z = 0.0f;

										text.aabb.vBoxMax.X = text.pos.X + xLength;
										text.aabb.vBoxMax.Y = text.pos.Y + (vDims.Height*0.75f);
										text.aabb.vBoxMax.Z = 0.0f;
									}
								}

								text.show = true;
								text.drawBox = false;
								text.boxColour = math::Vec4Lite( 0, 0, 0, 255 );

								m_Text.push_back(text);

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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// clear touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		buttonIt->touched = false;
		buttonIt->singleTouch = false;
		buttonIt->touchIndex = -1;
		buttonIt->touchPos = math::Vec3( 0.0f, 0.0f, 0.0f );

		buttonIt->touchCount = 0;
		buttonIt->eventCode = -1;
		buttonIt->hover = 0;

		// next
		buttonIt++;
	}

	// clear touch flag
	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		textIt->touched = false;
		textIt->singleTouch = false;
		textIt->touchIndex = -1;
		textIt->touchPos = math::Vec3( 0.0f, 0.0f, 0.0f );

		textIt->touchCount = 0;
		textIt->eventCode = -1;
		textIt->hover = 0;

		// next
		textIt++;
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
		buttonIt = m_Buttons.begin();
		while(buttonIt != m_Buttons.end())
		{
			if( buttonIt->areaType == 0 )
			{
				// test against aabb
				if( touch.SphereCollidesWithAABB( buttonIt->aabb ) )
				{
					if( (pData->bPress) || (pData->bRelease) || (pData->bHeld) )
					{
						buttonIt->touched = true;
						buttonIt->touchIndex = i;
						buttonIt->touchPos = pos;

						if( (pData->bPress) || (pData->bHeld) )
						{
							if(pData->Ticks < input::HOLD_TIME )
							{
								buttonIt->singleTouch = true;
								buttonIt->eventCode = EVENT_PRESS;
							}
							else
							{
								buttonIt->singleTouch = false;
								buttonIt->eventCode = EVENT_HELD;
							}
						}
						else if( (pData->bRelease) )
						{
							buttonIt->singleTouch = true;
							buttonIt->eventCode = EVENT_RELEASE;
						}

						buttonIt->touchCount++;
					}

					buttonIt->hover = 1;
				}

			}
			else
			{
				// test against sphere
				if( touch.SphereCollidesWithSphere( buttonIt->sphere ) )
				{
					if( (pData->bPress) || (pData->bRelease) || (pData->bHeld) )
					{
						buttonIt->touched = true;
						buttonIt->touchIndex = i;
						buttonIt->touchPos = pos;

						if( (pData->bPress) || (pData->bHeld) )
						{
							if(pData->Ticks <= 0.5f )
							{
								buttonIt->singleTouch = true;
								buttonIt->eventCode = EVENT_PRESS;
							}
							else
							{
								buttonIt->singleTouch = false;
								buttonIt->eventCode = EVENT_HELD;
							}
						}
						else if( (pData->bRelease) )
						{
							buttonIt->singleTouch = true;
							buttonIt->eventCode = EVENT_RELEASE;
						}

						buttonIt->touchCount++;
					}

					buttonIt->hover = 1;
				}
			}

			// next
			buttonIt++;
		}

		// check touch flag
		textIt = m_Text.begin();
		while(textIt != m_Text.end())
		{
			// test against aabb
			if( touch.SphereCollidesWithAABB( textIt->aabb ) )
			{
				if( (pData->bPress) || (pData->bRelease) || (pData->bHeld) )
				{
					textIt->touched = true;
					textIt->touchIndex = i;
					textIt->touchPos = pos;

					if( (pData->bPress) || (pData->bHeld) )
					{
						if(pData->Ticks <= 0.5f )
						{
							textIt->singleTouch = true;
							textIt->eventCode = EVENT_PRESS;
						}
						else
						{
							textIt->singleTouch = false;
							textIt->eventCode = EVENT_HELD;
						}
					}
					else if( (pData->bRelease) )
					{
						textIt->singleTouch = true;
						textIt->eventCode = EVENT_RELEASE;
					}

					textIt->touchCount++;
				}

				textIt->hover = 1;
			}

			// next
			textIt++;
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

	if( m_DevData.enableDebugUIDraw )
	{
		std::vector<SimpleButton>::iterator buttonIt;
		buttonIt = m_Buttons.begin();

		// change colour
		const math::Vec4Lite origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
		renderer::OpenGL::GetInstance()->SetColour4ub( 0, 0, 255, 255 );
		while(buttonIt != m_Buttons.end())
		{
			if( buttonIt->areaType == 0 )
			{
				renderer::DrawAABB( buttonIt->aabb.vBoxMin, buttonIt->aabb.vBoxMax );
			}
			else
			{
				glPushMatrix();
					glTranslatef( buttonIt->sphere.vCenterPoint.X, buttonIt->sphere.vCenterPoint.Y, buttonIt->sphere.vCenterPoint.Z );
					renderer::DrawSphere( buttonIt->sphere.fRadius );
				glPopMatrix();
			}

			// next
			buttonIt++;
		}

		// reset colour
		renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
	}

	std::vector<SimpleText>::iterator textIt;
	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		// draw BG box
		if( textIt->drawBox &&
			!core::IsEmptyString( textIt->characterBuffer) )
		{
			math::Vec4Lite origCol = renderer::OpenGL::GetInstance()->GetColour4ub();
			renderer::OpenGL::GetInstance()->SetColour4ub( textIt->boxColour.R, textIt->boxColour.G, textIt->boxColour.B, origCol.A );

			glPushMatrix();
				glTranslatef( textIt->aabb.vCenter.X, textIt->aabb.vCenter.Y, textIt->aabb.vCenter.Z );
				renderer::DrawAABB( textIt->aabb.vBoxMin, textIt->aabb.vBoxMax, true );
			glPopMatrix();
		}

		if( textIt->show )
		{
			math::Vec4Lite origCol = renderer::OpenGL::GetInstance()->GetColour4ub();
			math::Vec4Lite newColour;

			newColour.R = textIt->fontR; 
			newColour.G = textIt->fontG; 
			newColour.B = textIt->fontB; 
			newColour.A = origCol.A;

			textIt->pFontfreetype->SetBlockFillColour( newColour );
			textIt->pFontfreetype->Print(static_cast<int>(textIt->pos.X), static_cast<int>(textIt->pos.Y), (textIt->dropShadow != 0), GL_LINEAR, textIt->characterBuffer);
		}

		if( m_DevData.enableDebugUIDraw )
		{
			// change colour
			const math::Vec4Lite origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
			renderer::OpenGL::GetInstance()->SetColour4ub(0, 0, 255, 255);

			renderer::DrawAABB( textIt->aabb.vBoxMin, textIt->aabb.vBoxMax );

			// reset colour
			renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
		}

		// next
		textIt++;
	}

}

/////////////////////////////////////////////////////
/// Method: DrawSelected
/// Params: None
///
/////////////////////////////////////////////////////
void UIFileLoader::DrawSelected()
{
    if( !core::app::IstvOS() )
        return;
    
	if (m_SelectedButton != 0 &&
		m_SelectedButton->show &&
		m_SelectedButton->hightlightOnSelect )
	{
		glLineWidth(m_DevData.selectionBoxThickness);

		// change colour
		const math::Vec4Lite origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
		renderer::OpenGL::GetInstance()->SetColour4ub(m_SelectionColour.R, m_SelectionColour.G, m_SelectionColour.B, static_cast<GLubyte>(m_SelectionAlpha * 255.0f));
		collision::AABB aabb;
		aabb.vBoxMin = m_SelectedButton->aabb.vBoxMin;
		aabb.vBoxMax = m_SelectedButton->aabb.vBoxMax;

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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;
	
	// get touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if (buttonIt->elementId == elementId &&
			buttonIt->show)
		{
			if( eventCode == EVENT_HOVER )
				return (buttonIt->hover != 0 );
			else
			if( (buttonIt->eventCode == eventCode) && 
				buttonIt->touched )
				return(true);
			else
				return(false);
		}

		// next
		buttonIt++;
	}

	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId &&
			textIt->show )
		{
			if( eventCode == EVENT_HOVER )
				return (textIt->hover != 0 );
			else
			if( (textIt->eventCode == eventCode) && 
				textIt->touched )
				return(true);
			else
				return(false);
		}

		// next
		textIt++;
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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// get touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if (buttonIt->elementId == elementId &&
			buttonIt->show )
		{
			if(buttonIt->eventCode == EVENT_RELEASE) 
				return(buttonIt->singleTouch);
		}

		buttonIt++;
	}

	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId &&
			textIt->show )
		{
			if(textIt->eventCode == EVENT_RELEASE) 
				return(textIt->singleTouch);
		}

		// next
		textIt++;
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

	std::vector<SimpleText>::iterator textIt;
	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		if( elementId == textIt->elementId )
		{
			std::size_t len = std::strlen( szText );

			if( (int)len < UI_MAXSTATICTEXTBUFFER_SIZE )
			{
				textIt->currentCharCount = static_cast<unsigned int>(len);
				snprintf( textIt->characterBuffer, UI_MAXSTATICTEXTBUFFER_SIZE, "%s", szText );

				float xLength = 0.0f;
				int fontSpacing = 0;
				int fontWidth = 0;
				int fontHeight = 0;

				math::Vec2 vDims;
				vDims = textIt->pFontfreetype->GetDimensions();
				fontWidth = static_cast<int>(vDims.Width);
				fontHeight = static_cast<int>(vDims.Height);
				fontSpacing = fontWidth;

				if( textIt->currentCharCount >= 1 )
				{
					unsigned int i = 0;
					for( i = 0; i < textIt->currentCharCount; i++ )
					{
						// valid buffer
						if( textIt->characterBuffer )
						{
							// lower case, increase only by font spacing
							int nCharWidth = 0;
							textIt->pFontfreetype->GetCharacterWidth( textIt->characterBuffer[i], &nCharWidth );
							xLength += nCharWidth;					
						}
					}
				}

				if( textIt->alignment == TextAlignment_Left ) // left
				{
					textIt->pos.X = textIt->screenPos.X;
					textIt->pos.Y = textIt->screenPos.Y - (vDims.Height*0.25f);

					//textIt->aabb.vCenter.X = (textIt->pos.X + xLength*0.5f); 
					//textIt->aabb.vCenter.Y = textIt->pos.Y; 
					//textIt->aabb.vCenter.Z = 0.0f; 

					textIt->aabb.vBoxMin.X = textIt->pos.X;
					textIt->aabb.vBoxMin.Y = textIt->pos.Y - (vDims.Height*0.25f);
					textIt->aabb.vBoxMin.Z = 0.0f;

					textIt->aabb.vBoxMax.X = textIt->pos.X + xLength;
					textIt->aabb.vBoxMax.Y = textIt->pos.Y + (vDims.Height*0.75f);
					textIt->aabb.vBoxMax.Z = 0.0f;
					textIt->aabb.vCenter = textIt->aabb.vBoxMax - ((textIt->aabb.vBoxMax-textIt->aabb.vBoxMin)*0.5f); 
				}
				else
				if( textIt->alignment == TextAlignment_Center ) // center
				{
					textIt->pos.X = textIt->screenPos.X - xLength*0.5f;
					textIt->pos.Y = textIt->screenPos.Y - (vDims.Height*0.25f);

					//textIt->aabb.vCenter.X = textIt->screenPos.X; 
					//textIt->aabb.vCenter.Y = textIt->screenPos.Y; 
					//textIt->aabb.vCenter.Z = 0.0f; 

					textIt->aabb.vBoxMin.X = textIt->screenPos.X - xLength*0.5f;
					textIt->aabb.vBoxMin.Y = textIt->screenPos.Y - (vDims.Height*0.5f);
					textIt->aabb.vBoxMin.Z = 0.0f;

					textIt->aabb.vBoxMax.X = textIt->screenPos.X + xLength*0.5f;
					textIt->aabb.vBoxMax.Y = textIt->screenPos.Y + (vDims.Height*0.5f);
					textIt->aabb.vBoxMax.Z = 0.0f;
					textIt->aabb.vCenter = textIt->aabb.vBoxMax - ((textIt->aabb.vBoxMax-textIt->aabb.vBoxMin)*0.5f); 
				}
				else
				if( textIt->alignment == TextAlignment_Right ) // right
				{
					textIt->pos.X = textIt->screenPos.X - xLength;
					textIt->pos.Y = textIt->screenPos.Y - (vDims.Height*0.25f);

					//textIt->aabb.vCenter.X = (textIt->pos.X + xLength*0.5f); 
					//textIt->aabb.vCenter.Y = textIt->pos.Y; 
					//textIt->aabb.vCenter.Z = 0.0f; 

					textIt->aabb.vBoxMin.X = textIt->pos.X;
					textIt->aabb.vBoxMin.Y = textIt->pos.Y - (vDims.Height*0.25f);
					textIt->aabb.vBoxMin.Z = 0.0f;

					textIt->aabb.vBoxMax.X = textIt->pos.X + xLength;
					textIt->aabb.vBoxMax.Y = textIt->pos.Y + (vDims.Height*0.75f);
					textIt->aabb.vBoxMax.Z = 0.0f;
					textIt->aabb.vCenter = textIt->aabb.vBoxMax - ((textIt->aabb.vBoxMax-textIt->aabb.vBoxMin)*0.5f); 
				}
			}
			return;
		}

		// next
		textIt++;
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

	std::vector<SimpleText>::iterator textIt;
	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		if( elementId == textIt->elementId )
		{
			textIt->drawBox = state;
			return;
		}

		// next
		textIt++;
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

	std::vector<SimpleText>::iterator textIt;
	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		if( elementId == textIt->elementId )
		{
			textIt->boxColour = col;
			return;
		}

		// next
		textIt++;
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

	std::vector<SimpleText>::iterator textIt;
	textIt = m_Text.begin();

	while (textIt != m_Text.end())
	{
		if (elementId == textIt->elementId)
		{
			textIt->dropShadow = state;
			return;
		}

		// next
		textIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementDrawState
/// Params: [in]elementId
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementDrawState( int elementId, bool state )
{
	if( !m_Loaded )
		return;

	std::vector<SimpleText>::iterator textIt;
	std::vector<SimpleButton>::iterator buttonIt;

	buttonIt = m_Buttons.begin();
	while (buttonIt != m_Buttons.end())
	{
		if (elementId == buttonIt->elementId)
		{
			buttonIt->show = state;
			return;
		}
		buttonIt++;
	}

	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		if( elementId == textIt->elementId )
		{
			textIt->show = state;
			return;
		}

		// next
		textIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeElementDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void UIFileLoader::ChangeElementDrawState( bool state )
{
	if( !m_Loaded )
		return;

	std::vector<SimpleText>::iterator textIt;
	std::vector<SimpleButton>::iterator buttonIt;

	buttonIt = m_Buttons.begin();
	while (buttonIt != m_Buttons.end())
	{
		buttonIt->show = state;

		buttonIt++;
	}
	

	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		textIt->show = state;

		// next
		textIt++;
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

	std::vector<SimpleButton>::iterator buttonIt;

	// button change
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if( buttonIt->elementId == elementId )
		{
			if( buttonIt->areaType == 0 )
			{
				buttonIt->aabb.vCenter = pos;
				buttonIt->aabb.vBoxMin = pos - buttonIt->dims;
				buttonIt->aabb.vBoxMax = pos + buttonIt->dims;
			}
			else
			{
				buttonIt->sphere.vCenterPoint = pos;
			}

			if( m_Model != 0 )
			{
				if( buttonIt->meshAttach1Id != -1 )
					m_Model->SetMeshTranslation( buttonIt->meshAttach1Id, pos );

				if( buttonIt->meshAttach2Id != -1 )
					m_Model->SetMeshTranslation( buttonIt->meshAttach2Id, pos );

				if( buttonIt->meshAttach3Id != -1 )
					m_Model->SetMeshTranslation( buttonIt->meshAttach3Id, pos );

				if( buttonIt->meshAttach4Id != -1 )
					m_Model->SetMeshTranslation( buttonIt->meshAttach4Id, pos );
			
				if( buttonIt->meshAttach5Id != -1 )
					m_Model->SetMeshTranslation( buttonIt->meshAttach5Id, pos );
			}
			return;
		}

		// next
		buttonIt++;
	}

	// text change
	std::vector<SimpleText>::iterator textIt;
	textIt = m_Text.begin();

	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId )
		{
			textIt->screenPos.X = pos.X;
			textIt->screenPos.Y = pos.Y;

			ChangeElementText( elementId, textIt->characterBuffer );

			if( m_Model != 0 )
			{
				if( textIt->meshAttach1Id != -1 )
					m_Model->SetMeshTranslation( textIt->meshAttach1Id, pos );

				if( textIt->meshAttach2Id != -1 )
					m_Model->SetMeshTranslation( textIt->meshAttach2Id, pos );

				if( textIt->meshAttach3Id != -1 )
					m_Model->SetMeshTranslation( textIt->meshAttach3Id, pos );

				if( textIt->meshAttach4Id != -1 )
					m_Model->SetMeshTranslation( textIt->meshAttach4Id, pos );
			
				if( textIt->meshAttach5Id != -1 )
					m_Model->SetMeshTranslation( textIt->meshAttach5Id, pos );			
			}
			return;
		}
		// next
		textIt++;
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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// get touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if( buttonIt->elementId == elementId )
		{
			if( buttonIt->areaType == 0 )
			{
				return(buttonIt->aabb.vCenter);
			}
			else
			{
				return(buttonIt->sphere.vCenterPoint);
			}
		}

		// next
		buttonIt++;
	}

	// get touch flag
	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId )
		{
			return(textIt->aabb.vCenter);
		}

		// next
		textIt++;
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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// get touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if( buttonIt->elementId == elementId )
		{
			return( buttonIt->touchPos );
		}

		// next
		buttonIt++;
	}

	// get touch flag
	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId )
		{
			return( textIt->touchPos );
		}

		// next
		textIt++;
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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// get touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if( buttonIt->elementId == elementId )
		{
			return( buttonIt->touchIndex );
		}

		buttonIt++;
	}

	// get touch flag
	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId )
		{
			return( textIt->touchIndex );
		}

		// next
		textIt++;
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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// get touch flag
	buttonIt = m_Buttons.begin();
	while(buttonIt != m_Buttons.end())
	{
		if( buttonIt->elementId == elementId )
		{
			return( buttonIt->touchCount );
		}

		buttonIt++;
	}

	// get touch flag
	textIt = m_Text.begin();
	while(textIt != m_Text.end())
	{
		if( textIt->elementId == elementId )
		{
			return( textIt->touchCount );
		}

		// next
		textIt++;
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Element id does not exist in UI" );

	return(-1);
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

	std::vector<SimpleButton>::iterator buttonIt;
	std::vector<SimpleText>::iterator textIt;

	// get touch flag
	buttonIt = m_Buttons.begin();
	while (buttonIt != m_Buttons.end())
	{
		if (buttonIt->elementId == elementId)
		{
			return(buttonIt->aabb);
		}

		// next
		buttonIt++;
	}

	// get touch flag
	textIt = m_Text.begin();
	while (textIt != m_Text.end())
	{
		if (textIt->elementId == elementId)
		{
			return(textIt->aabb);
		}

		// next
		textIt++;
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
    
    if( !core::app::IstvOS() )
        return;
    
	int lastSelected = 9999;
	m_SelectedButton = 0;

	std::size_t i=0;
	bool foundSelected = false;

	for (i = 0; i != m_Buttons.size(); ++i)
	{
		if (m_Buttons[i].numeric1 != -9999)
		{
			// want the lowest selectable
			if (m_Buttons[i].show &&
				m_Buttons[i].selectable )
			{
				if (selectElement != -1)
				{
					if (m_Buttons[i].elementId == selectElement)
					{
						m_SelectedButton = &m_Buttons[i];
						foundSelected = true;
					}
				}

				if (!foundSelected)
				{
					if (m_Buttons[i].numeric1 < lastSelected)
					{
						lastSelected = m_Buttons[i].numeric1;
						m_SelectedButton = &m_Buttons[i];
					}
				}
			}
		}
	}

	if (m_SelectedButton)
	{
		input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedButton->pos.X);
		input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedButton->pos.Y);
	}
}

/////////////////////////////////////////////////////
/// Method: FindNearestElementToSelection
/// Params: None
///
/////////////////////////////////////////////////////
UIFileLoader::SimpleButton* UIFileLoader::FindNearestElementToSelection(int selectionIndex, int minValue, int maxValue, bool nextInc)
{
	std::size_t i = 0;
	std::vector<SimpleButton*> rangeList;

	// find the buttons within the range
	for (i = 0; i != m_Buttons.size(); ++i)
	{
		if (m_Buttons[i].numeric1 != -9999)
		{
			if (m_Buttons[i].show &&
				m_Buttons[i].selectable &&
                m_SelectedButton != &m_Buttons[i])
			{
				if (m_Buttons[i].numeric1 >= minValue &&
					m_Buttons[i].numeric1 < maxValue)
				{
					rangeList.push_back(&m_Buttons[i]);
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
    if( !core::app::IstvOS() )
        return false;
    
	if (m_SelectedButton != 0)
	{
		bool notFound = true;

		// what value was the current column
		int originalColumn = m_SelectedButton->numeric1;
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

			SimpleButton* nextButton = FindNearestElementToSelection(currentColumn, minValue, maxValue, true);

			if ( nextButton != 0)
			{
				// something in range
				m_SelectedButton = nextButton;

				if (m_SelectedButton)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedButton->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedButton->pos.Y);
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
    if( !core::app::IstvOS() )
        return false;
    
	if (m_SelectedButton != 0)
	{
		bool notFound = true;

		// what value was the current column
		int originalColumn = m_SelectedButton->numeric1;
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

			SimpleButton* prevButton = FindNearestElementToSelection(currentColumn, minValue, maxValue, false);

			if (prevButton != 0)
			{
				// something in range
				m_SelectedButton = prevButton;

				if (m_SelectedButton)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedButton->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedButton->pos.Y);
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
    if( !core::app::IstvOS() )
        return false;
    
	if (m_SelectedButton != 0)
	{
		bool notFound = true;

		// what value was the current row
		int originalRow = m_SelectedButton->numeric1;
		int currentRow = originalRow;

		// need to calculate the min/max between a range of 10
		if (originalRow == 0 ||
			((originalRow / 10) != 0) )
			originalRow += 1;

		int minValue = RoundDown(originalRow+10);
		int maxValue = RoundUp(originalRow+10);

		while (notFound)
		{
			currentRow += 10;

			SimpleButton* nextButton = FindNearestElementToSelection(currentRow, minValue, maxValue, true);

			if (nextButton != 0)
			{
				// something in range
				m_SelectedButton = nextButton;

				if (m_SelectedButton)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedButton->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedButton->pos.Y);
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
    if( !core::app::IstvOS() )
        return false;
    
	if (m_SelectedButton != 0)
	{
		bool notFound = true;

		// what value was the current row
		int originalRow = m_SelectedButton->numeric1;
		int currentRow = originalRow;

		// need to calculate the min/max between a range of 10
		if (originalRow == 0 ||
			((originalRow / 10) != 0))
			originalRow += 1;

		int minValue = RoundDown(std::abs(originalRow - 10));
		int maxValue = RoundUp(std::abs(originalRow - 10));

		if ((originalRow-10) <= 0)
			maxValue = 10;

		while (notFound)
		{
			currentRow -= 10;

			SimpleButton* nextButton = FindNearestElementToSelection(currentRow, minValue, maxValue, false);

			if (nextButton != 0)
			{
				// something in range
				m_SelectedButton = nextButton;

				if (m_SelectedButton)
				{
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = static_cast<long>(m_SelectedButton->pos.X);
					input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = static_cast<long>(m_SelectedButton->pos.Y);
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
void UIFileLoader::SelectionPress( bool state )
{
    if( !core::app::IstvOS() )
        return;
    
	if (state &&
		!input::gInputState.TouchesData[input::SECOND_TOUCH].bActive)
	{
		input::gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
		input::gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2(0.0f, 0.0f);
	}
	
	if ( !state &&
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress)
	{
		input::gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].Ticks = 0.0f;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress = false;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bHeld = false;
	}
}