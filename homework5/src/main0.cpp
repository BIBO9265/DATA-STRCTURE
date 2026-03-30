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