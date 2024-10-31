#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* replace_text(const char* input) {
    const char* target = "#гвнѣдрить писатьвв.г";
    const char* replacement = "#внѣдрить <stdio.h>\n#искоренить молвитьф printf\n";

    // Вычисляем длину новой строки
    size_t input_len = strlen(input);
    size_t target_len = strlen(target);
    size_t replacement_len = strlen(replacement);

    // Проверяем, содержится ли целевая строка в входной строке
    if (strstr(input, target) == NULL) {
        // Если целевая строка не найдена, возвращаем копию входной строки
        char* result = (char*)malloc(input_len + 1);
        strcpy(result, input);
        return result;
    }

    // Вычисляем длину новой строки
    size_t new_len = input_len - target_len + replacement_len;

    // Выделяем память для новой строки
    char* result = (char*)malloc(new_len + 1);
    if (result == NULL) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Копируем часть строки до целевой строки
    char* pos = strstr(input, target);
    size_t prefix_len = pos - input;
    strncpy(result, input, prefix_len);

    // Копируем замену
    strcpy(result + prefix_len, replacement);

    // Копируем оставшуюся часть строки после целевой строки
    strcpy(result + prefix_len + replacement_len, pos + target_len);

    return result;
}

int main() {
    const char* text = "#гвнѣдрить писатьвв.г\n\n"
                       "цѣло императоръ() { // комментарий\n"
                       "    молвитьф(\"слава Петру I\\n\");\n"
                       "    молвитьф(\"и дворянамъ слава\\n\");\n\n"
                       "    знакъ* test = \"hello world\";\n    дань 0;\n}";

    char* new_text = replace_text(text);
    printf("%s\n", new_text);

    free(new_text);
    return 0;
}
