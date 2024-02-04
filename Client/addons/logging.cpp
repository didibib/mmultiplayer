#include "logging.h"

#include "../engine.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"

#include "../menu.h"
#include "../settings.h"
#include "../util.h"
#include <fstream>
#include <time.h>
#include <chrono>

static int startLogKeybind = VK_J;
static int stopLogKeybind = VK_K;
static bool isLogging = false;
static float logInterval = 0.2f;
static float logTime = 0.f;
static std::string filePath = "log.txt";
static std::string lastLogMsg;
static Classes::EMovement prevState;

static void ClearLog( )
{
    std::ofstream ofs;
    ofs.open(filePath, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

template <typename T> static void AddToMsg(std::string &msg, T value, const char *format) {
    char buffer[0xFF];
    sprintf_s(buffer, format, value);
    msg += std::string(buffer) + ',';
}

static void LogMsg(std::string logMsg) {
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000;
    std::time_t t = std::time(0);
    std::tm time;
    localtime_s(&time, &t);

    char formattedTime[sizeof "59:59:9999"];
    sprintf_s(formattedTime, sizeof(formattedTime), "%02d:%02d:%lld", time.tm_min, time.tm_sec, millis);

    std::ofstream os(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    os << formattedTime << ',' << logMsg << '\n';
    os.close();
}

static void LogPlayerState(float deltaTime) {
    bool doLog = false;

    auto pawn = Engine::GetPlayerPawn();
    auto controller = Engine::GetPlayerController();
    auto ms = pawn->MovementState.GetValue();
    auto loc = pawn->Location;

    if (!pawn || !controller)
        return;

    if (prevState != ms) {
        doLog = true;
    }

    logTime += deltaTime;

    if (logTime >= logInterval) {
        logTime = 0.f;
        doLog = true;
    }

    if (doLog) {
        std::string msg = "";
        AddToMsg(msg, ms, "%d");
        AddToMsg(msg, loc.X, "%.2f");
        AddToMsg(msg, loc.Y, "%.2f");
        AddToMsg(msg, loc.Z, "%.2f");

        lastLogMsg = msg;
        LogMsg(msg);
    }
}

static void LogTab() {
    if (ImGui::Hotkey("Start Logging##start-logging", &startLogKeybind)) {
        Settings::SetSetting("Logging", "StartLogKeybind", startLogKeybind);
    }

    if (ImGui::Hotkey("Stop Logging##stop-logging", &stopLogKeybind)) {
        Settings::SetSetting("Logging", "StopLogKeybind", stopLogKeybind);
    }

    
    if (ImGui::InputFloat("Log Interval", &logInterval)) {
        Settings::SetSetting("Logging", "LogInterval", logInterval);
    }

    if (!isLogging) {
        if (ImGui::InputText("File Path", &filePath)) {
            Settings::SetSetting("Logging", "FilePath", filePath);
        }
    }

    if (ImGui::Button("Clear Log")) {
        ClearLog();
    }
}

static void OnTick(float deltaTime) {
    if (isLogging)
        LogPlayerState(deltaTime);
}

static void OnRender(IDirect3DDevice9 *) {
    if (isLogging) {

        const std::string text = "Logging\n" + lastLogMsg;
        const auto window = ImGui::BeginRawScene("##log-state");
        const auto &io = ImGui::GetIO();
        const auto size = ImGui::CalcTextSize(text.c_str(), nullptr, false);
        static const float padding = 5.0f;

        window->DrawList->AddRectFilled(ImVec2(io.DisplaySize.x - size.x - padding, 0),
                                        ImVec2(io.DisplaySize.x, ImGui::GetTextLineHeight() * 2),
                                        ImColor(ImVec4(0, 0, 0, 0.4f)));
        window->DrawList->AddText(ImVec2(io.DisplaySize.x - size.x, 0),
                                  ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), text.c_str());

        ImGui::EndRawScene();
    }
}

bool Logging::Initialize() {
    startLogKeybind = Settings::GetSetting("Logging", "StartLogKeybind", VK_J);
    stopLogKeybind = Settings::GetSetting("Logging", "StopLogKeybind", VK_K);
    logInterval = Settings::GetSetting("Logging", "LogInterval", 0.2f);
    filePath = Settings::GetSetting("Logging", "FilePath", "log.txt").dump();
    filePath.erase(std::remove(filePath.begin(), filePath.end(), '"'), filePath.end());
    isLogging = false;

    Menu::AddTab("Log", LogTab);
    Engine::OnTick(OnTick);
    Engine::OnRenderScene(OnRender);

    Engine::OnInput([](unsigned int &msg, int keycode) {
        if (msg == WM_KEYDOWN && keycode == startLogKeybind) {
            isLogging = true;
        }
        if (msg == WM_KEYDOWN && keycode == stopLogKeybind) {
            isLogging = false;
        }
    });

    return true;
}

std::string Logging::GetName() { return "Logging"; }
