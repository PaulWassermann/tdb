#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#define TDB_IMPLEMENTATION
#include "tdb.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Can't call the executable without arguments.\n");
        return EXIT_FAILURE;
    }

    char *command = argv[1];

    if (strcmp(command, "list") == 0) {
        Task task = {0};
        load_task_from_file(&task, "tasks/20260409-073245/TASK.md");
        print_task(&task);
        // char files[256];
        // walk_directory("tasks", files);
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
