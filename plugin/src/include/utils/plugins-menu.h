#ifndef PLUGINS_MENU_H
#define PLUGINS_MENU_H

#include <functional>
#include <string>
#include <vector>
#include <XPLMMenus.h>

// The plugin's submenu under X-Plane's Plugins menu. Created and destroyed with the plugin context,
// so the menu disappears when the plugin is disabled and nothing is left behind.
// Every method must be called on the main thread.
class PluginsMenu {
    public:
        explicit PluginsMenu(const std::string &title);
        ~PluginsMenu();

        PluginsMenu(const PluginsMenu &) = delete;
        PluginsMenu &operator=(const PluginsMenu &) = delete;

        // Returns the item index, used by the setters below
        int addItem(const std::string &name, std::function<void()> callback);
        // A greyed-out line, used to show status
        int addInfoItem(const std::string &name);
        void addSeparator();

        void setName(int index, const std::string &name);
        void setChecked(int index, bool checked);

    private:
        static void handleMenuAction(void *menuRef, void *itemRef);

        XPLMMenuID menuId = nullptr;
        int parentItemIndex = -1;
        // Indexed by the item's refcon; empty for info items and separators
        std::vector<std::function<void()>> callbacks;
};

#endif
