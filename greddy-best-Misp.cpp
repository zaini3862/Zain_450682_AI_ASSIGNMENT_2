#include <bits/stdc++.h>
using namespace std;

const int BOARD_SIZE = 3;
const int MAX_ITERATIONS = 200;

struct GameBoard {
    vector<vector<int>> cells;
    int heuristicValue;
    int moveCount;

    GameBoard(const vector<vector<int>>& c, int h, int m) : cells(c), heuristicValue(h), moveCount(m) {}

    bool operator<(const GameBoard& other) const {
        return heuristicValue > other.heuristicValue;
    }
};

int calculateMisplacedTiles(const vector<vector<int>>& state, const vector<vector<int>>& goal) {
    int misplacedTiles = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state[i][j] != goal[i][j] && state[i][j] != 0) {
                misplacedTiles++;
            }
        }
    }
    return misplacedTiles;
}

vector<GameBoard> generateSuccessors(const GameBoard& currentBoard, const vector<vector<int>>& goal) {
    vector<GameBoard> successors;
    int blankRow, blankCol;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (currentBoard.cells[i][j] == 0) {
                blankRow = i;
                blankCol = j;
            }
        }
    }

    const int moves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto& move : moves) {
        int newRow = blankRow + move[0];
        int newCol = blankCol + move[1];

        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE) {
            vector<vector<int>> newCells = currentBoard.cells;
            swap(newCells[blankRow][blankCol], newCells[newRow][newCol]);
            int h = calculateMisplacedTiles(newCells, goal);
            int m = currentBoard.moveCount + 1;
            successors.push_back(GameBoard(newCells, h, m));
        }
    }

    return successors;
}

void printBoard(const vector<vector<int>>& cells) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (cells[i][j] == 0) {
                cout << "  ";
            } else {
                cout << cells[i][j] << " ";
            }
        }
        cout << endl;
    }
}

bool isSolvable(const vector<vector<int>>& initialBoard, const vector<vector<int>>& goalBoard) {
    int invCount = 0;
    vector<int> initialArray(BOARD_SIZE * BOARD_SIZE, 0);
    vector<int> goalArray(BOARD_SIZE * BOARD_SIZE, 0);

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            initialArray[i * BOARD_SIZE + j] = initialBoard[i][j];
            goalArray[i * BOARD_SIZE + j] = goalBoard[i][j];
        }
    }

    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE - 1; i++) {
        for (int j = i + 1; j < BOARD_SIZE * BOARD_SIZE; j++) {
            if (initialArray[j] && initialArray[i] && initialArray[i] > initialArray[j]) {
                invCount++;
            }
        }
    }

    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE - 1; i++) {
        for (int j = i + 1; j < BOARD_SIZE * BOARD_SIZE; j++) {
            if (goalArray[j] && goalArray[i] && goalArray[i] > goalArray[j]) {
                invCount++;
            }
        }
    }

    return (invCount % 2 == 0);
}

vector<vector<int>> generateRandomBoard() {
    vector<int> numbers(BOARD_SIZE * BOARD_SIZE, 0);
    iota(numbers.begin(), numbers.end(), 0);
    random_shuffle(numbers.begin(), numbers.end());

    vector<vector<int>> randomBoard(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            randomBoard[i][j] = numbers[i * BOARD_SIZE + j];
        }
    }

    return randomBoard;
}

void solvePuzzle(const vector<vector<int>>& initialBoard, const vector<vector<int>>& goalBoard) {
    priority_queue<GameBoard> openList;
    unordered_set<string> visited;
    int iterations = 0;

    if (!isSolvable(initialBoard, goalBoard)) {
        cout << "The puzzle is not solvable. Generating a random initial state." << endl;
        solvePuzzle(generateRandomBoard(), goalBoard);
        return;
    }

    int h = calculateMisplacedTiles(initialBoard, goalBoard);
    GameBoard start(initialBoard, h, 0);
    openList.push(start);

    while (!openList.empty()) {
        GameBoard current = openList.top();
        openList.pop();

        if (current.cells == goalBoard) {
            cout << "Goal state reached in " << current.moveCount << " moves." << endl;
            cout << "Goal state:" << endl;
            printBoard(current.cells);
            return;
        }

        string hash = "";
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                hash += to_string(current.cells[i][j]);
            }
        }

        if (visited.find(hash) == visited.end()) {
            visited.insert(hash);
            vector<GameBoard> successors = generateSuccessors(current, goalBoard);
            for (const GameBoard& successor : successors) {
                openList.push(successor);
            }
            cout << "Moved to state (heuristic cost: " << current.heuristicValue << "):" << endl;
            printBoard(current.cells);
        }

        iterations++;
        if (iterations >= MAX_ITERATIONS) {
            cout << "Reached maximum iterations without finding a solution. Generating a random initial state." << endl;
            solvePuzzle(generateRandomBoard(), goalBoard);
            return;
        }
    }

    cout << "No solution found." << endl;
}

int main() {
    vector<vector<int>> initialBoard(BOARD_SIZE, vector<int>(BOARD_SIZE));
    vector<vector<int>> goalBoard(BOARD_SIZE, vector<int>(BOARD_SIZE));

    cout << "Enter the initial state (use 0 for the blank space): " << endl;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cin >> initialBoard[i][j];
        }
    }

    cout << "Enter the goal state (use 0 for the blank space): " << endl;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cin >> goalBoard[i][j];
        }
    }

    solvePuzzle(initialBoard, goalBoard);

    return 0;
}
