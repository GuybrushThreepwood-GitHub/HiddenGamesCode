
/*===================================================================
	File: MainStateUI.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "AppConsts.h"

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/MainStateUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameEffects/FullscreenEffects.h"
#include "GameEffects/InsectAttack.h"

#include "GameSystems.h"
#include "MicroGame/MicroGameID.h"

#include "GameStates/UI/GeneratedFiles/hud_l.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud_l.hui.h"

#include "GameStates/UI/GeneratedFiles/hud_r.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud_r.hui.h"

#include "GameStates/UI/GeneratedFiles/optionsgame.hgm.h"
#include "GameStates/UI/GeneratedFiles/optionsgame.hui.h"

namespace
{
	input::Input keyboardInput;

	bool hudToggle = true;
	const int DEFAULT_NOISE_ALPHA = 12;

	const float ANALOGUE_RADIUS = 65.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MainStateUI::MainStateUI( StateManager& stateManager, InputSystem& inputSystem )
: UIBaseState( 0 )
, IBaseGameState( stateManager, inputSystem )
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainStateUI::~MainStateUI()
{

}
