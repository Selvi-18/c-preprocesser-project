#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 1024
#define MAX_MACROS 100

typedef struct {
    char name[100];
    char value[100];
} Macro;

Macro macros[MAX_MACROS];
int macro_count = 0;

bool include_code = true;

// Function to add a macro
void add_macro(const char *line) {
    sscanf(line, "#define %s %s", macros[macro_count].name, macros[macro_count].value);
    macro_count++;
}

// Function to replace macros in a line
void replace_macros(char *line) {
    for (int i = 0; i < macro_count; i++) {
        char *pos = strstr(line, macros[i].name);
        while (pos) {
            char buffer[MAX_LINE];
            snprintf(buffer, pos - line + 1, "%s", line); // Copy up to the macro
            strcat(buffer, macros[i].value);             // Add the macro value
            strcat(buffer, pos + strlen(macros[i].name)); // Add the rest of the line
            strcpy(line, buffer);                        // Update the line
            pos = strstr(line, macros[i].name);          // Check for further occurrences
        }
    }
}

// Function to process comments
void remove_comments(char *line) {
    char *single_line_comment = strstr(line, "//");
    if (single_line_comment) {
        *single_line_comment = '\0'; // Truncate at //
    }

    char *multi_line_start = strstr(line, "/*");
    while (multi_line_start) {
        char *multi_line_end = strstr(multi_line_start, "*/");
        if (multi_line_end) {
            memmove(multi_line_start, multi_line_end + 2, strlen(multi_line_end + 2) + 1);
        } else {
            *multi_line_start = '\0'; // Truncate the line if no end found
            break;
        }
        multi_line_start = strstr(line, "/*");
    }
}

// Function to process include directives
void process_include(char *line, FILE *output) {
    char include_file[100];
    if (sscanf(line, "#include \"%[^\"]\"", include_file) == 1) {
        FILE *include_fp = fopen(include_file, "r");
        if (!include_fp) {
            fprintf(stderr, "Error: Unable to open file %s\n", include_file);
            return;
        }
        char buffer[MAX_LINE];
        while (fgets(buffer, MAX_LINE, include_fp)) {
            fputs(buffer, output);
        }
        fclose(include_fp);
    }
}

// Function to process conditional compilation
void process_conditional(char *line) {
    if (strstr(line, "#ifdef")) {
        char macro_name[100];
        sscanf(line, "#ifdef %s", macro_name);
        include_code = false;
        for (int i = 0; i < macro_count; i++) {
            if (strcmp(macros[i].name, macro_name) == 0) {
                include_code = true;
                break;
            }
        }
    } else if (strstr(line, "#ifndef")) {
        char macro_name[100];
        sscanf(line, "#ifndef %s", macro_name);
        include_code = true;
        for (int i = 0; i < macro_count; i++) {
            if (strcmp(macros[i].name, macro_name) == 0) {
                include_code = false;
                break;
            }
        }
    } else if (strstr(line, "#else")) {
        include_code = !include_code;
    } else if (strstr(line, "#endif")) {
        include_code = true;
    }
}

// Main preprocessor function
void preprocess_file(const char *input_file, const char *output_file) {
    FILE *input = fopen(input_file, "r");
    if (!input) {
        fprintf(stderr, "Error: Unable to open input file %s\n", input_file);
        return;
    }

    FILE *output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "Error: Unable to open output file %s\n", output_file);
        fclose(input);
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, input)) {
        remove_comments(line);

        if (strstr(line, "#include")) {
            process_include(line, output);
            continue;
        }

        if (strstr(line, "#define")) {
            add_macro(line);
            continue;
        }

        if (strstr(line, "#ifdef") || strstr(line, "#ifndef") || strstr(line, "#else") || strstr(line, "#endif")) {
            process_conditional(line);
            continue;
        }

        if (include_code) {
            replace_macros(line);
            fputs(line, output);
        }
    }

    fclose(input);
    fclose(output);
    printf("Preprocessing complete. Output written to %s\n", output_file);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    preprocess_file(argv[1], argv[2]);
    return 0;
}

