#include <utility>
#include "../csurf_ui_page_content.hpp"
#include "../../shared/csurf_utils.hpp"
#include "../../shared/csurf_plugin_utils.hpp"
#include "../../shared/csurf_daw.hpp"
#include "../components/csurf_ui_page_title.hpp"
#include "../components/csurf_ui_text_input.hpp"
#include "../components/csurf_ui_pagination_button.hpp"
#include "../components/csurf_ui_icon_button.hpp"
#include "../components/csurf_ui_search_combo_input.hpp"
#include "../components/csurf_ui_goto_input.hpp"
#include "../components/csurf_ui_int_input.hpp"
#include "../components/csurf_ui_combo_input.hpp"
#include "../components/csurf_ui_button_bar.hpp"
#include "../components/csurf_ui_plugin_selectable.hpp"
#include "../components/csurf_ui_developer_filter_form.hpp"
#include "../components/csurf_ui_add_plugin_mapping_form.hpp"
#include "../../i18n/i18n.hpp"
#include "../windows/csurf_ui_fp_8_control_panel.hpp"

class CSurf_FP_8_PluginMappingPage : public CSurf_UI_PageContent // NOLINT(*-use-internal-linkage)
{
    mINI::INIStructure plugin_params;
    mINI::INIStructure previous_plugin_params;

    const int max_items = 12;
    int channel_offset = 0;
    int changed_items = 0;

    bool render_started = false;
    std::string plugin_folder_path = PluginUtils::GetReaSonusPluginFolderPath();
    std::vector<std::string> developers;
    std::vector<std::string> plugin_types = PluginUtils::GetPluginTypes();
    std::set<std::string> installed_developers;

    int newly_selected_plugin_type = 0;
    bool save_selected_plugin_type = false;
    bool cancel_selected_plugin_type = false;
    std::string dummy;
    int selected_developer = -1;
    int previous_selected_developer = -1;

    std::vector<std::vector<std::string>> plugins;
    int hovered_plugin = -1;
    int selected_plugin = -1;
    int previous_selected_plugin = -1;
    bool selected_plugin_exists = false;
    bool selected_plugin_has_type = false;
    bool selected_plugin_filename_has_type = false;
    bool selected_plugin_type_mismatch = false;
    bool selected_plugin_params_error = false;
    std::string selected_plugin_filename_type;

    int nb_channels = 0;
    int selected_channel = -1;
    int previous_selected_channel = 0;

    std::vector<std::string> param_names;
    std::vector<PluginParam> param_data;

    std::string select_key;
    std::string select_name;
    int select_nb_steps{};
    int select_param_index = -1;
    int previous_select_param_index = -1;
    int select_uninvert_label = 0;

    bool show_developer_filters = false;
    bool show_add_plugin_mapping = false;

    std::string fader_key;
    std::string fader_name;
    int fader_param_index{};
    int fader_uninvert_label = 0;

    std::vector<std::string> invert_labels = {"Inverted", "Not inverted"};

    std::vector<int> dirty_groups;

    ReaSonusSearchComboInput *SelectParamList;
    ReaSonusSearchComboInput *FaderParamList;
    ReaSonusDeveloperFilterForm *DevelopersFilterForm;
    ReaSonusAddPluginMappingForm *AddPluginMappingForm;
    ReaSonusComboInput *SelectLabelInvertCombo;
    ReaSonusComboInput *FaderLabelInvertCombo;
    ReaSonusComboInput *PluginTypeCombo;

    bool plugin_dirty = false;

protected:
    void SetPluginFolders() {
        developers.clear();
        plugins.clear();

        developers = PluginUtils::GetpluginDevelopers(true);

        for (const auto &developer: developers) {
            plugins.push_back(PluginUtils::GetDeveloperPluginMappings(developer, true));
        }
    }

    std::string GetPluginPath() {
        return createPathName({
            plugin_folder_path, developers[selected_developer], plugins[selected_developer][selected_plugin]
        });
    }

    std::string GetPluginSavePath() {
        return createPathName({
            plugin_folder_path, developers[previous_selected_developer],
            plugins[previous_selected_developer][previous_selected_plugin]
        });
    }

    /**
     * Check if all the required fields are available. If not, add the field with a default value
     */
    void ValidatePluginData() {
        bool modified = false;
        for (const auto &[fst, snd]: plugin_params) {
            auto const &section = fst;
            if (section == "global") {
                continue;
            }

            if (!plugin_params[section].has("uninvert-label") || plugin_params[section]["uninvert-label"].empty()) {
                modified = true;
                plugin_params[section]["uninvert-label"] = "0";
            }
        }

        if (modified) {
            const mINI::INIFile file(GetPluginPath());
            file.write(plugin_params, true);
        }
    }

    bool CheckPathForPluginType(const std::string &plugin_path) {
        selected_plugin_filename_type = "";

        for (const auto &plugin_type: plugin_types) { // NOLINT(*-use-anyofallof)
            const std::string type_ini = plugin_type + ".ini";
            const size_t position = plugin_path.find(type_ini);

            if (position == std::string::npos) {
                continue;
            }

            if (position == plugin_path.length() - type_ini.length()) {
                selected_plugin_filename_type = plugin_type;
                return true;
            }
        }

        return false;
    }

    bool SetPluginData() {
        plugin_dirty = false;
        bool plugin_type_error = false;

        const std::string plugin_path = GetPluginPath();
        const mINI::INIFile file(plugin_path);
        file.read(plugin_params);
        ValidatePluginData();
        file.read(previous_plugin_params);
        nb_channels = 0;

        // Type not present in filename
        if (!CheckPathForPluginType(plugin_path)) {
            selected_plugin_filename_has_type = false;
            plugin_type_error = true;
        }

        // Type not present in data
        selected_plugin_has_type = !plugin_params["global"]["type"].empty();
        if (!selected_plugin_has_type) {
            plugin_type_error = true;
        }

        // Type in filename and data do not match
        if (selected_plugin_filename_type != plugin_params["global"]["type"]) {
            selected_plugin_type_mismatch = true;
            plugin_type_error = true;
        }

        if (plugin_type_error) {
            selected_plugin_exists = false;
            newly_selected_plugin_type = 0;
            return false;
        }

        selected_plugin_exists = true;
        selected_plugin_filename_has_type = true;
        selected_plugin_type_mismatch = false;

        for (const auto &[fst, snd]: plugin_params) {
            const std::string section = fst;
            const std::string group_id = split(section, "_").back();

            if (!isInteger(group_id)) {
                continue;
            }

            nb_channels = max(std::stoi(group_id) + 1, nb_channels);
        }

        selected_plugin_params_error = false;
        if (!GetPluginParams()) {
            selected_plugin_params_error = true;
            return false;
        }

        return true;
    }

    bool PluginExists() {
        // Create a track, add the plugin and start reading the params
        InsertTrackAtIndex(0, false);
        MediaTrack *media_track = GetTrack(nullptr, 0);
        DAW::SetMixerTrackVisible(media_track, false);
        DAW::SetTCPTrackVisible(media_track, false);

        const std::string plugin_request_string = PluginUtils::GetPluginRequestString(
            plugin_params["global"]["origname"],
            plugin_params["global"]["type"]
        );
        const int exist = TrackFX_AddByName(media_track, plugin_request_string.c_str(), false, -1);
        DeleteTrack(media_track);

        return exist > -1;
    }

    bool GetPluginParams() {
        // Prepare the vectors
        param_names.clear();
        param_data.clear();
        param_names.push_back(i18n->t("mapping", "edit.select.param.first-item"));
        param_data.emplace_back(-1, i18n->t("mapping", "edit.select.param.first-item"), 0);

        const std::string developer_name = PluginUtils::ExtractDeveloperName(plugin_params["global"]["origname"]);
        const std::string plugin_name = PluginUtils::ExtractPluginName(plugin_params["global"]["origname"]);

        if (!PluginUtils::HasPluginMappingCache(
            developer_name, plugin_name, plugin_params["global"]["type"]
        )) {
            if (!PluginExists()) {
                return false;
            }

            PluginUtils::UpdatePluginMappingCacheFile(PluginUtils::GetPluginRequestString(
                plugin_params["global"]["origname"],
                plugin_params["global"]["type"]
            ));

            ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "action.cache-created.success.message"));
        }

        mINI::INIStructure cache = PluginUtils::GetPluginMappingCacheStructure(
            developer_name, plugin_name, plugin_params["global"]["type"]
        );

        const int num_params = static_cast<int>(cache["id"].size());
        if (num_params < 1) {
            return false;
        }

        for (int i = 0; i < num_params; i++) {
            const std::string param_name = cache["params"][std::to_string(i)];

            param_names.emplace_back(param_name);
            param_data.emplace_back(stoi(cache["id"][std::to_string(i)]), param_name,
                                    stoi(cache["steps"][std::to_string(i)]));
        }

        // SelectParamList->SetListItems(&param_names);
        return true;
    }

    void PopulateFields() {
        select_key = "select_" + std::to_string(selected_channel);
        fader_key = "fader_" + std::to_string(selected_channel);

        if (plugin_params.has(select_key)) {
            if (!plugin_params[select_key].has("uninvert-label") || plugin_params[select_key]["uninvert-label"].
                empty()) {
                plugin_params[select_key]["uninvert-label"] = "0";
                previous_plugin_params[select_key]["uninvert-label"] = "0";
            }

            select_name = plugin_params[select_key]["name"];
            select_nb_steps = stoi(plugin_params[select_key]["steps"]);
            select_uninvert_label = stoi(plugin_params[select_key]["uninvert-label"]);

            const int param_id = stoi(plugin_params[select_key]["param"]);
            const auto iterator = std::find_if(
                param_data.begin(),
                param_data.end(),
                [param_id](const PluginParam &param) {
                    return param_id == std::get<0>(param);
                });

            if (iterator != param_data.end()) {
                select_param_index = iterator - param_data.begin();
                previous_select_param_index = select_param_index;
            }
        } else {
            select_name = "";
            select_nb_steps = 0;
            select_param_index = 0;
            select_uninvert_label = 0;
        }

        if (plugin_params.has(fader_key)) {
            if (!plugin_params[fader_key].has("uninvert-label") || plugin_params[fader_key]["uninvert-label"].empty()) {
                plugin_params[fader_key]["uninvert-label"] = "0";
                previous_plugin_params[fader_key]["uninvert-label"] = "0";
            }

            const int param_id = stoi(plugin_params[fader_key]["param"]);
            fader_name = plugin_params[fader_key]["name"];
            fader_uninvert_label = stoi(plugin_params[fader_key].has("uninvert-label")
                                            ? plugin_params[fader_key]["uninvert-label"]
                                            : "0");

            const auto iterator = std::find_if(
                param_data.begin(),
                param_data.end(),
                [param_id](const PluginParam &param) {
                    return param_id == std::get<0>(param);
                });
            if (iterator != param_data.end()) {
                fader_param_index = iterator - param_data.begin();
            }
        } else {
            fader_name = "";
            fader_param_index = 0;
            fader_uninvert_label = 0;
        }
    }

    void UpdateValues() {
        if (select_param_index > 0) {
            plugin_params[select_key]["name"] = select_name;
            plugin_params[select_key]["steps"] = std::to_string(select_nb_steps);
            plugin_params[select_key]["param"] = std::to_string(std::get<0>(param_data[select_param_index]));
            plugin_params[select_key]["uninvert-label"] = std::to_string(select_uninvert_label);
        }

        if (fader_param_index > 0) {
            plugin_params[fader_key]["name"] = fader_name;
            plugin_params[fader_key]["param"] = std::to_string(std::get<0>(param_data[fader_param_index]));
            plugin_params[fader_key]["uninvert-label"] = std::to_string(fader_uninvert_label);
        }
    }

    bool IsSelectDirty(const int key) {
        const std::string select = "select_" + std::to_string(key);
        if (key == selected_channel) {
            if (select_param_index <= 0 && plugin_params.has(select)) {
                plugin_params.remove(select);
            }

            UpdateValues();
        }

        if (!plugin_params.has(select)) {
            return false;
        }

        return plugin_params[select]["name"] != previous_plugin_params[select]["name"] ||
               plugin_params[select]["steps"] != previous_plugin_params[select]["steps"] ||
               plugin_params[select]["param"] != previous_plugin_params[select]["param"] ||
               plugin_params[select]["uninvert-label"] != previous_plugin_params[select]["uninvert-label"];
    }

    bool isFaderDirty(const int key) {
        const std::string fader = "fader_" + std::to_string(key);
        if (key == selected_channel) {
            if (select_param_index <= 0 && plugin_params.has(fader)) {
                plugin_params.remove(fader);
            }

            UpdateValues();
        }

        if (!plugin_params.has(fader)) {
            return false;
        }

        return plugin_params[fader]["name"] != previous_plugin_params[fader]["name"] ||
               plugin_params[fader]["param"] != previous_plugin_params[fader]["param"] ||
               plugin_params[fader]["uninvert-label"] != previous_plugin_params[fader]["uninvert-label"];
    }

    bool IsGroupDirty(const int key) {
        plugin_dirty = IsSelectDirty(key) || isFaderDirty(key);
        return plugin_dirty;
    }

    bool DirtyCheck() {
        if (changed_items > 0) {
            const int res = MB(
                i18n->t("mapping", "popup.unsaved.message").c_str(),
                i18n->t("mapping", "popup.unsaved.title").c_str(),
                3
            );

            if (res == 6) {
                Save();
            }

            return res != 2;
        }
        return true;
    }

    void HandleRemoveMappingClick(const int plugin_index) {
        if (PluginUtils::DeletePluginMappingFile(
            developers[selected_developer],
            ExtractPluginNameFromFile(plugins[selected_developer][plugin_index]),
            ExtractPluginTypeFromFile(plugins[selected_developer][plugin_index])
        )) {
            SetPluginFolders();
        }
    }

    void HandlePluginListItemClick(const int plugin_index) {
        (void) plugin_index;

        show_developer_filters = false;
    }

    void HandleRebuildPluginCacheClick(const int plugin_index) const {
        if (PluginUtils::UpdatePluginMappingCacheFile(PluginUtils::GetPluginRequestString(
            developers[selected_developer],
            ExtractPluginNameFromFile(plugins[selected_developer][plugin_index]),
            ExtractPluginTypeFromFile(plugins[selected_developer][plugin_index])
        ))) {
            ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "action.rebuild-cache.success.message"));
        }
    }

    void HandleAddToFilterListClick(const int param_index) {
        if (!PluginUtils::AddDeveloperParamFilter(developers[selected_developer], param_names[param_index])) {
            return;
        }

        if (PluginUtils::UpdatePluginMappingCacheFileByDeveloper(developers[selected_developer])) {
            GetPluginParams();
        }
    }

    /**
     * Callback function for the developers filter form.
     * This will trigger the rebuild of the params and show a friendly message
     */
    void HandleFilterChangesSaved() {
        GetPluginParams();
        ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "action.save.filters.message"));
    }

    void HandleManageFilterListClick() {
        show_developer_filters = true;
        DevelopersFilterForm = new ReaSonusDeveloperFilterForm(
            m_ctx,
            assets,
            &developers
        );
        DevelopersFilterForm->SetSelectedDeveloper(selected_developer);
    }

    void HandlePreviousClick() {
        int step_size = 1;

        if (ImGui::IsKeyDown(m_ctx, ImGui::Key_LeftShift)) {
            step_size = max_items;
        }

        selected_channel = max(selected_channel - step_size, 0);
        previous_selected_channel = selected_channel;
        PopulateFields();

        const int overflow = max(nb_channels - max_items, 0);
        if (overflow > 0 && selected_channel > 5 && selected_channel < 6 + overflow) {
            channel_offset -= step_size;
        } else {
            channel_offset = minmax(0, selected_channel - 6, max(nb_channels - max_items, 0));
        }
        SelectParamList->ClearSearchQuery();
        FaderParamList->ClearSearchQuery();
    }

    void HandleNextClick() {
        int step_size = 1;

        if (ImGui::IsKeyDown(m_ctx, ImGui::Key_LeftShift)) {
            step_size = max_items;
        }

        selected_channel = min(selected_channel + step_size, nb_channels - 1);
        previous_selected_channel = selected_channel;
        PopulateFields();

        const int overflow = max(nb_channels - max_items, 0);
        if (overflow > 0 && selected_channel > 6 && selected_channel < 7 + overflow) {
            channel_offset += step_size;
        } else {
            channel_offset = minmax(0, selected_channel - 6, max(nb_channels - max_items, 0));
        }
        SelectParamList->ClearSearchQuery();
        FaderParamList->ClearSearchQuery();
    }

    void HandleChannelClick(const int index) {
        selected_channel = index;
        previous_selected_channel = selected_channel;
        PopulateFields();

        channel_offset = minmax(0, index - 6, max(nb_channels - max_items, 0));

        SelectParamList->ClearSearchQuery();
        FaderParamList->ClearSearchQuery();
    }

    void HandleResetChannel() {
        const std::string select = "select_" + std::to_string(selected_channel);
        const std::string fader = "fader_" + std::to_string(selected_channel);

        if (previous_plugin_params.has(select) && !previous_plugin_params[select]["param"].empty()) {
            plugin_params.set(select, previous_plugin_params[select]);
        } else {
            plugin_params.remove(select);
        }

        if (previous_plugin_params.has(fader) && !previous_plugin_params[fader]["param"].empty()) {
            plugin_params.set(fader, previous_plugin_params[fader]);
        } else {
            plugin_params.remove(fader);
        }

        PopulateFields();
    }

    void HandleAddChannelAfterSelected() {
        // We only have global and no other mapping items, so the selected plugin has to be -1
        if (plugin_params.size() == 1) {
            selected_channel = -1;
        }
        HandleAddChannelAfter(selected_channel);
    }

    void HandleAddChannelAfter(int index) {
        std::string select;
        std::string fader;
        std::string next_select;
        std::string next_fader;

        selected_channel = index + 1;
        nb_channels += 1;

        for (int i = nb_channels - 1; i >= selected_channel; i--) {
            select = "select_" + std::to_string(i);
            fader = "fader_" + std::to_string(i);
            next_select = "select_" + std::to_string(i + 1);
            next_fader = "fader_" + std::to_string(i + 1);

            if (plugin_params.has(select)) {
                plugin_params.set(next_select, plugin_params[select]);
            } else {
                plugin_params.remove(next_select);
            }

            if (plugin_params.has(fader)) {
                plugin_params.set(next_fader, plugin_params[fader]);
            } else {
                plugin_params.remove(next_fader);
            }
        }

        select = "select_" + std::to_string(selected_channel);
        fader = "fader_" + std::to_string(selected_channel);

        if (plugin_params.has(select)) {
            plugin_params.remove(select);
        }
        if (plugin_params.has(fader)) {
            plugin_params.remove(fader);
        }

        HandleChannelClick(selected_channel);
    }

    void HandleDeleteChannel() {
        HandleDeleteChannelById(selected_channel);
    }

    void HandleDeleteChannelById(const int index) {
        std::string select = "select_" + std::to_string(index);
        std::string fader = "fader_" + std::to_string(index);

        if (plugin_params.has(select)) {
            plugin_params.remove(select);
        }
        if (plugin_params.has(fader)) {
            plugin_params.remove(fader);
        }

        nb_channels -= 1;

        if (selected_channel == nb_channels) {
            HandleChannelClick(selected_channel - 1);
            return;
        }

        for (int i = index; i <= nb_channels; i++) {
            select = "select_" + std::to_string(i);
            fader = "fader_" + std::to_string(i);
            const std::string next_select = "select_" + std::to_string(i + 1);
            const std::string next_fader = "fader_" + std::to_string(i + 1);

            if (plugin_params.has(next_select)) {
                plugin_params.set(select, plugin_params[next_select]);
            } else {
                plugin_params.remove(select);
            }

            if (plugin_params.has(next_fader)) {
                plugin_params.set(fader, plugin_params[next_fader]);
            } else {
                plugin_params.remove(fader);
            }
        }
        PopulateFields();
    }

    void HandleGroupDrop(const int from, const int to) // NOLINT(*-identifier-length)
    {
        if (from == to) {
            return;
        }
        const int from_id = to > from ? from : from + 1;
        const int to_id = to < from ? to : to + 1;

        const std::string select = "select_" + std::to_string(to_id);
        const std::string fader = "fader_" + std::to_string(to_id);
        const std::string from_select = "select_" + std::to_string(from_id);
        const std::string from_fader = "fader_" + std::to_string(from_id);

        this->HandleAddChannelAfter(to_id - 1);

        if (plugin_params.has(from_select)) {
            plugin_params.set(select, plugin_params[from_select]);
        } else {
            plugin_params.remove(select);
        }

        if (plugin_params.has(from_fader)) {
            plugin_params.set(fader, plugin_params[from_fader]);
        } else {
            plugin_params.remove(fader);
        }

        selected_channel = to;
        HandleDeleteChannelById(from_id);
    }

    static int CheckPluginTypeFromFile(const std::string &plugin) {
        const std::vector<std::string> plugin_types = PluginUtils::GetPluginTypes();

        for (const auto &plugin_type: plugin_types) {
            const std::string type_ini = "." + plugin_type + ".ini";
            const size_t position = plugin.find(type_ini);

            if (position == std::string::npos) {
                continue;
            }

            if (position == plugin.length() - type_ini.length()) {
                return position;
            }
        }

        return -1;
    }

    static std::string ExtractPluginNameFromFile(const std::string &plugin_name) {
        const int type_location = CheckPluginTypeFromFile(plugin_name);

        if (type_location > -1) {
            return plugin_name.substr(0, type_location);
        }

        return plugin_name;
    }

    static std::string ExtractPluginTypeFromFile(const std::string &plugin_name) {
        if (CheckPluginTypeFromFile(plugin_name) > -1) {
            std::vector<std::string> splitted_plugin_name = split(plugin_name, ".");

            // Remove the ini part. If the vector length is longer then we have a plugin type available
            splitted_plugin_name.pop_back();

            if (splitted_plugin_name.size() > 1) {
                return splitted_plugin_name.at(splitted_plugin_name.size() - 1);
            }
        }

        return "";
    }

    void RenderParamListContextMenu(int param_index) {
        ImGui::PushFont(m_ctx, assets->GetMainFont(), 13);
        if (ImGui::BeginChild(m_ctx, "plugin-mapping-context", 0.0, 0.0,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY |
                              ImGui::ChildFlags_AutoResizeX
        )) {
            if (ImGui::Selectable(
                m_ctx, I18n::GetInstance()->t(
                    "mapping", "list.param.item.context-menu.add-filter").c_str())) {
                ImGui::CloseCurrentPopup(m_ctx);
                HandleAddToFilterListClick(param_index);
            }
            if (ImGui::Selectable(m_ctx, I18n::GetInstance()->t("mapping", "list.item.context-menu.close").c_str())) {
                ImGui::CloseCurrentPopup(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
        ImGui::PopFont(m_ctx);
    }

    void RenderPluginListContextMenuItems(const int plugin_index) {
        ImGui::PushFont(m_ctx, assets->GetMainFont(), 13);
        if (ImGui::BeginChild(m_ctx, "plugin-mapping-context", 0.0, 0.0,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY |
                              ImGui::ChildFlags_AutoResizeX
        )) {
            if (ImGui::Selectable(m_ctx, I18n::GetInstance()->t("mapping", "list.item.context-menu.rebuild").c_str())) {
                ImGui::CloseCurrentPopup(m_ctx);
                HandleRebuildPluginCacheClick(plugin_index);
            }
            if (ImGui::Selectable(m_ctx, I18n::GetInstance()->t("mapping", "list.item.context-menu.delete").c_str())) {
                ImGui::CloseCurrentPopup(m_ctx);
                HandleRemoveMappingClick(plugin_index);
            }
            if (ImGui::Selectable(
                m_ctx, I18n::GetInstance()->t("mapping", "list.item.context-menu.manage-filter").c_str())) {
                ImGui::CloseCurrentPopup(m_ctx);
                HandleManageFilterListClick();
            }
            if (ImGui::Selectable(m_ctx, I18n::GetInstance()->t("mapping", "list.item.context-menu.close").c_str())) {
                ImGui::CloseCurrentPopup(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
        ImGui::PopFont(m_ctx);
    }

    /**
     * Select the newly created developer and plugin
     * @param developer
     * @param plugin_name
     * @param plugin_type
     */
    void HandlePluginMappingAdded(
        const std::string &developer,
        const std::string &plugin_name,
        const std::string &plugin_type
    ) {
        // First we're updating the
        SetPluginFolders();
        int developer_index = selected_developer;
        int plugin_index = selected_plugin;
        const auto developer_it = std::find(developers.begin(), developers.end(), developer);

        if (developer_it < developers.end()) {
            developer_index = developer_it - developers.begin();
        }
        selected_developer = developer_index;
        previous_selected_developer = developer_index;

        std::vector<std::string> developer_plugins = plugins.at(developer_index);
        const auto plugin_it = std::find(
            developer_plugins.begin(),
            developer_plugins.end(),
            plugin_name + "." + plugin_type + ".ini"
        );

        if (plugin_it < developer_plugins.end()) {
            plugin_index = plugin_it - developer_plugins.begin();
        }

        selected_plugin = plugin_index;
        PluginCheck(true);

        ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "add.mapping.success.message"));
    }

public:
    CSurf_FP_8_PluginMappingPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets) {
        using namespace std::placeholders; // for `_1, _2 etc`

        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_8);

        SelectParamList = new ReaSonusSearchComboInput(
            m_ctx,
            assets,
            i18n->t("mapping", "edit.select.param.label"),
            &param_names,
            &select_param_index,
            settings->ShouldClearParamInput()
        );

        FaderParamList = new ReaSonusSearchComboInput(
            m_ctx,
            assets,
            i18n->t("mapping", "edit.fader.param.label"),
            &param_names,
            &fader_param_index,
            settings->ShouldClearParamInput()
        );

        AddPluginMappingForm = new ReaSonusAddPluginMappingForm(
            m_ctx,
            assets,
            std::bind(&CSurf_FP_8_PluginMappingPage::HandlePluginMappingAdded, this, _1, _2, _3)

        );

        SelectLabelInvertCombo = new ReaSonusComboInput(
            m_ctx,
            assets,
            i18n->t("mapping", "edit.select.label-uninvert.label"),
            "edit-select-label-invert",
            invert_labels,
            &select_uninvert_label
        );

        FaderLabelInvertCombo = new ReaSonusComboInput(
            m_ctx,
            assets,
            i18n->t("mapping", "edit.fader.label-uninvert.label"),
            "edit-fader-label-invert",
            invert_labels,
            &fader_uninvert_label
        );

        PluginTypeCombo = new ReaSonusComboInput(
            m_ctx,
            assets,
            "",
            "plugin-ype-select",
            plugin_types,
            &newly_selected_plugin_type
        );

        SetPluginFolders();
    }

    ~CSurf_FP_8_PluginMappingPage() override = default;

    void RenderMappingListPlugin(const int index, const std::string &plugin_name) {
        using namespace std::placeholders; // for `_1, _2 etc`

        ReaSonusPluginSelectable(
            m_ctx,
            assets,
            ExtractPluginNameFromFile(plugin_name),
            ExtractPluginTypeFromFile(plugin_name),
            index,
            &selected_plugin,
            &hovered_plugin,
            std::bind(&CSurf_FP_8_PluginMappingPage::RenderPluginListContextMenuItems, this, _1),
            std::bind(&CSurf_FP_8_PluginMappingPage::HandleRemoveMappingClick, this, _1),
            std::bind(&CSurf_FP_8_PluginMappingPage::HandlePluginListItemClick, this, _1)
        );
    }

    void RenderMappingListDeveloper(const int index) {
        bool opened = false;

        if (index != selected_developer || selected_developer == -1) {
            ImGui::SetNextItemOpen(m_ctx, false);
        } else {
            ImGui::SetNextItemOpen(m_ctx, true);
            opened = true;
        }

        UiStyledElements::PushReaSonusTreeNodeStyle(m_ctx, selected_developer == index);
        // When opened we want to use reduced/custom spacing
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 0.0, opened ? 0.0 : 8.0);
        if (ImGui::TreeNode(
            m_ctx, developers.at(index).c_str(),
            ImGui::TreeNodeFlags_CollapsingHeader
        )) {
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
            selected_developer = index;
            const std::vector<std::string> developer_plugins = plugins.at(index);

            for (int j = 0; j < static_cast<int>(developer_plugins.size()); j++) {
                RenderMappingListPlugin(j, developer_plugins.at(j));
            }

            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
            ImGui::Dummy(m_ctx, 0, 0);
        }

        ImGui::PopStyleVar(m_ctx, 1);
        UiStyledElements::PopReaSonusTreeNodeStyle(m_ctx);
    }

    void RenderMappingsList() {
        double space_x;
        double space_y;

        if (ImGui::BeginChild(m_ctx, "mapping_lists", 280.0, 0.0)) {
            ImGui::Text(m_ctx, i18n->t("mapping", "list.label").c_str());

            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
            ImGui::SameLine(m_ctx);
            ImGui::SetCursorPosX(m_ctx, space_x - 18);

            UiStyledElements::PushReaSonusIconButtonStyle(m_ctx, assets, 16);
            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);

            if (ImGui::SmallButton(m_ctx, std::string(1, show_add_plugin_mapping ? IconRemove : IconAdd).c_str())) {
                show_add_plugin_mapping = !show_add_plugin_mapping;
            }

            if (ImGui::IsItemHovered(m_ctx)) {
                ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
            }

            ReaSonusSimpleTooltip(
                m_ctx, assets,
                show_add_plugin_mapping
                    ? i18n->t("mapping", "add.button.tooltip.close")
                    : i18n->t("mapping", "add.button.tooltip.open"),
                "add-mapping"
            );
            ImGui::PopStyleVar(m_ctx);
            UiStyledElements::PopReaSonusIconButtonStyle(m_ctx);

            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

            UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
            if (ImGui::BeginChild(
                m_ctx,
                "mapping_lists_content", 0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle
            )) {
                for (int i = 0; i < static_cast<int>(developers.size()); i++) {
                    RenderMappingListDeveloper(i);
                }

                UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderChannelsList() {
        using namespace std::placeholders; // for `_1, _2 etc`

        double space_x;
        double space_y;
        changed_items = 0;
        bool has_style_var = false;
        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 22, 12);

        if (ImGui::BeginChild(m_ctx, "##channel-list", 0.0, 30)) {
            ReaSonusIconButton(
                m_ctx,
                assets,
                IconArrowLeft,
                "previous-button",
                selected_channel == 0,
                ButtonThemeDefault,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandlePreviousClick, this));

            for (int i = channel_offset; i < min(max_items + channel_offset, nb_channels); i++) {
                has_style_var = false;
                if (i > channel_offset) {
                    has_style_var = true;
                    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 4, 0);
                }

                ImGui::SameLine(m_ctx);
                const bool dirty = IsGroupDirty(i);

                if (dirty) {
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
                    std::bind(&CSurf_FP_8_PluginMappingPage::HandleGroupDrop, this, _1, _2));

                if (has_style_var) {
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

    void RenderInformationBar() {
        double space_x;
        double space_y;

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, 54.0,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

            if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, space_y, ImGui::ChildFlags_AutoResizeY)) {
                ImGui::PushFont(m_ctx, assets->GetMainFontBold(), FontSizeDefault);
                ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 8);

                ImGui::Text(m_ctx, i18n->t("mapping", "edit.message.changed", std::to_string(changed_items),
                                           std::to_string(nb_channels)).c_str());

                ImGui::PopStyleColor(m_ctx);
                ImGui::PopFont(m_ctx);
                ImGui::EndChild(m_ctx);
            }

            ImGui::SameLine(m_ctx);

            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + space_x - 144);

            ReaSonusGoToInput(m_ctx, assets, &selected_channel, nb_channels);
            ReaSonusSimpleTooltip(m_ctx, assets, "Go to the group with a given number",
                                  "plugin-mapping-tooltip-undo-group");

            ImGui::SameLine(m_ctx);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconUndo,
                "mapping-reset-group",
                ButtonThemeAccent,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleResetChannel, this)
            );
            ReaSonusSimpleTooltip(
                m_ctx,
                assets,
                i18n->t("mapping", "tooltip.infobar.button.undo"),
                "plugin-mapping-tooltip-undo-group"
            );

            ImGui::SameLine(m_ctx);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconAdd,
                "mapping-add",
                ButtonThemeAccent,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleAddChannelAfterSelected, this)
            );
            ReaSonusSimpleTooltip(
                m_ctx,
                assets,
                i18n->t("mapping", "tooltip.infobar.button.add"),
                "plugin-mapping-tooltip-add-group"
            );

            ImGui::SameLine(m_ctx);

            ReaSonusIconButton(
                m_ctx,
                assets,
                IconDelete,
                "mapping-delete",
                ButtonThemeAccent,
                std::bind(&CSurf_FP_8_PluginMappingPage::HandleDeleteChannel, this)
            );
            ReaSonusSimpleTooltip(
                m_ctx,
                assets,
                i18n->t("mapping", "tooltip.infobar.button.delete"),
                "plugin-mapping-tooltip-delete-group"
            );

            ImGui::EndChild(m_ctx);
        }
        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
    }

    void RenderMappingSelect(double height) {
        using namespace std::placeholders; // for `_1, _2 etc`
        double space_x;
        double space_y;

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, height,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.select.label"), true);
            if (!param_names.empty()) {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                SelectParamList->Render(
                    std::bind(&CSurf_FP_8_PluginMappingPage::RenderParamListContextMenu, this, _1),
                    space_x * 0.7
                );

                ImGui::SameLine(m_ctx);

                ReaSonusIntInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.param.steps"),
                    &select_nb_steps,
                    0,
                    20,
                    0,
                    "%d");
            }

            if (ImGui::BeginChild(m_ctx, "filter_content_input", 0.0, 0.0, ImGui::ChildFlags_AutoResizeY)) {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusTextInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.param-name.label"),
                    &select_name,
                    i18n->t("mapping", "edit.select.param-name.placeholder"),
                    space_x * 0.7,
                    false
                );
                ImGui::SameLine(m_ctx);

                SelectLabelInvertCombo->Render();

                ImGui::EndChild(m_ctx);
            }
            UiStyledElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderMappingFader(double height) {
        using namespace std::placeholders; // for `_1, _2 etc`
        double space_x;
        double space_y;

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(m_ctx, "mapping_content_fader", 0.0, height,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.fader.label"), true);
            if (!param_names.empty()) {
                FaderParamList->Render(
                    std::bind(&CSurf_FP_8_PluginMappingPage::RenderParamListContextMenu, this, _1),
                    0.0
                );
            }
            if (ImGui::BeginChild(m_ctx, "filter_content_input", 0.0, 0.0, ImGui::ChildFlags_AutoResizeY)) {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusTextInput(
                    m_ctx,
                    i18n->t("mapping", "edit.fader.param-name.label"),
                    &fader_name,
                    i18n->t("mapping", "edit.fader.param-name.placeholder"),
                    space_x * 0.7,
                    false
                );

                ImGui::SameLine(m_ctx);

                FaderLabelInvertCombo->Render();

                ImGui::EndChild(m_ctx);
            }
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderCenteredText(const std::string &content, const IconFont icon) {
        double text_width;
        double text_height;
        double available_width;
        double available_height;

        ImGui::GetContentRegionAvail(m_ctx, &available_width, &available_height);
        ImGui::PushFont(m_ctx, assets->GetIconFont(), 84);
        ImGui::PushStyleColor(m_ctx, ImGui::Col_Text, UI_COLORS::Accent);
        ImGui::CalcTextSize(m_ctx, std::string(1, icon).c_str(), &text_width, &text_height);
        ImGui::SetCursorPosX(m_ctx, available_width / 2 - text_width / 2);
        ImGui::SetCursorPosY(m_ctx, available_height / 2 - text_height / 2 - 50);
        ImGui::Text(m_ctx, std::string(1, icon).c_str());
        ImGui::PopStyleColor(m_ctx);
        ImGui::PopFont(m_ctx);

        ImGui::PushFont(m_ctx, assets->GetMainFont(), FontSizeLarge);
        ImGui::CalcTextSize(m_ctx, content.c_str(), &text_width, &text_height);
        ImGui::SetCursorPosX(m_ctx, available_width / 2 - text_width / 2);
        ImGui::SetCursorPosY(m_ctx, available_height / 2 - text_height / 2 + 40);
        ImGui::Text(m_ctx, content.c_str());
        ImGui::PopFont(m_ctx);
    }

    void RenderPluginTypeSelect() {
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 22);
        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (
            ImGui::BeginChild(
                m_ctx, "type_select_content",
                0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
            )
        ) {
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.typeselect.label"), true);
            ImGui::TextWrapped(
                m_ctx,
                i18n->t("mapping", "edit.typeselect.description").c_str()
            );

            PluginTypeCombo->Render();

            ReaSonusButtonBar(
                m_ctx,
                assets,
                i18n->t("mapping", "edit.typeselect.button.save"),
                &save_selected_plugin_type,
                false,
                &cancel_selected_plugin_type,
                dummy,
                &dummy
            );

            UiStyledElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderDevelopersFilterList() {
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 22);
        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (
            ImGui::BeginChild(
                m_ctx, "type_select_content",
                0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
            )
        ) {
            DevelopersFilterForm->Render();

            ImGui::EndChild(m_ctx);
        }
        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
    }

    void PluginTypeSelectedCheck() {
        if (!save_selected_plugin_type) {
            return;
        }
        save_selected_plugin_type = false;

        // Create the current plugin path
        const std::string old_plugin_path = createPathName({
            plugin_folder_path,
            developers[selected_developer],
            plugins[selected_developer][selected_plugin]
        });
        const mINI::INIFile file(old_plugin_path);
        // Set the plugin type and store the data
        plugin_params["global"]["type"] = plugin_types[newly_selected_plugin_type];

        // File was moved after the list was refreshed?
        if (!file_exists(old_plugin_path.c_str())) {
            // TODO: Replace message box with something nicer?
            MessageBox(
                nullptr,
                "The file containing this plugin mapping was not found!\n\nHas the file been moved since the last time this list updated?",
                "ERROR: ReaSonus Native", 0);
            // This function can't continue because otherwise a ghost INI would be written alongside the old INI!
            return;
        }

        // Save plugin type to INI
        if (!file.write(plugin_params, true)) {
            return;
        }

        // Create the new filename and rename the file. Use name and type from INI data to prevent duplicate types in filename
        const std::vector<std::string> plugin_filename = {
            plugin_params["global"]["name"], plugin_params["global"]["type"], "ini"
        };
        const std::string new_plugin_path = createPathName({
            plugin_folder_path, developers[selected_developer], join(plugin_filename, ".")
        });

        std::rename(old_plugin_path.c_str(), new_plugin_path.c_str());

        // Now we can rebuild the plugins list and reset the previous selected plugin to trigger the plugin selection flow
        SetPluginFolders();
        previous_selected_plugin = -1;
    }

    void DeveloperCheck() {
        if (selected_developer != previous_selected_developer) {
            show_developer_filters = false;
            show_add_plugin_mapping = false;
            channel_offset = 0;
            selected_plugin_exists = false;

            if (DirtyCheck()) {
                selected_plugin = -1;
                previous_selected_plugin = -1;
                selected_channel = 0;
                changed_items = 0;
                previous_selected_channel = 0;
                previous_selected_developer = selected_developer;
            } else {
                selected_developer = previous_selected_developer;
            }
        }
    }

    /**
     * Check if the selected plugin has changed.
     */
    void PluginCheck(bool force = false) {
        if (selected_plugin == previous_selected_plugin && !force) {
            return;
        }

        show_developer_filters = false;

        show_add_plugin_mapping = false;

        // If data is dirty and user does not want to save or clear the changes, we do not change plugins
        if (!DirtyCheck() && !force) {
            selected_plugin = previous_selected_plugin;
            return;
        }

        channel_offset = 0;
        selected_channel = 0;

        if (SetPluginData()) {
            PopulateFields();
        } else {
            select_key = "";
            fader_key = "";
        }
        previous_selected_plugin = selected_plugin;
    }

    void ChannelCheck() {
        if (selected_channel != previous_selected_channel && selected_plugin_exists) {
            HandleChannelClick(selected_channel);
            previous_selected_channel = selected_channel;
        }
    }

    void ParamIndexCheck() {
        if (select_param_index != previous_select_param_index) {
            previous_select_param_index = select_param_index;
            select_nb_steps = std::get<2>(param_data[select_param_index]);
        }
    }

    void RenderMappingContent() {
        double space_x;
        double space_y;

        if (show_developer_filters) {
            RenderDevelopersFilterList();
        } else if (show_add_plugin_mapping) {
            AddPluginMappingForm->Render();
        } else if (selected_plugin > -1 && selected_plugin_exists && !selected_plugin_params_error) {
            ImGui::Text(
                m_ctx,
                selected_plugin < 0 || selected_plugin >= static_cast<int>(plugins[selected_developer].size())
                    ? "Groups"
                    : std::string(
                        developers[selected_developer] + " :: " + plugins[selected_developer][selected_plugin]).
                    c_str());
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);
            RenderInformationBar();
            RenderChannelsList();

            if (ImGui::BeginChild(m_ctx, "mapping_content_area", 0.0, 0.0)) {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                const double height = (space_y - 12) / 2;

                RenderMappingSelect(height);

                RenderMappingFader(height);

                UiStyledElements::PopReaSonusGroupStyle(m_ctx);

                ImGui::EndChild(m_ctx);
            }
        } else if (!selected_plugin_exists && selected_plugin > -1 && (
                       !selected_plugin_has_type || !selected_plugin_filename_has_type ||
                       selected_plugin_type_mismatch)) {
            RenderPluginTypeSelect();
        } else if (!selected_plugin_exists && selected_plugin > -1) {
            RenderCenteredText(i18n->t("mapping", "message.not-available"), IconRemove);
        } else if (selected_developer > -1 && selected_plugin == -1) {
            RenderCenteredText(i18n->t("mapping", "message.select-plugin"), IconInstrument);
        } else {
            RenderCenteredText(i18n->t("mapping", "message.select-developer"), IconUser);
        }
    }

    void Render() override {
        if (!render_started) {
            render_started = true;

            PluginTypeSelectedCheck();
            DeveloperCheck();
            PluginCheck();
            ChannelCheck();
            ParamIndexCheck();

            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
            if (ImGui::BeginChild(m_ctx, "main_mapping_content", 0.0, 0.0, ImGui::ChildFlags_None)) {
                RenderMappingsList();
                ImGui::SameLine(m_ctx);

                if (ImGui::BeginChild(m_ctx, "mapping_content", 0.0, 0.0)) {
                    RenderMappingContent();
                    ImGui::EndChild(m_ctx);
                }

                ImGui::PopStyleVar(m_ctx);
                ImGui::EndChild(m_ctx);
            }
            render_started = false;
        }
    }

    void Reset() override {
        if (selected_plugin < 0) {
            return;
        }

        channel_offset = 0;
        selected_channel = 0;
        SetPluginData();
        PopulateFields();
    }

    void Save() override {
        // We do not need a selected plugin to handle the developer filters
        if (show_developer_filters) {
            if (DevelopersFilterForm->Save()) {
                // Only re-get the params when we have a plugin selected
                if (selected_plugin > -1) {
                    GetPluginParams();
                }
                ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "action.save.filters.message"));
                show_developer_filters = false;
            }
            return;
        }

        if (selected_plugin < 0) {
            return;
        }

        const mINI::INIFile file(GetPluginSavePath());
        if (file.write(plugin_params, true)) {
            ReaSonus8ControlPanel::SetMessage(i18n->t("mapping", "action.save.message"));
        }
        SetPluginData();
        PopulateFields();
    }

    void SetPageProperty(const int type, const std::string value) override {
        switch (type) {
            case 0: {
                const auto developer_it = std::find(developers.begin(), developers.end(), value);
                if (developer_it != developers.end()) {
                    selected_developer = developer_it - developers.begin();
                    previous_selected_developer = selected_developer;
                }
                break;
            }

            case 1: {
                const std::vector<std::string> plugin_list = plugins.at(selected_developer);

                for (int i = 0; i < static_cast<int>(plugin_list.size()); i++) {
                    if (plugin_list.at(i) == value) {
                        selected_plugin = i;
                    }
                }

                previous_selected_plugin = selected_plugin;
                SetPluginData();
                PopulateFields();
                break;
            }
            default: ;
        }
    }

    void SetPageProperty(const int type, const int value) override {
        if (type == 0) {
            selected_channel = value;
            previous_selected_channel = value;
            PopulateFields();
        }
    }

    std::string GetPageStringProperty(const int type) override {
        switch (type) {
            case 0: {
                return developers.at(selected_developer);
            }

            case 1: {
                return plugins.at(selected_developer).at(selected_plugin);
            }
            default: {
                return "";
            }
        }
    }

    int GetPageProperty(const int type) override {
        if (type == 0) {
            return selected_channel;
        }

        return -1;
    }
};
