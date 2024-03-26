#include "pch.h"
#include "CinematicCam.h"
#include "bakkesmod\wrappers\includes.h"
#include <sstream>
#include <filesystem>
#include <cmath>
#include <chrono>


BAKKESMOD_PLUGIN(CinematicCam, "Cinematic Cam Plugin", "1.0", PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


auto lastTriggerTime = std::chrono::steady_clock::now();
void CinematicCam::onLoad()
{
	_globalCvarManager = cvarManager;
	Initialize();
	lastTriggerTime = std::chrono::steady_clock::now();
	cvarManager->registerCvar("cinematic_cam_enabled", "0", "Enable Cinematic Camera", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		cvar.getBoolValue() ? Enable() : Disable();
			});

	cvarManager->registerCvar("cam_car_lock", "0", "Lock to car", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		lockOnCar = cvar.getBoolValue();
			});

	cvarManager->registerCvar("cinematic_camera_mode", "0", "Camera mode", true, true, 0)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		cameraModeGUI = cvar.getIntValue();
			});

	cvarManager->registerCvar("CameraHeight", "500", "Set the height of the camera", true);
	cvarManager->registerCvar("CameraDistance", "1500", "Set the distance relative to the car of the camera", true);
	cvarManager->registerCvar("CameraOrbitSpeed", "0.02", "Set the speed of the camera", true);

	cvarManager->registerCvar("CameraFOV", "70", "Set the speed of the camera", true);

	cvarManager->registerCvar("CameraXOffset", "-40", "Set the xOffset of the camera", true);
	cvarManager->registerCvar("CameraYOffset", "35", "Set the yOffset of the camera", true);
	cvarManager->registerCvar("CameraZOffset", "25", "Set the zOffset of the camera", true);

	cvarManager->registerCvar("CameraTrackX", "3800", "Set the xOffset of the camera", true);
	cvarManager->registerCvar("CameraTrackY", "0", "Set the yOffset of the camera", true);
	cvarManager->registerCvar("CameraTrackZ", "250", "Set the zOffset of the camera", true);

	cvarManager->registerCvar("CameraX", "0", "Set the xOffset of the camera", true);
	cvarManager->registerCvar("CameraY", "4000", "Set the yOffset of the camera", true);
	cvarManager->registerCvar("CameraZ", "500", "Set the zOffset of the camera", true);

	cvarManager->registerCvar("CameraPitchOffset", "0", "Set the pitchOffset of the camera", true);
	cvarManager->registerCvar("CameraYawOffset", "0", "Set the yawOffset of the camera", true);
	cvarManager->registerCvar("CameraRollOffset", "0", "Set the rollOffset of the camera", true);
}


void CinematicCam::CreateValues()
{
	CarWrapper car = gameWrapper->GetLocalCar();
	CameraWrapper camera = gameWrapper->GetCamera();
	if (!car.IsNull() && !camera.IsNull())
	{
		if (cameraModeGUI == 0) {
			orbitMode(car, camera);
		}
		if (cameraModeGUI == 1) {
			freecamMode(car, camera);
		}
		if (cameraModeGUI == 2) {
			windowMode(car, camera);
		}
		if (cameraModeGUI == 3) {
			trackcamMode(car, camera);
		}
		if (cameraModeGUI == 4) {
			auto currentTime = std::chrono::steady_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTriggerTime).count();

			switch (currentMode)
			{
			case 0: {
				orbitMode(car, camera);
				break;
			}
			//case 1: {
			//	freecamMode(car, camera);
			//	break;
			//}
			case 1: {
				windowMode(car, camera);
				break;
			}
			case 2: {
				trackcamMode(car, camera);
				break;
			}
			default:
				break;
			}


			if (elapsedTime >= switchInterval) {
				currentMode += 1;
				if (currentMode == 3) {
					currentMode = 0;
				}
				lastTriggerTime = currentTime;
			}
		}
	}
}

void CinematicCam::freecamMode(CarWrapper car, CameraWrapper camera) {

	float camX = cvarManager->getCvar("CameraX").getFloatValue();
	float camY = cvarManager->getCvar("CameraY").getFloatValue();
	float camZ = cvarManager->getCvar("CameraZ").getFloatValue();

	Vector carLocation = car.GetLocation();

	FOCUS = Vector{ camX,camY,camZ };

	if (lockOnCar) {

		auto [pitch, yaw] = lookAt(FOCUS, carLocation);
		ROTATION = Rotator{ (int)(pitch), (int)(yaw), 0 };
	}
	else {

		auto [pitch, yaw] = lookAt(FOCUS, Vector{0,0,0});
		ROTATION = Rotator{ (int)(pitch), (int)(yaw), 0 };
	}


	DISTANCE = 0;
	FOV = 60;
}

void CinematicCam::trackcamMode(CarWrapper car, CameraWrapper camera) {

	float xOffset = cvarManager->getCvar("CameraTrackX").getFloatValue();
	float yOffset = cvarManager->getCvar("CameraTrackY").getFloatValue();
	float zOffset = cvarManager->getCvar("CameraTrackZ").getFloatValue();

	Vector carLocation = car.GetLocation();

	FOCUS = Vector{ xOffset,yOffset,zOffset };

	auto [pitch, yaw] = lookAt(FOCUS, carLocation);
	
	ROTATION = Rotator{ (int)(pitch), (int)(yaw), 0 };

	DISTANCE = 0;
	FOV = 60;
}

void CinematicCam::windowMode(CarWrapper car, CameraWrapper camera) {

	float xOffset = cvarManager->getCvar("CameraXOffset").getFloatValue();
	float yOffset = cvarManager->getCvar("CameraYOffset").getFloatValue();
	float zOffset = cvarManager->getCvar("CameraZOffset").getFloatValue();

	float pitchOffset = cvarManager->getCvar("CameraPitchOffset").getIntValue();
	float yawOffset = cvarManager->getCvar("CameraYawOffset").getIntValue();
	float rollOffset = cvarManager->getCvar("CameraRollOffset").getIntValue();


	float fov = cvarManager->getCvar("CameraFOV").getIntValue();


	Vector carLocation = car.GetLocation();
	Rotator carRotation = car.GetRotation();
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	BallWrapper ball = server.GetBall();
	Vector ballLocation = ball.GetLocation();

	float rotationYaw = carRotation.Yaw * 3.14159 / 32768;
	float rotationPitch = carRotation.Pitch * 3.14159 / 32768;
	float rotationRoll = carRotation.Roll * 3.14159 / 32768;

	// Calculate camera position relative to car's orientation
	float camPosX = carLocation.X +
		(xOffset * cos(rotationPitch) * cos(rotationYaw)) -
		(yOffset * (cos(rotationRoll) * sin(rotationYaw) - sin(rotationRoll) * sin(rotationPitch) * cos(rotationYaw))) +
		(zOffset * (sin(rotationRoll) * sin(rotationYaw) + cos(rotationRoll) * sin(rotationPitch) * cos(rotationYaw)));

	float camPosY = carLocation.Y +
		(xOffset * cos(rotationPitch) * sin(rotationYaw)) +
		(yOffset * (cos(rotationRoll) * cos(rotationYaw) + sin(rotationRoll) * sin(rotationPitch) * sin(rotationYaw))) -
		(zOffset * (sin(rotationRoll) * cos(rotationYaw) - cos(rotationRoll) * sin(rotationPitch) * sin(rotationYaw)));

	float camPosZ = carLocation.Z +
		(-xOffset * sin(rotationPitch)) +
		(yOffset * sin(rotationRoll) * cos(rotationPitch)) +
		(zOffset * cos(rotationRoll) * cos(rotationPitch));

	FOCUS = Vector{ camPosX,camPosY,camPosZ };

	if (isInBallCam) {
		auto [pitch, yaw] = lookAt(FOCUS, ballLocation);
		ROTATION = Rotator{ (int)(pitch), (int)(yaw), (int)(rollOffset) };
	}
	else
	{
		ROTATION = Rotator{ (int)(carRotation.Pitch + pitchOffset), (int)(carRotation.Yaw + yawOffset), (int)(carRotation.Roll + rollOffset) };
	}

	DISTANCE = 0;
	FOV = fov;

}

void CinematicCam::orbitMode(CarWrapper car, CameraWrapper camera) {

	float height = cvarManager->getCvar("CameraHeight").getFloatValue();
	float distance = cvarManager->getCvar("CameraDistance").getFloatValue();
	float speed = cvarManager->getCvar("CameraOrbitSpeed").getFloatValue();

	ProfileCameraSettings camSettings = camera.GetCameraSettings();
	Vector carLocation = car.GetLocation();
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	BallWrapper ball = server.GetBall();
	Vector ballLocation = ball.GetLocation();

	float newX = distance * cos(timeVal * 3.14 / 180.0);
	float newY = distance * sin(timeVal * 3.14 / 180.0);

	Vector camPos = { newX,  newY, height };

	auto [pitch, yaw] = lookAt(camPos, Vector{0,0,0});
	if (lockOnCar) {

		if (isInBallCam) {
			auto [pitch, yaw] = lookAt(camPos, ballLocation);
			camPos.X += ballLocation.X;
			camPos.Y += ballLocation.Y;
			camPos.Z += ballLocation.Z;
		}
		else{
		camPos.X += carLocation.X;
		camPos.Y += carLocation.Y;
		camPos.Z += carLocation.Z;
		auto [pitch, yaw] = lookAt(camPos, carLocation);
		}

	}

	FOCUS = Vector{ camPos.X , camPos.Y, camPos.Z};

	ROTATION.Pitch = pitch;
	ROTATION.Yaw = yaw;

	DISTANCE = distance;
	FOV = camSettings.FOV;

	timeVal += speed;
}


std::pair<float,float> CinematicCam::lookAt(Vector from, Vector to) {

	float dx = to.X - from.X;
	float dy = to.Y - from.Y;
	float dz = to.Z - from.Z;

	float pitch = atan2(dz, sqrt(dx * dx + dy * dy)) * 32768.0 / 3.14159;
	float yaw = atan2(dy, dx) * 32768.0 / 3.14159;;
	return std::make_pair(pitch, yaw);
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