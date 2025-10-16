#ifndef I18N_LOCALES_ENUS_HPP_
#define I18N_LOCALES_ENUS_HPP_

#include <string>
#include <map>

std::map<std::string, std::string> enUS = {
    // Global Control Panel
    {"control-panel.menu.functions", "Function Keys"},
    {"control-panel.menu.filters", "Custom Filters"},
    {"control-panel.menu.plugin", "Plugin Mapping"},
    {"control-panel.menu.settings", "Settings"},
    {"control-panel.menu.about", "About ReaSonus"},
    {"control-panel.button.save", "Save"},
    {"control-panel.button.cancel", "Cancel"},
    // Functions Page
    {"functions.item.label", "Function"},
    {"functions.item.no-group", "No Group"},
    {"functions.item.button.tooltip", "Open the actionlist to select the\naction for this function key."},
    // Filters Page
    {"filters.list.label", "Global filters"},
    {"filters.list.input.label", "Filter name"},
    {"filters.list.input.placeholder", "Enter a filter name"},
    {"filters.content.title.new", "New filter"},
    {"filters.content.title.edit", "Edit filter"},
    {"filters.content.name.label", "Filter name"},
    {"filters.content.name.placeholder", "Only 10 characters will be displayed"},
    {"filters.content.text.label", "Filter text"},
    {"filters.content.text.placeholder", "Enter a filter text"},
    {"filters.content.option.title", "Filter options"},
    {"filters.content.option.sibling", "Show track siblings"},
    {"filters.content.option.parents", "Show track parents"},
    {"filters.content.option.children", "Show track children"},
    {"filters.content.option.top-level", "Show top level tracks only"},
    {"filters.content.option.multiple", "Match multiple"},
    {"filters.content.popup.save.title", "Woohoo!"},
    {"filters.content.popup.save.message", "Changes saved with success"},
    {"filters.content.popup.unsaved.title", "Unsaved changes"},
    {"filters.content.popup.unsaved.message", "Do you want to save these first?"},

};

#endif
