# A programming for copying text files.
## Step to run the program
* `gcc fileCopy.c -o fileCopy`.
* `./fileCopy`.
* Enter input file and output file.
## Step to trace system call
* `strace -o system_call ./fileCopy`.
* Enter input file and output file.
## Some system calls in system_call
* Line 1 `int execve(const char *pathname, char* const argv[], char* const envp[])` \\
Executes the program pointed to by the filename.
***
* Line 2 `int brk(void* addr)`\\
Change data segment size, maybe allocates memory.
***
* Line 4 `void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offste)`\\
Map or unmap file or devices into memory.
***
* Line 5 `int access(const char *pathname, int mode)`\\
Check a user's permissions for a file.
***
* Line 6 `int openat(int dirfd, const char *pathname, int flags)`\\
Open and possibly create a file.
***
* Line 9 `int close(int fd)`\\
Close a file description so that it no longer refers to any file.
***
* Line 11 `ssize_t read(int fd, void *buf, size_t count)`\\
Read up to count bytes from file description fd into buf.
***
* Line 12 `pread(int fd, void *buf, size_t count, off_t offset)`\\
Read up to count byte from file description fd at offset into buf.
***
* Line 28 `int mprotect(void *addr, size_t len, int prot)`\\
Change the access protection in the interval [addr, addr + len - 1].
***
* Line 38 `ssize_t write(int fd, void *buf, size_t count)`\\
Write up to count bytes from file description fd into buf.
***
* Line 51 `off_t lseek(int fd, off_t offset, int whence)`\\
Reposition the file offset of the file description fd.
***
* Line 52 `void exit_group(int status)`\\
Exit all threads in a process.
