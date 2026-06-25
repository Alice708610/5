// Kernel parameters

#define NPROC         64    // Maximum number of processes
#define NCPU          8     // Maximum number of CPUs
#define NOFILE        16    // Open files per process
#define NFILE         100   // Open files in system
#define NBUF          40    // Size of block cache
#define NINODE        50    // Maximum number of active inodes
#define NDEV          10    // Maximum number of devices
#define ROOTDEV       1     // Device number of root file system
#define MAXPATH       128   // Maximum file path length
#define MAXARG        32    // Maximum number of arguments
#define MAXOPBLOCKS   10    // Maximum number of blocks in a single transaction
#define LOGSIZE       10    // Log blocks
#define NBUF          40    // Buffer cache size
#define PIPESIZE      512   // Pipe buffer size
#define FSMAGIC       0x10203040  // File system magic number
