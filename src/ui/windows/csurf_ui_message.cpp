#include "config.h"
#include "csurf_ui_plugin_type_mapping.hpp"
#include "csurf_ui_message.hpp"
#include "../csurf_ui_colors.hpp"
#include "../csurf_ui_vars.hpp"
#include "../components/csurf_ui_page_title.hpp"
#include "../components/csurf_ui_button_bar.hpp"
#include "../../shared/csurf_utils.hpp"

constexpr auto g_name{"ReaSonus Native New Version"};

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
    if (close_clicked) {
        close_clicked = false;
        Stop();
        return;
    }

    if (save_clicked) {
        save_clicked = false;
        ReaSonusPluginTypeMapping::Start();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    bool open{true};

    UiStyledElements::PushReaSonusWindowStyle(m_ctx);
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginChild(m_ctx, "logo", 640.0, 52.0, ImGui::ChildFlags_None)) {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);
            ImGui::SameLine(m_ctx);

            ImGui::EndChild(m_ctx); // logo
        }

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(
            m_ctx,
            "actions_container",
            0.0,
            0.0,
            ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
        )) {
            ReaSonusPageTitle(m_ctx, assets, "ReaSonus Native New Version: " + std::string(GIT_VERSION), false);
            ImGui::PushTextWrapPos(m_ctx, 0.0);

            ImGui::Text(m_ctx, "This release has the following changes:");
            ImGui::BulletText(
                m_ctx,
                "Respect plugin and send slots. This can be enabled in the settings under the Controller and Plugin Control tabs");

            ImGui::BulletText(
                m_ctx,
                "MIDI device disable check: Added MIDI-device disabled check in the setup window (REAPER settings)");

            ImGui::BulletText(
                m_ctx,
                "Settings to start with track mode for send, receive and plugin control. This can be enabled in the settings under the Controller and Plugin Control tabs");

            ImGui::BulletText(
                m_ctx,
                "Custom Forward and Rewind actions. In the ReaSonus Control Panel  under the Function Keys you can now set custom actions for the Forward and Rewind buttons");

            ImGui::BulletText(
                m_ctx,
                "Control input plugins: Added the option to control the input plugins");

            ImGui::BulletText(
                m_ctx,
                "3th line  in Send mode configurable. In the ReaSonus Control pannel under Settings > Displays, you can now set the value for the third line in send mode");

            ImGui::Text(m_ctx, " ");
            ImGui::Text(m_ctx, "Check the documentation for more info on these changes");
            ImGui::TextLinkOpenURL(
                m_ctx,
                "Check out the documentation for more info",
                "https://reasonus.net/documentation"
            );

            ImGui::PopTextWrapPos(m_ctx);
            UiStyledElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ReaSonusButtonBar(
            m_ctx,
            assets,
            "Close",
            &close_clicked,
            false,
            &close_clicked,
            "Close",
            &save_message);

        UiStyledElements::PopReaSonusWindowStyle(m_ctx);

        ImGui::End(m_ctx); // window
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open) {
        window_open = false;
        SetActionState("_REASONUS_TRANSLATIONN_EDITOR");
        s_inst.reset();
    }
}
