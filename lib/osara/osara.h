#ifndef _OSARA_H
#define _OSARA_H

#include <string>

void outputMessage(const std::string &message, bool interrupt = true);
void outputMessage(std::ostringstream &message, bool interrupt = true);

#endif