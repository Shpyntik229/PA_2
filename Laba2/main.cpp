#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>

#include "RBTree.h"

template<typename K, typename V>
void viewFirstElements(const RBTree<K, V>& tree, int count)
{
    for (int i = 0; i < 10; i++)
    {
        try
        {
            std::cout << tree[i] << std::endl;
        }
        catch (int err)
        {
            std::cout << "EMPTY" << std::endl;
        }
    }
}

int main()
{
    std::vector<int> vec;
    vec.reserve(10000);
    RBTree<int, int> tree;
    for (int i = 0; i < 10000; i++)
    {
        int rand_num = std::rand();
        while (!tree.add(rand_num, i))
        {
            rand_num = std::rand();
        }
        vec.push_back(rand_num);
    }
    std::ofstream out("test.txt");
    int acc = 0;
    for (int i = 0; i < 15; i++)
    {
        int ind = std::rand() % 10000;
        if (tree[vec[ind]] != ind) throw 666;
        int checkCount = tree.getLastCheckCount();
        out << i + 1 << "\t" << checkCount << "\n";
        acc += checkCount;
    }
    int mean = acc / 15;
    out << "\nMean: " << mean;
}