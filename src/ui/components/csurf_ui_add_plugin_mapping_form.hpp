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
    int filter_developer;
    int filter_plugin_type;

    std::vector<std::string> plugin_developers;
    std::vector<std::string> plugin_types;
    std::set<std::string> installed_developers;
    std::set<std::string> installed_plugin_types;
    std::set<std::string> plugin_categories;
    std::vector<PluginMeta> installed_plugins = PluginUtils::ExtractInstalledPluginMeta(
        installed_developers, installed_plugin_types, plugin_categories

    );
    ReaSonusComboInput *developers_combo;
    ReaSonusComboInput *plugin_type_combo;

protected:
    void CreateMappingFile(std::string plugin_name) const {
        mINI::INIStructure ini;
        std::string plugin_type = "No FX";
        const std::string full_name = PluginUtils::StripPluginNamePrefixes(plugin_name.c_str());
        const std::vector<std::string> plugin_name_parts = split(plugin_name, PREFIX_SEPARATOR);
        if (plugin_name_parts.size() > 1) {
            plugin_type = toLowerCase(plugin_name_parts.at(0));
        }
        const std::string name = PluginUtils::ExtractPluginName(plugin_name);
        const std::string developer_name = PluginUtils::ExtractDeveloperName(plugin_name);
        std::string fileName = PluginUtils::GetReaSonusPluginPath(developer_name, name, plugin_type, true);

        const mINI::INIFile file(fileName);
        if (!file.read(ini)) {
            ini["Global"];
            ini["Global"]["origName"] = full_name;
            ini["Global"]["name"] = name;
            ini["Global"]["type"] = plugin_type;
            ini["Global"]["developer"] = developer_name;

            if (file.generate(ini, true)) {
                PluginUtils::UpdatePluginMappingCacheFile(name);
            }
        }
    }

public:
    ReaSonusAddPluginMappingForm(
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets
    ) : m_ctx(m_ctx), assets(assets) {
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

    bool Save() const {
        // if (PluginUtils::SetFilterListByDevel2oper(developers[selected_developer], filter_item_list)) {
        //     // Changes saved. Now we have to rebuild all the cache
        //     return PluginUtils::UpdatePluginMappingCacheFileByDeveloper(developers[selected_developer]);
        // }
        return false;
    }

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
                i18n->t("mapping", "add.table.header.plugin").c_str(),
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
                    ImGui::TableNextRow(m_ctx);
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::Text(m_ctx, installed_plugin.GetShortName().c_str());
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::Text(m_ctx, installed_plugin.GetDeveloper().c_str());
                    ImGui::TableNextColumn(m_ctx);
                    ImGui::GetCursorScreenPos(m_ctx, &pos_screen_x, &pos_screen_y);
                    ReaSonusPluginTypeButton(
                        m_ctx,
                        assets,
                        pos_screen_x + 4,
                        pos_screen_y - 3,
                        installed_plugin.GetPluginType()
                    );
                    ImGui::TableNextColumn(m_ctx);

                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 2);
                    UiStyledElements::PushReaSonusIconButtonStyle(m_ctx, assets, 16);
                    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 0, 0);
                    if (ImGui::Button(
                        m_ctx, (std::string(1, IconAdd) + "##" + installed_plugin.GetFullName()).c_str(), 20, 20)) {
                        CreateMappingFile(installed_plugin.GetFullName());
                    }
                    if (ImGui::IsItemHovered(m_ctx)) {
                        ImGui::SetMouseCursor(m_ctx, ImGui::MouseCursor_Hand);
                    }
                    ReaSonusSimpleTooltip(
                        m_ctx,
                        assets,
                        i18n->t("mapping", "add.table.button.add", installed_plugin.GetShortName()),
                        "add-mapping"
                    );

                    UiStyledElements::PopReaSonusIconButtonStyle(m_ctx);

                    ImGui::PopStyleVar(m_ctx);
                }
            }

            ImGui::EndTable(m_ctx);
        }
        ImGui::PopStyleVar(m_ctx);
    }
};

#endif
