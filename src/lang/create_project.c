#include "create_project.h"
#include "py_structure_uv.h"
#include "rs_structure_cargo.h"
#include "config.h"

void create_project(GimbapConfig *config) {
    switch (config->lang) {
        case 1: // Rust
            // Call the function to set up a Rust project
            setup_rust_cargo_project(config);
            break;
        case 2: // Go
            // Call the function to set up a Go project
            break;
        case 3: // Python
            // Call the function to set up a Python project
            setup_python_uv_project(config);
            break;
        case 4: // NPM
            // Call the function to set up an NPM project
            break;
        case 5: // C
            // Call the function to set up a C project
            setup_c_project(config);
            break;
        default: //empty or unsupported language
            // Handle unsupported language or do nothing
            break;
    }
}
