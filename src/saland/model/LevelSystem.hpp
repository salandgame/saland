/*
 * LevelSystem - A standalone, portable level/stats calculation module.
 *
 * This module has no game-specific dependencies and can be reused
 * across different projects. It provides:
 *   - XP threshold calculations
 *   - Level derivation from total XP
 *   - Stat scaling based on level (health, mana, damage multiplier)
 *
 * Scaling formula: base * (1 + 0.05 * level^1.5)
 * XP thresholds: 150 * level^2
 */

#ifndef LEVELSYSTEM_HPP
#define LEVELSYSTEM_HPP

#include <cmath>

struct LevelStats {
	int level = 1;
	float maxHealth = 10.0f;
	float maxMana = 20.0f;
	float damageMultiplier = 1.0f;
	int xpToNextLevel = 150;
};

/**
 * Returns the total XP required to reach a given level.
 * Level 1 requires 0 XP. Level 2 requires 600 XP. Etc.
 */
int xpRequiredForLevel(int level);

/**
 * Derives the current level from total accumulated XP.
 * A player with 0 XP is level 1.
 */
int levelFromXp(int totalXp);

/**
 * Calculates scaled stats for a given level using polynomial scaling.
 * The formula is: stat = base * (1 + 0.05 * level^1.5)
 *
 * @param baseHealth      Base maximum health at level 0/1
 * @param baseMana        Base maximum mana at level 0/1
 * @param baseDamageMultiplier  Base damage multiplier (typically 1.0)
 * @param level           The level to calculate stats for
 * @return LevelStats with all computed values
 */
LevelStats calculateStats(float baseHealth, float baseMana, float baseDamageMultiplier, int level);

#endif /* LEVELSYSTEM_HPP */
