#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

char* insert_text(const char* original, const char* text_to_insert, int index) {
    if (index < 0 || index > strlen(original)) {
        perror("\033[31mГЦЦ006\033[0m: Курьезъ при высчитываніи длинны.");
        exit(7);
    }

    size_t original_len = strlen(original);
    size_t insert_len = strlen(text_to_insert);

    char* new_string = (char*)malloc(original_len + insert_len + 1);
    if (new_string == NULL) {
        perror("\033[31mГЦЦ002\033[0m: Курьезъ при выдѣленіи знати.");
        exit(6);
    }

    strncpy(new_string, original, index);
    new_string[index] = 0;

    strcat(new_string, text_to_insert);
    strcat(new_string, original + index);

    return new_string;
}

char* parseHolyCprint(char *str) {
    bool inBrackets = false;
    bool isPrint = false;
    for (int i=0; str[i]!=0; ++i) {
        if (str[i] == '(' || str[i] == ')' || str[i]==']' || str[i]=='[') inBrackets = !inBrackets;

        if (str[i] == '"' && !inBrackets) {
            int j = 0;
            while (str[i-j]!=';' || str[i-j]!='{') {
                if (str[i-j]=='=') {
                    isPrint = true;
                    break;
                }

                j++;
            }

            if (isPrint) {
                str = insert_text(str, "printf(", i);
                
                int j=8;
                while (str[j+i]!='"') {
                    ++j;
                }++j;
                
                printf("J: %d\n", j);
                str = insert_text(str, ")", i+j);
            }
        }
    }
    return str;
}

int main() {
    char *input = "int main() {\n    \"hello world\\n\";\n}";
    input = parseHolyCprint(input);
    printf(input);

    return 0;
}

