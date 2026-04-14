#include <iostream>
#include <string>
using namespace std;

class Node {
public:
    string value;
    Node* left;  
    Node* right;
    Node* parent;

    Node(string str) : value(str), left(nullptr), right(nullptr), parent(nullptr) {}
};

void rewireNode(Node* new_parent_Node, Node* new_child_Node, int direction){
    if (new_parent_Node == nullptr){
            new_child_Node->parent = nullptr;
            return;
    }
    if (direction == 0){
        if (new_child_Node == nullptr){
            new_parent_Node->left = nullptr;
            return;
        }
        new_parent_Node->left = new_child_Node;
        new_child_Node->parent = new_parent_Node;
    }
    else{
        if (new_child_Node == nullptr){
            new_parent_Node->right = nullptr;
            return;
        }
        new_parent_Node->right = new_child_Node;
        new_child_Node->parent = new_parent_Node;
    }
}

class BinaryTree {
private:
    Node* findRecursive(Node* curr_Node, string num) {
        if (curr_Node == nullptr) {
            return nullptr;
        }

        if (curr_Node->value == num){
            return curr_Node;
        }

        if (curr_Node->left != nullptr){
            if (curr_Node->left->value == num){
                return curr_Node->left;
            }
        }

        if (curr_Node->right != nullptr){
            if (curr_Node->right->value == num){
                return curr_Node->right;
            }
        }

        Node* leftResult = findRecursive(curr_Node->left, num);
        if (leftResult != nullptr) {
            return leftResult;
        }
        
        return findRecursive(curr_Node->right, num);
    }

public:
    Node* root;

    BinaryTree(Node* starting_node) : root(starting_node) {}

    void clearTree(Node* node){
        if (node == nullptr){
            return;
        } 

        clearTree(node->left);
        clearTree(node->right);
        
        if (node->parent == nullptr){
            root = nullptr;
        }
        else if (node->parent->left == node){
            rewireNode(node->parent, nullptr, 0);
        }
        else{
            rewireNode(node->parent, nullptr, 1);
        }
        delete node;
    }

    
};