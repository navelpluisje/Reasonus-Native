#include "../ui/csurf_ui_page_content.hpp"
#include <string.h>
#include <reaper_imgui_functions.h>
#include "../ui/csurf_ui_listbox.hpp"
#include "../ui/csurf_ui_text_input.hpp"
#include "../ui/csurf_ui_checkbox.hpp"
#include "../ui/csurf_ui_images.h"

class CSurf_FP_8_CustomFilterstPage : public CSurf_UI_PageContent
{
protected:
    ImGui_Image *icon_add;
    ImGui_Image *icon_remove_list_item;

    std::string device;
    int selected_filter = 0;
    int previous_selected_filter = -1;
    bool drag_and_drop = false;

    std::vector<std::string> filter_keys;
    std::vector<std::string> filter_labels;

    bool filter_dirty;
    std::string new_filter_text;

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

    void createFilter()
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));

        std::string newKey = GenerateUniqueKey("filter_");
        ini["filters"][ini["filters"]["nb-filters"]] = newKey;
        ini["filters"]["nb-filters"] = std::to_string(stoi(ini["filters"]["nb-filters"]) + 1);
        ini[newKey];
        ini[newKey]["name"] = "New Filter";
        ini[newKey]["text"] = "";
        ini[newKey]["sibblings"] = "0";
        ini[newKey]["parents"] = "0";
        ini[newKey]["children"] = "0";
        ini[newKey]["top-level"] = "0";
        ini[newKey]["match-multiple"] = "0";
        file.write(ini, true);

        // PopulateFiltersList(hwndDlg, stoi(ini["filters"]["nb-filters"]) - 1);
    }

    void PopulateFilter()
    {
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
        std::string filter_key = filter_keys[previous_selected_filter];
        std::string filter_text_string = join(filter_text, ",");
        // strcpy(filter_name, ini[filter_key]["name"].c_str());
        // Add text, name chacks as well
        filter_dirty =
            filter_name.compare(ini[filter_key]["name"]) != 0 ||
            filter_text_string.compare(ini[filter_key]["text"]) ||
            filter_siblings != (ini[filter_key]["sibblings"] == "1") ||
            filter_parents != (ini[filter_key]["parents"] == "1") ||
            filter_children != (ini[filter_key]["children"] == "1") ||
            filter_top_level != (ini[filter_key]["top-level"] == "1") ||
            filter_match_multiple != (ini[filter_key]["match-multiple"] == "1");
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

    void render() override
    {
        double space_x, space_y;

        if (selected_filter != previous_selected_filter)
        {
            IsFilterDirty();
            if (filter_dirty)
            {
                int res = MB("Do you want to save these first?", "Unsaved changes", 4);
                if (res == 6)
                {
                    save();
                }
            }
            selected_filter_text = -1;
            PopulateFilter();
            previous_selected_filter = selected_filter;
        }

        ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_ItemSpacing, 12.0, 12.0);
        if (ImGui::BeginChild(m_ctx, "main_filter_content", 0.0, 0.0, ImGui::ChildFlags_None))
        {
            if (ImGui::BeginChild(m_ctx, "filter_lists", 200.0, 0.0))
            {
                UiElements::PushReaSonusListBoxStyle(m_ctx);
                if (ImGui::BeginListBox(m_ctx, "##filters", 0.0, 0.0))
                {
                    for (int i = 0; i < (int)filter_labels.size(); i++)
                    {
                        bool selected = selected_filter == i;
                        if (ImGui::Selectable(m_ctx, filter_labels[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap))
                        {
                            selected_filter = i;
                        }

                        /**
                         * Implement dragand drop for the list items
                         */
                        if (ImGui::IsItemActive(m_ctx) && !ImGui::IsAnyItemHovered(m_ctx))
                        {
                            drag_and_drop = true;
                            ImGui::GetMouseDragDelta(m_ctx, &space_x, &space_y, ImGui::MouseButton_Left);
                            int next_index = i + (space_y < 0 ? -1 : 1);
                            if (next_index >= 0 and next_index < (int)filter_labels.size())
                            {
                                std::string previous_label = filter_labels[i];
                                std::string previous_key = filter_keys[i];
                                filter_labels[i] = filter_labels[next_index];
                                filter_keys[i] = filter_keys[next_index];
                                filter_labels[next_index] = previous_label;
                                filter_keys[next_index] = previous_key;
                                ImGui::ResetMouseDragDelta(m_ctx, ImGui::MouseButton_Left);
                                for (int i = 0; i < (int)filter_keys.size(); i++)
                                {
                                    ini["filters"][std::to_string(i)] = filter_keys[i];
                                }
                                selected_filter = next_index;
                                previous_selected_filter = next_index;
                                PopulateFilter();
                            }
                        }

                        if (selected)
                        {
                            ImGui::SameLine(m_ctx);
                            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                            ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + space_x - 38);
                            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 3);
                            ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4.0, 4.0);
                            if (ImGui::ImageButton(m_ctx, std::string("Remove-filter##" + std::to_string(i)).c_str(), icon_remove_list_item, 12, 12))
                            {
                                ini.remove(filter_keys.at(i));
                                filter_keys.erase(filter_keys.begin() + i);
                                filter_labels.erase(filter_labels.begin() + i);
                                ini["filters"]["nb-filters"] = std::to_string(filter_keys.size());
                                SetFiltersKeys();
                                selected_filter = selected_filter == 0 ? selected_filter : selected_filter - 1;
                                previous_selected_filter = selected_filter;
                                PopulateFilter();
                            }
                            ImGui::PopStyleVar(m_ctx);
                        }
                    }
                    ImGui::EndListBox(m_ctx);
                    UiElements::PopReaSonusListBoxStyle(m_ctx);
                }
                ImGui::EndChild(m_ctx);
            }
            ImGui::SameLine(m_ctx);
            if (ImGui::BeginChild(m_ctx, "filter_content", 0.0, 0.0))
            {
                ImGui::Text(m_ctx, "New or Edit filter");
                ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 4);
                UiElements::PushReaSonusGroupStyle(m_ctx);

                if (ImGui::BeginChild(m_ctx, "filter_content", 0.0, 0.0, ImGui::ChildFlags_FrameStyle))
                {
                    ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                    double column_width = (space_x - 12) / 2;

                    if (ImGui::BeginChild(m_ctx, "filter_content_left", column_width, 0.0, ImGui::ChildFlags_None))
                    {
                        ReaSonusTextInput(m_ctx, "Filter name", &filter_name);

                        if (ImGui::BeginChild(m_ctx, "filter_text_content", 0.0, 0.0, ImGui::ChildFlags_None | ImGui::ChildFlags_AutoResizeY))
                        {
                            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                            ReaSonusTextInput(m_ctx, "Filter text", &new_filter_text, space_x - 38);
                            ImGui::SameLine(m_ctx);

                            UiElements::PushReaSonusIconButtonStyle(m_ctx);
                            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 21);
                            if (ImGui::ImageButton(m_ctx, "add-filter-text", icon_add, 20, 20))
                            {
                                if (new_filter_text.compare("") != 0)
                                {
                                    filter_text.push_back(new_filter_text);
                                    new_filter_text = "";
                                }
                            }
                            UiElements::PopReaSonusIconButtonStyle(m_ctx);
                            ImGui::EndChild(m_ctx);
                        }

                        UiElements::PushReaSonusListBoxStyle(m_ctx);
                        if (ImGui::BeginListBox(m_ctx, "##filter-text-list", 0.0, 124.0))
                        {
                            for (int i = 0; i < (int)filter_text.size(); i++)
                            {
                                bool selected = selected_filter_text == i;
                                if (ImGui::Selectable(m_ctx, filter_text[i].c_str(), &selected, ImGui::SelectableFlags_AllowOverlap))
                                {
                                    selected_filter_text = selected_filter_text == i ? -1 : i;
                                }
                                if (selected)
                                {
                                    ImGui::SameLine(m_ctx);
                                    ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
                                    ImGui::SetCursorPosX(m_ctx, ImGui::GetCursorPosX(m_ctx) + space_x - 18);
                                    ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 3);
                                    ImGui::PushStyleVar(m_ctx, ImGui::StyleVar_FramePadding, 4.0, 4.0);
                                    if (ImGui::ImageButton(m_ctx, std::string("Remove##" + std::to_string(i)).c_str(), icon_remove_list_item, 12, 12))
                                    {
                                        filter_text.erase(filter_text.begin() + i);
                                    }
                                    ImGui::PopStyleVar(m_ctx);
                                }
                            }
                            ImGui::EndListBox(m_ctx);
                            UiElements::PopReaSonusListBoxStyle(m_ctx);
                        }
                        ImGui::EndChild(m_ctx);
                    }
                    ImGui::SameLine(m_ctx);
                    if (ImGui::BeginChild(m_ctx, "filter_content_right", column_width, 0.0, ImGui::ChildFlags_None))
                    {
                        ImGui::Text(m_ctx, "Filter options");
                        ReaSonusCheckBox(m_ctx, "Show track Siblings", &filter_siblings);
                        ReaSonusCheckBox(m_ctx, "Show track parents", &filter_parents);
                        ReaSonusCheckBox(m_ctx, "Show track children", &filter_children);
                        ReaSonusCheckBox(m_ctx, "Show top level tracks only", &filter_top_level);
                        ReaSonusCheckBox(m_ctx, "Match multiple", &filter_match_multiple);
                        ImGui::EndChild(m_ctx);

                        ImGui::EndChild(m_ctx);
                    }
                    UiElements::PopReaSonusGroupStyle(m_ctx);
                    ImGui::EndChild(m_ctx);
                }
            }
            ImGui::PopStyleVar(m_ctx);
            ImGui::EndChild(m_ctx);
        }
    }

    void reset() override
    {
        readAndCreateIni(ini, FP_8);
        SetFiltersKeys();
        PopulateFilter();
    }

    void save() override
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        readAndCreateIni(ini, FP_8);

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
            SetFiltersKeys();
            MB("Chages saved with success", "Woohoo", 0);
        };
    }
};
