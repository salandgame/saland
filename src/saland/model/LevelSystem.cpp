#include "LevelSystem.hpp"

int xpRequiredForLevel(int level) {
	if (level <= 1) {
		return 0;
	}
	return 150 * level * level;
}

int levelFromXp(int totalXp) {
	int level = 1;
	while (xpRequiredForLevel(level + 1) <= totalXp) {
		level++;
	}
	return level;
}

LevelStats calculateStats(float baseHealth, float baseMana, float baseDamageMultiplier, int level) {
	LevelStats stats;
	stats.level = level;
	float multiplier = 1.0f + 0.05f * std::pow(static_cast<float>(level), 1.5f);
	stats.maxHealth = baseHealth * multiplier;
	stats.maxMana = baseMana * multiplier;
	stats.damageMultiplier = baseDamageMultiplier * multiplier;
	stats.xpToNextLevel = xpRequiredForLevel(level + 1);
	return stats;
}
