#include <cstddef>
#include <fstream>
#include <vector>
#include <istream>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <sys/time.h>
#include <arm_neon.h>

using namespace std;

int main() {
    ifstream indexFile("ExpIndex", ios::binary);
    vector<vector<unsigned int>> indexData;

    if (indexFile.is_open()) {
        while (!indexFile.eof()) {
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
            vector<unsigned int> arrayData(arrayLength);
            indexFile.read(reinterpret_cast<char*>(arrayData.data()), arrayLength * sizeof(unsigned int));
            indexData.push_back(arrayData);
        }
        indexFile.close();
    } else {
        cout << "indexFile open failed!" << endl;
        return 0;
    }

    ifstream queryFile("ExpQuery");
    vector<vector<vector<unsigned int>>> queryData;

    if (queryFile.is_open()) {
        string line;
        while (getline(queryFile, line)) {
            vector<vector<unsigned int>> queryResult;
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                queryResult.push_back(indexData[index]);
            }
            queryData.push_back(queryResult);
        }
        queryFile.close();
    } else {
        cout << "queryFile open failed!" << endl;
        return 0;
    }

    vector<vector<unsigned int>> intersectionResults;
    size_t queryDataSize = queryData.size();
    cout << "queryDataSize: " << queryDataSize << endl;

    size_t times = 0;
    size_t index = 0;
    size_t step = 100;
    struct timeval t1, t2;
    vector<double> elapsedTime(queryDataSize / step);

    gettimeofday(&t1, NULL);

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

        times++;
        if (times % step == 0) {
            gettimeofday(&t2, NULL);
            elapsedTime[index] = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
            std::cout << "Elapsed time for 100 iterations: " << elapsedTime[index] << " ms.\n";
            index++;
            gettimeofday(&t1, NULL);
        }
    }

    for (size_t i = 0; i < queryDataSize / step; i++) {
        cout << "Elapsed time for 100 iterations: " << i << ":" << elapsedTime[i] << " ms.\n";
    }

    return 0;
}
