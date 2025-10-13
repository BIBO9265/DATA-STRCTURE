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


    powerset(set, n, index + 1, subset, subsetSize);


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