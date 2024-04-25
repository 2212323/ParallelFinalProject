#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
using namespace std;
const unsigned int MAX_SIZE = 40000000;  // 位图的最大大小
int main() {
    // 打开二进制文件 "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // 在堆上创建一个二维位图 indexData 来存储数据
    vector<bitset<MAX_SIZE>>* indexData = new vector<bitset<MAX_SIZE>>();

    // 如果文件成功打开
    if (indexFile.is_open()) {
        cout << "indexFile opened" << endl;
        // 循环读取文件直到文件末尾
        while (!indexFile.eof()) {
            // 读取数组的长度
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
            // 在堆上创建一个长度为 MAX_SIZE 的位图 arrayData
            bitset<MAX_SIZE>* arrayData = new bitset<MAX_SIZE>();
            // 循环读取数组的每个元素
            for (unsigned int i = 0; i < arrayLength; ++i) {
                // 读取元素的值
                unsigned int value;
                indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));
                // 将元素的值对应的位设置为1
                arrayData->set(value);
            }
            // 将位图 arrayData 存入二维位图 indexData
            indexData->push_back(*arrayData);
            // 释放动态分配的内存
            delete arrayData;
        }
        // 关闭文件
        indexFile.close();
    }

    // 打开查询文件 "ExpQuery"
    ifstream queryFile("ExpQuery");
    // 创建一个三维位图 queryData 来存储查询结果
    vector<vector<bitset<MAX_SIZE>>>* queryData = new vector<vector<bitset<MAX_SIZE>>>();

    // 如果文件成功打开
    if (queryFile.is_open()) {
        cout << "queryFile opened" << endl;
        // 循环读取文件直到文件末尾
        string line;
        while (getline(queryFile, line)) {
            // 创建一个二维位图 queryResult 来存储查询结果
            vector<bitset<MAX_SIZE>> queryResult;
            // 将行中的每个数字转换为索引文件的数组下标，并查询对应的位图
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                // 查询对应的位图并将其存入 queryResult
                queryResult.push_back((*indexData)[index]);
            }
            // 将二维位图 queryResult 存入三维位图 queryData
            queryData->push_back(queryResult);
        }
        // 关闭文件
        queryFile.close();
    }
    // 输出queryData的前十个查询结果
    for (int i = 0; i < 10 && i < queryData->size(); ++i) {
        cout << "Query Result " << i + 1 << ": ";
        for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
            cout << (*queryData)[i][j] << " ";
        }
        cout << endl;
    }

    // 在这里可以进行后续的求交操作
    // ...

    // 释放动态分配的内存
    delete indexData;
    delete queryData;

    return 0;
}

