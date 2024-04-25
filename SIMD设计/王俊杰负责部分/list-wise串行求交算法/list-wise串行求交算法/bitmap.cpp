#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
using namespace std;
const unsigned int MAX_SIZE = 40000000;  // λͼ������С
int main() {
    // �򿪶������ļ� "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // �ڶ��ϴ���һ����άλͼ indexData ���洢����
    vector<bitset<MAX_SIZE>>* indexData = new vector<bitset<MAX_SIZE>>();

    // ����ļ��ɹ���
    if (indexFile.is_open()) {
        cout << "indexFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        while (!indexFile.eof()) {
            // ��ȡ����ĳ���
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
            // �ڶ��ϴ���һ������Ϊ MAX_SIZE ��λͼ arrayData
            bitset<MAX_SIZE>* arrayData = new bitset<MAX_SIZE>();
            // ѭ����ȡ�����ÿ��Ԫ��
            for (unsigned int i = 0; i < arrayLength; ++i) {
                // ��ȡԪ�ص�ֵ
                unsigned int value;
                indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));
                // ��Ԫ�ص�ֵ��Ӧ��λ����Ϊ1
                arrayData->set(value);
            }
            // ��λͼ arrayData �����άλͼ indexData
            indexData->push_back(*arrayData);
            // �ͷŶ�̬������ڴ�
            delete arrayData;
        }
        // �ر��ļ�
        indexFile.close();
    }

    // �򿪲�ѯ�ļ� "ExpQuery"
    ifstream queryFile("ExpQuery");
    // ����һ����άλͼ queryData ���洢��ѯ���
    vector<vector<bitset<MAX_SIZE>>>* queryData = new vector<vector<bitset<MAX_SIZE>>>();

    // ����ļ��ɹ���
    if (queryFile.is_open()) {
        cout << "queryFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        string line;
        while (getline(queryFile, line)) {
            // ����һ����άλͼ queryResult ���洢��ѯ���
            vector<bitset<MAX_SIZE>> queryResult;
            // �����е�ÿ������ת��Ϊ�����ļ��������±꣬����ѯ��Ӧ��λͼ
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                // ��ѯ��Ӧ��λͼ��������� queryResult
                queryResult.push_back((*indexData)[index]);
            }
            // ����άλͼ queryResult ������άλͼ queryData
            queryData->push_back(queryResult);
        }
        // �ر��ļ�
        queryFile.close();
    }
    // ���queryData��ǰʮ����ѯ���
    for (int i = 0; i < 10 && i < queryData->size(); ++i) {
        cout << "Query Result " << i + 1 << ": ";
        for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
            cout << (*queryData)[i][j] << " ";
        }
        cout << endl;
    }

    // ��������Խ��к������󽻲���
    // ...

    // �ͷŶ�̬������ڴ�
    delete indexData;
    delete queryData;

    return 0;
}

