#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

void rmComments(char* input) {
    char* comment_start = strstr(input, "//");
    while (comment_start != NULL) {
        char* line_end = strchr(comment_start, '\n');
        if (line_end != NULL) {
            memmove(comment_start, line_end, strlen(line_end) + 1);
        } else {
            *comment_start = '\0';
        }
        comment_start = strstr(input, "//");
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

void replaceWord(char *str, const char *oldWord, const char *newWord) {
    char buffer[1024]; // Буфер для хранения результата
    char *pos;
    bool inQuotes = false;
    int index = 0;
    int oldWordLen = strlen(oldWord);
    int newWordLen = strlen(newWord);

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

int main(int argv, char** argc) {
    if (argv < 3) {
        printf("ГЦЦ005: очень мало тезисовъ.\n");
        return -1;
    }

    char* str = readFile(argc[2]);

    rmComments(str);
    rmNewLines(str);
    rmSpaces(str);


    //          STR  ЧТО МЕНЯТЬ       НА ЧТО МЕНЯТЬ
    replaceWord(str, "#внѣдрить",     "#include");
    replaceWord(str, "цѣло",          "int");
    replaceWord(str, "императоръ",    "main");
    replaceWord(str, "молвитьф",      "printf");
    replaceWord(str, "дань",          "return");
    // Respect to Xi816
    replaceWord(str, "долговязый",    "long");
    replaceWord(str, "краткій",       "short");
    replaceWord(str, "знакъ",         "char"); // Edited by adisteyf
    replaceWord(str, "машинный",      "auto");
    replaceWord(str, "коли",          "if");
    replaceWord(str, "коль",          "if");
    replaceWord(str, "але",           "else"); // Edited by adisteyf
    replaceWord(str, "егда",          "while");
    replaceWord(str, "конѣцъ",        "break");
    replaceWord(str, "далѣе",         "continue");
    replaceWord(str, "пути",          "switch");
    replaceWord(str, "яко",           "case");
    replaceWord(str, "кондиции",      "default"); // Edited by adisteyf
    replaceWord(str, "умолчаніе",     "default"); // Edited by adisteyf
    replaceWord(str, "делати",        "do");
    replaceWord(str, "кратокъ-плавъ", "float");
    replaceWord(str, "долгий-плавъ",  "double");
    replaceWord(str, "перѣпись",      "enum");
    replaceWord(str, "для",           "for");
    replaceWord(str, "походъ",        "goto");
    replaceWord(str, "дворянинъ",     "signed"); // Edited by adisteyf
    replaceWord(str, "крестьянинъ",   "unsigned"); // Edited by adisteyf
    replaceWord(str, "размѣръ",       "sizeof");
    replaceWord(str, "домъ",          "struct");
    replaceWord(str, "нѣту",          "void"); // Edited by adisteyf
    // End respect to Xi816

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

