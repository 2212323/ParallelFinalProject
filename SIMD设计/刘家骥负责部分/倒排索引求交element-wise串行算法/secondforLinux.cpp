#include <bitset>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/time.h> //gettimeofday()

using namespace std;
const unsigned int MAX_SIZE = 40000000; // 位图的最大大小
const unsigned int BLOCK_SIZE = 64;     // 位图块的大小

int main() {
  cout << MAX_SIZE / BLOCK_SIZE<<endl;

  // 打开二进制文件 "ExpIndex"
  ifstream indexFile("ExpIndex", ios::binary);
  // 在堆上创建一个二维位图 indexData 来存储数据
  vector<bitset<MAX_SIZE>> *indexData = new vector<bitset<MAX_SIZE>>();
  // 创建一个二级索引
  vector<bitset<MAX_SIZE / BLOCK_SIZE>> *secondaryIndexData =
      new vector<bitset<MAX_SIZE / BLOCK_SIZE>>();
  vector<unsigned int> query_Lengths;
  // 如果文件成功打开
  if (indexFile.is_open()) {
    cout << "indexFile opened" << endl;
    // 循环读取文件直到文件末尾
    int www = 0;

    while (!indexFile.eof()) {
      // 读取数组的长度
      unsigned int arrayLength;
      indexFile.read(reinterpret_cast<char *>(&arrayLength),
                     sizeof(arrayLength));

      query_Lengths.push_back(arrayLength);//存储每个数组的长度，按照下标排序

      // 在堆上创建一个长度为 MAX_SIZE 的位图 arrayData
      bitset<MAX_SIZE> *arrayData = new bitset<MAX_SIZE>();
      // 在堆上创建一个长度为 MAX_SIZE / BLOCK_SIZE 的位图 secondaryIndex
      bitset<MAX_SIZE / BLOCK_SIZE> *secondaryIndex = new bitset<MAX_SIZE / BLOCK_SIZE>();

      // 循环读取数组的每个元素
      for (unsigned int i = 0; i < arrayLength; ++i) {
        // 读取元素的值
        unsigned int value;
        indexFile.read(reinterpret_cast<char *>(&value), sizeof(value));

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
  vector<vector<bitset<MAX_SIZE>>> *BasequeryData =
      new vector<vector<bitset<MAX_SIZE>>>();
  // 创建一个三维位图 secondaryqueryData 来存储查询二级索引结果
  vector<vector<bitset<MAX_SIZE / BLOCK_SIZE>>> *secondaryqueryData =
      new vector<vector<bitset<MAX_SIZE / BLOCK_SIZE>>>();

  vector<vector<unsigned int>> query_Lengths_searched(1000);//有1000组数据（需随数据集修改）

  // 如果文件成功打开
  if (queryFile.is_open()) {
    cout << "queryFile opened" << endl;
    // 循环读取文件直到文件末尾
    string line;
    size_t count = 0;//记录数组最高维记录到那一步了
    while (getline(queryFile, line)) {
      // 创建一个二维位图 queryResult 来存储查询结果
      vector<bitset<MAX_SIZE>> BasequeryResult;
      vector<bitset<MAX_SIZE / BLOCK_SIZE>> secondaryqueryResult;

      // 将行中的每个数字转换为索引文件的数组下标，并查询对应的位图
      stringstream ss(line);
      unsigned int index;
      while (ss >> index) {
        // 查询对应的位图并将其存入 queryResult
        secondaryqueryResult.push_back((*secondaryIndexData)[index]);
        BasequeryResult.push_back((*indexData)[index]);
        cout<<"index:"<<index<<endl;
        query_Lengths_searched[count].push_back(query_Lengths[index]);

      }
      count++;
      cout<<"count:"<<count<<endl;
      // 将二维位图 queryResult 存入三维位图 queryData
      secondaryqueryData->push_back(secondaryqueryResult);
      BasequeryData->push_back(BasequeryResult);
    }
    // 关闭文件
    queryFile.close();
  }

//   for (const auto &innerVec : query_Lengths_searched) {
//     for (const auto &element : innerVec) {
//         cout << element << ' ';
//     }
//     cout << '\n'<<'\n';
//     cout<<"-----------------"<<endl;
// }
//   cout<<"query_Lengths:"<<query_Lengths_searched.size()<<endl;

  // // 输出queryData的前十个查询结果
  // for (int i = 0; i < 10 && i < BasequeryData->size(); ++i) {
  //   cout << "Query Result " << i + 1 << ": ";
  //   for (int j = 0; j < 5 && j < (*BasequeryData)[i].size(); ++j) {
  //     cout << (*BasequeryData)[i][j] << " ";
  //   }
  //   cout << endl
  //        << "------------------------------------------------------------------"
  //        << endl;
  // }
  // saveToDisk(indexData, queryData);


  //二级索引指针：secondaryqueryData，一级索引指针：BasequeryData





    size_t queryDataSize = BasequeryData->size(); //1000,组数，需要查询的个数，也是查询结果的个数
    //cout << "queryDataSize: " << queryDataSize << endl;
    size_t secondaryqueryDataSize = secondaryqueryData->size();//1000组二级索引，每组以64为单位
    //cout << "secondaryqueryDataSize: " << secondaryqueryDataSize << endl;
    vector<bitset<MAX_SIZE>> *intersectionResults = new vector<bitset<MAX_SIZE>>();
    vector<bitset<MAX_SIZE/BLOCK_SIZE>> *intersectionResults_second = new vector<bitset<MAX_SIZE/BLOCK_SIZE>>();



    size_t times=0;
    size_t index=0;
    size_t step=100;//每多少组数据测试一次时间
    struct timeval t1, t2; // Use struct timeval to record time
    vector<double> elapsedTime(queryDataSize/step);

    gettimeofday(&t1, NULL); // Start timer


    for(size_t i = 0; i < secondaryqueryDataSize; i++)
    {
      size_t minSize = query_Lengths_searched[i][0];
      size_t minIndex = 0;//初始化最短列表的数组下标

      for(size_t j = 0; j < query_Lengths_searched[i].size(); j++)//queryData[i].size()为每组列表的个数
      {
        if(query_Lengths_searched[i][j] < minSize)
        {

          minSize = query_Lengths_searched[i][j];
          minIndex = j;
        }
      }


      bitset<MAX_SIZE / BLOCK_SIZE> *S_second = new bitset<MAX_SIZE/BLOCK_SIZE>();
      *S_second=(*secondaryqueryData)[i][minIndex];//最短列表的二级索引
      bitset<MAX_SIZE> *S = new bitset<MAX_SIZE>();//
      *S=(*BasequeryData)[i][minIndex];//最短列表(位图)
        for(size_t k = 0; k < query_Lengths_searched[i].size(); k++)//检查
        {
          if(k==minIndex)//跳过最短列表
          {
            continue;
          }
          for(size_t l = 0; l < MAX_SIZE / BLOCK_SIZE; l++)
          {
            bool flag_second=false;
            if((*S_second)[l] & (*secondaryqueryData)[i][k][l])//如果按位与结果为1,进去检查一遍，有可能两数不同
            {
              for(size_t m = l*BLOCK_SIZE;  m < l*BLOCK_SIZE + BLOCK_SIZE; m++)//检查范围
              {
                 if(!((*S)[m]&(*BasequeryData)[i][k][m]))
                 {
                     S->reset(m);
                 }
                 else{//这组中存在相交的元素,flag置为true，表示二级索引此处应为1
                    flag_second=true;
                 }
              }
              if(!flag_second)
              {
                  S_second->reset(l);//此处为假1
              }
            }
            else
            {

              S_second->reset(l);//第l个BLOCK_SIZE位
              //进入一级索引置零
              for(size_t m = l*BLOCK_SIZE;  m < l*BLOCK_SIZE + BLOCK_SIZE; m++)//检查范围
              {
                  S->reset(m);
              }

            }
          }//进行求交算法

        cout<<i<<" k:"<<k<<endl;
        }
      intersectionResults->push_back(*S);
      intersectionResults_second->push_back(*S_second);

        times++;
        if(times%step==0)
        {
            gettimeofday(&t2, NULL); // Stop timer

        // compute and print the elapsed time in millisec
        elapsedTime[index] = (t2.tv_sec - t1.tv_sec) * 1000.0;// sec to ms
        std::cout << "Elapsed time for 100 iterations: " << elapsedTime[index] << " ms.\n";
        index++;
        gettimeofday(&t1, NULL); // Reset the start timer for the next 100 iterations
        }


      delete S;
      delete S_second;
    }
  //cout << "intersectionResults.size(): " << intersectionResults->size() << endl;
  //cout << "intersectionResults_second.size(): " << intersectionResults_second->size() << endl;

//   //翻译intersectionResults结果
//   for(size_t i = 0; i<5&&i < intersectionResults_second->size(); i++)
//   {
//     cout << "intersectionResults " << i + 1 << ": ";
//     for(size_t j = 0; j < MAX_SIZE/BLOCK_SIZE; j++)
//     {
//       if((*intersectionResults_second)[i][j])//如果是1
//       {
//         for(size_t k = j*BLOCK_SIZE; k < j*BLOCK_SIZE + BLOCK_SIZE; k++)
//         {
//           if((*intersectionResults)[i][k])
//           {
//             cout << k << " ";
//           }

//         }

//       }
//     }
//     cout << endl;
//     cout << "------------------------------------------------------------------" << endl;
//   }
  
    for(size_t i=0;i<queryDataSize/step;i++)
    {
        cout<<"Elapsed time for 100 iterations: "<<i<<":"<<elapsedTime[i]<<" ms.\n";
    }



  // 释放动态分配的内存
  delete indexData;
  delete BasequeryData;
  delete secondaryIndexData;
  delete intersectionResults;
  delete intersectionResults_second;


  return 0;
}
