
/*===================================================================
	File: VehicleAccess.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __VEHICLEACCESS_H__
#define __VEHICLEACCESS_H__

void RegisterVehicleFunctions( ScriptDataHolder& dataHolder );

int ScriptAddVehiclePack( lua_State* pState );
int ScriptSetupVehicle( lua_State* pState );

#endif // __VEHICLEACCESS_H__
