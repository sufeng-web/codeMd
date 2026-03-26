#include "themedisplay.h"
#include "configview.h"
#include "guardalloc.h"
#include <iostream>

void ThemeDisplay::attachView(ThemeNameView* view) {
    m_view = view;
}

void ThemeDisplay::setThemeNamePointer(const char* name) {
    m_themeName = name;
}

void ThemeDisplay::refresh() const {
    const char* name = nullptr;
    if (m_view) {
        name = m_view->getThemeName();
    } else if (m_themeName) {
        name = m_themeName;
    }
    if (name) {
        validatePointer(name);
        std::cout << "Theme: " << name << "\n";
        return;
    }
    std::cout << "Theme: (none)\n";
}
