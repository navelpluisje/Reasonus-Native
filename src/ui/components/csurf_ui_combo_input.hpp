#ifndef CSURF_FP_UI_COMBO_INPUT_H_
#define CSURF_FP_UI_COMBO_INPUT_H_

#include <reaper_imgui_functions.h>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include "../../shared/csurf_utils.hpp"
#include "csurf_ui_list_selectable.hpp"
#include "../csurf_ui_styles_elements.hpp"

class ReaSonusComboInput {
    ImGui_Context *m_ctx;
    CSurf_UI_Assets *assets;
    const std::string label;
    const std::string id;
    const std::vector<std::string> &list;
    int *selected_item;
    int hovered_item = -1;
    int active_item = -1;
    double width = 0.0;

public:
    ReaSonusComboInput(
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets,
        std::string label,
        std::string idx,
        const std::vector<std::string> &list,
        int *selected_item,
        const double width = 0.0
    ) : m_ctx(m_ctx), assets(assets), label(std::move(label)), id(std::move(idx)), list(list),
        selected_item(selected_item), width(width) {
    }

    ~ReaSonusComboInput() = default;

    void Render() {
        double space_x;
        double space_y;

        UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
        if (ImGui::BeginChild(
            m_ctx,
            ("container" + id).c_str(),
            width,
            0.0,
            ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY
        )) {
            ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);
            if (width == 0.0) {
                width = space_x;
            }

            if (!label.empty() && label.rfind("##", 0) != 0) {
                ImGui::Text(m_ctx, label.c_str());
            }

            UiStyledElements::PushReaSonusComboStyle(m_ctx);
            ImGui::SetNextItemWidth(m_ctx, width);
            if (ImGui::BeginCombo(m_ctx, ("##" + id).c_str(), list[*selected_item].c_str())) {
                UiStyledElements::PushReaSonusListBoxStyle(m_ctx);
                for (int i = 0; i < static_cast<int>(list.size()); i++) {
                    ReaSonusListSelectable(
                        m_ctx,
                        assets,
                        list[i],
                        i,
                        selected_item,
                        &hovered_item,
                        &active_item,
                        nullptr,
                        ImGui::CloseCurrentPopup,
                        nullptr,
                        nullptr
                    );
                }
                UiStyledElements::PopReaSonusListBoxStyle(m_ctx);
                ImGui::EndCombo(m_ctx);
            }
            UiStyledElements::PopReaSonusComboStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
        UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
    }
};

class ReaSonusComboInputRow {
    ImGui_Context *m_ctx;
    CSurf_UI_Assets *assets;
    const std::string label;
    const std::string id;
    const std::vector<std::string> &list;
    int *selected_item;
    int hovered_item = -1;
    int active_item = -1;
    double label_width = 0.0;

public:
    ReaSonusComboInputRow(
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets,
        const std::string &label,
        const std::string &id,
        const std::vector<std::string> &list,
        int *selected_item,
        const double label_width = 0.0
    ) : m_ctx(m_ctx), assets(assets), label(label), id(id), list(list), selected_item(selected_item),
        label_width(label_width) {
    }

    ~ReaSonusComboInputRow() = default;

    void Render() {
        double space_x;
        double space_y;
        double padding_x;
        double padding_y;
        const double start_pos = ImGui::GetCursorPosX(m_ctx);

        ImGui::GetStyleVar(m_ctx, ImGui::StyleVar_FramePadding, &padding_x, &padding_y);
        ImGui::GetContentRegionAvail(m_ctx, &space_x, &space_y);

        UiStyledElements::PushReaSonusFieldGroupStyle(m_ctx);
        if (ImGui::BeginChild(m_ctx, ("container-" + id).c_str(), 0.0, 0.0,
                              ImGui::ChildFlags_FrameStyle | ImGui::ChildFlags_AutoResizeY)) {
            const double combo_width = space_x - label_width - padding_x;

            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 8);
            ImGui::Text(m_ctx, label.c_str());

            ImGui::SameLine(m_ctx);

            ImGui::SetCursorPosX(m_ctx, start_pos + label_width);
            ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) - 8);
            ImGui::SetNextItemWidth(m_ctx, combo_width);

            UiStyledElements::PushReaSonusComboStyle(m_ctx);
            if (ImGui::BeginCombo(m_ctx, ("##" + id).c_str(), list[*selected_item].c_str())) {
                UiStyledElements::PushReaSonusListBoxStyle(m_ctx);
                for (int i = 0; i < static_cast<int>(list.size()); i++) {
                    ReaSonusListSelectable(
                        m_ctx,
                        assets,
                        list[i],
                        i,
                        selected_item,
                        &hovered_item,
                        &active_item,
                        nullptr,
                        ImGui::CloseCurrentPopup,
                        nullptr,
                        nullptr
                    );
                }
                UiStyledElements::PopReaSonusListBoxStyle(m_ctx);
                ImGui::EndCombo(m_ctx);
            }
            UiStyledElements::PopReaSonusComboStyle(m_ctx);
            ImGui::EndChild(m_ctx);
        }
        UiStyledElements::PopReaSonusFieldGroupStyle(m_ctx);
    }

    ReaSonusComboInputRow &operator=(int _cpp_par_);
};

class ReaSonusInfoComboInputRow {
    ImGui_Context *m_ctx;
    CSurf_UI_Assets *assets;
    const std::string label;
    const std::string id;
    const std::vector<std::string> &list;
    int *selected_item;
    const std::string &tooltip;
    ReaSonusComboInput *combo_input;

public:
    ReaSonusInfoComboInputRow(
        ImGui_Context *m_ctx,
        CSurf_UI_Assets *assets,
        const std::string &label,
        const std::string &id,
        const std::vector<std::string> &list,
        int *selected_item,
        const std::string &tooltip
    ) : m_ctx(m_ctx), assets(assets), label(label), id(id), list(list), selected_item(selected_item), tooltip(tooltip) {
        combo_input = new ReaSonusComboInput(
            m_ctx,
            assets,
            label,
            id,
            list,
            selected_item,
            -20.0
        );
    }

    ~ReaSonusInfoComboInputRow() = default;

    void Render() {
        const double x_pos = ImGui::GetCursorPosX(m_ctx);
        const double y_pos = ImGui::GetCursorPosY(m_ctx);
        const std::string combo_id = std::to_string(x_pos) + "-" + std::to_string(y_pos);

        combo_input->Render();
        ImGui::SameLine(m_ctx);
        ReaSonusTooltip(m_ctx, assets, tooltip, combo_id, -20, 26);
        ImGui::SetCursorPosY(m_ctx, ImGui::GetCursorPosY(m_ctx) + 4);
        ImGui::Dummy(m_ctx, 0, 0);
    }
};

#endif
