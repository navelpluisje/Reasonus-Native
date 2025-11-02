#include <filesystem>
#include <fstream>
#include <regex>
#include <cstdio>
#include "csurf_ui_translation_editor.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_vars.hpp"
#include "csurf_ui_page_title.hpp"
#include "csurf_ui_images.h"
#include "csurf_ui_button_bar.hpp"
#include "csurf_ui_list_box.hpp"
#include "csurf_ui_action_input_text.hpp"
#include "csurf_ui_checkbox.hpp"
#include "../shared/csurf_utils.hpp"

constexpr const char *g_name{"ReaSonus Native Translaions Editor"};

std::unique_ptr<ReaSonusTranslationEditor> ReaSonusTranslationEditor::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusTranslationEditor::ReaSonusTranslationEditor()
    : m_ctx{}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);

    InitAssets();
    GetBaseLanguage();
    GetLanguageList();

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusTranslationEditor::~ReaSonusTranslationEditor()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusTranslationEditor::InitAssets()
{
    main_font = ImGui::CreateFont("Arial");
    main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
    logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo), sizeof(reasonus_logo));
    icon_add = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_add), sizeof(img_icon_add));
    icon_remove_list_item = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_list_remove), sizeof(img_icon_list_remove));

    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(logo));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_add));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_remove_list_item));
}

void ReaSonusTranslationEditor::Start()
{
    try
    {
        window_open = true;
        if (s_inst)
        {
            ImGui::SetNextWindowFocus(s_inst->m_ctx);
        }
        else
        {
            s_inst.reset(new ReaSonusTranslationEditor);
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusTranslationEditor::Stop()
{
    try
    {
        window_open = false;
        if (s_inst)
        {
            s_inst.reset();
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusTranslationEditor::Loop()
{
    try
    {
        s_inst->Frame();
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusTranslationEditor::GetLanguageList()
{
    std::string path = GetReaSonusLocalesFolderPath();
    language_list.clear();

    for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (entry.is_regular_file() && !entry.is_symlink())
        {
            std::filesystem::path path(entry.path());
            if (path.has_extension() && path.extension() == ".ini")
            {
                language_list.push_back((split(path.filename().u8string(), ".").at(0)));
            }
        }
    }

    if (language_list.size() > 0)
    {
        selected_language = 0;
    }
}

void ReaSonusTranslationEditor::GetBaseLanguage()
{
    mINI::INIFile file(GetReaSonusLocalesPath("en-US"));
    file.read(base_file);
}

void ReaSonusTranslationEditor::GetSelectedLanguage()
{
    std::string language = language_list.at(selected_language);
    mINI::INIFile file(GetReaSonusLocalesPath(language));
    file.read(translation_file);
}

void ReaSonusTranslationEditor::HandleAddLanguage()
{
    language_list.push_back(new_language);
    mINI::INIFile file(GetReaSonusLocalesPath(new_language));
    file.write(base_file);
    new_language = "";
    selected_language = language_list.size() - 1;
}

void ReaSonusTranslationEditor::SaveChanges()
{
    mINI::INIFile file(GetReaSonusLocalesPath(language_list.at(selected_language)));
    if (file.write(translation_file))
    {
        save_message = "Woohoo! Changes were successfully saved";
    }
}

void ReaSonusTranslationEditor::HandleRemoveLanguageListItem(int index)
{
    std::string language = language_list.at(index);
    int status = std::remove(GetReaSonusLocalesPath(language).c_str());
    if (status == 0)
    {
        GetLanguageList();
    }
    logInteger("index", index);
}

void ReaSonusTranslationEditor::getMultilineString(std::string &value, double width, double &height)
{
    double text_width, text_height;
    std::string tmp_str = "";
    std::string final_str = "";
    int curChr = 0;
    while (curChr < (int)value.size())
    {

        if (value[curChr] == '\n')
        {
            final_str += tmp_str + "\n";
            tmp_str = "";
        }

        tmp_str += value[curChr];
        ImGui::CalcTextSize(m_ctx, tmp_str.c_str(), &text_width, &text_height);

        if (text_width > width)
        {
            int lastSpace = tmp_str.size() - 1;
            while (tmp_str[lastSpace] != ' ' && lastSpace > 0)
            {
                lastSpace--;
            }

            if (lastSpace == 0)
            {
                lastSpace = tmp_str.size() - 2;
            }

            final_str += tmp_str.substr(0, lastSpace + 1) + "\r\n";
            if (lastSpace + 1 > (int)tmp_str.size())
            {
                tmp_str = "";
            }
            else
            {
                tmp_str = tmp_str.substr(lastSpace + 1);
            }
        }
        curChr++;
    }
    if (tmp_str.size() > 0)
    {
        final_str += tmp_str;
    }

    ImGui::CalcTextSize(m_ctx, final_str.c_str(), &text_width, &text_height);

    value = final_str;
    height = text_height;
}

std::string removeNewLines(std::string value)
{
    std::string new_string = "";
    for (int i = 0; i < (int)value.size(); i++)
    {
        if (value[i] != '\n' && value[i] != '\r')
        {
            new_string += value[i];
        }
    }

    return new_string;
}

void ReaSonusTranslationEditor::RenderTranslation(std::string section, std::string key)
{
    double width, height, text_height;
    ImGui::GetContentRegionAvail(m_ctx, &width, &height);
    std::string val = translation_file[section][key];
    getMultilineString(val, width - 16, text_height);

    char value[512];
    strcpy(value, val.c_str());

    if (ImGui::BeginChild(m_ctx, (section + key + "-container").c_str(), 0.0, 0.0, ImGui::ChildFlags_AlwaysAutoResize | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_None))
    {
        UiElements::PushReaSonusTranslationItemStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, (section + key + "-label").c_str(), 0.0, 0.0, ImGui::ChildFlags_AlwaysAutoResize | ImGui::ChildFlags_AutoResizeY | ImGui::ChildFlags_FrameStyle))
        {
            ImGui::GetContentRegionAvail(m_ctx, &width, &height);
            ImGui::PushTextWrapPos(m_ctx, width);

            if (show_translation_key)
            {
                ImGui::Text(m_ctx, key.c_str());
            }
            ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
            ImGui::Text(m_ctx, base_file[section][key].c_str());
            ImGui::PopStyleColor(m_ctx);
            ImGui::Text(m_ctx, "  ");

            ImGui::PopTextWrapPos(m_ctx);

            UiElements::PopReaSonusTranslationItemStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ImGui::GetContentRegionAvail(m_ctx, &width, &height);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 33);
        ImGui::PushTextWrapPos(m_ctx, width - 18);

        UiElements::PushReaSonusInputStyle(m_ctx);
        if (ImGui::InputTextMultiline(m_ctx, ("##" + section + key).c_str(), value, 512, width, text_height + 16, ImGui::InputTextFlags_AlwaysOverwrite))
        {
            std::string new_value = removeNewLines(value);

            if (new_value.compare(translation_file[section][key]) != 0)
            {
                translation_file[section][key] = new_value;
            }
        }
        ImGui::PopTextWrapPos(m_ctx);
        UiElements::PopReaSonusInputStyle(m_ctx);

        ImGui::EndChild(m_ctx);
    }
}

void ReaSonusTranslationEditor::Frame()
{
    using namespace std::placeholders; // for `_1, _2 etc`

    if (!save_message.empty())
    {
        int now = (int)GetTickCount();
        if (save_message_timer == 0)
        {
            save_message_timer = now;
        }
        else if ((save_message_timer + 3000) < now)
        {
            save_message = "";
            save_message_timer = 0;
        }
    }

    if (previous_selected_language != selected_language)
    {
        previous_selected_language = selected_language;
        GetSelectedLanguage();
    }

    if (close_clicked)
    {
        close_clicked = false;
        Stop();
    }

    if (save_clicked)
    {
        save_clicked = false;
        if (selected_language > -1)
        {
            SaveChanges();
        }
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, main_font, 13);
    ImGui::SetNextWindowSize(m_ctx, 640, 612, ImGui::Cond_Once);
    bool open{true};

    UiElements::PushReaSonusWindowStyle(m_ctx);
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse))
    {
        if (ImGui::BeginChild(m_ctx, "logo", 0.0, 52.0, ImGui::ChildFlags_None))
        {
            ImGui::Image(m_ctx, logo, 200, 52);
            ImGui::SameLine(m_ctx);

            ImGui::EndChild(m_ctx); // logo
        }

        if (ImGui::BeginChild(m_ctx, "actions_container", 0.0, -52.0, ImGui::ChildFlags_None))
        {

            if (ImGui::BeginChild(m_ctx, "actions_info", 220.0, 0.0, ImGui::ChildFlags_None))
            {
                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "actions_convert_info", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
                {
                    ReaSonusPageTitle(m_ctx, "Languages", main_font_bold);

                    ReaSonusCheckBox(m_ctx, "Show only untranslated lines", &show_empty_only);
                    ReaSonusCheckBox(m_ctx, "Show the actual key", &show_translation_key);

                    ReaSonusActionInputText(
                        m_ctx,
                        "Add a new language",
                        &new_language,
                        "Please use ISO names",
                        icon_add,
                        std::bind(&ReaSonusTranslationEditor::HandleAddLanguage, this));

                    ReaSonusExtendedListBox(
                        m_ctx,
                        "filters",
                        language_list,
                        &selected_language,
                        false,
                        icon_remove_list_item,
                        std::bind(&ReaSonusTranslationEditor::HandleRemoveLanguageListItem, this, _1),
                        false);

                    // Explanation of the language code
                    // Language Code: https://en.wikipedia.org/wiki/List_of_ISO_639_language_codes
                    // Country Code: https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
                    UiElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx); // actions_convert_info
                }

                ImGui::EndChild(m_ctx); // actions_info
            }

            ImGui::SameLine(m_ctx);

            UiElements::PushReaSonusGroupStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
            {
                for (auto const &sections : base_file)
                {
                    std::string section = sections.first;

                    ReaSonusPageTitle(m_ctx, section, main_font_bold);

                    for (auto const &pair : sections.second)
                    {
                        auto const &key = pair.first;
                        if (!translation_file[section][key].empty() && show_empty_only)
                        {
                            continue;
                        }

                        RenderTranslation(section, key);
                    }
                }

                UiElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::EndChild(m_ctx); // main_content_area
            }

            ImGui::EndChild(m_ctx); // actions_container
        }

        ReaSonusButtonBar(m_ctx, "Save", main_font_bold, &save_clicked, true, &close_clicked, "Close", &save_message);

        UiElements::PopReaSonusWindowStyle(m_ctx);

        ImGui::End(m_ctx); // window
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
    {
        window_open = false;
        SetActionState("_REASONUS_TRANSLATIONN_EDITOR");
        return s_inst.reset();
    }
}