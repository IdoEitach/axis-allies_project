#include "stateMachine.h"	

StateMachine::StateMachine() {};
/// <summary>
/// this function adds a transition to the state machine
/// it takes a condition and an action
/// the condition is a function that returns a boolean
/// the action is a function that if the condiston is true the action will be executed
/// </summary>
/// <param name="condition"></param>
/// <param name="action"></param>
void StateMachine::addTransition(ConditionFunction condition, ActionFunction action) {
	transitions.push_back(std::make_pair(condition, action));
}

/// <summary>
/// this function removes a transition from the state machine 
/// </summary>
/// <param name="condition"></param>
void StateMachine::removeTransition(ConditionFunction condition) {
	transitions.erase(std::remove_if(transitions.begin(), transitions.end(),
		[&condition](const std::pair<ConditionFunction, ActionFunction>& transition) {
			return transition.first.target_type() == condition.target_type();
		}), transitions.end());
}



/// <summary>
/// this function runs the state machine 
/// it goes through all the transitions and if the condition is true it executes the action
/// </summary>
void StateMachine::run() {
    // Using find_if to find the first transition whose condition is true
    auto it = std::find_if(transitions.begin(), transitions.end(),
        [](const std::pair<ConditionFunction, ActionFunction>& transition) {
            return transition.first();
        });

    // If a transition is found, execute its action
    if (it != transitions.end()) {
        it->second();
    }
}


