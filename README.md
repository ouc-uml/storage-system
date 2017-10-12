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

#### memory_operation.h

 - `void clear_all_nodes()`
  
   清除数据库中所有节点数据块。注意：危险函数，在使用之后必须重新安装数据库。
  
 - `void new_node_position(unsigned *filenum,unsigned *linenum)`
  
   分配新的存储块，把分配的存储块编号传入filenum和linenum

 - `void delete_node(int filenum,int linenum)`
  
   释放指定位置的数据块单元。
 - `unsigned long long used_memory()`
  
   获取当前使用的存储块单元总量。

### 第二层级
#### node_class.h
##### memory_location类，用于封装一个数据块的位置
```
unsigned filenum 数据块文件编号

unsigned linenum 数据块行编号

bool is_null() 判断当前数据块位置是否为空

void set_null() 将当前的数据块的位置置空

memory_location = (memory_location x) 将x的位置赋给本身

memory_location = (unsigned char x[]) 将x的前4字节赋给filenum，后4个字节赋给linenum

bool == (memory_location x) 判断当前的位置和x的位置是否相同
```

##### node_class类，抽象类，用于规范节点类的定义格式
```
memory_location self 节点本身的存储位置。

unsigned block_size 节点存储块的大小，一般为node_block_size(默认128)。

virtual void save() = 0 纯虚函数，规范节点保存到文件的接口。

virtual void load() = 0 纯虚函数，规范节点从文件中加载的接口。

void create() 为当前节点分配一个存储块单元，self的值指向该单元的编号，但当前单元的其他内容不变。

void release() 释放当前节点的存储块单元。
```

##### data_type类，用于封装key和value，为具体节点类提供服务
```
unsigned char key[32] 健存储单元，最多存储32字节信息

unsigned char value[32] 值存储单元，最多存储32字节信息

bool k_type 健类型，0代表unsigned，1代表unsigned char[]

bool v_type 值类型，0代表unsigned，1代表unsigned char[]

int cmpstr(unsigned char a[],unsigned char b[]) 比较两个无符号字符数据块的字典序大小，1代表a小于b，-1代表a大于b，0代表相等

bool < (data_type &x) 比较本身key值是否小于x的key值，根据k_type选择，type为1则全文比较，否则只比较前4字节

bool == (data_type &x) 比较本身key值是否等于x的key值，根据k_type选择，type为1则全文比较，否则只比较前4字节

data_type& = (data_type &x) 将x的key值和value值赋给本身，根据k_type和v_type进行选择，type为1则全块复制，否则只复制4字节

data_type& operator = (unsigned char x[]) 将x数据块赋给自身的key，根据type进行选择，如果为1则全块复制，否则只复制4字节
```
##### queue_node类 （ 继承node_class）
```
memory_location prev 该节点的前驱节点的存储位置编号

memory_location succ 该节点的后继节点的存储位置编号

data_type data 该节点存储的数据信息

void save() 将节点数据信息保存到文件中

void load() 从self指定的数据块中加载该节点的数据信息
```

##### binary_tree_node类 (继承node_class类)
```
memory_location left 左儿子的地址位置。

memory_location right 右儿子的地址位置。

int size 以该节点为根的子树大小。

int HASH 该节点在树中的唯一标志符。

int RANDOM 该节点的随机值，用于维护treap的平衡性。

void save() 将该节点的数据信息保存到文件。

void load() 将该节点self指向的数据块加载到节点中。

binary_tree_node& = (binary_tree_node& x) 将x的除自身位置信息外的所有信息赋给该节点。
```

#### treap.h

###### TREAP类
这是一种基于随机的二叉搜索平衡树，为map提供数据结构基础。

```
memory_location self 标记平衡树存储的位置

memory_location root 平衡树根节点的位置

bool k_type 平衡树的键类型

bool v_type 平衡树的值类型

unsigned hash_top 用于生成节点的hash值

unsigned char name[32] 树的名称

unsigned block_size 树节点的大小

int get_size() 获得树的节点个数

void adapt(memory_location x) 调整树节点的属性（用于左旋和右旋）

void left_rot(memory_location fa,memory_location x) 左旋

void right_rot(memory_location fa,memory_location x) 右旋

memory_location insert(memory_location fa,memory_location x,data_type data) 向指定的子树中插入数据

memory_location remove(memory_location fa,memory_location x,data_type data) 在指定的子树中删除数据

void Insert(data_type data) 向树中插入数据

memory_location Remove(data_type data) 在树中删除数据

memory_location Find(data_type data) 在树中查找数据

void show(memory_location x) 展示树中所有节点

void save() 保存树的信息到self指定的文件块

void load() 从self指定的文件块中加载树的信息

void create() 分配一个新的地址给self

void release_node(memory_location x) 释放指定子树所有节点的存储

void clear() 删除树中的所有节点

void release() 释放整棵树的所有内存
```
