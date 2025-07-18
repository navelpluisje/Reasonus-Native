#ifndef CSURF_FP_UI_ASSETS_H_
#define CSURF_FP_UI_ASSETS_H_

#include <reaper_imgui_functions.h>
#include "csurf_ui_images.h"

static ImGui_Font *main_font = ImGui::CreateFont("System", 13);
static ImGui_Font *main_font_bold = ImGui::CreateFont("System", 13, ImGui::FontFlags_Bold);
static ImGui_Font *menu_font = ImGui::CreateFont("System", 15, ImGui::FontFlags_Bold);
static ImGui_Image *logo = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(reasonus_logo), sizeof(reasonus_logo));
static ImGui_Image *icon_settings = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_settings), sizeof(img_icon_settings));
static ImGui_Image *icon_function_actions = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_function_actions), sizeof(img_icon_function_actions));
static ImGui_Image *icon_custom_filters = ImGui::CreateImageFromMem(reinterpret_cast<const char *>(img_icon_custom_filters), sizeof(img_icon_custom_filters));

#endif
