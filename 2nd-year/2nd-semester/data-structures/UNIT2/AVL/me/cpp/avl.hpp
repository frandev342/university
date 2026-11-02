#ifndef AVL_HPP
#define AVL_HPP

#include <cstddef>
#include <vector>

class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    // Mutations and lookup are O(log n) for a valid AVL tree.
    bool insert(int value);
    bool erase(int value);
    bool contains(int value) const;

    // Cached metadata queries are O(1). Empty height is -1; leaf height is 0.
    std::size_t size() const;
    int height() const;
    bool empty() const;

    // Extrema are O(log n); false means the tree is empty.
    bool minimum(int& value) const;
    bool maximum(int& value) const;

    // clear is O(n), valid is O(n), and inorder is O(n).
    void clear();
    bool valid() const;
    std::vector<int> inorder() const;

private:
    struct Node {
        explicit Node(int node_value);

        int value;
        int height;
        Node* left;
        Node* right;
    };

    Node* root_;
    std::size_t size_;

    static int nodeHeight(const Node* node);
    static void updateHeight(Node* node);
    static int balanceFactor(const Node* node);
    static Node* rotateLeft(Node* node);
    static Node* rotateRight(Node* node);
    static Node* rebalance(Node* node);

    static Node* insertNode(Node* node, int value, bool& inserted);
    static Node* eraseNode(Node* node, int value, bool& erased);
    static Node* minimumNode(Node* node);
    static void destroy(Node* node);
    static void appendInorder(const Node* node, std::vector<int>& values);
    static bool validateNode(const Node* node, const int* lower,
                             const int* upper, std::size_t& count,
                             int& computed_height);
};

#endif
