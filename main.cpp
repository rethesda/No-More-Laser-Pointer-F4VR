#include "common/IDebugLog.h"  // IDebugLog
#include "f4se_common/f4se_version.h"  // RUNTIME_VERSION
#include "f4se/PluginAPI.h"  // SKSEInterface, PluginInfo
#include "f4sE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "F4SE_common/BranchTrampoline.h"
#include "f4se/GameData.h"

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"
#include "NoMoreLaser.h"

static PluginHandle g_pluginHandle = kPluginHandle_Invalid;

static F4SEMessagingInterface* g_messaging = NULL;


//Listener for F4SE Messages
void OnF4SEMessage(F4SEMessagingInterface::Message* msg)
{
	if (msg)
	{
		if (msg->type == F4SEMessagingInterface::kMessage_GameLoaded)
		{
			// PUT FUNCTIONS HERE THAT NEED TO RUN AFTER A NEW GAME OR SAVE GAME IS LOADED
			NoMoreLaser::setGameLoaded();
		}
	}
}

extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* a_f4se, PluginInfo* a_info)
	{
		Sleep(5000);
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, R"(\\My Games\\Fallout4VR\\F4SE\\NoMoreLaserPointerF4VR.log)");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("No More Laser Pointer F4VR v%s", NOMORELASERPOINTERF4VR_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "NoMoreLaserPointerF4VR";
		a_info->version = NOMORELASERPOINTERF4VR_VERSION_MAJOR;

		if (a_f4se->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		a_f4se->runtimeVersion;
		if (a_f4se->runtimeVersion < RUNTIME_VR_VERSION_1_2_72)
		{
			_FATALERROR("Unsupported runtime version %s!\n", a_f4se->runtimeVersion);
			return false;
		}

		return true;
	}


	bool F4SEPlugin_Load(const F4SEInterface* a_f4se)
	{
		_MESSAGE("F4SE TEMPLATE Init");

		g_pluginHandle = a_f4se->GetPluginHandle();

		if (g_pluginHandle == kPluginHandle_Invalid) {
			return false;
		}

		g_messaging = (F4SEMessagingInterface*)a_f4se->QueryInterface(kInterface_Messaging);
		g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		if (!g_branchTrampoline.Create(1024 * 64))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}
		// Usually put plugin functions here

		NoMoreLaser::hookMain();

		_MESSAGE("NoMoreLaserF4VR Loaded");

		return true;
	}
};
