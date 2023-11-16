#include "log.h"
#include "../engine.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../menu.h"
#include "../settings.h"
#include "../util.h"

static int logPlayerStateKeybind = VK_I;
static int savePlayerStateKeybind = VK_O;
static bool isLoggingPlayerState = false;

static void LogTab() {
    if (ImGui::Hotkey("Log Player State##log-player-state", &logPlayerStateKeybind)) {
        Settings::SetSetting("player", "logPlayerStateKeybind", logPlayerStateKeybind);
    }

    if (ImGui::Hotkey("Save Player State##save-player-state", &savePlayerStateKeybind)) {
        Settings::SetSetting("player", "savePlayerStateKeybind", savePlayerStateKeybind);
    }
}

static void OnTick(float deltaTime) {
    if (Engine::IsKeyDown(logPlayerStateKeybind)) {
        isLoggingPlayerState = !isLoggingPlayerState;
    }

    if (isLoggingPlayerState) {
        
    }
}

static void OnRender(IDirect3DDevice9 *) {
    if (isLoggingPlayerState) {
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
    logPlayerStateKeybind = Settings::GetSetting("player", "logPlayerStateKeybind", VK_I);
    savePlayerStateKeybind = Settings::GetSetting("player", "savePlayerStateKeybind", VK_O);
    isLoggingPlayerState = false;

    Menu::AddTab("Log", LogTab);
    Engine::OnTick(OnTick);
    Engine::OnRenderScene(OnRender);
    
    Engine::OnInput([](unsigned int &msg, int keycode) {
        if (msg == WM_KEYDOWN && keycode == logPlayerStateKeybind) {
            isLoggingPlayerState = !isLoggingPlayerState;
        }
    });

    return true;
}

std::string Log::GetName() { return "Log"; }
