#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 10 * 1024 * 1024  // 10 MB

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_to_read>\n", argv[0]);
        return 1;
    }
    
    const char *file_path = argv[1];
    
    // 打开文件
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    
    // 分配缓冲区
    char *buffer = malloc(BUFFER_SIZE + 1);  // +1 用于添加结束符以便打印
    if (!buffer) {
        perror("Error allocating buffer");
        close(fd);
        return 1;
    }
    
    printf("开始流式读取文件: %s\n", file_path);
    printf("每次读取: %d MB\n", BUFFER_SIZE / (1024 * 1024));
    
    // 读取文件
    off_t total_bytes_read = 0;
    ssize_t bytes_read = 0;
    time_t start_time = time(NULL);
    int read_count = 0;
    int total_size = 512 * 1024 * 1024;  // 假设总共要读取500MB
    while (1) {
        // 读取数据
        long curr_read_size = 0;
        while (curr_read_size < BUFFER_SIZE && total_size > 0) {
            bytes_read = read(fd, buffer + curr_read_size, BUFFER_SIZE - curr_read_size);
            curr_read_size += bytes_read;
            total_size -= bytes_read;
        }
        
        if (curr_read_size > 0) {
            // 处理读取到的数据
            total_bytes_read += curr_read_size;
            read_count++;
            
            // 设置结束符并打印少量数据作为示例
            buffer[curr_read_size < 1000 ? curr_read_size : 1000] = '\0';
            
            printf("\n--- 读取 #%d ---\n", read_count);
            printf("读取了 %zd 字节, 总计: %.2f MB\n", 
                   curr_read_size, 
                   (double)total_bytes_read / (1024 * 1024));
            printf("数据样本 (前100字节): %s\n", buffer);
            
            // 检查是否含有预期的模式
            int valid_pattern = 1;
            for (int i = 0; i < curr_read_size && i < 1000 && valid_pattern; i++) {
                if (buffer[i] != "abcd"[i % 4]) {
                    valid_pattern = 0;
                    printf("警告: 在位置 %d 处发现非预期数据\n", i);
                }
            }
            if (valid_pattern) {
                printf("数据验证: 通过 (前100字节符合预期模式)\n");
            }
        }
        
        // 检查是否已经读取完所有数据
        if (total_size <= 0) {
            break;
        }
    }
    
    time_t end_time = time(NULL);
    double elapsed = difftime(end_time, start_time);
    
    printf("\n读取完成! 总计读取: %.2f MB, 耗时: %.1f 秒, 平均速率: %.2f MB/秒\n", 
           (double)total_bytes_read / (1024 * 1024), 
           elapsed, 
           (double)total_bytes_read / (1024 * 1024) / elapsed);
    
    // 清理资源
    free(buffer);
    close(fd);
    
    return 0;
}

