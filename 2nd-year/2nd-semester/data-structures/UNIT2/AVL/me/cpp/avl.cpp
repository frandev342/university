#include "avl.hpp"

#include <algorithm>
#include <new>

AVLTree::Node::Node(int node_value)
    : value(node_value), height(0), left(NULL), right(NULL) {}

AVLTree::AVLTree() : root_(NULL), size_(0) {}

AVLTree::~AVLTree() {
    clear();
}

int AVLTree::nodeHeight(const Node* node) {
    return node == NULL ? -1 : node->height;
}

void AVLTree::updateHeight(Node* node) {
    node->height = 1 + std::max(nodeHeight(node->left), nodeHeight(node->right));
}

int AVLTree::balanceFactor(const Node* node) {
    return nodeHeight(node->right) - nodeHeight(node->left);
}

// Rotations update only the two affected nodes, so each is O(1).
AVLTree::Node* AVLTree::rotateLeft(Node* node) {
    Node* new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    updateHeight(node);
    updateHeight(new_root);
    return new_root;
}

AVLTree::Node* AVLTree::rotateRight(Node* node) {
    Node* new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    updateHeight(node);
    updateHeight(new_root);
    return new_root;
}

AVLTree::Node* AVLTree::rebalance(Node* node) {
    updateHeight(node);
    const int balance = balanceFactor(node);

    if (balance > 1) {
        if (balanceFactor(node->right) < 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }
    if (balance < -1) {
        if (balanceFactor(node->left) > 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    return node;
}

AVLTree::Node* AVLTree::insertNode(Node* node, int value, bool& inserted) {
    if (node == NULL) {
        Node* created = new (std::nothrow) Node(value);
        inserted = created != NULL;
        return created;
    }

    if (value <= node->value) {
        Node* child = insertNode(node->left, value, inserted);
        if (!inserted) {
            return node;
        }
        node->left = child;
    } else {
        Node* child = insertNode(node->right, value, inserted);
        if (!inserted) {
            return node;
        }
        node->right = child;
    }
    return rebalance(node);
}

bool AVLTree::insert(int value) {
    bool inserted = false;
    root_ = insertNode(root_, value, inserted);
    if (inserted) {
        ++size_;
    }
    return inserted;
}

AVLTree::Node* AVLTree::minimumNode(Node* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

AVLTree::Node* AVLTree::eraseNode(Node* node, int value, bool& erased) {
    if (node == NULL) {
        return NULL;
    }

    if (value < node->value) {
        node->left = eraseNode(node->left, value, erased);
    } else if (value > node->value) {
        node->right = eraseNode(node->right, value, erased);
    } else {
        erased = true;
        if (node->left == NULL || node->right == NULL) {
            Node* replacement = node->left != NULL ? node->left : node->right;
            delete node;
            return replacement;
        }

        Node* successor = minimumNode(node->right);
        node->value = successor->value;
        bool successor_erased = false;
        node->right = eraseNode(node->right, successor->value, successor_erased);
    }

    return rebalance(node);
}

bool AVLTree::erase(int value) {
    bool erased = false;
    root_ = eraseNode(root_, value, erased);
    if (erased) {
        --size_;
    }
    return erased;
}

bool AVLTree::contains(int value) const {
    const Node* node = root_;
    while (node != NULL) {
        if (value == node->value) {
            return true;
        }
        node = value < node->value ? node->left : node->right;
    }
    return false;
}

std::size_t AVLTree::size() const {
    return size_;
}

int AVLTree::height() const {
    return nodeHeight(root_);
}

bool AVLTree::empty() const {
    return size_ == 0;
}

bool AVLTree::minimum(int& value) const {
    if (root_ == NULL) {
        return false;
    }
    const Node* node = root_;
    while (node->left != NULL) {
        node = node->left;
    }
    value = node->value;
    return true;
}

bool AVLTree::maximum(int& value) const {
    if (root_ == NULL) {
        return false;
    }
    const Node* node = root_;
    while (node->right != NULL) {
        node = node->right;
    }
    value = node->value;
    return true;
}

void AVLTree::destroy(Node* node) {
    if (node == NULL) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void AVLTree::clear() {
    destroy(root_);
    root_ = NULL;
    size_ = 0;
}

bool AVLTree::validateNode(const Node* node, const int* lower,
                           const int* upper, std::size_t& count,
                           int& computed_height) {
    if (node == NULL) {
        computed_height = -1;
        return true;
    }
    if ((lower != NULL && node->value < *lower) ||
        (upper != NULL && node->value > *upper)) {
        return false;
    }

    int left_height = -1;
    int right_height = -1;
    if (!validateNode(node->left, lower, &node->value, count, left_height) ||
        !validateNode(node->right, &node->value, upper, count, right_height)) {
        return false;
    }

    computed_height = 1 + std::max(left_height, right_height);
    ++count;
    const int difference = right_height - left_height;
    return node->height == computed_height && difference >= -1 && difference <= 1;
}

bool AVLTree::valid() const {
    std::size_t count = 0;
    int computed_height = -1;
    return validateNode(root_, NULL, NULL, count, computed_height) &&
           count == size_ && computed_height == height();
}

void AVLTree::appendInorder(const Node* node, std::vector<int>& values) {
    if (node == NULL) {
        return;
    }
    appendInorder(node->left, values);
    values.push_back(node->value);
    appendInorder(node->right, values);
}

std::vector<int> AVLTree::inorder() const {
    std::vector<int> values;
    values.reserve(size_);
    appendInorder(root_, values);
    return values;
}
