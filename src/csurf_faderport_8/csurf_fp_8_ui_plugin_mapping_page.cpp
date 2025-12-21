#include "../ui/csurf_ui_page_content.hpp"
#include <string.h>
#include "../ui/csurf_ui_text_input.hpp"
#include "../ui/csurf_ui_page_title.hpp"

typedef std::tuple<int, std::string, int> PluginParam;

class CSurf_FP_8_PluginMappingPage : public CSurf_UI_PageContent
{
protected:
    ImGui_Font *main_font_bold;
    I18n *i18n = I18n::GetInstance();

    int x = 0;

    bool render_started = false;
    std::string plugin_folder_path = std::string(GetResourcePath()) + pathSeparator + "ReaSonus" + pathSeparator + "Plugins";
    std::vector<std::string> developers;
    int selected_developer = -1;
    int previous_selected_developer = -1;

    std::vector<std::vector<std::string>> plugins;
    int selected_plugin = -1;
    int previous_selected_plugin = -1;
    bool selected_plugin_exists = false;

    int nb_channels = 0;
    int selected_channel = 0;
    int previous_selected_channel = 0;

    std::vector<PluginParam> paramIds;
    std::vector<std::string> params;
    int selected_param = 0;

    std::string select_key = "";
    std::string select_name;
    int select_nb_steps;
    int select_param_index = -1;
    int previous_select_param_index = -1;
    int select_uninvert_label;

    std::string fader_key = "";
    std::string fader_name;
    int fader_param_index;
    int fader_uninvert_label;

    std::vector<std::string> invert_labels = {"Inverted", "Not inverted"};

    std::vector<int> dirty_groups;

    bool channel_dirty = false;

    void SetPluginFolders()
    {
        bool has_next = true;
        int index = 0;

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
            }
        }
    }

    std::vector<std::string> SetDeveloperPlugins(std::string developer)
    {
        bool has_next = true;
        int index = 0;
        std::string path = plugin_folder_path + pathSeparator + developer;
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

                if (splitted_name[splitted_name.size() - 1].compare("ini") == 0)
                {
                    developer_plugins.push_back(std::string(name));
                }
            }
        }

        return developer_plugins;
    }

    std::string GetPluginPath()
    {
        return plugin_folder_path + pathSeparator + developers[selected_developer] + pathSeparator + plugins[selected_developer][selected_plugin];
    }

    bool SetPluginData()
    {
        mINI::INIFile file(GetPluginPath());
        file.read(ini);

        if (!PluginExists())
        {
            selected_plugin_exists = false;
            return false;
        }

        selected_plugin_exists = true;

        for (int i = 0; i < 99; i++)
        {
            if (ini.has("select_" + std::to_string(i)) || ini.has("fader_" + std::to_string(i)))
            {
                nb_channels = i;
            }
        }

        // AN extra + to set the add button
        nb_channels++;
        GetPluginParams();

        return true;
    }

    bool PluginExists()
    {
        // Create a track, add the plugin and start reading the params
        InsertTrackAtIndex(0, false);
        MediaTrack *media_track = GetTrack(0, 0);
        int exist = TrackFX_AddByName(media_track, ini["global"]["origname"].c_str(), false, -1);
        DeleteTrack(media_track);

        return exist > -1;
    }

    void GetPluginParams()
    {
        // Prepare the vectors
        params.clear();
        paramIds.clear();
        params.push_back(i18n->t("mapping", "edit.select.param.first-item"));
        paramIds.push_back({-1, i18n->t("mapping", "edit.select.param.first-item"), 0});

        // Create a track, add the plugin and start reading the params
        InsertTrackAtIndex(0, false);
        MediaTrack *media_track = GetTrack(0, 0);

        TrackFX_AddByName(media_track, ini["global"]["origname"].c_str(), false, -1);

        for (int i = 0; i < std::min(TrackFX_GetNumParams(media_track, 0), 256); i++)
        {
            std::string param_name = DAW::GetTrackFxParamName(media_track, 0, i);

            if (IsWantedParam(std::string(param_name)))
            {
                int steps = DAW::GetTrackFxParamNbSteps(media_track, 0, i);
                params.push_back(std::string(param_name));
                paramIds.push_back({i, std::string(param_name), steps});
            }
        }

        // We're done, now delete the newly added track
        DeleteTrack(media_track);
    }

    void PopulateFields()
    {
        select_key = "select_" + std::to_string(selected_channel);
        fader_key = "fader_" + std::to_string(selected_channel);

        if (ini.has(select_key))
        {
            select_name = ini[select_key]["name"];
            select_nb_steps = stoi(ini[select_key]["steps"]);
            select_uninvert_label = stoi(ini[select_key].has("uninvert-label") ? ini[select_key]["uninvert-label"] : "0");
            int param_id = stoi(ini[select_key]["param"]);
            const auto it = std::find_if(paramIds.begin(), paramIds.end(), [param_id](PluginParam param)
                                         { return param_id == std::get<0>(param); });
            if (it != paramIds.end())
            {
                select_param_index = it - paramIds.begin();
                previous_select_param_index = select_param_index;
            }
        }
        else
        {
            select_name = "";
            select_nb_steps = 0;
            select_param_index = 0;
            select_uninvert_label = 0;
        }

        if (ini.has(fader_key))
        {
            fader_name = ini[fader_key]["name"];
            int param_id = stoi(ini[fader_key]["param"]);
            fader_uninvert_label = stoi(ini[fader_key].has("uninvert-label") ? ini[fader_key]["uninvert-label"] : "0");
            const auto it = std::find_if(paramIds.begin(), paramIds.end(), [param_id](PluginParam param)
                                         { return param_id == std::get<0>(param); });
            if (it != paramIds.end())
            {
                fader_param_index = it - paramIds.begin();
            }
        }
        else
        {
            fader_name = "";
            fader_param_index = 0;
            fader_uninvert_label = 0;
        }
    }

    void IsChannelDirty()
    {
        bool select_dirty = select_key.compare("") != 0 &&
                            ini.has(select_key) &&
                            (select_name.compare(ini[select_key]["name"]) != 0 ||
                             select_nb_steps != std::stoi(ini[select_key]["steps"]) ||
                             stoi(ini[select_key]["param"]) != std::get<0>(paramIds[select_param_index]));
        bool select_new = select_key.compare("") != 0 && !ini.has(select_key) && (select_param_index > 0);

        bool fader_dirty = fader_key.compare("") != 0 &&
                           ini.has(fader_key) &&
                           (fader_name.compare(ini[fader_key]["name"]) != 0 ||
                            stoi(ini[fader_key]["param"]) != std::get<0>(paramIds[fader_param_index]));
        bool fader_new = fader_key.compare("") != 0 && !ini.has(fader_key) && (fader_param_index > 0);

        channel_dirty = selected_channel > -1 && (select_dirty || fader_dirty || select_new || fader_new);
    }

    bool DirtyCheck()
    {
        IsChannelDirty();
        if (channel_dirty)
        {
            int res = MB(i18n->t("mapping", "popup.unsaved.message").c_str(), i18n->t("mapping", "popup.unsaved.title").c_str(), 3);
            if (res == 6)
            {
                Save();
            }
            return res != 2;
        }
        return true;
    }

public:
    CSurf_FP_8_PluginMappingPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx)
    {
        main_font_bold = ImGui::CreateFont("Arial", ImGui::FontFlags_Bold);
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(main_font_bold));
        settings = ReaSonusSettings::GetInstance(FP_8);

        SetPluginFolders();
    };

    virtual ~CSurf_FP_8_PluginMappingPage() {};

    void RenderMappingsLlist()
    {
        if (ImGui::BeginChild(m_ctx, "mapping_lists", 240.0, 0.0))
        {
            ImGui::Text(m_ctx, i18n->t("mapping", "list.label").c_str());
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

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

    void RenderChannelsList()
    {
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 4, 0);
        if (ImGui::BeginChild(m_ctx, "##channel-list", 0.0, 30))
        {
            for (int i = 0; i < nb_channels; i++)
            {
                bool selected = selected_channel == i;
                if (i > 0)
                {
                    ImGui::SameLine(m_ctx);
                }
                UiElements::PushReaSonusChannelTabStyle(m_ctx, selected);
                if (ImGui::Button(m_ctx, std::to_string(i + 1).c_str(), 30, 30))
                {
                    selected_channel = i;
                }
                UiElements::PopReaSonusChannelTabStyle(m_ctx);
            }

            ImGui::SameLine(m_ctx);
            UiElements::PushReaSonusChannelTabStyle(m_ctx, false);
            if (ImGui::Button(m_ctx, "+", 30, 30))
            {
                selected_channel = nb_channels;
                nb_channels += 1;
            }
            UiElements::PopReaSonusChannelTabStyle(m_ctx);

            ImGui::PopStyleVar(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderMappingSelect(double height)
    {
        double space_x, space_y;

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, height, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {
            ReaSonusPageTitle(m_ctx, i18n->t("mapping", "edit.select.label").c_str(), main_font_bold);
            if (params.size() > 0)
            {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusComboInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.param.label"),
                    params,
                    &select_param_index,
                    space_x * 0.7);

                ImGui::SameLine(m_ctx);

                ReaSonusIntInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.param.steps"),
                    &select_nb_steps,
                    0,
                    20);
            }

            if (ImGui::BeginChild(m_ctx, "filter_content_input", 0.0, 0.0, ImGui::ChildFlags_AutoResizeY))
            {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusTextInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.param-name.label"),
                    &select_name,
                    i18n->t("mapping", "edit.select.param-name.placeholder"),
                    space_x * 0.7);
                ImGui::SameLine(m_ctx);

                ReaSonusComboInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.label-uninvert.label"),
                    invert_labels,
                    &select_uninvert_label);

                ImGui::EndChild(m_ctx);
            }
            UiElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderMappingFader(double height)
    {
        double space_x, space_y;

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_content_fader", 0.0, height, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {
            ReaSonusPageTitle(m_ctx, i18n->t("mapping", "edit.fader.label"), main_font_bold);
            if (params.size() > 0)
            {
                ReaSonusComboInput(m_ctx, i18n->t("mapping", "edit.fader.param.label"), params, &fader_param_index);
            }
            if (ImGui::BeginChild(m_ctx, "filter_content_input", 0.0, 0.0, ImGui::ChildFlags_AutoResizeY))
            {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusTextInput(
                    m_ctx,
                    i18n->t("mapping", "edit.fader.param-name.label"),
                    &fader_name,
                    i18n->t("mapping", "edit.fader.param-name.placeholder"),
                    space_x * 0.7);

                ImGui::SameLine(m_ctx);

                ReaSonusComboInput(
                    m_ctx,
                    i18n->t("mapping", "edit.fader.label-uninvert.label"),
                    invert_labels,
                    &fader_uninvert_label);

                ImGui::EndChild(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderCenteredText(std::string content)
    {
        double text_width, text_height, available_width, available_height;
        ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
        ImGui::CalcTextSize(m_ctx, content.c_str(), &text_width, &text_height);
        ImGui::SetCursorPosX(m_ctx, available_width / 2 - text_width / 2);
        ImGui::SetCursorPosY(m_ctx, available_height / 2 - text_height / 2);
        ImGui::Text(m_ctx, content.c_str());
    }

    void Render() override
    {
        if (!render_started)
        {

            render_started = true;
            double space_x, space_y;

            if (selected_developer != previous_selected_developer)
            {
                selected_plugin_exists = false;
                if (DirtyCheck())
                {
                    selected_plugin = -1;
                    previous_selected_plugin = -1;
                    selected_channel = 0;
                    previous_selected_channel = 0;
                    previous_selected_developer = selected_developer;
                }
                else
                {
                    selected_developer = previous_selected_developer;
                }
            }

            if (selected_plugin != previous_selected_plugin)
            {
                if (DirtyCheck())
                {
                    selected_channel = 0;

                    if (SetPluginData())
                    {
                        PopulateFields();
                    }
                    else
                    {
                        select_key = "";
                        fader_key = "";
                    }
                    previous_selected_plugin = selected_plugin;
                }
                else
                {
                    selected_plugin = previous_selected_plugin;
                }
            }

            if (selected_channel != previous_selected_channel && selected_plugin_exists)
            {
                if (DirtyCheck())
                {
                    PopulateFields();
                    previous_selected_channel = selected_channel;
                }
                else
                {
                    selected_channel = previous_selected_channel;
                }
            }

            if (select_param_index != previous_select_param_index)
            {
                previous_select_param_index = select_param_index;
                select_nb_steps = std::get<2>(paramIds[select_param_index]);
            }

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
            if (ImGui::BeginChild(m_ctx, "main_mapping_content", 0.0, 0.0, ImGui::ChildFlags_None))
            {
                RenderMappingsLlist();
                ImGui::SameLine(m_ctx);

                if (ImGui::BeginChild(m_ctx, "mapping_content", 0.0, 0.0))
                {
                    if (selected_plugin > -1 && selected_plugin_exists)
                    {
                        ImGui::Text(m_ctx, selected_plugin < 0 ? "Channels" : std::string(developers[selected_developer] + " :: " + plugins[selected_developer][selected_plugin]).c_str());
                        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);
                        RenderChannelsList();

                        if (ImGui::BeginChild(m_ctx, "mapping_content_area", 0.0, 0.0))
                        {
                            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                            double height = (space_y - 12) / 2;

                            RenderMappingSelect(height);
                            RenderMappingFader(height);

                            UiElements::PopReaSonusGroupStyle(m_ctx);
                            ImGui::EndChild(m_ctx);
                        }
                    }
                    else if (!selected_plugin_exists && selected_plugin > -1)
                    {
                        RenderCenteredText(i18n->t("mapping", "message.not-available"));
                    }
                    else if (selected_developer > -1 && selected_plugin == -1)
                    {
                        RenderCenteredText(i18n->t("mapping", "message.select-plugin"));
                    }
                    else
                    {
                        RenderCenteredText(i18n->t("mapping", "message.select-developer"));
                    }

                    ImGui::EndChild(m_ctx);
                }

                ImGui::PopStyleVar(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            render_started = false;
        }
    }

    void Reset() override
    {
        mINI::INIFile file(GetPluginPath());
        file.read(ini);

        PopulateFields();
    }

    void Save() override
    {
        if (selected_plugin < 0)
        {
            return;
        }

        mINI::INIFile file(GetPluginPath());

        select_key = "select_" + std::to_string(selected_channel);
        fader_key = "fader_" + std::to_string(selected_channel);

        // We have a param
        if (select_param_index > 0)
        {
            ini[select_key]["name"] = select_name;
            ini[select_key]["param"] = std::to_string(std::get<0>(paramIds[select_param_index]));
            ini[select_key]["origname"] = std::get<1>(paramIds[select_param_index]);
            ini[select_key]["steps"] = std::to_string(select_nb_steps);
            ini[select_key]["uninvert-label"] = std::to_string(select_uninvert_label);
        }
        else
        {
            if (ini.has(select_key))
            {
                ini.remove(select_key);
            }
        }

        if (fader_param_index > 0)
        {
            ini[fader_key]["name"] = fader_name;
            ini[fader_key]["param"] = std::to_string(std::get<0>(paramIds[fader_param_index]));
            ini[fader_key]["origname"] = std::get<1>(paramIds[fader_param_index]);
            ini[fader_key]["uninvert-label"] = std::to_string(fader_uninvert_label);
        }
        else
        {
            if (ini.has(fader_key))
            {
                ini.remove(fader_key);
            }
        }

        if (file.write(ini, true))
        {
            Reset();
            ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "action.save.message"));
        };
    }

    void SetPageProperty(int type, std::string value) override
    {
        switch (type)
        {
        case 0:
        {
            const auto developer_it = std::find(developers.begin(), developers.end(), value);
            if (developer_it != developers.end())
            {
                selected_developer = developer_it - developers.begin();
                previous_selected_developer = selected_developer;
            }
        }
        break;

        case 1:
        {
            std::vector<std::string> plugin_list = plugins.at(selected_developer);

            for (int i = 0; i < (int)plugin_list.size(); i++)
            {
                if (plugin_list.at(i).compare(value) == 0)
                {
                    selected_plugin = i;
                }
            }

            previous_selected_plugin = selected_plugin;
            SetPluginData();
            PopulateFields();
        }
        break;
        }
    }

    void SetPageProperty(int type, int value) override
    {
        switch (type)
        {
        case 0:
        {
            selected_channel = value;
            previous_selected_channel = value;
            PopulateFields();
        }
        }
    }

    std::string GetPageStringProperty(int type) override
    {
        switch (type)
        {
        case 0:
        {
            return developers.at(selected_developer);
        }
        break;

        case 1:
        {
            return plugins.at(selected_developer).at(selected_plugin);
        }
        break;
        }

        return "";
    }

    int GetPageProperty(int type) override
    {
        switch (type)
        {
        case 0:
        {
            return selected_channel;
        }
        break;
        }

        return -1;
    };
};
