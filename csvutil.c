/**
 * @file csvutil.c
 * @brief Implementação do módulo para operações com arquivos CSV
 */

#include "csvutil.h"

int csv_read_line(FILE *file, char *buffer, int size) {
    if (fgets(buffer, size, file) == NULL) {
        return 0; // Fim do arquivo ou erro
    }
    
    // Remover o caractere de nova linha, se existir
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return 1;
}

int csv_parse_line(char *line, char **fields, int max_fields) {
    int field_count = 0;
    char *token;
    char *rest = line;
    
    while ((token = strtok_r(rest, ",", &rest)) && field_count < max_fields) {
        // Remover espaços em branco no início e fim do campo
        while (*token == ' ') token++;
        
        size_t len = strlen(token);
        while (len > 0 && (token[len - 1] == ' ' || token[len - 1] == '\r')) {
            token[--len] = '\0';
        }
        
        fields[field_count++] = token;
    }
    
    return field_count;
}

int csv_write_line(FILE *file, char **fields, int num_fields) {
    if (file == NULL || fields == NULL || num_fields <= 0) {
        return 0;
    }
    
    for (int i = 0; i < num_fields; i++) {
        fprintf(file, "%s", fields[i]);
        
        // Adicionar vírgula se não for o último campo
        if (i < num_fields - 1) {
            fprintf(file, ",");
        }
    }
    
    fprintf(file, "\n");
    return 1;
}