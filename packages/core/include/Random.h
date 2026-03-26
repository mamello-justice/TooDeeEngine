#pragma once

/**
 * Random number generation utilities
 */
namespace Random
{
    /**
     * Generates a random float between min and hi (inclusive).
     */
    float between(const float& min, const float& hi);

    /**
     * Generates a random int between min and max (inclusive).
     */
    int between(const int& min, const int& max);

    /**
     * Generates a random sign, either -1 or 1.
     */
    int sign();
} // namespace Random
