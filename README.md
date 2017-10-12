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

### 第三层级
第三层级的类直接为上层提供接口，因此需要熟悉。一些不重要的类成员变量和函数被省去。第三层级的每个头文件只包含有一个类。
#### db_list.h，db_list类
 - `db_list(const char x[],char k_ty)` 
 
   构造函数，指定名称和值的类型
  
 - `db_list(){}`
 
   空构造函数，得到一个未初始化的db_list实例

 - `void push_tail(unsigned char x[])`
 
   向末尾添加一个无符号字符数据块
  
 - `void push_tail(unsigned x)`
 
   向末尾添加一个无符号整型
   
 - `void push_head(unsigned char x[])`
 
   向头部添加一个无符号字符数据块
   
 - `void push_head(unsigned x)`
 
   向头部添加一个无符号整型
   
##### 注意上面重载函数并不检查值是否与k_type相容，这意味着如果k_type=1，但传入了无符号整型会导致只有前4个字节被复制
 - `void pop_tail()`
 
   删除末尾元素

 - `void pop_head()`
 
   删除头部元素
    
 - `unsigned get_all_value(unsigned char x[][32])`
 
   得到list中所有的值
 
 - `unsigned get_all_value(unsigned x[])`
 
   上面函数的重载，但是类型不同
   
##### 与上面的重载函数相同，并不检查k_type的相容性

 - `void show()`
 
   显示list中所有的值
 
 - `void create()`
 
   给self分配一个新的存储编号
   
 - `void release()`
 
   释放该链表
   
 - `void clear()`
 
   清空链表中的所有节点，但链表仍然存在
    
 - `void save()`
 
   向self指定的数据块中写入链表信息

 - `void load()`
 
   从self指定的数据块中载入链表信息 

#### db_map.h，db_map类
- `db_map(const char _name[],char k_ty,char v_ty)`

   构造函数，指定名称和键类型、值类型
   
- `db_map()`

   空构造函数
   
- `bool exists(unsigned char key[])`

   判断键是否存在于map中
   
- `bool exists(unsigned key)`

   上述函数重载
   
- `void add(unsigned char _key[],unsigned char _value[])`

   向map中增加键值对，注意如果key已经存在，将不会进行此操作。
   
- `void add(unsigned _key,unsigned char _value[])`

   上述函数重载
   
- `void add(unsigned char _key[],unsigned _value)`

   上述函数重载
   
- `void add(unsigned _key,unsigned _value)`

   上述函数重载
   
- `void drop(unsigned char _key[])`

   根据key删除指定的键值对，如果不存在key值将会停止此操作
   
- `void drop(unsigned _key)`

   上述函数重载
   
- `int get_by_key(unsigned char key[],unsigned char value[])`

   根据值键值获取value，如果不存在key值将会返回-1，否则返回0。
   
- `int get_by_key(unsigned char key[],unsigned *value)`

   上述函数重载
   
- `int get_by_key(unsigned key,unsigned char value[])`

   上述函数重载
   
- `int get_by_key(unsigned key,unsigned *value)`

   上述函数重载
   
- `int update(unsigned char key[],unsigned char value[])`

   根据指定的key值，用value对该键值对进行更新。如果key不存在，将会返回-1，否则返回0
   
- `int update(unsigned char key[],unsigned value)`

   上述函数重载
   
- `int update(unsigned key,unsigned char value[])`

   上述函数重载
   
- `int update(unsigned key,unsigned value)`

   上述函数重载
   
- `unsigned get_all_key(unsigned x[])`

   获取map中所有的key值，结果存入到x数组中
   
- `unsigned get_all_key(unsigned char x[][32])`

   上述函数重载

##### 注意：上面所有重载函数都不检查与key和value的类型

#### db_manage.h，db_struct
该类用于管理存储的所有数据结构

- `db_struct(int any_num)`

   参数没有具体含义，但是必须传入一个整数
   
- `db_map create_map(const char name[],char k_ty,char v_ty)`
   
   创建一个map，并且返回该map实例，k_ty代表键的类型，v_ty代表值的类型，'d'代表整型，'s'代表无符号字符块，如果已经存在该名称会导致返回值的map的self指向野指针，因此必须在create之前进行检查。
   
- `db_map get_map(const char name[])`

   根据name获取数据库中的map，如果不存在，返回的map中self是null值，可以根据self.is_null()来判断。
   
- `void delete_map(const char name[])`

   删除数据库中的某个map，如果不存在将停止该操作
   
- `void show_all_map()`

   显示所有map的名称
   
- `db_list create_list(const char name[],char k_ty)`

   创建一个list，并返回该list的一个实例，name表示list的名称，k_ty代表的是list的值类型，'d'代表整型，'s'代表无符号字符块，同样在create之前必须检查是否已经存在该list
   
- `db_list get_list(const char name[])`

   根据name获取一个list的实例，同样可以根据self是否为null来判断是否获取失败。
   
- `void delete_list(const char name[])`
   
   删除指定名称的list

- `void show_all_list()`

   
