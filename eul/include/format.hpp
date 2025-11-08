#pragma once

namespace eul
{

    /**
 * @brief Format a float as a string with up to 6 signficant digits a period and a sign if negative 
 * @returns 0-terminated string with the formatted float
 * Will return default strings for NaN INF and -INF 
 * @note The function uses a static buffer for all calls and is therefore not concurrency safe, even after return
 */
const char* to_string(float value);

}
