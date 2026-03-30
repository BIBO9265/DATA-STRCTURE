#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
using namespace std;

struct Node {
	int key;
	Node* left;
	Node* right;

	Node(int k) : key(k), left(nullptr), right(nullptr) {}
};

class BST {
private:
	Node* root;

	Node* insert(Node* node, int key) {
		if (node == nullptr) {
			return new Node(key);
		}

		if (key < node->key) {
			node->left = insert(node->left, key);
		}else if (key > node->key) {
			node->right = insert(node->right, key);
		}
		return node;
	}

	int height(Node* node) const {
		if (node == nullptr) {
			return 0;
		}
		return 1 + max(height(node->left), height(node->right));
	}

	void clear(Node* node) {
		if (node == nullptr) {
			return;
		}
		clear(node->left);
		clear(node->right);
		delete node;
	}

public:
	BST() : root(nullptr) {}

	~BST() {
		clear(root);
	}

	void insert(int key) {
		root = insert(root, key);
	}

	int height() const {
		return height(root);
	}
};

int main() {
	vector<int> n_values = { 100, 500, 1000, 2000, 3000, 4000, 5000,
							6000, 7000, 8000, 9000, 10000 };

	random_device rd;
	mt19937 gen(rd());

	ofstream fout("bst_height_ratio.csv");
	fout << "n,height,ratio\n";

	for (int n : n_values) {
		BST tree;

		vector<int> values;
		values.reserve(n * 2);
		for (int i = 1; i <= n * 2; i++) {
			values.push_back(i);
		}

		shuffle(values.begin(), values.end(), gen);

		for (int i = 0; i < n; i++) {
			tree.insert(values[i]);
		}

		int h = tree.height();
		double ratio = h / log2((double)n);

		cout << "n = " << n
			<< ", height = " << h
			<< ", height/log2(n) = " << ratio << endl;

		fout << n << "," << h << "," << ratio << "\n";
	}

	fout.close();
	cout << "\nData saved to bst_height_ratio.csv" << endl;

	return 0;
}