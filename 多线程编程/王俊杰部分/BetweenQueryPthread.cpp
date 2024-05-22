#include <stdlib.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <array>
#include <emmintrin.h> // for SSE2
#include <smmintrin.h> // for SSE4.1
using namespace std;

typedef struct {
    int k; //消去的轮次
    int t_id; // 线程 id
} threadParam_t;
int worker_count = 10; //工作线程数量
int queryDataSize = 1000;
vector<vector<unsigned int>> indexData;
vector<vector<vector<unsigned int>>> queryData;
vector<double> elapsedTime(queryDataSize/worker_count);
size_t index=0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *threadFunc(void *param) {
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id; //线程编号
    //int i = k + t_id + 1; //获取自己的计算任务
    int t_begin = t_id * queryDataSize/worker_count;//计算任务的起始位置
    int t_end = (t_id + 1) * queryDataSize/worker_count;//计算任务的结束位置

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks

    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&t1); // start timer at the beginning of the loop

    for (int Qi = t_begin; Qi < t_end; ++Qi) {
        const auto& result = queryData[Qi];

    // 创建一个一维向量 intersection 来存储求交结果
    vector<unsigned int> intersection = result[0];

    // 对于每一个查询结果里面的每一个向量
    for (const auto& value : result) {
        // 创建一个一维向量 tempIntersection 来存储当前向量与 intersection 的交集
        vector<unsigned int> tempIntersection;
        tempIntersection.reserve(intersection.size()); // 预分配内存

        // 对于当前向量中的每一个元素
        for (const auto& element : value) {
            // 创建一个包含4个相同元素的向量
            __m128i v1 = _mm_set1_epi32(element);

            // 对于 intersection 中的每一个元素
            auto it = intersection.begin();
            for (; std::distance(it, intersection.end()) >= 4; it += 4) {
                // 创建一个包含4个元素的向量
                __m128i v2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&(*it)));

                // 比较两个向量的每个元素
                __m128i v3 = _mm_cmpeq_epi32(v1, v2);

                // 将比较结果转换为一个整数
                int mask = _mm_movemask_epi8(v3);

                // 如果当前元素也存在于 intersection 中，则将其添加到 tempIntersection
                if (mask != 0) {
                    tempIntersection.push_back(element);
                    break;
                }
            }

            // 处理剩余的元素
            for (; it != intersection.end(); ++it) {
                if (element == *it) {
                    tempIntersection.push_back(element);
                    break;
                }
            }
        }

        // 更新 intersection
        intersection = move(tempIntersection);
    }
}

        QueryPerformanceCounter(&t2);
        pthread_mutex_lock(&mtx);
        // compute and print the elapsed time in millisec
        elapsedTime[t_id] = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        cout <<"thread"<<t_id<< ":" << elapsedTime[t_id] << " ms.\n";
        index++;
        pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

int mainP() {
      // 打开二进制文件 "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
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
    ifstream queryFile("ExpQuery");
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
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks

    // get ticks per second

    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&t1); // start timer at the beginning of the loop


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


        QueryPerformanceCounter(&t2);
        // compute and print the elapsed time in millisec
        cout <<"TOTAL TIME:"<< (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        cout << " ms.\n";

    for(int i=0;i<worker_count;i++)
        {
            cout<<elapsedTime[i]<<"\n";
        }
    return 0;
}
