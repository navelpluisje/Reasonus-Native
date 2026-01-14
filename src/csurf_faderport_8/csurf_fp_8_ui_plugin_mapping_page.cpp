#include "../ui/csurf_ui_page_content.hpp"
#include <string.h>
#include "../shared/csurf_utils.hpp"
#include "../ui/csurf_ui_page_title.hpp"
#include "../ui/csurf_ui_text_input.hpp"
#include "../ui/csurf_ui_pagination_button.hpp"
#include "../ui/csurf_ui_icon_button.hpp"
#include "../ui/csurf_ui_list_box.hpp"
#include "../ui/csurf_ui_search_combo_input.hpp"
#include "../ui/csurf_ui_goto_input.hpp"

typedef std::tuple<int, std::string, int> PluginParam;

class CSurf_FP_8_PluginMappingPage : public CSurf_UI_PageContent
{
protected:
    mINI::INIStructure plugin_params;
    mINI::INIStructure previous_plugin_params;

    const int max_items = 13;
    int channel_offset = 0;
    int changed_items = 0;

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
    std::string param_search_query = "";

    std::string select_key = "";
    std::string select_name;
    int select_nb_steps;
    int select_param_index = -1;
    int previous_select_param_index = -1;
    int select_uninvert_label = 0;

    std::string fader_key = "";
    std::string fader_name;
    int fader_param_index;
    int fader_uninvert_label = 0;

    bool open_modal = true;

    std::vector<std::string> invert_labels = {"Inverted", "Not inverted"};

    std::vector<int> dirty_groups;

    bool channel_dirty = false;
    bool plugin_dirty = false;

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

    /**
     * Check if all the required fields are availebla. If not, add the field with a default value
     */
    void ValidatePluginData()
    {
        bool modified = false;
        for (auto const &it : plugin_params)
        {
            auto const &section = it.first;
            if (section.compare("global") == 0)
            {
                continue;
            }

            if (!plugin_params[section].has("uninvert-label") || plugin_params[section]["uninvert-label"].empty())
            {
                modified = true;
                plugin_params[section]["uninvert-label"] = "0";
            }
        }

        if (modified)
        {
            mINI::INIFile file(GetPluginPath());
            file.write(plugin_params, true);
        }
    }

    bool SetPluginData()
    {
        plugin_dirty = false;
        mINI::INIFile file(GetPluginPath());
        file.read(plugin_params);
        ValidatePluginData();
        file.read(previous_plugin_params);

        if (!PluginExists())
        {
            selected_plugin_exists = false;
            return false;
        }

        selected_plugin_exists = true;

        nb_channels = 0;
        for (auto const& it : plugin_params)
        {
            bool found_section = false;
            std::string section = it.first.c_str();

            if (section.find("select_") == 0)
            {
                section.replace(0, 7, "");
                found_section = true;
            }
            else if (section.find("fader_") == 0)
            {
                section.replace(0, 6, "");
                found_section = true;
            }

            if (found_section && isInteger(section))
            {
                int index = stoi(section);
                if (index > nb_channels)
                {
                    nb_channels = index;
                }
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
        int exist = TrackFX_AddByName(media_track, plugin_params["global"]["origname"].c_str(), false, -1);
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

        TrackFX_AddByName(media_track, plugin_params["global"]["origname"].c_str(), false, -1);

        for (int i = 0; i < TrackFX_GetNumParams(media_track, 0); i++)
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

        if (plugin_params.has(select_key))
        {
            if (!plugin_params[select_key].has("uninvert-label") || plugin_params[select_key]["uninvert-label"].empty())
            {
                plugin_params[select_key]["uninvert-label"] = "0";
                previous_plugin_params[select_key]["uninvert-label"] = "0";
            }

            select_name = plugin_params[select_key]["name"];
            select_nb_steps = stoi(plugin_params[select_key]["steps"]);
            select_uninvert_label = stoi(plugin_params[select_key]["uninvert-label"]);
            int param_id = stoi(plugin_params[select_key]["param"]);
            const auto it = std::find_if(
                paramIds.begin(),
                paramIds.end(),
                [param_id](PluginParam param)
                {
                    return param_id == std::get<0>(param);
                });

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

        if (plugin_params.has(fader_key))
        {
            if (!plugin_params[fader_key].has("uninvert-label") || plugin_params[fader_key]["uninvert-label"].empty())
            {
                plugin_params[fader_key]["uninvert-label"] = "0";
                previous_plugin_params[fader_key]["uninvert-label"] = "0";
            }

            fader_name = plugin_params[fader_key]["name"];
            int param_id = stoi(plugin_params[fader_key]["param"]);
            fader_uninvert_label = stoi(plugin_params[fader_key].has("uninvert-label") ? plugin_params[fader_key]["uninvert-label"] : "0");
            const auto it = std::find_if(
                paramIds.begin(),
                paramIds.end(),
                [param_id](PluginParam param)
                {
                    return param_id == std::get<0>(param);
                });
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

    void UpdateValues()
    {
        if (select_param_index > 0)
        {
            plugin_params[select_key]["name"] = select_name;
            plugin_params[select_key]["steps"] = std::to_string(select_nb_steps);
            plugin_params[select_key]["param"] = std::to_string(std::get<0>(paramIds[select_param_index]));
            plugin_params[select_key]["uninvert-label"] = std::to_string(select_uninvert_label);
        }

        if (fader_param_index > 0)
        {
            plugin_params[fader_key]["name"] = fader_name;
            plugin_params[fader_key]["param"] = std::to_string(std::get<0>(paramIds[fader_param_index]));
            plugin_params[fader_key]["uninvert-label"] = std::to_string(fader_uninvert_label);
        }
    }

    bool IsSelectDirty(int key)
    {
        std::string select = "select_" + std::to_string(key);
        if (key == selected_channel)
        {
            if (select_param_index <= 0 && plugin_params.has(select))
            {
                plugin_params.remove(select);
            }

            UpdateValues();
        }

        if (!plugin_params.has(select))
        {
            return false;
        }

        return plugin_params[select]["name"].compare(previous_plugin_params[select]["name"]) != 0 ||
               plugin_params[select]["steps"].compare(previous_plugin_params[select]["steps"]) != 0 ||
               plugin_params[select]["param"].compare(previous_plugin_params[select]["param"]) != 0 ||
               plugin_params[select]["uninvert-label"].compare(previous_plugin_params[select]["uninvert-label"]) != 0;
    }

    bool isFaderDirty(int key)
    {
        std::string fader = "fader_" + std::to_string(key);
        if (key == selected_channel)
        {
            if (select_param_index <= 0 && plugin_params.has(fader))
            {
                plugin_params.remove(fader);
            }

            UpdateValues();
        }

        if (!plugin_params.has(fader))
        {
            return false;
        }

        return plugin_params[fader]["name"].compare(previous_plugin_params[fader]["name"]) != 0 ||
               plugin_params[fader]["param"].compare(previous_plugin_params[fader]["param"]) != 0 ||
               plugin_params[fader]["uninvert-label"].compare(previous_plugin_params[fader]["uninvert-label"]) != 0;
    }

    bool IsGroupDirty(int key)
    {
        plugin_dirty = IsSelectDirty(key) || isFaderDirty(key);
        return plugin_dirty;
    }

    bool DirtyCheck()
    {
        if (changed_items > 0)
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

    void HandlePreviousClick()
    {
        int step_size = 1;

        if (ImGui::IsKeyDown(m_ctx, ImGui::Key_LeftShift))
        {
            step_size = 13;
        }

        selected_channel = max(selected_channel - step_size, 0);
        previous_selected_channel = selected_channel;
        PopulateFields();

        int overflow = max(nb_channels - max_items, 0);
        if (overflow > 0 && selected_channel > 5 && selected_channel < (6 + overflow))
        {
            channel_offset -= step_size;
        }
        else
        {
            channel_offset = minmax(0, selected_channel - 6, max(nb_channels - max_items, 0));
        }
    }

    void HandleNextClick()
    {
        int step_size = 1;

        if (ImGui::IsKeyDown(m_ctx, ImGui::Key_LeftShift))
        {
            step_size = 13;
        }

        selected_channel = min(selected_channel + step_size, nb_channels - 1);
        previous_selected_channel = selected_channel;
        PopulateFields();

        int overflow = max(nb_channels - max_items, 0);
        if (overflow > 0 && selected_channel > 6 && selected_channel < (7 + overflow))
        {
            channel_offset += step_size;
        }
        else
        {
            channel_offset = minmax(0, selected_channel - 6, max(nb_channels - max_items, 0));
        }
    }

    void HandleChannelClick(int index)
    {
        selected_channel = index;
        previous_selected_channel = selected_channel;
        PopulateFields();

        channel_offset = minmax(0, index - 6, max(nb_channels - max_items, 0));
    }

    void HandleResetChannel()
    {
        std::string select = "select_" + std::to_string(selected_channel);
        std::string fader = "fader_" + std::to_string(selected_channel);

        if (previous_plugin_params.has(select) && !previous_plugin_params[select]["param"].empty())
        {
            plugin_params.set(select, previous_plugin_params[select]);
        }
        else
        {
            plugin_params.remove(select);
        }

        if (previous_plugin_params.has(fader) && !previous_plugin_params[fader]["param"].empty())
        {
            plugin_params.set(fader, previous_plugin_params[fader]);
        }
        else
        {
            plugin_params.remove(fader);
        }

        PopulateFields();
    }

    void HandleAddChannelAfterSelected()
    {
        HandleAddChannelAfter(selected_channel);
    }

    void HandleAddChannelAfter(int index)
    {
        std::string select, fader, next_select, next_fader;

        selected_channel = index + 1;
        nb_channels += 1;

        for (int i = nb_channels - 1; i >= selected_channel; i--)
        {
            select = "select_" + std::to_string(i);
            fader = "fader_" + std::to_string(i);
            next_select = "select_" + std::to_string(i + 1);
            next_fader = "fader_" + std::to_string(i + 1);

            if (plugin_params.has(select))
            {
                plugin_params.set(next_select, plugin_params[select]);
            }
            else
            {
                plugin_params.remove(next_select);
            }

            if (plugin_params.has(fader))
            {
                plugin_params.set(next_fader, plugin_params[fader]);
            }
            else
            {
                plugin_params.remove(next_fader);
            }
        }

        select = "select_" + std::to_string(selected_channel);
        fader = "fader_" + std::to_string(selected_channel);

        if (plugin_params.has(select))
        {
            plugin_params.remove(select);
        }
        if (plugin_params.has(fader))
        {
            plugin_params.remove(fader);
        }

        HandleChannelClick(selected_channel);
    }

    void HandleDeleteChannel()
    {
        HandleDeleteChannelById(selected_channel);
    }

    void HandleDeleteChannelById(int index)
    {
        std::string select = "select_" + std::to_string(index);
        std::string fader = "fader_" + std::to_string(index);
        std::string next_select, next_fader;

        if (plugin_params.has(select))
        {
            plugin_params.remove(select);
        }
        if (plugin_params.has(fader))
        {
            plugin_params.remove(fader);
        }

        nb_channels -= 1;

        if (selected_channel == nb_channels)
        {
            HandleChannelClick(selected_channel - 1);
            return;
        }

        for (int i = index; i <= nb_channels; i++)
        {
            select = "select_" + std::to_string(i);
            fader = "fader_" + std::to_string(i);
            next_select = "select_" + std::to_string(i + 1);
            next_fader = "fader_" + std::to_string(i + 1);

            if (plugin_params.has(next_select))
            {
                plugin_params.set(select, plugin_params[next_select]);
            }
            else
            {
                plugin_params.remove(select);
            }

            if (plugin_params.has(next_fader))
            {
                plugin_params.set(fader, plugin_params[next_fader]);
            }
            else
            {
                plugin_params.remove(fader);
            }
        }
        PopulateFields();
    }

    void handleGroupDrop(int from, int to)
    {
        if (from == to)
        {
            return;
        }
        int from_id = to > from ? from : from + 1;
        int to_id = to < from ? to : to + 1;

        std::string select = "select_" + std::to_string(to_id);
        std::string fader = "fader_" + std::to_string(to_id);
        std::string from_select = "select_" + std::to_string(from_id);
        std::string from_fader = "fader_" + std::to_string(from_id);

        this->HandleAddChannelAfter(to_id - 1);

        if (plugin_params.has(from_select))
        {
            plugin_params.set(select, plugin_params[from_select]);
        }
        else
        {
            plugin_params.remove(select);
        }

        if (plugin_params.has(from_fader))
        {
            plugin_params.set(fader, plugin_params[from_fader]);
        }
        else
        {
            plugin_params.remove(fader);
        }

        selected_channel = to;
        HandleDeleteChannelById(from_id);
    }

public:
    CSurf_FP_8_PluginMappingPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets)
    {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_8);

        SetPluginFolders();
    };

    virtual ~CSurf_FP_8_PluginMappingPage() {};

    void RenderMappingsList()
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
        using namespace std::placeholders; // for `_1, _2 etc`

        double space_x, space_y;
        changed_items = 0;
        bool has_style_var = false;
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 22, 12);

        if (ImGui::BeginChild(m_ctx, "##channel-list", 0.0, 30))
        {
            ReaSonusIconButton(
                m_ctx,
                assets,
                IconArrowLeft,
                "previous-button",
                selected_channel == 0,
                ButtonThemeDefault,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandlePreviousClick, this));

            for (int i = channel_offset; i < min(max_items + channel_offset, nb_channels); i++)
            {
                has_style_var = false;
                if (i > channel_offset)
                {
                    has_style_var = true;
                    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 4, 0);
                }

                ImGui::SameLine(m_ctx);
                bool dirty = IsGroupDirty(i);

                if (dirty)
                {
                    changed_items += 1;
                }

                ReaSonusPaginationButton(
                    m_ctx,
                    assets,
                    std::to_string(i + 1),
                    i,
                    dirty,
                    selected_channel == i,
                    std::bind(&CSurf_FP_8_PluginMappingPage::HandleChannelClick, this, _1),
                    std::bind(&CSurf_FP_8_PluginMappingPage::handleGroupDrop, this, _1, _2));

                if (has_style_var)
                {
                    ImGui::PopStyleVar(m_ctx);
                }
            }

            ImGui::SameLine(m_ctx);
            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + space_x - 32);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconArrowRight,
                "next-button",
                selected_channel == nb_channels - 1,
                ButtonThemeDefault,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleNextClick, this));

            ImGui::EndChild(m_ctx);
        }
        ImGui::PopStyleVar(m_ctx);
    }

    void RenderInformationBar()
    {
        double space_x, space_y;

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, 54.0, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {
            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

            if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, space_y, ImGui::ChildFlags_AutoResizeY))
            {
                ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
                ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 8);

                ImGui::Text(m_ctx, i18n->t("mapping", "edit.message.changed", std::to_string(changed_items), std::to_string(nb_channels)).c_str());

                ImGui::PopStyleColor(m_ctx);
                ImGui::PopFont(m_ctx);
                ImGui::EndChild(m_ctx);
            }

            ImGui::SameLine(m_ctx);

            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + space_x - 144);

            ReaSonusGoToInput(m_ctx, assets, &selected_channel);

            ImGui::SameLine(m_ctx);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconUndo,
                "mapping-reset-group",
                ButtonThemeAccent,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleResetChannel, this));

            ImGui::SameLine(m_ctx);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconAdd,
                "mapping-add",
                ButtonThemeAccent,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleAddChannelAfterSelected, this));

            ImGui::SameLine(m_ctx);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconDelete,
                "mapping-delete",
                ButtonThemeAccent,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleDeleteChannel, this));

            ImGui::EndChild(m_ctx);
        }
        UiElements::PopReaSonusGroupStyle(m_ctx);
    }

    void RenderMappingSelect(double height)
    {
        double space_x, space_y;

        UiElements::PushReaSonusGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, height, ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY))
        {
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.select.label").c_str(), true);
            if (params.size() > 0)
            {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusSearchComboInput(
                    m_ctx,
                    assets,
                    i18n->t("mapping", "edit.select.param.label"),
                    params,
                    &select_param_index,
                    &param_search_query,
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
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.fader.label"), true);
            if (params.size() > 0)
            {
                ReaSonusSearchComboInput(
                    m_ctx,
                    assets,
                    i18n->t("mapping", "edit.fader.param.label"),
                    params,
                    &fader_param_index,
                    &param_search_query);
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

    void RenderCenteredText(std::string content, IconFont icon)
    {
        double text_width, text_height, available_width, available_height;
        ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
        ImGui::PushFont(m_ctx, assets->GetIconFont(), 84);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
        ImGui::CalcTextSize(m_ctx, std::string(1, icon).c_str(), &text_width, &text_height);
        ImGui::SetCursorPosX(m_ctx, available_width / 2 - text_width / 2);
        ImGui::SetCursorPosY(m_ctx, (available_height / 2 - text_height / 2) - 50);
        ImGui::Text(m_ctx, std::string(1, icon).c_str());
        ImGui::PopStyleColor(m_ctx);
        ImGui::PopFont(m_ctx);

        ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeLarge);
        ImGui::CalcTextSize(m_ctx, content.c_str(), &text_width, &text_height);
        ImGui::SetCursorPosX(m_ctx, available_width / 2 - text_width / 2);
        ImGui::SetCursorPosY(m_ctx, (available_height / 2 - text_height / 2) + 40);
        ImGui::Text(m_ctx, content.c_str());
        ImGui::PopFont(m_ctx);
    }

    void Render() override
    {
        if (!render_started)
        {
            render_started = true;
            double space_x, space_y;

            if (selected_developer != previous_selected_developer)
            {
                channel_offset = 0;
                selected_plugin_exists = false;
                if (DirtyCheck())
                {
                    selected_plugin = -1;
                    previous_selected_plugin = -1;
                    selected_channel = 0;
                    changed_items = 0;
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
                channel_offset = 0;
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
                PopulateFields();
                previous_selected_channel = selected_channel;
            }

            if (select_param_index != previous_select_param_index)
            {
                previous_select_param_index = select_param_index;
                select_nb_steps = std::get<2>(paramIds[select_param_index]);
            }

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
            if (ImGui::BeginChild(m_ctx, "main_mapping_content", 0.0, 0.0, ImGui::ChildFlags_None))
            {
                RenderMappingsList();
                ImGui::SameLine(m_ctx);

                if (ImGui::BeginChild(m_ctx, "mapping_content", 0.0, 0.0))
                {
                    if (selected_plugin > -1 && selected_plugin_exists)
                    {
                        ImGui::Text(m_ctx, selected_plugin < 0 || selected_plugin >= plugins[selected_developer].size() ? "Groups" : std::string(developers[selected_developer] + " :: " + plugins[selected_developer][selected_plugin]).c_str());
                        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);
                        RenderInformationBar();
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
                        RenderCenteredText(i18n->t("mapping", "message.not-available"), IconRemove);
                    }
                    else if (selected_developer > -1 && selected_plugin == -1)
                    {
                        RenderCenteredText(i18n->t("mapping", "message.select-plugin"), IconInstrument);
                    }
                    else
                    {
                        RenderCenteredText(i18n->t("mapping", "message.select-developer"), IconUser);
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
        channel_offset = 0;
        selected_channel = 0;
        SetPluginData();
        PopulateFields();
    }

    void Save() override
    {
        int prev_selected_plugin = selected_channel;
        if (selected_plugin < 0)
        {
            return;
        }

        mINI::INIFile file(GetPluginPath());
        if (file.write(plugin_params, true))
        {
            Reset();
            selected_channel = prev_selected_plugin;
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
