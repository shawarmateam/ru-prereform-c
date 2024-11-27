char* insert_text(const char* original, const char* text_to_insert, int index) {
    if (index < 0 || index > strlen(original)) {
        return NULL;
    }

    size_t original_len = strlen(original);
    size_t insert_len = strlen(text_to_insert);

    char* new_string = (char*)malloc(original_len + insert_len + 1);
    if (new_string == NULL) {
        return NULL;    }

    strncpy(new_string, original, index);
    new_string[index] = 0;

    strcat(new_string, text_to_insert);
    strcat(new_string, original + index);

    return new_string;
}

int main() {
    const char* original = "test text";
    const char* text_to_insert = "printf(";
    int index = 5;

    char* result = insert_text(original, text_to_insert, index);
    result = insert_text(result, ")", 9+7);
    printf("Результат: %s\n", result);
    free(result);

    return 0;
}
