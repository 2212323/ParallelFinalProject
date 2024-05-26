#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <array>
#include <omp.h>
using namespace std;
int main()
{
    

    // 打开二进制文件 "ExpIndex"
    ifstream indexFile("D:\\ParallelFinalProject Data\\ExpIndex", ios::binary);
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
    ifstream queryFile("D:\\ParallelFinalProject Data\\ExpQuery");
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
    int worker_count = 100; //工作线程数量
    int queryDataSize = 1000;
    size_t times=0;
    size_t index=0;
    size_t step=100;//每多少组数据测试一次时间
    

    vector<chrono::duration<double, std::milli>> elapsedTime(queryDataSize/worker_count);
    chrono::duration<double, std::milli> TotalTime;

    auto Mt1 = std::chrono::high_resolution_clock::now(); // start timer at the beginning of the loop
    vector<vector<unsigned int>> intersectionResults;
    #pragma omp parallel num_threads(worker_count)
    {
        
        #pragma omp for
        for(size_t i=0;i<queryDataSize;i++)
        {
        


        
        auto t1 = std::chrono::high_resolution_clock::now(); // start timer at the beginning of the loop

        size_t minSize = queryData[i][0].size();
        size_t minIndex = 0;//最短列表的数组下标
            for (size_t j = 0; j < queryData[i].size(); j++) //queryData[i].size()为每组列表的个数
            {
                if (queryData[i][j].size() < minSize) 
                {
                    minSize = queryData[i][j].size();
                    minIndex = j;
                }
            }

        vector<unsigned int> S = queryData[i][minIndex];//最短的列表
        for (size_t k = 0; k < queryData[i].size(); k++) //检查
        {
            auto it = S.begin();
            while (it != S.end())
            {
                if (find(queryData[i][k].begin(), queryData[i][k].end(), *it) == queryData[i][k].end())   // std::find函数没有找到等于*it的元素，它会返回lists[i].end()
                {
                    it = S.erase(it);//未找到即无交集，删除它
                } 
                else 
                {
                    it++;
                }
            }
            cout << i<<" k:"<<k<<endl;
        }
        intersectionResults.push_back(S);//S即为每组的交集

        // stop timer
        auto t2 = std::chrono::high_resolution_clock::now();

        // compute and print the elapsed time in millisec
        elapsedTime[index] = t2 - t1;
        //cout << "THIS TIME: " << elapsedTime[index] << " ms.\n";
        index++;
        //QueryPerformanceCounter(&t1); // reset the start timer for the next 100 iterations
        }


    }
        auto Mt2 = std::chrono::high_resolution_clock::now();

        // compute and print the elapsed time in millisec
        //elapsedTime[index] = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        TotalTime = Mt2 - Mt1;
        cout << "TOTAL TIME: " << TotalTime.count()<< " ms.\n";

    for(int i=0;i<worker_count;i++)
    {
        cout<<"线程"<<i<<":"<<" "<<elapsedTime[i].count()<<"\n";
    }

}
