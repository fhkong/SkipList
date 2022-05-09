 ```
level 4                                      50
                 

level 3                      10              50        70     100
                                                 
                                                 
level 2         1            10    30        50        70     100
                                                 
                                                  
level 1         1    3       10    30        50        70     100
                                                   
                                                   
level 0         1    3   5   10    30   40   50   60   70     100
```

<p align=center><img src="https://img.shields.io/badge/DataStruct-SkipList-blue" alt=""><img src="https://img.shields.io/badge/ConcurrentTest-Passed-orange" alt=""></a> </p>

## 1. 引言

墨尔本大学计算机本科实验作业，使用C语言实现一个简单的SkipList结构。受此启发，本项目在此之上实现一个强化版本的SkipList。  
SkipList作为B+Tree的一种替代品具有很多的应用场景，著名的`Google/leveldb`中的内存表就是使用SkipList作为存储结构，其性能可以与B+Tree相媲美。  
更多有关SkipList的算法设计和理论分析参考[skiplist1990.pdf](./resources/skiplist1990.pdf)论文   


## 2. 项目结构
项目构建按照标准的C++项目格式，包含完整的代码测试。
```
.
├── build  //项目构建文件夹
├── build_support  // 构建项目支持文件夹
├── CMakeLists.txt 
├── notes  // 笔记
├── readme.md
├── resources  // 项目资源
├── src  // 项目源码
├── test  // 测试文件
└── third_party  // 第三方库支持
```

## 3. 代码编写
### 3.1 接口
项目预留了增删查改基本的操作，以及文件插入和输出SkipList。  
- Insert(key, value)
- Remove(key)
- Lookup(key, result)
- InsertFromFile(filename)
- Print()

### 3.2 SkipList结构  
SkipList中需要控制的超参数主要有：
1. 高度(max_height)。SkipList的高度需要控制，一般来讲高度设置在12左右差不多可以满足需求
2. 概率(branching)。每个元素以多大的概率(1/branching)增加高度，一般设置branching为2，即概率为1/2
3. 比较器(comparator)。由于需要按照key进行排序，所以需要自定义key的大小比较函数（仿生函数）

## 4. 快速食用
### 4.1. 构建项目
#### 4.1.1 克隆项目
`mkdir build && cd build`   
`cmake ..`  
####  4.1.2 运行测试  
`make skiplist_sequential_test`   
`./test/skiplist_sequential_test`   
`make skiplist_concurrent_test`    
`./test/skiplist_concurrent_test`   

#### 4.1.3 性能测试
`make skiplist_performance_test`  
`./test/skiplist_performance_test`  

### 4.2 作为插件
将`./src`目录下的头文件和源文件放入项目编译即可。
> 如果需要自定义Key, Value类型，只需做如下两个步骤即可。  
1. 在`generic_key.h`中添加Key Value类型  
2. 在`skiplist.cpp`文件中最后一行加入`template class SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>>;`来预定义。




**TODO**

- [x] 编码格式。遵循Google C++ Style编写
- [x] 代码抽象。将Key, Value抽象出来, 支持多种类型
- [ ] 并发模块。Reader Writer Latch, 支持并发读。
- [x] 测试模块。使用GTest对源码测试，包括正确, 并发, 性能
- [x] 迭代器。 能够使用迭代器访问SkipList,    `for (auto iter : skiplist)`



## 参考资料
1. [Google/leveldb](https://github.com/google/leveldb)
2. [cmu-db/bustub](https://github.com/cmu-db/bustub)