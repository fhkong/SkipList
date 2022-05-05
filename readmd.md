TODO

- [ ] 编码格式。按照Google format编写，CMake，Format.
- [ ] 代码抽象。将key，value抽象出来，能够支持多种类型的插入。

- [ ] 并发模块。Reader Writer Latch，支持并发读。
- [ ] 测试模块。使用GTest对源码测试，包括正确性，并发性，高效性。
- [ ] 性能测试。每秒的读写次数，顺序读写，随机读写等等。

<a href="https://mp.weixin.qq.com/s/RsdcQ9umo09R6cfnwXZlrQ"><img src="https://img.shields.io/badge/PDF下载-代码随想录-blueviolet" alt=""></a>  <a href="https://mp.weixin.qq.com/s/b66DFkOp8OOxdZC_xLZxfw"><img src="https://img.shields.io/badge/刷题-微信群-green" alt=""></a>  <a href="https://space.bilibili.com/525438321"><img src="https://img.shields.io/badge/B站-代码随想录-orange" alt=""></a><a href="https://mp.weixin.qq.com/s/QVF6upVMSbgvZy8lHZS3CQ"><img src="https://img.shields.io/badge/知识星球-代码随想录-blue" alt=""></a>

## 引言

墨尔本大学计算机本科实验作业，使用C语言实现一个简单的SkipList结构。本项目在此之上实现一个强化版本的SkipList。

## 项目结构
```
├─notes   // 笔记
├─resources // 资源文件
├─build_support // 项目支持文件
├─performance  // 系统性能
├─src  // 源码文件
└─test // 测试文件
```

## 代码编写
1. 用到测试的时候，可以指定种子从而保证产出的随机数不变，保证skiplist的结构不变。
2. template<typename Key, typename Value, typename KeyComparator>




## 快速食用


## 参考资料
1. [leveldb]()
2. [bustub]()