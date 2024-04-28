#include <cstddef>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <sys/time.h> //gettimeofday()
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
        // 一次性读取整个数组
        indexFile.read(reinterpret_cast<char*>(arrayData.data()), arrayLength * sizeof(unsigned int));

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
                // 查询对应的向量并将其存入 queryResult
                queryResult.push_back(indexData[index]);
            }

            // 将一维向量 queryResult 存入二维向量 queryData queryData变为三维向量
            queryData.push_back(queryResult);
        }

        // 关闭文件
        queryFile.close();
    }

    

    // 在这里可以进行后续的求交操作
    // ...
    vector<vector<unsigned int>> intersectionResults;
    size_t queryDataSize = queryData.size();//组数，需要查询的个数，也是查询结果的个数
    
    cout<<"queryDataSize: "<<queryDataSize<<endl;


    size_t times=0;
    size_t index=0;
    size_t step=100;//每多少组数据测试一次时间
    struct timeval t1, t2; // Use struct timeval to record time
    vector<double> elapsedTime(queryDataSize/step);

    // get ticks per second
    //QueryPerformanceFrequency(&frequency);

   gettimeofday(&t1, NULL); // Start timer

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
            cout<<i<<" k:"<<k<<endl;
        }
        intersectionResults.push_back(S);//S即为每组的交集
        times++;
        if(times%step==0)
        {
            gettimeofday(&t2, NULL); // Stop timer

        // compute and print the elapsed time in millisec
        elapsedTime[index] = (t2.tv_sec - t1.tv_sec) * 1000.0;// sec to ms
        std::cout << "Elapsed time for 100 iterations: " << elapsedTime[index] << " ms.\n";
        index++;
        gettimeofday(&t1, NULL); // Reset the start timer for the next 100 iterations
        }
    }


    for(size_t i=0;i<queryDataSize/step;i++)
    {
        cout<<"Elapsed time for 100 iterations: "<<i<<":"<<elapsedTime[i]<<" ms.\n";
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