# 41343115

# Tree Traversal

## 解題說明

**問題描述**  
實作二元樹（Binary Tree）的四種基本走訪方式，包含 `preorder`、`inorder`、`postorder` 與 `levelorder`。程式需建立一棵簡單的二元樹，並依序輸出四種走訪結果，以觀察不同走訪規則下的節點拜訪順序。

**解題策略**  
1. **資料結構**：使用 `struct Node` 表示二元樹節點，每個節點包含資料欄位 `data`，以及左右子節點指標 `left`、`right`。  
2. **深度優先走訪**：`preorder`、`inorder`、`postorder` 皆使用遞迴實作。三者差異只在於「根節點」輸出的時機不同。  
3. **廣度優先走訪**：`levelorder` 使用 `queue` 逐層處理節點，先將根節點放入佇列，再依序取出並將其左右子節點加入。  
4. **結果驗證**：以固定結構的測試樹驗證四種走訪輸出是否符合定義，並比較 DFS 與 BFS 的差異。

## 程式實作

> 檔名：`main.cpp`，可以 `g++` 編譯執行。

```cpp
#include <iostream>
#include <queue>
using namespace std;

struct Node {
    int data;
    Node* left;
    Node* right;
};

Node* newNode(int value) {
    Node* temp = new Node;
    temp->data = value;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

void preorder(Node* root) {
    if (root == NULL) {
        return;
    }
    cout << root->data << " ";
    preorder(root->left);
    preorder(root->right);
}

void inorder(Node* root) {
    if (root == NULL) {
        return;
    }
    inorder(root->left);
    cout << root->data << " ";
    inorder(root->right);
}

void postorder(Node* root) {
    if (root == NULL) {
        return;
    }
    postorder(root->left);
    postorder(root->right);
    cout << root->data << " ";
}

void levelorder(Node* root) {
    if (root == NULL) {
        return;
    }

    queue<Node*> q;
    q.push(root);

    while (q.empty() == false) {
        Node* current = q.front();
        q.pop();

        cout << current->data << " ";

        if (current->left != NULL) {
            q.push(current->left);
        }
        if (current->right != NULL) {
            q.push(current->right);
        }
    }
}

int main() {
    Node* root = newNode(1);
    root->left = newNode(2);
    root->right = newNode(3);
    root->left->left = newNode(4);
    root->left->right = newNode(5);
    root->right->right = newNode(6);

    cout << "preorder: ";
    preorder(root);
    cout << endl;

    cout << "inorder: ";
    inorder(root);
    cout << endl;

    cout << "postorder: ";
    postorder(root);
    cout << endl;

    cout << "levelorder: ";
    levelorder(root);
    cout << endl;

    return 0;
}
```

## 效能分析

設二元樹共有 `n` 個節點。

- **`preorder` / `inorder` / `postorder`**：每個節點都只會被拜訪一次，因此時間複雜度皆為 `O(n)`。  
- **`levelorder`**：每個節點最多進入佇列一次、離開佇列一次，因此時間複雜度也是 `O(n)`。  
- **空間複雜度**：  
  - 遞迴版本的 `preorder`、`inorder`、`postorder` 需使用系統呼叫堆疊，最差情況下為 `O(h)`，其中 `h` 為樹高；若樹退化成鏈結串列，最差可達 `O(n)`。  
  - `levelorder` 額外使用 `queue`，在最寬層節點數最多時，空間複雜度為 `O(w)`，其中 `w` 為樹的最大寬度。

## 測試與驗證

編譯與執行結果：

```shell
preorder: 1 2 4 5 3 6
inorder: 4 2 5 1 3 6
postorder: 4 5 2 6 3 1
levelorder: 1 2 3 4 5 6
```

驗證點：
1. **Preorder**：先輸出根節點，再輸出左子樹與右子樹，因此結果為 `1 2 4 5 3 6`。  
2. **Inorder**：先左子樹，再根節點，最後右子樹，因此結果為 `4 2 5 1 3 6`。  
3. **Postorder**：先完成左右子樹處理，再輸出根節點，因此結果為 `4 5 2 6 3 1`。  
4. **Levelorder**：依照層數由上而下、由左至右輸出，因此結果為 `1 2 3 4 5 6`。

## 申論及開發報告

- **遞迴走訪的特性**：`preorder`、`inorder`、`postorder` 都屬於深度優先搜尋（DFS），程式結構簡潔，適合表達樹狀結構的遞迴性質。  
- **不同走訪的用途**：  
  - `preorder` 常用於複製樹、輸出樹的結構。  
  - `inorder` 在二元搜尋樹（BST）中可得到遞增排序結果。  
  - `postorder` 適合先處理子節點，再處理父節點，例如刪除整棵樹。  
  - `levelorder` 適合觀察每一層的節點分布，也常用在最短步數、層級關係等問題。  
- **實作上的差異**：前三種走訪主要依靠遞迴呼叫順序決定輸出結果；`levelorder` 則必須使用佇列，因為它不是一路往下，而是依照層級逐步展開。  
- **程式設計重點**：本題雖然程式不長，但能清楚展示二元樹的基本概念，也是後續學習 BST、Heap、Graph Traversal 的基礎。
