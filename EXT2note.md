# ext2 study

## block
>文件系统中存储的最小单位是block<br>
> 一个块究竟有多是在格式化的时候确定的<br>
> 
## bootblock
>启动块<br>
> 大小1kB,由pc标准来规定，
> 用来存储磁盘分区和启动信息，
> 任何文件系统都不能使用该块

## superblock(超级块)
>描述整个分区的文件系统信息，例如块大小
> 文件系统版本号，上次mount的时间等
> superblock在每个块组的开头都有一份拷贝
> 


## 块组描述符表(GDT Group Descriptor Table)

>由很多块组描述符组成，整个分区分成多少个块组就对应有多少个块组描述符
>每个块组描述符存储一个块组的描述信息，包括iNode表从哪里开始，数据块
>从哪里开始，空闲的inode和数据块还有多少个等。
>块组描述符在每个块组的开头也都有拷贝，这些信息是非常重要的，因此他们有多分拷贝

## Block Bitmap(块位图)
>块位图是用来描述整个块组中哪些块已用哪些块空闲的
> ，本身占一个块，其中的每个bit代表本块组中的一个块，
> 这个bit为1表示该块已用，bit为0表示该块空闲可用

## inode Bitmap (Inode位图)
>和位图块类似，本身占一个块，
> 其中每个bit表示一个inode是否空闲可用

## inode table(inode 表)
> ##### 可以当成一个信息表<br>
> 文件类型（常规，目录，符号链接等），
> 权限，文件大小，创建/修改/访问时间等信息
> 存在inode中，每一个问加你都有一个inode

## 数据块
>
>> * 常规文件: 文件的数据存储在数据块中

>> * 目录：该目录下所有文件名和，目录名都存储在数据块中
>> （注意：文件名保存在它所在的目录的数据块中，其他信息都保存在该文件的inode中）

>> * 符号链接 如果目标路径名较短则直接保存在inode中以方便查找
>> 否则分配一个数据块来保存

>> * 设备文件 FIFO和socket等特殊文件：
>> 没有数据块，设备文件的主设备号和次设备号保存在inode中


```
sdfsdf
4235343
4323234
```

