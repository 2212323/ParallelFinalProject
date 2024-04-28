#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fstream>
#include <emmintrin.h> // Include SSE header
#include <smmintrin.h>
using namespace std;

const unsigned int MAX_SIZE = 40000000; // λͼ������С
const unsigned int BLOCK_SIZE = 128;    // λͼ��Ĵ�С
__m128i bitset_to_m128i(const std::bitset<128>& b) {
    if(b.size() != 128) {
        cout<<"bitset size is not 128"<<endl;
    }
    uint64_t lower = 0;
    uint64_t upper = 0;

    for (int i = 0; i < 64; ++i) {
        lower |= (uint64_t(b[i]) << i);
        upper |= (uint64_t(b[i + 64]) << i);
    }

    __m128i result = _mm_set_epi64x(upper, lower);
    return result;
}
int main()
{
    cout << MAX_SIZE / BLOCK_SIZE;

    // �򿪶������ļ� "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // �ڶ��ϴ���һ����άλͼ indexData ���洢����
    vector<bitset<MAX_SIZE>> *indexData = new vector<bitset<MAX_SIZE>>();
    // ����һ����������
    vector<bitset<MAX_SIZE / BLOCK_SIZE>> *secondaryIndexData = new vector<bitset<MAX_SIZE / BLOCK_SIZE>>();

    // ����ļ��ɹ���
    if (indexFile.is_open())
    {
        cout << "indexFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        int www = 0;
        while (!indexFile.eof())
        {
            // ��ȡ����ĳ���
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char *>(&arrayLength), sizeof(arrayLength));
            // �ڶ��ϴ���һ������Ϊ MAX_SIZE ��λͼ arrayData
            bitset<MAX_SIZE> *arrayData = new bitset<MAX_SIZE>();
            // �ڶ��ϴ���һ������Ϊ MAX_SIZE / BLOCK_SIZE ��λͼ secondaryIndex
            bitset<MAX_SIZE / BLOCK_SIZE> *secondaryIndex = new bitset<MAX_SIZE / BLOCK_SIZE>();
            // ѭ����ȡ�����ÿ��Ԫ��
            for (unsigned int i = 0; i < arrayLength; ++i)
            {
                // ��ȡԪ�ص�ֵ
                unsigned int value;
                indexFile.read(reinterpret_cast<char *>(&value), sizeof(value));

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
    vector<vector<bitset<MAX_SIZE>>> *BasequeryData = new vector<vector<bitset<MAX_SIZE>>>();
    // ����һ����άλͼ secondaryqueryData ���洢��ѯ�����������
    vector<vector<bitset<MAX_SIZE / BLOCK_SIZE>>> *secondaryqueryData = new vector<vector<bitset<MAX_SIZE / BLOCK_SIZE>>>();

    // ����ļ��ɹ���
    if (queryFile.is_open())
    {
        cout << "queryFile opened" << endl;
        // ѭ����ȡ�ļ�ֱ���ļ�ĩβ
        string line;
        while (getline(queryFile, line))
        {
            // ����һ����άλͼ queryResult ���洢��ѯ���
            vector<bitset<MAX_SIZE>> *BasequeryResult = new vector<bitset<MAX_SIZE>>;
            vector<bitset<MAX_SIZE / BLOCK_SIZE>> *secondaryqueryResult = new vector<bitset<MAX_SIZE / BLOCK_SIZE>>;

            // �����е�ÿ������ת��Ϊ�����ļ��������±꣬����ѯ��Ӧ��λͼ
            stringstream ss(line);
            unsigned int index;
            while (ss >> index)
            {
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
    // ���queryData��ǰʮ����ѯ���
    //  for (int i = 0; i < 10 && i < queryData->size(); ++i) {
    //      cout << "Query Result " << i + 1 << ": ";
    //      for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
    //          cout << (*queryData)[i][j] << " ";
    //      }
    //      cout <<endl<<"------------------------------------------------------------------"<<endl;
    //  }
    // saveToDisk(secondaryIndexData, queryData);

    // ��������Խ��к������󽻲���
    // ����һ����ά���� intersectionData ���洢�󽻽��
    cout << "intersectionData created" << endl;
    int DoNum = 100;
    vector<vector<unsigned int>> intersectionData(DoNum);
    //// ����ÿ����ѯ���
    for (int i = 0; i < DoNum; i++)
    {
        cout << "DoNum:" << i << endl;
        // ��ȡ��ǰ��ѯ����ĵ�һ��λͼ
        bitset<MAX_SIZE> *intersection = new bitset<MAX_SIZE>((*BasequeryData)[i][0]);
        bitset<MAX_SIZE / BLOCK_SIZE> *secondaryintersection = new bitset<MAX_SIZE / BLOCK_SIZE>((*secondaryqueryData)[i][0]); // ����ÿһ����ѯ��������ÿһ��λͼ
        bitset<MAX_SIZE> *tempintersection;
        bitset<MAX_SIZE / BLOCK_SIZE> *tempsecondaryintersection;
        bitset<128> *smallintersection;// ����һ���µ�bitset
        bitset<128> *smallquery;// ����һ���µ�bitset

        for (int j = 0; j < (*BasequeryData)[i].size(); ++j)
        {
            cout << " j:" << j << endl;
            // ����һ��һάλͼ tempIntersection ���洢��ǰλͼ�� intersection �Ľ���
            tempintersection = new bitset<MAX_SIZE>();
            tempsecondaryintersection = new bitset<MAX_SIZE / BLOCK_SIZE>();
            // ������ǰλͼ��ÿ����
            for (int k = 0; k < MAX_SIZE / BLOCK_SIZE; k++)
            {

                // �����ǰλͼ�Ķ��������� intersection �Ķ��������н���
                if ((*secondaryintersection)[k] & (*secondaryqueryData)[i][j][k])
                {
                    //cout << " k:" << k << endl;
                    smallintersection=new bitset<128>() ;// ����һ���µ�bitset
                    smallquery=new bitset<128>() ;// ����һ���µ�bitset

                    // ����bitset�е��ض���Χ��λ���Ƶ�Сbitset��
                    for (int l = 0; l < BLOCK_SIZE; l++) {
                        (*smallintersection)[l] = (*intersection)[k*BLOCK_SIZE + l];
                        (*smallquery)[l] = (*BasequeryData)[i][j][k*BLOCK_SIZE + l];
                    }
                    // ��bitset�ж�ȡ64λ����ת��Ϊunsigned long long
                    // unsigned long long lower1 = smallintersection->to_ullong();
                    // unsigned long long upper1 = (*smallintersection >> 64).to_ullong();
                    // unsigned long long lower2 = smallquery->to_ullong();
                    // unsigned long long upper2 = (*smallquery >> 64).to_ullong();

                    // smallintersection=nullptr;
                    // smallquery=nullptr;
                    // // ʹ��_mm_set_epi64x����������unsigned long longֵ��ȡ��__m128i��
                    // __m128i vec1 = _mm_set_epi64x(upper1, lower1);
                    // __m128i vec2 = _mm_set_epi64x(upper2, lower2);
                    
                    
                    __m128i vec1 = bitset_to_m128i(*smallintersection);
                    __m128i vec2 = bitset_to_m128i(*smallquery);
                    smallintersection=nullptr;
                    smallquery=nullptr;

                    // ʹ��_mm_and_si128����ִ�а�λ�����
                    __m128i andResult = _mm_and_si128(vec1, vec2);
                    // ��__m128iת��Ϊ����unsigned long long
                    unsigned long long lower = _mm_cvtsi128_si64(andResult);
                    unsigned long long upper = _mm_cvtsi128_si64(_mm_srli_si128(andResult, 8));

                    // ������unsigned long longֵд��bitset
                    for (int l = 0; l < 64; ++l) {
                         (*tempintersection)[l+k*BLOCK_SIZE] = (lower >> l) & 1;
                         (*tempintersection)[l+k*BLOCK_SIZE + 64] = (upper >> l) & 1;
                        if(j==(*BasequeryData)[i].size()-1){
                            if((*tempintersection)[l+k*BLOCK_SIZE] == 1){
                                intersectionData[i].push_back(l+k*BLOCK_SIZE);
                            }
                            if((*tempintersection)[l+k*BLOCK_SIZE + 64] == 1){
                                intersectionData[i].push_back(l+k*BLOCK_SIZE + 64);
                            }
                        }
                    }
                    if(lower || upper)
                    {
                        tempsecondaryintersection->set(k);
                    }

                }
            }

            // �� tempIntersection ����Ϊ�µ� intersection
            intersection = tempintersection;
            secondaryintersection = tempsecondaryintersection;
            tempintersection = nullptr;
            tempsecondaryintersection = nullptr;

    }


// �ͷŶ�̬������ڴ�
delete intersection;
delete secondaryintersection;
//delete tempintersection;
//delete tempsecondaryintersection;
}
    
// ѭ������󽻽����ǰ5��
for (int i = 0; i < DoNum; ++i)
{
    cout << "�󽻽����ǰ5��Ϊ��";
    int count = 0;
    for (const auto &value : intersectionData[i])
    {
        cout << value << " ";
        count++;
        if (count == 5)
        {
            break;
        }
    }
    cout << endl
         << "----------------------" << endl;
}
// �ͷŶ�̬������ڴ�
delete indexData;
delete secondaryIndexData;
delete BasequeryData;
delete secondaryqueryData;

return 0;
}
