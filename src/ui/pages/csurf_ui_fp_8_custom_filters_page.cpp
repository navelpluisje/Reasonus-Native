#include "../csurf_ui_page_content.hpp"
#include "../components/csurf_ui_action_text_input.hpp"
#include "../components/csurf_ui_list_box.hpp"
#include "../components/csurf_ui_checkbox.hpp"
#include "../components/csurf_ui_filter_preview.hpp"
#include "../windows/csurf_ui_fp_8_control_panel.hpp"

class CSurf_FP_8_CustomFiltersPage : public CSurf_UI_PageContent {
    std::string device;
    int selected_filter = -1;
    int previous_selected_filter = -1;

    std::vector<std::string> filter_keys;
    std::vector<std::string> filter_labels;

    bool filter_dirty;
    std::string new_filter_text;
    std::string new_filter_name;
    bool edit_new_filter = false;

    std::string filter_name;
    std::vector<std::string> filter_text;
    int selected_filter_text = -1;
    bool filter_case_insensitive;
    bool filter_siblings;
    bool filter_parents;
    bool filter_children;
    bool filter_top_level;
    bool filter_match_multiple;

protected:
    void SetFiltersKeys() {
        filter_keys.clear();
        filter_labels.clear();

        filter_keys = settings->GetFilterKeys();
        filter_labels = settings->GetFilterNames();
    }

    void CreateFilter(const std::string &filter_name) {
        selected_filter = settings->AddNewFilter(filter_name);
        SetFiltersKeys();
        previous_selected_filter = selected_filter;
        edit_new_filter = true;
        PopulateFilter();
    }

    void PopulateFilter() {
        if (filter_keys.empty()) {
            filter_name = "";
            filter_text.clear();
            filter_case_insensitive = false;
            filter_siblings = false;
            filter_parents = false;
            filter_children = false;
            filter_top_level = false;
            filter_match_multiple = false;
            return;
        }

        const std::string filter_key = filter_keys[selected_filter];
        mINI::INIMap filter = settings->GetFilter(filter_key);

        filter_text = split(filter["text"], ",");
        filter_name = filter["name"];
        filter_case_insensitive = filter["case-insensitive"] == "1";
        filter_siblings = filter["sibblings"] == "1";
        filter_parents = filter["parents"] == "1";
        filter_children = filter["children"] == "1";
        filter_top_level = filter["top-level"] == "1";
        filter_match_multiple = filter["match-multiple"] == "1";
    }

    void IsFilterDirty() {
        if (previous_selected_filter < 0) {
            filter_dirty = false;
            return;
        }

        const std::string filter_text_string = join(filter_text, ",");
        const std::string filter_key = filter_keys[previous_selected_filter];
        mINI::INIMap filter = settings->GetFilter(filter_key);

        filter_dirty =
                filter_name != filter["name"] ||
                filter_text_string != filter["text"] ||
                filter_case_insensitive != (filter["case-insensitive"] == "1") ||
                filter_siblings != (filter["sibblings"] == "1") ||
                filter_parents != (filter["parents"] == "1") ||
                filter_children != (filter["children"] == "1") ||
                filter_top_level != (filter["top-level"] == "1") ||
                filter_match_multiple != (filter["match-multiple"] == "1");
    }

    void DirtyCheck() {
        IsFilterDirty();
        if (filter_dirty) {
            const int res = MB(
                i18n->t("filters", "popup.unsaved.message").c_str(),
                i18n->t("filters", "popup.unsaved.title").c_str(),
                4);

            if (res == 6) {
                Save();
            }
        }
    }

    void HandleAddFilter() {
        DirtyCheck();
        CreateFilter(new_filter_name);
        new_filter_name = "";
    }

    void HandleAddFilterText() {
        filter_text.push_back(new_filter_text);
        new_filter_text = "";
    }

    void HandleRemoveFilterTextListItem(const int index) {
        filter_text.erase(filter_text.begin() + index);
    }

    void HandleRemoveFilterListItem(const int index) {
        const std::string filter_key = filter_keys.at(index);
        filter_keys.erase(filter_keys.begin() + index);

        settings->Removefilter(filter_key);

        SetFiltersKeys();
        selected_filter = selected_filter == 0 ? selected_filter : selected_filter - 1;
        previous_selected_filter = selected_filter;
        edit_new_filter = false;
        PopulateFilter();
    }

    void HandleSortFilterList(const int current_item, const int next_item) {
        const std::string previous_label = filter_labels[current_item];
        const std::string previous_key = filter_keys[current_item];
        filter_labels[current_item] = filter_labels[next_item];
        filter_keys[current_item] = filter_keys[next_item];
        filter_labels[next_item] = previous_label;
        filter_keys[next_item] = previous_key;
        ImGui::ResetMouseDragDelta(m_ctx, ImGui::MouseButton_Left);

        settings->UpdateFilterOrder(filter_keys);
        selected_filter = next_item;
        previous_selected_filter = next_item;
        PopulateFilter();
    }

    [[nodiscard]] int GetOptionsValue() const {
        int res = 0;

        res += (filter_siblings ? 1 : 0) << 0;
        res += (filter_parents ? 1 : 0) << 1;
        res += (filter_children ? 1 : 0) << 2;
        res += (filter_top_level ? 1 : 0) << 3;
        res += (filter_match_multiple ? 1 : 0) << 4;

        return res;
    }

public:
    CSurf_FP_8_CustomFiltersPage(ImGui_Context *m_ctx, CSurf_UI_Assets *assets) : CSurf_UI_PageContent(m_ctx, assets) {
        i18n = I18n::GetInstance();
        settings = ReaSonusSettings::GetInstance(FP_8);

        SetFiltersKeys();
    };

    ~CSurf_FP_8_CustomFiltersPage() override = default;

    void Render() override {
        using namespace std::placeholders; // for `_1, _2 etc`
        double space_x;
        double space_y;

        if (selected_filter == -1 && !filter_labels.empty()) {
            selected_filter = 0;
        }

        if (selected_filter != previous_selected_filter) {
            DirtyCheck();
            selected_filter_text = -1;
            edit_new_filter = false;
            PopulateFilter();
            previous_selected_filter = selected_filter;
        }

        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
        if (ImGui::BeginChild(m_ctx, "main_filter_content", 0.0, 0.0, ImGui::ChildFlags_None)) {
            if (ImGui::BeginChild(m_ctx, "filter_lists", 240.0, 0.0)) {
                ImGui::Text(m_ctx, i18n->t("filters", "list.label").c_str());
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(m_ctx, "filter_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle,
                                      ImGui::ChildFlags_AutoResizeY)) {
                    ReaSonusActionTextInput(
                        m_ctx,
                        assets,
                        i18n->t("filters", "list.input.label"),
                        &new_filter_name,
                        i18n->t("filters", "list.input.placeholder"),
                        std::bind(&CSurf_FP_8_CustomFiltersPage::HandleAddFilter, this));

                    ReaSonusExtendedListBox(
                        m_ctx,
                        assets,
                        "filters",
                        filter_labels,
                        &selected_filter,
                        false,
                        std::bind(&CSurf_FP_8_CustomFiltersPage::HandleRemoveFilterListItem, this, _1),
                        true,
                        std::bind(&CSurf_FP_8_CustomFiltersPage::HandleSortFilterList, this, _1, _2));

                    UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx);
                }

                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);

            if (ImGui::BeginChild(m_ctx, "filter_content", 0.0, 0.0)) {
                ImGui::Text(m_ctx, edit_new_filter
                                       ? i18n->t("filters", "content.title.new").c_str()
                                       : i18n->t("filters", "content.title.edit").c_str());
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(m_ctx, "filter_content_edit", 0.0, 276.0, ImGui::ChildFlags_FrameStyle)) {
                    ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                    const double column_width = (space_x - 12) / 2;

                    if (ImGui::BeginChild(m_ctx, "filter_content_left", column_width, 0.0, 0.0,
                                          ImGui::ChildFlags_AutoResizeY)) {
                        ReaSonusTextInput(
                            m_ctx,
                            i18n->t("filters", "content.name.label"),
                            &filter_name,
                            i18n->t("filters", "content.name.placeholder"),
                            0.0,
                            false);

                        ReaSonusActionTextInput(
                            m_ctx,
                            assets,
                            i18n->t("filters", "content.text.label"),
                            &new_filter_text,
                            i18n->t("filters", "content.text.placeholder"),
                            std::bind(&CSurf_FP_8_CustomFiltersPage::HandleAddFilterText, this));

                        ReaSonusExtendedListBox(
                            m_ctx,
                            assets,
                            "filter-text-list",
                            filter_text,
                            &selected_filter_text,
                            true,
                            std::bind(&CSurf_FP_8_CustomFiltersPage::HandleRemoveFilterTextListItem, this, _1),
                            false,
                            nullptr
                        );

                        ImGui::EndChild(m_ctx);
                    }
                    ImGui::SameLine(m_ctx);

                    if (ImGui::BeginChild(m_ctx, "filter_content_right", column_width, 0.0, 0.0,
                                          ImGui::ChildFlags_AutoResizeY)) {
                        ImGui::Text(m_ctx, i18n->t("filters", "content.option.title").c_str());
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.case-insensitive"),
                                         &filter_case_insensitive);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.sibling"), &filter_siblings);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.parents"), &filter_parents);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.children"), &filter_children);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.top-level"), &filter_top_level);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.multiple"), &filter_match_multiple);
                        ImGui::EndChild(m_ctx);
                    }

                    ImGui::EndChild(m_ctx);
                    UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                }

                UiStyledElements::PushReaSonusGroupStyle(m_ctx, false);
                if (ImGui::BeginChild(m_ctx, "filter_content_preview", 0.0, 0.0, ImGui::ChildFlags_FrameStyle)) {
                    ReaSonusFilterPreview(m_ctx, GetOptionsValue());

                    ImGui::EndChild(m_ctx);
                    UiStyledElements::PopReaSonusGroupStyle(m_ctx);
                }

                ImGui::EndChild(m_ctx);
            }

            ImGui::PopStyleVar(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Reset() override {
        settings->UpdateSettings();
        SetFiltersKeys();
        PopulateFilter();
    }

    void Save() override {
        if (selected_filter < 0) {
            return;
        }

        int selected = selected_filter;
        if (selected_filter != previous_selected_filter) {
            selected = previous_selected_filter;
        }
        const std::string filter_key = filter_keys[selected];
        mINI::INIMap filter = settings->GetFilter(filter_key);

        filter["name"] = filter_name;
        filter["text"] = join(filter_text, ",");
        filter["case-insensitive"] = filter_case_insensitive ? "1" : "0";
        filter["sibblings"] = filter_siblings ? "1" : "0";
        filter["parents"] = filter_parents ? "1" : "0";
        filter["children"] = filter_children ? "1" : "0";
        filter["top-level"] = filter_top_level ? "1" : "0";
        filter["match-multiple"] = filter_match_multiple ? "1" : "0";

        settings->UpdateFilter(filter_key, filter);
        settings->UpdateFilterOrder(filter_keys);

        if (settings->StoreSettings()) {
            edit_new_filter = false;
            ReaSonus8ControlPanel::SetMessage(i18n->t("filters", "action.save.message"));
            Reset();
        }
    }

    void SetPageProperty(const int type, const int value) override {
        if (type == 0) {
            selected_filter = value;
        }
    }

    int GetPageProperty(const int type) override {
        if (type == 0) {
            return selected_filter;
        }

        return -1;
    }
};
