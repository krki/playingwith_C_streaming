/**
 * @file content.c
 * @brief Implementação do módulo para gerenciamento de conteúdos do Streamflix
 */

#include "content.h"
#include "csvutil.h"
#include <ctype.h>

int content_init_catalog(ContentCatalog *catalog, int initial_capacity) {
    if (catalog == NULL || initial_capacity <= 0) {
        return 0;
    }
    
    catalog->items = (Content*)malloc(initial_capacity * sizeof(Content));
    if (catalog->items == NULL) {
        return 0;
    }
    
    catalog->count = 0;
    catalog->capacity = initial_capacity;
    return 1;
}

void content_free_catalog(ContentCatalog *catalog) {
    if (catalog == NULL) {
        return;
    }
    
    free(catalog->items);
    catalog->items = NULL;
    catalog->count = 0;
    catalog->capacity = 0;
}

int content_load_from_csv(ContentCatalog *catalog, const char *filename) {
    if (catalog == NULL || filename == NULL) {
        return -1;
    }
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    
    char buffer[1024];
    char *fields[MAX_FIELD_COUNT];
    int loaded_count = 0;
    
    // Pular a linha de cabeçalho
    csv_read_line(file, buffer, sizeof(buffer));
    
    // Ler os dados
    while (csv_read_line(file, buffer, sizeof(buffer))) {
        int field_count = csv_parse_line(buffer, fields, MAX_FIELD_COUNT);
        
        if (field_count >= 5) {  // ID, título, categoria, duração, classificação, visualizações
            // Verificar se precisamos aumentar a capacidade do catálogo
            if (catalog->count >= catalog->capacity) {
                int new_capacity = catalog->capacity * 2;
                Content *new_items = (Content*)realloc(catalog->items, new_capacity * sizeof(Content));
                
                if (new_items == NULL) {
                    fclose(file);
                    return -1;
                }
                
                catalog->items = new_items;
                catalog->capacity = new_capacity;
            }
            
            Content *content = &catalog->items[catalog->count];
            
            content->id = atoi(fields[0]);
            strncpy(content->title, fields[1], MAX_TITLE_LENGTH - 1);
            content->title[MAX_TITLE_LENGTH - 1] = '\0';
            
            strncpy(content->category, fields[2], MAX_CATEGORY_LENGTH - 1);
            content->category[MAX_CATEGORY_LENGTH - 1] = '\0';
            
            content->duration = atoi(fields[3]);
            content->age_rating = atoi(fields[4]);
            content->views = field_count > 5 ? atoi(fields[5]) : 0;
            
            catalog->count++;
            loaded_count++;
        }
    }
    
    fclose(file);
    return loaded_count;
}

int content_save_to_csv(ContentCatalog *catalog, const char *filename) {
    if (catalog == NULL || filename == NULL) {
        return 0;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    
    // Escrever cabeçalho
    fprintf(file, "ID,Titulo,Categoria,Duração,Classificacao,Visualizacoes\n");
    
    // Escrever dados
    for (int i = 0; i < catalog->count; i++) {
        Content *content = &catalog->items[i];
        
        char id_str[20], duration_str[20], age_rating_str[20], views_str[20];
        sprintf(id_str, "%d", content->id);
        sprintf(duration_str, "%d", content->duration);
        sprintf(age_rating_str, "%d", content->age_rating);
        sprintf(views_str, "%d", content->views);
        
        char *fields[6] = {
            id_str,
            content->title,
            content->category,
            duration_str,
            age_rating_str,
            views_str
        };
        
        csv_write_line(file, fields, 6);
    }
    
    fclose(file);
    return 1;
}

int content_add(ContentCatalog *catalog, const char *title, const char *category, 
               int duration, int age_rating) {
    if (catalog == NULL || title == NULL || category == NULL || 
        duration <= 0 || age_rating < 0) {
        return -1;
    }
    
    // Verificar se precisamos aumentar a capacidade do catálogo
    if (catalog->count >= catalog->capacity) {
        int new_capacity = catalog->capacity * 2;
        Content *new_items = (Content*)realloc(catalog->items, new_capacity * sizeof(Content));
        
        if (new_items == NULL) {
            return -1;
        }
        
        catalog->items = new_items;
        catalog->capacity = new_capacity;
    }
    
    // Encontrar o próximo ID disponível
    int next_id = 1;
    for (int i = 0; i < catalog->count; i++) {
        if (catalog->items[i].id >= next_id) {
            next_id = catalog->items[i].id + 1;
        }
    }
    
    // Adicionar o novo conteúdo
    Content *content = &catalog->items[catalog->count];
    
    content->id = next_id;
    strncpy(content->title, title, MAX_TITLE_LENGTH - 1);
    content->title[MAX_TITLE_LENGTH - 1] = '\0';
    
    strncpy(content->category, category, MAX_CATEGORY_LENGTH - 1);
    content->category[MAX_CATEGORY_LENGTH - 1] = '\0';
    
    content->duration = duration;
    content->age_rating = age_rating;
    content->views = 0;
    
    catalog->count++;
    return next_id;
}

int content_remove(ContentCatalog *catalog, int id) {
    if (catalog == NULL || id <= 0) {
        return 0;
    }
    
    // Buscar o conteúdo com o ID especificado
    int index = -1;
    for (int i = 0; i < catalog->count; i++) {
        if (catalog->items[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return 0; // ID não encontrado
    }
    
    // Mover os conteúdos seguintes uma posição para trás
    for (int i = index; i < catalog->count - 1; i++) {
        catalog->items[i] = catalog->items[i + 1];
    }
    
    catalog->count--;
    return 1;
}

int content_edit(ContentCatalog *catalog, int id, const char *title, 
                const char *category, int duration, int age_rating) {
    if (catalog == NULL || id <= 0) {
        return 0;
    }
    
    // Buscar o conteúdo com o ID especificado
    Content *content = NULL;
    for (int i = 0; i < catalog->count; i++) {
        if (catalog->items[i].id == id) {
            content = &catalog->items[i];
            break;
        }
    }
    
    if (content == NULL) {
        return 0; // ID não encontrado
    }
    
    // Atualizar os campos especificados
    if (title != NULL) {
        strncpy(content->title, title, MAX_TITLE_LENGTH - 1);
        content->title[MAX_TITLE_LENGTH - 1] = '\0';
    }
    
    if (category != NULL) {
        strncpy(content->category, category, MAX_CATEGORY_LENGTH - 1);
        content->category[MAX_CATEGORY_LENGTH - 1] = '\0';
    }
    
    if (duration > 0) {
        content->duration = duration;
    }
    
    if (age_rating >= 0) {
        content->age_rating = age_rating;
    }
    
    return 1;
}

int content_search_by_title(ContentCatalog *catalog, const char *title, 
                            int *results, int max_results) {
    if (catalog == NULL || title == NULL || results == NULL || max_results <= 0) {
        return 0;
    }
    
    int found_count = 0;
    
    for (int i = 0; i < catalog->count && found_count < max_results; i++) {
        // Busca por substring, ignorando maiúsculas/minúsculas
        char title_lower[MAX_TITLE_LENGTH];
        char search_lower[MAX_TITLE_LENGTH];
        
        strncpy(title_lower, catalog->items[i].title, MAX_TITLE_LENGTH - 1);
        title_lower[MAX_TITLE_LENGTH - 1] = '\0';
        
        strncpy(search_lower, title, MAX_TITLE_LENGTH - 1);
        search_lower[MAX_TITLE_LENGTH - 1] = '\0';
        
        // Converter para minúsculas
        for (int j = 0; title_lower[j]; j++) {
            title_lower[j] = tolower(title_lower[j]);
        }
        
        for (int j = 0; search_lower[j]; j++) {
            search_lower[j] = tolower(search_lower[j]);
        }
        
        if (strstr(title_lower, search_lower) != NULL) {
            results[found_count++] = catalog->items[i].id;
        }
    }
    
    return found_count;
}

int content_search_by_category(ContentCatalog *catalog, const char *category, 
                              int *results, int max_results) {
    if (catalog == NULL || category == NULL || results == NULL || max_results <= 0) {
        return 0;
    }
    
    int found_count = 0;
    
    for (int i = 0; i < catalog->count && found_count < max_results; i++) {
        // Busca exata, ignorando maiúsculas/minúsculas
        char category_lower[MAX_CATEGORY_LENGTH];
        char search_lower[MAX_CATEGORY_LENGTH];
        
        strncpy(category_lower, catalog->items[i].category, MAX_CATEGORY_LENGTH - 1);
        category_lower[MAX_CATEGORY_LENGTH - 1] = '\0';
        
        strncpy(search_lower, category, MAX_CATEGORY_LENGTH - 1);
        search_lower[MAX_CATEGORY_LENGTH - 1] = '\0';
        
        // Converter para minúsculas
        for (int j = 0; category_lower[j]; j++) {
            category_lower[j] = tolower(category_lower[j]);
        }
        
        for (int j = 0; search_lower[j]; j++) {
            search_lower[j] = tolower(search_lower[j]);
        }
        
        if (strcmp(category_lower, search_lower) == 0) {
            results[found_count++] = catalog->items[i].id;
        }
    }
    
    return found_count;
}

int content_search_by_age_rating(ContentCatalog *catalog, int age_rating, 
                                int *results, int max_results) {
    if (catalog == NULL || age_rating < 0 || results == NULL || max_results <= 0) {
        return 0;
    }
    
    int found_count = 0;
    
    for (int i = 0; i < catalog->count && found_count < max_results; i++) {
        if (catalog->items[i].age_rating == age_rating) {
            results[found_count++] = catalog->items[i].id;
        }
    }
    
    return found_count;
}

int content_increment_views(ContentCatalog *catalog, int id) {
    if (catalog == NULL || id <= 0) {
        return 0;
    }
    
    Content *content = content_get_by_id(catalog, id);
    if (content == NULL) {
        return 0;
    }
    
    content->views++;
    return 1;
}

Content* content_get_by_id(ContentCatalog *catalog, int id) {
    if (catalog == NULL || id <= 0) {
        return NULL;
    }
    
    for (int i = 0; i < catalog->count; i++) {
        if (catalog->items[i].id == id) {
            return &catalog->items[i];
        }
    }
    
    return NULL;
}