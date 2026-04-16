#ifndef CSURF_FP_8_CHANNEL_MANAGER_RESOURCES_H_
#define CSURF_FP_8_CHANNEL_MANAGER_RESOURCES_H_

#include <string>

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 5, it will return the left pan representation
 *
 * @param pan The pan value
 * @param pan_mode The pan mode
 * @return string
 */
std::string GetPan1String(double pan, int pan_mode = 0);

/**
 * @brief Get a string representation of the width value and the panMode. If panMode is 5, it will return the right pan representation
 *
 * @param value The pan value
 * @param pan_mode The pan mode
 * @return string
 */
std::string GetPan2String(double value, int pan_mode);

/**
 * @brief Get a string representation of the automtion mode
 *
 * @param automationMode The automation mode
 * @return string
 */
std::string GetAutomationString(int automationMode);

/**
 * @brief Get a string representation of the send mode
 *
 * @param sendMode The send mode
 * @return string
 */
std::string GetSendModeString(int sendMode);

/**
 * @brief Get a string representation of the armed mode
 *
 * @param armed The send mode
 * @return string
 */
std::string GetArmedString(bool armed);

/**
 * @brief Get a string representation of the phase
 *
 * @param phase Is pahse inverted or not
 * @return
 */
std::string GetPhaseString(bool phase);

/**
 * @brief Get a string representation of the fx state
 *
 * @param fx_on Is pahse inverted or not
 * @return
 */
std::string GetTrackFXString(bool fx_on);

std::string GetVolumeString(double volume);


#endif
