#ifndef CSURF_FP_UI_ADD_PLUGIN_MAPPING_FORM_H_
#define CSURF_FP_UI_ADD_PLUGIN_MAPPING_FORM_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <set>
#include <vector>

#include "../../shared/csurf_plugin_utils.hpp"

class ReaSonusAddPluginMappingForm {
    ImGui_Context *m_ctx;
    CSurf_UI_Assets *assets;
    I18n *i18n;

    double pos_screen_x = 0;
    double pos_screen_y = 0;

    std::string search_query;
    int filter_developer = 0;
    int filter_plugin_type = 0;

    const std::function<void(std::string developer, std::string plugin_name, std::string plugin_type)> save_callback;

    std::vector<std::string> plugin_developers;
    std::vector<std::string> plugin_types;
    std::set<std::string> installed_developers;
    std::set<std::string> installed_plugin_types;
    std::set<std::string> plugin_categories;
    std::vector<PluginMeta> installed_plugins = PluginUtils::ExtractInstalledPluginMeta(
        installed_developers,
        installed_plugin_types,
        plugin_categories
    );
    std::vector<PluginMeta> filtered_plugins;
    ReaSonusComboInput *developers_combo;
    ReaSonusComboInput *plugin_type_combo;

protected:
    void CreateMappingFile(std::string orig_plugin_name) const {
        mINI::INIStructure plugin_mapping_ini;

        PluginUtils::ReadCreatePluginMappingFileByOrigPluginName(orig_plugin_name, plugin_mapping_ini);

        if (save_callback != nullptr) {
            save_callback(
                plugin_mapping_ini["Global"]["developer"],
                plugin_mapping_ini["Global"]["name"],
                plugin_mapping_ini["Global"]["type"]
            );
        }
    }

public:
    ReaSonusAddPluginMappingForm(
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets,
        const std::function<void(std::string developer, std::string plugin_name, std::string plugin_type)> &
        save_callback
    ) : m_ctx(m_ctx), assets(assets), save_callback(save_callback) {
        i18n = I18n::GetInstance();
        installed_plugins = PluginUtils::ExtractInstalledPluginMeta(
            installed_developers, installed_plugin_types, plugin_categories
        );

        plugin_developers.assign(installed_developers.begin(), installed_developers.end());
        plugin_developers.insert(plugin_developers.begin(), i18n->t("mapping", "add.developer-filter.first"));

        plugin_types.assign(installed_plugin_types.begin(), installed_plugin_types.end());
        plugin_types.insert(plugin_types.begin(), i18n->t("mapping", "add.plugin-type-filter.first"));

        developers_combo = new ReaSonusComboInput(
            m_ctx,
            assets,
            "##developers-combo",
            "developers-combo",
            plugin_developers,
            &filter_developer,
            180.0
        );

        plugin_type_combo = new ReaSonusComboInput(
            m_ctx,
            assets,
            "##plugin-types-combo",
            "plugin-types-combo",
            plugin_types,
            &filter_plugin_type,
            0.0
        );
    }

    ~ReaSonusAddPluginMappingForm() = default;

    void RenderFilterBar() {
        double space_x;
        double space_y;

        static char text[255];              // NOLINT(*-avoid-c-arrays)
        strcpy(text, search_query.c_str()); // NOLINT(*-security.insecureAPI.strcpy)

        UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
        if (ImGui::BeginChild(
            m_ctx,
            "mapping_content_select",
            0.0,
            58.0,
            ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
        )) {
            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

            if (ImGui::BeginChild(m_ctx, "mapping_content_select-1", 0.0, space_y, ImGui::ChildFlags_AutoResizeY)) {
                UiStyledElements::PushReaSonusInputStyle(m_ctx);
                ImGui::SetNextItemWidth(m_ctx, 180.0);
                if (ImGui::InputTextWithHint(
                    m_ctx,
                    "##plugin-search-query",
                    i18n->t("mapping", "add.name-filter.placeholder").c_str(),
                    text,
                    sizeof(text)
                )) {
                    search_query = std::string(text);
                    ImGui::TextFilter_Set(assets->GetReaComboFilter(), text);
                }
                UiStyledElements::PopReaSonusInputStyle(m_ctx);

                ImGui::SameLine(m_ctx);

                developers_combo->Render();
                ImGui::SameLine(m_ctx);
                plugin_type_combo->Render();

                ImGui::EndChild(m_ctx);
            }

            ImGui::EndChild(m_ctx);
        }
        UiStyledElements::PopReaSonusGroupStyle(m_ctx);
    }

    void Render() {
        filtered_plugins.clear();
        for (const auto &installed_plugin: installed_plugins) {
            if (
                ImGui::TextFilter_PassFilter(
                    assets->GetReaComboFilter(),
                    installed_plugin.GetShortName().c_str()
                )
                && (
                    plugin_developers[filter_developer] == i18n->t("mapping", "add.developer-filter.first")
                    || plugin_developers[filter_developer] == installed_plugin.GetDeveloper()
                )
                && (
                    plugin_types[filter_plugin_type] == i18n->t("mapping", "add.plugin-type-filter.first")
                    || plugin_types[filter_plugin_type] == installed_plugin.GetPluginType()

                )
            ) {
                filtered_plugins.emplace_back(installed_plugin);
            }
        }

        RenderFilterBar();

        if (ImGui::IsWindowAppearing(m_ctx)) {
            ImGui::TextFilter_Clear(assets->GetReaComboFilter());
        }

        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_CellPadding, 4.0, 4.0);
        if (ImGui::BeginTable(
            m_ctx,
            "installed-plugins-table",
            4,
            ImGui::TableFlags_BordersV | ImGui::TableFlags_RowBg |
            ImGui::TableFlags_SizingStretchProp | ImGui::TableFlags_ScrollY | ImGui::TableFlags_BordersOuterH,
            0.0,
            0.0
        )) {
            ImGui::TableSetupColumn(
                m_ctx,
                (i18n->t("mapping", "add.table.header.plugin") + " :: (" + std::to_string(filtered_plugins.size()) + "/"
                 +
                 std::to_string(installed_plugins.size()) + ")").
                c_str(),
                ImGui::TableColumnFlags_WidthStretch\
            );
            ImGui::TableSetupColumn(
                m_ctx,
                i18n->t("mapping", "add.table.header.developer").c_str(),
                ImGui::TableColumnFlags_WidthFixed,
                130
            );
            ImGui::TableSetupColumn(m_ctx, "", ImGui::TableColumnFlags_WidthFixed, 28);
            ImGui::TableSetupColumn(m_ctx, "", ImGui::TableColumnFlags_WidthFixed, 20);
            ImGui::TableSetupScrollFreeze(m_ctx, 0, 1);
            ImGui::TableHeadersRow(m_ctx);

            for (const auto &filtered_plugin: filtered_plugins) {
                ImGui::TableNextRow(m_ctx);
                ImGui::TableNextColumn(m_ctx);
                ImGui::Text(m_ctx, filtered_plugin.GetShortName().c_str());
                ImGui::TableNextColumn(m_ctx);
                ImGui::Text(m_ctx, filtered_plugin.GetDeveloper().c_str());
                ImGui::TableNextColumn(m_ctx);
                ImGui::GetCursorScreenPos(m_ctx, &pos_screen_x, &pos_screen_y);
                ReaSonusPluginTypeButton(
                    m_ctx,
                    assets,
                    pos_screen_x + 4,
                    pos_screen_y - 3,
                    filtered_plugin.GetPluginType()
                );
                ImGui::TableNextColumn(m_ctx);

                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 2);
                UiStyledElements::PushReaSonusIconButtonStyle(m_ctx, assets, 16);
                ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
                if (ImGui::Button(
                    m_ctx, (std::string(1, IconAdd) + "##" + filtered_plugin.GetFullName()).c_str(), 20, 20
                )) {
                    CreateMappingFile(filtered_plugin.GetFullName());
                }
                if (ImGui::IsItemHovered(m_ctx)) {
                    ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
                }
                ReaSonusSimpleTooltip(
                    m_ctx,
                    assets,
                    i18n->t("mapping", "add.table.button.add", filtered_plugin.GetShortName()),
                    "add-mapping"
                );

                UiStyledElements::PopReaSonusIconButtonStyle(m_ctx);

                ImGui::PopStyleVar(m_ctx);
            }

            ImGui::EndTable(m_ctx);
        }
        ImGui::PopStyleVar(m_ctx);
    }
};

#endif
