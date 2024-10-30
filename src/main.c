#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Defs {
    int len_d; // 30 by def.
    int len_do; // 30 by def.

    //char[][len] initDefs = {"#внѣдрить", "цѣло", "императоръ", "дань", "долговязый", "краткій", "знакъ", "машинный", "коли", "коль", "але", "егда", "конѣцъ", "далѣе",
    //"пути", "яко", "кондиции", "умолчаніе", "делати", "кратокъ-плавъ", "дологъ-плавъ", "перѣпись", "для", "походъ", "дворянинъ", "крестьянинъ", "размеръ", "домъ", "нѣту", NULL};

    //char[][len] initDefsOn = {"#include", "int", "main", "return", "long", "short", "char", "auto", "if", "if", "else", "while", "break", "continue",
    //"switch", "case", "default", "default", "do", "float", "double", "enum", "for", "goto", "signed", "unsigned", "sizeof", "struct", "void", NULL};

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
    // Увеличиваем размер массива на 1
    char** newArray = realloc(array, (*size + 1) * sizeof(char*));
    if (newArray == NULL) {
        // Обработка ошибки при выделении памяти
        perror("Unable to allocate memory");
        return array; // Возвращаем старый массив, если realloc не удался
    }

    // Выделяем память для новой строки и копируем её
    newArray[*size] = malloc(strlen(newString) + 1);
    if (newArray[*size] == NULL) {
        // Обработка ошибки при выделении памяти
        perror("Unable to allocate memory for new string");
        return newArray; // Возвращаем новый массив, но без добавленной строки
    }
    strcpy(newArray[*size], newString);

    // Увеличиваем размер
    (*size)++;
    return newArray;
}

bool replaceWord(char *str, const char *oldWord, const char *newWord) {
    char buffer[1024]; // Буфер для хранения результата
    char *pos;
    bool inQuotes = false;
    int index = 0;
    int oldWordLen = strlen(oldWord);
    int newWordLen = strlen(newWord);
    bool wasReplaced = false;

    for (int i = 0; str[i] != '\0'; i++) {
        // Проверяем, находимся ли мы в кавычках
        if (str[i] == '"') {
            inQuotes = !inQuotes; // Меняем состояние
        }

        // Если не в кавычках, проверяем на замену
        if (!inQuotes) {
            // Проверяем, совпадает ли слово с oldWord
            if (strncmp(&str[i], oldWord, oldWordLen) == 0 && 
                (i == 0 || str[i - 1] == ' ') && 
                (str[i + oldWordLen] == ' ' || str[i + oldWordLen] == '(' || 
                str[i + oldWordLen] == '*' || str[i + oldWordLen] == '\0')) {

                // Если совпадает, добавляем newWord в буфер
                if (!wasReplaced) wasReplaced = true;
                strcpy(&buffer[index], newWord);
                index += newWordLen;
                i += oldWordLen - 1; // Пропускаем oldWord
            } else {
                // Иначе просто копируем текущий символ
                buffer[index++] = str[i];
            }
        } else {
            // Если в кавычках, просто копируем символ
            buffer[index++] = str[i];
        }
    }
    buffer[index] = '\0'; // Завершаем строку

    // Копируем результат обратно в исходную строку
    strcpy(str, buffer);
    return wasReplaced;
}

char* readFile(const char* filename) {
    FILE *file;
    char line[256];
    char *content = NULL; // Указатель на строку для хранения всего содержимого
    size_t total_length = 0; // Общая длина содержимого

    // Открываем файл для чтения
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("ГЦЦ001: Курьезъ при открытіи лѣтописи.");
        return NULL;
    }

    // Читаем файл построчно
    while (fgets(line, sizeof(line), file)) {
        size_t line_length = strlen(line);

        // Увеличиваем общий размер содержимого
        char *new_content = realloc(content, total_length + line_length + 1); // +1 для '\0'
        if (new_content == NULL) {
            perror("ГЦЦ002: Курьезъ при выдѣленіи знати.");
            free(content);
            fclose(file);
            return NULL;
        }
        content = new_content;

        // Копируем строку в содержимое
        strcpy(content + total_length, line);
        total_length += line_length; // Обновляем общую длину
    }

    // Закрываем файл
    fclose(file);
    return content;
}

void g_includes(char *g_content) {
    // find index of #внѣдрить
    const char *p = (const char*) g_content;
    const char *word = "#внѣдрить";

    int index = 0;
    int wordLength = strlen(word);

    while ((p = strstr(p, word)) != NULL) {
        printf("Найдено слово '%s' на индексе: %d\n", word, p - g_content);
        p += wordLength;
    }
}

char** splitString(const char *input, int *count) {
    char *input_copy = strdup(input); // Копируем входную строку
    if (!input_copy) {
        return NULL; // Проверка на успешное выделение памяти
    }

    char **args = NULL;
    *count = 0;

    // Разделяем строку на аргументы
    char *token = strtok(input_copy, " ");
    while (token != NULL) {
        // Увеличиваем размер массива
        args = (char **)realloc(args, (*count + 1) * sizeof(char *));
        if (!args) {
            free(input_copy); // Освобождаем память в случае ошибки
            return NULL; // Проверка на успешное выделение памяти
        }

        // Выделяем память для нового аргумента и копируем его
        args[*count] = strdup(token);
        if (!args[*count]) {
            free(input_copy);
            for (int i = 0; i < *count; i++) {
                free(args[i]); // Освобождаем ранее выделенную память
            }
            free(args);
            return NULL; // Проверка на успешное выделение памяти
        }

        (*count)++;
        token = strtok(NULL, " ");
    }

    free(input_copy); // Освобождаем память, выделенную для копии строки
    return args; // Возвращаем массив аргументов
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

int main(int argv, char** argc) {
    if (argv == 3 && argc[1] == "checkg") {
        g_includes(readFile(argc[2]));
        return 0;
    }

    if (argv < 3) {
        printf("ГЦЦ005: очень мало тезисовъ.\n");
        return -1;
    }

    char* str = readFile(argc[2]);
    struct Defs defs;
    char allDefs[][30] = {"#внѣдрить", "цѣло", "императоръ", "дань", "долговязый", "краткій", "знакъ", "машинный", "коли", "коль", "але", "егда", "конѣцъ", "далѣе",
    "пути", "яко", "кондиции", "умолчаніе", "делати", "кратокъ-плавъ", "дологъ-плавъ", "перѣпись", "для", "походъ", "дворянинъ", "крестьянинъ", "размеръ", "домъ", "нѣту", "немой"};

    char defsIn[][30] = {"#include", "int", "main", "return", "long", "short", "char", "auto", "if", "if", "else", "while", "break", "continue",
    "switch", "case", "default", "default", "do", "float", "double", "enum", "for", "goto", "signed", "unsigned", "sizeof", "struct", "void", "const"};
    
    defs.len_d = 30;
    defs.len_do = 30;

    defs.allDefs = malloc(defs.len_d * sizeof(char*));
    defs.allDefsOn = malloc(defs.len_do * sizeof(char*));

    for (int i = 0; i < defs.len_d; i++) {
        defs.allDefs[i] = allDefs[i];
    }
    for (int i = 0; i < defs.len_d; i++) {
        defs.allDefsOn[i] = defsIn[i];
    }

    printf("1'%s'\n", str);
    // TODO: сделать по токенам
    int len = 0;
    const int strl = strlen(str);
    printf("tokens start\n");
    char **tokens = slavenizator(str, &len);  len-=3;
    printf("tokens end\n");

    printf("len: %d", len);
    for (int i=0; i<len; ++i) {
        if (tokens[i][0] == '#') {
            int lena = 0;
            char** args = splitString(tokens[i], &lena);
            printf("split str\n");

            if (lena != 0) {
                if (lena > 1 && strcmp(args[0], "#искоренить") == 0) {
                    if (lena == 2) {
                        defs.allDefs = addString(defs.allDefs, &defs.len_d, args[1]);
                        defs.allDefsOn = addString(defs.allDefsOn, &defs.len_do, "");
                    } else if (lena == 3) {
                        printf("defs start\n");
                        defs.allDefs = addString(defs.allDefs, &defs.len_d, args[1]);
                        defs.allDefsOn = addString(defs.allDefsOn, &defs.len_do, args[2]);
                    }
                    printf("искоренено\n");
                }
            }
        }
        printf("i: '%d'\n", i);
    }

    printf("2'%s'\n", str);
    rmSth(str, "//");
    rmSth(str, "#искоренить");

    rmNewLines(str);
    rmSpaces(str);

    // обработка инфы дефов
    printf("i (all): %d\n", defs.len_d);
    for (int i=0; i<defs.len_d; ++i) {
        printf("i: %d\n", i);
        replaceWord(str, defs.allDefs[i], defs.allDefsOn[i]);
        printf("'%s'\n", defs.allDefs[i]);
    }

    
    // For .г (like .h, but глава)
    //g_includes();
    printf("defs:");
    for (int i=0; i<defs.len_d; ++i) {
        printf("%s\n", defs.allDefs[i]);
    }

    FILE *file = fopen(".gcc_temp.c", "w");
    if (file == NULL) {
        perror("ГЦЦ003: Курьезъ при открытіи лѣтописи для сохраненія.");
        return EXIT_FAILURE;
    }

    fprintf(file, str);
    fclose(file);
    

    FILE *build = fopen("./build", "w");
    if (file == NULL) {
        perror("ГЦЦ004: Курьезъ при открытіи лѣтописи.");
        return EXIT_FAILURE;
    }

    char buff[50];
    sprintf(buff, "#! /bin/sh\n\n%s", argc[1]);
    fprintf(build, buff);
    fclose(build);

    int status = system("./build");
    if (status == 0)
        printf("ГЦЦ: Лѣтопись переписана и ждетъ запуска.\n");

    return 0;
}

