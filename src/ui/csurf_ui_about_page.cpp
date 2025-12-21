#include "csurf_ui_page_content.hpp"
#include <string.h>
#include <reaper_imgui_functions.h>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_page_title.hpp"
#include "../i18n/i18n.hpp"

class CSurf_UI_AboutPage : public CSurf_UI_PageContent
{
protected:
    std::string device;
    ImGui_Font *main_font_bold;
    I18n *i18n = I18n::GetInstance();

public:
    CSurf_UI_AboutPage(ImGui_Context *m_ctx, std::string _device) : CSurf_UI_PageContent(m_ctx)
    {
        device = _device;
        main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    };

    virtual ~CSurf_UI_AboutPage() {};

    void Render() override
    {
        double available_width, available_height;

        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
        if (ImGui::BeginChild(m_ctx, "main_about_page", 0.0, 0.0, ImGui::ChildFlags_None))
        {
            UiElements::PushReaSonusGroupStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "about", 256.0, 0.0, ImGui::ChildFlags_FrameStyle))
            {
                ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
                ImGui::PushTextWrapPos(m_ctx, available_width);

                ReaSonusPageTitle(m_ctx, i18n->t("about", "about.title").c_str(), main_font_bold);

                ImGui::Text(m_ctx, i18n->t("about", "about.top").c_str());
                ImGui::BulletText(m_ctx, i18n->t("about", "about.list.1").c_str());
                ImGui::BulletText(m_ctx, i18n->t("about", "about.list.2").c_str());
                ImGui::BulletText(m_ctx, i18n->t("about", "about.list.3").c_str());
                ImGui::BulletText(m_ctx, i18n->t("about", "about.list.4").c_str());

                UiElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::PopTextWrapPos(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);

            if (ImGui::BeginChild(m_ctx, "filter_content", 0.0, 0.0))
            {
                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "Contribute & Links", 0.0, 278.0, ImGui::ChildFlags_FrameStyle))
                {
                    ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
                    ImGui::PushTextWrapPos(m_ctx, available_width);

                    ReaSonusPageTitle(m_ctx, i18n->t("about", "contribute.title").c_str(), main_font_bold);
                    ImGui::Text(m_ctx, i18n->t("about", "contribute.top").c_str());
                    ImGui::TextLinkOpenURL(m_ctx, i18n->t("about", "contribute.link.coffee").c_str(), "https://buymeacoffee.com/navelpluisje");
                    ImGui::SameLine(m_ctx);
                    ImGui::Text(m_ctx, ", or ");
                    ImGui::SameLine(m_ctx);
                    ImGui::TextLinkOpenURL(m_ctx, i18n->t("about", "contribute.link.tipeee").c_str(), "https://en.tipeee.com/navelpluisje");

                    ImGui::Separator(m_ctx);

                    ImGui::Text(m_ctx, i18n->t("about", "contribute.center").c_str());
                    ImGui::TextLinkOpenURL(m_ctx, i18n->t("about", "contribute.link.documentation").c_str(), "https://navelpluisje.github.io/reasonus/");

                    ImGui::Separator(m_ctx);

                    ImGui::Text(m_ctx, i18n->t("about", "contribute.bottom").c_str());
                    ImGui::TextLinkOpenURL(m_ctx, i18n->t("about", "contribute.link.bug").c_str(), "https://github.com/navelpluisje/Reasonus-Native/issues");
                    ImGui::SameLine(m_ctx);
                    ImGui::Text(m_ctx, ", or ");
                    ImGui::SameLine(m_ctx);
                    ImGui::TextLinkOpenURL(m_ctx, i18n->t("about", "contribute.link.question").c_str(), "https://forum.cockos.com/showthread.php?t=267116");

                    UiElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::PopTextWrapPos(m_ctx);
                    ImGui::EndChild(m_ctx);
                }

                if (device != FP_V2)
                {
                    UiElements::PushReaSonusGroupStyle(m_ctx);
                    if (ImGui::BeginChild(m_ctx, "Thanks", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
                    {
                        ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
                        ImGui::PushTextWrapPos(m_ctx, available_width);

                        ReaSonusPageTitle(m_ctx, i18n->t("about", "thanks.title").c_str(), main_font_bold);
                        ImGui::Text(m_ctx, i18n->t("about", "thanks.bottom").c_str());
                        ImGui::BulletText(m_ctx, i18n->t("about", "thanks.list.1").c_str());
                        ImGui::BulletText(m_ctx, i18n->t("about", "thanks.list.2").c_str());
                        ImGui::BulletText(m_ctx, i18n->t("about", "thanks.list.3").c_str());

                        UiElements::PopReaSonusGroupStyle(m_ctx);
                        ImGui::PopTextWrapPos(m_ctx);
                        ImGui::EndChild(m_ctx);
                    }
                }

                ImGui::EndChild(m_ctx);
            }

            ImGui::PopStyleVar(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }
};
