#include "plugins-menu.h"

#include "logger.h"

#include <cstdint>

PluginsMenu::PluginsMenu(const std::string &title) {
    parentItemIndex = XPLMAppendMenuItem(XPLMFindPluginsMenu(), title.c_str(), nullptr, 0);
    menuId = XPLMCreateMenu(title.c_str(), XPLMFindPluginsMenu(), parentItemIndex, handleMenuAction, this);
}

PluginsMenu::~PluginsMenu() {
    if (menuId != nullptr) {
        XPLMDestroyMenu(menuId);
        menuId = nullptr;
    }
    if (parentItemIndex >= 0) {
        XPLMRemoveMenuItem(XPLMFindPluginsMenu(), parentItemIndex);
        parentItemIndex = -1;
    }
}

int PluginsMenu::addItem(const std::string &name, std::function<void()> callback) {
    intptr_t ref = static_cast<intptr_t>(callbacks.size());
    callbacks.push_back(std::move(callback));
    return XPLMAppendMenuItem(menuId, name.c_str(), reinterpret_cast<void *>(ref), 0);
}

int PluginsMenu::addInfoItem(const std::string &name) {
    intptr_t ref = static_cast<intptr_t>(callbacks.size());
    callbacks.emplace_back();
    int index = XPLMAppendMenuItem(menuId, name.c_str(), reinterpret_cast<void *>(ref), 0);
    XPLMEnableMenuItem(menuId, index, 0);
    return index;
}

void PluginsMenu::addSeparator() {
    XPLMAppendMenuSeparator(menuId);
}

void PluginsMenu::setName(int index, const std::string &name) {
    XPLMSetMenuItemName(menuId, index, name.c_str(), 0);
}

void PluginsMenu::setChecked(int index, bool checked) {
    XPLMCheckMenuItem(menuId, index, checked ? xplm_Menu_Checked : xplm_Menu_Unchecked);
}

void PluginsMenu::handleMenuAction(void *menuRef, void *itemRef) {
    auto *self = static_cast<PluginsMenu *>(menuRef);
    intptr_t ref = reinterpret_cast<intptr_t>(itemRef);
    if (self == nullptr || ref < 0 || static_cast<size_t>(ref) >= self->callbacks.size()) {
        return;
    }

    // Exceptions must never cross back into X-Plane
    try {
        // Called through a copy, in case the callback changes the menu
        auto callback = self->callbacks[static_cast<size_t>(ref)];
        if (callback) {
            callback();
        }
    } catch (const std::exception &e) {
        Logger::getInstance().error("Menu action failed: %s", e.what());
    } catch (...) {
        Logger::getInstance().error("Menu action failed");
    }
}
