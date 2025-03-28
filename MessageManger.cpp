#include "MessageManeger.h"

MessageManeger::MessageManeger() {
}


/// <summary>
/// this function is adding a message to the message vector
/// </summary>
/// <param name="text"></param>
/// <param name="duration"></param>
void MessageManeger::addMessage(const std::string& text, float duration) {
	messages.emplace_back(text, duration);
}


/// <summary>
/// This function is adding a debug message to the debug message vector
/// </summary>
/// <param name="text"></param>
/// <param name="duration"></param>
void MessageManeger::addDebugMessage(const std::string& text, float duration) {
	debugMessages.emplace_back(Message(text, duration));
}


/// <summary>
/// this function is drawing the messages
/// </summary>
void MessageManeger::drawMessages() {
	for (int i = 0; i < messages.size(); i++) {
		DrawText(messages[i].text.c_str(), 200, 10 + i * 20 + 20, 20, RED);

	}
}


/// <summary>
/// This function is drawing the debug messages
/// </summary>
void MessageManeger::drawDebugMessages() {
	for (int i = 0; i < debugMessages.size(); i++) {
		DrawText(debugMessages[i].text.c_str(), 200, 10 + i * 20, 20, BLACK);
	}
}


/// <summary>
/// This function is clearing the messages from the vector
/// </summary>
void MessageManeger::clearMessages() {
	messages.clear();
}


/// <summary>
/// This function is clearing the debug messages from the vector
/// </summary>
void MessageManeger::clearDebugMessages() {
	debugMessages.clear();
}


/// <summary>
/// This function is updating the messages vector
/// </summary>
/// <param name="deltaTime"></param>
void MessageManeger::updateMessages(float deltaTime) {
	for (int i = 0; i < messages.size(); i++) {
		messages[i].elapsedTime += deltaTime;
		if (messages[i].elapsedTime >= messages[i].duration) {
			messages.erase(messages.begin() + i);
		}
	}
	for (int i = 0; i < debugMessages.size(); i++) {
		debugMessages[i].elapsedTime += deltaTime;
		if (debugMessages[i].elapsedTime >= debugMessages[i].duration) {
			debugMessages.erase(debugMessages.begin() + i);
		}
	}
}
