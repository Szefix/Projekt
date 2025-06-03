#include "SkinsManager.h"
#include <string>

extern "C" {
    SKINSMANAGER_API bool isClassicSkinUnlocked(int highScore) {
        return highScore >= 0;
    }

    SKINSMANAGER_API bool isGoldenSkinUnlocked(int highScore) {
        return highScore >= 50;
    }

    SKINSMANAGER_API bool isRainbowSkinUnlocked(int highScore) {
        return highScore >= 100;
    }

    SKINSMANAGER_API bool isLegendarySkinUnlocked(int highScore) {
        return highScore >= 200;
    }

    SKINSMANAGER_API const char* getSkinName(int skinId) {
        static std::string names[] = {"Classic", "Golden", "Rainbow", "Legendary"};
        if (skinId >= 0 && skinId < 4) {
            return names[skinId].c_str();
        }
        return "Unknown";
    }

    SKINSMANAGER_API int getRequiredScore(int skinId) {
        int scores[] = {0, 50, 100, 200};
        if (skinId >= 0 && skinId < 4) {
            return scores[skinId];
        }
        return 0;
    }
}