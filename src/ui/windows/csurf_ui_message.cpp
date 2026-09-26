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

            ImGui::Text(m_ctx, "This release is a bugfix release:");
            ImGui::BulletText(
                m_ctx, "Fixed issue where plugin mapping values were not saved correct");

            ReaSonusPageTitle(m_ctx, assets, "Version 0.11.0, 0.11.1", true);
            ImGui::Text(m_ctx, "Forgot to update these during the previous release. But here they are:");
            ImGui::BulletText(
                m_ctx,
                "Single Point Automation; With single point automation you set an envelope point while touching the fader. You can then adjust the fader to where you want it to be and release it. On release another envelope point will get set. This will reduce the amount of automation points. Check the Automation settings in the reaSonus Control Panel for settings and info. (Implementation for the FaderPort V2 is on the road map)");
            ImGui::BulletText(
                m_ctx,
                "Able to create project specific filters; The custom filters can now also be stored per project. Above the filter list select `Project Filter`. The filters added here will be saved in the project. This way you can also add custom filters to project templates ");
            ImGui::BulletText(
                m_ctx,
                "Add option to set colors to custom filters; You can now set a color per filter. This makes selecting filters from your list a bit easier. "
            );
            ImGui::BulletText(
                m_ctx,
                "Add option to set custom colors to the automation buttons: In the Automation settings in the ReaSonus Control Panel select the colors you like for use with the automation buttons. This way you can align them better with your Reaper theme colors. "
            );
            ImGui::BulletText(
                m_ctx,
                "Overhaul of the settings page; Added an extra tab for automation, moved some settings around and implemented a better grouping for a better overview (Implementation for the FaderPort V2 is on the road map)"
            );
            ImGui::BulletText(
                m_ctx,
                "Add extension info for the new extension screen in the REAPER settings (from v7.79)"
            );
            ImGui::Text(m_ctx, " ");
            ImGui::Text(m_ctx, "Check the documentation for more info on these changes");
            ImGui::TextLinkOpenURL(
                m_ctx,
                "Documentation about Single Point Automation",
                "https://reasonus.net/documentation/faderport8/#automation"
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
