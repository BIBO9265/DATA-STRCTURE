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