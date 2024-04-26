#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fstream>
using namespace std;
const unsigned int MAX_SIZE = 40000000;  // 位图的最大大小
const unsigned int BLOCK_SIZE = 64;  // 位图块的大小

// void saveToDisk(vector<bitset<MAX_SIZE>>* indexData, vector<vector<bitset<MAX_SIZE>>>* queryData) {
//     cout<<"saveToDisk"<<endl;
//     // 创建一个输出文件流
//     ofstream outFile;

//     // 打开一个文件来写入indexData
//     outFile.open("indexData.bin", std::ios::binary);
//     for (const auto& bitset : *indexData) {
//         for (int i = 0; i < MAX_SIZE; i += 64) {
//             std::bitset<64> smallBitset;
//             for (int j = 0; j < 64; ++j) {
//                 smallBitset[j] = bitset[i + j];
//             }
//             unsigned long long value = smallBitset.to_ullong();
//             outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
//         }
//     }
//     outFile.close();
//     cout<<"saveToDisk1"<<endl;

//     // 打开一个文件来写入queryData
//     outFile.open("queryData.bin", std::ios::binary);
//     for (const auto& vector : *queryData) {
//         for (const auto& bitset : vector) {
//             for (int i = 0; i < MAX_SIZE; i += 64) {
//                 std::bitset<64> smallBitset;
//                 for (int j = 0; j < 64; ++j) {
//                     smallBitset[j] = bitset[i + j];
//                 }
//                 unsigned long long value = smallBitset.to_ullong();
//                 outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
//             }
//         }
//     }
//     outFile.close();
//     cout<<"saveToDisk2"<<endl;
// }

// void loadFromDisk(vector<bitset<MAX_SIZE>>* indexData, vector<vector<bitset<MAX_SIZE>>>* queryData) {
//     // 创建一个输入文件流
//     std::ifstream inFile;

//     // 打开一个文件来读取indexData
//     inFile.open("indexData.bin", std::ios::binary);
//     if (!inFile) {
//         std::cerr << "Unable to open file indexData.bin";
//         exit(1);   // call system to stop
//     }
//     unsigned long long value;
//     while (inFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
//         indexData->push_back(bitset<MAX_SIZE>(value));
//     }
//     inFile.close();

//     // 打开一个文件来读取queryData
//     inFile.open("queryData.bin", std::ios::binary);
//     if (!inFile) {
//         std::cerr << "Unable to open file queryData.bin";
//         exit(1);   // call system to stop
//     }
//     vector<bitset<MAX_SIZE>> queryResult;
//     while (inFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
//         queryResult.push_back(bitset<MAX_SIZE>(value));
//         if (queryResult.size() == 5) {  // 假设每个查询结果包含5个位图
//             queryData->push_back(queryResult);
//             queryResult.clear();
//         }
//     }
//     inFile.close();
// }
int main() {
    cout<<MAX_SIZE / BLOCK_SIZE;

    // 打开二进制文件 "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // 在堆上创建一个二维位图 indexData 来存储数据
    vector<bitset<MAX_SIZE>>* indexData = new vector<bitset<MAX_SIZE>>();
    // 创建一个二级索引
    vector<bitset<MAX_SIZE / BLOCK_SIZE>>* secondaryIndexData = new vector<bitset<MAX_SIZE / BLOCK_SIZE>>();

    // 如果文件成功打开
    if (indexFile.is_open()) {
        cout << "indexFile opened" << endl;
        // 循环读取文件直到文件末尾
        int www = 0;
        while (!indexFile.eof()) {
            // 读取数组的长度
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
            // 在堆上创建一个长度为 MAX_SIZE 的位图 arrayData
            bitset<MAX_SIZE>* arrayData = new bitset<MAX_SIZE>();
            // 在堆上创建一个长度为 MAX_SIZE / BLOCK_SIZE 的位图 secondaryIndex
            bitset<MAX_SIZE / BLOCK_SIZE>* secondaryIndex = new bitset<MAX_SIZE / BLOCK_SIZE>();
            // 循环读取数组的每个元素
            for (unsigned int i = 0; i < arrayLength; ++i) {
                // 读取元素的值
                unsigned int value;
                indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));

                // 将元素的值对应的位设置为1
                arrayData->set(value);
                secondaryIndex->set(value / BLOCK_SIZE);
            }
            // 将位图 arrayData 存入二维位图 indexData
            indexData->push_back(*arrayData);
            secondaryIndexData->push_back(*secondaryIndex);
            // 释放动态分配的内存
            delete arrayData;
        }
        // 关闭文件
        indexFile.close();
    }
  

// 打开查询文件 "ExpQuery"
ifstream queryFile("ExpQuery");
// 创建一个三维位图 queryData 来存储查询结果
vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>* queryData = new vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>();

    // 如果文件成功打开
    if (queryFile.is_open()) {
        cout << "queryFile opened" << endl;
        // 循环读取文件直到文件末尾
        string line;
        while (getline(queryFile, line)) {
            // 创建一个二维位图 queryResult 来存储查询结果
            vector<bitset<MAX_SIZE/ BLOCK_SIZE>> queryResult;
            // 将行中的每个数字转换为索引文件的数组下标，并查询对应的位图
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                // 查询对应的位图并将其存入 queryResult
                queryResult.push_back((*secondaryIndexData)[index]);
            }
            // 将二维位图 queryResult 存入三维位图 queryData
            queryData->push_back(queryResult);
        }
        // 关闭文件
        queryFile.close();
    }
    //输出queryData的前十个查询结果
    for (int i = 0; i < 10 && i < queryData->size(); ++i) {
        cout << "Query Result " << i + 1 << ": ";
        for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
            cout << (*queryData)[i][j] << " ";
        }
        cout <<endl<<"------------------------------------------------------------------"<<endl;
    }
    //saveToDisk(indexData, queryData);

    // 在这里可以进行后续的求交操作
    // ...

    // 释放动态分配的内存
    delete indexData;
    delete queryData;
    delete secondaryIndexData;

    return 0;
}

