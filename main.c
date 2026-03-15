#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define TODO(message) do { fprintf(stderr, "%s:%d -- TODO: %s\n", __FILE__, __LINE__, message); exit(EXIT_FAILURE); } while (0)
#define UNUSED(value) (void)(value)

#define ID_MAX_LENGTH    16
#define BODY_MAX_LENGTH  1024
#define TITLE_MAX_LENGTH 128

#define LARGE_BUFFER 2048

#define debug_print(t) (printf("Sizeof type %s: %zu bytes\n", #t, sizeof(t)))

typedef struct Task {
    char id[ID_MAX_LENGTH];
    int priority;
    char title[TITLE_MAX_LENGTH];
    char body[BODY_MAX_LENGTH];
} Task;

typedef struct TaskList {
    Task *list;
    size_t length;
} TaskList;

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

void generate_task_id(char *buffer) {
    time_t timestamp = time(NULL);
    struct tm *gmtime_info = gmtime(&timestamp);
    strftime(buffer, ID_MAX_LENGTH, "%Y%m%d-%H%M%S", gmtime_info);
}

void initialize_task(Task *task, int priority, const char *title, const char *body) {
    char id[ID_MAX_LENGTH];
    generate_task_id(id);

    strncpy(task->id, id, ID_MAX_LENGTH);
    task->priority = priority;
    strncpy(task->title, title, TITLE_MAX_LENGTH);
    strncpy(task->body, body, BODY_MAX_LENGTH);
}

int portable_mkdir(char *dirname) {
#ifdef _WIN32
    if (!CreateDirectory(dirname, NULL)) {
	fprintf(stderr, "Could not write directory '%s'\n", dirname);
	return 1;
    }
    return 0;
#else
    TODO("Implement portabe_mkdir for Linux\n");
#endif
}

void print_task(Task *task) {
    printf("Title:    %s (#%s)\n", task->title, task->id);
    printf("Priority: %d\n\n", task->priority);
    printf("%s\n", task->body);
}

void print_tasks(TaskList *tasks) {
    for (size_t i = 0; i < tasks->length; i++) {
        print_task(&tasks->list[i]);
    }
}

void walk_directory(char *path) {
#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE h;

    h = FindFirstFile(path, &fd);

    if (h == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Couldn't find files in %s\n", path);
    }

    do {
        printf("File name: %s\n", fd.cFileName);
    } while(FindNextFile(h, &fd) != 0);

    FindClose(h);
#else
    TODO("Implement walk_directory for Unix\n");
#endif
}

void write_task(Task *task) {
    char *template = "# %s\n(#%s)\n\nPRIORITY: %d\nSTATUS: OPEN\n\n%s\n";

    char dirname[LARGE_BUFFER];
    sprintf(dirname, "tasks/%s", task->id);
    portable_mkdir(dirname);

    char filename[LARGE_BUFFER];
    sprintf(filename, "tasks/%s/TASK.md", task->id);

    FILE *f = fopen(filename, "w");
    fprintf(f, template, task->title, task->id, task->priority, task->body);
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Can't call the executable without arguments.\n");
        return EXIT_FAILURE;
    }

    char *command = argv[1];

    if (strcmp(command, "list") == 0) {
        char files[256];
        walk_directory("tasks", files);
        // print_tasks(&task_list);
    } else if (strcmp(command, "create") == 0) {
        if (argc < 3) {
           fprintf(stderr, "Title is mandatory when creating a task.\n");
	   return EXIT_FAILURE;
	}

        if (!directory_exists("tasks")) {
            portable_mkdir("tasks");
        }

	Task task;
        initialize_task(&task, 40, argv[2], "No description.");
        write_task(&task);
    } else {
        fprintf(stderr, "Action '%s' is not supported.\n", command);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
