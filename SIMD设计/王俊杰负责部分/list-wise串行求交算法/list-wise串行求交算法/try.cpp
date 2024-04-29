#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <windows.h>
using namespace std;

int main()
{
    // 打开二进制文件 "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // 创建一个二维向量 indexData 来存储数据
    vector<vector<unsigned int>> indexData;

    // 如果文件成功打开
    if (indexFile.is_open())
    {
        // 循环读取文件直到文件末尾
        while (!indexFile.eof()) {
            // 读取数组的长度
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));

            // 创建一个长度为 arrayLength 的一维向量 arrayData
            vector<unsigned int> arrayData(arrayLength);
            // 循环读取数组的每个元素
            for (unsigned int i = 0; i < arrayLength; ++i) {
                // 读取元素的值
                unsigned int value;
                indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));
                // 将元素的值存入 arrayData
                arrayData[i] = value;
            }

            // 将一维向量 arrayData 存入二维向量 indexData
            indexData.push_back(arrayData);
        }

        // 关闭文件
        indexFile.close();




    }
    // 打开查询文件 "ExpQuery"
    ifstream queryFile("ExpQuery");
    // 创建一个三维向量 queryData 来存储查询结果
    vector<vector<vector<unsigned int>>> queryData;

    // 如果文件成功打开
    if (queryFile.is_open())
    {
        // 循环读取文件直到文件末尾
        string line;
        while (getline(queryFile, line)) {
            // 创建一个二维向量 queryResult 来存储查询结果
            vector<vector<unsigned int>> queryResult;

            // 将行中的每个数字转换为索引文件的数组下标，并查询对应的向量
            stringstream ss(line);
            unsigned int index;

            while (ss >> index) {
                // 查询对应的向量并将其存入 queryResultItem
                queryResult.push_back(indexData[index]);
            }

            // 将一维向量 queryResult 存入二维向量 queryData
            queryData.push_back(queryResult);
        }

        // 关闭文件
        queryFile.close();
    }

    // 在这里可以进行后续的求交操作
    // ...

    int queryDataSize = 1000;
    size_t times=0;
    size_t index=0;
    size_t step=100;//每多少组数据测试一次时间
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    vector<double> elapsedTime(queryDataSize/step);

    // get ticks per second
    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&t1); // start timer at the beginning of the loop

    for (const auto& result : queryData) {

        // 创建一个一维向量 intersection 来存储求交结果
        vector<unsigned int> intersection = result[0];

        // 对于每一个查询结果里面的每一个向量
        for (const auto& value : result) {
            // 创建一个一维向量 tempIntersection 来存储当前向量与 intersection 的交集
            vector<unsigned int> tempIntersection;

            // 对于当前向量中的每一个元素
            for (const auto& element : value) {
                // 如果当前元素也存在于 intersection 中，则将其添加到 tempIntersection
                if (find(intersection.begin(), intersection.end(), element) != intersection.end()) {
                    tempIntersection.push_back(element);
                }
            }

            // 将 tempIntersection 更新为新的 intersection
            intersection = tempIntersection;
        }

        // 输出求交结果的前5项

         times++;
        if(times%step==0)
        {
            // stop timer
        QueryPerformanceCounter(&t2);

        // compute and print the elapsed time in millisec
        elapsedTime[index] = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        std::cout << "Elapsed time for 100 iterations: " << elapsedTime[index] << " ms.\n";
        index++;
        QueryPerformanceCounter(&t1); // reset the start timer for the next 100 iterations
        }

    }


    for(int i=0;i<queryDataSize/step;i++)
    {
        cout<<"Elapsed time for 100 iterations: "<<i<<":"<<elapsedTime[i]<<" ms.\n";
    }


}
