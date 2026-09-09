#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "c_structure.h"
#include "config.h"


static int is_gcc_installed(void) {
    int status = system("command -v gcc > /dev/null 2>&1");
    if (status == 0) {
        return 1;   // gcc compiler is ready
    }
    return 0;       // gcc is missing
}

void setup_c_project(GimbapConfig *config) {
    char original_dir[1024];

    if (getcwd(original_dir, sizeof(original_dir)) == NULL) {
        perror("Error: Failed to get current directory");
        return;
    }

    // Pre-check if gcc is installed in the current environment
    if (!is_gcc_installed()) {
        fprintf(stderr, "\n[Error] GCC compiler was not found in your PATH!\n");
        fprintf(stderr, "Please install build-essential or gcc and try again.\n\n");
        return;
    }

    printf("Creating standard C project directory: %s...\n", config->name);

    // Create the root project directory
    if (mkdir(config->name, 0755) != 0) {
        perror("Error: Failed to create project directory");
        return;
    }

    // Move into the newly created project folder
    if (chdir(config->name) != 0) {
        perror("Error: Failed to enter project directory");
        return;
    }

    // Build standard C project directory layouts
    printf("Generating standard C project layout (bin, build, include, src)...\n");
    mkdir("bin", 0755);         // For executable binaries
    mkdir("build", 0755);       // For compiler intermediate object files (.o)
    mkdir("src", 0755);         // For source code files
    mkdir("include", 0755);     // For public headers

    // Create a namespaced subdirectory inside 'include' to prevent conflicts
    char namespaced_include_path[1024];
    snprintf(namespaced_include_path, sizeof(namespaced_include_path), "include/%s", config->name);
    mkdir(namespaced_include_path, 0755);

    // Generate a default boilerplate 'src/main.c'
    printf("Creating starter source code (src/main.c)...\n");
    FILE *main_fp = fopen("src/main.c", "w");
    if (main_fp != NULL) {
        fprintf(main_fp, "// Entry point for %s\n", config->name);
        fprintf(main_fp, "#include <stdio.h>\n\n");
        fprintf(main_fp, "int main(void) {\n");
        fprintf(main_fp, "    printf(\"Hello, World! Welcome to %s!\\n\");\n", config->name);
        fprintf(main_fp, "    return 0;\n");
        fprintf(main_fp, "}\n");
        fclose(main_fp);
    }

    // Generate README.md if configured by Gimbap
    if (config->readme) {
        FILE *readme_fp = fopen("README.md", "w");
        if (readme_fp != NULL) {
            fprintf(readme_fp, "# %s\n\nThis C project was automatically initialized by gimbap.\n", config->name);
            fprintf(readme_fp, "## Build Instructions\nTo compile the project, run:\n```bash\nmake\n```\n");
            fprintf(readme_fp, "To run the executable:\n```bash\n./bin/main\n```\n");
            fclose(readme_fp);
            printf("Generated README.md.\n");
        }
    }

    // Restore the original directory context so subsequent program flows run correctly
    chdir(original_dir);
    printf("\nSuccessfully generated C project structure!\n");
}