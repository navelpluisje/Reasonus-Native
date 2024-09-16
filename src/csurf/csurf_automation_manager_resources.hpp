#ifndef CSURF_AUTOMATION_MANAGER_RESOURCES_H_
#define CSURF_AUTOMATION_MANAGER_RESOURCES_H_

const int AUTOMATION_OFF = -1;
const int AUTOMATION_TRIM = 0;
const int AUTOMATION_READ = 1;
const int AUTOMATION_TOUCH = 2;
const int AUTOMATION_LATCH = 4;
const int AUTOMATION_PREVIEW = 5;
const int AUTOMATION_WRITE = 3;

enum AutomationMode
{
    AutomationModeTrim = 0,
    AutomationModeRead = 1,
    AutomationModeTouch = 2,
    AutomationModeLatch = 4,
    AutomationModePreview = 5,
    AutomationModeWrite = 3,
};

#endif // CSURF_AUTOMATION_MANAGER_RESOURCES_H_