#include <stdio.h>
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

void replaceWord(char *str, const char *oldWord, const char *newWord) {
    rmNewLines(str);
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
                (str[i + oldWordLen] == ' ' || str[i + oldWordLen] == '(' || str[i + oldWordLen] == '\0')) {
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

int main() {
    char str[1024] = "#внѣдрить <stdio.h>\n\nцѣло императоръ() {\n    молвитьф(\"слава Петру I\");\n    дань 0;\n}";

    //          STR  ЧТО МЕНЯТЬ       НА ЧТО МЕНЯТЬ
    replaceWord(str, "#внѣдрить",     "#include");
    replaceWord(str, "цѣло",          "int");
    replaceWord(str, "императоръ()",  "main()");
    replaceWord(str, "молвитьф",      "printf");
    replaceWord(str, "дань",          "return");


    printf("%s\n", str);

    return 0;
}

