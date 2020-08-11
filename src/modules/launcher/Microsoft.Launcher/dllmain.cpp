#include "pch.h"
#include <interface/powertoy_module_interface.h>
#include <common/settings_objects.h>
#include <common/common.h>
#include "trace.h"
#include "resource.h"
#include <common/os-detect.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Trace::RegisterProvider();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Trace::UnregisterProvider();
        break;
    }
    return TRUE;
}

// These are the properties shown in the Settings page.
struct ModuleSettings
{
} g_settings;

// Implement the PowerToy Module Interface and all the required methods.
class Microsoft_Launcher : public PowertoyModuleIface
{
private:
    // The PowerToy state.
    bool m_enabled = false;

    // Load initial settings from the persisted values.
    void init_settings();

    // Handle to launch and terminate the launcher
    HANDLE m_hProcess;

    //contains the name of the powerToys
    std::wstring app_name;

    // Time to wait for process to close after sending WM_CLOSE signal
    static const int MAX_WAIT_MILLISEC = 10000;

public:
    // Constructor
    Microsoft_Launcher()
    {
        app_name = GET_RESOURCE_STRING(IDS_LAUNCHER_NAME);
        init_settings();
    };

    ~Microsoft_Launcher()
    {
        if (m_enabled)
        {
            terminateProcess();
        }
        m_enabled = false;
    }

    // Destroy the powertoy and free memory
    virtual void destroy() override
    {
        delete this;
    }

    // Return the display name of the powertoy, this will be cached by the runner
    virtual const wchar_t* get_name() override
    {
        return app_name.c_str();
    }

    // Return JSON with the configuration options.
    virtual bool get_config(wchar_t* buffer, int* buffer_size) override
    {
        HINSTANCE hinstance = reinterpret_cast<HINSTANCE>(&__ImageBase);

        // Create a Settings object.
        PowerToysSettings::Settings settings(hinstance, get_name());
        settings.set_description(GET_RESOURCE_STRING(IDS_LAUNCHER_SETTINGS_DESC));
        settings.set_overview_link(L"https://aka.ms/PowerToysOverview_PowerToysRun");

        return settings.serialize_to_buffer(buffer, buffer_size);
    }

    // Signal from the Settings editor to call a custom action.
    // This can be used to spawn more complex editors.
    virtual void call_custom_action(const wchar_t* action) override
    {
        static UINT custom_action_num_calls = 0;
        try
        {
            // Parse the action values, including name.
            PowerToysSettings::CustomActionObject action_object =
                PowerToysSettings::CustomActionObject::from_json_string(action);
        }
        catch (std::exception ex)
        {
            // Improper JSON.
        }
    }

    // Called by the runner to pass the updated settings values as a serialized JSON.
    virtual void set_config(const wchar_t* config) override
    {
        try
        {
            // Parse the input JSON string.
            PowerToysSettings::PowerToyValues values =
                PowerToysSettings::PowerToyValues::from_json_string(config);

            // If you don't need to do any custom processing of the settings, proceed
            // to persists the values calling:
            values.save_to_settings_file();
            // Otherwise call a custom function to process the settings before saving them to disk:
            // save_settings();
        }
        catch (std::exception ex)
        {
            // Improper JSON.
        }
    }

    // Enable the powertoy
    virtual void enable()
    {
        // Start PowerLauncher.exe only if the OS is 19H1 or higher
        if (UseNewSettings())
        {
            unsigned long powertoys_pid = GetCurrentProcessId();

            std::wstring executable_args = L"";
            executable_args.append(std::to_wstring(powertoys_pid));

            SHELLEXECUTEINFOW sei{ sizeof(sei) };
            sei.fMask = { SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI };
            sei.lpFile = L"modules\\launcher\\PowerLauncher.exe";
            sei.nShow = SW_SHOWNORMAL;
            sei.lpParameters = executable_args.data();
            ShellExecuteExW(&sei);

            m_hProcess = sei.hProcess;
        }

        m_enabled = true;
    }

    // Disable the powertoy
    virtual void disable()
    {
        if (m_enabled)
        {
            terminateProcess();
        }

        m_enabled = false;
    }

    // Returns if the powertoys is enabled
    virtual bool is_enabled() override
    {
        return m_enabled;
    }

    // Callback to send WM_CLOSE signal to each top level window.
    static BOOL CALLBACK requestMainWindowClose(HWND nextWindow, LPARAM closePid)
    {
        DWORD windowPid;
        GetWindowThreadProcessId(nextWindow, &windowPid);

        if (windowPid == (DWORD)closePid)
            ::PostMessage(nextWindow, WM_CLOSE, 0, 0);

        return true;
    }

    // Terminate process by sending WM_CLOSE signal and if it fails, force terminate.
    void terminateProcess()
    {
        DWORD processID = GetProcessId(m_hProcess);
        EnumWindows(&requestMainWindowClose, processID);
        const DWORD result = WaitForSingleObject(m_hProcess, MAX_WAIT_MILLISEC);
        if (result == WAIT_TIMEOUT || result == WAIT_FAILED)
        {
            TerminateProcess(m_hProcess, 1);
        }
    }
};

// Load the settings file.
void Microsoft_Launcher::init_settings()
{
    try
    {
        // Load and parse the settings file for this PowerToy.
        PowerToysSettings::PowerToyValues settings =
            PowerToysSettings::PowerToyValues::load_from_settings_file(get_name());
    }
    catch (std::exception ex)
    {
        // Error while loading from the settings file. Let default values stay as they are.
    }
}

extern "C" __declspec(dllexport) PowertoyModuleIface* __cdecl powertoy_create()
{
    return new Microsoft_Launcher();
}
