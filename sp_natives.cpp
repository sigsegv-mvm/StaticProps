#include "sp_natives.h"


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
	
	CUtlVector<ICollideable *> props;
	staticpropmgr->GetAllStaticProps(&props);
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
	return num_stored;
}

cell_t SP_GetAllStaticPropsInAABB(IPluginContext *pContext, const cell_t *params)
{
	const cell_t array_max = params[2];
	cell_t *array; pContext->LocalToPhysAddr(params[1], &array);
	cell_t *mins;  pContext->LocalToPhysAddr(params[3], &mins);
	cell_t *maxs;  pContext->LocalToPhysAddr(params[4], &maxs);
	
	CUtlVector<ICollideable *> props;
	staticpropmgr->GetAllStaticPropsInAABB(CellsToVector(mins), CellsToVector(maxs), &props);
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
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
	
	CUtlVector<ICollideable *> props;
	staticpropmgr->GetAllStaticPropsInOBB(CellsToVector(origin), CellsToVector(extent1), CellsToVector(extent2), CellsToVector(extent3), &props);
	
	int num_stored = 0;
	for (int i = 0; i < props.Count() && i < array_max; ++i) {
		array[num_stored++] = i;
	}
	
	return num_stored;
}


cell_t SP_StaticProp_GetMins(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *mins; pContext->LocalToPhysAddr(params[2], &mins);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	VectorToCells(collideable->OBBMins(), mins);
	
	return true;
}

cell_t SP_StaticProp_GetMaxs(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *maxs; pContext->LocalToPhysAddr(params[2], &maxs);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	VectorToCells(collideable->OBBMaxs(), maxs);
	
	return true;
}

cell_t SP_StaticProp_GetOrigin(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *origin; pContext->LocalToPhysAddr(params[2], &origin);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	VectorToCells(collideable->GetCollisionOrigin(), origin);
	
	return true;
}

cell_t SP_StaticProp_GetAngles(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *angles; pContext->LocalToPhysAddr(params[2], &angles);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	QAngleToCells(collideable->GetCollisionAngles(), angles);
	
	return true;
}

cell_t SP_StaticProp_GetSolidType(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *solid_type; pContext->LocalToPhysAddr(params[2], &solid_type);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	*solid_type = collideable->GetSolid();
	
	return true;
}

cell_t SP_StaticProp_GetSolidFlags(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *solid_flags; pContext->LocalToPhysAddr(params[2], &solid_flags);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	*solid_flags = collideable->GetSolidFlags();
	
	return true;
}

cell_t SP_StaticProp_GetCollisionGroup(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index = params[1];
	cell_t *collision_group; pContext->LocalToPhysAddr(params[2], &collision_group);
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	*collision_group = collideable->GetCollisionGroup();
	
	return true;
}

cell_t SP_StaticProp_GetModelName(IPluginContext *pContext, const cell_t *params)
{
	const cell_t index    = params[1];
	const cell_t name     = params[2];
	const cell_t name_max = params[3];
	
	ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(index);
	if (collideable == nullptr) return false;
	
	const model_t *model = collideable->GetCollisionModel();
	if (model == nullptr) return false;
	
	const char *model_name = modelinfo->GetModelName(model);
	if (model_name == nullptr) model_name = "";
	
	pContext->StringToLocal(name, name_max, model_name);
	
	return true;
}


const sp_nativeinfo_t g_Natives[] = {
	{ "GetAllStaticProps",            &SP_GetAllStaticProps            },
	{ "GetAllStaticPropsInOBB",       &SP_GetAllStaticPropsInOBB       },
	{ "GetAllStaticPropsInAABB",      &SP_GetAllStaticPropsInAABB      },
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
