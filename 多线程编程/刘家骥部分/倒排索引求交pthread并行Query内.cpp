#include <stdlib.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
//#include <windows.h>
//#include <sys/time.h>
#include <chrono>
#include <array>
using namespace std;

typedef struct { 
    int t_id; // 线程 id
    vector<unsigned int> *S;
    int i;
    int k; //消去的轮次
} threadParam_t;
//int worker_count = 10; //工作线程数量
int queryDataSize = 1000;
vector<vector<unsigned int>> indexData;
vector<vector<vector<unsigned int>>> queryData;
vector<vector<unsigned int>> intersectionResults;
//vector<double> elapsedTime(queryDataSize/worker_count);
vector<chrono::duration<double, std::milli>> elapsedTime(10);
//size_t index=0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *threadFunc(void *param) {
    
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id; //线程编号
    //int i = k + t_id + 1; //获取自己的计算任务
    
    vector<unsigned int> S=(*(p->S));
    int i=p->i;
    int k=p->k;


            auto it = S.begin();
            while (it != S.end())
            {
                if (find(queryData[i][k].begin(), queryData[i][k].end(), *it) == queryData[i][k].end())   // std::find函数没有找到等于*it的元素，它会返回lists[i].end()
                {
                    pthread_mutex_lock(&mtx);
                    it = S.erase(it);//未找到即无交集，删除它
                    pthread_mutex_unlock(&mtx);
                } 
                else 
                {
                    it++;
                }
            }
            cout << i<<" k:"<<k<<endl;
            
        
        pthread_exit(NULL);
        return nullptr;
}

int main() {
      // 打开二进制文件 "ExpIndex"
    ifstream indexFile("D:\\ParallelFinalProject Data\\ExpIndex", ios::binary);
    // 创建一个二维向量 indexData 来存储数据

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

    
    //struct timeval t1, t2; // Use struct timeval to record time

    auto t1 = chrono::high_resolution_clock::now();
    int index=0;
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
        //--------------------------
        for (size_t k = 0; k < queryData[i].size(); k++) //检查
    {
        pthread_t* handles = (pthread_t*)malloc(queryData[i].size() * sizeof(pthread_t)); // 创建对应的 Handle
        threadParam_t* param = (threadParam_t*)malloc(queryData[i].size() * sizeof(threadParam_t)); // 创建对应的线程数据结构

        //分配任务
        for(int t_id = 0; t_id < queryData[i].size(); t_id++) {//初始化传入的参数
        param[t_id].t_id = t_id;
        param[t_id].S = &S;
        param[t_id].i=i;
        param[t_id].k=k;
        }

        //创建线程
        for(int t_id = 0; t_id < queryData[i].size(); t_id++)
            pthread_create(&handles[t_id], NULL, threadFunc, &param[t_id]);

        //主线程挂起等待所有的工作线程完成此轮消去工作
        for(int t_id = 0; t_id < queryData[i].size(); t_id++)
            pthread_join(handles[t_id], NULL);

    }
            intersectionResults.push_back(S);//S即为每组的交集
        //--------------------------
        
        if((i+1)%step==0)
        {
            St2 = chrono::high_resolution_clock::now();
            elapsedTime[index]=St2-St1;
            index++;
            St1 = chrono::high_resolution_clock::now();
        }
        
}

        auto t2 = chrono::high_resolution_clock::now();
        // compute and print the elapsed time in millisec
        chrono::duration<double, std::milli> TotalTime = t2 - t1;
        cout <<"TOTAL TIME:"<< TotalTime.count();
        cout << " ms.\n";

    for(int i=0;i<10;i++)
    {
        cout<<"线程"<<i<<" "<<elapsedTime[i].count()<<"\n";
    }
    return 0;
}
