#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <ctype.h>
#include "config.h"
#include "tui.h"

#define Enter 10

const char* lang_name[] = {"empty", "rust", "go", "python", "npm"};
const char* license_name[] = {"None", "MIT License", "Apache License 2.0", "GPL v3"};

int width = 60;


void draw_logo(WINDOW *win);
void get_name(WINDOW *win, GimbapConfig *config);
void select_lang(WINDOW *win, GimbapConfig *config);
void set_options(WINDOW *win, GimbapConfig *config);
void set_license(WINDOW *win, GimbapConfig *config);
void set_readme(WINDOW *win, GimbapConfig *config);
void print_summary(GimbapConfig *config);



void tui_run(GimbapConfig *config) {
    setlocale(LC_ALL, ""); 

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int start_y = (LINES - 15) / 2;
    int start_x = (COLS - 60) / 2;
    WINDOW *win = newwin(15, 60, start_y, start_x);
    keypad(win, TRUE);

    draw_logo(win);
    get_name(win, config);
    select_lang(win, config);
    set_options(win, config);
    set_license(win, config);
    set_readme(win, config);

    endwin();

    print_summary(config);
}

void draw_logo(WINDOW *win){
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 2, 11, "████   ██   █   █  ████▄   ▄██▄   ████▄");
    mvwprintw(win, 3, 11, "█▄      ██   █▀▄▀█  █   █  █▄▄▄█  █   █");
    mvwprintw(win, 4, 11, "█▀ ██   ██   █ █ █  ████▀  █▀▀▀█  ████▀");
    mvwprintw(win, 5, 11, "█▄▄▄█  ▄██▄  █   █  █▄▄▄█  █   █  █");
    mvwprintw(win, 7, 15, "- INTEGRATED PROJECT MANAGER -");
    mvwprintw(win, 8, 18, "v0.0.1 (Debian Package)");
    mvwhline(win, 10, 1, ACS_HLINE, width - 2);
    mvwprintw(win, 12, 16, "[ Press any key to start ]");
    wrefresh(win);
    wgetch(win);
}

// check if the project name is valid (no special characters, no spaces, etc.)
int is_valid_project_name(const char *name) {
    // Check if the name pointer is NULL or pointing to an empty string
    if (name == NULL || strlen(name) == 0) {
        return 1; // Valid: empty name
    }

    // Loop through each character of the string to find invalid symbols
    for (int i = 0; name[i] != '\0'; i++) {
        char c = name[i];

        // Allow only alphanumeric characters (a-z, A-Z, 0-9), underscores (_), and hyphens (-)
        // Reject spaces, exclamation marks (!), and other special symbols
        if (!isalnum(c) && c != '_' && c != '-') {
            return 2; // Invalid: forbidden character found
        }
    }

    return 0; // Valid: safe project name
}

// Get the name of the current directory
const char* get_current_directory_name(char *path_buf, size_t buf_size) {
    if (getcwd(path_buf, buf_size) == NULL) {
        perror("Error: getcwd failed");
        return NULL;
    }

    // Find the last occurrence of the directory separator '/'
    char *last_slash = strrchr(path_buf, '/');
    
    if (last_slash != NULL) {
        // If '/' is found, return the pointer pointing to the next character (the folder name)
        return last_slash + 1;
    }
    
    // Fallback: If no '/' is found, return the full path buffer
    return path_buf;
}

// step1: enter the project name
void get_name(WINDOW *win, GimbapConfig *config) {
    if (strlen(config->name) > 0) return; // if name is already set, skip input


    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 20, " [ Project Name ] ");
    mvwprintw(win, 3, 4, "enter the project name:");
    mvwprintw(win, 5, 4, "Name: [ %-30s ]", "");
    mvwprintw(win, 8, 4, "(if not entered, current directory name will be used)");
    mvwprintw(win, 4, 4, "(only '_' and '-' are allowed, and no spaces)");
    mvwhline(win, 11, 1, ACS_HLINE, width - 2);
    mvwprintw(win, 12, 2, "[Enter] Next");
    wmove(win, 5, 12);
    echo();
    
    while (1)
    {
        char temp_name[50];
        wgetnstr(win, temp_name, 30);
        switch (is_valid_project_name(temp_name))
        {
        case 0:
            strncpy(config->name, temp_name, sizeof(config->name) - 1);
            config->name[sizeof(config->name) - 1] = '\0';
            return;
        case 1:
            strncpy(config->name, get_current_directory_name(temp_name, sizeof(temp_name)), sizeof(config->name) - 1);
            config->name[sizeof(config->name) - 1] = '\0';
            return;
        case 2:
            mvwprintw(win, 6, 4, "Invalid project name.");
            mvwprintw(win, 5, 4, "Name: [ %-30s ]", "");
            wmove(win, 5, 12);
            wrefresh(win);
            break;
        }
    }        
    
    noecho();
}

// step2: select the programming language
void select_lang(WINDOW *win, GimbapConfig *config) {
    if (config->lang != 0) return; // if language is already selected, skip input

    int choice = 0;
    int n_langs = sizeof(lang_name) / sizeof(char *);
    int ch;

    keypad(win, TRUE);

    while (1) {
        wclear(win);
        box(win, 0, 0);

        mvwprintw(win, 0, (width - 21) / 2, " [ Select Language ] ");
        mvwprintw(win, 2, 4, "Select the programming language/environment:");

        for (int i = 0; i < n_langs; i++) {
            if (i == choice) {
                wattron(win, A_REVERSE);
                mvwprintw(win, 4 + i, 4, " > [*] %-10s ", lang_name[i]);
                wattroff(win, A_REVERSE);
            } else {
                mvwprintw(win, 4 + i, 4, "   [ ] %-10s ", lang_name[i]);
            }
        }

        mvwhline(win, 11, 1, ACS_HLINE, width - 2);
        mvwprintw(win, 12, 2, " [↑/↓] Move   [Enter] Next   [q] Quit ");
        wrefresh(win);

        ch = wgetch(win);
        switch (ch) {
            case KEY_UP:
                choice = (choice - 1 + n_langs) % n_langs;
                break;
            case KEY_DOWN:
                choice = (choice + 1) % n_langs;
                break;
            case Enter:
                config->lang = choice;
                return;
            case 'q':
                return;
        }
    }
}

// step3: remote repository setting
void set_options(WINDOW *win, GimbapConfig *config) {
    if (config->remote) return;

    int choice = 1; // 0: No, 1: Yes
    int ch;

    keypad(win, TRUE);

    while (1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 19, " [ GitHub Remote ] ");
        mvwprintw(win, 2, 4, "Do you want to create or connect");
        mvwprintw(win, 3, 4, "a remote repository on GitHub?");
        if (choice == 1) {
                wattron(win, A_REVERSE);
                mvwprintw(win, 6, 6, " [*] Yes, Create or Connect Remote Repository ");
                wattroff(win, A_REVERSE);
                mvwprintw(win, 7, 6, " [ ] No, Local only ");
            } else {
                mvwprintw(win, 6, 6, " [ ] Yes, Create or Connect Remote Repository ");
                wattron(win, A_REVERSE);
                mvwprintw(win, 7, 6, " [*] No, Local only ");
                wattroff(win, A_REVERSE);
            }
            mvwhline(win, 11, 1, ACS_HLINE, width - 2);
            mvwprintw(win, 12, 2, " [↑/↓] Move   [Enter] Next   [q] Quit ");
            wrefresh(win);

        ch = wgetch(win);
            switch (ch) {
                case KEY_UP:
                case KEY_DOWN:
                    choice = !choice;
                    break;
                case Enter:
                    config->remote = (choice == 1);
                    return;
                case 'q':
                    return;
        }
    }
}

// step4: select the license
void set_license(WINDOW *win, GimbapConfig *config) {
    if (config->license != 0) return; // if license is already selected, skip input

    int n_licenses = sizeof(license_name) / sizeof(char *);
    int choice = 0;
    int ch;

    keypad(win, TRUE); 
    while (1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 0, (width - 13) / 2, " [ License ] ");
        mvwprintw(win, 2, 4, "select a license for your project:");

        for (int i = 0; i < n_licenses; i++) {
            if (i == choice) {
                wattron(win, A_REVERSE);
                mvwprintw(win, 4 + i, 4, " > [*] %-20s ", license_name[i]);
                wattroff(win, A_REVERSE);
            } else {
                mvwprintw(win, 4 + i, 4, "   [ ] %-20s ", license_name[i]);
            }
        }
        mvwhline(win, 11, 1, ACS_HLINE, width - 2);
        mvwprintw(win, 12, 2, " [↑/↓] Move   [Enter] Next   [q] Quit ");
        wrefresh(win);

        ch = wgetch(win);
        switch (ch) {
            case KEY_UP:
                choice = (choice - 1 + n_licenses) % n_licenses;
                break;
            case KEY_DOWN:
                choice = (choice + 1) % n_licenses;
                break;
            case Enter:
                config->license = choice; // (0: None, 1: MIT, ...)
                return;
            case 'q':
                return;
        }
    }
}

//step5: readme.md setting
void set_readme(WINDOW *win, GimbapConfig *config) {
    if(config->readme) return; // if readme is already set, skip input

    int choice = 0; // 0: Yes (Create template), 1: No
    int ch;

    keypad(win, TRUE);

    while (1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 0, (width - 13) / 2, " [ README.md ] ");
        mvwprintw(win, 2, 4, "Do you want to create a README.md");
        mvwprintw(win, 3, 4, "template for your project?");

        if (choice == 0) {
            wattron(win, A_REVERSE);
            mvwprintw(win, 5, 6, " > [*] Yes, create a template ");
            wattroff(win, A_REVERSE);
            mvwprintw(win, 6, 6, "   [ ] No ");
        } else {
            mvwprintw(win, 5, 6, "   [ ] Yes, create a template ");
            wattron(win, A_REVERSE);
            mvwprintw(win, 6, 6, " > [*] No ");
            wattroff(win, A_REVERSE);
        }

        mvwprintw(win, 8, 6, "* A basic template including project ");
        mvwprintw(win, 7, 6, "description and usage instructions will be created.");
        mvwhline(win, 11, 1, ACS_HLINE, width - 2);
        mvwprintw(win, 12, 2, " [↑/↓] Move   [Enter] Finish   [q] Quit ");
        wrefresh(win);

        ch = wgetch(win);
        switch (ch) {
            case KEY_UP:
            case KEY_DOWN:
            case ' ':
                choice = !choice;
                break;
            case Enter:
                config->readme = (choice == 0); 
                return;
            case 'q':
                return;
        }
    }
}

void print_summary(GimbapConfig *config) {
    int total_width = 60;
    char title_text[100];
    
    sprintf(title_text, " %s Configuration ", config->name);
    
    int text_len = strlen(title_text);
    int side_padding = (total_width - text_len) / 2;

    for (int i = 0; i < side_padding; i++) printf("=");
    if (text_len % 2 != 0) printf("=");
    printf("%s", title_text);
    for (int i = 0; i <  side_padding; i++) printf("=");
    printf("\n");

    printf("Language     : %s\n", lang_name[config->lang]);
    printf("Git Remote   : %s\n", config->remote ? "Yes" : "No");
    printf("License      : %s\n", license_name[config->license]);
    printf("Add README   : %s\n", config->readme ? "Yes" : "No");

    for (int i = 0; i < total_width; i++) printf("=");
    printf("\n");
}