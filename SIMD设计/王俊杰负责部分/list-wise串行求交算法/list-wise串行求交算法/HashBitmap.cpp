#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fstream>
using namespace std;
const unsigned int MAX_SIZE = 40000000;  // λͼ������С
const unsigned int BLOCK_SIZE = 64;  // λͼ��Ĵ�С

// void saveToDisk(vector<bitset<MAX_SIZE>>* indexData, vector<vector<bitset<MAX_SIZE>>>* queryData) {
//     cout<<"saveToDisk"<<endl;
//     // ����һ������ļ���
//     ofstream outFile;

//     // ��һ���ļ���д��indexData
//     outFile.open("indexData.bin", std::ios::binary);
//     for (const auto& bitset : *indexData) {
//         for (int i = 0; i < MAX_SIZE; i += 64) {
//             std::bitset<64> smallBitset;
//             for (int j = 0; j < 64; ++j) {
//                 smallBitset[j] = bitset[i + j];
//             }
//             unsigned long long value = smallBitset.to_ullong();
//             outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
//         }
//     }
//     outFile.close();
//     cout<<"saveToDisk1"<<endl;

//     // ��һ���ļ���д��queryData
//     outFile.open("queryData.bin", std::ios::binary);
//     for (const auto& vector : *queryData) {
//         for (const auto& bitset : vector) {
//             for (int i = 0; i < MAX_SIZE; i += 64) {
//                 std::bitset<64> smallBitset;
//                 for (int j = 0; j < 64; ++j) {
//                     smallBitset[j] = bitset[i + j];
//                 }
//                 unsigned long long value = smallBitset.to_ullong();
//                 outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
//             }
//         }
//     }
//     outFile.close();
//     cout<<"saveToDisk2"<<endl;
// }

// void loadFromDisk(vector<bitset<MAX_SIZE>>* indexData, vector<vector<bitset<MAX_SIZE>>>* queryData) {
//     // ����һ�������ļ���
//     std::ifstream inFile;

//     // ��һ���ļ�����ȡindexData
//     inFile.open("indexData.bin", std::ios::binary);
//     if (!inFile) {
//         std::cerr << "Unable to open file indexData.bin";
//         exit(1);   // call system to stop
//     }
//     unsigned long long value;
//     while (inFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
//         indexData->push_back(bitset<MAX_SIZE>(value));
//     }
//     inFile.close();

//     // ��һ���ļ�����ȡqueryData
//     inFile.open("queryData.bin", std::ios::binary);
//     if (!inFile) {
//         std::cerr << "Unable to open file queryData.bin";
//         exit(1);   // call system to stop
//     }
//     vector<bitset<MAX_SIZE>> queryResult;
//     while (inFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
//         queryResult.push_back(bitset<MAX_SIZE>(value));
//         if (queryResult.size() == 5) {  // ����ÿ����ѯ�������5��λͼ
//             queryData->push_back(queryResult);
//             queryResult.clear();
//         }
//     }
//     inFile.close();
// }
int main() {
    cout<<MAX_SIZE / BLOCK_SIZE;

    // �򿪶������ļ� "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // �ڶ��ϴ���һ����άλͼ indexData ���洢����
    vector<bitset<MAX_SIZE>>* indexData = new vector<bitset<MAX_SIZE>>();
    // ����һ����������
    vector<bitset<MAX_SIZE / BLOCK_SIZE>>* secondaryIndexData = new vector<bitset<MAX_SIZE / BLOCK_SIZE>>();

    // ����ļ��ɹ���
    if (indexFile.is_open()) {
        cout << "indexFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        int www = 0;
        while (!indexFile.eof()) {
            // ��ȡ����ĳ���
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
            // �ڶ��ϴ���һ������Ϊ MAX_SIZE ��λͼ arrayData
            bitset<MAX_SIZE>* arrayData = new bitset<MAX_SIZE>();
            // �ڶ��ϴ���һ������Ϊ MAX_SIZE / BLOCK_SIZE ��λͼ secondaryIndex
            bitset<MAX_SIZE / BLOCK_SIZE>* secondaryIndex = new bitset<MAX_SIZE / BLOCK_SIZE>();
            // ѭ����ȡ�����ÿ��Ԫ��
            for (unsigned int i = 0; i < arrayLength; ++i) {
                // ��ȡԪ�ص�ֵ
                unsigned int value;
                indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));

                // ��Ԫ�ص�ֵ��Ӧ��λ����Ϊ1
                arrayData->set(value);
                secondaryIndex->set(value / BLOCK_SIZE);
            }
            // ��λͼ arrayData �����άλͼ indexData
            indexData->push_back(*arrayData);
            secondaryIndexData->push_back(*secondaryIndex);
            // �ͷŶ�̬������ڴ�
            delete arrayData;
        }
        // �ر��ļ�
        indexFile.close();
    }
  

// �򿪲�ѯ�ļ� "ExpQuery"
ifstream queryFile("ExpQuery");
// ����һ����άλͼ queryData ���洢��ѯ���
vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>* queryData = new vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>();

    // ����ļ��ɹ���
    if (queryFile.is_open()) {
        cout << "queryFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        string line;
        while (getline(queryFile, line)) {
            // ����һ����άλͼ queryResult ���洢��ѯ���
            vector<bitset<MAX_SIZE/ BLOCK_SIZE>> queryResult;
            // �����е�ÿ������ת��Ϊ�����ļ��������±꣬����ѯ��Ӧ��λͼ
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                // ��ѯ��Ӧ��λͼ��������� queryResult
                queryResult.push_back((*secondaryIndexData)[index]);
            }
            // ����άλͼ queryResult ������άλͼ queryData
            queryData->push_back(queryResult);
        }
        // �ر��ļ�
        queryFile.close();
    }
    //���queryData��ǰʮ����ѯ���
    for (int i = 0; i < 10 && i < queryData->size(); ++i) {
        cout << "Query Result " << i + 1 << ": ";
        for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
            cout << (*queryData)[i][j] << " ";
        }
        cout <<endl<<"------------------------------------------------------------------"<<endl;
    }
    //saveToDisk(indexData, queryData);

    // ��������Խ��к������󽻲���
    // ...

    // �ͷŶ�̬������ڴ�
    delete indexData;
    delete queryData;
    delete secondaryIndexData;

    return 0;
}

