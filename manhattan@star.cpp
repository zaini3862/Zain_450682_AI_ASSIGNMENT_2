#include <bits/stdc++.h>
using namespace std;

const int PUZZLE_SIZE = 3;
const int MAX_ITERATIONS = 200;

struct Puzzle {
    vector<vector<int>> cells;
    int heuristicValue;
    int moveCount;

    Puzzle(const vector<vector<int>>& c, int h, int m) : cells(c), heuristicValue(h), moveCount(m) {}

    bool operator<(const Puzzle& other) const {
        return heuristicValue + moveCount > other.heuristicValue + other.moveCount;
    }
};

int calculateManhattanDistance(const vector<vector<int>>& state, const vector<vector<int>>& goal) {
    int distance = 0;
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            int value = state[i][j];
            if (value != 0) {
                int goalRow, goalCol;
                for (int x = 0; x < PUZZLE_SIZE; ++x) {
                    for (int y = 0; y < PUZZLE_SIZE; ++y) {
                        if (goal[x][y] == value) {
                            goalRow = x;
                            goalCol = y;
                        }
                    }
                }
                distance += abs(i - goalRow) + abs(j - goalCol);
            }
        }
    }
    return distance;
}

vector<Puzzle> generateSuccessors(const Puzzle& currentPuzzle, const vector<vector<int>>& goal) {
    vector<Puzzle> successors;
    int blankRow, blankCol;
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            if (currentPuzzle.cells[i][j] == 0) {
                blankRow = i;
                blankCol = j;
            }
        }
    }

    const int moves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (const auto& move : moves) {
        int newRow = blankRow + move[0];
        int newCol = blankCol + move[1];

        if (newRow >= 0 && newRow < PUZZLE_SIZE && newCol >= 0 && newCol < PUZZLE_SIZE) {
            vector<vector<int>> newCells = currentPuzzle.cells;
            swap(newCells[blankRow][blankCol], newCells[newRow][newCol]);
            int h = calculateManhattanDistance(newCells, goal);
            int m = currentPuzzle.moveCount + 1;
            successors.push_back(Puzzle(newCells, h, m));
        }
    }

    return successors;
}

void printPuzzle(const vector<vector<int>>& cells) {
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            if (cells[i][j] == 0) {
                cout << "  ";
            } else {
                cout << cells[i][j] << " ";
            }
        }
        cout << endl;
    }
}

bool isSolvable(const vector<vector<int>>& initialPuzzle, const vector<vector<int>>& goalPuzzle) {
    int invCount = 0;
    vector<int> initialArray(PUZZLE_SIZE * PUZZLE_SIZE, 0);
    vector<int> goalArray(PUZZLE_SIZE * PUZZLE_SIZE, 0);

    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            initialArray[i * PUZZLE_SIZE + j] = initialPuzzle[i][j];
            goalArray[i * PUZZLE_SIZE + j] = goalPuzzle[i][j];
        }
    }

    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE - 1; i++) {
        for (int j = i + 1; j < PUZZLE_SIZE * PUZZLE_SIZE; j++) {
            if (initialArray[j] && initialArray[i] && initialArray[i] > initialArray[j]) {
                invCount++;
            }
        }
    }

    for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE - 1; i++) {
        for (int j = i + 1; j < PUZZLE_SIZE * PUZZLE_SIZE; j++) {
            if (goalArray[j] && goalArray[i] && goalArray[i] > goalArray[j]) {
                invCount++;
            }
        }
    }

    return (invCount % 2 == 0);
}

vector<vector<int>> generateRandomPuzzle() {
    vector<int> numbers(PUZZLE_SIZE * PUZZLE_SIZE, 0);
    iota(numbers.begin(), numbers.end(), 0);
    random_shuffle(numbers.begin(), numbers.end());

    vector<vector<int>> randomPuzzle(PUZZLE_SIZE, vector<int>(PUZZLE_SIZE, 0));
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            randomPuzzle[i][j] = numbers[i * PUZZLE_SIZE + j];
        }
    }

    return randomPuzzle;
}

void solvePuzzle(const vector<vector<int>>& initialPuzzle, const vector<vector<int>>& goalPuzzle) {
    priority_queue<Puzzle> openList;
    unordered_set<string> visited;
    int iterations = 0;

    if (!isSolvable(initialPuzzle, goalPuzzle)) {
        cout << "The puzzle is not solvable. Generating a random initial state." << endl;
        solvePuzzle(generateRandomPuzzle(), goalPuzzle);
        return;
    }

    int h = calculateManhattanDistance(initialPuzzle, goalPuzzle);
    Puzzle start(initialPuzzle, h, 0);
    openList.push(start);

    while (!openList.empty()) {
        Puzzle current = openList.top();
        openList.pop();

        if (current.cells == goalPuzzle) {
            cout << "Goal state reached in " << current.moveCount << " moves." << endl;
            cout << "Goal state:" << endl;
            printPuzzle(current.cells);
            return;
        }

        string hash = "";
        for (int i = 0; i < PUZZLE_SIZE; ++i) {
            for (int j = 0; j < PUZZLE_SIZE; ++j) {
                hash += to_string(current.cells[i][j]);
            }
        }

        if (visited.find(hash) == visited.end()) {
            visited.insert(hash);
            vector<Puzzle> successors = generateSuccessors(current, goalPuzzle);
            for (const Puzzle& successor : successors) {
                openList.push(successor);
            }
            cout << "Moved to state (heuristic cost: " << current.heuristicValue << "):" << endl;
            printPuzzle(current.cells);
        }

        iterations++;
        if (iterations >= MAX_ITERATIONS) {
            cout << "Reached maximum iterations without finding a solution. Generating a random initial state." << endl;
            solvePuzzle(generateRandomPuzzle(), goalPuzzle);
            return;
        }
    }

    cout << "No solution found." << endl;
}

int main() {
    vector<vector<int>> initialPuzzle(PUZZLE_SIZE, vector<int>(PUZZLE_SIZE));
    vector<vector<int>> goalPuzzle(PUZZLE_SIZE, vector<int>(PUZZLE_SIZE));

    cout << "Enter the initial state (use 0 for the blank space): " << endl;
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            cin >> initialPuzzle[i][j];
        }
    }

    cout << "Enter the goal state (use 0 for the blank space): " << endl;
    for (int i = 0; i < PUZZLE_SIZE; ++i) {
        for (int j = 0; j < PUZZLE_SIZE; ++j) {
            cin >> goalPuzzle[i][j];
        }
    }

    solvePuzzle(initialPuzzle, goalPuzzle);

    return 0;
}
