#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<getopt.h>

#define true 1
#define block_size_error "Block size error"
#define params_error "Invalid params"
#define reading_error "Error while reading file"
#define writing_error "Error while writing to file"
#define lseek_error "Can't lseek in file"
#define closing_file_error "Can't close file"
#define file_names_error "Input and output files can't be the same"

int main(int argc, char *argv[]) {
    const struct option params[] = {
            { "block",     1,  NULL,   'b'},
            { NULL,   0,        NULL, 0}
    };
    const char* const one_param = "-b";
    char *input = NULL;
    char *output = NULL;
    int param;
    int block_size = 4096;
    do {
        param = getopt_long(argc, argv, one_param, params, NULL);
        if (param == 'b') {
            if (sscanf(optarg, "%d", & block_size) != 1 || block_size <= 0) {
                fprintf(stderr, block_size_error);
                exit(EXIT_FAILURE);
            }
        }
        if (param == 1) {
            if (input == NULL) {
                char *file = (char *) malloc(strlen(optarg) + 1);
                strcpy(file, optarg);
                input = file;
            }
            if (output == NULL) {
                char *file = (char *) malloc(strlen(optarg) + 1);
                strcpy(file, optarg);
                output = file;
            }
            fprintf(stderr, params_error);
            exit(EXIT_FAILURE);
        }
    } while (param != -1);
    if (input == NULL) {
        fprintf(stderr, params_error);
        exit(EXIT_FAILURE);
    }
    if (output == NULL) {
        output = input;
        input = NULL;
    }
    if ((input != NULL) && (output != NULL) && (strcmp(input, output) == 0)) {
        fprintf(stderr, file_names_error);
        exit(EXIT_FAILURE);
    }
    int in_fd = input != NULL ? open(input, O_RDONLY) : 0;
    if (in_fd == -1) {
        fprintf(stderr, "Can't open '%s'\n", input);
        exit(EXIT_FAILURE);
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int out_fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (out_fd == -1) {
        fprintf(stderr, "Can't open '%s'\n", output);
        exit(EXIT_FAILURE);
    }
    char *buffer = (char *) malloc(block_size);
    int read_blocks = 0;
    ssize_t count;
    while (true) {
        count = read(in_fd, buffer, block_size);
        if (count == 0) {
            break;
        }
        if (count == -1) {
            fprintf(stderr, reading_error);
            exit(EXIT_FAILURE);
        }
        read_blocks++;
        int flag = 0;
        char *buffer2 = buffer;
        for (int i = count; i >= 0; --i) {
            if (*(buffer2++)) {
                flag = 1;
                break;
            }
        }
        if (flag) {
            if (lseek(out_fd, block_size * (read_blocks - 1), SEEK_SET) == -1) {
                fprintf(stderr, lseek_error);
                exit(EXIT_FAILURE);
            }
            if (write(out_fd, buffer, count) != count) {
                fprintf(stderr, writing_error);
                exit(EXIT_FAILURE);
            }
        }
    }
    if (input != NULL) {
        if (close(in_fd) == -1) {
            fprintf(stderr, closing_file_error);
            exit(EXIT_FAILURE);
        }
        free(input);
    }
    if (close(out_fd) == -1) {
        fprintf(stderr, closing_file_error);
        exit(EXIT_FAILURE);
    }
    if (output != NULL) {
        free(output);
    }
    return 0;
}
