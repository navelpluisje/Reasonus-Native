#ifndef CSURF_PROJECT_STATE_HPP_
#define CSURF_PROJECT_STATE_HPP_

#include <string>
#include <vector>
#include <mini/ini.h>
#include "../shared/csurf_utils.hpp"

class ProjectState {
    // Private Constructor
    explicit ProjectState();

    const std::string project_state_key = "ReaSonus";

    mINI::INIStructure project_state;
    mINI::INIStructure previous_project_state;

    static ProjectState *instancePtr;

public:
    ProjectState(const ProjectState &obj) = delete;

    void LoadProjectState();

    static ProjectState *GetInstance() {
        if (instancePtr == nullptr) {
            instancePtr = new ProjectState();
        }
        return instancePtr;
    }

    void ReadProjectState(mINI::INIStructure &data) const;

    /**
     * @brief Set a Setting without saving to file
     *
     * @param key The key within the project state to write to
     * @param value The actual value to write
     */
    void SetProjectState(const std::string &key, const std::string &value) const;

    void SetProjectState(const std::string &key, bool value) const;

    void SetProjectState(const std::string &key, int value) const;

    /**
     * @brief Get the Setting vy its group and key
     *
     * @param group
     * @param key
     * @return std::string
     */
    std::string GetProjectState(const std::string &key, std::string default_value = "") const;

    int GetNumberOfFilters();

    bool StoreProjectState();

    void ReloadProjectState();

    /**
     * Filters related
     */
    int AddNewFilter(const std::string &filter_name);

    std::vector<std::string> GetFilterKeys();

    void UpdateFilter(const std::string &key, const mINI::INIMap<std::string> &filter);

    void UpdateFilterOrder(const std::vector<std::string> &keys);

    void Removefilter(const std::string &key);

    std::string GetFilterKeyByIndex(int index);

    std::vector<std::string> GetFilterNames();

    std::vector<int> GetFilterColors();

    mINI::INIMap<std::string> GetFilter(const std::string &key);

    mINI::INIMap<std::string> GetFilter(int index);
};

#endif
