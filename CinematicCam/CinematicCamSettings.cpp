#include "pch.h"
#include "CinematicCam.h"

void CinematicCam::RenderSettings() {
    ImGui::TextUnformatted("Cinematic camera modes that look cool");

    CVarWrapper enableCvar = cvarManager->getCvar("cinematic_cam_enabled");
    if (!enableCvar) { return; }
    if (ImGui::Button("Enable")) {
        enableCvar.setValue(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Disable")) {
        enableCvar.setValue(false);
    }

    //if (ImGui::Button("Enable HUD")) {
    //    cvarManager->executeCommand("cl_rendering_scaleform_disabled 0", true);
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Disable HUD")) {
    //    cvarManager->executeCommand("cl_rendering_scaleform_disabled 1", false);
    //}

    if (!enabled) {
        return;
    }

    ImGui::RadioButton("Orbit Mode", &cameraModeGUI, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Freecam Mode", &cameraModeGUI, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Window Mode", &cameraModeGUI, 2);
    ImGui::SameLine();
    ImGui::RadioButton("Trackcam Mode", &cameraModeGUI, 3);
    ImGui::SameLine();
    ImGui::RadioButton("Switching Mode", &cameraModeGUI, 4);

    if (cameraModeGUI == 0) {
        CVarWrapper enableCarLockCvar = cvarManager->getCvar("cam_car_lock");
        if (!enableCarLockCvar) { return; }
        bool enabledCarLock = enableCarLockCvar.getBoolValue();
        if (ImGui::Checkbox("Lock on car", &enabledCarLock)) {
            enableCarLockCvar.setValue(enabledCarLock);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Lock on car");
        }
        
        createDragSlider("CameraHeight", "Height", 0.1f,"height is");
        createDragSlider("CameraDistance", "Distance", 0.1f,"distance is");
        createDragSlider("CameraOrbitSpeed", "Speed", 0.01f,"Orbit speed is");
    }
    if (cameraModeGUI == 1) {

        CVarWrapper enableCarLockCvar = cvarManager->getCvar("cam_car_lock");
        if (!enableCarLockCvar) { return; }
        bool enabledCarLock = enableCarLockCvar.getBoolValue();
        if (ImGui::Checkbox("Lock on car", &enabledCarLock)) {
            enableCarLockCvar.setValue(enabledCarLock);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Lock on car");
        }

        createDragSlider("CameraX", "X", 10, "X is");
        createDragSlider("CameraY", "Y", 10, "Y is");
        createDragSlider("CameraZ", "Z", 10, "Z is");
    }
    if (cameraModeGUI == 2) {

        createDragSlider("CameraFOV", "FOV", 1, "fov is");

        createDragSlider("CameraXOffset", "xOffset", 0.1f, "xOffset is");
        createDragSlider("CameraYOffset", "yOffset", 0.1f, "yOffset is");
        createDragSlider("CameraZOffset", "zOffset", 0.1f, "zOffset is");

        createDragSlider("CameraPitchOffset", "PitchOffset", 1, "PitchOffset is");
        createDragSlider("CameraYawOffset", "YawOffset", 1, "YawOffset is");
        createDragSlider("CameraRollOffset", "RollOffset", 1, "RollOffset is");
    }
    if (cameraModeGUI == 3) {
        createDragSlider("CameraTrackX", "xOffset", 1, "xOffset is");
        createDragSlider("CameraTrackY", "yOffset", 1, "yOffset is");
        createDragSlider("CameraTrackZ", "zOffset", 1, "zOffset is");
    }
    if (cameraModeGUI == 4) {
        CVarWrapper enableCarLockCvar = cvarManager->getCvar("cam_car_lock");
        if (!enableCarLockCvar) { return; }
        bool enabledCarLock = enableCarLockCvar.getBoolValue();
        if (ImGui::Checkbox("Lock on car", &enabledCarLock)) {
            enableCarLockCvar.setValue(enabledCarLock);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Lock on car");
        }
    }
}

void CinematicCam::createDragSlider(std::string cvarname, const char* label, float dragspeed, std::string tooltip) {
    CVarWrapper cvarWrapper = cvarManager->getCvar(cvarname);
    if (!cvarWrapper) { return; }
    float cvar = cvarWrapper.getFloatValue();
    if (ImGui::DragFloat(label, &cvar, dragspeed)) {
        cvarWrapper.setValue(cvar);
    }
    if (ImGui::IsItemHovered()) {
        std::string hoverText = tooltip + std::to_string(cvar);
        ImGui::SetTooltip(hoverText.c_str());
    }
}