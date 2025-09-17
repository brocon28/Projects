//http://bridges-cs.herokuapp.com/assignments/1/brocon28
#include <iostream>
#include <string>
#include <vector>
#include "Bridges.h"
#include "DataSource.h"
#include "BSTElement.h"
#include <queue>
using namespace std;
using namespace bridges;

/*
  Step 1: Get the Bridges USER ID and API KEY for your program
          by creating an account and signing up (You can use any
          email): https://bridges-cs.herokuapp.com/signup

  Step 2: User ID (Username) and Key (API Sha1 Key) can be
          found under Profile on the top left
*/

// Step 8: Insert function for BST
BSTElement<double, string>* insertIntoBST(double key, string value, BSTElement<double, string>* root) {
    if (root == nullptr) {
        return new BSTElement<double, string>(key, value);
    }
    if (key < root->getKey()) {
        root->setLeft(insertIntoBST(key, value, root->getLeft()));
    } else {
        root->setRight(insertIntoBST(key, value, root->getRight()));
    }
    return root;
}

int main(int argc, char **argv)
{
    // Step 3: Test if the following print statement is being run
    cout << "username=" << (getenv("username") ? getenv("username") : "NULL") << endl;
    cout << "api_key=" << (getenv("api_key") ? getenv("api_key") : "NULL") << endl;
    cout << "Bridges Test: I can see this is getting printed\n";

    // Step 4: Add your User ID and API Key as secrets on Replit

    /* Step 5: Print your User ID and API Key from secrets to the console
               to test if secrets were properly loaded
    */
    const char* user = getenv("username");
    const char* key = getenv("api_key");

    if (user && key) {
        cout << "User ID: " << user << endl;
        cout << "API Key: " << key << endl;
    } else {
        cout << "Error: Could not load Bridges credentials.\n";
        return -1;
    }

    /*
       Step 6: Create a Bridges object by uncommenting the next line
               and inserting the values from steps 1 and 2
    */
    Bridges bridges(1, user, key);
    bridges.setTitle("BST Visualization with Game Data");
    bridges.setDescription("Building a BST of games using rating as the key.");

    /*
       Step 7: Import your selected data into a vector<> by
               referring to C++ examples in the Bridges datasets documentation:
               https://bridgesuncc.github.io/datasets.html
    */
    DataSource ds;
    vector<Game> game_data = ds.getGameData();

    /*
       Step 8: Open the file "insertIntoBST.txt" and copy the provided
               function for inserting data into a BST.
               Paste the function into this file above your "main" function.
    */

    /*
       Step 9: Use the insertion function from the previous step to build a BST with
                data points. Here, we use game ratings as keys and titles as values.
    */
    BSTElement<double, string>* root = nullptr;

    // Limit to first 100 games to avoid hitting Bridges size limits
    int limit = min((int)game_data.size(), 100);
    for (int i = 0; i < limit; i++) {
        double rating = game_data[i].getRating();
        string title = game_data[i].getTitle();
        if (rating > 0) { // skip unrated games
            root = insertIntoBST(rating, title, root);
        }
    }

    /*
       Step 10: Visualize this tree by referring to the Bridges BST documentation:
                https://bridgesuncc.github.io/tutorials/BinarySearchTree.html
       (Commented out because we will replace with Step 12 visualization)
    */
    /*
    bridges.setDataStructure(root);
    bridges.visualize();
    cout << "Tree visualized successfully!" << endl;
    */

    /*
       Step 11: Color each level of the tree using a level-order traversal.
                Every node in a given level should have the same color.
                Do not use the same color in two consecutive levels.
    */
    vector<string> colors = {"red", "blue", "green", "orange", "purple", "cyan", "magenta", "yellow"};
    int color_index = 0;

    if (root) {
        queue<BSTElement<double, string>*> q;
        q.push(root);

        while (!q.empty()) {
            int level_size = q.size();

            // Assign same color to all nodes in this level
            for (int i = 0; i < level_size; i++) {
                BSTElement<double, string>* node = q.front();
                q.pop();

                node->getVisualizer()->setColor(colors[color_index]);

                if (node->getLeft()) q.push(node->getLeft());
                if (node->getRight()) q.push(node->getRight());
            }

            // Move to next color for the next level
            color_index = (color_index + 1) % colors.size();
        }
    }

    /*
       Step 12: Visualize the updated tree. Comment out or remove the old
                visualization code from Step 10
    */
    bridges.setDataStructure(root);
    bridges.visualize();
    cout << "Colored tree visualized successfully!" << endl;

    return 0;
}
