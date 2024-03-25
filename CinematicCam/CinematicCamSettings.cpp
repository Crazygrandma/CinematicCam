#include "pch.h"
#include "CinematicCam.h"

void CinematicCam::RenderSettings() {
    ImGui::TextUnformatted("Let the camera orbit around your car");

    CVarWrapper enableCvar = cvarManager->getCvar("gta_enabled");
    if (!enableCvar) { return; }
    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable plugin", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle Plugin");
    }

    CVarWrapper orbitModeCvar = cvarManager->getCvar("orbit_mode");
    if (!orbitModeCvar) { return; }
    bool enabledOrbitMode = orbitModeCvar.getBoolValue();
    if (ImGui::Checkbox("Orbit mode", &enabledOrbitMode)) {
        orbitModeCvar.setValue(enabledOrbitMode);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle orbit mode");
    }

    /*   CVarWrapper freecamModeCvar = cvarManager->getCvar("freecam_mode");
       if (!freecamModeCvar) { return; }
       bool enabledFreeCam = freecamModeCvar.getBoolValue();
       if (ImGui::Checkbox("Freecam mode", &enabledFreeCam)) {
           freecamModeCvar.setValue(enabledFreeCam);
       }
       if (ImGui::IsItemHovered()) {
           ImGui::SetTooltip("Toggle freecam mode");
       }*/

    if (orbitMode)
    {
        CVarWrapper enableCarLockCvar = cvarManager->getCvar("gta_car_lock");
        if (!enableCarLockCvar) { return; }
        bool enabledCarLock = enableCarLockCvar.getBoolValue();
        if (ImGui::Checkbox("Lock on car", &enabledCarLock)) {
            enableCarLockCvar.setValue(enabledCarLock);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Lock on car");
        }
        createFloatSlider("GTADistance", 0.0, 10000.0, "Distance", "distance is");
        createFloatSlider("GTAHeight", 0.0, 5000.0, "Height", "height is");
        createFloatSlider("GTAOrbitSpeed", 0.0, 1.0, "Speed", "Orbit speed is");
        //createIntSlider("GTAFOV", 0, 150, "FOV", "fov is");
    }
    //if (freecamMode) {

    //    CVarWrapper enableCarLockCvar = cvarManager->getCvar("gta_car_lock");
    //    if (!enableCarLockCvar) { return; }
    //    bool enabledCarLock = enableCarLockCvar.getBoolValue();
    //    if (ImGui::Checkbox("Enable lock on car", &enabledCarLock)) {
    //        enableCarLockCvar.setValue(enabledCarLock);
    //    }
    //    if (ImGui::IsItemHovered()) {
    //        ImGui::SetTooltip("Lock on car");
    //    }
    //    /*float val;
    //    ImGui::DragFloat("drag int",&val);*/
    //    createFloatSlider("GTAX", 0, 10.0, "X", "X is");
    //    createFloatSlider("GTAY", 0, 10.0, "Y", "Y is");
    //    createFloatSlider("GTAZ", 0, 1000.0, "Z", "Z is");

    //    createFloatSlider("GTAXCamTarget", 0, 10.0, "CAMX", "X is");
    //    createFloatSlider("GTAYCamTarget", 0, 10.0, "CAMY", "Y is");
    //    createFloatSlider("GTAZCamTarget", 0, 10.0, "CAMZ", "Z is");
    //}

}

void CinematicCam::createFloatSlider(std::string cvarname, float min, float max, const char* label, std::string tooltip) {
    CVarWrapper cvarWrapper = cvarManager->getCvar(cvarname);
    if (!cvarWrapper) { return; }
    float cvar = cvarWrapper.getFloatValue();
    if (ImGui::SliderFloat(label, &cvar, min, max)) {
        cvarWrapper.setValue(cvar);
    }
    if (ImGui::IsItemHovered()) {
        std::string hoverText = tooltip + std::to_string(cvar);
        ImGui::SetTooltip(hoverText.c_str());
    }
}

void CinematicCam::createIntSlider(std::string cvarname, int min, int max, const char* label, std::string tooltip) {
    CVarWrapper cvarWrapper = cvarManager->getCvar(cvarname);
    if (!cvarWrapper) { return; }
    int cvar = cvarWrapper.getIntValue();
    if (ImGui::SliderInt(label, &cvar, min, max)) {
        cvarWrapper.setValue(cvar);
    }
    if (ImGui::IsItemHovered()) {
        std::string hoverText = tooltip + std::to_string(cvar);
        ImGui::SetTooltip(hoverText.c_str());
    }
}