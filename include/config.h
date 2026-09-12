#ifndef CONFIG_H
#define CONFIG_H

typedef enum {
    LANG_NONE = 0,
    LANG_RUST,
    LANG_GO,
    LANG_PYTHON,
    LANG_NPM,
    LANG_C
} Lang;

typedef enum {
    LICENSE_NONE = 0,
    LICENSE_MIT,
    LICENSE_APACHE_2_0,
    LICENSE_GPL_V3
} License;

/* 
 * GimbapConfig Structure
 * This struct stores all project initialization settings gathered via 
 * Command Line Flags or the ncurses TUI.
 */
typedef struct {
    // Project Name
    char name[50];   
       
    // Selected Language/Environment
    // LANG_NONE: None, LANG_RUST: Rust, LANG_GO: Go, LANG_PYTHON: Python, LANG_NPM: NPM, LANG_C: C
    Lang lang;        

    // GitHub Remote Connection
    // 0: No (Local only), 1: Yes (Create/Connect Remote)
    int remote;      

    // Repository License
    // LICENSE_NONE: None, LICENSE_MIT: MIT, LICENSE_APACHE_2_0: Apache 2.0, LICENSE_GPL_V3: GPL v3
    License license;    

    // README.md Template Generation
    // 0: No, 1: Yes
    int readme;      
} GimbapConfig;

#endif