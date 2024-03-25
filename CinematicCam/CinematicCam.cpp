#include "pch.h"
#include "CinematicCam.h"
#include "bakkesmod\wrappers\includes.h"
#include <sstream>
#include <filesystem>
#include <cmath>


BAKKESMOD_PLUGIN(CinematicCam, "Cinematic Cam Plugin", "1.0", PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CinematicCam::onLoad()
{
	_globalCvarManager = cvarManager;
	Initialize();
	cvarManager->registerNotifier("enableGTACam", [this](std::vector<std::string> params) {Enable(); }, "Enables camera control plugin", PERMISSION_ALL);
	cvarManager->registerNotifier("disableGTACam", [this](std::vector<std::string> params) {Disable(); }, "Disables camera control plugin", PERMISSION_ALL);

	cvarManager->registerCvar("gta_enabled", "0", "Enable GTA Camera", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		cvar.getBoolValue() ? Enable() : Disable();
			});

	cvarManager->registerCvar("gta_car_lock", "0", "Lock to car", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		lockOnCar = cvar.getBoolValue();
			});


	cvarManager->registerCvar("orbit_mode", "0", "Orbit mode", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		orbitMode = cvar.getBoolValue();
			});

	cvarManager->registerCvar("freecam_mode", "0", "Freecam mode", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		freecamMode = cvar.getBoolValue();
			});

	cvarManager->registerCvar("GTAHeight", "500", "Set the height of the camera", true, true, 0, false);
	cvarManager->registerCvar("GTADistance", "1000", "Set the distance relative to the car of the camera", true, true, 0, false);


	//cvarManager->registerCvar("GTAX", "0", "Set the x position relative to the car", true, true, 0, false);
	//cvarManager->registerCvar("GTAY", "0", "Set the y position relative to the car", true, true, 0, false);
	//cvarManager->registerCvar("GTAZ", "0", "Set the z position relative to the car", true, true, 0, false);

	//cvarManager->registerCvar("GTAXCamTarget", "0", "Cam Pivot X", true, true, 0, false);
	//cvarManager->registerCvar("GTAYCamTarget", "0", "Cam Pivot Y", true, true, 0, false);
	//cvarManager->registerCvar("GTAZCamTarget", "0", "Cam Pivot Z", true, true, 0, false);

	cvarManager->registerCvar("GTAFOV", "90", "Set the FOV of the camera", true, true, 0, false);
	cvarManager->registerCvar("GTAOrbitSpeed", "0.10", "Set the speed of the camera", true, true, 0, false);
}



void CinematicCam::CreateValues()
{

	if (orbitMode)
	{
		float height = cvarManager->getCvar("GTAHeight").getFloatValue();
		float distance = cvarManager->getCvar("GTADistance").getFloatValue();
		float speed = cvarManager->getCvar("GTAOrbitSpeed").getFloatValue();
		CarWrapper car = gameWrapper->GetLocalCar();
		CameraWrapper camera = gameWrapper->GetCamera();
		if (!car.IsNull() && !camera.IsNull())
		{
			Vector carLocation = car.GetLocation();
			float angleRadians = timeVal * 3.14 / 180.0;
			float newX = distance * cos(angleRadians);
			float newY = distance * sin(angleRadians);
			float dx = -newX;
			float dy = -newY;
			if (lockOnCar) {
				newX += carLocation.X;
				newY += carLocation.Y;
				dx = carLocation.X - newX;
				dy = carLocation.Y - newY;
			}

			FOCUS = Vector{ newX , newY, height };
			float dz = -height;

			double pitch = atan2(dz, sqrt(dx * dx + dy * dy)) * 32768.0 / 3.14;
			double yaw = atan2(dy, dx) * 32768.0 / 3.14;

			ROTATION.Pitch = pitch;
			ROTATION.Yaw = yaw;

			ProfileCameraSettings camSettings = camera.GetCameraSettings();
			DISTANCE = camSettings.Distance;
			FOV = camSettings.FOV;

			timeVal += speed;
		}
	}
	if (freecamMode) {
		float xpos = cvarManager->getCvar("GTAX").getFloatValue();
		float ypos = cvarManager->getCvar("GTAY").getFloatValue();
		float zpos = cvarManager->getCvar("GTAZ").getFloatValue();
		float xpivotpos = cvarManager->getCvar("GTAXCamTarget").getFloatValue();
		float ypivotpos = cvarManager->getCvar("GTAYCamTarget").getFloatValue();
		float zpivotpos = cvarManager->getCvar("GTAZCamTarget").getFloatValue();
		CarWrapper car = gameWrapper->GetLocalCar();
		CameraWrapper camera = gameWrapper->GetCamera();
		if (!car.IsNull() && !camera.IsNull())
		{
			Vector carLocation = car.GetLocation();
			// Camera position
			float newX = carLocation.X;
			float newY = carLocation.Y;
			float newZ = zpos;

			FOCUS = Vector{ newX , newY, newZ };

			// Point camera should look at
			float lookX = carLocation.X;
			float lookY = carLocation.Y;
			float lookZ = 0;

			////// 
			float dx = lookX - newX;
			float dy = lookY - newY;
			float dz = lookZ - newZ;

			double pitch = atan2(dz, sqrt(dx * dx + dy * dy)) * 32768.0 / 3.14;
			double yaw = atan2(dy, dx) * 32768.0 / 3.14;

			ROTATION.Pitch = pitch;
			ROTATION.Yaw = yaw;
			ProfileCameraSettings camSettings = camera.GetCameraSettings();
			DISTANCE = 0;
			FOV = camSettings.FOV;
		}
	}
}

ServerWrapper CinematicCam::GetCurrentGameState()
{
	if (gameWrapper->IsInReplay())
		return gameWrapper->GetGameEventAsReplay().memory_address;
	else if (gameWrapper->IsInOnlineGame())
		return gameWrapper->GetOnlineGame();
	else
		return gameWrapper->GetGameEventAsServer();
}




//LEAVE THESE UNCHANGED


void CinematicCam::onUnload() {}
void CinematicCam::Initialize()
{
	//Install parent plugin if it isn't already installed. Ensure parent plugin is loaded.
	if (!std::filesystem::exists(gameWrapper->GetBakkesModPath() / "plugins" / "CameraControl.dll"))
		cvarManager->executeCommand("bpm_install 71");
	cvarManager->executeCommand("plugin load CameraControl", false);

	//Hook events
	gameWrapper->HookEvent("Function ProjectX.Camera_X.ClampPOV", std::bind(&CinematicCam::HandleValues, this));
	gameWrapper->HookEvent("Function TAGame.PlayerController_TA.PressRearCamera", [&](std::string eventName) {isInRearCam = true; });
	gameWrapper->HookEvent("Function TAGame.PlayerController_TA.ReleaseRearCamera", [&](std::string eventName) {isInRearCam = false; });
	gameWrapper->HookEvent("Function TAGame.CameraState_BallCam_TA.BeginCameraState", [&](std::string eventName) {isInBallCam = true; });
	gameWrapper->HookEvent("Function TAGame.CameraState_BallCam_TA.EndCameraState", [&](std::string eventName) {isInBallCam = false; });
}
bool CinematicCam::CanCreateValues()
{
	if (!enabled || IsCVarNull("CamControl_Swivel_READONLY") || IsCVarNull("CamControl_Focus") || IsCVarNull("CamControl_Rotation") || IsCVarNull("CamControl_Distance") || IsCVarNull("CamControl_FOV"))
		return false;
	else
		return true;
}
bool CinematicCam::IsCVarNull(std::string cvarName)
{
	struct CastStructOne
	{
		struct CastStructTwo { void* address; };
		CastStructTwo* casttwo;
	};

	CVarWrapper cvar = cvarManager->getCvar(cvarName);
	CastStructOne* castone = (CastStructOne*)&cvar;
	return castone->casttwo->address == nullptr;
}
void CinematicCam::Enable()
{
	cvarManager->executeCommand("CamControl_Enable 1", false);
	enabled = true;
}
void CinematicCam::Disable()
{
	enabled = false;
	cvarManager->executeCommand("CamControl_Enable 0", false);
}
void CinematicCam::HandleValues()
{
	if (!CanCreateValues())
		return;

	//Reset values so that the game won't crash if the developer doesn't assign values to variables
	overrideValue[0] = true;//Focus X
	overrideValue[1] = true;//Focus Y
	overrideValue[2] = true;//Focus Z
	overrideValue[3] = true;//Rotation Pitch
	overrideValue[4] = true;//Rotation Yaw
	overrideValue[5] = true;//Rotation Roll
	overrideValue[6] = true;//Distance
	overrideValue[7] = true;//FOV

	SWIVEL = GetSwivel();
	FOCUS = Vector{ 0,0,0 };
	ROTATION = Rotator{ 0,0,0 };
	DISTANCE = 100;
	FOV = 90;

	//Get values from the developer
	CreateValues();

	//Send value requests to the parent mod
	std::string values[8];
	values[0] = std::to_string(FOCUS.X);
	values[1] = std::to_string(FOCUS.Y);
	values[2] = std::to_string(FOCUS.Z);
	values[3] = std::to_string(ROTATION.Pitch);
	values[4] = std::to_string(ROTATION.Yaw);
	values[5] = std::to_string(ROTATION.Roll);
	values[6] = std::to_string(DISTANCE);
	values[7] = std::to_string(FOV);

	for (int i = 0; i < 8; i++)
	{
		if (!overrideValue[i])
			values[i] = "NULL";
	}

	cvarManager->getCvar("CamControl_Focus").setValue(values[0] + "," + values[1] + "," + values[2]);
	cvarManager->getCvar("CamControl_Rotation").setValue(values[3] + "," + values[4] + "," + values[5]);
	cvarManager->getCvar("CamControl_Distance").setValue(values[6]);
	cvarManager->getCvar("CamControl_FOV").setValue(values[7]);
}
Rotator CinematicCam::GetSwivel()
{
	if (IsCVarNull("CamControl_Swivel_READONLY"))
		return Rotator{ 0,0,0 };

	std::string readSwivel = cvarManager->getCvar("CamControl_Swivel_READONLY").getStringValue();
	std::string swivelInputString;
	std::stringstream ssSwivel(readSwivel);

	Rotator SWIVEL = { 0,0,0 };

	getline(ssSwivel, swivelInputString, ',');
	SWIVEL.Pitch = stof(swivelInputString);
	getline(ssSwivel, swivelInputString, ',');
	SWIVEL.Yaw = stof(swivelInputString);
	getline(ssSwivel, swivelInputString, ',');
	SWIVEL.Roll = stof(swivelInputString);

	return SWIVEL;
}