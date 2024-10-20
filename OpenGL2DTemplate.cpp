#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <sstream> // Include for std::stringstream
#include <iomanip> // Include for std::fixed and std::setprecision
#include <vector>
#include <cstdlib>
#include <cmath>
#include <glut.h>


enum ObstacleType { LOW, HIGH };
static bool canSpawnItem(float thingCenterX, float thingCenterY, float thingWidth);
static int getRandomNumber(int low, int high);


// Window Variables
int screenWidth = 1920;
int screenHeight = 1080;
int windowWidth = 1000;
int windowHeight = 1000;

// Set up the orthographic projection
float orthoWidth = 300.0f;
float orthoHeight = 300.0f;

// Obstacle Variables
float obstacleWidth = 20.0f;
float obstacleSpawnLocation = 400.0f;
float collectableWidth = 20.0f;


// Power up variables
float powerupWidth = 20.0f;
bool isShieldActive = false;
bool isDoublePointsActive = false;
float shieldDuration = 10.0f;
float doublePointsDuration = 10.0f;

// Player variables
float groundLevel = 40.0f;
float playerX = 50.0f, playerY = groundLevel + 10;
bool isJumping = false;
bool isDucking = false;
float jumpHeight = 90.0f;  // Max jump height
float jumpSpeed = 0.0002f;      // Speed of the jump
float duckHeight = 20.0f;    // Height of the player when ducking
float initialPlayerY = playerY; // Store initial Y position for jumping
    
// Define a struct to represent an obstacle
struct Obstacle {
    float centerX, centerY; // Center position of the obstacle
    ObstacleType type;       // Type of the obstacle (high or low)

    // Constructor to initialize an obstacle
    Obstacle(float centerX, float centerY, ObstacleType t)
        : centerX(centerX), centerY(centerY), type(t) {
    }
};

struct Collectable {
    float centerX;
    float centerY;

	Collectable(float centerX, float centerY)
		: centerX(centerX), centerY(centerY) {
	}
};

struct Powerup {
    float centerX;
    float centerY;
    std::string type; // Use std::string for the type

    // Constructor with type parameter
    Powerup(float centerX, float centerY, const std::string& type)
        : centerX(centerX), centerY(centerY), type(type) {
    }
};




// Game variables
int playerLives = 5;
float score = 0;
float timeLeft = 60.0f;
bool isGameOver = false;
float speed = 1.0f;
// Vector to hold all the obstacles
std::vector<Obstacle> obstaclesList;
std::vector<Collectable> collectablesList;
std::vector<Powerup> powerupsList;


static void updatePowerups() {
    powerupsList.erase(
        std::remove_if(powerupsList.begin(), powerupsList.end(), [](const Powerup& powerup) {
            return powerup.centerX < -obstacleWidth; // Check if the collectible is out of bounds
            }),
        powerupsList.end() // Corrected to remove from collectablesList
    );

    // Shift the remaining collectibles
    for (Powerup& powerup: powerupsList) {
        powerup.centerX -= 1.0f * speed; // Shift the center by speed units
    }
}

static void spawnPowerup() {
    // Randomly choose between 0 and 1
    int randomChoice = getRandomNumber(1, 2);

	//printf("Random Choice: %d\n", randomChoice);

    // Create a new obstacle based on the random choice

    float centerY = getRandomNumber(60, 150);

	Powerup newPowerup = randomChoice == 1 ? Powerup(obstacleSpawnLocation, centerY, "shield") : Powerup(obstacleSpawnLocation, centerY, "doublePoints");

    if (canSpawnItem(newPowerup.centerX, newPowerup.centerY, powerupWidth)) {
        powerupsList.push_back(newPowerup);
    }
}

static void drawShield(float centerX, float centerY) {
    float color[3] = { 0.0f, 0.5f, 1.0f }; // Color (blue)

    // Draw a rectangle (as the base)
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_QUADS);
    glVertex2f(centerX - collectableWidth / 2, centerY - collectableWidth / 2); // Bottom left
    glVertex2f(centerX + collectableWidth / 2, centerY - collectableWidth / 2); // Bottom right
    glVertex2f(centerX + collectableWidth / 2, centerY + collectableWidth / 2); // Top right
    glVertex2f(centerX - collectableWidth / 2, centerY + collectableWidth / 2); // Top left
    glEnd();

    // Draw a circle (inside the rectangle)
    int numSegments = 30;
    float radius = collectableWidth / 2; // Radius of the circle
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the circle
    glBegin(GL_POLYGON); // Start a polygon
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * 3.14159265358979323846f * float(i) / float(numSegments); // Current angle
        float dx = radius * cos(angle); // X component
        float dy = radius * sin(angle); // Y component
        glVertex2f(centerX + dx, centerY + dy); // Vertex on the circumference
    }
    glEnd();

    // Draw a square inside the circle
    float squareSize = radius * 0.7f; // Size of the square (70% of the circle's radius)
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for the square
    glBegin(GL_QUADS);
    glVertex2f(centerX - squareSize / 2, centerY - squareSize / 2); // Bottom left
    glVertex2f(centerX + squareSize / 2, centerY - squareSize / 2); // Bottom right
    glVertex2f(centerX + squareSize / 2, centerY + squareSize / 2); // Top right
    glVertex2f(centerX - squareSize / 2, centerY + squareSize / 2); // Top left
    glEnd();

    // Draw a triangle inside the square
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX, centerY + squareSize / 3); // Top vertex
    glVertex2f(centerX - squareSize / 2.5f, centerY - squareSize / 2.5f); // Bottom left vertex
    glVertex2f(centerX + squareSize / 2.5f, centerY - squareSize / 2.5f); // Bottom right vertex
    glEnd();
}



static void drawDoublePoints(float centerX, float centerY) {
    float color[3] = { 1.0f, 0.5f, 0.0f }; // Color (orange)

    // Draw a rectangle (as the base)
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_QUADS);
    glVertex2f(centerX - collectableWidth / 2, centerY - collectableWidth / 2); // Bottom left
    glVertex2f(centerX + collectableWidth / 2, centerY - collectableWidth / 2); // Bottom right
    glVertex2f(centerX + collectableWidth / 2, centerY + collectableWidth / 2); // Top right
    glVertex2f(centerX - collectableWidth / 2, centerY + collectableWidth / 2); // Top left
    glEnd();

    // Draw a circle (inside the rectangle)
    int numSegments = 30;
    float radius = collectableWidth / 2; // Radius of the circle
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the circle
    glBegin(GL_POLYGON); // Start a polygon
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * 3.14159265358979323846f * float(i) / float(numSegments); // Current angle
        float dx = radius * cos(angle); // X component
        float dy = radius * sin(angle); // Y component
        glVertex2f(centerX + dx, centerY + dy); // Vertex on the circumference
    }
    glEnd();

    // Draw a square inside the circle
    float squareSize = radius * 0.7f; // Size of the square (70% of the circle's radius)
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for the square
    glBegin(GL_QUADS);
    glVertex2f(centerX - squareSize / 2, centerY - squareSize / 2); // Bottom left
    glVertex2f(centerX + squareSize / 2, centerY - squareSize / 2); // Bottom right
    glVertex2f(centerX + squareSize / 2, centerY + squareSize / 2); // Top right
    glVertex2f(centerX - squareSize / 2, centerY + squareSize / 2); // Top left
    glEnd();

    // Draw a triangle inside the square
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX, centerY + squareSize / 3); // Top vertex
    glVertex2f(centerX - squareSize / 2.5f, centerY - squareSize / 2.5f); // Bottom left vertex
    glVertex2f(centerX + squareSize / 2.5f, centerY - squareSize / 2.5f); // Bottom right vertex
    glEnd();
}



static void drawPowerups() {
    for (const Powerup& powerup : powerupsList) {
        if (powerup.type == "shield") {
            drawShield(powerup.centerX, powerup.centerY);
        }
        else if (powerup.type == "doublePoints") {
            drawDoublePoints(powerup.centerX, powerup.centerY);
        }
    }
}



static void updateCollectables() {
    collectablesList.erase(
        std::remove_if(collectablesList.begin(), collectablesList.end(), [](const Collectable& col) {
            return col.centerX < -obstacleWidth; // Check if the collectible is out of bounds
            }),
        collectablesList.end() // Corrected to remove from collectablesList
    );

    // Shift the remaining collectibles
    for (Collectable& col : collectablesList) {
        col.centerX -= 1.0f * speed; // Shift the center by speed units
    }
}

static void spawnCollectable() {

    // Create a new obstacle based on the random choice

    float centerY = getRandomNumber(60, 150);
    Collectable newCollectable = Collectable(obstacleSpawnLocation, centerY);

    if (canSpawnItem(newCollectable.centerX, newCollectable.centerY, collectableWidth)) {
        collectablesList.push_back(newCollectable);
    }
}

static void drawCollectable(float centerX, float centerY) {
    float color[3] = { 1.0f, 0.8f, 0.0f }; // Color (yellow)

    // Draw a rectangle (as the base)
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_QUADS);
    glVertex2f(centerX - collectableWidth / 2, centerY - collectableWidth / 2); // Bottom left
    glVertex2f(centerX + collectableWidth / 2, centerY - collectableWidth / 2); // Bottom right
    glVertex2f(centerX + collectableWidth / 2, centerY + collectableWidth / 2); // Top right
    glVertex2f(centerX - collectableWidth / 2, centerY + collectableWidth / 2); // Top left
    glEnd();

	
    int numSegments = 30;
	float radius = collectableWidth / 2; // Radius of the circle

    glColor3f(1.0f, 1.0f, 1.0f); // White color for the circle
    glBegin(GL_POLYGON); // Start a polygon
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * 3.14159265358979323846f * float(i) / float(numSegments); // Current angle
        float dx = radius * cos(angle); // X component
        float dy = radius * sin(angle); // Y component
        glVertex2f(centerX + dx, centerY + dy); // Vertex on the circumference
    }
    glEnd();

    // Draw a triangle (on top of the rectangle)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX, centerY + collectableWidth / 5); // Top vertex
    glVertex2f(centerX - collectableWidth / 3, centerY - collectableWidth / 5); // Bottom left vertex
    glVertex2f(centerX + collectableWidth / 3, centerY - collectableWidth / 5); // Bottom right vertex
    glEnd();

}



static void drawCollectables() {
	for (const Collectable& col : collectablesList) {
		drawCollectable(col.centerX, col.centerY);
	}
}

// Function to render a low obstacle with a rectangle and triangle (spike)
static void drawLowObstacle(float centerX) {
    // Render the rectangle for the low obstacle
    glColor3f(0.0f, 1.0f, 0.0f);  // Set the color to green for low obstacles
    glBegin(GL_QUADS);  // Render the obstacle as a rectangle
    glVertex2f(centerX - obstacleWidth / 2, 40); // Bottom-left corner
    glVertex2f(centerX + obstacleWidth / 2, 40); // Bottom-right corner
    glVertex2f(centerX + obstacleWidth / 2, 60); // Top-right corner
    glVertex2f(centerX - obstacleWidth / 2, 60); // Top-left corner
    glEnd();

    // Render the triangle (spike) on top of the low obstacle
    glColor3f(0.8f, 1.0f, 0.5f); 
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - obstacleWidth / 2, 50); // Left corner of the rectangle
    glVertex2f(centerX + obstacleWidth / 2, 50); // Right corner of the rectangle
    glVertex2f(centerX, 60); // Top point of the triangle (spike)
    glEnd();
}

// Function to render a high obstacle with a rectangle and triangle (spike)
static void drawHighObstacle(float centerX) {
    // Render the rectangle for the high obstacle
    glColor3f(1.0f, 0.0f, 0.0f);  // Set the color to red for high obstacles
    glBegin(GL_QUADS);  // Render the obstacle as a rectangle
    glVertex2f(centerX - obstacleWidth / 2, 100); // Bottom-left corner
    glVertex2f(centerX + obstacleWidth / 2, 100); // Bottom-right corner
    glVertex2f(centerX + obstacleWidth / 2, 260); // Top-right corner
    glVertex2f(centerX - obstacleWidth / 2, 260); // Top-left corner
    glEnd();

    // Render the triangle (spike) on top of the high obstacle
    glColor3f(1.0f, 0.5f, 0.5f); 
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - obstacleWidth / 2, 110); // Left corner of the rectangle
    glVertex2f(centerX + obstacleWidth / 2, 110); // Right corner of the rectangle
    glVertex2f(centerX, 100); // Top point of the triangle (spike)
    glEnd();
}


static int getRandomNumber(int low, int high) {
    return rand() % (high - low + 1) + low; // Generates a number between low and high inclusive
}


static bool canSpawnItem(float thingCenterX, float thingCenterY, float thingWidth) {
    for (const Obstacle& obs : obstaclesList) {

        float randomCoefficient = getRandomNumber(5, 10);

        //printf("Random Coefficient: %f\n", randomCoefficient);

        // Calculate minimumObstacleSpacing
        float minimumObstacleSpacing = thingWidth * randomCoefficient;

        /*  minimumObstacleSpacing += obstacleWidth;*/

        if (fabs(thingCenterX - obs.centerX) < minimumObstacleSpacing) {
            return false;
        }
    }

    for (const Collectable& col : collectablesList) {

        float randomCoefficient = getRandomNumber(5, 10);

        //printf("Random Coefficient: %f\n", randomCoefficient);

        // Calculate minimumObstacleSpacing
        float minimumObstacleSpacing = thingWidth * randomCoefficient;

        /*  minimumObstacleSpacing += obstacleWidth;*/

        if (fabs(thingCenterX - col.centerX) < minimumObstacleSpacing) {
            return false;
        }
    }

	for (const Powerup& powerup : powerupsList) {

		float randomCoefficient = getRandomNumber(5, 10);

		//printf("Random Coefficient: %f\n", randomCoefficient);

		// Calculate minimumObstacleSpacing
		float minimumObstacleSpacing = thingWidth * randomCoefficient;

		/*  minimumObstacleSpacing += obstacleWidth;*/

		if (fabs(thingCenterX - powerup.centerX) < minimumObstacleSpacing) {
			return false;
		}
	}



    return true;
}

static void spawnObstacle() {
    // Randomly choose between 0 and 1
    int randomChoice = rand() % 2; // 0 for low, 1 for high

    // Create a new obstacle based on the random choice
    Obstacle newObstacle = (randomChoice == 0)
        ? Obstacle(obstacleSpawnLocation, 50.0f, LOW)  // Low obstacle
        : Obstacle(obstacleSpawnLocation, 230.0f, HIGH); // High obstacle

    if (canSpawnItem(newObstacle.centerX, newObstacle.centerY, obstacleWidth)) {
        obstaclesList.push_back(newObstacle);
    }
}




// Function to render all obstacles
static void drawObstacles() {
    for (const Obstacle& obs : obstaclesList) {
        if (obs.type == LOW) {
            drawLowObstacle(obs.centerX);
        }
        else if (obs.type == HIGH) {
            drawHighObstacle(obs.centerX);
        }
    }
}

static void updateObstacles() {
    // Use a lambda to remove elements from the list while iterating
    obstaclesList.erase(
        std::remove_if(obstaclesList.begin(), obstaclesList.end(), [](const Obstacle& obs) {
            return obs.centerX < -obstacleWidth; // Check if the obstacle is out of bounds
            }),
        obstaclesList.end()
    );

    // Shift the remaining obstacles
    for (Obstacle& obs : obstaclesList) {
        obs.centerX -= 1.0f * speed; // Shift the center by 10.0 units
    }
}





static void drawText(float x, float y, const std::string& text) {
    // Calculate text width and height
    int textWidth = text.length() * 4; // Approximate width per character (adjust if needed)
    int textHeight = 40; // Height of the font

    // Draw background rectangle
    glColor3f(0.0f, 0.0f, 0.0f); // Black background
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y - 5); // Bottom left
    glVertex2f(x + textWidth - 5, y - 5); // Bottom right
    glVertex2f(x + textWidth - 5, y + textHeight ); // Top right
    glVertex2f(x - 5, y + textHeight); // Top left
    glEnd();

    // Set text color
    glColor3f(1.0f, 1.0f, 1.0f); // White text

    // Draw the text in the original position without offset
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c); // Render the text normally
    }
}


// Draw Lives on screen
static void drawLives() {
    std::stringstream ss;
    ss << "Lives: " << playerLives; // Concatenate the lives into a string
    drawText(10.0f, 290.0f, ss.str()); // Call drawText with the formatted string
}

// Draw Score on screen
static void drawScore() {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "Score: " << score; // Format the score
    drawText(260.0f, 290.0f, ss.str()); // Call drawText with the formatted string
}

// Draw Time on screen
static void drawTime() {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "Time: " << timeLeft; // Format the time
    drawText(130.0f, 290.0f, ss.str()); // Call drawText with the formatted string
}

// Draw the top boundary (ceiling and decorations)
static void drawTopBoundary() {
    // Draw the ceiling as a black rectangle
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for the ceiling
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 300.0f);     // Top-left corner
    glVertex2f(300.0f, 300.0f);   // Top-right corner
    glVertex2f(300.0f, 260.0f);   // Bottom-right corner
    glVertex2f(0.0f, 260.0f);      // Bottom-left corner
    glEnd();

    // Draw the spikes below the ceiling
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray for the spikes
    float noTriangles = 15;
    float baseWidth = 300.0f / noTriangles;

    for (int i = 0; i < noTriangles; ++i) {
        glBegin(GL_TRIANGLES);
        // Left vertex of the triangle
        glVertex2f(i * baseWidth, 300.0f);       // Top-left corner
        glVertex2f((i + 1) * baseWidth, 300.0f); // Top-right corner
        glVertex2f((i + 0.5f) * baseWidth, 260.0f); // Bottom vertex
        glEnd();
    }
}




// Draw the bottom boundary (ground, water, and decorations)
static void drawBottomBoundary() {
    // Draw the water section
    glColor3f(0.0f, 0.5f, 1.0f); // Blue color for water
    glRectf(0.0f, 0.0f, 300.0f, 15.0f);  // Water section (height reduced)

    // Draw the brown section (land) above the water
    glColor3f(0.4f, 0.26f, 0.13f); // Brownish color for the ground
    glRectf(0.0f, 15.0f, 300.0f, 40.0f);  // Land section

    // Draw rocks on the land (adjusted to match the land level)
    glColor3f(0.5f, 0.5f, 0.5f); // Grey color for rocks
    glBegin(GL_TRIANGLES);
    glVertex2f(50.0f, 15.0f);  // Rock 1 - Base left (aligned with land)
    glVertex2f(75.0f, 35.0f);  // Rock 1 - Top
    glVertex2f(100.0f, 15.0f); // Rock 1 - Base right

    glVertex2f(200.0f, 15.0f); // Rock 2 - Base left (aligned with land)
    glVertex2f(225.0f, 35.0f); // Rock 2 - Top
    glVertex2f(250.0f, 15.0f); // Rock 2 - Base right
    glEnd();

    // Draw grass on the land (adjusted to match the land level)
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for grass
    glBegin(GL_TRIANGLES);
    glVertex2f(10.0f, 15.0f);  // Grass 1 - Base left (aligned with land)
    glVertex2f(15.0f, 35.0f);  // Grass 1 - Top
    glVertex2f(20.0f, 15.0f);  // Grass 1 - Base right

    glVertex2f(30.0f, 15.0f);  // Grass 2 - Base left (aligned with land)
    glVertex2f(35.0f, 35.0f);  // Grass 2 - Top
    glVertex2f(40.0f, 15.0f);  // Grass 2 - Base right

    glVertex2f(110.0f, 15.0f);  // Grass 3 - Base left (aligned with land)
    glVertex2f(115.0f, 35.0f);  // Grass 3 - Top
    glVertex2f(120.0f, 15.0f);  // Grass 3 - Base right

    glVertex2f(145.0f, 15.0f); // Grass 4 - Base left (aligned with land)
    glVertex2f(150.0f, 35.0f); // Grass 4 - Top
    glVertex2f(155.0f, 15.0f); // Grass 4 - Base right

    glVertex2f(175.0f, 15.0f); // Grass 5 - Base left (aligned with land)
    glVertex2f(180.0f, 35.0f); // Grass 5 - Top
    glVertex2f(185.0f, 15.0f); // Grass 5 - Base right

    glVertex2f(270.0f, 15.0f); // Grass 6 - Base left (aligned with land)
    glVertex2f(275.0f, 35.0f); // Grass 6 - Top
    glVertex2f(280.0f, 15.0f); // Grass 6 - Base right
    glEnd();
}

static void updatePlayer() {
    /*printf("Initial Player Y: %f\n", initialPlayerY);
	printf("Player Y: %f\n", playerY);*/

    // Jumping logic
    if (isJumping) {
        // Move player up
        playerY += jumpSpeed;

        // Check if the player has reached the peak of the jump
        if (playerY >= initialPlayerY + jumpHeight) {
            isJumping = false; // Start falling down
        }
    }
    else {
        // Apply gravity when not jumping
        if (playerY > initialPlayerY) {
            playerY -= jumpSpeed; // Move down
        }
        else {
            playerY = initialPlayerY; // Reset to ground level
        }
    }
}

static void drawPlayer() {
    // Adjust height based on ducking state
    float playerHeight = isDucking ? 40.0f - duckHeight : 40.0f;


    // Body (taller and narrower rectangle)
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue shirt
    glBegin(GL_QUADS);
    glVertex2f(playerX - 5.0f, playerY); // Bottom-left (narrower width)
    glVertex2f(playerX + 5.0f, playerY); // Bottom-right (narrower width)
    glVertex2f(playerX + 5.0f, playerY + playerHeight); // Top-right
    glVertex2f(playerX - 5.0f, playerY + playerHeight); // Top-left
    glEnd();


    // Head (polygon approximation for the circle)
    glColor3f(1.0f, 0.8f, 0.6f); // Skin color
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; ++i) {
        float angle = 2.0f * 3.14159f * i / 20;
        glVertex2f(playerX + cos(angle) * 10.0f, playerY + playerHeight + sin(angle) * 10.0f);  // Head at top
    }
    glEnd();

    // Arms (lines, tilted)
    glColor3f(1.0f, 0.8f, 0.6f);  // Skin color
    glBegin(GL_LINES);
    // Left arm (tilted downwards)
    glVertex2f(playerX - 5.0f, playerY + playerHeight - 10.0f); // Shoulder (attached to body)
    glVertex2f(playerX - 15.0f, playerY + playerHeight - 20.0f); // Hand (tilted downwards)

    // Right arm (tilted downwards)
    glVertex2f(playerX + 5.0f, playerY + playerHeight - 10.0f); // Shoulder (attached to body)
    glVertex2f(playerX + 15.0f, playerY + playerHeight - 20.0f); // Hand (tilted downwards)
    glEnd();

    // Legs (lines, tilted)
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue pants
    glBegin(GL_LINES);
    // Left leg (tilted outwards)
    glVertex2f(playerX - 3.0f, playerY); // Hip
    glVertex2f(playerX - 10.0f, playerY - 10); // Foot (tilted outwards)

    // Right leg (tilted outwards)
    glVertex2f(playerX + 3.0f, playerY); // Hip
    glVertex2f(playerX + 10.0f, playerY - 10); // Foot (tilted outwards)
    glEnd();

    // Spiky hair (triangles above the head)
    glColor3f(1.0f, 0.0f, 0.0f); // Red hair
    glBegin(GL_TRIANGLES);

    // Left spike
    glVertex2f(playerX - 10.0f, playerY + playerHeight + 10.0f);  // Left base point
    glVertex2f(playerX - 4.0f, playerY + playerHeight);  // Base center
    glVertex2f(playerX - 2.0f, playerY + playerHeight + 20.0f);  // Tip of the left spike

    // Middle spike
    glVertex2f(playerX - 2.0f, playerY + playerHeight + 10.0f);  // Left base point
    glVertex2f(playerX + 2.0f, playerY + playerHeight + 10.0f);  // Right base point
    glVertex2f(playerX, playerY + playerHeight + 15.0f);         // Tip of the middle spike

    // Right spike
    glVertex2f(playerX + 2.0f, playerY + playerHeight + 20.0f);  // Tip of the right spike
    glVertex2f(playerX + 4.0f, playerY + playerHeight);  // Base center
    glVertex2f(playerX + 10.0f, playerY + playerHeight + 10.0f);  // Right base point
    glEnd();
}

//static void drawPlayer() {
//    // Head (polygon approximation for the circle)
//    glColor3f(1.0f, 0.8f, 0.6f); // Skin color
//    glBegin(GL_POLYGON);
//    for (int i = 0; i <= 20; ++i) {
//        float angle = 2.0f * 3.14159f * i / 20;
//        glVertex2f(playerX + cos(angle) * 10.0f, playerY + 50.0f + sin(angle) * 10.0f);  // Head at top
//    }
//    glEnd();
//
//    // Body (taller and narrower rectangle)
//    glColor3f(0.0f, 0.0f, 1.0f);  // Blue shirt
//    glBegin(GL_QUADS);
//    glVertex2f(playerX - 5.0f, playerY + 10.0f); // Bottom-left (narrower width)
//    glVertex2f(playerX + 5.0f, playerY + 10.0f); // Bottom-right (narrower width)
//    glVertex2f(playerX + 5.0f, playerY + 40.0f); // Top-right
//    glVertex2f(playerX - 5.0f, playerY + 40.0f); // Top-left
//    glEnd();
//
//    // Arms (lines, tilted)
//    glColor3f(1.0f, 0.8f, 0.6f);  // Skin color
//    glBegin(GL_LINES);
//    // Left arm (tilted downwards)
//    glVertex2f(playerX - 5.0f, playerY + 30.0f); // Shoulder (attached to body)
//    glVertex2f(playerX - 15.0f, playerY + 20.0f); // Hand (tilted downwards)
//
//    // Right arm (tilted downwards)
//    glVertex2f(playerX + 5.0f, playerY + 30.0f); // Shoulder (attached to body)
//    glVertex2f(playerX + 15.0f, playerY + 20.0f); // Hand (tilted downwards)
//    glEnd();
//
//    // Legs (lines, tilted)
//    glColor3f(0.0f, 0.0f, 1.0f);  // Blue pants
//    glBegin(GL_LINES);
//    // Left leg (tilted outwards)
//    glVertex2f(playerX - 3.0f, playerY + 10.0f); // Hip
//    glVertex2f(playerX - 10.0f, playerY - 10.0f); // Foot (tilted outwards)
//
//    // Right leg (tilted outwards)
//    glVertex2f(playerX + 3.0f, playerY + 10.0f); // Hip
//    glVertex2f(playerX + 10.0f, playerY - 10.0f); // Foot (tilted outwards)
//    glEnd();
//
//    // Spiky hair (triangles above the head)
//    glColor3f(1.0f, 0.0f, 0.0f); // Red hair
//    glBegin(GL_TRIANGLES);
//
//    // Left spike
//    glVertex2f(playerX - 10.0f, playerY + 60.0f);  // Left base point
//    glVertex2f(playerX - 4.0f, playerY + 50.0f);  // Base center
//    glVertex2f(playerX - 2.0f, playerY + 70.0f);  // Tip of the left spike
//
//    // Middle spike
//    glVertex2f(playerX - 2.0f, playerY + 60.0f);  // Left base point
//    glVertex2f(playerX + 2.0f, playerY + 60.0f);  // Right base point
//    glVertex2f(playerX, playerY + 75.0f);         // Tip of the middle spike
//
//    // Right spike
//    glVertex2f(playerX + 2.0f, playerY + 70.0f);  // Tip of the right spike
//    glVertex2f(playerX + 4.0f, playerY + 50.0f);  // Base center
//    glVertex2f(playerX + 10.0f, playerY + 60.0f);  // Right base point
//    glEnd();
//}


// Keyboard handling
static void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') { // Space key for jumping
        if (!isJumping && playerY <= initialPlayerY) { // Can jump only if on the ground
            isJumping = true;
        }
    }
    if (key == 's') { // 's' key for ducking
        isDucking = true;
    }
}

static void keyboardUp(unsigned char key, int x, int y) {
    if (key == 's') { // Release 's' key to stand up
        isDucking = false;
    }
}

static void drawImaginaryBoundary() {
    glColor3f(1.0f, 1.0f, 0.0f); // Set the color to yellow (RGB: 1, 1, 0)

    glBegin(GL_LINES);  // Begin drawing lines
    glVertex2f(65.0f, 0.0f);   // Starting point of the line at (65, 0)
    glVertex2f(65.0f, 300.0f); // End point of the line at (65, 300)
    glEnd();  // End drawing lines
}


//// Draw Collectable
//static void drawCollectable() {
//    glColor3f(0.0f, 0.0f, 1.0f);
//    glBegin(GL_TRIANGLE_FAN);
//    for (int i = 0; i <= 20; ++i) {
//        float angle = 2.0f * 3.14159f * i / 20;
//        glVertex2f(collectableX + cos(angle) * 10.0f, collectableY + sin(angle) * 10.0f);
//    }
//    glEnd();
//}

//// Draw PowerUp
//static void drawPowerUp() {
//    glColor3f(1.0f, 1.0f, 0.0f);
//    glBegin(GL_TRIANGLE_FAN);
//    for (int i = 0; i <= 20; ++i) {
//        float angle = 2.0f * 3.14159f * i / 20;
//        glVertex2f(powerupX + cos(angle) * 10.0f, powerupY + sin(angle) * 10.0f);
//    }
//    glEnd();
//}

// Initialize the game
static void initGame() {
    srand(time(0));
    playerX = 50.0f;
    playerY = groundLevel;
    playerLives = 3;
    score = 0;
    timeLeft = 60.0f;
    isGameOver = false;


}

// Update player, obstacles, collectables, and power-ups
static void updateGame(float deltaTime) {
    if (isGameOver) return;

    // Update player logic (currently static)

    //// Check if player collides with obstacles
    //if (fabs(playerX - obstacleX) < 20 && fabs(playerY - obstacleY) < 20) {
    //    playerLives--;
    //    if (playerLives == 0) {
    //        isGameOver = true;
    //    }
    //    obstacleX = rand() % 300;
    //    obstacleY = rand() % 300;
    //}

    //// Check if player collects collectables
    //if (fabs(playerX - collectableX) < 20 && fabs(playerY - collectableY) < 20) {
    //    score += 10;
    //    collectableX = rand() % 300;
    //    collectableY = rand() % 300;
    //}

    //// Check if player gets a power-up
    //if (fabs(playerX - powerupX) < 20 && fabs(playerY - powerupY) < 20) {
    //    playerLives++;
    //    powerupX = rand() % 300;
    //    powerupY = rand() % 300;
    //}

    // Decrease time left
    timeLeft -= deltaTime;
	//score = score + (deltaTime * speed * 10);
    if (timeLeft <= 0) {
        isGameOver = true;
    }
    speed += 0.001;
}

static void handleCollisions() {
	// Check if player collides with obstacles
	for (Obstacle& obs : obstaclesList) {
		if (fabs(playerX - obs.centerX) < 20 && fabs(playerY - obs.centerY) < 20) {
			playerLives--;
			if (playerLives == 0) {
				isGameOver = true;
			}
			obs.centerX = rand() % 300;
			obs.centerY = rand() % 300;
		}
	}

	// Check if player collects collectables
	for (Collectable& col : collectablesList) {
		if (fabs(playerX - col.centerX) < 20 && fabs(playerY - col.centerY) < 20) {
			score += 10;
			col.centerX = rand() % 300;
			col.centerY = rand() % 300;
		}
	}

	// Check if player gets a power-up
	for (Powerup& powerup : powerupsList) {
		if (fabs(playerX - powerup.centerX) < 20 && fabs(playerY - powerup.centerY) < 20) {
			playerLives++;
			powerup.centerX = rand() % 300;
			powerup.centerY = rand() % 300;
		}
	}


// Render the game
static void renderGame() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the environment (ground and sky)
    drawTopBoundary();
    drawBottomBoundary();

    // Draw player, obstacle, collectable, and power-up
    drawPlayer();

    //drawObstacle();
    //drawCollectable();
    //drawPowerUp();

    // Draw game stats
    drawLives();
    drawScore();
    drawTime();
    updateObstacles();
	spawnObstacle();

	updateCollectables();
	spawnCollectable();

    updatePowerups();
	spawnPowerup();

    drawObstacles();
	drawCollectables();
	drawPowerups();


    // Imaginary Boundary
    drawImaginaryBoundary();

    // Flush the OpenGL commands
    glFlush();
}

// Display function for GLUT
static void Display() {
    if (isGameOver) {
        glClear(GL_COLOR_BUFFER_BIT);
        std::string message;  

        if (playerLives <= 0) {
            message = "Game Over! Score : " + std::to_string(score);
        }
		else {
			message = "Game End! You won with Score: " + std::to_string(score);

        }

		drawText(100.0f, 150.0f, message.c_str());
        glFlush();

    }
    else{
        renderGame();
    }
}

// Main loop
void mainLoop(int value) {
    static float previousTime = 0.0f;
    float currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f; // Convert milliseconds to seconds
    float deltaTime = currentTime - previousTime; // Calculate time since last frame
    previousTime = currentTime;
    
    // Update the game state
    updateGame(deltaTime);

    // Redisplay the game
    glutPostRedisplay();
    glutTimerFunc(16, mainLoop, 0); // Call this function again after 16 milliseconds (approx. 60 FPS)
}

// Initialize OpenGL and GLUT
static void initOpenGL() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // Set the clear color to black
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, orthoWidth, 0, orthoHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}






int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("2D Infinite Runner Game");

    initOpenGL();
	srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
    initGame();

    // Test object spawns
    //initObstacles();

    glutDisplayFunc(Display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIdleFunc(updatePlayer); // Call update in idle state

    glutTimerFunc(0, mainLoop, 0); // Start the main loop

    glutMainLoop(); // Enter the GLUT event loop
    return 0;
}
