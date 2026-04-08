#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define LOG_FORMAT "[%s][%s][%s] Line %d :: "
#define LOG_ARGS(TAG) TAG, __FILE__, __FUNCTION__, __LINE__
#define LOG_INFO(format, ...) \
    fprintf(stdout, LOG_FORMAT format, LOG_ARGS("INFO"), __VA_ARGS__)
#define LOG_WARNING(format, ...) \
    fprintf(stderr, LOG_FORMAT format, LOG_ARGS("WARNING"), __VA_ARGS__)
#define LOG_ERROR(format, ...) \
    fprintf(stderr, LOG_FORMAT format, LOG_ARGS("ERROR"), __VA_ARGS__)

#define DEBUG_TYPE(t) (printf("Sizeof type %s: %zu bytes\n", #t, sizeof(t)))
#define MIN(a, b) (a) < (b) ? (a) : (b)
#define SAFE_MALLOC(size) safe_malloc(size, __FILE__, __FUNCTION__, __LINE__)
#define TODO(message) do {            \
    LOG_ERROR("TODO: %s\n", message); \
    exit(EXIT_FAILURE);               \
} while (0)
#define UNUSED(value) (void)(value)

#define LARGE_BUFFER 2048

// FUNCTIONS PROTOTYPES
typedef struct StringView StringView;
StringView sv(const char *str);
StringView sv_chop_left(StringView *sv, size_t n);
StringView sv_chop_right(StringView *sv, size_t n);
StringView sv_copy(StringView *sv);
bool sv_endswith(StringView *sv, const char *suffix);
bool sv_remove_prefix(StringView *sv, const char *prefix);
bool sv_remove_suffix(StringView *sv, const char *suffix);
bool sv_startswith(StringView *sv, const char *prefix);
char *sv_to_cstr(StringView sv);
StringView sv_trim(StringView sv);
StringView sv_trim_left(StringView sv);
StringView sv_trim_right(StringView sv);

typedef enum Status Status;
Status str_to_status(char *status);

typedef struct Task Task;
typedef struct TaskList TaskList;
void generate_task_id(char *buffer);
void initialize_task(Task *task, int priority, const char *title, const char *body);
bool load_task_from_file(Task *task, char *filename);
void print_task(Task *task);
void print_tasks(TaskList *tasks);
void write_task(Task *task);

bool directory_exists(char *path);
int portable_mkdir(char *dirname);
void walk_directory(char *path);
char read_space(FILE *stream);
void *safe_malloc(size_t size, char *file, char *function, int line);

// STRING VIEW RELATED CODE
#define SV_FMT "%.*s"
#define SV_ARGS(sv) (int) (sv).count, (sv).data

typedef struct StringView {
    const char *data;
    size_t count;
} StringView;

StringView sv(const char *str) {
    return (StringView) {
        .data = str,
        .count = strlen(str)
    };
}

StringView sv_chop_left(StringView *sv, size_t n) {
    n = MIN(sv->count, n);

    sv->data += n;
    sv->count -= n;

    return sv_copy(sv);
}

StringView sv_chop_right(StringView *sv, size_t n) {
    n = MIN(sv->count, n);

    sv->count -= n;

    return sv_copy(sv);
}

StringView sv_copy(StringView *sv) {
    return (StringView) {
        .data = sv->data,
        .count = sv->count
    };
}

bool sv_endswith(StringView *sv, const char *suffix) {
    if (strlen(suffix) > sv->count)  return false;

    for (size_t i = 0; i < strlen(suffix); i++) {
        if (sv->data[sv->count - i - 1] != suffix[strlen(suffix) - i - 1])  return false;
    }
    return true;
}

bool sv_remove_prefix(StringView *sv, const char *prefix) {
    if (sv_startswith(sv, prefix)) {
        sv_chop_left(sv, strlen(prefix));
        return true;
    }
    return false;
}

bool sv_remove_suffix(StringView *sv, const char *suffix) {
    if (sv_endswith(sv, suffix)) {
        sv_chop_right(sv, strlen(suffix));
        return true;
    }
    return false;
}

bool sv_startswith(StringView *sv, const char *prefix) {
    if (strlen(prefix) > sv->count)  return false;

    for (size_t i = 0; i < strlen(prefix); i++) {
        if (sv->data[i] != prefix[i])  return false;
    }
    return true;
}

// WORK IN PROGRESS HERE
char *sv_to_cstr(StringView sv) {
    char *out = SAFE_MALLOC(sizeof(*out) * (sv.count + 1));
    strncpy(out, sv.data, sv.count);
    out[sv.count] = '\0';
    return out;
}

StringView sv_trim(StringView sv) {
    return sv_trim_left(sv_trim_right(sv));
}

StringView sv_trim_left(StringView sv) {
    StringView sv_out = sv_copy(&sv);
    while (sv_out.count > 0 && isspace(sv_out.data[0])) {
        sv_out.data++;
        sv_out.count--;
    }
    return sv_out;
}

StringView sv_trim_right(StringView sv) {
    StringView sv_out = sv_copy(&sv);
    while (sv_out.count > 0 && isspace(sv_out.data[sv_out.count - 1])) {
        sv_out.count--;
    }
    return sv_out;
}

// STATUS RELATED CODE
typedef enum Status {
    OPEN   = 0,
    CLOSED = 1
} Status;

Status str_to_status(char *status) {
    if (strcmp(status, "OPEN") == 0) return OPEN;
    if (strcmp(status, "CLOSED") == 0) return CLOSED;
}

// TASK RELATED CODE
#define ID_MAX_LENGTH     16
#define TITLE_MAX_LENGTH  128
#define STATUS_MAX_LENGTH 8
#define BODY_MAX_LENGTH   1024

char *TEMPLATE = "# %s\n(%s)\n\nPRIORITY: %d\nSTATUS: OPEN\n\n%s\n";

typedef struct Task {
    char id[ID_MAX_LENGTH];
    int priority;
    char title[TITLE_MAX_LENGTH];
    char status[STATUS_MAX_LENGTH];
    char body[BODY_MAX_LENGTH];
} Task;

typedef struct TaskList {
    Task *list;
    size_t length;
} TaskList;

void generate_task_id(char *buffer) {
    time_t timestamp = time(NULL);
    struct tm *gmtime_info = gmtime(&timestamp);
    strftime(buffer, ID_MAX_LENGTH, "%Y%m%d-%H%M%S", gmtime_info);
}

void initialize_task(
        Task *task, int priority, const char *title, const char *body
) {
    char id[ID_MAX_LENGTH];
    generate_task_id(id);

    strncpy(task->id, id, ID_MAX_LENGTH);
    task->priority = priority;
    strncpy(task->title, title, TITLE_MAX_LENGTH);
    strncpy(task->body, body, BODY_MAX_LENGTH);
}

bool load_task_from_file(Task *task, char *filename) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        LOG_ERROR("Cannot open file '%s'", filename);
        exit(EXIT_FAILURE);
    }

    char buffer[LARGE_BUFFER] = {0};

    // Parse title
    fgets(buffer, LARGE_BUFFER, f);
    StringView title_sv = sv_trim(sv(buffer));

    if (!sv_remove_prefix(&title_sv, "# ")) {
        LOG_ERROR("Couldn't load title from '%s'\n", filename);
        return false;
    }

    if (title_sv.count > TITLE_MAX_LENGTH) {
        LOG_WARNING(
            "Truncating title for file '%s' (max length: %d; received: %zu)\n",
            filename, TITLE_MAX_LENGTH, title_sv.count
        );
    }

    strncpy(task->title, title_sv.data, title_sv.count);

    // Parse id
    fgets(buffer, LARGE_BUFFER, f);
    StringView id_sv = sv_trim(sv(buffer));

    if (!sv_remove_prefix(&id_sv, "(") || !sv_remove_suffix(&id_sv, ")")) {
        LOG_ERROR("Couldn't load id from '%s'\n", filename);
        return false;
    }

    if (title_sv.count > ID_MAX_LENGTH) {
        LOG_WARNING(
            "Truncating id for file '%s' (max length: %d; received: %zu)\n",
            filename, ID_MAX_LENGTH, title_sv.count
        );
    }

    strncpy(task->id, id_sv.data, id_sv.count);

    // Parse priority
    read_space(f);
    fgets(buffer, LARGE_BUFFER, f);
    StringView priority_sv = sv_trim(sv(buffer));

    if (!sv_remove_prefix(&priority_sv, "PRIORITY:")) {
        LOG_ERROR("Couldn't load priority from '%s'\n", filename);
        return false;
    }

    char *priority_cstr = sv_to_cstr(priority_sv);
    int priority = atoi(priority_cstr);
    task->priority = priority;

    // Parse status
    // Parse body
    print_task(task);

    TODO("need to finish implementing task loading logic");

    fclose(f);
    return true;
}

void print_task(Task *task) {
    printf("Title:    %s\n(%s)\n", task->title, task->id);
    printf("Priority: %d\n\n", task->priority);
    printf("%s\n", task->body);
}

void print_tasks(TaskList *tasks) {
    for (size_t i = 0; i < tasks->length; i++) {
        print_task(&tasks->list[i]);
    }
}

void write_task(Task *task) {
    char dirname[LARGE_BUFFER];
    sprintf(dirname, "tasks/%s", task->id);
    portable_mkdir(dirname);

    char filename[LARGE_BUFFER];
    sprintf(filename, "tasks/%s/TASK.md", task->id);

    FILE *f = fopen(filename, "w");
    fprintf(f, TEMPLATE, task->title, task->id, task->priority, task->body);
    fclose(f);
}

// FILESYSTEM RELATED CODE
bool directory_exists(char *path) {
#ifdef _WIN32
    DWORD attr = GetFileAttributes(path);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    } else {
        return false;
    }
#else
    TODO("Implement directory_exists for Linux\n");
#endif
}

int portable_mkdir(char *dirname) {
#ifdef _WIN32
    if (!CreateDirectory(dirname, NULL)) {
        LOG_ERROR("Could not write directory '%s'\n", dirname);
        return 1;
    }
    return 0;
#else
    TODO("Implement portabe_mkdir for Linux\n");
#endif
}

void walk_directory(char *path) {
#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE h;

    h = FindFirstFile(path, &fd);

    if (h == INVALID_HANDLE_VALUE) {
        LOG_ERROR("Couldn't find files in %s\n", path);
    }

    do {
        printf("File name: %s\n", fd.cFileName);
    } while(FindNextFile(h, &fd) != 0);

    FindClose(h);
#else
    TODO("Implement walk_directory for Unix\n");
#endif
}

// UTILS
char read_space(FILE *stream) {
    char c;

    while (isspace((c = fgetc(stream)))) {};

    // Move cursor one character back to allow caller to process all 
    // non-whitespace characters
    if (fseek(stream, -1, SEEK_CUR) != 0) {
        LOG_ERROR("Could not deplace cursor in current stream\n");
    }

    return c;
}

void *safe_malloc(size_t size, char *file, char *function, int line) {
    void *ptr = malloc(size);

    if (ptr == NULL) {
        LOG_ERROR(
            "Could not allocate %zu bytes (File: %s, line: %d, function: %s)\n",
            size, file, line, function);
        exit(EXIT_FAILURE);
    }

    return ptr;
}


