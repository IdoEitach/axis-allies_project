#pragma once
#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>

class StateMachine {
public:
    using ConditionFunction = std::function<bool()>;
    using ActionFunction = std::function<void()>;

    StateMachine();
    void addTransition(ConditionFunction condition, ActionFunction action);
    void removeTransition(ConditionFunction condition);
    void run();
	void clearTransitions() { transitions.clear(); } // Optional: Clear all transitions

private:
    std::vector<std::pair<ConditionFunction, ActionFunction>> transitions;
};
