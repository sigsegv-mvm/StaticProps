#include "extension.h"
#include "sp_natives.h"


ExtStaticProps g_Ext;
SMEXT_LINK(&g_Ext);


/* Source interfaces */
ICvar *icvar                        = nullptr;
IStaticPropMgrServer *staticpropmgr = nullptr;
IVModelInfo *modelinfo              = nullptr;


bool ExtStaticProps::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	return true;
}

void ExtStaticProps::SDK_OnAllLoaded()
{
	sharesys->AddNatives(myself, g_Natives);
}


bool ExtStaticProps::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	GET_V_IFACE_CURRENT(GetEngineFactory, staticpropmgr, IStaticPropMgrServer, INTERFACEVERSION_STATICPROPMGR_SERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, modelinfo,     IVModelInfo,          VMODELINFO_SERVER_INTERFACE_VERSION);
	
	return true;
}


bool ExtStaticProps::RegisterConCommandBase(ConCommandBase *pCommand)
{
	META_REGCVAR(pCommand);
	return true;
}
