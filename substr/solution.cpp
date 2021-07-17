#include <cstring>
#include <cerrno>
#include <malloc.h>

enum class Result {
    FOUND,
    NOT_FOUND,
    ERROR,
};

Result find_pattern(char* pattern, FILE* input) {
    size_t len = strlen(pattern);
    auto pr = (size_t*)malloc(len * sizeof(size_t));
    if (pr == nullptr) {
        return Result::ERROR;
    }
    size_t i = 1;
    size_t prevLen = 0;
    pr[0] = 0;
    while (i < len || !feof(input)) {
        int ch = i < len ? pattern[i] : fgetc(input);
        if (ferror(input)) {
            free(pr);
            return Result::ERROR;
        }
        while (prevLen > 0 && ch != pattern[prevLen]) {
            prevLen = pr[prevLen - 1];
        }
        if (ch == pattern[prevLen]) {
            prevLen++;
        }
        if (i < len) {
            pr[i] = prevLen;
        }
        if (prevLen == len) {
            free(pr);
            return Result::FOUND;
        }
        i++;
    }
    free(pr);
    return Result::NOT_FOUND;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errno = EINVAL;
        perror("Usage: substr FNAME QUERY");
        return -1;
    }
    FILE* input = fopen(argv[1], "rb");
    if (input == nullptr) {
        perror("File error");
        return -1;
    }
    char* pattern = argv[2];
    Result ans = find_pattern(pattern, input);
    fclose(input);
    if (ans == Result::FOUND) {
        fprintf(stdout, "%s\n", "Yes");
        return 0;
    }
    if (ans == Result::ERROR) {
        perror("Error");
        return -1;
    }
    fprintf(stdout, "%s\n", "No");
    return 0;
}
