#include "GameConfig.h"

GameConfig& GameConfig::getInstance() {
    static GameConfig instance;
    return instance;
}

GameConfig::GameConfig() {
    configs["lang"] = "zh_CN";
}

QString GameConfig::getText(const QString& key) {
    if (configs.find(key) != configs.end()) return configs[key];
    return "";
}

void GameConfig::setLanguage(const QString& lang) {
    configs["lang"] = lang;
}