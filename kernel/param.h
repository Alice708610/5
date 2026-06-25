// param.h - System parameters and constants for ZephyrOS

#define NCPU          3       // Number of CPU cores
#define NPROC        64       // Maximum number of processes
#define NFILE        128      // Open files per process
#define NINODE       50       // Maximum number of active inodes
#define NDEV         10       // Maximum number of devices
#define ROOTDEV       1       // Device number of the root filesystem
#define MAXARG       128      // Max exec arguments
#define MAXOPBLOCKS   10      // Max number of blocks in a write operation
#define LOGSIZE      30       // Size of log for file system transactions
#define NBUF         100      // Buffer cache size
#define FSSIZE       2000     // Size of file system in blocks
#define MAXPATH     256       // Max path length
