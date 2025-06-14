#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include <memory>
#include <reaper_imgui_functions.h>

class Example
{
public:
    static void start();
    ~Example();

private:
    static void loop();
    static std::unique_ptr<Example> s_inst;

    Example();
    void frame();

    ImGui_Context *m_ctx;
    int m_click_count;
    char m_text[255];
};
