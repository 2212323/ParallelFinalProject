#include <mpi.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
using namespace std;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    vector<vector<unsigned int>> indexData;

    // 主进程读取索引文件
    if (world_rank == 0) {
        ifstream indexFile("D:\\ParallelFinalProject Data\\ExpIndex", ios::binary);
        if (indexFile.is_open())
        {
            cout << "Reading ExpIndex file..." << endl;
            while (indexFile.peek() != EOF) {
                unsigned int arrayLength;
                indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
                vector<unsigned int> arrayData(arrayLength);
                for (unsigned int i = 0; i < arrayLength; ++i) {
                    unsigned int value;
                    indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));
                    arrayData[i] = value;
                }
                indexData.push_back(arrayData);
            }
            indexFile.close();
        }
        else {
            cerr << "Failed to open ExpIndex file." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // 广播 indexData 的大小
    int indexDataSize = indexData.size();
    MPI_Bcast(&indexDataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (world_rank != 0) {
        indexData.resize(indexDataSize);
    }

    // 广播每个向量的大小和数据
    for (auto& arrayData : indexData) {
        unsigned int arrayLength = arrayData.size();
        MPI_Bcast(&arrayLength, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        if (world_rank != 0) {
            arrayData.resize(arrayLength);
        }
        MPI_Bcast(arrayData.data(), arrayLength, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    }

    vector<vector<vector<unsigned int>>> queryData;
    if (world_rank == 0) {
        ifstream queryFile("D:\\ParallelFinalProject Data\\ExpQuery");
        if (queryFile.is_open())
        {
            cout << "Reading ExpQuery file..." << endl;
            string line;
            while (getline(queryFile, line)) {
                vector<vector<unsigned int>> queryResult;
                stringstream ss(line);
                unsigned int index;
                while (ss >> index) {
                    if (index < indexData.size()) {
                        queryResult.push_back(indexData[index]);
                    }
                    else {
                        cerr << "Index out of range: " << index << endl;
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
                }
                queryData.push_back(queryResult);
            }
            queryFile.close();

        }
        else {
            cerr << "Failed to open ExpQuery file." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    int queryDataSize = queryData.size();
    MPI_Bcast(&queryDataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (world_rank != 0) {
        queryData.resize(queryDataSize);
    }
    for (auto& queryResult : queryData) {
        int queryResultSize = queryResult.size();
        MPI_Bcast(&queryResultSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (world_rank != 0) {
            queryResult.resize(queryResultSize);
        }
        for (auto& queryArray : queryResult) {
            unsigned int queryArraySize = queryArray.size();
            MPI_Bcast(&queryArraySize, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
            if (world_rank != 0) {
                queryArray.resize(queryArraySize);
            }
            MPI_Bcast(queryArray.data(), queryArraySize, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        }
    }

    int queriesPerProcess = (queryDataSize + world_size - 1) / world_size;
    int startQuery = world_rank * queriesPerProcess;
    int endQuery = min(startQuery + queriesPerProcess, queryDataSize);

    vector<double> elapsedTime(world_size);

    auto start = chrono::high_resolution_clock::now(); // 开始计时 

    for (size_t i = startQuery; i < endQuery; i++)
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
            cout << i << " k:" << k << endl;
        }

    }

    auto end = chrono::high_resolution_clock::now(); // 结束计时
    chrono::duration<double, milli> localElapsedTime = end - start;
    double localElapsedTimeMs = localElapsedTime.count();
    MPI_Gather(&localElapsedTimeMs, 1, MPI_DOUBLE, elapsedTime.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        for (int i = 0; i < world_size; ++i) {
            cout << elapsedTime[i] << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
