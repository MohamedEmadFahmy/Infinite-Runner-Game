#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <sstream> // Include for std::stringstream
#include <iomanip> // Include for std::fixed and std::setprecision
#include <glut.h>

// Window Variables
int screenWidth = 1920;
int screenHeight = 1080;
int windowWidth = 1000;
int windowHeight = 1000;

// Set up the orthographic projection
float orthoWidth = 300.0f;
float orthoHeight = 300.0f;

// Game variables
float playerX = 50.0f, playerY = 40.0f;
int playerLives = 5;
float obstacleX = 200.0f, obstacleY = 100.0f;
float collectableX = 250.0f, collectableY = 150.0f;
float powerupX = 300.0f, powerupY = 150.0f;
float score = 0;
float timeLeft = 60.0f;
bool isGameOver = false;

static void drawText(float x, float y, const std::string& text) {
    // Calculate text width and height
    int textWidth = text.length() * 4; // Approximate width per character (adjust if needed)
    int textHeight = 40; // Height of the font

    // Draw background rectangle
    glColor3f(0.0f, 0.0f, 0.0f); // Black background
    glBegin(GL_QUADS);
    glVertex2f(x, y); // Bottom left
    glVertex2f(x + textWidth, y); // Bottom right
    glVertex2f(x + textWidth , y + textHeight ); // Top right
    glVertex2f(x, y + textHeight); // Top left
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
    // Draw the ceiling using 6 triangles
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray for the triangles

    // Each triangle has a base width of 50 (300 / 6 = 50)
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


// Draw Player as a human-shaped figure using 4 different primitives
static void drawPlayer() {
    // Head (polygon approximation for the circle)
    glColor3f(1.0f, 0.8f, 0.6f); // Skin color
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; ++i) {
        float angle = 2.0f * 3.14159f * i / 20;
        glVertex2f(playerX + cos(angle) * 10.0f, playerY + 50.0f + sin(angle) * 10.0f);  // Head at top
    }
    glEnd();

    // Body (taller and narrower rectangle)
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue shirt
    glBegin(GL_QUADS);
    glVertex2f(playerX - 5.0f, playerY + 10.0f); // Bottom-left (narrower width)
    glVertex2f(playerX + 5.0f, playerY + 10.0f); // Bottom-right (narrower width)
    glVertex2f(playerX + 5.0f, playerY + 40.0f); // Top-right
    glVertex2f(playerX - 5.0f, playerY + 40.0f); // Top-left
    glEnd();

    // Arms (lines, tilted)
    glColor3f(1.0f, 0.8f, 0.6f);  // Skin color
    glBegin(GL_LINES);
    // Left arm (tilted downwards)
    glVertex2f(playerX - 5.0f, playerY + 30.0f); // Shoulder (attached to body)
    glVertex2f(playerX - 15.0f, playerY + 20.0f); // Hand (tilted downwards)

    // Right arm (tilted downwards)
    glVertex2f(playerX + 5.0f, playerY + 30.0f); // Shoulder (attached to body)
    glVertex2f(playerX + 15.0f, playerY + 20.0f); // Hand (tilted downwards)
    glEnd();

    // Legs (lines, tilted)
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue pants
    glBegin(GL_LINES);
    // Left leg (tilted outwards)
    glVertex2f(playerX - 3.0f, playerY + 10.0f); // Hip
    glVertex2f(playerX - 10.0f, playerY - 10.0f); // Foot (tilted outwards)

    // Right leg (tilted outwards)
    glVertex2f(playerX + 3.0f, playerY + 10.0f); // Hip
    glVertex2f(playerX + 10.0f, playerY - 10.0f); // Foot (tilted outwards)
    glEnd();

    // Spiky hair (triangles above the head)
    glColor3f(1.0f, 0.0f, 0.0f); // Red hair
    glBegin(GL_TRIANGLES);

    // Left spike
    glVertex2f(playerX - 10.0f, playerY + 60.0f);  // Left base point
    glVertex2f(playerX - 4.0f, playerY + 50.0f);  // Base center
    glVertex2f(playerX - 2.0f, playerY + 70.0f);  // Tip of the left spike

    // Middle spike
    glVertex2f(playerX - 2.0f, playerY + 60.0f);  // Left base point
    glVertex2f(playerX + 2.0f, playerY + 60.0f);  // Right base point
    glVertex2f(playerX, playerY + 75.0f);         // Tip of the middle spike

    // Right spike
    glVertex2f(playerX + 2.0f, playerY + 70.0f);  // Tip of the right spike
    glVertex2f(playerX + 4.0f, playerY + 50.0f);  // Base center
    glVertex2f(playerX + 10.0f, playerY + 60.0f);  // Right base point
    glEnd();
}





// Draw Obstacle
static void drawObstacle() {
    glColor3f(0.0f, 1.0f, 0.0f);
    glRectf(obstacleX - 10.0f, obstacleY - 10.0f, obstacleX + 10.0f, obstacleY + 10.0f);  // Green square for obstacle
}

// Draw Collectable
static void drawCollectable() {
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 20; ++i) {
        float angle = 2.0f * 3.14159f * i / 20;
        glVertex2f(collectableX + cos(angle) * 10.0f, collectableY + sin(angle) * 10.0f);
    }
    glEnd();
}

// Draw PowerUp
static void drawPowerUp() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 20; ++i) {
        float angle = 2.0f * 3.14159f * i / 20;
        glVertex2f(powerupX + cos(angle) * 10.0f, powerupY + sin(angle) * 10.0f);
    }
    glEnd();
}

// Initialize the game
static void initGame() {
    srand(time(0));
    playerX = 50.0f;
    playerY = 50.0f;
    playerLives = 3;
    score = 0;
    timeLeft = 60.0f;
    isGameOver = false;

    /*obstacleX = rand() % 300;
    obstacleY = rand() % 300;
    collectableX = rand() % 300;
    collectableY = rand() % 300;
    powerupX = rand() % 300;
    powerupY = rand() % 300;*/
}

// Update player, obstacles, collectables, and power-ups
static void updateGame(float deltaTime) {
    if (isGameOver) return;

    // Update player logic (currently static)

    // Check if player collides with obstacles
    if (fabs(playerX - obstacleX) < 20 && fabs(playerY - obstacleY) < 20) {
        playerLives--;
        if (playerLives == 0) {
            isGameOver = true;
        }
        obstacleX = rand() % 300;
        obstacleY = rand() % 300;
    }

    // Check if player collects collectables
    if (fabs(playerX - collectableX) < 20 && fabs(playerY - collectableY) < 20) {
        score += 10;
        collectableX = rand() % 300;
        collectableY = rand() % 300;
    }

    // Check if player gets a power-up
    if (fabs(playerX - powerupX) < 20 && fabs(playerY - powerupY) < 20) {
        playerLives++;
        powerupX = rand() % 300;
        powerupY = rand() % 300;
    }

    // Decrease time left
    timeLeft -= deltaTime;
    if (timeLeft <= 0) {
        isGameOver = true;
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

    // Flush the OpenGL commands
    glFlush();
}

// Display function for GLUT
static void Display() {
    renderGame();
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clear color to black
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
    initGame();

    glutDisplayFunc(Display);
    glutTimerFunc(0, mainLoop, 0); // Start the main loop

    glutMainLoop(); // Enter the GLUT event loop
    return 0;
}
