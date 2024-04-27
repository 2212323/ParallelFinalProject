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
            delete secondaryIndex;
        }
        // �ر��ļ�
        indexFile.close();
    }
  

// �򿪲�ѯ�ļ� "ExpQuery"
ifstream queryFile("ExpQuery");
// ����һ����άλͼ BasequeryData ���洢��ѯһ���������
vector<vector<bitset<MAX_SIZE>>>* BasequeryData = new vector<vector<bitset<MAX_SIZE>>>();
// ����һ����άλͼ secondaryqueryData ���洢��ѯ�����������
vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>* secondaryqueryData = new vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>();

    // ����ļ��ɹ���
    if (queryFile.is_open()) {
        cout << "queryFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        string line;
        while (getline(queryFile, line)) {
            // ����һ����άλͼ queryResult ���洢��ѯ���
            vector<bitset<MAX_SIZE>> *BasequeryResult=new vector<bitset<MAX_SIZE>>;
            vector<bitset<MAX_SIZE/ BLOCK_SIZE>> *secondaryqueryResult=new vector<bitset<MAX_SIZE/ BLOCK_SIZE>>;

            // �����е�ÿ������ת��Ϊ�����ļ��������±꣬����ѯ��Ӧ��λͼ
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                // ��ѯ��Ӧ��λͼ��������� queryResult
                secondaryqueryResult->push_back((*secondaryIndexData)[index]);
                BasequeryResult->push_back((*indexData)[index]);

            }
            // ����άλͼ queryResult ������άλͼ queryData
            secondaryqueryData->push_back(*secondaryqueryResult);
            BasequeryData->push_back(*BasequeryResult);
        }
        // �ر��ļ�
        queryFile.close();
    }
    //���queryData��ǰʮ����ѯ���
    // for (int i = 0; i < 10 && i < queryData->size(); ++i) {
    //     cout << "Query Result " << i + 1 << ": ";
    //     for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
    //         cout << (*queryData)[i][j] << " ";
    //     }
    //     cout <<endl<<"------------------------------------------------------------------"<<endl;
    // }
    //saveToDisk(secondaryIndexData, queryData);

    // ��������Խ��к������󽻲���
    // ����һ����ά���� intersectionData ���洢�󽻽��
    cout<<"intersectionData created"<<endl;
    int DoNum = 100;
    vector<vector<unsigned int>> intersectionData(DoNum);
    //// ����ÿ����ѯ���
    for(int i=0;i<DoNum;i++){
        cout<<"DoNum:"<<i<<endl;
        // ��ȡ��ǰ��ѯ����ĵ�һ��λͼ
        bitset<MAX_SIZE>* intersection = new bitset<MAX_SIZE>((*BasequeryData)[i][0]);
        bitset<MAX_SIZE/BLOCK_SIZE>* secondaryintersection = new bitset<MAX_SIZE/BLOCK_SIZE>((*secondaryqueryData)[i][0]);        // ����ÿһ����ѯ��������ÿһ��λͼ
        for (int j = 0; j < (*BasequeryData)[i].size(); ++j) {
            cout<<" j:"<<j<<endl;
           // ����һ��һάλͼ tempIntersection ���洢��ǰλͼ�� intersection �Ľ���
            bitset<MAX_SIZE>* tempintersection = new bitset<MAX_SIZE>();
            bitset<MAX_SIZE/BLOCK_SIZE>* tempsecondaryintersection = new bitset<MAX_SIZE/BLOCK_SIZE>();
           // ������ǰλͼ��ÿ����
           for(int k=0;k<MAX_SIZE/ BLOCK_SIZE;k++){
               // �����ǰλͼ�Ķ��������� intersection �Ķ��������н���
               if((*secondaryintersection)[k] & (*secondaryqueryData)[i][j][k]){
                   // ������ǰ���ÿ��λ
                   for(int l=0;l<BLOCK_SIZE;l++){
                       // �����ǰλͼ��λ�� intersection ��λ�н���
                       if((*intersection)[k*BLOCK_SIZE+l] & (*BasequeryData)[i][j][k*BLOCK_SIZE+l]){
                           // ����ǰλ����Ϊ1�������¶�������
                           tempintersection->set(k*BLOCK_SIZE+l);
                           tempsecondaryintersection->set(k);
                           if(j==(*BasequeryData)[i].size()-1){
                               //cout<<"intersectionData:"<<k*BLOCK_SIZE+l<<endl;
                               intersectionData[i].push_back(k*BLOCK_SIZE+l);

                           }
                       }
                   }
               }
           }
            // �� tempIntersection ����Ϊ�µ� intersection
        intersection = tempintersection;
        secondaryintersection = tempsecondaryintersection;

        
        }


        // �ͷŶ�̬������ڴ�
        delete intersection;
        delete secondaryintersection;


   
    }

     // ѭ������󽻽����ǰ5��
    for (int i = 0; i < DoNum; ++i) {
        cout << "�󽻽����ǰ5��Ϊ��";
        int count = 0;
        for (const auto& value : intersectionData[i]) {
            cout << value << " ";
            count++;
            if (count == 5) {
                break;
            }
        }
        cout <<endl<< "----------------------" << endl;

    // �ͷŶ�̬������ڴ�
    delete indexData;
    delete secondaryIndexData;
    delete BasequeryData;
    delete secondaryqueryData;

    return 0;
}

