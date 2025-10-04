#pragma once
#include <string>

void ensureSettings();
std::string getSetting(const std::string &key);
void setSetting(const std::string &key, const std::string &value);
bool getBoolSetting(const std::string &key);
int getIntSetting(const std::string &key, int defaultValue);

