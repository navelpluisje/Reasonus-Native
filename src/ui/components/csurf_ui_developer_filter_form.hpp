#ifndef CSURF_FP_UI_DEVELOPER_FILTER_FORM_H_
#define CSURF_FP_UI_DEVELOPER_FILTER_FORM_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <utility>
#include <vector>

#include "csurf_ui_action_text_input.hpp"
#include "csurf_ui_list_box.hpp"
#include "csurf_ui_page_title.hpp"
#include "../../shared/csurf_plugin_utils.hpp"

class ReaSonusDeveloperFilterForm {
    ImGui_Context *m_ctx;
    CSurf_UI_Assets *assets;
    I18n *i18n;

    std::vector<std::string> &developers;
    int selected_developer;
    std::string selected_filter_item_name;
    std::string previous_filter_item_name;
    int selected_filter_item = -1;
    int previous_selected_filter_item = -1;
    std::string new_item_name;
    std::vector<std::string> filter_item_list;
    ReaSonusExtendedListBox *filter_list;

protected:
    void GetSelectedDeveloperFilterList() {
        auto list = PluginUtils::GetFilterListByDeveloper(developers[selected_developer]);
        filter_item_list.clear();

        for (const auto &[key, item]: list) {
            filter_item_list.emplace_back(item);
        }
    }

    void AddItemToFilterList() {
        filter_item_list.emplace_back(new_item_name);
    }

    void RemoveItemFromFilterList(const int index) {
        const std::string removeable_item = filter_item_list.at(index);
        filter_item_list.erase(filter_item_list.begin() + index);

        selected_filter_item = selected_filter_item == 0 ? selected_filter_item : selected_filter_item - 1;
        previous_selected_filter_item = selected_filter_item;
    }

public:
    ReaSonusDeveloperFilterForm(
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets,
        std::vector<std::string> *developers
    ) : m_ctx(m_ctx), assets(assets), developers(*developers) {
        using namespace std::placeholders; // for `_1, _2 etc`

        filter_list = new ReaSonusExtendedListBox(
            m_ctx,
            assets,
            "filter-list",
            filter_item_list,
            &selected_filter_item,
            std::bind(&ReaSonusDeveloperFilterForm::RemoveItemFromFilterList, this, _1),
            false,
            nullptr
        );
    }

    ~ReaSonusDeveloperFilterForm() = default;

    void SetSelectedDeveloper(const int developer_index) {
        selected_developer = developer_index;
        i18n = I18n::GetInstance();
        GetSelectedDeveloperFilterList();
    }

    bool Save() const {
        if (PluginUtils::SetFilterListByDeveloper(developers[selected_developer], filter_item_list)) {
            // Changes saved. Now we have to rebuild all the cache
            return PluginUtils::UpdatePluginMappingCacheFileByDeveloper(developers[selected_developer]);
        }
        return false;
    }

    void Render() {
        using namespace std::placeholders; // for `_1, _2 etc`

        if (selected_filter_item != previous_selected_filter_item) {
            selected_filter_item_name = filter_item_list[selected_filter_item];
            previous_selected_filter_item = selected_filter_item;
        }

        if (selected_filter_item_name != previous_filter_item_name) {
            if (selected_filter_item > -1) {
                filter_item_list[selected_filter_item] = selected_filter_item_name;
                previous_filter_item_name = selected_filter_item_name;
            }
        }

        ReaSonusPageTitle(
            m_ctx,
            assets,
            i18n->t("mapping", "edit.developer-filter.label", developers[selected_developer]),
            true
        );
        
        if (ImGui::BeginChild(m_ctx, "list-filter-items", 200.0, 436.0, ImGui::ChildFlags_None)) {
            ReaSonusActionTextInput(
                m_ctx,
                assets,
                i18n->t("mapping", "edit.developer-filter.new.label"),
                &new_item_name,
                i18n->t("mapping", "edit.developer-filter.new.hint"),
                std::bind(&ReaSonusDeveloperFilterForm::AddItemToFilterList, this));

            filter_list->Render();

            ImGui::EndChild(m_ctx);
        }

        ImGui::SameLine(m_ctx);

        if (ImGui::BeginChild(
            m_ctx,
            "edit-filter-item",
            0.0,
            0.0,
            ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY
        )) {
            ReaSonusTextInput(
                m_ctx,
                i18n->t("mapping", "edit.developer-filter.edit.label"),
                &selected_filter_item_name,
                "",
                0.0,
                false
            );

            UiStyledElements::PushReaSonusGroupStyle(m_ctx, true);
            if (ImGui::BeginChild(
                m_ctx,
                "settings-display-line",
                0.0,
                0.0,
                ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
            )) {
                ImGui::PushTextWrapPos(m_ctx, 0.0);
                std::string explanation = i18n->t("mapping", "edit.developer-filter.edit.explanation");
                ImGui::Text(
                    m_ctx,
                    replaceAll(explanation, "\\n", "\n").c_str()
                );
                ImGui::PopTextWrapPos(m_ctx);

                ImGui::EndChild(m_ctx);
            }
            UiStyledElements::PopReaSonusGroupStyle(m_ctx);

            ImGui::EndChild(m_ctx);
        }
    }
};

#endif
