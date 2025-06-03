#pragma once

#ifdef SKINSMANAGER_EXPORTS
#define SKINSMANAGER_API __declspec(dllexport)
#else
#define SKINSMANAGER_API __declspec(dllimport)
#endif

extern "C" {
    SKINSMANAGER_API bool isClassicSkinUnlocked(int highScore);
    SKINSMANAGER_API bool isGoldenSkinUnlocked(int highScore);
    SKINSMANAGER_API bool isRainbowSkinUnlocked(int highScore);
    SKINSMANAGER_API bool isLegendarySkinUnlocked(int highScore);
    SKINSMANAGER_API const char* getSkinName(int skinId);
    SKINSMANAGER_API int getRequiredScore(int skinId);
}