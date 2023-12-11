#include <bits/stdc++.h>
using namespace std;

const int BOARD_SIZE = 3;
const int MAX_ITERATIONS = 200;

struct GameBoard {
    vector<vector<int>> cells;
    int heuristic;
    int moves;

    GameBoard(const vector<vector<int>>& c, int h, int m) : cells(c), heuristic(h), moves(m) {}

    bool operator<(const GameBoard& other) const {
        return heuristic > other.heuristic;
    }
};

int calculateManhattanDistance(const vector<vector<int>>& current, const vector<vector<int>>& target) {
    int distance = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            int value = current[i][j];
            if (value != 0) {
                int targetRow, targetCol;
                for (int x = 0; x < BOARD_SIZE; ++x) {
                    for (int y = 0; y < BOARD_SIZE; ++y) {
                        if (target[x][y] == value) {
                            targetRow = x;
                            targetCol = y;
                        }
                    }
                }
                distance += abs(i - targetRow) + abs(j - targetCol);
            }
        }
    }
    return distance;
}

vector<GameBoard> generateMoves(const GameBoard& board, const vector<vector<int>>& target) {
    vector<GameBoard> moves;
    int blankRow, blankCol;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board.cells[i][j] == 0) {
                blankRow = i;
                blankCol = j;
            }
        }
    }

    const int possibleMoves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto& move : possibleMoves) {
        int newRow = blankRow + move[0];
        int newCol = blankCol + move[1];

        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE) {
            vector<vector<int>> newCells = board.cells;
            swap(newCells[blankRow][blankCol], newCells[newRow][newCol]);
            int h = calculateManhattanDistance(newCells, target);
            int m = board.moves + 1;
            moves.push_back(GameBoard(newCells, h, m));
        }
    }

    return moves;
}

void displayBoard(const vector<vector<int>>& cells) {
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

bool isSolvable(const vector<vector<int>>& initial, const vector<vector<int>>& target) {
    int invCount = 0;
    vector<int> initialArray(BOARD_SIZE * BOARD_SIZE, 0);
    vector<int> targetArray(BOARD_SIZE * BOARD_SIZE, 0);

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            initialArray[i * BOARD_SIZE + j] = initial[i][j];
            targetArray[i * BOARD_SIZE + j] = target[i][j];
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
            if (targetArray[j] && targetArray[i] && targetArray[i] > targetArray[j]) {
                invCount++;
            }
        }
    }

    return (invCount % 2 == 0);
}

vector<vector<int>> generateRandomState() {
    vector<int> numbers(BOARD_SIZE * BOARD_SIZE, 0);
    iota(numbers.begin(), numbers.end(), 0);
    random_shuffle(numbers.begin(), numbers.end());

    vector<vector<int>> randomState(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            randomState[i][j] = numbers[i * BOARD_SIZE + j];
        }
    }

    return randomState;
}

void solvePuzzle(const vector<vector<int>>& initial, const vector<vector<int>>& target) {
    priority_queue<GameBoard> openList;
    unordered_set<string> visited;
    int iterations = 0;

    if (!isSolvable(initial, target)) {
        cout << "The puzzle is not solvable. Generating a random initial state." << endl;
        solvePuzzle(generateRandomState(), target);
        return;
    }

    int h = calculateManhattanDistance(initial, target);
    GameBoard start(initial, h, 0);
    openList.push(start);

    while (!openList.empty()) {
        GameBoard current = openList.top();
        openList.pop();

        if (current.cells == target) {
            cout << "Goal state reached in " << current.moves << " moves." << endl;
            cout << "Goal state:" << endl;
            displayBoard(current.cells);
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
            vector<GameBoard> moves = generateMoves(current, target);
            for (const GameBoard& move : moves) {
                openList.push(move);
            }
            cout << "Moved to state (heuristic cost: " << current.heuristic << "):" << endl;
            displayBoard(current.cells);
        }

        iterations++;
        if (iterations >= MAX_ITERATIONS) {
            cout << "Reached maximum iterations without finding a solution. Generating a random initial state." << endl;
            solvePuzzle(generateRandomState(), target);
            return;
        }
    }

    cout << "No solution found." << endl;
}

int main() {
    vector<vector<int>> initial(BOARD_SIZE, vector<int>(BOARD_SIZE));
    vector<vector<int>> target(BOARD_SIZE, vector<int>(BOARD_SIZE));

    cout << "Enter the initial state (use 0 for the blank space): " << endl;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cin >> initial[i][j];
        }
    }

    cout << "Enter the goal state (use 0 for the blank space): " << endl;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cin >> target[i][j];
        }
    }

    solvePuzzle(initial, target);

    return 0;
}
