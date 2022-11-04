#include <iostream>
#include <vector>
#include <queue>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Global variable to avoid declaring multiple times
vector<vector<int>> goalState = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};   // End 3x3 goal state to compare to, change for other puzzle versions

// Nodes to contain important info of puzzle state
struct TreeNode
{
    int gVal;                                   // Cost of traveling form one node to another, set to 1 for brute force
    int hVal;                                   // Heuristic value, 0 for brute force
    vector <vector <int>> currPuzzle;           // 2D vector puzzle state
    vector <TreeNode*> puzzleDeviation;         // All possible states the current puzzle can go to in a vector
};

// Helper function to print vector, non-formatted.
void printVec(vector <vector <int>> outputVec)
{
    for(int a = 0; a < outputVec.size(); a++)
    {
        for(int b = 0; b < outputVec[0].size(); b++)
        {
            cout << outputVec[a][b] << " ";
        }
        cout << endl;
    }
}

//Helper function to compare 2 vectors, both should be same size
//If a difference is found, they are not the same, otherwise they are the same state
bool compareVecs(vector <vector<int>> vecOne, vector <vector<int>> vecTwo)
{
    for(int a = 0; a < vecOne.size(); a++)
    {
        for(int b = 0; b < vecOne[0].size(); b++)
        {
            if(vecOne[a][b] != vecTwo[a][b])
            {
                return false;
            }
        }
    }

    return true;
}

// Helper function to trace back solution from root to goal state
bool findPath(TreeNode* rootNode, vector<vector<vector<int>>>& traceVec)
{
    vector<vector<int>> puzzleVec = rootNode->currPuzzle;
    if(compareVecs(puzzleVec, goalState))
    {
        return true;
    }

    traceVec.push_back(puzzleVec);

    bool childGoal = false;
    for(int i = 0; i < rootNode->puzzleDeviation.size(); ++i)
    {
        if(findPath(rootNode->puzzleDeviation[i], traceVec))
        {
            childGoal = true;
            break;
        }
    }
    if(childGoal)
    {
        return true;
    }

    traceVec.pop_back();
    return false;
}

// Helper function to split the current node into the others
vector<vector<vector<int>>> allPossibles(TreeNode* originalNode)
{
    vector<vector<vector<int>>> allChildNodes;            // All possibilites or children from original node passed
    bool leftMove, rightMove, upMove, downMove = true;    // Set all possible moves to true
    vector<vector<int>> puzzleNow = originalNode->currPuzzle;
    int zeroPosRow;
    int zeroPosCol;
    
    // Find position of 0 in puzzle, keep track of which row and column it's in
    for(int i = 0; i < puzzleNow.size(); i++)
    {
        bool zeroFound = false;
        for(int j = 0; j < puzzleNow[0].size(); j++)
        {
            if(puzzleNow[i][j] == 0)
            {
                zeroFound = true;
                zeroPosRow = i;
                zeroPosCol = j;
                break;
            }
        }
        if(zeroFound)
        {
            break;
        }
    }
    
    if(zeroPosCol == 0)                                           // If leftmost column, left move not possible
    {
        leftMove = false;
    }
    else
    {
        leftMove = true;
    }
    if(zeroPosCol == originalNode->currPuzzle.size() - 1)    // Rightmost column means no right move
    {
        rightMove = false;
    }
    else
    {
        rightMove = true;
    }
    if(zeroPosRow == 0)                                           // Top row means no up move possible
    {
        upMove = false;
    }
    else
    {
        upMove = true;
    }
    if(zeroPosRow == originalNode->currPuzzle.size() - 1)    // Bottom row means no down move possible
    {
        downMove = false;
    }
    else
    {
        downMove = true;
    }

    // Make puzzle vector and int to swap values
    vector<vector<int>> swappedVec;
    int tempSwapVal = 0;
    // For each of the 4 possible moves of the blank square (0), swap elements in that move and store in possible end puzzles
    if(leftMove)
    {
        swappedVec = originalNode->currPuzzle;
        tempSwapVal = originalNode->currPuzzle[zeroPosRow][zeroPosCol - 1];
        swappedVec[zeroPosRow][zeroPosCol - 1] = 0;
        swappedVec[zeroPosRow][zeroPosCol] = tempSwapVal;

        allChildNodes.push_back(swappedVec);
    }
    if(rightMove)
    {
        swappedVec = originalNode->currPuzzle;
        tempSwapVal = originalNode->currPuzzle[zeroPosRow][zeroPosCol + 1];
        swappedVec[zeroPosRow][zeroPosCol + 1] = 0;
        swappedVec[zeroPosRow][zeroPosCol] = tempSwapVal;

        allChildNodes.push_back(swappedVec);
    }
    if(upMove)
    {
        swappedVec = originalNode->currPuzzle;
        tempSwapVal = originalNode->currPuzzle[zeroPosRow - 1][zeroPosCol];
        swappedVec[zeroPosRow - 1][zeroPosCol] = 0;
        swappedVec[zeroPosRow][zeroPosCol] = tempSwapVal;

        allChildNodes.push_back(swappedVec);
    }
    if(downMove)
    {
        swappedVec = originalNode->currPuzzle;
        tempSwapVal = originalNode->currPuzzle[zeroPosRow + 1][zeroPosCol];
        swappedVec[zeroPosRow + 1][zeroPosCol] = 0;
        swappedVec[zeroPosRow][zeroPosCol] = tempSwapVal;

        allChildNodes.push_back(swappedVec);
    }
    return allChildNodes;
}

// The search algorithm
void SearchAlgorithm(vector <vector <int>> puzzleVec, int funcNum)
{
    TreeNode* rootNode = new TreeNode;                         // Starting position, KEPT AS IS
    rootNode->gVal = 0;                                        // 0 for now since only uniform cost search for now
    rootNode->hVal = 0;                                        // ^ Same reasoning as above
    int totalNum = 0;                                          // Total number of nodes searched

    rootNode->currPuzzle = puzzleVec;
    
    vector<vector<vector<int>>> repeatStates;                 // Vector of all possible 2D states from starting puzzle
    repeatStates.push_back(puzzleVec);
    
    queue <TreeNode*> nodes;                       // Queue of states that may make further states, initially size 1
    nodes.push(rootNode);

    while(nodes.size() > 0)
    {
        // Get the first node in the queue
        TreeNode* currNode = new TreeNode;
        currNode = nodes.front();
        nodes.pop();
        
        // Get all possible next states of the vector, remove any states that are already encountered (stored in repeatStates), add amount to total nodes searched
        vector<vector<vector<int>>> poStates = allPossibles(currNode);
        
        for(int i = 0; i < poStates.size(); i++)
        {
            for(int j = 0; j < repeatStates.size(); j++)
            {
                if(compareVecs(poStates[i], repeatStates[j]))
                {
                    poStates.erase(poStates.begin() + i);
                    i--;
                    break;
                }
            }
        }
        totalNum += poStates.size();
        
        // Check if any states are the goal state, otherwise add to repeatStates and queue, create nodes to tree, repeat
        bool finished = false;
        vector<TreeNode*> currChildren;
        for(int i = 0; i < poStates.size(); i++)
        {
            TreeNode* tempNode = new TreeNode;
            if(funcNum == 1)
            {
                tempNode->gVal = currNode->gVal + 1;
            }
            if(funcNum == 1)
            {
                tempNode->hVal = 0;
            }
            tempNode->currPuzzle = poStates[i];
            
            repeatStates.push_back(poStates[i]);
            nodes.push(tempNode);
            currChildren.push_back(tempNode);
            if(compareVecs(poStates[i], goalState))
            {
                cout << "Goal stat found after " << totalNum << " nodes expanded" << endl;
                finished = true;
                break;
            }
        }
        currNode->puzzleDeviation = currChildren;
        if(finished) // Break out of while loop since goal state was reached
        {
            break;
        }
        cout << "Running " << totalNum << endl;                                     // This is just to make sure it's still running
    }
    if(nodes.size() == 0)
    {
        cout << "Solution not found, invalid puzzle, test" << endl;
    }
    else
    {
        vector<vector<vector<int>>> solutionTrace;
        findPath(rootNode, solutionTrace);
        cout << "The solution is depth "  << solutionTrace.size() << endl;
        for(int i = 0; i < solutionTrace.size(); ++i)
        {
            printVec(solutionTrace[i]);
            cout << endl;
        }
    }
}

int main()
{
    vector <vector <int>> userPuzzle;                          // Current state of puzzle
    //userPuzzle = {{1, 3, 6}, {5, 0, 7}, {4, 8, 2}};
    
    // Input calls for 3x3 puzzle
    for(int i = 0; i < 3; i++)
    {
        cout << "Enter numbers for row " << i + 1 << ": "; 
        vector <int> tempVec;
        for(int j = 0; j < 3; j++)
        {
            int tempVal;
            cin >> tempVal;
            tempVec.push_back(tempVal);
        }
        userPuzzle.push_back(tempVec);
    }
    cout << endl;
    
    auto start = high_resolution_clock::now();
    SearchAlgorithm(userPuzzle, 1);
    auto end = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Total time taken is " << duration.count() << " ms" << endl;
    
    return 0;
}