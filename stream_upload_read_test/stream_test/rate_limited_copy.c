#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define BUFFER_SIZE (1024 * 1024)  // 1MB 缓冲区

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <source_file> <dest_file> <rate_MB_per_sec>\n", argv[0]);
        return 1;
    }
    
    const char *source_path = argv[1];
    const char *dest_path = argv[2];
    int rate_MB_per_sec = atoi(argv[3]);
    
    // 打开源文件
    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        perror("Error opening source file");
        return 1;
    }
    
    // 获取源文件大小
    struct stat st;
    if (stat(source_path, &st) == -1) {
        perror("Error getting file size");
        close(source_fd);
        return 1;
    }
    off_t file_size = st.st_size;
    
    // 创建目标文件
    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("Error creating destination file");
        close(source_fd);
        return 1;
    }
    
    // 计算每次拷贝的大小和延迟
    int bytes_per_second = rate_MB_per_sec * 1024 * 1024;
    int chunk_size = BUFFER_SIZE < bytes_per_second ? BUFFER_SIZE : bytes_per_second;
    double sleep_time = (double)chunk_size / bytes_per_second;
    
    // 分配缓冲区
    char *buffer = malloc(chunk_size);
    if (!buffer) {
        perror("Error allocating buffer");
        close(source_fd);
        close(dest_fd);
        return 1;
    }
    
    printf("开始拷贝文件, 速率: %d MB/秒\n", rate_MB_per_sec);
    printf("总文件大小: %.2f MB\n", (double)file_size / (1024 * 1024));
    
    // 拷贝文件
    off_t bytes_copied = 0;
    ssize_t bytes_read;
    struct timespec sleep_spec;
    sleep_spec.tv_sec = (time_t)sleep_time;
    sleep_spec.tv_nsec = (long)((sleep_time - (time_t)sleep_time) * 1000000000L);
    
    time_t start_time = time(NULL);
    
    while ((bytes_read = read(source_fd, buffer, chunk_size)) > 0) {
        // 写入数据
        ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing data");
            break;
        }
        
        bytes_copied += bytes_written;
        
        // 显示进度
        printf("已拷贝: %.2f MB / %.2f MB (%.1f%%)\r", 
               (double)bytes_copied / (1024 * 1024), 
               (double)file_size / (1024 * 1024),
               (double)bytes_copied * 100 / file_size);
        fflush(stdout);
        
        // 限制速率
        nanosleep(&sleep_spec, NULL);
    }
    
    time_t end_time = time(NULL);
    double elapsed = difftime(end_time, start_time);
    
    printf("\n拷贝完成! 耗时: %.1f 秒, 平均速率: %.2f MB/秒\n", 
           elapsed, 
           (double)bytes_copied / (1024 * 1024) / elapsed);
    
    // 清理资源
    free(buffer);
    close(source_fd);
    close(dest_fd);
    
    return 0;
}
