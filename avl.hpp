#pragma once 

#include <iostream>
#include <algorithm>

// Node structure with template for type T
template <class T>
struct Node {
    T key;
    Node* left;
    Node* right;
    int height;

    Node(const T& _key) : key(_key), left(nullptr), right(nullptr), height(1) {}

    static int get_height(Node* node) {
        return node ? node->height : 0;
    }

    static int get_balance(Node* node) {
        return node ? get_height(node->left) - get_height(node->right) : 0;
    }

    static Node* right_rotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        // Perform rotation
        x->right = y;
        y->left = T2;

        // Update heights
        y->height = std::max(get_height(y->left), get_height(y->right)) + 1;
        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;

        // Return new root
        return x;
    }

    static Node* left_rotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        // Perform rotation
        y->left = x;
        x->right = T2;

        // Update heights
        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;
        y->height = std::max(get_height(y->left), get_height(y->right)) + 1;

        // Return new root
        return y;
    }

    static Node* min_value_node(Node* node) {
        Node* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }
};

// AVLTree class with custom comparator
template <class T, class Compare = std::less<T>>
class AVLTree {
public:
    AVLTree() : root(nullptr), comp(Compare()) {}
    
    // copy constructor 
    AVLTree(const AVLTree& other) : root(nullptr), comp(other.comp){
        root = copy_tree(other.root);

    }

    // Move Constructor 
    AVLTree(AVLTree&& other) noexcept : root(other.root), comp(other.comp){
        other.root = nullptr;
    }

    // copy assignment 
    AVLTree& operator=(const AVLTree& other){
        if(this != &other){
            delete_tree(root);
            root = copy_tree(other.root);
            comp = other.comp;
        }
        return *this;
    }

    // Move assignment 
    AVLTree& operator=(AVLTree&& other) noexcept {
        if(this != &other){
            delete_tree(root);
            root = other.root;
            comp = std::move(other.comp);
            other.root = nullptr;
        }
        return *this;
    }

    ~AVLTree() {
        delete_tree(root);
    }
    void insert(const T& key) {
        root = _insert(root, key);
    }

    void remove(const T& key) {
        root = _remove(root, key);
    }

    void inorder() {
        _inorder(root);
        std::cout << std::endl;
    }

    bool search(T key){
        auto curr = root;

        while(curr != nullptr){
            if(comp(key,curr->key)){
                curr = curr->left;
            }else if(comp(curr->key,key)){
                curr = curr->right;
            }else{
                return true;
            }
        }
        return false;
    }

private:
    Node<T>* root;
    Compare comp;

    Node<T>* copy_tree(Node<T>* node) {
        if (!node) return nullptr;
        Node<T>* new_node = new Node<T>(node->key);
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        new_node->height = node->height;
        return new_node;
    }

    void delete_tree(Node<T>* node) {
        if (node) {
            delete_tree(node->left);
            delete_tree(node->right);
            delete node;
        }
    }

    Node<T>* _insert(Node<T>* node, const T& key) {
        if (node == nullptr)
            return new Node<T>(key);

        if (comp(key, node->key))
            node->left = _insert(node->left, key);
        else if (comp(node->key, key))
            node->right = _insert(node->right, key);
        else
            return node;

        node->height = 1 + std::max(Node<T>::get_height(node->left), Node<T>::get_height(node->right));

        int balance = Node<T>::get_balance(node);

        // Left Left Case
        if (balance > 1 && comp(key, node->left->key))
            return Node<T>::right_rotate(node);

        // Right Right Case
        if (balance < -1 && comp(node->right->key, key))
            return Node<T>::left_rotate(node);

        // Left Right Case
        if (balance > 1 && comp(node->left->key, key)) {
            node->left = Node<T>::left_rotate(node->left);
            return Node<T>::right_rotate(node);
        }

        // Right Left Case
        if (balance < -1 && comp(key, node->right->key)) {
            node->right = Node<T>::right_rotate(node->right);
            return Node<T>::left_rotate(node);
        }

        return node;
    }

    Node<T>* _remove(Node<T>* root, const T& key) {
        if (root == nullptr)
            return root;

        if (comp(key, root->key))
            root->left = _remove(root->left, key);
        else if (comp(root->key, key))
            root->right = _remove(root->right, key);
        else {
            if ((root->left == nullptr) || (root->right == nullptr)) {
                Node<T>* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else
                    *root = *temp;
                delete temp;
            } else {
                Node<T>* temp = Node<T>::min_value_node(root->right);
                root->key = temp->key;
                root->right = _remove(root->right, temp->key);
            }
        }

        if (root == nullptr)
            return root;

        root->height = std::max(Node<T>::get_height(root->left), Node<T>::get_height(root->right)) + 1;

        int balance = Node<T>::get_balance(root);

        if (balance > 1 && Node<T>::get_balance(root->left) >= 0)
            return Node<T>::right_rotate(root);

        if (balance > 1 && Node<T>::get_balance(root->left) < 0) {
            root->left = Node<T>::left_rotate(root->left);
            return Node<T>::right_rotate(root);
        }

        if (balance < -1 && Node<T>::get_balance(root->right) <= 0)
            return Node<T>::left_rotate(root);

        if (balance < -1 && Node<T>::get_balance(root->right) > 0) {
            root->right = Node<T>::right_rotate(root->right);
            return Node<T>::left_rotate(root);
        }

        return root;
    }

    void _inorder(Node<T>* root) {
        if (root != nullptr) {
            _inorder(root->left);
            std::cout << root->key << " ";
            _inorder(root->right);
        }
    }
};


#if 0
int main() {
    // Create an AVL tree with a custom comparator (greater to make it decreasing order)
    AVLTree<int> avlTree;

    avlTree.insert(1);
    avlTree.insert(2);
    avlTree.insert(3);
    avlTree.insert(4);
    avlTree.insert(5);

    avlTree.inorder();

    std::cout << avlTree.search(20) << std::endl;
    return 0;
}

#endif