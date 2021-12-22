#include "ArgumentParser.hpp"
#include <string>
#include <cstring>
#include <limits>
#include <algorithm>
#include <numeric>

namespace orbtool
{

    void ArgumentParser::PrintUsage() const
    {
        for (auto config : m_validConfigs)
        {
            printf_s("$>%s ", m_executableName);
            auto len = strlen(config);
            if (len > m_switchDescs.size())
            {
                printf_s("[ERROR] Something went wrong. The configuration '%s' contains more bits than there are flags (%lld).\n", config, m_switchDescs.size());
                return;
            }
            for (auto i = 0u; i < len; ++i)
            {
                if (std::toupper(config[i]) == 'X')
                {
                    switch (m_switchDescs.at(i).type)
                    {
                    case SwitchType::SWITCH_DEFAULT: 
                        printf_s("[<-%s %s_value>] ", m_switchDescs.at(i).name, m_switchDescs.at(i).name);
                        break;
                    case SwitchType::SWITCH_ARRAY:
                        printf_s("[<--%s %s_value*>] ", m_switchDescs.at(i).name, m_switchDescs.at(i).name);
                        break;
                    case SwitchType::SWITCH_FLAG:
                        // Fallthrough
                    default:
                        printf_s("[<-%s>] ", m_switchDescs.at(i).name);
                        break;
                    }
                }
                else if (config[i] == '1')
                {
                    switch (m_switchDescs.at(i).type)
                    {
                    case SwitchType::SWITCH_DEFAULT:
                        printf_s("<-%s %s_value> ", m_switchDescs.at(i).name, m_switchDescs.at(i).name);
                        break;
                    case SwitchType::SWITCH_ARRAY:
                        printf_s("<--%s %s_value*> ", m_switchDescs.at(i).name, m_switchDescs.at(i).name);
                        break;
                    case SwitchType::SWITCH_FLAG:
                        // Fallthrough
                    default:
                        printf_s("<-%s> ", m_switchDescs.at(i).name);
                        break;
                    }
                }
            }
            for (auto i = 0u; i < len; ++i)
            {
                if (std::toupper(config[i]) == 'X' || config[i] == '1')
                {
                    printf_s("\n  -%s ", m_switchDescs.at(i).name);
                    if (m_switchDescs.at(i).alias)
                        printf_s("(-%s) ", m_switchDescs.at(i).alias);
                    printf_s("- %s", m_switchDescs.at(i).description);
                }
            }
            printf_s("\n\n");
        }
    }

    bool ArgumentParser::ParseArguments(int argc, const char** argv)
    {
        m_switches.clear();
        m_executablePath = argv[0];
        for (auto i = 1u; i < argc; ++i)
        {
            if (argv[i][0] == '-')
            {
                auto len = strlen(argv[i]);
                if (len > 1 && argv[i][1] == '-')
                {
                    auto paramName = argv[i] + 2;
                    std::vector<const char*> parameters;
                    for (++i ; i < argc; ++i)
                    {
                        if (argv[i][0] == '-')
                        {
                            --i;
                            break;
                        }
                        parameters.emplace_back(argv[i]);
                    }
                    m_switches.emplace(paramName, std::move(parameters));
                    if (!ValidateArgument(paramName, SwitchType::SWITCH_ARRAY))
                        return false;
                }
                else
                {
                    if (len == 1)
                        continue;

                    if (i < argc - 1)
                    {
                        if (argv[i + 1][0] == '-')
                        {
                            if (!ValidateArgument(argv[i] + 1, SwitchType::SWITCH_FLAG))
                                return false;

                            m_switches.emplace(argv[i] + 1, std::vector<const char*>{ "flag:dummy" });
                        }
                        else
                        {
                            if (!ValidateArgument(argv[i] + 1, SwitchType::SWITCH_DEFAULT))
                                return false;

                            m_switches.emplace(argv[i] + 1, std::vector<const char*>{ argv[i + 1] });
                            ++i;
                        }
                    }
                    else
                    {
                        if (!ValidateArgument(argv[i] + 1, SwitchType::SWITCH_FLAG))
                            return false;

                        m_switches.emplace(argv[i] + 1, std::vector<const char*>{ "flag:dummy" });
                    }
                }
            }
            else
            {
                if (!m_allowFreeArguments)
                {
                    printf_s("[ERROR] Unbound arguments not allowed: %s\n\n", argv[i]);
                    PrintUsage();
                    return false;
                }
                m_unboundParams.emplace_back(argv[i]);
            }
        }

        // Validate the parsed arguments
        if (!m_warnOnInvalid)
            return true;

        auto config = GetConfiguration();
        if (config == std::numeric_limits<uint32_t>::max())
        {
            printf_s("[ERROR] Invalid parameter configuration (ERR_CONFIGURATION_INVALID)\n");
            PrintUsage();
            return false;
        }

        return true;
    }

    uint32_t ArgumentParser::GetConfiguration() const
    {
        if (m_validConfigs.empty())
            return std::numeric_limits<uint32_t>::max();

        auto configs = m_validConfigs;
        for (const auto& sw : m_switches)
        {
            auto index = FindSwitchIndex(sw.first.c_str());
            configs.erase(std::remove_if(
                    configs.begin(),
                    configs.end(),
                    [&](const char* ptr) -> bool {
                        return ptr[index] == '0';
                    }
                ),
                configs.end()
            );
            if (configs.empty())
                return std::numeric_limits<uint32_t>::max();
        }

        configs.erase(
            std::remove_if(
                configs.begin(),
                configs.end(),
                [&](const char* config) { 
                    for (auto i = 0u; i < m_switchDescs.size(); ++i)
                    {
                        auto is_set = (GetSwitch(m_switchDescs.at(i).name) != nullptr);
                        if (config[i] == '1' && !is_set)
                            return true;
                    }
                    return false;
                }
            ),
            configs.end()
        );

        if (configs.empty())
            return std::numeric_limits<uint32_t>::max();

        auto result = configs[0];
        auto count = 0u;
        for (auto config : configs)
        {
            uint32_t current = std::count_if(config, config + strlen(config), [](char c) { return c == '1'; });
            if (current > count)
            {
                count = current;
                result = config;
            }
        }
        for (auto i = 0u; i < m_validConfigs.size(); ++i)
        {
            if (!strcmp(m_validConfigs[i], result))
                return i;
        }
        return std::numeric_limits<uint32_t>::max();
    }

    uint32_t ArgumentParser::FindSwitchIndex(const char* name) const
    {
        for (auto i = 0u; i < m_switchDescs.size(); ++i)
        {
            if (!strcmp(m_switchDescs[i].name, name) || !strcmp(m_switchDescs[i].alias, name))
                return i;
        }

        // Not found
        return std::numeric_limits<uint32_t>::max();
    }

    bool ArgumentParser::ValidateArgument(const char* name, SwitchType type) const
    {
        if (!m_warnOnUnknown)
            return true;

        for (const auto& desc : m_switchDescs)
        {
            if (!strcmp(desc.name, name) || !strcmp(desc.alias, name))
            {
                if (desc.type == type || (type == SwitchType::SWITCH_DEFAULT && desc.type == SwitchType::SWITCH_FLAG))
                    return true;

                switch (desc.type)
                {
                case SwitchType::SWITCH_ARRAY: printf_s("Use '--%s' instead of '-%s' (ERR_PARAM_IS_ARRAY)\n", name, name); break;
                case SwitchType::SWITCH_FLAG: printf_s("Use '-%s' instead of '--%s' (ERR_PARAM_IS_FLAG)\n", name, name); break;
                case SwitchType::SWITCH_DEFAULT: printf_s("Parameter '-%s' needs exactly one value (ERR_VALUE_MISSING or ERR_TOO_MANY_VALUES)\n", name); break;
                }
                return false;
            }
        }
        
        printf_s("Unknown parameter '%s'\n", name);
        return false;
    }

    const char* ArgumentParser::GetAlias(const char* name) const
    {
        for (const auto& desc : m_switchDescs)
            if (!strcmp(desc.name, name))
                return desc.alias;
        
        return nullptr;
    }

    const char*const* ArgumentParser::GetSwitch(const char* name, uint32_t* outNumParams) const
    {
        auto it = m_switches.find(name);
        if (it == m_switches.end())
        {
            // Try finding the switch by its alias:
            auto alias = GetAlias(name);
            if (!alias)
                return nullptr;
            
            it = m_switches.find(alias);
            if (it == m_switches.end())
                return nullptr;
            
            // Else: Fallthrough
        }
        
        if (outNumParams)
            *outNumParams = it->second.size();

        return it->second.data();
    }

    const char* ArgumentParser::GetExecutablePath() const
    {
        return m_executablePath;
    }

    const char* ArgumentParser::GetUnboundParameter(uint32_t index) const
    {
        if (index >= m_unboundParams.size())
            return nullptr;

        return m_unboundParams.at(index);
    }

    uint32_t    ArgumentParser::GetNumberOfUnboundParameters() const
    {
        return m_unboundParams.size();
    }

    uint32_t ArgumentParser::RegisterArgument(const char* description, const char* name, const char* alias)
    {
        if (m_validConfigs.size() != 0)
        {
            printf_s("[ERROR] You can only register arguments if there are no valid configurations.\n");
            return 0;
        }

        m_switchDescs.emplace_back(SwitchDesc{ description, name, alias, SwitchType::SWITCH_DEFAULT });
        return m_currentFlagBit++;
    }

    uint32_t ArgumentParser::RegisterFlag(const char* description, const char* name, const char* alias)
    {
        if (m_validConfigs.size() != 0)
        {
            printf_s("[ERROR] You can only register arguments if there are no valid configurations.\n");
            return 0;
        }

        m_switchDescs.emplace_back(SwitchDesc{ description, name, alias, SwitchType::SWITCH_FLAG });
        return m_currentFlagBit++;
    }

    uint32_t ArgumentParser::RegisterArray(const char* description, const char* name, const char* alias)
    {
        if (m_validConfigs.size() != 0)
        {
            printf_s("[ERROR] You can only register arguments if there are no valid configurations.\n");
            return 0;
        }

        m_switchDescs.emplace_back(SwitchDesc{ description, name, alias, SwitchType::SWITCH_ARRAY });
        return m_currentFlagBit++;
    }

    void ArgumentParser::RegisterValidConfigurations(std::initializer_list<const char*> validConfigs, bool append)
    {
        for (auto config : validConfigs)
        {
            if (strlen(config) != m_switchDescs.size())
            {
                printf_s("[ERROR] Every configuration needs to contain as many characters as there possible arguments (%lld)\n", m_switchDescs.size());
                return;
            }
        }

        if (!append)
            m_validConfigs = validConfigs;
        else 
            m_validConfigs.insert(m_validConfigs.end(), validConfigs.begin(), validConfigs.end());
    }

}