#include "config.h"
#include "csurf_ui_plugin_type_mapping.hpp"
#include "csurf_ui_message.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_vars.hpp"
#include "csurf_ui_page_title.hpp"
#include "csurf_ui_button_bar.hpp"
#include "../shared/csurf_utils.hpp"

constexpr const char *g_name{"ReaSonus Native New Version"};

std::unique_ptr<ReaSonusMessage> ReaSonusMessage::s_inst;

static void reportError(const ImGui_Error &e) {
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusMessage::ReaSonusMessage()
    : m_ctx{} {
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    assets = new CSurf_UI_Assets(m_ctx);

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusMessage::~ReaSonusMessage() {
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusMessage::Start() {
    try {
        window_open = true;
        if (s_inst) {
            ImGui::SetNextWindowFocus(s_inst->m_ctx);
        } else {
            s_inst.reset(new ReaSonusMessage);
        }
    } catch (const ImGui_Error &e) {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusMessage::Stop() {
    try {
        window_open = false;
        if (s_inst) {
            s_inst.reset();
        }
    } catch (const ImGui_Error &e) {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusMessage::Loop() {
    try {
        s_inst->Frame();
    } catch (const ImGui_Error &e) {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusMessage::Frame() {
    double width, height;

    if (close_clicked) {
        close_clicked = false;
        return Stop();
    }

    if (save_clicked) {
        save_clicked = false;
        ReaSonusPluginTypeMapping::Start();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    bool open{true};

    UiElements::PushReaSonusWindowStyle(m_ctx);
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginChild(m_ctx, "logo", 640.0, 52.0, ImGui::ChildFlags_None)) {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);
            ImGui::SameLine(m_ctx);

            ImGui::EndChild(m_ctx); // logo
        }

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "actions_container", 0.0, 0.0,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            ReaSonusPageTitle(m_ctx, assets, "ReaSonus Native New Version: " + std::string(GIT_VERSION));
            ImGui::GetContentRegionAvail(m_ctx, &width, &height);
            ImGui::PushTextWrapPos(m_ctx, width - 24);

            ImGui::Text(m_ctx, "This version fixed some bugs:");
            ImGui::BulletText(m_ctx, "Fixed issue where the selected track was not visible in the mixer");
            ImGui::BulletText(m_ctx, "Fixed issue where REAPER could crash when no translation file was loaded yet");
            ImGui::BulletText(m_ctx, "Some code cleanup");
            ImGui::BulletText(
                m_ctx, "(FP 16) Disable the last touched fader mode when switching to any plugin related channel mode");
            ImGui::BulletText(m_ctx, "(FP V2) Fixed issue where disabled fader was not working");

            ImGui::Separator(m_ctx);
            ImGui::Text(m_ctx, "Message from 0.8.0, but keep it here until the next version");
            ImGui::Text(
                m_ctx,
                "When creating a plugin mapping, the plugin type was not taken in account. This can potentially cause issues as the mapping between plugin types can differ");
            ImGui::Text(
                m_ctx,
                "There has been created a Plugin Type Mapping tool to help you set the type for your mappings. The possitive is you only have to do this once. Click the `Open type Mapping` button below to start the mapping.");
            ImGui::Text(m_ctx, "The tool is also available in the actions list (search for: 'plugin type`)");

            ImGui::PopTextWrapPos(m_ctx);
            UiElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ReaSonusButtonBar(
            m_ctx,
            assets,
            "Open Type Mapping",
            &save_clicked,
            true,
            &close_clicked,
            "Close",
            &save_message);

        UiElements::PopReaSonusWindowStyle(m_ctx);

        ImGui::End(m_ctx); // window
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open) {
        window_open = false;
        SetActionState("_REASONUS_TRANSLATIONN_EDITOR");
        return s_inst.reset();
    }
}
