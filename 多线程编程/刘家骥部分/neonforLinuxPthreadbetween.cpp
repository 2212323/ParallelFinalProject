#include <stdlib.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <array>
#include <arm_neon.h>
using namespace std;

typedef struct {
    int k; //消去的轮次
    int t_id; // 线程 id
} threadParam_t;
int worker_count = 10; //工作线程数量
int queryDataSize = 1000;
vector<vector<unsigned int>> indexData;
vector<vector<vector<unsigned int>>> queryData;
vector<vector<unsigned int>> intersectionResults;
vector<chrono::duration<double, std::milli>> elapsedTime(queryDataSize/worker_count);
size_t index=0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *threadFunc(void *param) {
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id; //线程编号
    //int i = k + t_id + 1; //获取自己的计算任务
    int t_begin = t_id * queryDataSize/worker_count;//计算任务的起始位置
    int t_end = (t_id + 1) * queryDataSize/worker_count;//计算任务的结束位置



    auto t1 = chrono::high_resolution_clock::now(); // start timer at the beginning of the loop


    for (size_t i = 0; i < queryDataSize; i++) {
        size_t minSize = queryData[i][0].size();
        size_t minIndex = 0;
        for (size_t j = 0; j < queryData[i].size(); j++) {
            if (queryData[i][j].size() < minSize) {
                minSize = queryData[i][j].size();
                minIndex = j;
            }
        }

        vector<unsigned int> S = queryData[i][minIndex];

        for (size_t k = 0; k < queryData[i].size(); k++) {
            if (k == minIndex) continue;

            vector<unsigned int> &currentList = queryData[i][k];
            auto it = S.begin();

            while (it != S.end()) {
                uint32_t value = *it;
                uint32x4_t neon_value = vdupq_n_u32(value);
                bool found = false;

                for (size_t idx = 0; idx < currentList.size(); idx += 4) {
                    uint32x4_t data = vld1q_u32(&currentList[idx]);
                    uint32x4_t result = vceqq_u32(data, neon_value);
                    if (vmaxvq_u32(result) != 0) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    it = S.erase(it);
                } else {
                    ++it;
                }
            }
            cout<<i<<" k:"<<k<<endl;
        }
        intersectionResults.push_back(S);
    }

        auto t2 = chrono::high_resolution_clock::now();
        pthread_mutex_lock(&mtx);
        // compute and print the elapsed time in millisec
        elapsedTime[t_id] = t2 - t1;
        cout <<"thread"<<t_id<< ":" << elapsedTime[t_id].count() << " ms.\n";
        index++;
        pthread_mutex_unlock(&mtx);
        pthread_exit(NULL);
}

int main() {
      // 打开二进制文件 "ExpIndex"
    ifstream indexFile("../ExpIndex", ios::binary);
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
    ifstream queryFile("../ExpQuery");
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

    int queryDataSize = 1000;
    size_t times=0;
    size_t index=0;
    size_t step=100;//每多少组数据测试一次时间


    auto t1 = chrono::high_resolution_clock::now(); // start timer at the beginning of the loop


    pthread_t* handles = (pthread_t*)malloc(worker_count * sizeof(pthread_t)); // 创建对应的 Handle
    threadParam_t* param = (threadParam_t*)malloc(worker_count * sizeof(threadParam_t)); // 创建对应的线程数据结构

    //分配任务
    for(int t_id = 0; t_id < worker_count; t_id++) {
        param[t_id].t_id = t_id;
    }

    //创建线程
    for(int t_id = 0; t_id < worker_count; t_id++)
        pthread_create(&handles[t_id], NULL, threadFunc, &param[t_id]);

    //主线程挂起等待所有的工作线程完成此轮消去工作
    for(int t_id = 0; t_id < worker_count; t_id++)
        pthread_join(handles[t_id], NULL);


        auto t2 = chrono::high_resolution_clock::now();
        // compute and print the elapsed time in millisec
        cout <<"TOTAL TIME:"<< (t2-t1).count() << " ms.\n";

    for(int i=0;i<worker_count;i++)
    {
        cout<<"线程"<<i<<" "<<elapsedTime[i].count()<<"\n";
    }
    return 0;
}
