# 41343115

# Polynomial

## 解題說明

**問題描述**  
實作多項式 ADT，使用 帶標頭節點的環狀鏈結串列來儲存每一項 `Term(coef, exp)`。需支援加法運算 `operator+`、輸入輸出 `<<`，並實作可用空間串列機制以優化記憶體管理（減少 `new`/`delete` 系統呼叫）。

**解題策略**  
1. **資料結構**：設計 `ChainNode<T>` 與 `Chain<T>`。`Chain` 為環狀結構，包含一個空的 Header Node，尾端節點指回 Header。  
2. **記憶體優化**：在 `ChainNode` 中維護一靜態成員 `av` (Available List)。重載 `operator new` 與 `operator delete`，優先從 `av` 回收桶存取節點。`Chain` 的解構子採用 **O(1) 串列回收**（將整條鏈結掛回 `av` 前端）。  
3. **多項式運算**：`Polynomial` 內部包含一個 `Chain<Term>`。加法運算採用 **雙指標 (Two-pointer)** 策略，同步遍歷兩個依指數降冪排列的串列進行合併。  
4. **錯誤處理**：使用 `std::cerr` 與 `exit(1)` 處理空指標解引用。

## 程式實作

> 檔名：`main.cpp`，可以 `g++` 編譯執行。

```cpp
#include <iostream>
#include <cstdlib>

template <class T> class Chain;
template <class T> class ChainIterator;
template <class T> class Polynomial;

// 0. Term Structure
template <class T>
struct Term {
    T coef;
    int exp;

    void set(T c, int e) { coef = c; exp = e; }

    friend std::ostream& operator<<(std::ostream& os, const Term<T>& t) {
        os << t.coef << "x^" << t.exp;
        return os;
    }
};

// 1. ChainNode
template <class T>
class ChainNode {
    friend class Chain<T>;
    friend class ChainIterator<T>;
    template <class U> friend class Polynomial;

private:
    T data;
    ChainNode<T>* link;

public:
    static ChainNode<T>* av;

    ChainNode(const T& element, ChainNode<T>* next = nullptr)
        : data(element), link(next) {
    }
    ChainNode(ChainNode<T>* next = nullptr) : link(next) {}

    void* operator new(size_t size) {
        ChainNode<T>* p;
        if (av) {
            p = av;
            av = av->link;
        }
        else {
            p = ::new ChainNode<T>;
        }
        return p;
    }

    void operator delete(void* p) {
        ChainNode<T>* node = static_cast<ChainNode<T>*>(p);
        node->link = av;
        av = node;
    }
};

template <class T>
ChainNode<T>* ChainNode<T>::av = nullptr;

// 2. ChainIterator
template <class T>
class ChainIterator {
    friend class Chain<T>;
    template <class U> friend class Polynomial;

private:
    ChainNode<T>* current;

public:
    ChainIterator(ChainNode<T>* startNode = nullptr) : current(startNode) {}

    T& operator*() const {
        if (current == nullptr) {
            std::cerr << "Error: Null iterator" << std::endl;
            exit(1);
        }
        return current->data;
    }

    T* operator->() const {
        if (current == nullptr) {
            std::cerr << "Error: Null iterator" << std::endl;
            exit(1);
        }
        return &current->data;
    }

    ChainIterator& operator++() {
        if (current) current = current->link;
        return *this;
    }

    ChainIterator operator++(int) {
        ChainIterator old = *this;
        if (current) current = current->link;
        return old;
    }

    bool operator!=(const ChainIterator& right) const { return current != right.current; }
    bool operator==(const ChainIterator& right) const { return current == right.current; }
};

// 3. Chain
template <class T>
class Chain {
    friend class ChainIterator<T>;
    template <class U> friend class Polynomial;

private:
    ChainNode<T>* first;

public:
    typedef ChainIterator<T> iterator;

    Chain() {
        first = new ChainNode<T>();
        first->link = first;
    }

    ~Chain() {
        if (first) {
            ChainNode<T>* last = first->link;
            while (last->link != first) {
                last = last->link;
            }
            last->link = ChainNode<T>::av;
            ChainNode<T>::av = first;
            first = nullptr;
        }
    }

    Chain(const Chain<T>& other) {
        first = new ChainNode<T>();
        first->link = first;
        ChainNode<T>* source = other.first->link;
        while (source != other.first) {
            InsertBack(source->data);
            source = source->link;
        }
    }

    const Chain<T>& operator=(const Chain<T>& other) {
        if (this != &other) {
            ChainNode<T>* current = first->link;
            while (current != first) {
                ChainNode<T>* next = current->link;
                delete current;
                current = next;
            }
            first->link = first;

            ChainNode<T>* source = other.first->link;
            while (source != other.first) {
                InsertBack(source->data);
                source = source->link;
            }
        }
        return *this;
    }

    iterator Begin() const { return iterator(first->link); }
    iterator End() const { return iterator(first); }

    void InsertBack(const T& e) {
        ChainNode<T>* newNode = new ChainNode<T>(e);
        ChainNode<T>* last = first;
        while (last->link != first) {
            last = last->link;
        }
        last->link = newNode;
        newNode->link = first;
    }
};

// 4. Polynomial
template <class T>
class Polynomial {
private:
    Chain<Term<T>> polyList;

public:
    Polynomial() {}

    void AddTerm(T coef, int exp) {
        Term<T> t;
        t.set(coef, exp);
        polyList.InsertBack(t);
    }

    Polynomial<T> operator+(const Polynomial<T>& b) const {
        Polynomial<T> c;
        auto aIter = polyList.Begin();
        auto bIter = b.polyList.Begin();
        auto aEnd = polyList.End();
        auto bEnd = b.polyList.End();

        while (aIter != aEnd && bIter != bEnd) {
            if (aIter->exp == bIter->exp) {
                T sum = aIter->coef + bIter->coef;
                if (sum != 0) c.AddTerm(sum, aIter->exp);
                ++aIter; ++bIter;
            }
            else if (aIter->exp > bIter->exp) {
                c.AddTerm(aIter->coef, aIter->exp);
                ++aIter;
            }
            else {
                c.AddTerm(bIter->coef, bIter->exp);
                ++bIter;
            }
        }
        while (aIter != aEnd) {
            c.AddTerm(aIter->coef, aIter->exp);
            ++aIter;
        }
        while (bIter != bEnd) {
            c.AddTerm(bIter->coef, bIter->exp);
            ++bIter;
        }
        return c;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polynomial<T>& p) {
        auto it = p.polyList.Begin();
        auto end = p.polyList.End();
        bool first = true;
        for (; it != end; ++it) {
            if (!first && it->coef >= 0) os << " + ";
            else if (!first) os << " ";
            os << it->coef << "x^" << it->exp;
            first = false;
        }
        if (first) os << "0";
        return os;
    }
};

int main() {
    Polynomial<int> A, B;

    // A = 3x^14 + 2x^8 + 1
    A.AddTerm(3, 14);
    A.AddTerm(2, 8);
    A.AddTerm(1, 0);

    // B = 8x^14 - 3x^10 + 10x^6
    B.AddTerm(8, 14);
    B.AddTerm(-3, 10);
    B.AddTerm(10, 6);

    std::cout << "A: " << A << std::endl;
    std::cout << "B: " << B << std::endl;

	// Add
    Polynomial<int> C = A + B;
    std::cout << "C (A+B): " << C << std::endl;

    return 0;
}
```

## 效能分析

設 `n` 與 `m` 分別為兩多項式的項數。

- **`AddTerm` / `InsertBack`**：因未維護 `last` 指標，每次插入需遍歷至串列尾端，單次操作為 `O(k)`（`k` 為當前長度），建立長度 `n` 的多項式總時間為 `O(n^2)`。若改進維護 `last` 指標可降至 `O(n)`。  
- **`operator+` (加法)**：採用 Merge 合併邏輯，同步遍歷兩串列，時間複雜度為 `O(n+m)`。  
- **`~Chain` (Available List 回收)**：  
  - 遍歷尋找尾端節點：`O(n)`（未維護尾指標時）。  
  - 指標操作掛回 `av` list：`O(1)`。  
  - **優勢**：相較於標準 `delete` 需呼叫 `n` 次釋放，Available List 主要為指標操作，降低常數開銷。  
- **空間複雜度**：`O(n)`，每項需額外儲存一個 `link` 指標。

## 測試與驗證

編譯與執行結果：

```shell
A: 3x^14 + 2x^8 + 1x^0
B: 8x^14 -3x^10 + 10x^6
C (A+B): 11x^14 -3x^10 + 2x^8 + 10x^6 + 1x^0
```

驗證點：
1. **同次項合併**：同次方（如 `x^14`）係數相加，若為 0 則略過。  
2. **次序保持**：結果依指數由大到小排列 (14 → 10 → 8 → 6 → 0)。  
3. **格式輸出**：正負號處理正確，首項無額外 `+`。

## 申論及開發報告

- **Available List 的效益**：在多項式乘法或複雜運算中會產生大量暫存節點。透過 `static av` 暫存已刪除節點，避免頻繁 `new/delete` 的系統配置/釋放成本，提升效率。  
- **環狀串列的優勢**：帶 Header 的環狀串列可簡化邊界條件；遍歷終止條件一致（`current == head`），減少空指標分支。  
- **安全性設計**：提供 Deep Copy（Copy Constructor 與 Assignment Operator），避免淺拷貝造成 Double Free。
