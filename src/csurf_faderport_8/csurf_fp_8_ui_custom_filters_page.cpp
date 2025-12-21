#include "../ui/csurf_ui_page_content.hpp"
#include "../ui/csurf_ui_action_input_text.hpp"
#include "../ui/csurf_ui_list_box.hpp"
#include "../ui/csurf_ui_filter_preview.hpp"

class CSurf_FP_8_CustomFilterstPage : public CSurf_UI_PageContent
{
protected:
    ImGui_Image *icon_add;
    ImGui_Image *icon_remove_list_item;

    std::string device;
    int selected_filter = -1;
    int previous_selected_filter = -1;

    std::vector<std::string> filter_keys = {};
    std::vector<std::string> filter_labels = {};

    bool filter_dirty;
    std::string new_filter_text;
    std::string new_filter_name = "";
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

    I18n *i18n = I18n::GetInstance();
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_8);

    void SetFiltersKeys()
    {
        filter_keys.clear();
        filter_labels.clear();

        filter_keys = settings->GetFilterKeys();
        filter_labels = settings->GetFilterNames();
    }

    void CreateFilter(std::string filter_name)
    {
        selected_filter = settings->AddNewFilter(filter_name);
        SetFiltersKeys();
        previous_selected_filter = selected_filter;
        edit_new_filter = true;
        PopulateFilter();
    }

    void PopulateFilter()
    {
        if (filter_keys.size() == 0)
        {
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

        std::string filter_key = filter_keys[selected_filter];
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

    void IsFilterDirty()
    {
        if (previous_selected_filter < 0)
        {
            filter_dirty = false;
            return;
        }

        std::string filter_text_string = join(filter_text, ",");
        std::string filter_key = filter_keys[previous_selected_filter];
        mINI::INIMap filter = settings->GetFilter(filter_key);

        filter_dirty =
            filter_name.compare(filter["name"]) != 0 ||
            filter_text_string.compare(filter["text"]) ||
            filter_case_insensitive != (filter["case-insensitive"] == "1") ||
            filter_siblings != (filter["sibblings"] == "1") ||
            filter_parents != (filter["parents"] == "1") ||
            filter_children != (filter["children"] == "1") ||
            filter_top_level != (filter["top-level"] == "1") ||
            filter_match_multiple != (filter["match-multiple"] == "1");
    }

    void DirtyCheck()
    {
        IsFilterDirty();
        if (filter_dirty)
        {
            int res = MB(
                i18n->t("filters", "popup.unsaved.message").c_str(),
                i18n->t("filters", "popup.unsaved.title").c_str(),
                4);

            if (res == 6)
            {
                Save();
            }
        }
    }

    void HandleAddFilter()
    {
        DirtyCheck();
        CreateFilter(new_filter_name);
        new_filter_name = "";
    }

    void HandleAddFilterText()
    {
        filter_text.push_back(new_filter_text);
        new_filter_text = "";
    }

    void HandleRemoveFilterTextListItem(int index)
    {
        filter_text.erase(filter_text.begin() + index);
    }

    void HandleRemoveFilterListItem(int index)
    {
        std::string filter_key = filter_keys.at(index);
        filter_keys.erase(filter_keys.begin() + index);

        settings->Removefilter(filter_key);

        SetFiltersKeys();
        selected_filter = selected_filter == 0 ? selected_filter : selected_filter - 1;
        previous_selected_filter = selected_filter;
        edit_new_filter = false;
        PopulateFilter();
    }

    void HandleSortFilterList(int current_item, int next_item)
    {
        std::string previous_label = filter_labels[current_item];
        std::string previous_key = filter_keys[current_item];
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

    int GetOptionsValue()
    {
        int res = 0;
        res += (filter_siblings ? 1 : 0) << 0;
        res += (filter_parents ? 1 : 0) << 1;
        res += (filter_children ? 1 : 0) << 2;
        res += (filter_top_level ? 1 : 0) << 3;
        res += (filter_match_multiple ? 1 : 0) << 4;

        return res;
    }

public:
    CSurf_FP_8_CustomFilterstPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx)
    {
        settings = ReaSonusSettings::GetInstance(FP_8);

        icon_add = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_add), sizeof(img_icon_add));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_add));

        icon_remove_list_item = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_list_remove), sizeof(img_icon_list_remove));
        ImGui::Attach(m_ctx, reinterpret_cast<ImGui_Resource *>(icon_remove_list_item));

        SetFiltersKeys();
    };

    virtual ~CSurf_FP_8_CustomFilterstPage() {};

    void Render() override
    {
        using namespace std::placeholders; // for `_1, _2 etc`
        double space_x, space_y;

        if ((selected_filter == -1) && (filter_labels.size() > 0))
        {
            selected_filter = 0;
        }

        if (selected_filter != previous_selected_filter)
        {
            DirtyCheck();
            selected_filter_text = -1;
            edit_new_filter = false;
            PopulateFilter();
            previous_selected_filter = selected_filter;
        }

        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
        if (ImGui::BeginChild(m_ctx, "main_filter_content", 0.0, 0.0, ImGui::ChildFlags_None))
        {
            if (ImGui::BeginChild(m_ctx, "filter_lists", 240.0, 0.0))
            {
                ImGui::Text(m_ctx, i18n->t("filters", "list.label").c_str());
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "filter_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
                {
                    ReaSonusActionInputText(
                        m_ctx,
                        i18n->t("filters", "list.input.label").c_str(),
                        &new_filter_name,
                        i18n->t("filters", "list.input.placeholder").c_str(),
                        icon_add,
                        std::bind(&CSurf_FP_8_CustomFilterstPage::HandleAddFilter, this));

                    ReaSonusExtendedListBox(
                        m_ctx,
                        "filters",
                        filter_labels,
                        &selected_filter,
                        false,
                        icon_remove_list_item,
                        std::bind(&CSurf_FP_8_CustomFilterstPage::HandleRemoveFilterListItem, this, _1),
                        true,
                        std::bind(&CSurf_FP_8_CustomFilterstPage::HandleSortFilterList, this, _1, _2));

                    UiElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx);
                }

                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);

            if (ImGui::BeginChild(m_ctx, "filter_content", 0.0, 0.0))
            {
                ImGui::Text(m_ctx, edit_new_filter
                                       ? i18n->t("filters", "content.title.new").c_str()
                                       : i18n->t("filters", "content.title.edit").c_str());
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "filter_content_edit", 0.0, 276.0, ImGui::ChildFlags_FrameStyle))
                {
                    ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                    double column_width = (space_x - 12) / 2;

                    if (ImGui::BeginChild(m_ctx, "filter_content_left", column_width, 0.0, 0.0, ImGui::ChildFlags_AutoResizeY))
                    {
                        ReaSonusTextInput(
                            m_ctx,
                            i18n->t("filters", "content.name.label").c_str(),
                            &filter_name,
                            i18n->t("filters", "content.name.placeholder").c_str());

                        ReaSonusActionInputText(
                            m_ctx,
                            i18n->t("filters", "content.text.label").c_str(),
                            &new_filter_text,
                            i18n->t("filters", "content.text.placeholder").c_str(),
                            icon_add,
                            std::bind(&CSurf_FP_8_CustomFilterstPage::HandleAddFilterText, this));

                        ReaSonusExtendedListBox(
                            m_ctx,
                            "filter-text-list",
                            filter_text,
                            &selected_filter_text,
                            true,
                            icon_remove_list_item,
                            std::bind(&CSurf_FP_8_CustomFilterstPage::HandleRemoveFilterTextListItem, this, _1));

                        ImGui::EndChild(m_ctx);
                    }
                    ImGui::SameLine(m_ctx);

                    if (ImGui::BeginChild(m_ctx, "filter_content_right", column_width, 0.0, 0.0, ImGui::ChildFlags_AutoResizeY))
                    {
                        ImGui::Text(m_ctx, i18n->t("filters", "content.option.title").c_str());
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.case-insensitive"), &filter_case_insensitive);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.sibling"), &filter_siblings);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.parents"), &filter_parents);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.children"), &filter_children);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.top-level"), &filter_top_level);
                        ReaSonusCheckBox(m_ctx, i18n->t("filters", "content.option.multiple"), &filter_match_multiple);
                        ImGui::EndChild(m_ctx);
                    }

                    ImGui::EndChild(m_ctx);
                    UiElements::PopReaSonusGroupStyle(m_ctx);
                }

                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "filter_content_preview", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
                {
                    ReaSonusFilterPreview(m_ctx, GetOptionsValue());

                    ImGui::EndChild(m_ctx);
                    UiElements::PopReaSonusGroupStyle(m_ctx);
                }

                ImGui::EndChild(m_ctx);
            }

            ImGui::PopStyleVar(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void Reset() override
    {
        settings->UpdateSettings();
        SetFiltersKeys();
        PopulateFilter();
    }

    void Save() override
    {
        if (selected_filter < 0)
        {
            return;
        }

        int selected = selected_filter;
        if (selected_filter != previous_selected_filter)
        {
            selected = previous_selected_filter;
        }
        std::string filter_key = filter_keys[selected];
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

        if (settings->StoreSettings())
        {
            edit_new_filter = false;
            ReaSonus8ControlPanel::SetMessage(i18n->t("filters", "action.save.message"));
            Reset();
        };
    }

    void SetPageProperty(int type, int value) override
    {
        switch (type)
        {
        case 0:
            selected_filter = value;
            break;
        }
    }

    int GetPageProperty(int type) override
    {
        switch (type)
        {
        case 0:
            return selected_filter;
            break;
        }

        return -1;
    }
};
