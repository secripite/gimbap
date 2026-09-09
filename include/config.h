#ifndef CONFIG_H
#define CONFIG_H

/* 
 * GimbapConfig Structure
 * This struct stores all project initialization settings gathered via 
 * Command Line Flags or the ncurses TUI.
 */
typedef struct {
    // Project Name
    char name[50];      
    // Selected Language/Environment
    // 0: None, 1: Rust, 2: Go, 3: Python, 4: NPM 5: C
    int lang;        

    // GitHub Remote Connection
    // 0: No (Local only), 1: Yes (Create/Connect Remote)
    int remote;      

    // Repository License
    // 0: None, 1: MIT, 2: Apache 2.0, 3: GPL v3
    int license;    

    // README.md Template Generation
    // 0: No, 1: Yes
    int readme;      
} GimbapConfig;

#endif