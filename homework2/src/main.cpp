#include <iostream>
#include <algorithm>

struct Term { int exp; double coef; };

class Polynomial {
    Term* a = nullptr; int n = 0, cap = 0;
    static double dabs(double x) { return x < 0 ? -x : x; }
    static bool isZero(double x) { return dabs(x) <= 1e-12; }
    void reserve(int c) {
        if (c <= cap) return;
        Term* b = new Term[c];
        for (int i = 0; i < n; ++i) b[i] = a[i];
        delete[] a; a = b; cap = c;
    }
    void push(Term t) { if (n == cap) reserve(cap ? cap * 2 : 8); a[n++] = t; }
    static bool cmp(const Term& x, const Term& y) { return x.exp > y.exp; }
    void normalize() {
        if (n == 0) { push({ 0,0 }); return; }
        std::sort(a, a + n, cmp);
        int m = 0;
        for (int i = 0; i < n; ++i) {
            if (isZero(a[i].coef)) continue;
            if (m && a[m - 1].exp == a[i].exp) {
                a[m - 1].coef += a[i].coef;
                if (isZero(a[m - 1].coef)) --m;
            }
            else a[m++] = a[i];
        }
        n = m;
        if (n == 0) { push({ 0,0 }); return; }
    }
    static double powi(double x, int e) {
        double r = 1; while (e > 0) { if (e & 1) r *= x; x *= x; e >>= 1; } return r;
    }
public:
    Polynomial() = default;
    Polynomial(const Polynomial& o) { reserve(o.n); n = o.n; for (int i = 0; i < n; ++i)a[i] = o.a[i]; }
    Polynomial& operator=(const Polynomial& o) {
        if (this == &o) return *this; delete[] a; a = nullptr; n = cap = 0;
        reserve(o.n); n = o.n; for (int i = 0; i < n; ++i)a[i] = o.a[i]; return *this;
    }
    ~Polynomial() { delete[] a; }
    void newTerm(double c, int e) { push({ e,c }); normalize(); }
    static Polynomial Add(const Polynomial& A, const Polynomial& B) {
        Polynomial R; R.reserve(A.n + B.n);
        for (int i = 0; i < A.n; ++i) R.push(A.a[i]);
        for (int i = 0; i < B.n; ++i) R.push(B.a[i]);
        R.normalize(); return R;
    }
    static Polynomial Mult(const Polynomial& A, const Polynomial& B) {
        Polynomial R; R.reserve(A.n * B.n > 0 ? A.n * B.n : 1);
        for (int i = 0; i < A.n; ++i) for (int j = 0; j < B.n; ++j)
            R.push({ A.a[i].exp + B.a[j].exp, A.a[i].coef * B.a[j].coef });
        R.normalize(); return R;
    }
    double Eval(double x) const {
        if (n == 0) return 0;
        double res = a[0].coef; int cur = a[0].exp;
        for (int i = 1; i < n; ++i) {
            int gap = cur - a[i].exp; if (gap > 0) res *= powi(x, gap);
            res += a[i].coef; cur = a[i].exp;
        }
        if (cur > 0) res *= powi(x, cur);
        return res;
    }
    friend std::istream& operator>>(std::istream& is, Polynomial& p) {
        delete[] p.a; p.a = nullptr; p.n = p.cap = 0;
        int m; if (!(is >> m)) return is;
        p.reserve(m);
        for (int i = 0; i < m; ++i) { double c; int e; is >> c >> e; p.push({ e,c }); }
        p.normalize(); return is;
    }
    friend std::ostream& operator<<(std::ostream& os, const Polynomial& p) {
        bool first = true;
        for (int i = 0; i < p.n; ++i) {
            double c = p.a[i].coef; int e = p.a[i].exp;
            if (isZero(c)) continue;
            if (!first) os << (c < 0 ? " - " : " + "); else if (c < 0) os << "-";
            double ac = c < 0 ? -c : c;
            if (e == 0) { os << ac; }
            else {
                if (ac != 1.0) os << ac;
                os << "x"; if (e != 1) os << "^" << e;
            }
            first = false;
        }
        if (first) os << 0;
        return os;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Polynomial A, B;
    if (!(std::cin >> A)) return 0;
    if (!(std::cin >> B)) return 0;
    Polynomial S = Polynomial::Add(A, B);
    Polynomial P = Polynomial::Mult(A, B);
    std::cout << "A(x) = " << A << "\n";
    std::cout << "B(x) = " << B << "\n";
    std::cout << "A(x)+B(x) = " << S << "\n";
    std::cout << "A(x)*B(x) = " << P << "\n";
    double x; std::cout << "Enter x for Eval: ";
    if (std::cin >> x) {
        std::cout << "A(" << x << ") = " << A.Eval(x) << "\n";
        std::cout << "B(" << x << ") = " << B.Eval(x) << "\n";
    }
    return 0;
}
