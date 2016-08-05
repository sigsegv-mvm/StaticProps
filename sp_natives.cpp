#include "sp_natives.h"


const char *Stringify_SolidType(SolidType_t solid_type)
{
	static std::map<SolidType_t, const char *> the_map{
		{ SOLID_NONE,     "SOLID_NONE"     },
		{ SOLID_BSP,      "SOLID_BSP"      },
		{ SOLID_BBOX,     "SOLID_BBOX"     },
		{ SOLID_OBB,      "SOLID_OBB"      },
		{ SOLID_OBB_YAW,  "SOLID_OBB_YAW"  },
		{ SOLID_CUSTOM,   "SOLID_CUSTOM"   },
		{ SOLID_VPHYSICS, "SOLID_VPHYSICS" },
	};
	
	static char buf[4096];
	
	auto it = the_map.find(solid_type);
	if (it != the_map.end()) {
		return (*it).second;
	} else {
		V_sprintf_safe(buf, "INVALID(%d)", solid_type);
		return buf;
	}
}

const char *Stringify_SolidFlags(SolidFlags_t solid_flags)
{
	static std::map<SolidFlags_t, const char *> the_map{
		{ FSOLID_CUSTOMRAYTEST,        "FSOLID_CUSTOMRAYTEST"        },
		{ FSOLID_CUSTOMBOXTEST,        "FSOLID_CUSTOMBOXTEST"        },
		{ FSOLID_NOT_SOLID,            "FSOLID_NOT_SOLID"            },
		{ FSOLID_TRIGGER,              "FSOLID_TRIGGER"              },
		{ FSOLID_NOT_STANDABLE,        "FSOLID_NOT_STANDABLE"        },
		{ FSOLID_VOLUME_CONTENTS,      "FSOLID_VOLUME_CONTENTS"      },
		{ FSOLID_FORCE_WORLD_ALIGNED,  "FSOLID_FORCE_WORLD_ALIGNED"  },
		{ FSOLID_USE_TRIGGER_BOUNDS,   "FSOLID_USE_TRIGGER_BOUNDS"   },
		{ FSOLID_ROOT_PARENT_ALIGNED,  "FSOLID_ROOT_PARENT_ALIGNED"  },
		{ FSOLID_TRIGGER_TOUCH_DEBRIS, "FSOLID_TRIGGER_TOUCH_DEBRIS" },
	};
	
	static char buf[4096];
	buf[0] = '\0';
	
	int num_flags = 0;
	for (unsigned int i = 0; i < 32; ++i) {
		auto flag = (SolidFlags_t)(1U << i);
		
		if ((solid_flags & flag) != 0) {
			if (num_flags != 0) {
				V_strcat_safe(buf, "|");
			}
			
			auto it = the_map.find(flag);
			if (it != the_map.end()) {
				V_strcat_safe(buf, (*it).second);
			} else {
				V_strcat_safe(buf, CFmtStrN<64>("INVALID(b%u)", i));
			}
			
			++num_flags;
		}
	}
	
	if (num_flags == 0) {
		V_strcpy_safe(buf, "0");
	}
	
	return buf;
}


const char *Stringify_CollisionGroup(Collision_Group_t collision_group)
{
	static std::map<Collision_Group_t, const char *> the_map{
		{ COLLISION_GROUP_NONE,               "COLLISION_GROUP_NONE"               },
		{ COLLISION_GROUP_DEBRIS,             "COLLISION_GROUP_DEBRIS"             },
		{ COLLISION_GROUP_DEBRIS_TRIGGER,     "COLLISION_GROUP_DEBRIS_TRIGGER"     },
		{ COLLISION_GROUP_INTERACTIVE_DEBRIS, "COLLISION_GROUP_INTERACTIVE_DEBRIS" },
		{ COLLISION_GROUP_INTERACTIVE,        "COLLISION_GROUP_INTERACTIVE"        },
		{ COLLISION_GROUP_PLAYER,             "COLLISION_GROUP_PLAYER"             },
		{ COLLISION_GROUP_BREAKABLE_GLASS,    "COLLISION_GROUP_BREAKABLE_GLASS"    },
		{ COLLISION_GROUP_VEHICLE,            "COLLISION_GROUP_VEHICLE"            },
		{ COLLISION_GROUP_PLAYER_MOVEMENT,    "COLLISION_GROUP_PLAYER_MOVEMENT"    },
		{ COLLISION_GROUP_NPC,                "COLLISION_GROUP_NPC"                },
		{ COLLISION_GROUP_IN_VEHICLE,         "COLLISION_GROUP_IN_VEHICLE"         },
		{ COLLISION_GROUP_WEAPON,             "COLLISION_GROUP_WEAPON"             },
		{ COLLISION_GROUP_VEHICLE_CLIP,       "COLLISION_GROUP_VEHICLE_CLIP"       },
		{ COLLISION_GROUP_PROJECTILE,         "COLLISION_GROUP_PROJECTILE"         },
		{ COLLISION_GROUP_DOOR_BLOCKER,       "COLLISION_GROUP_DOOR_BLOCKER"       },
		{ COLLISION_GROUP_PASSABLE_DOOR,      "COLLISION_GROUP_PASSABLE_DOOR"      },
		{ COLLISION_GROUP_DISSOLVING,         "COLLISION_GROUP_DISSOLVING"         },
		{ COLLISION_GROUP_PUSHAWAY,           "COLLISION_GROUP_PUSHAWAY"           },
		{ COLLISION_GROUP_NPC_ACTOR,          "COLLISION_GROUP_NPC_ACTOR"          },
		{ COLLISION_GROUP_NPC_SCRIPTED,       "COLLISION_GROUP_NPC_SCRIPTED"       },
	};
	
	static char buf[4096];
	
	auto it = the_map.find(collision_group);
	if (it != the_map.end()) {
		return (*it).second;
	} else {
		V_sprintf_safe(buf, "UNKNOWN(%d)", collision_group);
		return buf;
	}
}


Vector CellsToVector(const cell_t *cells)
{
	return Vector(sp_ctof(cells[0]), sp_ctof(cells[1]), sp_ctof(cells[2]));
}

void VectorToCells(const Vector& vec, cell_t *cells)
{
	cells[0] = sp_ftoc(vec.x);
	cells[1] = sp_ftoc(vec.y);
	cells[2] = sp_ftoc(vec.z);
}

void QAngleToCells(const QAngle& ang, cell_t *cells)
{
	cells[0] = sp_ftoc(ang.x);
	cells[1] = sp_ftoc(ang.y);
	cells[2] = sp_ftoc(ang.z);
}


cell_t SP_GetAllStaticProps(IPluginContext *pContext, const cell_t *params)
{
	const cell_t array_max = params[2];
	cell_t *array; pContext->LocalToPhysAddr(params[1], &array);
	
	DEBUG_LOG("%s: [array: %p] [array_max: %d]", __FUNCTION__, array, array_max);
	
	CUtlVector<ICollideable *> props;
	staticpropmgr->GetAllStaticProps(&props);
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
	DEBUG_LOG("%s: got %d from engine, stored %d in array", __FUNCTION__, props.Count(), num_stored);
	
	return num_stored;
}

cell_t SP_GetAllStaticPropsInAABB(IPluginContext *pContext, const cell_t *params)
{
	const cell_t array_max = params[2];
	cell_t *array; pContext->LocalToPhysAddr(params[1], &array);
	cell_t *mins;  pContext->LocalToPhysAddr(params[3], &mins);
	cell_t *maxs;  pContext->LocalToPhysAddr(params[4], &maxs);
	
	Vector vecMins = CellsToVector(mins);
	Vector vecMaxs = CellsToVector(maxs);
	
	DEBUG_LOG("%s: [array: %p] [array_max: %d] [mins: %+5.0f %+5.0f %+5.0f] [maxs: %+5.0f %+5.0f %+5.0f]",
		__FUNCTION__, array, array_max,
		vecMins.x, vecMins.y, vecMins.z,
		vecMaxs.x, vecMaxs.y, vecMaxs.z);
	
	CUtlVector<ICollideable *> props;
	staticpropmgr->GetAllStaticPropsInAABB(vecMins, vecMaxs, &props);
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
	DEBUG_LOG("%s: got %d from engine, stored %d in array", __FUNCTION__, props.Count(), num_stored);
	
	return num_stored;
}

cell_t SP_GetAllStaticPropsInOBB(IPluginContext *pContext, const cell_t *params)
{
	const cell_t array_max = params[2];
	cell_t *array;   pContext->LocalToPhysAddr(params[1], &array);
	cell_t *origin;  pContext->LocalToPhysAddr(params[3], &origin);
	cell_t *extent1; pContext->LocalToPhysAddr(params[4], &extent1);
	cell_t *extent2; pContext->LocalToPhysAddr(params[5], &extent2);
	cell_t *extent3; pContext->LocalToPhysAddr(params[6], &extent3);
	
	Vector vecOrigin  = CellsToVector(origin);
	Vector vecExtent1 = CellsToVector(extent1);
	Vector vecExtent2 = CellsToVector(extent2);
	Vector vecExtent3 = CellsToVector(extent3);
	
	DEBUG_LOG("%s: [array: %p] [array_max: %d] [origin: %+5.0f %+5.0f %+5.0f] [extent1: %+5.0f %+5.0f %+5.0f] [extent2: %+5.0f %+5.0f %+5.0f] [extent3: %+5.0f %+5.0f %+5.0f]",
		__FUNCTION__, array, array_max,
		vecOrigin.x, vecOrigin.y, vecOrigin.z,
		vecExtent1.x, vecExtent1.y, vecExtent1.z,
		vecExtent2.x, vecExtent2.y, vecExtent2.z,
		vecExtent3.x, vecExtent3.y, vecExtent3.z);
	
	CUtlVector<ICollideable *> props;
	staticpropmgr->GetAllStaticPropsInOBB(vecOrigin, vecExtent1, vecExtent2, vecExtent3, &props);
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
	DEBUG_LOG("%s: got %d from engine, stored %d in array", __FUNCTION__, props.Count(), num_stored);
	
	return num_stored;
}


cell_t SP_GetAllStaticPropsInAABB_Alt(IPluginContext *pContext, const cell_t *params)
{
	const cell_t array_max = params[2];
	cell_t *array; pContext->LocalToPhysAddr(params[1], &array);
	cell_t *mins;  pContext->LocalToPhysAddr(params[3], &mins);
	cell_t *maxs;  pContext->LocalToPhysAddr(params[4], &maxs);
	
	Vector vecMins = CellsToVector(mins);
	Vector vecMaxs = CellsToVector(maxs);
	
	DEBUG_LOG("%s: [array: %p] [array_max: %d] [mins: %+5.0f %+5.0f %+5.0f] [maxs: %+5.0f %+5.0f %+5.0f]",
		__FUNCTION__, array, array_max,
		vecMins.x, vecMins.y, vecMins.z,
		vecMaxs.x, vecMaxs.y, vecMaxs.z);
	
	CUtlVector<ICollideable *> props_all;
	staticpropmgr->GetAllStaticProps(&props_all);
	
	CUtlVector<ICollideable *> props;
	FOR_EACH_VEC(props_all, i) {
		auto prop = props_all[i];
		
		const Vector& vecPropOrigin = prop->GetCollisionOrigin();
		Vector vecPropMins = vecPropOrigin + prop->OBBMins();
		Vector vecPropMaxs = vecPropOrigin + prop->OBBMaxs();
		
		if (IsBoxIntersectingBox(vecMins, vecMaxs, vecPropMins, vecPropMaxs)) {
			props.AddToTail(prop);
			DEBUG_LOG("%s: INCLUDED: prop #%d with [absmins: %+5.0f %+5.0f %+5.0f] [absmaxs: %+5.0f %+5.0f %+5.0f]",
				__FUNCTION__, i, vecPropMins.x, vecPropMins.y, vecPropMins.z, vecPropMaxs.x, vecPropMaxs.y, vecPropMaxs.z);
		} else {
			DEBUG_LOG("%s: EXCLUDED: prop #%d with [absmins: %+5.0f %+5.0f %+5.0f] [absmaxs: %+5.0f %+5.0f %+5.0f]",
				__FUNCTION__, i, vecPropMins.x, vecPropMins.y, vecPropMins.z, vecPropMaxs.x, vecPropMaxs.y, vecPropMaxs.z);
		}
	}
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
	DEBUG_LOG("%s: got %d from engine, filtered down to %d, stored %d in array", __FUNCTION__, props_all.Count(), props.Count(), num_stored);
	
	return num_stored;
}


cell_t SP_StaticProp_GetMins(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *mins; pContext->LocalToPhysAddr(params[2], &mins);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	Vector vecMins = collideable->OBBMins();
	VectorToCells(vecMins, mins);
	
	DEBUG_LOG("%s: [index: %d] [mins: %+5.0f %+5.0f %+5.0f]", __FUNCTION__,
		index, vecMins.x, vecMins.y, vecMins.z);
	
	return true;
}

cell_t SP_StaticProp_GetMaxs(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *maxs; pContext->LocalToPhysAddr(params[2], &maxs);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	Vector vecMaxs = collideable->OBBMaxs();
	VectorToCells(vecMaxs, maxs);
	
	DEBUG_LOG("%s: [index: %d] [maxs: %+5.0f %+5.0f %+5.0f]", __FUNCTION__,
		index, vecMaxs.x, vecMaxs.y, vecMaxs.z);
	
	return true;
}

cell_t SP_StaticProp_GetOrigin(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *origin; pContext->LocalToPhysAddr(params[2], &origin);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	Vector vecOrigin = collideable->GetCollisionOrigin();
	VectorToCells(vecOrigin, origin);
	
	DEBUG_LOG("%s: [index: %d] [origin: %+5.0f %+5.0f %+5.0f]", __FUNCTION__,
		index, vecOrigin.x, vecOrigin.y, vecOrigin.z);
	
	return true;
}

cell_t SP_StaticProp_GetAngles(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *angles; pContext->LocalToPhysAddr(params[2], &angles);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	QAngle angAngles = collideable->GetCollisionAngles();
	QAngleToCells(angAngles, angles);
	
	DEBUG_LOG("%s: [index: %d] [angles: %+5.1f %+5.1f %+5.1f]", __FUNCTION__,
		index, angAngles.x, angAngles.y, angAngles.z);
	
	return true;
}

cell_t SP_StaticProp_GetSolidType(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *solid_type; pContext->LocalToPhysAddr(params[2], &solid_type);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	*solid_type = collideable->GetSolid();
	
	DEBUG_LOG("%s: [index: %d] [solid_type: %s]", __FUNCTION__, index, Stringify_SolidType(collideable->GetSolid()));
	
	return true;
}

cell_t SP_StaticProp_GetSolidFlags(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *solid_flags; pContext->LocalToPhysAddr(params[2], &solid_flags);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	*solid_flags = collideable->GetSolidFlags();
	
	DEBUG_LOG("%s: [index: %d] [solid_flags: %s]", __FUNCTION__, index, Stringify_SolidFlags((SolidFlags_t)collideable->GetSolidFlags()));
	
	return true;
}

cell_t SP_StaticProp_GetCollisionGroup(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *collision_group; pContext->LocalToPhysAddr(params[2], &collision_group);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	*collision_group = collideable->GetCollisionGroup();
	
	DEBUG_LOG("%s: [index: %d] [collision_group: %s]", __FUNCTION__, index, Stringify_CollisionGroup((Collision_Group_t)collideable->GetCollisionGroup()));
	
	return true;
}

cell_t SP_StaticProp_GetModelName(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index    = params[1];
	const cell_t name     = params[2];
	const cell_t name_max = params[3];
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetStaticPropByIndex returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	const model_t *model = collideable->GetCollisionModel();
	if (model == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetCollisionModel returned nullptr", __FUNCTION__, index);
		return false;
	}
	
	const char *model_name = modelinfo->GetModelName(model);
	if (model_name == nullptr) {
		DEBUG_LOG("%s: [index: %d] GetModelName returned nullptr", __FUNCTION__, index);
		model_name = "";
	}
	
	pContext->StringToLocal(name, name_max, model_name);
	
	DEBUG_LOG("%s: [index: %d] [name: %s] [name_max: %d]", __FUNCTION__, index, model_name, name_max);
	
	return true;
}


const sp_nativeinfo_t g_Natives[] = {
	{ "GetAllStaticProps",            &SP_GetAllStaticProps            },
	{ "GetAllStaticPropsInAABB",      &SP_GetAllStaticPropsInAABB      },
	{ "GetAllStaticPropsInOBB",       &SP_GetAllStaticPropsInOBB       },
	{ "GetAllStaticPropsInAABB_Alt",  &SP_GetAllStaticPropsInAABB_Alt  },
	{ "StaticProp_GetMins",           &SP_StaticProp_GetMins           },
	{ "StaticProp_GetMaxs",           &SP_StaticProp_GetMaxs           },
	{ "StaticProp_GetOrigin",         &SP_StaticProp_GetOrigin         },
	{ "StaticProp_GetAngles",         &SP_StaticProp_GetAngles         },
	{ "StaticProp_GetSolidType",      &SP_StaticProp_GetSolidType      },
	{ "StaticProp_GetSolidFlags",     &SP_StaticProp_GetSolidFlags     },
	{ "StaticProp_GetCollisionGroup", &SP_StaticProp_GetCollisionGroup },
	{ "StaticProp_GetModelName",      &SP_StaticProp_GetModelName      },
	{ nullptr,                        nullptr                          },
};
