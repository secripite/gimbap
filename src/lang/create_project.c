#include "create_project.h"
#include "py_structure_uv.h"
#include "rs_structure_cargo.h"
#include "c_structure.h"
#include "config.h"

void create_project(GimbapConfig *config) {
    switch (config->lang) {
        case LANG_NONE: // No language selected
            // Call the function to set up a default project
            break;
        case LANG_RUST: // Rust
            // Call the function to set up a Rust project
            setup_rust_cargo_project(config);
            break;
        case LANG_GO: // Go
            // Call the function to set up a Go project
            break;
        case LANG_PYTHON: // Python
            // Call the function to set up a Python project
            setup_python_uv_project(config);
            break;
        case LANG_NPM: // NPM
            // Call the function to set up an NPM project
            break;
        case LANG_C: // C
            // Call the function to set up a C project
            setup_c_project(config);
            break;
        default: //empty or unsupported language
            // Handle unsupported language or do nothing
            break;
    }
}
