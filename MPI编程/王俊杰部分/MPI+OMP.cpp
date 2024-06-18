#include <mpi.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <omp.h>
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
        ifstream indexFile("D:\\ExpIndex", ios::binary);
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
        ifstream queryFile("D:\\ExpQuery");
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

    // 并行化查询处理
    #pragma omp parallel for
    for (int i = startQuery; i < endQuery; ++i) {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        cout << "Process " << world_rank << ", Thread " << thread_id << "/" << num_threads << " processing query " << i << endl;

        vector<unsigned int> intersection = queryData[i][0];
        for (const auto& value : queryData[i]) {
            vector<unsigned int> tempIntersection;
            for (const auto& element : value) {
                if (find(intersection.begin(), intersection.end(), element) != intersection.end()) {
                    tempIntersection.push_back(element);
                }
            }
            intersection = tempIntersection;
        }
    }

    auto end = chrono::high_resolution_clock::now(); // 结束计时
    chrono::duration<double, milli> localElapsedTime = end - start;
    double localElapsedTimeMs = localElapsedTime.count();
    MPI_Gather(&localElapsedTimeMs, 1, MPI_DOUBLE, elapsedTime.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        for (int i = 0; i < world_size; ++i) {
            cout << "Elapsed time for process " << i << ": " << elapsedTime[i] << " ms" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
