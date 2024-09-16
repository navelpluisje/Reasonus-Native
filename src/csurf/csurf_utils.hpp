#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

/**
 * @brief Check if the bit with index key is set in val
 *
 * @param val The value to check
 * @param key The key to search for
 * @return true
 * @return false
 */
static bool hasBit(int val, int key)
{
    return val & (1 << key);
};

#endif // CSURF_UTILS_H_