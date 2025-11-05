#ifndef I18N_HPP_
#define I18N_HPP_

#include <string>
#include <map>

class I18n
{
private:
    // Private Constructor
    I18n();

    mINI::INIStructure ini;
    std::string language;
    mINI::INIStructure translations;
    static I18n *instancePtr;

public:
    I18n(const I18n &obj) = delete;

    static I18n *GetInstance()
    {
        if (instancePtr == nullptr)
        {
            instancePtr = new I18n();
        }
        return instancePtr;
    }

    void SetLanguage(std::string lang);
    static void checkLocalesFiles();

    std::string t(std::string group, std::string key);
    std::string t(std::string group, std::string key, std::string arg1);
    std::string t(std::string group, std::string key, std::string arg1, std::string arg2);
    std::string t(std::string group, std::string key, std::string arg1, std::string arg2, std::string arg3);
};

#endif