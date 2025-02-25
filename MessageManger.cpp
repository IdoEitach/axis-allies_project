#include "MessageManeger.h"

MessageManeger::MessageManeger() {
}

void MessageManeger::addMessage(const std::string& text , float duration) {
	messages.emplace_back(text, duration);
}
void MessageManeger::addDebugMessage(const std::string& text , float duration) {
	debugMessages.emplace_back(Message(text, duration));
}
void MessageManeger::drawMessages() {
	for (int i = 0; i < messages.size(); i++) {
		DrawText(messages[i].text.c_str(), 200, 10 + i * 20+20, 20, RED);
		
	}
}
void MessageManeger::drawDebugMessages() {
	for (int i = 0; i < debugMessages.size(); i++) {
		DrawText(debugMessages[i].text.c_str(), 200, 10 + i * 20, 20, BLACK);
	}
}
void MessageManeger::clearMessages() {
	messages.clear();
}
void MessageManeger::clearDebugMessages() {
	debugMessages.clear();
}
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
