#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class CinematicCam : public BakkesMod::Plugin::BakkesModPlugin
	, public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{
private:
	bool enabled = false;
	Vector FOCUS;
	Rotator ROTATION, SWIVEL;
	float DISTANCE, FOV;
	bool overrideValue[8];
	bool isInBallCam = false;
	bool isInRearCam = false;
	float timeVal = 0;
	bool lockOnCar = false;
	bool gtaEnabled = false;
	bool orbitMode = false;
	bool freecamMode = false;

public:
	void onLoad() override;
	void onUnload() override;

	void CreateValues();
	ServerWrapper GetCurrentGameState();
	void RenderSettings() override;
	void createFloatSlider(std::string cvarname, float min, float max, const char* label, std::string tooltip);
	void createIntSlider(std::string cvarname, int min, int max, const char* label, std::string tooltip);
	void Initialize();
	bool CanCreateValues();
	bool IsCVarNull(std::string cvarName);
	void Enable();
	void Disable();
	void HandleValues();
	Rotator GetSwivel();
};