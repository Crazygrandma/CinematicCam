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
	int cameraMode = 0;
	int velocity = 10;
	Vector freecamPosition = { 0,0,0 };

public:
	void onLoad() override;
	void onUnload() override;

	// User defined functions
	void CreateValues();
	void RenderSettings() override;
	void orbitMode(CarWrapper car, CameraWrapper camera);
	void windowMode(CarWrapper car, CameraWrapper camera);

	std::pair<float, float> lookAt(Vector from, Vector to);

	void createDragSlider(std::string cvarname, const char* label, float dragspeed, std::string tooltip);

	// Template functions
	void Initialize();
	bool CanCreateValues();
	bool IsCVarNull(std::string cvarName);
	void Enable();
	void Disable();
	void HandleValues();
	Rotator GetSwivel();
};