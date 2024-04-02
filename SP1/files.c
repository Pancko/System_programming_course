// Andreev S.V. 37 group
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int open_file(const char* NAME, const int flags)
{
    int result = open(NAME, flags, S_IRWXO);
    if (result == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    return result;
}

void filecpy(const char* FROM, const char* TO)
{
    int file_from = open_file(FROM, O_CREAT | O_RDONLY);

    size_t message_size = 0;
    char* buffer = malloc(1025);
    ssize_t reading;
    while (reading = read(file_from, buffer + message_size, 1024))
    {
        if (reading == -1)
        {
            if (errno == EINTR)
                continue;
            close(file_from);
            perror("read");
            break;
        }
        message_size += reading;
        buffer = realloc(buffer, 1025 + message_size);
    }
    buffer[message_size] = 0;

    close(file_from);

    int file_to = open_file(TO, O_CREAT | O_RDWR | O_APPEND);

    ssize_t writing = write(file_to, buffer, message_size);

    if (writing == -1)
    {
        close(file_to);
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(file_to);
}

int main()
{
    filecpy("from", "to");
    return 0;
}