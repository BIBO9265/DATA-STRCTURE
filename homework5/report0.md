# 41343115
# Min Priority Queue
## 解題說明
**問題描述**  
本題要求參考 ADT 5.2 的 `MaxPQ` 抽象類別，改寫出定義最小優先佇列的 `MinPQ`，並撰寫一個 `MinHeap` 類別繼承自 `MinPQ`，完整實作 `IsEmpty()`、`Top()`、`Push()`、`Pop()` 等虛擬函式。各函式的時間複雜度需與對應的 `MaxHeap` 相同。

**解題策略**  
1. **抽象介面設計**：先以 template 撰寫 `MinPQ<T>` 抽象類別，宣告最小優先佇列必備操作，並將成員函式設為 pure virtual。  
2. **陣列式堆積實作**：以動態陣列表示 complete binary tree，採 1-based indexing 簡化父子節點位置計算。  
3. **插入操作**：新元素先放入尾端，再用 `BubbleUp` 向上調整，使父節點永遠不大於子節點。  
4. **刪除最小值**：將根節點移除後，以最後一個元素補到根，再用 `BubbleDown` 向下調整恢復 min-heap 性質。  
5. **例外處理**：當 heap 為空時呼叫 `Top()` 或 `Pop()`，以 `throw` 回報錯誤，避免非法存取。

## 程式實作
> 檔名：`main0.cpp`，可以 `g++` 編譯執行。

```cpp
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

template <class T>
class MinPQ {
public:
    virtual ~MinPQ() {}
    virtual bool IsEmpty() const = 0;
    virtual const T& Top() const = 0;
    virtual void Push(const T&) = 0;
    virtual void Pop() = 0;
};

template <class T>
class MinHeap : public MinPQ<T> {
private:
    vector<T> heap;

    void HeapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index] < heap[parent]) {
                swap(heap[index], heap[parent]);
                index = parent;
            }else {
                break;
            }
        }
    }

    void HeapifyDown(int index) {
        int n = heap.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < n && heap[left] < heap[smallest]){
                smallest = left;
            }
            if (right < n && heap[right] < heap[smallest]){
                smallest = right;
            }
            if (smallest != index) {
                swap(heap[index], heap[smallest]);
                index = smallest;
            }else {
                break;
            }
        }
    }

public:
    MinHeap() {}

    bool IsEmpty() const override {
        return heap.empty();
    }

    const T& Top() const override {
        if (IsEmpty()){
            throw out_of_range("MinHeap Empty");
        }
        return heap[0];
    }

    void Push(const T& x) override {
        heap.push_back(x);
        HeapifyUp(heap.size() - 1);
    }

    void Pop() override {
        if (IsEmpty()){
            throw out_of_range("MinHeap Empty");
        }
        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty()){
            HeapifyDown(0);
        }
    }

    void Print() const {
        for (const auto& x : heap){
            cout << x << " ";
        }
        cout << endl;
    }
};

int main() {
    MinHeap<int> h;

    h.Push(40);
    h.Push(10);
    h.Push(30);
    h.Push(5);
    h.Push(20);

    cout << "Heap: ";
    h.Print();

    cout << "Top = " << h.Top() << endl;

    h.Pop();
    cout << "After Pop, Top = " << h.Top() << endl;

    return 0;
}
```

## 效能分析
- **`IsEmpty()`**：只檢查 `heapSize`，時間複雜度為 `O(1)`。  
- **`Top()`**：直接回傳根節點 `heap[1]`，時間複雜度為 `O(1)`。  
- **`Push()`**：插入後最多沿樹高向上交換一次，時間複雜度為 `O(log n)`。若發生陣列擴充，單次重新配置為 `O(n)`，但攤銷後仍可視為插入成本主要受 `BubbleUp` 支配。  
- **`Pop()`**：根節點刪除後最多沿樹高向下交換一次，時間複雜度為 `O(log n)`。  
- **空間複雜度**：heap 陣列需儲存所有元素，為 `O(n)`。

## 測試與驗證
編譯與執行結果：

```shell
Heap: 5 10 30 40 20
Top = 5
After Pop, Top = 10
```

驗證點：  
1. **最小值位於根節點**：插入 `40, 10, 30, 5, 20` 後，`Top()` 應回傳 `5`。  
2. **刪除後仍維持 min-heap**：刪除最小值 `5` 後，新根節點應為 `10`。  
3. **介面符合題意**：`MinHeap` 透過繼承 `MinPQ` 完成所有虛擬函式實作。  
4. **複雜度符合要求**：各操作與課本 `MaxHeap` 對應操作相同，只是比較方向相反。

## 申論及開發報告
- **與 MaxHeap 的本質差異**：資料結構本身不變，真正不同的是比較規則。`MaxHeap` 維持父節點最大，`MinHeap` 則維持父節點最小。  
- **抽象類別的必要性**：以 `MinPQ` 明確定義介面後，可讓其他最小優先佇列實作（例如 leftist tree、binomial heap）共用同一組操作規格。  
- **1-based indexing 的優點**：父節點與子節點位置分別可直接寫成 `i/2`、`2i`、`2i+1`，實作簡潔，較貼近教科書推導。  
- **實務限制**：若資料型別 `T` 不支援 `<` 比較運算子，則需另外注入 comparator，否則無法泛型化到所有型別。
