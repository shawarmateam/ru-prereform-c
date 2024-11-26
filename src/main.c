#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define INIT_LEN 36

struct Defs {
    int len_d;
    int len_do;

    char** allDefs;
    char** allDefsOn;
};

void rmNewLines(char *str) {
    bool isHashStr = false;

    for (int i = 0; str[i] != '\0'; i++) {
        if (i == 0) {
            isHashStr = (str[0] == '#');
        }

        if (str[i] == '\n') {
            if (!isHashStr)
                str[i] = ' ';
            
            isHashStr = (str[i+1] == '#');
        }
    }
}

void rmSth(char* input, const char* symbols) {
    char* comment_start = strstr(input, symbols);
    while (comment_start != NULL) {
        char* line_end = strchr(comment_start, '\n');
        if (line_end != NULL) {
            memmove(comment_start, line_end, strlen(line_end) + 1);
        } else {
            *comment_start = '\0';
        }
        comment_start = strstr(input, symbols);
    }
}

void rmSpaces(char *str) {
    int inQuotes = 0;
    int j = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '"') {
            inQuotes = !inQuotes;
        }

        if (inQuotes) { // skip
            str[j++] = str[i];
        } else {
            if (str[i] != ' ' || (j > 0 && str[j - 1] != ' ')) {
                str[j++] = str[i];
            }
        }
    }
    str[j] = '\0';
}

char** addString(char** array, int* size, const char* newString) {
    char** newArray = realloc(array, (*size + 1) * sizeof(char*));
    if (newArray == NULL) {
        perror("Unable to allocate memory");
        return array;
    }

    newArray[*size] = malloc(strlen(newString) + 1);
    if (newArray[*size] == NULL) {
        perror("Unable to allocate memory for new string");
        return newArray;
    }
    strcpy(newArray[*size], newString);
    (*size)++;
    return newArray;
}

bool replaceWord(char *str, const char *oldWord, const char *newWord) {
    char buffer[1024];
    char *pos;
    bool inQuotes = false;
    int index = 0;
    int oldWordLen = strlen(oldWord);
    int newWordLen = strlen(newWord);
    bool wasReplaced = false;
    // printf("WORD TO REPLACE: '%s'\n", newWord);

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '"') {
            inQuotes = !inQuotes;
        }

        if (!inQuotes) {
            if (strncmp(&str[i], oldWord, oldWordLen) == 0 && 
                (i == 0 || str[i - 1] == ' ' || str[i-1] == '(' || str[i-1] == '{') && 
                (str[i + oldWordLen] == ' ' || str[i + oldWordLen] == '(' || str[i + oldWordLen] == '*' || str[i + oldWordLen] == '\0' ||
                 str[i + oldWordLen] == ',' || str[i + oldWordLen] == ')' || str[i + oldWordLen] == ';' || str[i + oldWordLen] == ':')) {

                if (!wasReplaced) wasReplaced = true;
                strcpy(&buffer[index], newWord);
                index += newWordLen;
                i += oldWordLen - 1;
            } else {
                buffer[index++] = str[i];
            }
        } else {
            buffer[index++] = str[i];
        }
    }
    buffer[index] = '\0';

    strcpy(str, buffer);

    return wasReplaced;
}

char* readFile(const char* filename) {
    FILE *file;
    char line[256];
    char *content = NULL;
    size_t total_length = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "\033[31mГЦЦ001\033[0m: Курьезъ при открытіи лѣтописи %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        size_t line_length = strlen(line);

        char *new_content = realloc(content, total_length + line_length + 1);
        if (new_content == NULL) {
            perror("\033[31mГЦЦ002\033[0m: Курьезъ при выдѣленіи знати.");
            free(content);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        content = new_content;

        strcpy(content + total_length, line);
        total_length += line_length;
    }

    fclose(file);
    return content;
}

char** splitString(const char *input, int *count) {
    char *input_copy = strdup(input);
    if (!input_copy) {
        return NULL;
    }

    char **args = NULL;
    *count = 0;

    char *token = strtok(input_copy, " ");
    while (token != NULL) {
        args = (char **)realloc(args, (*count + 1) * sizeof(char *));
        if (!args) {
            free(input_copy);
            return NULL;
        }

        args[*count] = strdup(token);
        if (!args[*count]) {
            free(input_copy);
            for (int i = 0; i < *count; i++) {
                free(args[i]);
            }
            free(args);
            return NULL;
        }

        (*count)++;
        token = strtok(NULL, " ");
    }

    free(input_copy);
    return args;
}

char** slavenizator(const char* str, int* count) {
    int token_count = 0;
    const char* temp = str;
    while (*temp) {
        if (*temp == '\n') {
            token_count++;
        }
        temp++;
    }
    if (str[0] != 0) {
        token_count++;
    }

    char** tokens = malloc(token_count * sizeof(char*));
    if (!tokens) {
        return NULL;
    }

    char* str_copy = strdup(str);
    if (!str_copy) {
        free(tokens);
        return NULL;
    }

    char* token = strtok(str_copy, "\n");
    int index = 0;
    while (token) {
        tokens[index++] = strdup(token);
        token = strtok(NULL, "\n");
    }

    free(str_copy);
    *count = token_count;
    return tokens;
}

char* replaceText(const char* input, const char* target, const char* replacement) {
    size_t input_len = strlen(input);
    size_t target_len = strlen(target);
    size_t replacement_len = strlen(replacement);

    if (strstr(input, target) == NULL) {
        char* result = (char*)malloc(input_len + 1);
        strcpy(result, input);
        return result;
    }

    size_t new_len = input_len - target_len + replacement_len;

    char* result = (char*)malloc(new_len + 1);
    if (result == NULL) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    char* pos = strstr(input, target);
    size_t prefix_len = pos - input;
    strncpy(result, input, prefix_len);
    strcpy(result + prefix_len, replacement);
    strcpy(result + prefix_len + replacement_len, pos + target_len);

    return result;
}

int getLines(const char *str) {
    int count = 0;
    const char *line_start = str;
    
    while (*line_start) {
        const char *line_end = strchr(line_start, '\n');
        
        if (!line_end) {
            return 0;
        }
        
        if (line_end > line_start) {
            int is_empty = 1;
            for (const char *p = line_start; p < line_end; p++) {
                if (!isspace((unsigned char)*p)) {
                    is_empty = 0;
                    break;
                }
            }
            if (!is_empty) count++;
        }
        
        line_start = line_end + 1;
    }
    
    return count;
}

char* parsePreproc(struct Defs *defs, char *str) {
    int len = 0;
    const int strl = strlen(str);
    // printf("tokens start\n");
    char **tokens = slavenizator(str, &len);  len=getLines(str);
    // printf("tokens end\n");

    // printf("len: %d", len);

    for (int i=0; i<len; ++i) { // first check vnedreniya (внедрения)
        if (tokens[i][0] == '#') {
            int lena = 0;
            char** args = splitString(tokens[i], &lena);

            if (lena != 0) {
                if (lena > 1 && strcmp(args[0], "#гвнѣдрить") == 0) {
                    char* g_file = readFile(args[1]);
                    // printf("replacing (\n%s\n)\n", str);
                    rmSth(g_file, "//");
                    str = replaceText(str, tokens[i], g_file);
                    // printf("replaced\n");
                    // printf("LOG: STR: '%s'\n", str);
                    
                    FILE *save_log = fopen("./logs", "w");
                    fputs(str, save_log); fclose(save_log);
                }
            }
        }
    }

    tokens = slavenizator(str, &len);  len=getLines(str); // new parse (это костыль. мне лень. потом сделаю адекватную длинну)
    // printf("getlines: '%d'\n", getLines(str));
    for (int i=0; i<len; ++i) {
        // printf("TOKEN: '%s'", tokens[i]);
        if (tokens[i][0] == '#') {
            // printf("TOKENS[i] == '%s'\n", tokens[i]);
            int lena = 0;
            char** args = splitString(tokens[i], &lena);
            // printf("split str (%s)\n", args[0]);

            if (lena != 0) {
                if (lena > 1 && strcmp(args[0], "#искоренить") == 0) {
                    if (lena == 2) {
                        defs->allDefs = addString(defs->allDefs, &defs->len_d, args[1]);
                        defs->allDefsOn = addString(defs->allDefsOn, &defs->len_do, "");
                    } else if (lena == 3) {
                        // printf("defs start\n");
                        defs->allDefs = addString(defs->allDefs, &defs->len_d, args[1]);
                        defs->allDefsOn = addString(defs->allDefsOn, &defs->len_do, args[2]);
                    }
                    // printf("\nискоренено (%s)\n", defs->allDefs[defs->len_d-1]);
                }
            }
        }
        // printf("             I == '%d', len == '%d'\n", i, len);
    }

    rmSth(str, "//");
    rmSth(str, "#искоренить");

    rmNewLines(str);
    rmSpaces(str);
    return str;
}

void screeningStr(const char *input, char *output) {
    const char *cyrillic_n = "\\\xd0\xbd"; // utf-8 '\н'
    const char *latin_n = "\\n"; // '\n'
    size_t cyrillic_n_len = strlen(cyrillic_n);
    size_t latin_n_len = strlen(latin_n);
    
    while (*input) {
        if (strncmp(input, cyrillic_n, cyrillic_n_len) == 0) {
            strcpy(output, latin_n);
            output += latin_n_len;
            input += cyrillic_n_len;
        } else {
            *output++ = *input++;
        }
    }
    *output = 0;
}

int main(int argv, char** argc) {
    if (argv < 3) {
        printf("\033[31mГЦЦ005\033[0m: очень мало тезисовъ.\n");
        return EXIT_FAILURE;
    }

    char* str = readFile(argc[2]);
    struct Defs* defs = (struct Defs*)malloc(sizeof(struct Defs));
    char allDefs[][INIT_LEN] = {"#внѣдрить", "цѣло", "дань", "долговязый", "краткій", "знакъ", "машинный", "коли", "коль", "але", "егда", "конѣцъ", "далѣе",
    "пути", "яко", "кондиціи", "умолчаніе", "дѣлати", "кратокъ-плавъ", "дологъ-плавъ", "перѣпись", "для", "походъ", "дворянинъ", "крестьянинъ", "размеръ", "домъ", "нѣту", "немой",
    "НИЧТО", "размеръ", "БЪ0", "БЪ1", "БЪ2", "БЪ4", "БЪ8"};

    char defsIn[][INIT_LEN] = {"#include", "int", "return", "long", "short", "char", "auto", "if", "if", "else", "while", "break", "continue",
    "switch", "case", "default", "default", "do", "float", "double", "enum", "for", "goto", "signed", "unsigned", "sizeof", "struct", "void", "const", "NULL", "sizeof", "void", "char", "short", "int", "long"};

    defs->len_d = INIT_LEN;
    defs->len_do = INIT_LEN;

    defs->allDefs = malloc(defs->len_d * sizeof(char*));
    defs->allDefsOn = malloc(defs->len_do * sizeof(char*));

    for (int i = 0; i < defs->len_d; i++) {
        defs->allDefs[i] = allDefs[i];
    }
    for (int i = 0; i < defs->len_d; i++) {
        defs->allDefsOn[i] = defsIn[i];
    }

    str = parsePreproc(defs, str);

    // обработка инфы дефов
    for (int i=0; i<defs->len_d; ++i) {
        replaceWord(str, defs->allDefs[i], defs->allDefsOn[i]);
        
        char *str_new = malloc(strlen(str)*2);
        screeningStr(str, str_new);
        str = strdup(str_new);
        free(str_new);
    }

    FILE *file = fopen("/tmp/.gcc_temp.c", "w");
    if (file == NULL) {
        perror("\033[31mГЦЦ003\033[0m: Курьезъ при открытіи лѣтописи для сохраненія.");
        return EXIT_FAILURE;
    }

    fputs(str, file);
    fputs("int main(){императоръ();return 0;}", file);
    fclose(file);
    
    if (strcmp(argc[1], "^") != 0) {
        FILE *build = fopen("./build", "w");
        if (file == NULL) {
            perror("\033[31mГЦЦ004\033[0m: Курьезъ при открытіи лѣтописи сборки.");
            return EXIT_FAILURE;
        }

        char buff[180];
        sprintf(buff, "#! /bin/sh\n\n%.128s", argc[1]);
        fputs(buff, build);
        fclose(build);
    }

    int status = system("./build");
    if (status == 0)
        printf("\033[32mГЦЦ\033[0m: Лѣтопись переписана и ждетъ запуска.\n");

    return 0;
}

