#include "csurf_project_state.hpp"
#include <reaper_plugin_functions.h>
#include <utility>
#include "../shared/csurf_utils.hpp"

ProjectState::ProjectState() {
    ReadProjectState(project_state);
    // store the project_state to previous_project_state, to make a reset possible
    // as changes are written to the state instant (but not saved instant)
    previous_project_state = project_state;
}

void ProjectState::UpdateSettings() {
    ReadProjectState(project_state);
    previous_project_state = project_state;
}

void ProjectState::ReadProjectState(mINI::INIStructure &data) const {
    // Read all the available data from the project state.
    data["filters"];
    data["filters"]["nb-filters"] = GetProjectState("filters-nb-filters", "0");

    if (stoi(data["filters"]["nb-filters"]) > 0) {
        const std::vector<std::string> keys = split(GetProjectState("filters-keys", ""), ",");

        for (int i = 0; i < static_cast<int>(keys.size()); i++) {
            std::string const &filter_key = keys.at(i);

            data["filters"][std::to_string(i)] = filter_key;

            std::string const filter = GetProjectState(filter_key, "");
            if (!filter.empty()) {
                data[filter_key];

                const std::vector<std::string> filter_parts = split(filter, "$");
                for (const std::string &filter_part: filter_parts) {
                    const std::vector<std::string> filter_value = split(filter_part, "=");
                    data[filter_key][filter_value[0]] = filter_value[1];
                }
            }
        }
    }
}

void ProjectState::SetProjectState(const std::string &key, const std::string &value) const {
    SetProjExtState(nullptr, project_state_key.c_str(), key.c_str(), value.c_str());
}

void ProjectState::SetProjectState(const std::string &key, const bool value) const {
    SetProjExtState(nullptr, project_state_key.c_str(), key.c_str(), value ? "1" : "0");
}

void ProjectState::SetProjectState(const std::string &key, const int value) const {
    SetProjExtState(nullptr, project_state_key.c_str(), key.c_str(), std::to_string(value).c_str());
}

std::string ProjectState::GetProjectState(const std::string &key, std::string default_value) const {
    char buffer[256]; // NOLINT(*-avoid-c-arrays)
    if (GetProjExtState(nullptr, project_state_key.c_str(), key.c_str(), buffer, sizeof buffer) == 0) {
        return default_value;
    }

    std::string value = buffer;
    return value;
}

/**
 * Filters related
 */
int ProjectState::GetNumberOfFilters() {
    return stoi(project_state["filters"]["nb-filters"]);
}

std::string CreateProjectStateFilter(const mINI::INIMap<std::string> &filter) { // NOLINT(*-use-internal-linkage)
    std::string result;

    for (auto const &[key, value]: filter) {
        if (!result.empty()) {
            result += "$";
        }
        result += key + "=" + value;
    }

    return result;
}

bool ProjectState::StoreProjectState() {
    try {
        const std::vector<std::string> keys = GetFilterKeys();
        for (auto const &key: keys) {
            const auto filter = project_state[key];
            SetProjectState(key, CreateProjectStateFilter(filter));
        }
        SetProjectState("filters-keys", join(keys, ","));
        SetProjectState("filters-nb-filters", static_cast<int>(keys.size()));

        Main_SaveProject(nullptr, false);

        return true;
    } catch (...) {
        return false;
    }
}

void ProjectState::ReloadProjectState() {
    project_state = previous_project_state;
}

/**
 * @brief Create a new filter and return the index of the filter
 *
 * @return int The index of the newly created filter
 */
int ProjectState::AddNewFilter(const std::string &filter_name) {
    const std::string newKey = GenerateUniqueKey("filter_");
    // Need to check how to save this

    project_state["filters"][project_state["filters"]["nb-filters"]] = newKey;
    project_state["filters"]["nb-filters"] = std::to_string(GetNumberOfFilters() + 1);

    project_state[newKey];
    project_state[newKey]["name"] = filter_name;
    project_state[newKey]["text"] = "";
    project_state[newKey]["case-insensitive"] = "0";
    project_state[newKey]["sibblings"] = "0";
    project_state[newKey]["parents"] = "0";
    project_state[newKey]["children"] = "0";
    project_state[newKey]["top-level"] = "0";
    project_state[newKey]["match-multiple"] = "0";

    return GetNumberOfFilters() - 1;
}

std::vector<std::string> ProjectState::GetFilterKeys() {
    std::vector<std::string> keys = {};
    keys.reserve(GetNumberOfFilters());

    for (int i = 0; i < GetNumberOfFilters(); i++) {
        keys.push_back(project_state["filters"][std::to_string(i)]);
    }

    return keys;
}

void ProjectState::UpdateFilter(const std::string &key, const mINI::INIMap<std::string> &filter) {
    project_state[key] = filter;
}

void ProjectState::UpdateFilterOrder(const std::vector<std::string> &keys) {
    for (int i = 0; i < static_cast<int>(keys.size()); i++) {
        project_state["filters"][std::to_string(i)] = keys[i];
    }
    project_state["filters"]["nb-filters"] = std::to_string(keys.size());
}

void ProjectState::Removefilter(const std::string &key) {
    project_state.remove(key);

    // Check if the key is in the vector and get its position
    // If we have a value, erase it from the vector
    std::vector<std::string> keys = GetFilterKeys();
    const auto itterator = std::find(keys.begin(), keys.end(), key);
    if (itterator != keys.end()) {
        keys.erase(itterator);
    }

    // Clear all the filters and create the new list
    for (int i = 0; i < GetNumberOfFilters(); i++) {
        project_state["filters"].remove(std::to_string(i));
    }

    // With setting an empty value, the setting will get deleted
    SetProjectState(key, ""); // NOLINT(*-implicit-bool-conversion)
    UpdateFilterOrder(keys);
}

std::string ProjectState::GetFilterKeyByIndex(const int index) {
    return project_state["filters"][std::to_string(index)];
}

std::vector<std::string> ProjectState::GetFilterNames() {
    std::vector<std::string> names = {};
    names.reserve(GetNumberOfFilters());

    for (int i = 0; i < GetNumberOfFilters(); i++) {
        names.push_back(project_state[project_state["filters"][std::to_string(i)]]["name"]);
    }

    return names;
}

std::vector<int> ProjectState::GetFilterColors() {
    std::vector<int> colors = {};
    colors.reserve(GetNumberOfFilters());

    for (int i = 0; i < GetNumberOfFilters(); i++) {
        if (project_state[project_state["filters"][std::to_string(i)]].has("color")) {
            colors.push_back(stoi(project_state[project_state["filters"][std::to_string(i)]]["color"]));
        } else {
            colors.push_back(0x00ffffff);
        }
    }

    return colors;
}

mINI::INIMap<std::string> ProjectState::GetFilter(const std::string &key) {
    return project_state[key];
}

mINI::INIMap<std::string> ProjectState::GetFilter(const int index) {
    return GetFilter(GetFilterKeyByIndex(index));
}
