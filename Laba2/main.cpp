#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <chrono>

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

void test()
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
        using namespace std::chrono;
        std::chrono::time_point<std::chrono::steady_clock> curtime = std::chrono::high_resolution_clock::now();;
        if (tree[vec[ind]] != ind) throw 666;
        std::size_t time = duration_cast<microseconds>(high_resolution_clock::now() - curtime).count();
        int checkCount = tree.getLastCheckCount();
        out << i + 1 << "\t" << checkCount << "\t" << time << "\n";
        acc += checkCount;
    }
    int mean = acc / 15;
    out << "\nMean: " << mean;
}

int main()
{
    test();
    RBTree<int, int> tree;
    std::cout << "Adding\n";
    int key, value;
    std::cin >> key >> value;
    if (tree.add(key, value))
    {
        std::cout << "Key/Value succesfully added!\n";
    }
    else
    {
        std::cout << "ERROR!\n";
    }
    std::cout << "Finding\n";
    std::cin >> key;
    std::cout << "Value: ";
    try
    {
        std::cout << tree[key] << std::endl;
    }
    catch (int err)
    {
        std::cout << "EMPTY" << std::endl;
    }
}