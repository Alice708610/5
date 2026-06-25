# 系统调用

// 进程控制
int fork(void);                    // 创建子进程
int exit(int status);              // 终止当前进程
int wait(int *status);             // 等待子进程
int exec(const char *path, char **argv);  // 执行新程序
int getpid(void);                  // 获取进程ID

// 文件操作
int open(const char *path, int flags);    // 打开文件
int close(int fd);                        // 关闭文件
int read(int fd, void *buf, int n);       // 读取文件
int write(int fd, const void *buf, int n); // 写入文件
int fstat(int fd, struct stat *st);       // 获取文件状态
int dup(int fd);                          // 复制文件描述符

// 目录操作
int mkdir(const char *path);       // 创建目录
int chdir(const char *path);       // 切换当前目录
int link(const char *old, const char *new); // 创建硬链接
int unlink(const char *path);      // 删除文件

// 设备
int mknod(const char *path, short major, short minor); // 创建设备节点

// 管道
int pipe(int fd[2]);               // 创建管道

// 内存
void *sbrk(int n);                 // 扩展进程堆空间

// 进程间通信
int kill(int pid);                 // 发送信号给进程

// 时间
int sleep(int n);                  // 睡眠 n 个时钟滴答
int uptime(void);                  // 获取系统启动后的时钟滴答数
