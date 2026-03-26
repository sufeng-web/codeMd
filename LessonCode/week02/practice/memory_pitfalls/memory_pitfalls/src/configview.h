#pragma once

class ThemeNameView {
public:
    void setThemeName(const char* name) { m_name = name; }
    const char* getThemeName() const { return m_name ? m_name : ""; }

private:
    const char* m_name = nullptr;
};

class ConfigValueView {
public:
    void setValue(const char* value) { m_value = value; }
    const char* getValue() const { return m_value ? m_value : ""; }

private:
    const char* m_value = nullptr;
};
