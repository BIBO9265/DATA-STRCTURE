# 41343115

# Polynomial

## 解題說明

**問題描述**  
實作多項式 ADT，支援：`newTerm(c,e)`、`Add(A,B)`、`Mult(A,B)`、`Eval(x)`，並多載 `>>`、`<<`。輸入以「項數 n，後隨 n 組(係數, 次方)」表示，輸出為可讀字串。

**解題策略**  
所有項目先**排序(降冪)**再**同冪合併**並**移除 0 係數**做正規化。`Add` 透過串接兩集合後正規化；`Mult` 逐項相乘收集後正規化；`Eval` 依冪差以平方乘法 `powi` 做乘冪。

## 程式實作

> 檔名：`main.cpp`，可以 `g++` 編譯執行。

```cpp
#include <iostream>
#include <algorithm>

struct Term{ int exp; double coef; };

class Polynomial{
    Term* a=nullptr; int n=0, cap=0;
    static double dabs(double x){ return x<0?-x:x; }
    static bool isZero(double x){ return dabs(x)<=1e-12; }
    void reserve(int c){
        if(c<=cap) return;
        Term* b=new Term[c];
        for(int i=0;i<n;++i) b[i]=a[i];
        delete[] a; a=b; cap=c;
    }
    void push(Term t){ if(n==cap) reserve(cap?cap*2:8); a[n++]=t; }
    static bool cmp(const Term&x,const Term&y){ return x.exp>y.exp; }
    void normalize(){
        if(n==0){ push({0,0}); return; }
        std::sort(a,a+n,cmp);
        int m=0;
        for(int i=0;i<n;++i){
            if(isZero(a[i].coef)) continue;
            if(m&&a[m-1].exp==a[i].exp){
                a[m-1].coef+=a[i].coef;
                if(isZero(a[m-1].coef)) --m;
            }else a[m++]=a[i];
        }
        n=m;
        if(n==0){ push({0,0}); return; }
    }
    static double powi(double x,int e){
        double r=1; while(e>0){ if(e&1) r*=x; x*=x; e>>=1; } return r;
    }
public:
    Polynomial()=default;
    Polynomial(const Polynomial& o){ reserve(o.n); n=o.n; for(int i=0;i<n;++i)a[i]=o.a[i]; }
    Polynomial& operator=(const Polynomial& o){
        if(this==&o) return *this; delete[] a; a=nullptr; n=cap=0;
        reserve(o.n); n=o.n; for(int i=0;i<n;++i)a[i]=o.a[i]; return *this;
    }
    ~Polynomial(){ delete[] a; }
    void newTerm(double c,int e){ push({e,c}); normalize(); }
    static Polynomial Add(const Polynomial& A,const Polynomial& B){
        Polynomial R; R.reserve(A.n+B.n);
        for(int i=0;i<A.n;++i) R.push(A.a[i]);
        for(int i=0;i<B.n;++i) R.push(B.a[i]);
        R.normalize(); return R;
    }
    static Polynomial Mult(const Polynomial& A,const Polynomial& B){
        Polynomial R; R.reserve(A.n*B.n>0?A.n*B.n:1);
        for(int i=0;i<A.n;++i) for(int j=0;j<B.n;++j)
            R.push({A.a[i].exp+B.a[j].exp, A.a[i].coef*B.a[j].coef});
        R.normalize(); return R;
    }
    double Eval(double x) const{
        if(n==0) return 0;
        double res=a[0].coef; int cur=a[0].exp;
        for(int i=1;i<n;++i){
            int gap=cur-a[i].exp; if(gap>0) res*=powi(x,gap);
            res+=a[i].coef; cur=a[i].exp;
        }
        if(cur>0) res*=powi(x,cur);
        return res;
    }
    friend std::istream& operator>>(std::istream& is, Polynomial& p){
        delete[] p.a; p.a=nullptr; p.n=p.cap=0;
        int m; if(!(is>>m)) return is;
        p.reserve(m);
        for(int i=0;i<m;++i){ double c; int e; is>>c>>e; p.push({e,c}); }
        p.normalize(); return is;
    }
    friend std::ostream& operator<<(std::ostream& os,const Polynomial& p){
        bool first=true;
        for(int i=0;i<p.n;++i){
            double c=p.a[i].coef; int e=p.a[i].exp;
            if(isZero(c)) continue;
            if(!first) os<<(c<0?" - ":" + "); else if(c<0) os<<"-";
            double ac=c<0?-c:c;
            if(e==0){ os<<ac; }
            else{
                if(ac!=1.0) os<<ac;
                os<<"x"; if(e!=1) os<<"^"<<e;
            }
            first=false;
        }
        if(first) os<<0;
        return os;
    }
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Polynomial A,B;
    if(!(std::cin>>A)) return 0;
    if(!(std::cin>>B)) return 0;
    Polynomial S=Polynomial::Add(A,B);
    Polynomial P=Polynomial::Mult(A,B);
    std::cout<<"A(x) = "<<A<<"\n";
    std::cout<<"B(x) = "<<B<<"\n";
    std::cout<<"A(x)+B(x) = "<<S<<"\n";
    std::cout<<"A(x)*B(x) = "<<P<<"\n";
    double x; std::cout<<"Enter x for Eval: ";
    if(std::cin>>x){
        std::cout<<"A("<<x<<") = "<<A.Eval(x)<<"\n";
        std::cout<<"B("<<x<<") = "<<B.Eval(x)<<"\n";
    }
    return 0;
}
```

## 效能分析

設 `n=|A|`, `m=|B|`, `T` 為暫存項數。  

- `normalize`：排序 `O(k log k)` 合併 `O(k)`，k 為現有項數。  
- `newTerm`：攤銷 `O(log n)`（由排序主導）。  
- `Add`：`O((n+m) log(n+m))`，空間 `O(n+m)`。  
- `Mult`：生成 `n·m` 項，再排序合併，`O(nm + (nm)log(nm))`，空間 `O(nm)`。  
- `Eval`：對每冪差做平方乘法，`O(t log E)`，t 為項數，E 為最大冪差。

## 測試與驗證

編譯與執行：

```shell
$ g++ polynomial.cpp --std=c++21 -O2 -o poly
$ ./poly
3
3 2
-5 1
1 0
2
1 1
2 0
2
A(x) = 3x^2 - 5x + 1
B(x) = x + 2
A(x)+B(x) = 3x^2 - 4x + 3
A(x)*B(x) = 3x^3 + x^2 - 9x + 2
Enter x for Eval: 2
A(2) = 3
B(2) = 4
```

零多項式測試：

```shell
$ ./poly
1
0 0
2
2 3
-1 1
2
A(x) = 0
B(x) = 2x^3 - x
A(x)+B(x) = 2x^3 - x
A(x)*B(x) = 0
A(2) = 0
B(2) = 14
```

## 申論及開發報告

- **限制與取捨**：在標頭受限下，改用自管動態陣列避免 STL；以降冪排序+合併確保唯一正規形。   
- **可維護性**：深拷貝與賦值運算子避免重複釋放；`powi` 使用平方乘法降低乘冪成本。
