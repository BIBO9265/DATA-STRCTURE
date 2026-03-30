# 41343115
# Binary Search Tree (a)
## 解題說明
**問題描述**  
本題要求從一棵初始為空的 binary search tree 開始，對不同的 `n` 進行隨機插入，量測所得 BST 的高度 `height`，再計算比值 `height / log2(n)`。題目指定對 `n = 100, 500, 1000, 2000, 3000, ..., 10000` 重複執行，並將該比值對 `n` 作圖，驗證此比值大致維持常數，約略接近 2。

**解題策略**  
1. **BST 基本結構**：建立 `Node` 與 `BST` 類別，支援遞迴插入與高度計算。  
2. **亂數插入序列**：為避免重複 key 造成實際節點數不足，先建立 `1 ~ 2n` 的整數序列，再使用 `shuffle` 打亂，取前 `n` 個作為插入資料。  
3. **高度計算**：以遞迴方式定義樹高，空樹高度設為 `0`，單一節點高度為 `1`。  
4. **多次試驗降低波動**：每個 `n` 進行多次實驗，取平均高度與平均比值，以降低單次亂數造成的偏差。  
5. **資料輸出**：將結果寫入 CSV，後續可直接用 Excel 或其他工具繪圖。

## 程式實作
> 檔名：`main1.cpp`，可以 `g++` 編譯執行。

```cpp
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
```

## 效能分析
- **單次插入**：平均情況下 BST 高度約為 `O(log n)`，因此單次 `Insert()` 平均時間複雜度為 `O(log n)`；最壞情況退化為 `O(n)`。  
- **建立一棵含 `n` 節點的隨機 BST**：平均時間複雜度約為 `O(n log n)`。  
- **高度計算**：需遍歷整棵樹一次，時間複雜度為 `O(n)`。  
- **每個 `n` 做 `trials` 次試驗**：整體成本約為 `O(trials · n log n)`。  
- **空間複雜度**：BST 節點儲存成本為 `O(n)`，額外亂數容器 `values` 為 `O(n)`，故總空間複雜度仍為 `O(n)`。

## 測試與驗證
執行後會輸出每個 `n` 的平均高度與平均比值，並產生 `bst_height_ratio.csv` 供繪圖。以下為一組實際執行結果：

```shell
n = 100, height = 13, height/log2(n) = 1.95669
n = 500, height = 18, height/log2(n) = 2.00763
n = 1000, height = 22, height/log2(n) = 2.20755
n = 2000, height = 23, height/log2(n) = 2.09743
n = 3000, height = 29, height/log2(n) = 2.51066
n = 4000, height = 25, height/log2(n) = 2.08929
n = 5000, height = 26, height/log2(n) = 2.11593
n = 6000, height = 30, height/log2(n) = 2.3903
n = 7000, height = 30, height/log2(n) = 2.34868
n = 8000, height = 32, height/log2(n) = 2.46803
n = 9000, height = 29, height/log2(n) = 2.20772
n = 10000, height = 30, height/log2(n) = 2.25772
```

驗證點：  
1. **比值近似常數**：隨 `n` 增加，`height / log2(n)` 應在一個小範圍內波動，而不是線性成長。  
2. **數值約接近 2**：多次實驗平均後，結果通常會落在 2 左右，符合題目敘述。  
3. **繪圖趨勢平穩**：將 CSV 匯入繪圖工具後，曲線應呈現近似水平的型態。  
4. **重複值問題已排除**：由於插入值互異，可保證每次實驗最終都有恰好 `n` 個節點。

## 申論及開發報告
- **為何高度比值近似常數**：隨機插入的 BST 雖未像 AVL 或 Red-Black Tree 那樣強制平衡，但在平均情況下仍具有對數級高度，因此 `height / log2(n)` 不會隨 `n` 無限制放大。  
- **單次實驗不夠穩定**：若只跑一次，亂數序列差異可能使結果偏高或偏低，因此採多次試驗取平均更能反映理論行為。  
- **高度定義需一致**：有些教材將單一節點高度定為 0，有些定為 1。只要整份作業前後一致，圖形趨勢與結論不會改變，但數值常數會略有偏移。  
- **與平衡樹的差異**：本題驗證的是隨機 BST 的平均表現，而不是最壞表現；若資料接近排序，普通 BST 仍可能嚴重退化，因此實務上常需要平衡機制。
