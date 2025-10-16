#ifndef I18N_LOCALES_DEDE_HPP_
#define I18N_LOCALES_DEDE_HPP_

#include <string>
#include <map>

std::map<std::string, std::string> deDE = {
    // Global Control Panel
    {"control-panel.menu.functions", "Funktionstasten"},
    {"control-panel.menu.filters", "Benutzerfilter"},
    {"control-panel.menu.plugin", "Plugin-Zuordnung"},
    {"control-panel.menu.settings", "Einstellungen"},
    {"control-panel.menu.about", "Über ReaSonus"},
    {"control-panel.button.save", "Speichern"},
    {"control-panel.button.cancel", "Abbrechen"},
    // Functions Page
    {"functions.item.label", "Funktion"},
    {"functions.item.no-group", "Keine Gruppe"},
    {"functions.item.button.tooltip", "Öffnen Sie die Aktionsliste, um die Aktion für diese Funktionstaste auszuwählen."},
    // Filters Page
    {"filters.list.label", "Globale Filter"},
    {"filters.list.input.label", "Filtername"},
    {"filters.list.input.placeholder", "Geben Sie einen Filternamen ein."},
    {"filters.content.title.new", "Neuer Filter"},
    {"filters.content.title.edit", "Filter bearbeiten"},
    {"filters.content.name.label", "Filtername"},
    {"filters.content.name.placeholder", "Es werden nur 10 Zeichen angezeigt."},
    {"filters.content.text.label", "Text filtern"},
    {"filters.content.text.placeholder", "Geben Sie einen Filtertext ein"},
    {"filters.content.option.title", "Filteroptionen"},
    {"filters.content.option.sibling", "Spur-Geschwister anzeigen"},
    {"filters.content.option.parents", "Spur-Eltern anzeigen"},
    {"filters.content.option.children", "Spur-Kinder anzeigen"},
    {"filters.content.option.top-level", "Nur Spuren der obersten Ebene anzeigen"},
    {"filters.content.option.multiple", "Mehrfach übereinstimmen"},
    {"filters.content.popup.save.title", "Woohoo!"},
    {"filters.content.popup.save.message", "Änderungen erfolgreich gespeichert"},
    {"filters.content.popup.unsaved.title", "Nicht gespeicherte Änderungen"},
    {"filters.content.popup.unsaved.message", "Möchten Sie diese zuerst speichern?"},
};

#endif
