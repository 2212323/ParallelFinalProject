#include <cstddef>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
//#include <wincrypt.h>
//#include<windows.h>
#include <chrono>
using namespace std;
int main()
{
    // 打开二进制文件 "ExpIndex"
ifstream indexFile("D:\\ParallelFinalProject Data\\ExpIndex", ios::binary);//在上两级目录中
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
        // 一次性读取整个数组
        indexFile.read(reinterpret_cast<char*>(arrayData.data()), arrayLength * sizeof(unsigned int));

        // 将一维向量 arrayData 存入二维向量 indexData
        indexData.push_back(arrayData);
    }

    // 关闭文件
    indexFile.close();
}
else{
    cout<<"indexFile open failed!"<<endl;
    return 0;
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
                // 查询对应的向量并将其存入 queryResult
                queryResult.push_back(indexData[index]);
            }

            // 将一维向量 queryResult 存入二维向量 queryData queryData变为三维向量
            queryData.push_back(queryResult);
        }

        // 关闭文件
        queryFile.close();
    }
    else {
        cout << "queryFile open failed!" << endl;
        return 0;
    }

    

    // 在这里可以进行后续的求交操作
    // ...
    vector<vector<unsigned int>> intersectionResults;
    size_t queryDataSize = queryData.size();//组数，需要查询的个数，也是查询结果的个数
    
    cout<<"queryDataSize: "<<queryDataSize<<endl;


    size_t index=0;
    size_t step=100;
    size_t worker_count=10;
    vector<chrono::duration<double, std::milli>> elapsedTime(10);//测10组


    auto t1 = chrono::high_resolution_clock::now(); // start timer at the beginning of the loop
    auto St1 = t1;
    auto St2 = t1;
    for(size_t i=0;i<queryDataSize;i++)
    {
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
        
          #pragma omp parallel num_threads(worker_count)
        {
         #pragma omp for    
    for (size_t k = 0; k < queryData[i].size(); k++) //检查
    {
        vector<unsigned int> local_S;
        #pragma omp critical
        {
            local_S = S;
        }
        
        for(auto it = local_S.begin(); it != local_S.end(); )
        {
            if (find(queryData[i][k].begin(), queryData[i][k].end(), *it) == queryData[i][k].end())
            {
                it = local_S.erase(it);
            } 
            else 
            {
                it++;
            }
        }
        
        #pragma omp critical
        {
            S = local_S;
        }
        cout<<i<<" k:"<<k<<endl;
    }
            
        }
        intersectionResults.push_back(S);//S即为每组的交集
        if((i+1)%step==0)
        {
            // stop timer
        St2 = chrono::high_resolution_clock::now();

        // compute and print the elapsed time in millisec
        elapsedTime[index]=St2-St1;
        std::cout << "Elapsed time for 100 iterations: " << elapsedTime[index].count() << " ms.\n";
        index++;
        St1 = chrono::high_resolution_clock::now(); // reset the start timer for the next 100 iterations
        }
    }

    auto t2 = chrono::high_resolution_clock::now();
    // compute and print the elapsed time in millisec
    chrono::duration<double, std::milli> TotalTime = t2 - t1;
    cout <<"TOTAL TIME:"<< TotalTime.count();
    cout << " ms.\n";

    for(size_t i=0;i<queryDataSize/step;i++)
    {
        cout<<"Elapsed time for 100 iterations: "<<i<<":"<<elapsedTime[i].count()<<" ms.\n";
    }

    // // 输出查询结果的前10项
    // int count = 0;
    // for (const auto& result : intersectionResults) {
    //     for (const auto& element : result) {
    //         cout<<count<<":"<< element << " ";
    //     }
    //     cout << endl;
    //     cout << "----------------------" << endl;
    //     count++;
    //     if (count == 10)
    //         break;
    // }
    return 0;

}