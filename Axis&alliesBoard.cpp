#include "Axis&alliesBoard.h"

void RiskBoard::addTerritory(const std::string& territoryName, int owner, int forces, Vector2 position, Color color) {
	Territory territory(territoryName, owner, forces, position, color);
	territories[territoryName] = territory;
	adjacencyList[territoryName] = {};
}
void RiskBoard::addBorder(const std::string& territoryName1, const std::string& territoryName2) {
	adjacencyList[territoryName1].push_back(territoryName2);
	adjacencyList[territoryName2].push_back(territoryName1);
}
void RiskBoard::drawBoard() {
	// Draw background texture scaled to fit the window
	DrawTexturePro(backgroundTexture,
		{ 0, 0, static_cast<float>(backgroundTexture.width), static_cast<float>(backgroundTexture.height) },
		{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
		{ 0, 0 }, 0.0f, WHITE);

	std::string phaseText = "";
	switch (currentPhase)
	{
	case InitializingTerritories:
		phaseText = "Initialization Phase: Place your forces on the territories";
		break;
	case ReinforcingChooseTerritory:
		phaseText = "Reinforcement Phase: choose a territory to add forces to";
		break;
	case ReinforcingAddForces:
		phaseText = "Reinforcement Phase: add forces to the selected territory";
		break;
	case ChooseTerritoryToAttackFrom:
		phaseText = "Attacking Phase: Choose a territory to attack from";
		break;
	case ChooseTerritoryToAttack:
		phaseText = "Attacking Phase: Choose a territory to attack";
		break;
	case MovingForcesFrom:
		phaseText = "Moving Forces Phase: Choose a territory to move forces from";
		break;
	case MovingForcesTo:
		phaseText = "Moving Forces Phase: Choose a territory to move forces to";
		break;
	case EndGame:
		phaseText = "Game Over!";
		break;
	}
	DrawText(phaseText.c_str(), 200, 20, 20, BLACK);

	DrawText(phaseText.c_str(), 200, 20, 20, BLACK);
	// Draw borders (connections) first for visual layering
	for (const auto& adjacencyPair : adjacencyList) {
		const std::string& territoryName = adjacencyPair.first;
		const std::vector<std::string>& neighbors = adjacencyPair.second;
		for (const auto& neighbor : neighbors) {
			DrawLineV(territories.at(territoryName).getPosition(), territories.at(neighbor).getPosition(), BLACK);
		}
	}
	// Draw territories (nodes)
	for (const auto& territoryPair : territories) {
		const Territory& territory = territoryPair.second;
		if (territory.getName() == clickedTerritory->getName()) {
			// Draw selected territory in bold
			DrawCircleV(territory.getPosition(), 25, territory.getColor()); // Larger circle
            DrawText((territory.getName() + " the forces :"+ std::to_string(territory.getForces())).c_str(), static_cast<int>(territory.getPosition().x) - 20, static_cast<int>(territory.getPosition().y) - 10, 20, BLACK);
		}
		else {
			DrawCircleV(territory.getPosition(), 20, territory.getColor());			
            DrawText((territory.getName() + " the forces :" + std::to_string(territory.getForces())).c_str(), static_cast<int>(territory.getPosition().x) - 20, static_cast<int>(territory.getPosition().y) - 10, 15, BLACK);
		}
	}
	if (!clickedTerritory->getName().empty()) {
		DrawText(clickedTerritory->getName().c_str(), 10, 10, 20, BLACK);
	}
}

void RiskBoard::loadTextures() {
	backgroundTexture = LoadTexture("pics/background.png");
	territoryTexture = LoadTexture("pics/territory.png");
}

void RiskBoard::unloadTextures() {
	UnloadTexture(backgroundTexture);
	UnloadTexture(territoryTexture);
}
/// <summary>
/// this function draws the initialization phase
/// </summary>
void RiskBoard::drawInitPhase() {
	drawForcesInfo(); // Draw forces information
	// Draw instructions for the initialization phase
	//DrawText("Initialization Phase: Place your forces on the territories", 200, 20, 20, BLACK);
}
/// <summary>
/// this function draws the reinforcement phase
/// </summary>
void RiskBoard::drawChoosingTerritoryToReinforce() {
	drawForcesInfo(); // Draw forces information

	// Draw instructions for the reinforcement phase
	//DrawText("Reinforcement Phase: Choose a territory to add forces to", 200, 20, 20, BLACK);
}

//getInput
/// <summary>
/// this function is getting the input from the user 
/// its used in the reinforcement phase and in the attacking phase and in the moving forces phase
/// </summary>
/// <param name="clickedTerritoryPtr"></param>
/// <returns></returns>
int RiskBoard::getInput(Territory* clickedTerritoryPtr) {
	drawForcesInfo(); // Draw forces information
	// Draw instructions for the reinforcement phase
	//DrawText("Reinforcement Phase: Add forces to the selected territory", 200, 20, 20, BLACK);
	char inputText[5] = "\0"; // Buffer to store the input text
	int letterCount = 0; // Number of letters in the input text
	bool enterPressed = false; // Flag to check if enter key is pressed
	while (!enterPressed) {

		BeginDrawing();
		ClearBackground(RAYWHITE);

		drawBoard(); // Redraw the board in each iteration
		drawForcesInfo();
		// Draw the selected territory in bold
		const Territory& territory = *clickedTerritoryPtr;
		DrawCircleV(territory.getPosition(), 25, territory.getColor()); // Larger circle
		DrawText(territory.getName().c_str(), static_cast<int>(territory.getPosition().x) - 25, static_cast<int>(territory.getPosition().y) - 15, 25, BLACK); // Larger font

		switch (currentPhase)
		{
		case ReinforcingAddForces:
			DrawText("Enter the amount of forces to add :", 200, 40, 20, BLACK);
			break;
		case ChooseTerritoryToAttackFrom:
			DrawText("Enter the amount of forces to attack with :", 200, 40, 20, BLACK);
			break;
		case ChooseTerritoryToAttack:
			DrawText("Enter the amount of forces to defened with :", 200, 40, 20, BLACK);
			break;
		case MovingForcesFrom:
			DrawText("Enter the amount of forces to move :", 200, 40, 20, BLACK);
			break;
		}
		DrawRectangle(200, 70, 140, 30, LIGHTGRAY);

		DrawText(inputText, 210, 75, 20, BLACK);

		// Check for key input
		if (letterCount < 4) {
			int key = GetCharPressed();
			while (key > 0) {
				if ((key >= 48) && (key <= 57)) {
					inputText[letterCount] = (char)key;
					inputText[letterCount + 1] = '\0';
					letterCount++;
				}
				key = GetCharPressed();
			}
		}
		// Check for backspace
		if (IsKeyPressed(KEY_BACKSPACE)) {
			letterCount--;
			if (letterCount < 0) letterCount = 0;
			inputText[letterCount] = '\0';
		}
		// Check for enter key
		if (IsKeyPressed(KEY_ENTER)) {
			enterPressed = true;
		}
		EndDrawing();
	}
	return atoi(inputText);
}

bool RiskBoard::drawYesNoMessageBox(const std::string& message) {
	Rectangle messageBox = { 200, 200, 400, 200 };
	Rectangle yesButton = { 250, 350, 100, 50 };
	Rectangle noButton = { 450, 350, 100, 50 };

	float fadeDuration = 1.0f; // Duration of the fade effect in seconds
	float elapsedTime = 0.0f;
	float alpha = 0.0f; // Initial alpha value

	while (true) {
		BeginDrawing();
		
		// Calculate the alpha value for the fade effect
		if (elapsedTime < fadeDuration) {
			alpha = elapsedTime / fadeDuration;
		}
		else {
			alpha = 1.0f;
		}

		// Draw the message box with the fade effect
		DrawRectangleRec(messageBox, Fade(LIGHTGRAY, alpha));
		DrawText(message.c_str(), messageBox.x + 20, messageBox.y + 20, 20, Fade(BLACK, alpha));
		DrawRectangleRec(yesButton, Fade(GREEN, alpha));
		DrawText("Yes", yesButton.x + 35, yesButton.y + 10, 20, Fade(BLACK, alpha));
		DrawRectangleRec(noButton, Fade(RED, alpha));
		DrawText("No", noButton.x + 35, noButton.y + 10, 20, Fade(BLACK, alpha));

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mousePoint = GetMousePosition();
			if (CheckCollisionPointRec(mousePoint, yesButton)) {
				EndDrawing();
				return true;
			}
			else if (CheckCollisionPointRec(mousePoint, noButton)) {
				EndDrawing();
				return false;
			}
		}

		EndDrawing();

		// Update the elapsed time
		elapsedTime += GetFrameTime();
	}
}

void RiskBoard::drawChoosingTerritoryToMoveFrom() {
	drawBoard();
	drawForcesInfo(); // Draw forces information
}
/// <summary>
/// this function draws the attacking phase
/// </summary>
void RiskBoard::drawChoosingTerritoryToAttackFrom() {

	drawBoard();
	drawForcesInfo(); // Draw forces information
}

void RiskBoard::drawChoosingTerritoryToAttack() {
	drawBoard();
	drawForcesInfo(); // Draw forces information

}
/// <summary>
/// this function rolls the dice
/// and making the dice animation
/// </summary>
/// <param name="value1"></param>
/// <param name="value2"></param>
void RiskBoard::RollCubes() {

	Camera camera = { 0 };
	camera.position = { 6.0f, 6.0f, 6.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float deltaTime = 0;
	Dice3D dice1Attacker({ -1, 0, 0 });
	Dice3D dice2Attacker({ 1, 0, 0 });

	dice1Attacker.StartRoll();
	dice2Attacker.StartRoll();

	while (dice1Attacker.IsRolling() || dice2Attacker.IsRolling()) {
		deltaTime = GetFrameTime();
		dice1Attacker.Update(deltaTime);
		dice2Attacker.Update(deltaTime);
		BeginDrawing();
		ClearBackground(RED);

		BeginMode3D(camera);
		dice1Attacker.Draw();
		dice2Attacker.Draw();

		EndMode3D();

		EndDrawing();
	}
}
void RiskBoard::setPhase(Phase newPhase) {
	currentPhase = newPhase;
}
Phase RiskBoard::getPhase() const {
	return currentPhase;
}
/// <summary>
/// this function draws the forces information on the screen
/// </summary>
void RiskBoard::drawForcesInfo() {
	int yOffset = 40; // Initial y offset for drawing text
	Color textColor = BLACK;
	for (const auto& territoryPair : territories) {
		textColor = BLACK;
		const Territory& territory = territoryPair.second;
		if (territory.getName() == clickedTerritory->getName())
			textColor = RED; // Highlight the selected territory

		std::string info = territory.getName() + ": " + std::to_string(territory.getForces()) + " forces , owner : " + std::to_string(territory.getOwner());
		DrawText(info.c_str(), 820, yOffset, 15, textColor);
		yOffset += 30; // Increment y offset for the next line
	}
}
/// <summary>
/// this function checks if the user clicked on a territory
/// </summary>
/// <returns></returns>
Territory* RiskBoard::checkClick() {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mousePosition = GetMousePosition();
		std::cout << "\n " << std::endl;
		for (auto& territoryPair : territories) {
			const auto& territoryName = territoryPair.first;
			auto& territory = territoryPair.second;
			if (CheckCollisionPointCircle(mousePosition, territory.getPosition(), 40)) {
				clickedTerritory = &territory;
				// printig log to the console
				std::cout << "the territory is: " << clickedTerritory->getName().c_str() << std::endl;
				for (const auto& neighbor : adjacencyList[clickedTerritory->getName()])
					std::cout << "the neighbor is: " << neighbor << std::endl;
				// end of printing log to the console
				return &territory;
			}
		}
	}
	return nullptr;
}

bool RiskBoard::hasAdjacentEnemies(const std::string& territoryName, int currentPlayer) const {
	auto it = adjacencyList.find(territoryName);
	if (it != adjacencyList.end()) {
		const std::vector<std::string>& neighbors = it->second;
		for (const auto& neighbor : neighbors) {
			const Territory* neighborTerritory = getTerritoryByName(neighbor);

			if (neighborTerritory && neighborTerritory->getOwner() != currentPlayer) {
				return true;
			}
		}
	}
	return false;
}

Territory* RiskBoard::getTerritoryByName(const std::string& territoryName) const {
	auto it = territories.find(territoryName);
	if (it != territories.end()) {
		return const_cast<Territory*>(&(it->second));
	}
	return nullptr;
}

void RiskBoard::displayLoadingScreen() {
	// Load the tank texture
	Texture2D tank = LoadTexture("pics/tank.png"); // Ensure the file path is correct

	// Check if the texture is loaded correctly
	if (tank.id == 0) {
		std::cerr << "Failed to load tank texture!" << std::endl;
		return;
	}

	// Define animation properties
	float tankX = -static_cast<float>(tank.width); // Start off-screen
	float tankY = static_cast<float>(GetScreenHeight()) / 2.0f - static_cast<float>(tank.height) / 2.0f;
	float tankSpeed = 200.0f; // Pixels per second

	// Define the duration of the loading screen
	float loadingDuration = 4.5f; // 3 seconds
	float fadeDuration = 1.0f; // 1 second for fade-in and fade-out
	float elapsedTime = 0.0f;

	while (elapsedTime < loadingDuration + fadeDuration * 2) {
		// Update tank position
		if (elapsedTime > fadeDuration && elapsedTime < loadingDuration + fadeDuration) {
			tankX += tankSpeed * GetFrameTime();
		}

		// Calculate alpha value for fade-in and fade-out
		float alpha = 1.0f;
		if (elapsedTime < fadeDuration) {
			alpha = elapsedTime / fadeDuration;
		}
		else if (elapsedTime > loadingDuration + fadeDuration) {
			alpha = 1.0f - (elapsedTime - (loadingDuration + fadeDuration)) / fadeDuration;
		}

		// Draw the loading screen
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// Draw the moving tank with fade effect
		DrawTexture(tank, static_cast<int>(tankX), static_cast<int>(tankY), Fade(WHITE, alpha));
		DrawText("Loading...", GetScreenWidth() / 2 - MeasureText("Loading...", 20) / 2, static_cast<int>(tankY) + tank.height + 20, 20, Fade(BLACK, alpha));

		EndDrawing();

		// Update elapsed time
		elapsedTime += GetFrameTime();
	}

	// Unload the tank texture
	UnloadTexture(tank);
}

