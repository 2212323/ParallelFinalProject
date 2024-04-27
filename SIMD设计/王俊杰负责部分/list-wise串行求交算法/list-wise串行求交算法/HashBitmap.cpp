#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fstream>
using namespace std;

const unsigned int MAX_SIZE = 40000000;  // 位图的最大大小
const unsigned int BLOCK_SIZE = 64;  // 位图块的大小

int main() {
    cout<<MAX_SIZE / BLOCK_SIZE;

    // 打开二进制文件 "ExpIndex"
    ifstream indexFile("ExpIndex", ios::binary);
    // 在堆上创建一个二维位图 indexData 来存储数据
    vector<bitset<MAX_SIZE>>* indexData = new vector<bitset<MAX_SIZE>>();
    // 创建一个二级索引
    vector<bitset<MAX_SIZE / BLOCK_SIZE>>* secondaryIndexData = new vector<bitset<MAX_SIZE / BLOCK_SIZE>>();

    // 如果文件成功打开
    if (indexFile.is_open()) {
        cout << "indexFile opened" << endl;
        // 循环读取文件直到文件末尾
        int www = 0;
        while (!indexFile.eof()) {
            // 读取数组的长度
            unsigned int arrayLength;
            indexFile.read(reinterpret_cast<char*>(&arrayLength), sizeof(arrayLength));
            // 在堆上创建一个长度为 MAX_SIZE 的位图 arrayData
            bitset<MAX_SIZE>* arrayData = new bitset<MAX_SIZE>();
            // 在堆上创建一个长度为 MAX_SIZE / BLOCK_SIZE 的位图 secondaryIndex
            bitset<MAX_SIZE / BLOCK_SIZE>* secondaryIndex = new bitset<MAX_SIZE / BLOCK_SIZE>();
            // 循环读取数组的每个元素
            for (unsigned int i = 0; i < arrayLength; ++i) {
                // 读取元素的值
                unsigned int value;
                indexFile.read(reinterpret_cast<char*>(&value), sizeof(value));

                // 将元素的值对应的位设置为1
                arrayData->set(value);
                secondaryIndex->set(value / BLOCK_SIZE);
            }
            // 将位图 arrayData 存入二维位图 indexData
            indexData->push_back(*arrayData);
            secondaryIndexData->push_back(*secondaryIndex);
            // 释放动态分配的内存
            delete arrayData;
            delete secondaryIndex;
        }
        // 关闭文件
        indexFile.close();
    }
  

// 打开查询文件 "ExpQuery"
ifstream queryFile("ExpQuery");
// 创建一个三维位图 BasequeryData 来存储查询一级索引结果
vector<vector<bitset<MAX_SIZE>>>* BasequeryData = new vector<vector<bitset<MAX_SIZE>>>();
// 创建一个三维位图 secondaryqueryData 来存储查询二级索引结果
vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>* secondaryqueryData = new vector<vector<bitset<MAX_SIZE/ BLOCK_SIZE>>>();

    // 如果文件成功打开
    if (queryFile.is_open()) {
        cout << "queryFile opened" << endl;
        // 循环读取文件直到文件末尾
        string line;
        while (getline(queryFile, line)) {
            // 创建一个二维位图 queryResult 来存储查询结果
            vector<bitset<MAX_SIZE>> *BasequeryResult=new vector<bitset<MAX_SIZE>>;
            vector<bitset<MAX_SIZE/ BLOCK_SIZE>> *secondaryqueryResult=new vector<bitset<MAX_SIZE/ BLOCK_SIZE>>;

            // 将行中的每个数字转换为索引文件的数组下标，并查询对应的位图
            stringstream ss(line);
            unsigned int index;
            while (ss >> index) {
                // 查询对应的位图并将其存入 queryResult
                secondaryqueryResult->push_back((*secondaryIndexData)[index]);
                BasequeryResult->push_back((*indexData)[index]);

            }
            // 将二维位图 queryResult 存入三维位图 queryData
            secondaryqueryData->push_back(*secondaryqueryResult);
            BasequeryData->push_back(*BasequeryResult);
        }
        // 关闭文件
        queryFile.close();
    }
    //输出queryData的前十个查询结果
    // for (int i = 0; i < 10 && i < queryData->size(); ++i) {
    //     cout << "Query Result " << i + 1 << ": ";
    //     for (int j = 0; j < 5 && j < (*queryData)[i].size(); ++j) {
    //         cout << (*queryData)[i][j] << " ";
    //     }
    //     cout <<endl<<"------------------------------------------------------------------"<<endl;
    // }
    //saveToDisk(secondaryIndexData, queryData);

    // 在这里可以进行后续的求交操作
    // 创建一个二维向量 intersectionData 来存储求交结果
    cout<<"intersectionData created"<<endl;
    int DoNum = 100;
    vector<vector<unsigned int>> intersectionData(DoNum);
    //// 遍历每个查询结果
    for(int i=0;i<DoNum;i++){
        cout<<"DoNum:"<<i<<endl;
        // 获取当前查询结果的第一个位图
        bitset<MAX_SIZE>* intersection = new bitset<MAX_SIZE>((*BasequeryData)[i][0]);
        bitset<MAX_SIZE/BLOCK_SIZE>* secondaryintersection = new bitset<MAX_SIZE/BLOCK_SIZE>((*secondaryqueryData)[i][0]);        // 对于每一个查询结果里面的每一个位图
        for (int j = 0; j < (*BasequeryData)[i].size(); ++j) {
            cout<<" j:"<<j<<endl;
           // 创建一个一维位图 tempIntersection 来存储当前位图与 intersection 的交集
            bitset<MAX_SIZE>* tempintersection = new bitset<MAX_SIZE>();
            bitset<MAX_SIZE/BLOCK_SIZE>* tempsecondaryintersection = new bitset<MAX_SIZE/BLOCK_SIZE>();
           // 遍历当前位图的每个块
           for(int k=0;k<MAX_SIZE/ BLOCK_SIZE;k++){
               // 如果当前位图的二级索引与 intersection 的二级索引有交集
               if((*secondaryintersection)[k] & (*secondaryqueryData)[i][j][k]){
                   // 遍历当前块的每个位
                   for(int l=0;l<BLOCK_SIZE;l++){
                       // 如果当前位图的位与 intersection 的位有交集
                       if((*intersection)[k*BLOCK_SIZE+l] & (*BasequeryData)[i][j][k*BLOCK_SIZE+l]){
                           // 将当前位设置为1，并更新二级索引
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
            // 将 tempIntersection 更新为新的 intersection
        intersection = tempintersection;
        secondaryintersection = tempsecondaryintersection;

        
        }


        // 释放动态分配的内存
        delete intersection;
        delete secondaryintersection;


   
    }

     // 循环输出求交结果的前5项
    for (int i = 0; i < DoNum; ++i) {
        cout << "求交结果的前5项为：";
        int count = 0;
        for (const auto& value : intersectionData[i]) {
            cout << value << " ";
            count++;
            if (count == 5) {
                break;
            }
        }
        cout <<endl<< "----------------------" << endl;

    // 释放动态分配的内存
    delete indexData;
    delete secondaryIndexData;
    delete BasequeryData;
    delete secondaryqueryData;

    return 0;
}

