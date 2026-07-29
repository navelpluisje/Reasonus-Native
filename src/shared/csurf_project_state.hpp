#ifndef CSURF_PROJECT_STATE_HPP_
#define CSURF_PROJECT_STATE_HPP_

#include <string>
#include <vector>
#include <mini/ini.h>
#include "../shared/csurf_utils.hpp"

class ProjectState {
    // Private Constructor
    explicit ProjectState();

    const std::string project_state_key = "REASONUS";
    const std::string string_separator = "%||%";

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
     * @param key
     * @param default_value
     * @return std::string
     */
    [[nodiscard]] std::string GetProjectState(const std::string &key, std::string default_value = "") const;

    /**
     * Get the number of filters
     * @return
     */
    int GetNumberOfFilters();

    /**
     * Store the ReaSonus project state.
     * @return true on success
     */
    bool StoreProjectState();

    /**
     * Reset all changes and set the current state with the previous one
     */
    void ReloadProjectState();

    /**
     * Add a new filter to the list of filters
     * @param filter_name The name for the new filter
     * @return
     */
    int AddNewFilter(const std::string &filter_name);

    /**
     * Get a list with all the filter keys
     * @return
     */
    std::vector<std::string> GetFilterKeys();

    /**
     * Update the filter by the given key with the given data
     * @param key The key of the filter to change
     * @param filter The filter data to update the filter with
     */
    void UpdateFilter(const std::string &key, const mINI::INIMap<std::string> &filter);

    /**
     * Callback function for sorting the filters in the filter list
     * @param keys
     */
    void UpdateFilterOrder(const std::vector<std::string> &keys);

    /**
     * Remove the filter with the given key
     * @param key The key of the filter to remove
     */
    void Removefilter(const std::string &key);

    /**
     * Get the key of a filter by the given filter index
     * @param index The index of the filter we want the key for
     * @return The filter key
     */
    std::string GetFilterKeyByIndex(int index);

    /**
     * Get a list with all the filter names. This is used to display the filter names in the filters list
     * @return
     */
    std::vector<std::string> GetFilterNames();

    /**
     * Get a list with all the colors used in the filters. This is used to display the filter color in the filters list
     * @return
     */
    std::vector<int> GetFilterColors();

    /**
     * Get a filter with the given key
     * @param key The filter key to get the data for
     * @return The filter
     */
    mINI::INIMap<std::string> GetFilter(const std::string &key);

    /**
     * Get a filter with the given index
     * @param index The filter index to get the data for
     * @return The filter
     */
    mINI::INIMap<std::string> GetFilter(int index);
};

#endif
