#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void removeChar(char *str, int index) {
    int length = strlen(str);
    
    if (index < 0 || index >= length) {
        return;
    }

    for (int i = index; i < length - 1; i++) {
        str[i] = str[i + 1];
    }
    str[length - 1] = 0;
}

void screeningStr(char *str, bool *inQuote) {
    for (int i=0; str[i]!=0; ++i) {
        if (str[i]=='"') *inQuote = !*inQuote;              // 'н'
        else if (str[i]=='\'' && str[i+1]=='\\' && str[i+2]==0x04 && str[i+3]==0x3d) {
            str[i+3] = 'n';
            removeChar(str, i+2);
        }

        if (*inQuote && str[i-2]=='\\' && str[i]==0x3d && str[i-1]==0x04 && str[i-3]!='\\') {
            str[i] = 'n';
            removeChar(str, i-1);
        }
    }
}
int main() {
    bool test = false;
    char *text = "test text \"\\н пваапв \\\н\" '\\н'";


    screeningStr(text, &test);
    return 0;
}
