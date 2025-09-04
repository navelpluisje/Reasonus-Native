#include "../ui/csurf_ui_page_content.hpp"
#include <string.h>
#include <reaper_imgui_functions.h>
#include "../ui/csurf_ui_colors.hpp"
#include "../ui/csurf_ui_text_input.hpp"
#include "../ui/csurf_ui_checkbox.hpp"
#include "../ui/csurf_ui_images.h"
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
    bool filter_siblings;
    bool filter_parents;
    bool filter_children;
    bool filter_top_level;
    bool filter_match_multiple;

    void SetFiltersKeys()
    {
        filter_keys.clear();
        filter_labels.clear();
        int nbFilters = stoi(ini["filters"]["nb-filters"]);

        for (int i = 0; i < nbFilters; i++)
        {
            filter_keys.push_back(ini["filters"][std::to_string(i)]);
            filter_labels.push_back(ini[ini["filters"][std::to_string(i)]]["name"]);
        }
    }

    void SetFilterText()
    {
        std::string filter_key = ini["filters"][std::to_string(selected_filter)];
        std::string filter_text_string = ini[filter_key]["text"];
        filter_text = split(filter_text_string, ",");
    }

    void CreateFilter()
    {
        std::string newKey = GenerateUniqueKey("filter_");
        ini["filters"][ini["filters"]["nb-filters"]] = newKey;
        ini["filters"]["nb-filters"] = std::to_string(stoi(ini["filters"]["nb-filters"]) + 1);
        ini[newKey];
        ini[newKey]["name"] = new_filter_name;
        ini[newKey]["text"] = "";
        ini[newKey]["sibblings"] = "0";
        ini[newKey]["parents"] = "0";
        ini[newKey]["children"] = "0";
        ini[newKey]["top-level"] = "0";
        ini[newKey]["match-multiple"] = "0";

        SetFiltersKeys();
        selected_filter = filter_keys.size() - 1;
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
            filter_siblings = false;
            filter_parents = false;
            filter_children = false;
            filter_top_level = false;
            filter_match_multiple = false;
            return;
        }
        std::string filter_key = filter_keys[selected_filter];
        SetFilterText();
        filter_name = ini[filter_key]["name"];
        filter_siblings = ini[filter_key]["sibblings"] == "1";
        filter_parents = ini[filter_key]["parents"] == "1";
        filter_children = ini[filter_key]["children"] == "1";
        filter_top_level = ini[filter_key]["top-level"] == "1";
        filter_match_multiple = ini[filter_key]["match-multiple"] == "1";
    }

    void IsFilterDirty()
    {
        if (previous_selected_filter < 0)
        {
            filter_dirty = false;
            return;
        }
        std::string filter_key = filter_keys[previous_selected_filter];
        std::string filter_text_string = join(filter_text, ",");

        filter_dirty =
            filter_name.compare(ini[filter_key]["name"]) != 0 ||
            filter_text_string.compare(ini[filter_key]["text"]) ||
            filter_siblings != (ini[filter_key]["sibblings"] == "1") ||
            filter_parents != (ini[filter_key]["parents"] == "1") ||
            filter_children != (ini[filter_key]["children"] == "1") ||
            filter_top_level != (ini[filter_key]["top-level"] == "1") ||
            filter_match_multiple != (ini[filter_key]["match-multiple"] == "1");
    }

    void DirtyCheck()
    {
        IsFilterDirty();
        if (filter_dirty)
        {
            int res = MB("Do you want to save these first?", "Unsaved changes", 4);
            if (res == 6)
            {
                Save();
            }
        }
    }

    void HandleAddFilter()
    {
        DirtyCheck();
        CreateFilter();
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
        ini.remove(filter_keys.at(index));
        filter_keys.erase(filter_keys.begin() + index);
        filter_labels.erase(filter_labels.begin() + index);
        ini["filters"]["nb-filters"] = std::to_string(filter_keys.size());
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
        for (int i = 0; i < (int)filter_keys.size(); i++)
        {
            ini["filters"][std::to_string(i)] = filter_keys[i];
        }
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
    CSurf_FP_8_CustomFilterstPage(ImGui_Context *m_ctx) : CSurf_UI_PageContent(m_ctx, FP_8)
    {
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
                ImGui::Text(m_ctx, "Global filters");
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "filter_lists_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle, ImGui::ChildFlags_AutoResizeY))
                {
                    ReaSonusActionInputText(m_ctx, "Filter name", &new_filter_name, "Enter a filter name", icon_add, std::bind(&CSurf_FP_8_CustomFilterstPage::HandleAddFilter, this));
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
                ImGui::Text(m_ctx, edit_new_filter ? "New filter" : "Edit filter");
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);

                UiElements::PushReaSonusGroupStyle(m_ctx);
                if (ImGui::BeginChild(m_ctx, "filter_content_edit", 0.0, 276.0, ImGui::ChildFlags_FrameStyle))
                {
                    ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                    double column_width = (space_x - 12) / 2;

                    if (ImGui::BeginChild(m_ctx, "filter_content_left", column_width, 0.0, 0.0, ImGui::ChildFlags_AutoResizeY))
                    {
                        ReaSonusTextInput(m_ctx, "Filter name", &filter_name, "Only 10 characters will be displayed");
                        ReaSonusActionInputText(m_ctx, "Filter text", &new_filter_text, "Enter a filter text", icon_add, std::bind(&CSurf_FP_8_CustomFilterstPage::HandleAddFilterText, this));
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
                        ImGui::Text(m_ctx, "Filter options");
                        ReaSonusCheckBox(m_ctx, "Show track Siblings", &filter_siblings);
                        ReaSonusCheckBox(m_ctx, "Show track parents", &filter_parents);
                        ReaSonusCheckBox(m_ctx, "Show track children", &filter_children);
                        ReaSonusCheckBox(m_ctx, "Show top level tracks only", &filter_top_level);
                        ReaSonusCheckBox(m_ctx, "Match multiple", &filter_match_multiple);
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
        readAndCreateIni(ini, FP_8);
        SetFiltersKeys();
        PopulateFilter();
    }

    void Save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));

        int selected = selected_filter;
        if (selected_filter != previous_selected_filter)
        {
            selected = previous_selected_filter;
        }
        std::string filter_key = filter_keys[selected];

        ini[filter_key]["name"] = filter_name;
        ini[filter_key]["text"] = join(filter_text, ",");
        ini[filter_key]["sibblings"] = filter_siblings ? "1" : "0";
        ini[filter_key]["parents"] = filter_parents ? "1" : "0";
        ini[filter_key]["children"] = filter_children ? "1" : "0";
        ini[filter_key]["top-level"] = filter_top_level ? "1" : "0";
        ini[filter_key]["match-multiple"] = filter_match_multiple ? "1" : "0";

        for (int i = 0; i < (int)filter_keys.size(); i++)
        {
            ini["filters"][std::to_string(i)] = filter_keys[i];
        }

        if (file.write(ini, true))
        {
            edit_new_filter = false;
            Reset();
            MB("Changes saved with success", "Woohoo", 0);
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
