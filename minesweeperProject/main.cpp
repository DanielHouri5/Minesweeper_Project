#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <chrono>
#include <cstdlib>

// Constants

#define sizeOfName 100         ///< Maximum size for the filename.
#define minCoordinate 8        ///< Minimum coordinate value for grid dimensions.
#define twentyPercents 0.2     ///< Represents 20% of the grid size.
#define error (-1)             ///< Error code for failure.
#define valid 1                ///< Code indicating valid operation.
#define zero 0                 ///< Constant for zero value.
#define minPost 0              ///< Minimum index for array operations.
#define mine 1                 ///< Constant indicating a mine.

// Function prototypes
void buildUserMat(char**& userMat, int rows, int cols);
void console(int color);
void printUserMat(char** userMat, int rows, int cols);
void printMat(int** userMat, int rows, int cols);
int takeDetails(char* fileName, int& rows, int& cols, int& mines);
void buildGameMat(int**& gameMat, int rows, int cols);
int random(int min , int max);
void postMines(int**& gameMat,int row , int col , int size);
void fixDetails(int& rows, int& cols, int& mines);
int game(char**& userMat, int** gameMat,int rows , int cols , int mines);
char countOfMines (int** gameMat, int startR, int startC , int endR, int endC , int rows , int cols);
char date(char* fileName);
void deleteGameMat(int** gameMat,int rows);
void deleteUserMat(char** gameMat,int rows);

/**
 * @brief Main function to execute the Minesweeper game.
 *
 * Initializes the game parameters, reads from a file, and manages the game loop.
 *
 * @return 0 on successful execution.
 */
 int main()
{
    srand((unsigned)time(NULL)); // Initialization of random
    char fileName[sizeOfName];
    std::cout << "Please enter the name of the file" << std::endl;
    std::cin.getline(fileName, sizeOfName);
    int rows, cols, mines;

    // Load game details from file
    if (takeDetails(fileName, rows, cols, mines) == error)
        std::cout << "Error, saving game details" << std::endl;
    else
    {
        std::cout << rows << cols << mines << std::endl;
        fixDetails(rows, cols, mines);

        // Initialize game matrices
        int **gameMat = nullptr;
        buildGameMat(gameMat, rows, cols);
        printMat(gameMat, rows, cols);
        postMines(gameMat, rows, cols, mines);
        printMat(gameMat, rows, cols);

        char **userMat = nullptr;
        buildUserMat(userMat, rows, cols);
        printUserMat(userMat, rows, cols);

        // Start the game
        if (game(userMat, gameMat, rows, cols, mines) == -1)
            std::cout << "Error, saving game details" << std::endl;

        // Cleanup
        deleteUserMat(userMat, rows);
        deleteGameMat(gameMat, rows);
    }
    return 0;
}


/**
 * @brief Builds the user matrix.
 *
 * Allocates memory for the user matrix and initializes it with spaces.
 *
 * @param userMat Reference to the user matrix pointer.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 */
void buildUserMat(char**& userMat, int rows, int cols)
{
    // Dynamic allocation of the user matrix.
    userMat = new char*[rows];
    for (int i = 0; i < rows; ++i) {
        userMat[i] = new char [cols];
        for (int j = 0; j < cols; ++j)
            userMat[i][j] = ' '; // Initialize each cell to space.
    }
}

/**
 * @brief Prints the user matrix.
 *
 * Displays the current state of the user matrix to the console.
 *
 * @param userMat The user matrix to be printed.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 */
void printUserMat(char** userMat, int rows, int cols)
{
    std::cout << "  "; // Print leading spaces for column indices.
    for (int j = 0; j < cols; ++j)
        std::cout << "  " << j << " "; // Print column indices.
    std::cout << std::endl;

    for (int i = 0; i < rows; ++i)
    {
        std::cout << "  "; // Print leading spaces for row separator.
        for (int j = 0; j < cols; ++j)
            std::cout << " ---"; // Print horizontal separator.
        std::cout << std::endl << i << " |"; // Print row index.

        for (int k = 0; k < cols; ++k)
        {
            // Use console color based on the value in the user matrix.
            if (userMat[i][k] == 48) // Check for '0'
            {
                console(2); // Set color for '0'.
                std::cout << " " << userMat[i][k]; // Print '0'.
                console(7); // Reset color.
            }
            else if (userMat[i][k] > 48 && userMat[i][k] < 52) // Check for '1' or '2'.
            {
                console(1); // Set color for '1' or '2'.
                std::cout << " " << userMat[i][k]; // Print the value.
                console(7); // Reset color.

            }
            else if (userMat[i][k] > 51 && userMat[i][k] < 55) // Check for '3' or '4'.
            {
                console(4|2); // Set color for '3' or '4'.
                std::cout << " " << userMat[i][k]; // Print the value.
                console(7); // Reset color.

            }
            else if (userMat[i][k] > 54 && userMat[i][k] < 57) // Check for '5' or '6'.
            {
                console(4); // Set color for '5' or '6'.
                std::cout << " " << userMat[i][k]; // Print the value.
                console(7); // Reset color.
            }
            else
                std::cout << " " << userMat[i][k]; // Print other values normally.
            std::cout << " |"; // Print column separator.
        }
        std::cout << std::endl; // New line for the next row.
    }
    std::cout << "  "; // Print leading spaces for the last row separator.
    for (int j = 0; j < cols; ++j)
        std::cout << " ---"; // Print horizontal separator.
    std::cout << std::endl; // End of the user matrix.
}
/**
 * @brief Prints the game matrix.
 *
 * Displays the current state of the game matrix to the console.
 *
 * @param gameMat The game matrix to be printed.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 */
void printMat(int** gameMat, int rows, int cols)
{
    std::cout << "  "; // Print leading spaces for column indices.
    for (int j = 0; j < cols; ++j)
        std::cout << "  " << j << " "; // Print column indices.
    std::cout << std::endl;

    for (int i = 0; i < rows; ++i)
    {
        std::cout << "  "; // Print leading spaces for row separator.
        for (int j = 0; j < cols; ++j)
            std::cout << " ---"; // Print horizontal separator.
        std::cout << std::endl << i << " |"; // Print row index.

        for (int k = 0; k < cols; ++k)
            std::cout << " " << gameMat[i][k] << " |"; // Print the value in each cell.

        std::cout << std::endl; // New line for the next row.
    }
    std::cout << "  "; // Print leading spaces for the last row separator.
    for (int j = 0; j < cols; ++j)
        std::cout << " ---"; // Print horizontal separator.
    std::cout << std::endl << std::endl; // End of the game matrix with additional spacing.
}
/**
 * @brief Changes the console text color.
 *
 * This function takes a color code and sets the console text color
 * accordingly, allowing for color customization of output in the terminal.
 *
 * @param color Color code for the console text.
 */
void console(int color)
{
    HANDLE console_color; // Declaring the variable console_color from HANDLE type.
    console_color = GetStdHandle(STD_OUTPUT_HANDLE); // Get the standard output handle.
    SetConsoleTextAttribute(console_color, color); // Set the console text color.
}
/**
 * @brief Takes the game parameters from a specified file.
 *
 * Reads the number of rows, columns, and mines from a file.
 *
 * @param fileName Name of the file to read from.
 * @param rows Reference to the number of rows.
 * @param cols Reference to the number of columns.
 * @param mines Reference to the number of mines.
 * @return error code if file operation fails, otherwise valid.
 */
int takeDetails(char* fileName, int& rows, int& cols, int& mines)
{
    std::fstream file; // Create file stream.
    file.open(fileName, std::ios::in); // Open file for input.
    if (!file)
        return error; // Return error if file cannot be opened.

    file >> rows >> cols >> mines; // Read rows, columns, and mines from file.
    file.close(); // Close the file.
    return valid; // Return valid status.
}
/**
 * @brief Validates and adjusts game parameters.
 *
 * This function ensures that the number of rows, columns, and mines
 * meet the game's requirements. It modifies the parameters as necessary
 * to ensure valid gameplay.
 *
 * @param rows Reference to the number of rows.
 * @param cols Reference to the number of columns.
 * @param mines Reference to the number of mines.
 */
void fixDetails(int& rows, int& cols, int& mines)
{
    double newRows = rows; // Store original rows count.
    double newCols = cols; // Store original columns count.
    double newMines = mines; // Store original mines count.

    // Adjust values to meet minimum requirements.
    if (newRows < minCoordinate)
        newRows = minCoordinate;
    if (newCols < minCoordinate)
        newCols = minCoordinate;
    if (newMines < newRows*newCols*twentyPercents)
        newMines = newRows*newCols*twentyPercents+1-(newRows*newCols*twentyPercents-(int)(newRows*newCols*twentyPercents));

    rows = (int)newRows;   // Update rows count.
    cols = (int)newCols;   // Update columns count.
    mines = (int)newMines; // Update mines count.

    std::cout << rows << cols << mines << std::endl; // Output adjusted parameters.
}
/**
 * @brief Allocates memory for the game matrix.
 *
 * This function creates a dynamic 2D array (matrix) for the game,
 * initializing all cells to zero, indicating that they are unmarked.
 *
 * @param gameMat Reference to the game matrix pointer.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 */
void buildGameMat(int**& gameMat, int rows, int cols)
{
    // Dynamic allocation of the game matrix.
    gameMat = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        gameMat[i] = new int [cols];
        for (int j = 0; j < cols; ++j)
            gameMat[i][j] = zero; // Initialize each cell to zero.
    }
}
/**
 * @brief Generates a random number within a specified range.
 *
 * This function returns a random integer between the provided min and max values.
 *
 * @param min Minimum value for the random number.
 * @param max Maximum value for the random number.
 * @return Random integer between min and max.
 */
int random(int min , int max)
{
    int value = min + (rand() % (max-min)) ; // Generate a random number.
    return value ;
}
/**
 * @brief Places mines randomly on the game matrix.
 *
 * This function randomly selects positions in the game matrix and places mines
 * until the specified number of mines has been placed.
 *
 * @param gameMat Reference to the game matrix.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 * @param mines Number of mines to be placed.
 */
void postMines(int**& gameMat,int rows , int cols , int mines)
{
    int randRow,randCol; // Variables to hold random row and column indices.
    do {
        randRow = random(minPost, rows); // Generate random row index.
        randCol = random(minPost, cols); // Generate random column index.

        // If the selected cell is unoccupied, place a mine.
        if (gameMat[randRow][randCol] == zero)
        {
            gameMat[randRow][randCol] = mine; // Mark the cell as a mine.
            --mines; // Decrease the remaining mines count.
        }
    } while (mines != zero); // Continue until all mines are placed.
}
/**
 * @brief Manages the main game loop.
 *
 * This function controls the flow of the game, prompting the user to enter
 * coordinates, checking for mine hits, and updating the game state accordingly.
 *
 * @param userMat Reference to the user matrix.
 * @param gameMat The game matrix containing mines.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 * @param mines Number of mines in the game.
 * @return 0 if the game is won, -1 if an error occurs.
 */
int game(char**& userMat, int** gameMat, int rows, int cols , int mines)
{
    char fileName[sizeOfName];
    std::cout << "Please enter the name of the file" << std::endl;
    std::cin.getline(fileName, sizeOfName);
    std::ofstream documentationFile; // File stream for logging.
    int resultFile = 0; // Counter for logged results.
    int result = (rows*cols)-mines; // Total number of cells to reveal.
    int count = 0; // Count of revealed cells.
    int chosenRow; // User's chosen row.
    int chosenCol; // User's chosen column.

    do{
        std::cout << "please enter number of row" << std::endl;
        std::cin >> chosenRow; // Get user input for row.
        std::cout << "please enter number of col" << std::endl;
        std::cin >> chosenCol; // Get user input for column.

        // Check for out-of-bounds input.
        if (chosenRow < 0 || chosenRow > rows || chosenCol < 0 || chosenCol > cols)
        {
            documentationFile.open(fileName, std::ios::app | std::ios::out); // Open log file for appending.
            if (!documentationFile)
                return error; // Return error if file cannot be opened.

            documentationFile << "Leave" << " "; // Log exit.
            documentationFile.close(); // Close log file.
            std::cout << "Error, illegal position, see you next time!" << std::endl; // Inform user of error.
            return 1; // Return error code.
        }

        // Check if the chosen cell is a mine.
        if (gameMat[chosenRow][chosenCol] == 1)
        {
            documentationFile.open(fileName, std::ios::app | std::ios::out); // Open log file for appending.
            if (!documentationFile)
                return error; // Return error if file cannot be opened.

            documentationFile << 'X' << '\n' << "Loss" << " "; // Log loss due to mine.
            documentationFile.close(); // Close log file.
            std::cout << "Error, you have chosen a mine, see you next time!" << std::endl; // Inform user of loss.
            return 2; // Return loss code.
        }

        // Process a valid move.
        if (gameMat[chosenRow][chosenCol] == 0)
        {
            userMat[chosenRow][chosenCol] = countOfMines(gameMat, chosenRow-1, chosenCol-1, chosenRow+1, chosenCol+1, rows, cols); // Update user matrix with mine count.
            ++count; // Increment count of revealed cells.
            ++resultFile; // Increment result log count.

            printUserMat(userMat, rows, cols); // Display updated user matrix.

            documentationFile.open(fileName, std::ios::app | std::ios::out); // Open log file for appending.
            if (!documentationFile)
                return error; // Return error if file cannot be opened.

            documentationFile << cols << " " << rows << " "; // Log current game state.
            if(date(fileName) == 48)
                return error; // Return error if date logging fails.

            documentationFile << resultFile << '\n'; // Log the result of this move.
            documentationFile.close(); // Close log file.
        }
    } while (count != result); // Continue until all non-mine cells are revealed.

    documentationFile.open(fileName, std::ios::app | std::ios::out); // Open log file for appending.
    if (!documentationFile)
        return error; // Return error if file cannot be opened.
    documentationFile << "Win" << " "; // Log win.
    documentationFile.close(); // Close log file.
    std::cout << "Congratulations, you win!" << std::endl; // Inform user of win.
    return 0; // Return success code.
}
/**
 * @brief Counts the number of mines around a given cell.
 *
 * This function checks adjacent cells of the specified cell and counts the
 * number of mines present.
 *
 * @param gameMat The game matrix containing mines.
 * @param startR Starting row index.
 * @param startC Starting column index.
 * @param endR Ending row index.
 * @param endC Ending column index.
 * @param rows Total number of rows in the matrix.
 * @param cols Total number of columns in the matrix.
 * @return Character representing the count of adjacent mines.
 */
char countOfMines (int** gameMat, int startR, int startC , int endR, int endC , int rows , int cols)
{
    char count = 48; // Start count at 0 (character '0').
    if (startR < zero) startR = zero; // Adjust boundaries.
    if (startC < zero) startC = zero; // Adjust boundaries.
    if (endR >= rows) endR = rows-1; // Adjust boundaries.
    if (endC >= cols) endC = cols-1; // Adjust boundaries.

    for (int i = startR; i <= endR; ++i)
        for (int j = startC; j <= endC; ++j)
            if (gameMat[i][j] == 1)
                ++count; // Count mines.
    return count; // Return the mine count.
}
/**
 * @brief Records the current date and time to a file.
 *
 * This function appends the current date and time to the specified file.
 *
 * @param fileName Name of the file to log date and time.
 * @return Character indicating success or failure.
 */
char date(char* fileName)
{
    std::fstream documentationFile;
    documentationFile.open(fileName, std::ios::app | std::ios::out);
    if (!documentationFile)
        return 48; // Error if file fails to open.

    auto curr_time = std::chrono::system_clock::now(); // Get current time.
    std::time_t c_time = std::chrono::system_clock::to_time_t(curr_time);
    documentationFile << ctime(&c_time) << " "; // Write date and time to file.
    documentationFile.close();
    return 49; // Success.
}
/**
 * @brief Deallocates memory for the game matrix.
 *
 * This function releases memory allocated for the game matrix.
 *
 * @param gameMat The game matrix to be deleted.
 * @param rows Number of rows in the matrix.
 */
void deleteGameMat(int** gameMat,int rows)
{
    for (int i = 0; i < rows; i++)
        delete[] gameMat[i]; // Delete each row.
    delete[] gameMat; // Delete the row pointers.
}
/**
 * @brief Deallocates memory for the user matrix.
 *
 * This function releases memory allocated for the user matrix.
 *
 * @param userMat The user matrix to be deleted.
 * @param rows Number of rows in the matrix.
 */
void deleteUserMat(char** userMat,int rows)
{
    for (int i = 0; i < rows; i++)
        delete[] userMat[i]; // Delete each row.
    delete[] userMat; // Delete the row pointers.
}

