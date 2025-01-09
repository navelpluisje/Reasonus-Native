#ifndef CSURF_CHANNEL_MANAGER_RESOURCES_H_
#define CSURF_CHANNEL_MANAGER_RESOURCES_H_

#include <string>

using namespace std;

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 5, it will return the left pan representation
 *
 * @param pan The pan value
 * @param panMode The pan mode
 * @return string
 */
string GetPanString(double pan);

/**
 * @brief Get a string representation of the width value and the panMode. If panMode is 5, it will return the right pan representation
 *
 * @param width The pan value
 * @param panMode The pan mode
 * @return string
 */
string GetWidthString(double width, int panMode);

/**
 * @brief Get a string representation of the automtion mode
 *
 * @param automationMode The automation mode
 * @return string
 */
string GetAutomationString(int automationMode);

/**
 * @brief Get a string representation of the send mode
 *
 * @param sendMode The send mode
 * @return string
 */
string GetSendModeString(int sendMode);

#endif