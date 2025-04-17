# 流式文件拷贝和读取项目

## 简介

​	本测试项目旨在模拟文件流式拷贝的过程，在文件尚未完全拷贝完成时，应用程序可以在目标文件夹中打开并流式读取目标文件。通过这一过程，模拟了文件传输过程中目标文件可被即时访问和使用的场景。

目录结构:

stream_test/
|
|— create_file.py        # 创建一个大小为512MB的文件，内容为 "abcd" 循环填充
|— rate_limited_copy.c   # 按照指定的传输速率限制进行文件拷贝
|— stream_reader.c       # 用于流式读取目标文件，每次读取10MB并进行计算

README.md

- **项目核心组件：**
  - **create_file.py**：在 `test_dir_A` 目录下创建一个大小为 512MB、内容为 "abcd" 循环填充的名为 `text.txt` 的文件。
  - **rate_limited_copy.c**：按照指定的传输速率限制对文件进行拷贝。
  - **stream_reader.c**：一个应用程序，用于流式读取目标文件，目标文件可能尚未完全传输。该应用程序每次读取 10MB 数据，并在读取满 10MB 后进行相应的计算。

## 项目环境

Ubuntu 22.04

gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0

python 3.10.12

## 运行方法

1.编译方法

编译rate_limited_copy.c

```shell
gcc -O2 rate_limited_copy.c -o rate_limited_copy
```

编译stream_reader.c

```shell
gcc -O2 stream_reader.c -o stream_reader
```

2.启动流程

​	先创建test_dir_A, test_dir_B目录

```shell
mkdir test_dir_A test_dir_B
```

​	在执行create_file.py在test_dir_A中创建abcd循环的512M的名为text.txt

```shell
python3 create_file.py
```

​	然后在一个终端中执行一下命令, 这会以5MB/秒的速度将文件从`test_dir_A`拷贝到`test_dir_B`

```shell
./rate_limited_copy test_dir_A/text.txt test_dir_B/text.txt 5
```

​	项目根目录开启一个新的终端执行，就可以看到流式读取数据被打印到终端

```shell
./stream_reader test_dir_B/text.txt
```

