#include <filesystem>
#include <fstream>
#include <regex>
#include "csurf_ui_plugin_type_mapping.hpp"
#include "csurf_ui_colors.hpp"
#include "csurf_ui_vars.hpp"
#include "csurf_ui_page_title.hpp"
#include "csurf_ui_button_bar.hpp"
#include "csurf_ui_list_box.hpp"
#include "../shared/csurf_utils.hpp"

constexpr const char *g_name{"ReaSonus Native plugin type mapping"};

std::unique_ptr<ReaSonusPluginTypeMapping> ReaSonusPluginTypeMapping::s_inst;

static void reportError(const ImGui_Error &e)
{
    ShowMessageBox(e.what(), g_name, 0);
}

ReaSonusPluginTypeMapping::ReaSonusPluginTypeMapping()
    : m_ctx{}
{
    ImGui::init(plugin_getapi);
    m_ctx = ImGui::CreateContext(g_name);
    assets = new CSurf_UI_Assets(m_ctx);

    plugin_register("timer", reinterpret_cast<void *>(&Loop));
}

ReaSonusPluginTypeMapping::~ReaSonusPluginTypeMapping()
{
    plugin_register("-timer", reinterpret_cast<void *>(&Loop));
}

void ReaSonusPluginTypeMapping::Start()
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
            s_inst.reset(new ReaSonusPluginTypeMapping);
        }
        s_inst->SetPluginFolders();
    }
    catch (const ImGui_Error &e)
    {
        reportError(e);
        s_inst.reset();
    }
}

void ReaSonusPluginTypeMapping::Stop()
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

void ReaSonusPluginTypeMapping::Loop()
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

void ReaSonusPluginTypeMapping::SetPluginFolders()
{
    bool has_next = true;
    int index = 0;
    developers.clear();
    plugins.clear();
    selected_developer = -1;
    selected_plugin = -1;
    selected_plugin_type = -1;

    while (has_next)
    {
        const char *name = EnumerateSubdirectories(plugin_folder_path.c_str(), index);
        if (!name)
        {
            has_next = false;
        }
        else
        {
            index++;
            std::vector<std::string> splitted_name = split(name, ".");
            developers.push_back(std::string(name));
            plugins.push_back(SetDeveloperPlugins(name));

            if (plugins.at(plugins.size() - 1).size() == 0)
            {
                developers.pop_back();
                plugins.pop_back();
            }
        }
    }

    if (developers.size() > 0)
    {
        selected_developer = 0;
        selected_plugin = 0;
    }
}

std::vector<std::string> ReaSonusPluginTypeMapping::SetDeveloperPlugins(std::string developer)
{
    bool has_next = true;
    int index = 0;
    std::string path = createPathName({plugin_folder_path, developer});
    std::vector<std::string> developer_plugins;

    while (has_next)
    {
        const char *name = EnumerateFiles(path.c_str(), index);
        if (!name)
        {
            has_next = false;
        }
        else
        {
            index++;
            std::vector<std::string> splitted_name = split(std::string(name), ".");

            if (splitted_name[splitted_name.size() - 1].compare("ini") == 0 && splitted_name.size() == 2)
            {
                developer_plugins.push_back(std::string(name));
            }
        }
    }

    return developer_plugins;
}

bool ReaSonusPluginTypeMapping::AddPluginType()
{
    if (selected_plugin_type < 0 || selected_plugin < 0)
    {
        return false;
    }

    /**
     * Create the current plugin path
     */
    std::string old_plugin_path = createPathName({plugin_folder_path, developers[selected_developer], plugins[selected_developer][selected_plugin]});
    mINI::INIFile file(old_plugin_path);

    /**
     * Read theh file, write the type to the file and save the file
     */
    if (!file.read(plugin_data))
    {
        return false;
    }

    plugin_data["global"]["type"] = plugin_types[selected_plugin_type];

    if (!file.write(plugin_data, true))
    {
        return false;
    }

    /**
     * Create the new file name and rename the file
     */
    std::vector<std::string> splitted_name = split(plugins[selected_developer][selected_plugin], ".");
    splitted_name.insert(splitted_name.begin() + 1, plugin_types[selected_plugin_type]);
    std::string path_name = createPathName({plugin_folder_path, developers[selected_developer], join(splitted_name, ".")});

    std::rename(old_plugin_path.c_str(), path_name.c_str());

    SetPluginFolders();

    return true;
}

void ReaSonusPluginTypeMapping::RenderMappingsList()
{
    if (ImGui::BeginChild(m_ctx, "mapping_lists", 240.0, 0.0))
    {
        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
        {
            for (int i = 0; i < (int)developers.size(); i++)
            {
                if (i != selected_developer || selected_developer == -1)
                {
                    ImGui::SetNextItemOpen(m_ctx, false);
                }
                else
                {
                    ImGui::SetNextItemOpen(m_ctx, true);
                }

                UiElements::PushReaSonusTreeNodeStyle(m_ctx, selected_developer == i);
                if (ImGui::TreeNode(m_ctx, developers.at(i).c_str(), ImGui::TreeNodeFlags_CollapsingHeader))
                {
                    selected_developer = i;
                    std::vector<std::string> developer_plugins = plugins.at(i);

                    for (int j = 0; j < (int)developer_plugins.size(); j++)
                    {
                        bool selected = selected_developer == i && selected_plugin == j;
                        if (ImGui::Selectable(m_ctx, developer_plugins.at(j).c_str(), &selected))
                        {
                            selected_plugin = j;
                        }
                    }
                }
                UiElements::PopReaSonusTreeNodeStyle(m_ctx);
            }

            UiElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }

        ImGui::EndChild(m_ctx);
    }
}

void ReaSonusPluginTypeMapping::Frame()
{
    if (close_clicked)
    {
        close_clicked = false;
        return Stop();
    }

    if (add_type_clicked)
    {
        add_type_clicked = false;
        AddPluginType();
    }

    PushReaSonusColors(m_ctx);
    PushReaSonusStyle(m_ctx);
    ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeDefault);
    ImGui::SetNextWindowSize(m_ctx, 640, 682, ImGui::Cond_Once);
    bool open{true};

    UiElements::PushReaSonusWindowStyle(m_ctx);
    if (ImGui::Begin(m_ctx, g_name, &open, ImGui::WindowFlags_NoCollapse | ImGui::WindowFlags_NoResize))
    {
        if (ImGui::BeginChild(m_ctx, "logo", 0.0, 52.0, ImGui::ChildFlags_None))
        {
            ImGui::Image(m_ctx, assets->GetReaSonusLogo(), 200, 52);

            ImGui::EndChild(m_ctx); // logo
        }

        ImGui::Text(m_ctx, "Select a plugin from the list.");
        ImGui::Text(m_ctx, "Select a plugin type.");
        ImGui::Text(m_ctx, "Press the Add Type button to set the type to the plugin");

        if (ImGui::BeginChild(m_ctx, "plugins_container", 0.0, -52.0, ImGui::ChildFlags_None))
        {

            RenderMappingsList();

            ImGui::SameLine(m_ctx);

            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 26);

            if (ImGui::BeginChild(m_ctx, "actions", 0.0, 0.0, ImGui::ChildFlags_None))
            {
                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "main_content_area", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
                {
                    ReaSonusPageTitle(m_ctx, assets, "Select Plugin type");

                    UiElements::PushReaSonusListBoxStyle(m_ctx);
                    if (ImGui::BeginListBox(m_ctx, "##converted_files", 0.0, 250))
                    {
                        for (int i = 0; i < (int)plugin_types.size(); i++)
                        {
                            bool selected = selected_plugin_type == i;
                            if (ImGui::Selectable(m_ctx, plugin_types[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap))
                            {
                                selected_plugin_type = i;
                            }
                        }
                        UiElements::PopReaSonusListBoxStyle(m_ctx);
                        ImGui::EndListBox(m_ctx); // converted_files
                    }

                    ImGui::Text(m_ctx, ("Set type: " + ((selected_plugin_type > -1) ? plugin_types[selected_plugin_type] : "-")).c_str());
                    ImGui::Text(m_ctx, ("To: " + ((selected_plugin > -1) ? plugins[selected_developer][selected_plugin] : "-")).c_str());

                    UiElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx); // main_content_area
                }

                ImGui::EndChild(m_ctx); // report
            }

            ImGui::EndChild(m_ctx); // actions_container
        }

        ReaSonusButtonBar(
            m_ctx,
            assets,
            "Add Type",
            &add_type_clicked,
            true,
            &close_clicked,
            "Cancel",
            &save_message);

        UiElements::PopReaSonusWindowStyle(m_ctx);

        ImGui::End(m_ctx); // window
    }

    ImGui::PopFont(m_ctx);
    PopReaSonusColors(m_ctx);
    PopReaSonusStyle(m_ctx);

    if (!open)
    {
        window_open = false;
        return s_inst.reset();
    }
}