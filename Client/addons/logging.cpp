#include "logging.h"

#define NOMINMAX
#include "../engine.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"

#include "../date.h"
#include "../json.h"
#include "../menu.h"
#include "../settings.h"
#include "../util.h"
#include <chrono>
#include <fstream>
#include <time.h>

static json playerLog = json::array();
static int startLogKeybind = VK_J;
static int stopLogKeybind = VK_K;
static int saveLogKeybind = VK_L;
static int clearLogKeybind = VK_0;
static bool isLogging = false;
static float logInterval = 0.2f;
static float logTime = 0.f;
static std::string filePath = "log.json";

static float minDistance = 1;
static Classes::EMovement prevState;
static Classes::FVector prevLocation;

static std::string message = "Logging";
static float opacity = 0.f;

static void ShowMessage(std::string msg) {
    message = msg;
    opacity = 1.f;
}

static void Clear() { 
    playerLog = json::array();
    ShowMessage("Cleared Log!");
}

static void Save() {
    std::ofstream file(filePath, std::ios::out);
    if (!file) {
        printf("settings: failed to save %s\n", filePath.c_str());
        return;
    }

    auto dump = playerLog.dump(4);
    file.write(dump.c_str(), dump.size());
    file.close();

    ShowMessage("Log Saved!");
}

static void AddEntry(Classes::EMovement state, Classes::FVector location) {
    auto now = std::chrono::system_clock::now();
    std::string time = date::format("%T", date::floor<std::chrono::milliseconds>(now));

    json entry;
    entry["timestamp"] = time;
    entry["player_state"] = state;
    entry["location"]["x"] = location.X * 0.01f;
    entry["location"]["y"] = location.Y * 0.01f;
    entry["location"]["z"] = location.Z * 0.01f;

    playerLog.emplace_back(entry);
}

static void LogPlayerState(float deltaTime) {
    bool doLog = false;

    auto pawn = Engine::GetPlayerPawn();
    auto controller = Engine::GetPlayerController();

    if (!pawn || !controller)
        return;

    auto currState = pawn->MovementState.GetValue();
    auto currLocation = pawn->Location;

    if (prevState != currState) {
        prevState = currState;
        doLog = true;
    }

    logTime += deltaTime;

    if (logTime >= logInterval) {
        logTime = 0.f;

        if (Distance(prevLocation, currLocation) >= minDistance) {
            prevLocation = currLocation;
            doLog = true;
        }
    }

    if (doLog) {
        AddEntry(currState, currLocation);
        ShowMessage("Logging Player State");
    }
}

static void LogTab() {
    if (ImGui::Hotkey("Start Logging##start-logging", &startLogKeybind)) {
        Settings::SetSetting("logging", "startLogKeybind", startLogKeybind);
    }

    if (ImGui::Hotkey("Stop Logging##stop-logging", &stopLogKeybind)) {
        Settings::SetSetting("logging", "stopLogKeybind", stopLogKeybind);
    }

    if (ImGui::Hotkey("Save Log##save-logging", &saveLogKeybind)) {
        Settings::SetSetting("logging", "saveLogKeybind", saveLogKeybind);
    }

    if (ImGui::Hotkey("Clear Log##save-logging", &clearLogKeybind)) {
        Settings::SetSetting("logging", "clearLogKeybind", clearLogKeybind);
    }

    ImGui::SeperatorWithPadding(2.5f);

    if (ImGui::InputFloat("Log Interval", &logInterval)) {
        Settings::SetSetting("logging", "logInterval", logInterval);
    }

    if (ImGui::InputFloat("Min Distance", &minDistance)) {
        Settings::SetSetting("logging", "minDistance", minDistance);
    }
    
    ImGui::SeperatorWithPadding(2.5f);

    if (ImGui::InputText("File Path", &filePath)) {
        Settings::SetSetting("logging", "filePath", filePath);
    }
}

static void OnTick(float deltaTime) {
    if (isLogging)
        LogPlayerState(deltaTime);

    if (opacity > 0.0)
        opacity -= deltaTime * 0.5f;
}

static void OnRender(IDirect3DDevice9 *) {
    if (opacity > 0.0) {

        const auto window = ImGui::BeginRawScene("##logging");
        const auto &io = ImGui::GetIO();
        const auto size = ImGui::CalcTextSize(message.c_str(), nullptr, false);
        static const float padding = 5.0f;

        window->DrawList->AddRectFilled(ImVec2(io.DisplaySize.x - size.x - padding, 0),
                                        ImVec2(io.DisplaySize.x, ImGui::GetTextLineHeight() * 1.2f),
                                        ImColor(ImVec4(0, 0, 0, 0.4f * opacity)));
        window->DrawList->AddText(ImVec2(io.DisplaySize.x - size.x, 0),
                                  ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f * opacity)),
                                  message.c_str());

        ImGui::EndRawScene();
    }
}

bool Logging::Initialize() {
    startLogKeybind = Settings::GetSetting("logging", "startLogKeybind", VK_J);
    stopLogKeybind = Settings::GetSetting("logging", "stopLogKeybind", VK_K);
    saveLogKeybind = Settings::GetSetting("logging", "saveLogKeybind", VK_L);
    clearLogKeybind = Settings::GetSetting("logging", "clearLogKeybind", VK_0);

    logInterval = Settings::GetSetting("logging", "logInterval", 0.2f);
    minDistance = Settings::GetSetting("logging", "minDistance", 1.f);

    filePath = Settings::GetSetting("logging", "filePath", "log.json").dump();
    filePath.erase(std::remove(filePath.begin(), filePath.end(), '"'), filePath.end());

    Menu::AddTab(GetName().c_str(), LogTab);
    Engine::OnTick(OnTick);
    Engine::OnRenderScene(OnRender);


    Engine::OnInput([](unsigned int &msg, int keycode) {
        if (msg == WM_KEYDOWN && keycode == startLogKeybind) {
            isLogging = true;
            ShowMessage("Start Logging");
        }
        if (msg == WM_KEYDOWN && keycode == stopLogKeybind) {
            isLogging = false;
            ShowMessage("Stop Logging");
        }
        if (msg == WM_KEYDOWN && keycode == saveLogKeybind) {
            Save();
        }
        if (msg == WM_KEYDOWN && keycode == clearLogKeybind) {
            Clear();
        }
    });

    return true;
}

std::string Logging::GetName() { return "Logging"; }
