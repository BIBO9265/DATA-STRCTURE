#include <iostream>
#include <utility>
using namespace std;

template <class K, class E>
struct TreeNode {
	pair<K, E> data;
	TreeNode* left;
	TreeNode* right;

	TreeNode(const pair<K, E>& d) : data(d), left(nullptr), right(nullptr) {}
};

template <class K, class E>
TreeNode<K, E>* findMin(TreeNode<K, E>* node) {
	while (node != nullptr && node->left != nullptr) {
		node = node->left;
	}
	return node;
}

template <class K, class E>
TreeNode<K, E>* deleteNode(TreeNode<K, E>* root, const K& k) {
	if (root == nullptr) {
		return nullptr;
	}
	if (k < root->data.first) {
		root->left = deleteNode(root->left, k);
	}
	else if (k > root->data.first) {
		root->right = deleteNode(root->right, k);
	}
	else {
		if (root->left == nullptr) {
			TreeNode<K, E>* temp = root->right;
			delete root;
			return temp;
		}
		else if (root->right == nullptr) {
			TreeNode<K, E>* temp = root->left;
			delete root;
			return temp;
		}
		else {
			TreeNode<K, E>* succ = findMin(root->right);
			root->data = succ->data;
			root->right = deleteNode(root->right, succ->data.first);
		}
	}
	return root;
}

template <class K, class E>
void inorder(TreeNode<K, E>* root) {
	if (root == nullptr) {
		return;
	}
	inorder(root->left);
	cout << "(" << root->data.first << ", " << root->data.second << ") ";
	inorder(root->right);
}

int main() {
	TreeNode<int, char>* root = new TreeNode<int, char>({ 50, 'A' });
	root->left = new TreeNode<int, char>({ 30, 'B' });
	root->right = new TreeNode<int, char>({ 70, 'C' });
	root->left->left = new TreeNode<int, char>({ 20, 'D' });
	root->left->right = new TreeNode<int, char>({ 40, 'E' });
	root->right->left = new TreeNode<int, char>({ 60, 'F' });
	root->right->right = new TreeNode<int, char>({ 80, 'G' });

	cout << "Before delete: ";
	inorder(root);
	cout << endl;

	root = deleteNode(root, 50);

	cout << "After delete 50: ";
	inorder(root);
	cout << endl;

	return 0;
}