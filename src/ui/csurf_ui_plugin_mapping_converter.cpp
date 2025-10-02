#include <filesystem>
#include <fstream>
#include <regex>
#include "csurf_ui_plugin_mapping_converter.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_vars.hpp"
#include "csurf_ui_page_title.hpp"
#include "csurf_ui_images.h"
#include "csurf_ui_button_bar.hpp"
#include "csurf_ui_list_box.hpp"

constexpr const char *g_name{"ReaSonus Native V2 Control Panel"};

std::unique_ptr<ReaSonusV2PluginMappingConverter> ReaSonusV2PluginMappingConverter::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusV2PluginMappingConverter::ReaSonusV2PluginMappingConverter()
    : m_ctx{}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    InitAssets();

    select = std::regex("Select([0-9]{1,2})[[:space:]]+FXParam[[:space:]]+([0-9]{1,6})[[:space:]]+\"([a-zA-Z0-9[:space:]]+)\"([[:space:]]+[[][[:space:]]([0-9.[:space:]]+)[[:space:]])?");
    fader = std::regex("Fader([0-9]{1,2})[[:space:]]+FXParam[[:space:]]+([0-9]{1,6})[[:space:]]+\"([a-zA-Z0-9[:space:]]+)\"");
    scribble = std::regex("ScribbleLine([0-9])_([0-9])[[:space:]]+FXParamNameDisplay[[:space:]]+([0-9]{1,6})[[:space:]]+\"([a-zA-Z0-9[:space:]]+)\"");
    zone = std::regex("Zone[[:space:]]+\"([a-zA-Z0-9[:space:]:()-_]+)\"");
    namedZone = std::regex("Zone[[:space:]]+\"([a-zA-Z0-9[:space:]:()]+)\"[[:space:]]+\"?([a-zA-Z0-9-_[:space:]]+)");

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusV2PluginMappingConverter::~ReaSonusV2PluginMappingConverter()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusV2PluginMappingConverter::InitAssets()
{
    main_font = ImGui::CreateFont("Arial");
    main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
    logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo), sizeof(reasonus_logo));

    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
    ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(logo));
}

void ReaSonusV2PluginMappingConverter::Start()
{
    try
    {
        converter_open = true;
        if (s_inst)
        {
            ImGui::SetNextWindowFocus(s_inst->m_ctx);
        }
        else
        {
            s_inst.reset(new ReaSonusV2PluginMappingConverter);
        }
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusV2PluginMappingConverter::Stop()
{
    try
    {
        converter_open = false;
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

void ReaSonusV2PluginMappingConverter::Loop()
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

void ReaSonusV2PluginMappingConverter::SetBaseFileNames()
{
    file_names.clear();
    int index = 0;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(GetReaSonusZonesPath()))
    {
        if (entry.is_regular_file() && !entry.is_symlink())
        {
            std::filesystem::path path(entry.path());
            if (path.has_extension() && path.extension() == ".zon")
            {
                file_names.insert(file_names.begin() + index, entry.path().u8string());
                index++;
            }
        }
    }
}

void ReaSonusV2PluginMappingConverter::ConvertFile(std::string file_path)
{
    std::string plugin_name;
    ini.clear();
    bool has_plugin = false;

    std::smatch matches;
    std::ifstream zon_file(file_path);
    std::filesystem::path path(file_path);

    for (std::string line; getline(zon_file, line);)
    {
        if ((line.rfind("Zone ", 0) == 0))
        {
            if (std::regex_search(line, matches, zone))
            {
                plugin_name = matches[1];

                ini["global"]["origname"] = plugin_name;
                ini["global"]["name"] = StripPluginName(matches[1]);
                ini["global"]["developer"] = StripPluginDeveloper(matches[1]);
                has_plugin = true;
            }
        }
        else if ((line.find("Fader") - std::string::npos > 0))
        {
            if (std::regex_search(line, matches, fader))
            {
                std::string key = "fader_" + std::to_string(stoi(matches[1].str()) - 1);

                ini[key]["origname"] = matches[3].str();
                ini[key]["name"] = matches[3].str();
                ini[key]["param"] = matches[2].str();
            }
        }
        else if (line.find("Select") - std::string::npos > 0)
        {
            if (std::regex_search(line, matches, select))
            {
                std::string key = "select_" + std::to_string(stoi(matches[1].str()) - 1);
                std::string steps = matches[5].str().compare("") == 0 ? "2" : std::to_string(split(matches[5].str(), " ").size());

                ini[key]["origname"] = matches[3].str();
                ini[key]["name"] = matches[3].str();
                ini[key]["param"] = matches[2].str();
                ini[key]["steps"] = steps;
            }
        }
        else if (line.find("ScribbleLine") - std::string::npos > 0)
        {
            if (std::regex_search(line, matches, scribble))
            {
                std::string key = "";

                if (stoi(matches[1].str()) == 3)
                {
                    key = "fader_" + std::to_string(stoi(matches[2].str()) - 1);
                }
                else if (stoi(matches[1].str()) == 1)
                {
                    key = "select_" + std::to_string(stoi(matches[2].str()) - 1);
                }

                ini[key]["name"] = matches[4].str();
            }
        }
    }

    if (has_plugin)
    {
        std::string fileName = GetReaSonusPluginPath(ini["global"]["developer"], ini["global"]["name"], true);
        mINI::INIFile file(fileName);
        if (!file_exists(fileName.c_str()) && file.write(ini, true))
        {
            converted_files.insert(converted_files.begin() + converted_files.size(), plugin_name);
        }
        else
        {
            unconverted_files.insert(unconverted_files.begin() + unconverted_files.size(), plugin_name);
        }
    }
    else
    {
        unconverted_files.insert(unconverted_files.begin() + unconverted_files.size(), path.filename().u8string());
    }
}

void ReaSonusV2PluginMappingConverter::ConvertFiles()
{
    for (auto &file_name : file_names)
    {
        ConvertFile(file_name);
    }
}

void ReaSonusV2PluginMappingConverter::Convert()
{
    unconverted_files.clear();
    converted_files.clear();
    SetBaseFileNames();
    ConvertFiles();
}

void ReaSonusV2PluginMappingConverter::Frame()
{
    if (close_clicked)
    {
        close_clicked = false;
        Stop();
    }

    if (convert_clicked)
    {
        convert_clicked = false;
        Convert();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, main_font, 13);
    ImGui::SetNextWindowSize(m_ctx, 640, 446, ImGui::Cond_Once);
    bool open{true};

    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_NoResize))
    {
        UiElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "actions_info", 300.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            ImGui::Image(m_ctx, logo, 200, 52);
            UiElements::PushReaSonusGroupStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "actions_info", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
            {
                ReaSonusPageTitle(m_ctx, "Convert", main_font_bold);
                ImGui::PushTextWrapPos(m_ctx, 260);
                ImGui::Text(m_ctx, "This action will convert your old zon plugin files to the new ini files used by ReaSonus Native. If a file already exists for ReaSonus Native, the file will be ignored.");
                ImGui::Text(m_ctx, "Once the action is finished, it will list the converted files and the skipped files.");
                ImGui::Text(m_ctx, "The unconverted files have 2 types:");
                ImGui::BulletText(m_ctx, "With 'zon' extension. These files were invalid");
                ImGui::BulletText(m_ctx, "Without 'zon' extension. These files already exist and will not be overwritten");
                ImGui::Text(m_ctx, "Do you want to convert the old files?");
                ImGui::PopTextWrapPos(m_ctx);

                ImGui::EndChild(m_ctx);
                UiElements::PopReaSonusGroupStyle(m_ctx);
            }
            ImGui::EndChild(m_ctx);
            UiElements::PopReaSonusContentStyle(m_ctx);
        }
        ImGui::SameLine(m_ctx);
        UiElements::PushReaSonusContentStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "report", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
        {
            UiElements::PushReaSonusGroupStyle(m_ctx);
            if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, -44.0, ImGui::ChildFlags_FrameStyle))
            {

                if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, 0.0, ImGui::ChildFlags_None))
                {
                    ReaSonusPageTitle(m_ctx, "Converted files", main_font_bold);

                    UiElements::PushReaSonusListBoxStyle(m_ctx);
                    if (ImGui::BeginListBox(m_ctx, "##converted_files", 0.0, 110))
                    {
                        bool selected = false;
                        for (int i = 0; i < (int)converted_files.size(); i++)
                        {
                            ImGui::Selectable(m_ctx, converted_files[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap);
                        }
                        UiElements::PopReaSonusListBoxStyle(m_ctx);
                        ImGui::EndListBox(m_ctx);
                    }

                    ReaSonusPageTitle(m_ctx, "Unconverted files", main_font_bold);

                    UiElements::PushReaSonusListBoxStyle(m_ctx);
                    if (ImGui::BeginListBox(m_ctx, "##unconverted_files", 0.0, 110))
                    {
                        bool selected = false;
                        for (int i = 0; i < (int)unconverted_files.size(); i++)
                        {
                            ImGui::Selectable(m_ctx, unconverted_files[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap);
                        }
                        UiElements::PopReaSonusListBoxStyle(m_ctx);
                        ImGui::EndListBox(m_ctx);
                    }
                    ImGui::EndChild(m_ctx);
                }
                UiElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            UiElements::PopReaSonusContentStyle(m_ctx);
            ReaSonusButtonBar(m_ctx, "Convert", main_font_bold, &convert_clicked, true, &close_clicked, "Close");
            ImGui::EndChild(m_ctx);
        }

        ImGui::End(m_ctx);
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
    {
        converter_open = false;
        return s_inst.reset();
    }
}