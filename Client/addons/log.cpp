#include "log.h"
#include "../engine.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../menu.h"
#include "../settings.h"
#include "../util.h"
#include <time.h>
#include <fstream>

static int logKeybind = VK_J;
static bool isLogging = false;
static float logTimer = 0.1f;

template <typename T> static void AddToMsg(std::string& msg, T value, const char *format) {
    char buffer[0xFF];
    sprintf_s(buffer, format, value);
    msg += std::string(buffer) + ',';
}

static void LogMsg(std::string logMsg) {
    std::time_t t = std::time(0);
    std::tm time; 
    localtime_s(&time, &t);

    char formattedTime[0xFF];
    sprintf_s(formattedTime, sizeof(formattedTime), "%02d:%02d", time.tm_min, time.tm_sec);

    std::string filePath = "log.txt";
    std::ofstream os(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    os << formattedTime << ',' << logMsg << '\n';
    os.close();
}

static void LogTab() {
    if (ImGui::Hotkey("Toggle Logging##toggle-logging", &logKeybind)) {
        Settings::SetSetting("player", "logKeybind", logKeybind);
    }
}

static void OnTick(float deltaTime) {
    if (Engine::IsKeyDown(logKeybind)) {
        isLogging = !isLogging;
    }

    if (isLogging) {
        auto pawn = Engine::GetPlayerPawn();
        auto controller = Engine::GetPlayerController();

        if (!pawn || !controller)
            return;

        auto ms = pawn->MovementState.GetValue();
        auto p = pawn->Location;

        std::string msg = "";
        AddToMsg(msg, ms, "%d");
        AddToMsg(msg, p.X, "%.2f");
        AddToMsg(msg, p.Y, "%.2f");
        AddToMsg(msg, p.Z, "%.2f");

        LogMsg(msg);
    }
}

static void OnRender(IDirect3DDevice9 *) {
    if (isLogging) {
        const char *text = "Logging";
        const auto window = ImGui::BeginRawScene("##log-state");
        const auto &io = ImGui::GetIO();
        const auto width = ImGui::CalcTextSize(text, nullptr, false).x;
        static const float padding = 5.0f;

        window->DrawList->AddRectFilled(ImVec2(io.DisplaySize.x - width - padding, 0),
                                        ImVec2(io.DisplaySize.x, ImGui::GetTextLineHeight()),
                                        ImColor(ImVec4(0, 0, 0, 0.4f)));
        window->DrawList->AddText(ImVec2(io.DisplaySize.x - width, 0),
                                  ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), text);

        ImGui::EndRawScene();
    }
}

bool Log::Initialize() {
    logKeybind = Settings::GetSetting("player", "logKeybind", VK_J);
    isLogging = false;

    Menu::AddTab("Log", LogTab);
    Engine::OnTick(OnTick);
    Engine::OnRenderScene(OnRender);

    Engine::OnInput([](unsigned int &msg, int keycode) {
        if (msg == WM_KEYDOWN && keycode == logKeybind) {
            isLogging = !isLogging;
        }
    });

    return true;
}

std::string Log::GetName() { return "Log"; }
