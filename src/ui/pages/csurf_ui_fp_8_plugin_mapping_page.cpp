#include <utility>

#include "../csurf_ui_page_content.hpp"
#include "../../shared/csurf_utils.hpp"
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
#include "../../i18n/i18n.hpp"
#include "../windows/csurf_ui_fp_8_control_panel.hpp"

using PluginParam = std::tuple<int, std::string, int>;

class CSurf_FP_8_PluginMappingPage : public CSurf_UI_PageContent // NOLINT(*-use-internal-linkage)
{
    mINI::INIStructure plugin_params;
    mINI::INIStructure previous_plugin_params;

    const int max_items = 13;
    int channel_offset = 0;
    int changed_items = 0;

    bool render_started = false;
    std::string plugin_folder_path = createPathName({std::string(GetResourcePath()), "ReaSonus", "Plugins"});
    std::vector<std::string> developers;
    std::vector<std::string> plugin_types = GetPluginTypes();
    int newly_selected_plugin_type = 0;
    bool save_selected_plugin_type = false;
    bool cancel_selected_plugin_type = false;
    std::string dummy;
    int selected_developer = -1;
    int previous_selected_developer = -1;

    std::vector<std::vector<std::string> > plugins;
    int selected_plugin = -1;
    int previous_selected_plugin = -1;
    bool selected_plugin_exists = false;
    bool selected_plugin_has_type = false;
    bool selected_plugin_filename_has_type = false;
    bool selected_plugin_type_mismatch = false;
    std::string selected_plugin_filename_type;

    int nb_channels = 0;
    int selected_channel = 0;
    int previous_selected_channel = 0;

    std::vector<PluginParam> paramIds;
    std::vector<std::string> params;
    std::string select_search_query;
    std::string fader_search_query;

    std::string select_key;
    std::string select_name;
    int select_nb_steps{};
    int select_param_index = -1;
    int previous_select_param_index = -1;
    int select_uninvert_label = 0;

    std::string fader_key;
    std::string fader_name;
    int fader_param_index{};
    int fader_uninvert_label = 0;

    std::vector<std::string> invert_labels = {"Inverted", "Not inverted"};

    std::vector<int> dirty_groups;

    bool plugin_dirty = false;

protected:
    void SetPluginFolders() {
        developers.clear();
        plugins.clear();
        bool has_next = true;
        int index = 0;

        while (has_next) {
            const char *name = EnumerateSubdirectories(plugin_folder_path.c_str(), index);
            
            if (name == nullptr) {
                has_next = false;
            } else {
                index++;
                const std::vector<std::string> splitted_name = split(name, ".");
                developers.emplace_back(name);
            }
        }
        std::sort(developers.begin(), developers.end());

        for (const auto &developer: developers) {
            plugins.push_back(SetDeveloperPlugins(developer));
        }
    }

    std::vector<std::string> SetDeveloperPlugins(std::string developer) {
        bool has_next = true;
        int index = 0;
        const std::string path = createPathName({plugin_folder_path, std::move(developer)});
        std::vector<std::string> developer_plugins;

        while (has_next) {
            const char *name = EnumerateFiles(path.c_str(), index);
            if (name == nullptr) {
                has_next = false;
            } else {
                index++;
                std::vector<std::string> splitted_name = split(std::string(name), ".");

                if (splitted_name[splitted_name.size() - 1] == "ini") {
                    developer_plugins.emplace_back(name);
                }
            }
        }

        std::sort(developer_plugins.begin(), developer_plugins.end());
        return developer_plugins;
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
        if (!PluginExists()) {
            selected_plugin_has_type = !plugin_params["global"]["type"].empty();
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

        GetPluginParams();

        return true;
    }

    bool PluginExists() {
        // Create a track, add the plugin and start reading the params
        InsertTrackAtIndex(0, false);
        MediaTrack *media_track = GetTrack(nullptr, 0);
        DAW::SetMixerTrackVisible(media_track, false);
        DAW::SetTCPTrackVisible(media_track, false);

        const std::string plugin_request_string = GetPluginRequestString(
            plugin_params["global"]["origname"],
            plugin_params["global"]["type"]
        );
        const int exist = TrackFX_AddByName(media_track, plugin_request_string.c_str(), false, -1);
        DeleteTrack(media_track);

        return exist > -1;
    }

    void GetPluginParams() {
        // Prepare the vectors
        params.clear();
        paramIds.clear();
        params.push_back(i18n->t("mapping", "edit.select.param.first-item"));
        paramIds.emplace_back(-1, i18n->t("mapping", "edit.select.param.first-item"), 0);

        // Create a track, add the plugin and start reading the params
        InsertTrackAtIndex(0, false);
        MediaTrack *media_track = GetTrack(nullptr, 0);

        // Placeholder until first iteration of plugin caching is implemented
        const std::string plugin_request_string = GetPluginRequestString(
            plugin_params["global"]["origname"],
            plugin_params["global"]["type"]
        );
        TrackFX_AddByName(media_track, plugin_request_string.c_str(), false, -1);

        for (int i = 0; i < TrackFX_GetNumParams(media_track, 0); i++) {
            const std::string param_name = DAW::GetTrackFxParamName(media_track, 0, i);

            if (IsWantedParam(std::string(param_name))) {
                const int steps = DAW::GetTrackFxParamNbSteps(media_track, 0, i);
                params.emplace_back(param_name);
                paramIds.emplace_back(i, std::string(param_name), steps);
            }
        }

        // We're done, now delete the newly added track
        DeleteTrack(media_track);
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
                paramIds.begin(),
                paramIds.end(),
                [param_id](const PluginParam &param) {
                    return param_id == std::get<0>(param);
                });

            if (iterator != paramIds.end()) {
                select_param_index = iterator - paramIds.begin();
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
                paramIds.begin(),
                paramIds.end(),
                [param_id](const PluginParam &param) {
                    return param_id == std::get<0>(param);
                });
            if (iterator != paramIds.end()) {
                fader_param_index = iterator - paramIds.begin();
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
            plugin_params[select_key]["param"] = std::to_string(std::get<0>(paramIds[select_param_index]));
            plugin_params[select_key]["uninvert-label"] = std::to_string(select_uninvert_label);
        }

        if (fader_param_index > 0) {
            plugin_params[fader_key]["name"] = fader_name;
            plugin_params[fader_key]["param"] = std::to_string(std::get<0>(paramIds[fader_param_index]));
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
            const int res = MB(i18n->t("mapping", "popup.unsaved.message").c_str(),
                               i18n->t("mapping", "popup.unsaved.title").c_str(), 3);
            if (res == 6) {
                ShowConsoleMsg("SAVE!1");
                Save();
            }
            return res != 2;
        }
        return true;
    }

    void HandlePreviousClick() {
        int step_size = 1;

        if (ImGui::IsKeyDown(m_ctx, ImGui::Key_LeftShift)) {
            step_size = 13;
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
        select_search_query.clear();
        fader_search_query.clear();
    }

    void HandleNextClick() {
        int step_size = 1;

        if (ImGui::IsKeyDown(m_ctx, ImGui::Key_LeftShift)) {
            step_size = 13;
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
        select_search_query.clear();
        fader_search_query.clear();
    }

    void HandleChannelClick(const int index) {
        selected_channel = index;
        previous_selected_channel = selected_channel;
        PopulateFields();

        channel_offset = minmax(0, index - 6, max(nb_channels - max_items, 0));

        select_search_query.clear();
        fader_search_query.clear();
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

    void handleGroupDrop(const int from, const int to) // NOLINT(*-identifier-length)
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

    static std::string formatPluginName(const std::string &plugin_name) {
        std::vector<std::string> splitted_plugin_name = split(plugin_name, ".");

        splitted_plugin_name.pop_back();

        if (splitted_plugin_name.size() > 1) {
            return splitted_plugin_name.at(0) + " (" + splitted_plugin_name.at(1) + ")";
        }

        return splitted_plugin_name.at(0);
    }

public:
    CSurf_FP_8_PluginMappingPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets) {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_8);

        SetPluginFolders();
    }

    ~CSurf_FP_8_PluginMappingPage() override = default;

    void RenderMappingListPlugin(const int index, const std::string &plugin_name, const int developer_index) {
        bool selected = selected_developer == developer_index && selected_plugin == index;

        if (ImGui::Selectable(m_ctx, formatPluginName(plugin_name).c_str(), &selected)) {
            selected_plugin = index;
        }
    }

    void RenderMappingListDeveloper(const int index) {
        if (index != selected_developer || selected_developer == -1) {
            ImGui::SetNextItemOpen(m_ctx, false);
        } else {
            ImGui::SetNextItemOpen(m_ctx, true);
        }

        UiStyledElements::PushReaSonusTreeNodeStyle(m_ctx, selected_developer == index);
        if (ImGui::TreeNode(m_ctx, developers.at(index).c_str(), ImGui::TreeNodeFlags_CollapsingHeader)) {
            selected_developer = index;
            const std::vector<std::string> developer_plugins = plugins.at(index);

            for (int j = 0; j < static_cast<int>(developer_plugins.size()); j++) {
                RenderMappingListPlugin(j, developer_plugins.at(j), index);
            }
        }
        UiStyledElements::PopReaSonusTreeNodeStyle(m_ctx);
    }

    void RenderMappingsList() {
        if (ImGui::BeginChild(m_ctx, "mapping_lists", 240.0, 0.0)) {
            ImGui::Text(m_ctx, i18n->t("mapping", "list.label").c_str());
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

            UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
            if (ImGui::BeginChild(m_ctx, "mapping_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle,
                                  ImGui::ChildFlags_AutoResizeY)) {
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
                    std::bind(&CSurf_FP_8_PluginMappingPage::handleGroupDrop, this, _1, _2));

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
        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
    }

    void RenderMappingSelect(double height) {
        double space_x;
        double space_y;

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(m_ctx, "mapping_content_select", 0.0, height,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.select.label"), true);
            if (!params.empty()) {
                ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

                ReaSonusSearchComboInput(
                    m_ctx,
                    assets,
                    i18n->t("mapping", "edit.select.param.label"),
                    params,
                    &select_param_index,
                    &select_search_query,
                    settings->ShouldClearParamInput(),
                    space_x * 0.7);

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

                ReaSonusComboInput(
                    m_ctx,
                    i18n->t("mapping", "edit.select.label-uninvert.label"),
                    invert_labels,
                    &select_uninvert_label);

                ImGui::EndChild(m_ctx);
            }
            UiStyledElements::PopReaSonusGroupStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void RenderMappingFader(double height) {
        double space_x;
        double space_y;

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(m_ctx, "mapping_content_fader", 0.0, height,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            ReaSonusPageTitle(m_ctx, assets, i18n->t("mapping", "edit.fader.label"), true);
            if (!params.empty()) {
                ReaSonusSearchComboInput(
                    m_ctx,
                    assets,
                    i18n->t("mapping", "edit.fader.param.label"),
                    params,
                    &fader_param_index,
                    &fader_search_query,
                    settings->ShouldClearParamInput(),
                    0.0);
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

            ReaSonusComboInput(
                m_ctx,
                "",
                plugin_types,
                &newly_selected_plugin_type);

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

    void PluginCheck() {
        if (selected_plugin != previous_selected_plugin) {
            channel_offset = 0;
            if (DirtyCheck()) {
                selected_channel = 0;

                if (SetPluginData()) {
                    PopulateFields();
                } else {
                    select_key = "";
                    fader_key = "";
                }
                previous_selected_plugin = selected_plugin;
            } else {
                selected_plugin = previous_selected_plugin;
            }
        }
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
            select_nb_steps = std::get<2>(paramIds[select_param_index]);
        }
    }

    void RenderMappingContent() {
        double space_x;
        double space_y;

        if (selected_plugin > -1 && selected_plugin_exists) {
            ImGui::Text(
                m_ctx, selected_plugin < 0 || selected_plugin >= static_cast<int>(plugins[selected_developer].size())
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
