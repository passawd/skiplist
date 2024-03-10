# skiplist


skiplist.h 跳表实现
store/dumpFile 数据存放本地磁盘文件
main.cpp 使用skiplist.h进行数据操作
stress_test.cpp 压力测试


insert_element（插入）
delete_element（删除）
search_element（查询）
display_list（展示）
dump_file（数据保存）
load_file（数据加载）
size（返回数据规模）


# 压力测试

4G2核linux虚拟机测试结果：

skiplist size()=632229

get elapsed:2.15024

读取100w条数据耗时2.15024s

跳表树高：20
采用随机插入数据测试：

skiplist size()=63177

insert elapsed:0.0737531

插入10w条数据耗时0.0737531s，实际插入数据63177条




 
