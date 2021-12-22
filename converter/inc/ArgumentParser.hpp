#pragma once
#include <vector>
#include <unordered_map>
#include <string>

namespace orbtool
{

    class ArgumentParser
    {
    private:
        enum class SwitchType : uint8_t
        {
            SWITCH_DEFAULT,
            SWITCH_FLAG,
            SWITCH_ARRAY
        };
        struct SwitchDesc
        {
            const char* description;
            const char* name;
            const char* alias;
            SwitchType type;
        };
        const char* m_executablePath;
        const char* m_executableName;
        std::unordered_map<std::string, std::vector<const char*>> m_switches;
        std::vector<const char*> m_unboundParams;
        std::vector<const char*> m_validConfigs;
        std::vector<SwitchDesc> m_switchDescs;
        bool m_warnOnInvalid = false;
        bool m_warnOnUnknown = false;
        bool m_allowFreeArguments = true;
        uint32_t m_currentFlagBit = 0u;
        uint32_t m_parsedFlags = 0u;
    private:
        bool ValidateArgument(const char* name, SwitchType type) const;
        uint32_t FindSwitchIndex(const char* name) const;
    public:
        void PrintUsage() const;
        bool ParseArguments(int argc, const char** argv);
        const char*const* GetSwitch(const char* name, uint32_t* outNumParams = nullptr) const;
        const char* GetExecutablePath() const;
        const char* GetUnboundParameter(uint32_t index) const;
        uint32_t GetConfiguration() const;
        const char* GetAlias(const char* name) const;
        uint32_t    GetNumberOfUnboundParameters() const;
        void WarnOnInvalid(bool warn = true) { m_warnOnInvalid = warn; }
        void WarnOnUnknownSwitch(bool warn = true) { m_warnOnUnknown = warn; }
        void AllowFreeArguments(bool allow = true) { m_allowFreeArguments = allow; }
        uint32_t RegisterArgument(const char* description, const char* name, const char* alias = nullptr);
        uint32_t RegisterFlag(const char* description, const char* name, const char* alias = nullptr);
        uint32_t RegisterArray(const char* description, const char* name, const char* alias = nullptr);
        void RegisterValidConfigurations(std::initializer_list<const char*> validConfigs, bool append = true);
        void SetExecutableName(const char* name) { m_executableName = name; }
    };

}