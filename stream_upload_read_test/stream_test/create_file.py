#!/usr/bin/env python3

import os
import time

# 确保目标目录存在
os.makedirs("test_dir_A", exist_ok=True)

# 定义文件路径
filepath = "test_dir_A/text.txt"

# 定义数据块大小和模式
pattern = b"lya1"
chunk_size_mb = 4  #
chunks_count = 128  #

print(f"开始生成文件: {filepath}")
print(f"目标大小: 约 {chunks_count * chunk_size_mb} MB")

start_time = time.time()

# 打开文件并写入数据
with open(filepath, "wb") as f:
    # 创建一个内存高效的chunk (约7MB)
    chunk = pattern * (chunk_size_mb * 1024 * 1024 // len(pattern))
    
    # 写入所需数量的chunk
    for i in range(chunks_count):
        f.write(chunk)
        
        # 每写入10个chunk显示一次进度
        if i % 10 == 0 or i == chunks_count - 1:
            mb_written = (i + 1) * chunk_size_mb
            percent = (i + 1) * 100 / chunks_count
            elapsed = time.time() - start_time
            speed = mb_written / elapsed if elapsed > 0 else 0
            
            print(f"已写入: {mb_written} MB / {chunks_count * chunk_size_mb} MB ({percent:.1f}%) - 速度: {speed:.2f} MB/s")

end_time = time.time()
file_size = os.path.getsize(filepath) / (1024 * 1024)  # 实际大小(MB)
elapsed = end_time - start_time

print(f"\n文件生成完成!")
print(f"文件路径: {os.path.abspath(filepath)}")
print(f"实际大小: {file_size:.2f} MB")
print(f"耗时: {elapsed:.2f} 秒")
print(f"平均速度: {file_size / elapsed:.2f} MB/s")
