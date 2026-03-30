# 41343115
# Binary Search Tree (b)
## 解題說明
**問題描述**  
本題要求撰寫一個 C++ 函式，從 binary search tree 中刪除 key 為 `k` 的 pair，並分析該函式的時間複雜度。

**解題策略**  
1. **遞迴搜尋目標節點**：根據 BST 性質，若 `k` 較小則往左子樹找，若 `k` 較大則往右子樹找。  
2. **分三種刪除情況處理**：  
   - 節點沒有子節點：直接刪除。  
   - 節點只有一個子節點：以其子節點直接接回父節點。  
   - 節點有兩個子節點：找出右子樹中最小節點作為 inorder successor，將其資料複製到目前節點，再遞迴刪除 successor。  
3. **輔助函式 `findMin()`**：用來找到某子樹中的最小 key 節點。  
4. **維持 BST 性質**：整個刪除過程需確保左子樹鍵值仍小於根、右子樹鍵值仍大於根。

## 程式實作
> 檔名：`main2.cpp`，可以 `g++` 編譯執行。

```cpp
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
```

## 效能分析
- **搜尋要刪除的節點**：沿著 BST 高度往下搜尋，時間複雜度為 `O(h)`，其中 `h` 為樹高。  
- **找 inorder successor**：若目標節點有兩個子節點，需要在右子樹中一路往左找最小節點，成本同樣為 `O(h)`。  
- **整體刪除操作**：時間複雜度為 `O(h)`。  
- **若 BST 平衡**：`h = O(log n)`，故刪除平均可視為 `O(log n)`。  
- **最壞情況**：若 BST 退化成鏈結串列，則 `h = O(n)`，時間複雜度退化為 `O(n)`。  
- **空間複雜度**：若採遞迴實作，呼叫堆疊深度為 `O(h)`。

## 測試與驗證
執行結果範例：

```shell
Before delete: (20, D) (30, B) (40, E) (50, A) (60, F) (70, C) (80, G)
After delete 50: (20, D) (30, B) (40, E) (60, F) (70, C) (80, G)
```

驗證點：  
1. **中序走訪仍遞增**：刪除前後的 inorder 結果都必須依 key 由小到大排列，表示 BST 性質未被破壞。  
2. **刪除兩子節點案例正確**：範例刪除 `50` 時，會以右子樹最小值 `60` 補上。  
3. **其他節點保留完整**：除了被刪除 key 之外，其餘 pair 應完整留在樹中。  
4. **函式可擴充到一般 template 型別**：key 與 element 使用 template 設計，符合教材泛型資料結構風格。

## 申論及開發報告
- **兩子節點情況最關鍵**：單子節點或葉節點的刪除相對直接，真正容易出錯的是兩子節點時 successor 或 predecessor 的替換邏輯。  
- **為何常選 successor**：使用右子樹最小值能保證替換後仍大於左子樹所有節點，且不大於右子樹其餘節點，容易維持 BST 結構；當然也可改用左子樹最大值 predecessor。  
- **遞迴寫法的可讀性**：遞迴可直接反映 BST 的分治結構，程式短而清楚；若要避免函式呼叫堆疊，也可改寫為迭代版本。  
- **普通 BST 的限制**：雖然刪除邏輯正確，但若樹長期不平衡，刪除效率仍可能退化到線性時間，因此高效系統通常搭配 AVL Tree 或 Red-Black Tree。
