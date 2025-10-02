#include "csurf_ui_page_content.hpp"
#include <string.h>
#include <reaper_imgui_functions.h>
#include "csurf_ui_elements.hpp"
#include "csurf_ui_page_title.hpp"

class CSurf_UI_AboutPage : public CSurf_UI_PageContent
{
protected:
    std::string device;
    ImGui_Font *main_font_bold;

public:
    CSurf_UI_AboutPage(ImGui_Context *m_ctx, std::string _device) : CSurf_UI_PageContent(m_ctx, _device)
    {
        device = _device;
        main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    };

    virtual ~CSurf_UI_AboutPage() {};

    void Render() override
    {
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
        if (ImGui::BeginChild(m_ctx, "main_about_page", 0.0, 0.0, ImGui::ChildFlags_None))
        {
            UiElements::PushReaSonusGroupStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "about", 240.0, 0.0, ImGui::ChildFlags_FrameStyle))
            {
                ReaSonusPageTitle(m_ctx, "About", main_font_bold);
                ImGui::Text(m_ctx, "Thank you for using ReaSonus Native.");
                ImGui::Text(m_ctx, "ReaSonus Native has beside ±95% of \nthe Studio One features also:");
                ImGui::BulletText(m_ctx, "‘Unlimited’ custom editable \nfilters");
                ImGui::BulletText(m_ctx, "MIDI learn style plugin mapping");
                ImGui::BulletText(m_ctx, "Swap Shift buttons");
                ImGui::BulletText(m_ctx, "Controll last touched plugin \nparam");

                UiElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);

            if (ImGui::BeginChild(m_ctx, "filter_content", 0.0, 0.0))
            {
                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "Contribute & Links", 0.0, 284.0, ImGui::ChildFlags_FrameStyle))
                {
                    ReaSonusPageTitle(m_ctx, "Contribute & Links", main_font_bold);
                    ImGui::Text(m_ctx, "ReaSonus Native is a free and open source project, but takes some time and effort to \ncreate and maintain. It is not required of course but a donation for maintaining would be \nhighly appreciated.");
                    ImGui::TextLinkOpenURL(m_ctx, "Buy me a coffee", "https://buymeacoffee.com/navelpluisje");
                    ImGui::SameLine(m_ctx);
                    ImGui::Text(m_ctx, ", or ");
                    ImGui::SameLine(m_ctx);
                    ImGui::TextLinkOpenURL(m_ctx, "Tippee me", "https://en.tipeee.com/navelpluisje");

                    ImGui::Separator(m_ctx);

                    ImGui::Text(m_ctx, "ReaSonus Native has a website for the documentation. Before asking question, please \ncheck these first.");
                    ImGui::TextLinkOpenURL(m_ctx, "ReaSonus Native Documentation", "https://navelpluisje.github.io/reasonus/");

                    ImGui::Separator(m_ctx);

                    ImGui::Text(m_ctx, "If you encounter a bug or anything ales, please report it, so ReaSonus Native can only \nget better");
                    ImGui::TextLinkOpenURL(m_ctx, "Report your bug", "https://github.com/navelpluisje/Reasonus-Native/issues");
                    ImGui::SameLine(m_ctx);
                    ImGui::Text(m_ctx, ", or ");
                    ImGui::SameLine(m_ctx);
                    ImGui::TextLinkOpenURL(m_ctx, "Ask your question", "https://forum.cockos.com/showthread.php?t=267116");

                    UiElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx);
                }

                if (device != FP_V2)
                {
                    UiElements::PushReaSonusGroupStyle(m_ctx);
                    if (ImGui::BeginChild(m_ctx, "Thanks", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
                    {
                        ReaSonusPageTitle(m_ctx, "Thanks", main_font_bold);
                        ImGui::Text(m_ctx, "This project would not have been possible without you, reading this. But also a \nbig thanx goes to the following people:");
                        ImGui::BulletText(m_ctx, "Geoff Waddington, The man behind the impressive CSI. Without the CSI project, \nthis would probably never been created.");
                        ImGui::BulletText(m_ctx, "Cfillion, for helping me out with some annoyances I ran into with C++ and CMake.");
                        ImGui::BulletText(m_ctx, "All volunteers helping with testing, reviewing the documentation and being patient.");

                        UiElements::PopReaSonusGroupStyle(m_ctx);
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
