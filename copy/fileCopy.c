#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    char srcFile[256], dstFile[256];
    int srcFd, dstFd, n;
    char buf[4096];

    printf("Enter source file: ");
    scanf("%s", srcFile);
    printf("Enter destination file: ");
    scanf("%s", dstFile);

    if (access(srcFile, F_OK) == -1) {
        perror("Error: Source file does not exit!");
        exit(EXIT_FAILURE);
    }

    srcFd = open(srcFile, O_RDONLY);
    if (srcFd == -1) {
        perror("Error: Can not open source file");
        exit(EXIT_FAILURE);
    }

    dstFd = open(dstFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dstFd == -1) {
        perror("Error: Can not open destination file");
        exit(EXIT_FAILURE);
    }

    while ((n = read(srcFd, buf, sizeof(buf) - 1)) > 0) {
        if (write(dstFd, buf, n) != n) {
            perror("Error: Can not write to destination file");
            exit(EXIT_FAILURE);
        }
    }

    if (n == -1) {
        perror("Error: Can not read from source file");
        exit(EXIT_FAILURE);
    }

    if (close(srcFd) == -1) {
        perror("Error: Can not close source file");
        exit(EXIT_FAILURE);
    }

    if (close(dstFd) == -1) {
        perror("Error: Can not close destination file");
        exit(EXIT_FAILURE);
    }

    printf("File copied successfully\n");
    return 0;
}