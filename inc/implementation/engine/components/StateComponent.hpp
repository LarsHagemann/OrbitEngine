#pragma once

#include "interfaces/engine/GameComponent.hpp"
#include "implementation/engine/components/KeyboardComponent.hpp"
#include "implementation/engine/components/MouseComponent.hpp"

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <unordered_set>

namespace orbit
{

    struct DefaultState
    {
        size_t stateID;
        std::string_view stateName;
    };

    struct KeyboardTransition
    {
        KeyCode keyCode;
    };
    struct TemporalTransition
    {
        uint32_t elapsedTimeInMS;
    };
    struct MouseTransition
    {
        MouseComponent::MouseButton button;
    };
    struct CustomTransition
    {
        std::string_view customId;
    };
    struct Dummy
    {};

    union DefaultTransitionContainer
    {
        KeyboardTransition keyboardTransition;
        TemporalTransition temporalTransition;
        MouseTransition mouseTransition;
        CustomTransition customTransition;

        Dummy dummyTransition;

        DefaultTransitionContainer(KeyboardTransition t) :
            keyboardTransition(t)
        {}
        DefaultTransitionContainer(TemporalTransition t) :
            temporalTransition(t)
        {}
        DefaultTransitionContainer(MouseTransition t) :
            mouseTransition(t)
        {}
        DefaultTransitionContainer(Dummy t) :
            dummyTransition(t)
        {}
        DefaultTransitionContainer(CustomTransition t) :
            customTransition(t)
        {}
    };

    template<class TransitionContainer = DefaultTransitionContainer>
    struct DefaultTransition
    {
        using container_type = TransitionContainer;

        constexpr static uint32_t TRANSITION_KEYDOWN = 0;
        constexpr static uint32_t TRANSITION_KEYUP = 1;
        constexpr static uint32_t TRANSITION_TIME_ELAPSED = 2;
        constexpr static uint32_t TRANSITION_MOUSE_KEY_DOWN = 3;
        constexpr static uint32_t TRANSITION_MOUSE_KEY_UP = 4;
        constexpr static uint32_t TRANSITION_MOUSE_WHEEL_UP = 5;
        constexpr static uint32_t TRANSITION_MOUSE_WHEEL_DOWN = 6;
        constexpr static uint32_t TRANSITION_CUSTOM_EVENT = 7;

        TransitionContainer container;
    };

    template<class StateClass = DefaultState, class Transition = DefaultTransition<>>
    class StateComponent : public Updatable
    {
    public:
        using StateIndex = size_t;
        using StateName  = std::string_view;
        using TransitionType = size_t;
        using StateChangeCallback = std::function<void(StateIndex, StateName/* previous/next state */)>;
    protected:
        std::shared_ptr<KeyboardComponent> _keyboard;
        std::shared_ptr<MouseComponent> _mouse;
        Clock _stateClock;

        std::vector<StateClass> _states;
        std::vector<std::tuple<
            StateIndex/*     <-- from*/,
            StateIndex/*     <-- to  */,
            TransitionType/* <-- type of transition*/,
            Transition/*     <-- transition data*/>
            > _transitions;
        
        std::unordered_map<StateIndex, StateChangeCallback> _onEnterCallbacks;
        std::unordered_map<StateIndex, StateChangeCallback> _onLeaveCallbacks;

        std::unordered_set<std::string_view> _customEvents;

        StateIndex _currentState;
    public:
        StateComponent(GameObject* object, std::shared_ptr<KeyboardComponent> keyboard, std::shared_ptr<MouseComponent> mouse) : 
            Updatable(object),
            _keyboard(keyboard),
            _mouse(mouse),
            _currentState(0)
        {}
        static std::shared_ptr<StateComponent> create(GameObject* object, std::shared_ptr<KeyboardComponent> keyboard, std::shared_ptr<MouseComponent> mouse)
        {
            return std::make_shared<StateComponent>(object, keyboard, mouse);
        }

        void SetNumberOfStates(size_t numberOfStates)
        {
            _states.resize(numberOfStates);
        }
        void SetStateName(StateIndex id, std::string_view name)
        {
#ifdef _DEBUG
            if (id >= _states.size())
            {
                ORBIT_ERROR("State index %d out of range. Max state index: %d", id, _states.size() - 1);
                return "<out_of_range>";
            }
#endif
            _states[id].stateName = name;
        }
        StateIndex AddState(std::string_view name)
        {
            _states.emplace_back(StateClass{ _states.size() - 1, name });
            return _states.size() - 2;
        }
        StateName GetStateName(StateIndex id) const
        {
#ifdef _DEBUG
            if (id >= _states.size())
            {
                ORBIT_ERROR("State index %d out of range. Max state index: %d", id, _states.size() - 1);
                return "<out_of_range>";
            }
#endif
            return _states[id].stateName;
        }

#define VERIFY_FROM_TO(from, to) \
if (from >= _states.size()) \
{ \
    ORBIT_ERRER("Parameter <from=%d> is out of range [0, %d).", from, _states.size()); \
    return; \
} \
if (to >= _states.size()) \
{ \
    ORBIT_ERROR("Parameter <to=%d> is out of range [0, %d).", to, _states.size()); \
    return; \
}
#define TRANSITION(from, to, name, transition_type, parameter) \
ORBIT_INFO_LEVEL(FormatString("Transition from %d to %d on event <%s>.", from, to, #transition_type), 32);  \
_transitions.emplace_back(from, to, Transition::name, Transition{ Transition::container_type{ transition_type{ parameter } } })

        void Transition_OnKeyDown(StateIndex from, StateIndex to, KeyCode keyCode)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_KEYDOWN, KeyboardTransition, keyCode);
        }
        void Transition_OnKeyUp(StateIndex from, StateIndex to, KeyCode keyCode)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_KEYUP, KeyboardTransition, keyCode);
        }
        void Transition_OnTimeElapsed(StateIndex from, StateIndex to, uint32_t elapsedTimeInMS)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_TIME_ELAPSED, TemporalTransition, elapsedTimeInMS);
        }
        void Transition_OnMouseKeyDown(StateIndex from, StateIndex to, MouseComponent::MouseButton button)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_MOUSE_KEY_DOWN, MouseTransition, button);
        }
        void Transition_OnMouseKeyUp(StateIndex from, StateIndex to, MouseComponent::MouseButton button)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_MOUSE_KEY_UP, MouseTransition, button);
        }
        void Transition_OnMouseWheelUp(StateIndex from, StateIndex to)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_MOUSE_WHEEL_DOWN, Dummy, );
            _transitions.emplace_back(from, to, Transition::TRANSITION_MOUSE_WHEEL_DOWN, Transition{ Dummy{  } });
        }
        void Transition_OnMouseWheelDown(StateIndex from, StateIndex to)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_MOUSE_WHEEL_UP, Dummy, );
        }
        void Transition_OnCustomEvent(StateIndex from, StateIndex to, std::string_view customEventName)
        {
            VERIFY_FROM_TO(from, to);
            TRANSITION(from, to, TRANSITION_CUSTOM_EVENT, CustomTransition, customEventName);
        }

        void Transition_OnKeyDown(StateName from, StateName to, KeyCode keyCode)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_KEYDOWN, KeyboardTransition, keyCode);
        }
        void Transition_OnKeyUp(StateName from, StateName to, KeyCode keyCode)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_KEYUP, KeyboardTransition, keyCode);
        }
        void Transition_OnTimeElapsed(StateName from, StateName to, uint32_t elapsedTimeInMS)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_TIME_ELAPSED, TemporalTransition, elapsedTimeInMS);
        }
        void Transition_OnMouseKeyDown(StateName from, StateName to, MouseComponent::MouseButton button)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_MOUSE_KEY_DOWN, MouseTransition, button);
        }
        void Transition_OnMouseKeyUp(StateName from, StateName to, MouseComponent::MouseButton button)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_MOUSE_KEY_UP, MouseTransition, button);
        }
        void Transition_OnMouseWheelUp(StateName from, StateName to)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_MOUSE_WHEEL_DOWN, Dummy, );
        }
        void Transition_OnMouseWheelDown(StateName from, StateName to)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_MOUSE_WHEEL_UP, Dummy, );
        }
        void Transition_OnCustomEvent(StateName from, StateName to, std::string_view customEventName)
        {
            auto fromId = GetStateByName(from), toId = GetStateByName(to);
            VERIFY_FROM_TO(fromId, toId);
            TRANSITION(fromId, toId, TRANSITION_CUSTOM_EVENT, CustomTransition, customEventName);
        }

#undef TRANSITION
#undef VERIFY_FROM_TO

        void Trigger_CustomEvent(std::string_view customEventName)
        {
            _customEvents.emplace(customEventName);
        }

        StateName GetCurrentStateName() const { return GetStateName(_currentState); }
        StateIndex GetCurrentStateId() const { return _currentState; }

        void SetCurrentState(StateName name)
        {
            const auto idx = GetStateByName(name);
            SetCurrentState(idx);
        }
        void SetCurrentState(StateIndex index) { _currentState = index; }

        void RegisterOnStateEnterCallback(StateIndex index, StateChangeCallback&& callback)
        {
#ifdef _DEBUG
            if (index >= _states.size())
            {
                ORBIT_ERROR("State index %d out of range. Max state index: %d", index, _states.size() - 1);
                return;
            }
#endif
            _onEnterCallbacks.emplace(index, std::move(callback));
        }
        void RegisterOnStateEnterCallback(StateName name, StateChangeCallback&& callback)
        {
            const auto idx = GetStateByName(name);
            RegisterOnStateEnterCallback(idx, std::move(callback));
        }
        void RegisterOnStateLeaveCallback(StateIndex index, StateChangeCallback&& callback)
        {
#ifdef _DEBUG
            if (index >= _states.size())
            {
                ORBIT_ERROR("State index %d out of range. Max state index: %d", index, _states.size() - 1);
                return;
            }
#endif
            _onLeaveCallbacks.emplace(index, std::move(callback));
        }
        void RegisterOnStateLeaveCallback(StateName name, StateChangeCallback&& callback)
        {
            const auto idx = GetStateByName(name);
            RegisterOnStateLeaveCallback(idx, std::move(callback));
        }

        StateIndex GetStateByName(StateName name) const
        {
            for (auto i = 0u; i < _states.size(); ++i)
            {
                if (_states.at(i).stateName == name)
                    return i;
            }

#ifdef _DEBUG
            ORBIT_ERROR("Cannot find state with name '%s'.", name.data());
#endif
            return std::numeric_limits<StateIndex>::max();
        }

        virtual void Update(const Time& dt) override
        {
            for (const auto& t : _transitions)
            {
                if (std::get<0>(t) == _currentState)
                {
                    bool condition_met = false;
                    switch (std::get<2>(t))
                    {
                    case Transition::TRANSITION_KEYDOWN: {
                        auto key = std::get<3>(t).container.keyboardTransition.keyCode;
                        if (_keyboard->keydownThisFrame(key))
                            condition_met = true;
                    } break;

                    case Transition::TRANSITION_KEYUP: {
                        auto key = std::get<3>(t).container.keyboardTransition.keyCode;
                        if (_keyboard->keyupThisFrame(key))
                            condition_met = true;
                    } break;
                    
                    case Transition::TRANSITION_TIME_ELAPSED: {
                        auto elapsed = std::get<3>(t).container.temporalTransition.elapsedTimeInMS;
                        if (elapsed <= static_cast<uint32_t>(_stateClock.GetElapsedTime().asMilliseconds()))
                            condition_met = true;
                    } break;

                    case Transition::TRANSITION_MOUSE_KEY_DOWN: {
                        auto button = std::get<3>(t).container.mouseTransition.button;
                        if (_mouse->buttonDownThisFrame(button))
                            condition_met = true;
                    } break;

                    case Transition::TRANSITION_MOUSE_KEY_UP: {
                        auto button = std::get<3>(t).container.mouseTransition.button;
                        if (_mouse->buttonUpThisFrame(button))
                            condition_met = true;
                    } break;

                    case Transition::TRANSITION_MOUSE_WHEEL_UP: {
                        if (_mouse->wheelDelta() > 0)
                            condition_met = true;
                    } break;

                    case Transition::TRANSITION_MOUSE_WHEEL_DOWN: {
                        if (_mouse->wheelDelta() < 0)
                            condition_met = true;
                    } break;

                    case Transition::TRANSITION_CUSTOM_EVENT: {
                        auto eventName = std::get<3>(t).container.customTransition.customId;
                        if (_customEvents.find(eventName) != _customEvents.end())
                            condition_met = true;
                    } break;

                    } /* end of switch */

                    if (condition_met)
                    {
                        auto nextState = std::get<1>(t);
                        auto leaveIt = _onLeaveCallbacks.find(_currentState);
                        auto enterIt = _onEnterCallbacks.find(nextState);

                        if (leaveIt != _onLeaveCallbacks.end())
                            leaveIt->second(nextState, GetStateName(nextState));
                        if (enterIt != _onEnterCallbacks.end())
                            enterIt->second(_currentState, GetStateName(_currentState));

                        _currentState = nextState;
                        _stateClock.Restart();

                        _customEvents.clear();

                        return;
                    }
                }
            }

            _customEvents.clear();
        }
    };

}
