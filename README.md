# storage-system
## 功能
该仓库可以被视为一个经过封装的非结构化数据库，提供了两种主要的数据结构，分别为map和list。
## 实现方法
该数据库的实现使用了三层架构，第一层为文件操作/存储调度层，第二层为数据结构层，第三层为数据结构管理层。

 - 文件操作/存储调度层，该层封装了一系列用于底层文件读写操作的函数，并利用文件局部锁实现了并发读写控制。同时存储调度系统可以动态地分配存储块，每个存储块都有一个唯一的标志符（编号），上层可以根据存储块的编号来寻址从而实现虚拟内存的效果。
 - 数据结构层，该层封装了两种数据结构：treap和list。treap是用来建立一对一映射的数据结构，而list主要用来进行顺序存储。treap的查询、删除、操作复杂度都是log级别。list可以用来方便地组织元组，尽管其不能进行高效查询。该层的数据结构利用文件操作/存储调度层封装过的函数进行分配存储块，建立节点、指针映射等。
 - 数据结构管理层，该层进一步封装了treap，形成map，同时封装了一系列数据库性质的接口用于为应用层的开发提供map和list的有效管理。该层的map和list都是使用根map来进行组织，可以很高效地通过命名来对map和list进行寻址。由于根map本身也是map，所以创建、删除、查询一个数据结构的复杂度都是log级别。

## 函数使用说明
### 第一层级
#### file_operation.h

 - `void load_configuration()`

   加载配置文件，在所有的数据操作进行之前，必须使用该函数加载配置信息（只需加载一次）。
   

 - `void write_node_block(unsigned char data[],int file_num,int line_num)`
 
   将无符号字符数据块写入指定文件的指定行。

 - `void read_node_block(int file_num,int line_num,unsigned char result[])`
  
   读取指定文件的指定行。

 - `int node_lines_num(int file_num)`
  
   获取指定文件的当前行数。

 - `unsigned trans_block_to_int(unsigned char block[],int start,int len)`
  
   将无符号字符数据块的内容转换为无符号整形，block是指定的数据块指针，start是开始转化的位置，len代表转换长度，一般取4。

 - `void trans_block_to_char_array(unsigned char block[],int start,int len,unsigned char ret[])`
  
   从数据块中截取一段，block代表源数据块，start是起始位置，len是截取长度，ret是目的数据块。

 - `void put_int_to_block(unsigned char block[],int start,unsigned num)`
  
   将整形以4个字节的形式写入数据块中，block是目的数据块，start是开始写入的位置，num是要写入的整形变量。

 - `void put_char_to_block(unsigned char block[],int block_start,int data_start,int len,unsigned char data[])`
  
   将源数据块写入到目的数据块中，block是目的数据块，block_start是写入的起始位置，data_start是源数据块开始的位置，data是源数据块。

 - 注意：最后四个函数是提供给第二层、第三层使用的，如果要进行数据块的转化操作，最好使用useful_tools.h中提供的uprint和uscan函数。
