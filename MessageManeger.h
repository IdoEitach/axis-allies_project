#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Axis&alliesBoard.h"


struct Message {
	std::string text;
	float duration; // How long the message should display
	float elapsedTime; // Tracks how long it has been displayed

	Message(const std::string& text, float duration)
		: text(text), duration(duration), elapsedTime(0.0f) {
	}
};
class MessageManeger {
private:
#pragma region ============messages
	std::vector<Message> messages;
	std::vector<Message> debugMessages;
#pragma endregion
	
public:
	MessageManeger();
	void addMessage(const std::string& text, float duration);
	void addDebugMessage(const std::string& text, float duration);
	void drawMessages();
	void drawDebugMessages();
	void clearMessages();
	void clearDebugMessages();
	void updateMessages(float deltaTime);
};


	
