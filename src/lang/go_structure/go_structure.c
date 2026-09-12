#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "go_structure.h"

// check if Go is installed in the system
static int is_go_installed(void) {
    int status = system("command -v go > /dev/null 2>&1");
    return (status == 0) ? 1 : 0;
}

void setup_go_project(GimbapConfig *config) {
    char original_dir[1024];

    // Get current working directory to restore it later
    if (getcwd(original_dir, sizeof(original_dir)) == NULL) {
        perror("Error: Failed to get current directory");
        return;
    }

    // Check if Go is installed in the system
    if (!is_go_installed()) {
        fprintf(stderr, "\n[Error] Go toolchain was not found in your system PATH!\n");
        fprintf(stderr, "Please install Go and try again.\n\n");
        return;
    }

    printf("Creating Go project directory based on golang-standards: %s...\n", config->name);

    // Create and enter the project directory
    if (mkdir(config->name, 0755) != 0) {
        perror("Error: Failed to create project directory");
        return;
    }
    if (chdir(config->name) != 0) {
        perror("Error: Failed to enter project directory");
        return;
    }

    // Initialize Go module (go.mod)
    char init_cmd[1024];
    snprintf(init_cmd, sizeof(init_cmd), "go mod init %s", config->name);
    printf("Initializing Go module: '%s'...\n", init_cmd);

    if (system(init_cmd) != 0) {
        fprintf(stderr, "Error: 'go mod init' failed. Rolling back...\n");
        chdir(original_dir);
        return;
    }

    // Create standard layout directories (cmd, internal, pkg, vendor)
    printf("Generating standard layout subdirectories (cmd, internal, pkg, vendor)...\n");
    mkdir("cmd", 0755);         // Main applications folder
    mkdir("internal", 0755);    // Private application/library code
    mkdir("pkg", 0755);         // Public library code
    mkdir("vendor", 0755);      // External dependencies (if vendored)

    // Create executable directory inside /cmd
    char cmd_app_path[1024];
    snprintf(cmd_app_path, sizeof(cmd_app_path), "cmd/%s", config->name);
    mkdir(cmd_app_path, 0755);

    // Generate main.go inside /cmd/<project_name>/main.go
    char main_go_path[1024];
    snprintf(main_go_path, sizeof(main_go_path), "cmd/%s/main.go", config->name);
    printf("Creating main entry point (%s)...\n", main_go_path);

    FILE *fp = fopen(main_go_path, "w");
    if (fp != NULL) {
        fprintf(fp, "package main\n\n");
        fprintf(fp, "import \"fmt\"\n\n");
        fprintf(fp, "func main() {\n");
        fprintf(fp, "    fmt.Println(\"Hello, World! Welcome to %s!\")\n", config->name);
        fprintf(fp, "}\n");
        fclose(fp);
    } else {
        perror("Warning: Failed to create main.go");
    }

    // Generate README.md if configured by Gimbap
    if (config->readme) {
        FILE *readme_fp = fopen("README.md", "w");
        if (readme_fp != NULL) {
            fprintf(readme_fp, "# %s\n\nThis Go project was initialized by gimbap following golang-standards/project-layout.\n\n", config->name);
            fprintf(readme_fp, "## Project Layout\n");
            fprintf(readme_fp, "- `cmd/%s/`: Main executable entry point\n", config->name);
            fprintf(readme_fp, "- `internal/`: Private library code\n");
            fprintf(readme_fp, "- `pkg/`: Public library code\n\n");
            fprintf(readme_fp, "## How to Run\n```bash\ngo run cmd/%s/main.go\n```\n", config->name);
            fclose(readme_fp);
            printf("Generated README.md in package root.\n");
        } else {
            perror("Warning: Failed to write README.md");
        }
    }

    // Restore the original directory context so subsequent program flows run correctly
    chdir(original_dir);
    printf("\nSuccessfully generated Go project structure matching golang-standards/project-layout!\n");
}