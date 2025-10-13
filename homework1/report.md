#41343115

## Ackermann函數 (Problem1)

### 1. 解題說明

#### 1.1 問題定義

本作業旨在實作 Ackermann 函數 (A(m,n))。此函數為著名的**非原始遞迴**函數，定義域為非負整數，回傳值對 (m)、(n) 的增加呈指數性增長。

Ackermann–Péter 函數的數學定義：

$$
A(m,n)=
\begin{cases}
n+1, & m=0 \
A(m-1,1), & m>0\ \land\ n=0 \
A\big(m-1,\ A(m,n-1)\big), & m>0\ \land\ n>0
\end{cases}
$$

### 1.2 解題挑戰與策略

直接依定義撰寫之**一般遞迴法**會產生大量重複計算，效能極差。本次採用**記憶化（Memoization）**最佳化：以二維陣列快取已計算的 (A(m,n))；呼叫前先查表，存在即回傳，避免重算。基於標頭限制與效能考量，使用二維陣列可達 (O(1)) 存取時間。

---

## 2. 程式實作

### 2.1 程式碼

```cpp
#include <iostream>
#include <algorithm>
#include <cstring>
#include <ctime>   

long long int vec[100][20000];
int cache_hits = 0;

long long Ackerman(int m, int n) {
    if (m >= 100 || n >= 20000) {
        return -1;
    }

    if (vec[m][n] != -1) {
        cache_hits++;
        //std::cout << "CACHE" << std::endl;
        return vec[m][n];
    }

    long long int result;
    if (m == 0) {
        result = n + 1;
    }
    else if (n == 0) {
        result = Ackerman(m - 1, 1);
    }
    else {
        long long int inner_result = Ackerman(m, n - 1);
        result = Ackerman(m - 1, inner_result);
    }
    return vec[m][n] = result;
}

int main() {
    int m = 3;
    int n = 9;
    memset(vec, -1, sizeof(vec));
    clock_t t0 = clock();
    int ans = Ackerman(m, n);
    clock_t t1 = clock();
    double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    std::cout << "Ackerman(" << m << "," << n << ") = " << ans << " (" << ms << "ms" << ")" << " CACHE = " << cache_hits << std::endl;
}
```

---

## 3. 效能分析

### 3.1 時間複雜度

* **一般遞迴法（未最佳化）**：時間複雜度與 (A(m,n)) 規模近似，存在巨量重複計算，呈指數級增長。
* **本實作（陣列記憶化）**：每個唯一狀態 ((m,n)) 僅計算一次，查表為 (O(1))，總時間與唯一狀態數成正比，遠優於一般遞迴法。

### 3.2 空間複雜度

- **快取空間**：二維陣列 `vec` 之大小為 O(M_max*N_max)。本例為 100*20000 個 `long long`，約 16 MB。
* **遞迴堆疊**：遞迴深度不可避免；記憶化能減少呼叫數量，但無法縮短**最長計算鏈**，成為瓶頸。

---

## 4. 測試與驗證

### 4.1 編譯與執行

```shell
Ackerman = 16381 (0.354ms) CACHE = 8200
[1] + Done                       "/usr/bin/gdb" --interpreter=mi --tty=${DbgTerm} 0<"/tmp/Microsoft-MIEngine-In-eq5gbvzl.33q" 1>"/tmp/Microsoft-MIEngine-Out-siqazwvq.rju"
@BIBO9265 ➜ /workspaces/DATA-STRCTURE (main) $ 
```

### 4.2 實驗數據

| 輸入 (m, n) | 一般遞迴法（時間 / 快取） |  陣列版本（時間 / 快取） |  輸出結果  |
| --------: | :------------: | :------------: | :----: |
|   A(3, 6) |    2 ms / 0    |   0 ms / 259   |   509  |
|   A(3, 7) |    6 ms / 0    |   0 ms / 516   |  1021  |
|   A(3, 8) |    24 ms / 0   |   0 ms / 1029  |  2045  |
|   A(3, 9) | Stack overflow |   1 ms / 2054  |  4093  |
|  A(3, 10) |     (無法執行)     |   0 ms / 4103  |  8189  |
|  A(3, 11) |     (無法執行)     | Stack overflow | (無法執行) |

## 5. 申論及開發報告

### 5.1 設計抉擇與考量

* 採用「**遞迴 + 二維陣列**」的記憶化方案：

  * 二維陣列：在標頭限制下是最直接的快取，存取 (O(1))。
  * 遞迴：貼近數學定義，程式直觀易讀。
* 然而其固有缺陷是無法降低**遞迴深度**，最終受限於堆疊容量。

### 5.2 困難與限制的實驗證明

* A(3,11) 失敗顯示：記憶化只能減少**廣度**（分支數），無法縮短**深度**（最長鏈）。當計算 (A(2, A(3,10))) 時，(A(3,10)=8189) 會引發極深的 (A(2,8189)) 呼叫鏈，最終堆疊耗盡。
* 若再提升輸入，例如 (A(3,13))，中間值 (A(3,12)=32765) 亦將超出 `vec[100][20000]` 的第二維上限，造成索引越界。

### 5.3 未來改進方向

* **改用迭代法（Iteration）消除遞迴**：以**手動管理**之狀態堆疊（配置於 Heap）取代呼叫堆疊，從根本排除遞迴深度限制。若解除標頭限制，結合 `std::map` 等稀疏儲存，可建構更強健的 Ackermann 計算器；亦可加入邊界檢查與溢位保護。

<br>
<br>
<br>
<br>

# Powerset (Problem2)

## 1. 解題說明

### 問題描述

輸入一組長度為 *n* 的元素集合 `set[]`，列舉其所有子集合（冪集），並逐一輸出，如 `{}`, `{a}`, `{a,b}`。

### 解題策略

採用遞迴回溯：對每個索引 `index` 做二選一分支——不選當前元素或選取當前元素。當 `index==n` 時輸出目前累積之 `subset`。此法天然覆蓋全部 `2^n` 子集，實作簡潔。

---

## 2. 程式實作

```cpp
#include <iostream>

void printSubset(char subset[], int size) {
    std::cout << "{";
    for (int i = 0; i < size; i++) {
        std::cout << subset[i];
        if (i < size - 1) std::cout << ",";
    }
    std::cout << "}" << std::endl;
}

void powerset(char set[], int n, int index, char subset[], int subsetSize) {
    if (index == n) {
        printSubset(subset, subsetSize);
        return;
    }
    // 不選 set[index]
    powerset(set, n, index + 1, subset, subsetSize);
    // 選 set[index]
    subset[subsetSize] = set[index];
    powerset(set, n, index + 1, subset, subsetSize + 1);
}

int main() {
    char set[] = { 'a', 'b', 'c' };
    int n = 3;
    char subset[10];
    powerset(set, n, 0, subset, 0);
    return 0;
}
```

---

## 3. 效能分析

* 通用格式

  * 時間複雜度：`O(n·2^n)`
  * 空間複雜度：`O(n)`

* 詳細分析

  * 節點數：每元素二分，總狀態 `2^n`。
  * 輸出成本：所有子集長度總和為 `n·2^(n-1)`。
  * 總時間：`O(2^n + n·2^(n-1)) = O(n·2^n)`。
  * 遞迴深度與輔助陣列：`O(n)`。

---

## 4. 測試與驗證

### 編譯與執行

```shell
$ g++ main.cpp --std=c++21 -O2 -Wall -Wextra -o main.exe
$ ./main.exe
{}
{c}
{b}
{b,c}
{a}
{a,c}
{a,b}
{a,b,c}
```

`$` 為使用者輸入；無 `$` 行為程式輸出。
（Windows 亦可：`$ .\main.exe`）

### 測試要點

* 邊界：`set` 為空時輸出僅 `{}`。
* 重複元素：若集合含重複，輸出會含重複子集；需唯一性時改用排序+去重或位元枚舉搭配集合判重。
* 緩衝區：`subset` 長度需 ≥ `n`。

---

## 5. 申論及開發報告

* 選擇遞迴回溯的原因：程式結構直觀，易證明覆蓋完整冪集；修改成本低（可加過濾條件）。
* 可替代法：位元遮罩迭代 `for mask in [0, 2^n)`，檢查各 bit 是否取用元素；迭代法同為 `O(n·2^n)`，常數小，便於去重處理與序列化輸出。
* 資料結構：使用陣列作為暫存子集，原地覆寫，空間最小化到 `O(n)`，易於在遞迴中傳遞長度指標控制。


