#pragma once

class ThemeNameView;

class ThemeDisplay {
public:
    ThemeDisplay() = default;

    void attachView(ThemeNameView* view);
    void setThemeNamePointer(const char* name);
    void refresh() const;

    const char* currentThemeName() const { return m_themeName; }

private:
    ThemeNameView* m_view = nullptr;
    const char* m_themeName = nullptr;
};
