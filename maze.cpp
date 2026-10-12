// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    Vector<GridLocation> candidates = {
        {cur.row - 1, cur.col},  // Up
        {cur.row + 1, cur.col},  // Down
        {cur.row, cur.col - 1},  // Left
        {cur.row, cur.col + 1}   // Right
    };

    for (GridLocation next : candidates) {
        if (maze.inBounds(next) && maze[next]) {
            neighbors.add(next);
        }
    }

    return neighbors;
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
void validatePath(Grid<bool>& maze, Vector<GridLocation>& path) {
    GridLocation exit = { maze.numRows()-1, maze.numCols()-1 }; // last row, last col
    GridLocation origin = {0,0};
    Set<GridLocation> dupeCheck;
    dupeCheck.add(origin);
    if (path.isEmpty()) {
        error("Path is empty!");
    }
    if (path[0] != origin) {
        error("Path does not start at the origin!");
    }
    if (path[path.size()-1] != exit) {
        error("Path does end at the proper location!");
    }

    /* If you find a problem with the path, call error() to report it.
     * If the path is a valid solution, then this function should run to completion
     * without raising any errors.
     */

    for (int i = 1; i < path.size(); i++) {
        if (dupeCheck.contains(path[i])) {
            error("Path contains a repeated location!");
        }
        dupeCheck.add(path[i]);
        Set<GridLocation> validCheck = generateValidMoves(maze, path[i-1]);
        if (!validCheck.contains(path[i])) {
            error("Wrong location detected in path!");
        }
    }
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
Vector<GridLocation> solveMazeBFS(Grid<bool>& maze) {
    Vector<GridLocation> path = {{0, 0}};
    Queue<Vector<GridLocation>> allPaths;
    allPaths.enqueue(path);
    GridLocation exit = { maze.numRows()-1, maze.numCols()-1 };
    Set<GridLocation> dupeCheck = {{0, 0}};

    drawMaze(maze);
    while(!allPaths.isEmpty()) {
        path = allPaths.dequeue();
        highlightPath(path, "blue",10);
        if (path[path.size()-1] == exit) {
            break;
        }
        Set<GridLocation> neighbors = generateValidMoves(maze,path[path.size()-1]);
        for (GridLocation test : neighbors) {
            if (!dupeCheck.contains(test)) {
                dupeCheck.add(test);
                Vector<GridLocation> pathCopy = path;
                pathCopy.add(test);
                allPaths.enqueue(pathCopy);
            }
        }
    }

    return path;
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
Vector<GridLocation> solveMazeDFS(Grid<bool>& maze) {
    Vector<GridLocation> path = {{0, 0}};
    Stack<Vector<GridLocation>> allPaths;
    allPaths.push(path);
    GridLocation exit = { maze.numRows()-1, maze.numCols()-1 };
    Set<GridLocation> dupeCheck = {{0, 0}};

    drawMaze(maze);
    while(!allPaths.isEmpty()) {
        path = allPaths.pop();
        highlightPath(path, "blue",10);
        if (path[path.size()-1] == exit) {
            break;
        }
        Set<GridLocation> neighbors = generateValidMoves(maze,path[path.size()-1]);
        for (GridLocation test : neighbors) {
            if (!dupeCheck.contains(test)) {
                dupeCheck.add(test);
                Vector<GridLocation> pathCopy = path;
                pathCopy.add(test);
                allPaths.push(pathCopy);
            }
        }
    }

    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines = stringSplit(readEntire(in), '\n');

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Vector<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0, 0}, {0, 2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution, hand-constructed maze") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Vector<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution read from file, medium maze") {
    Grid<bool> maze;
    Vector<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> notBeginAtEntry = { {1, 1}, {2, 1} };
    Vector<GridLocation> notEndAtExit = { {0, 0}, {1, 0}, {2, 0} };
    Vector<GridLocation> moveThroughWall = { {0 ,0}, {0, 1}, {1, 1}, {2, 1} };

    EXPECT_ERROR(validatePath(maze, notBeginAtEntry));
    EXPECT_ERROR(validatePath(maze, notEndAtExit));
    EXPECT_ERROR(validatePath(maze, moveThroughWall));
}

PROVIDED_TEST("solveMazeBFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMazeDFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// STUDENT_TEST()
STUDENT_TEST("generateValidMoves excludes walls and diagonal passages") {
    Grid<bool> maze = {{true,  false, true},
                       {false, true,  false},
                       {true,  false, true}};
    GridLocation cur = {1, 1};
    Set<GridLocation> expected;

    EXPECT_EQUAL(generateValidMoves(maze, cur), expected);
}

STUDENT_TEST("generateValidMoves at bottom-right corner of rectangular grid") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true}};
    GridLocation cur = {1, 2};
    Set<GridLocation> expected = {{0, 2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, cur), expected);
}

STUDENT_TEST("generateValidMoves in a 1x1 maze returns no neighbors") {
    Grid<bool> maze = {{true}};
    GridLocation cur = {0, 0};
    Set<GridLocation> expected;

    EXPECT_EQUAL(generateValidMoves(maze, cur), expected);
}

STUDENT_TEST("validatePath rejects an empty path") {
    Grid<bool> maze = {{true, true},
                       {true, true}};
    Vector<GridLocation> path;

    EXPECT_ERROR(validatePath(maze, path));
}

STUDENT_TEST("validatePath rejects a diagonal move") {
    Grid<bool> maze = {{true, true},
                       {true, true}};
    Vector<GridLocation> path = {{0, 0}, {1, 1}};

    EXPECT_ERROR(validatePath(maze, path));
}

STUDENT_TEST("validatePath rejects a repeated origin") {
    Grid<bool> maze = {{true, true},
                       {true, true}};
    Vector<GridLocation> path = {
        {0, 0}, {1, 0}, {0, 0}, {0, 1}, {1, 1}
    };

    EXPECT_ERROR(validatePath(maze, path));
}

STUDENT_TEST("solveMazeBFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeBFS on file 25x33") {
    Grid<bool> maze;
    readMazeFile("res/25x33.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeDFS on file 13x39") {
    Grid<bool> maze;
    readMazeFile("res/13x39.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeDFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}