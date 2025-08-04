/**
 * @file list.c
 * @brief Implementação do módulo para gerenciamento de listas personalizadas
 */

#include "list.h"
#include "csvutil.h"

int list_init_manager(ListManager *manager, int initial_capacity) {
    if (manager == NULL || initial_capacity <= 0) {
        return 0;
    }
    
    manager->lists = (CustomList*)malloc(initial_capacity * sizeof(CustomList));
    if (manager->lists == NULL) {
        return 0;
    }
    
    manager->count = 0;
    manager->capacity = initial_capacity;
    return 1;
}

void list_free_manager(ListManager *manager) {
    if (manager == NULL) {
        return;
    }
    
    free(manager->lists);
    manager->lists = NULL;
    manager->count = 0;
    manager->capacity = 0;
}

int list_load_from_csv(ListManager *manager, const char *filename) {
    if (manager == NULL || filename == NULL) {
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
        
        if (field_count >= 3) {  // ID, user_id, nome, conteúdos...
            // Verificar se precisamos aumentar a capacidade do gerenciador
            if (manager->count >= manager->capacity) {
                int new_capacity = manager->capacity * 2;
                CustomList *new_lists = (CustomList*)realloc(manager->lists, new_capacity * sizeof(CustomList));
                
                if (new_lists == NULL) {
                    fclose(file);
                    return -1;
                }
                
                manager->lists = new_lists;
                manager->capacity = new_capacity;
            }
            
            CustomList *list = &manager->lists[manager->count];
            
            list->id = atoi(fields[0]);
            list->user_id = atoi(fields[1]);
            
            strncpy(list->name, fields[2], MAX_LIST_NAME_LENGTH - 1);
            list->name[MAX_LIST_NAME_LENGTH - 1] = '\0';
            
            list->count = 0;
            
            // Carregar conteúdos (campo 3 em diante)
            for (int i = 3; i < field_count && list->count < MAX_LIST_ITEMS; i++) {
                list->content_ids[list->count++] = atoi(fields[i]);
            }
            
            manager->count++;
            loaded_count++;
        }
    }
    
    fclose(file);
    return loaded_count;
}

int list_save_to_csv(ListManager *manager, const char *filename) {
    if (manager == NULL || filename == NULL) {
        return 0;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    
    // Escrever cabeçalho
    fprintf(file, "ID,ID_Utilizador,Nome,Conteudos\n");
    
    // Escrever dados
    for (int i = 0; i < manager->count; i++) {
        CustomList *list = &manager->lists[i];
        
        // Calcular o número de campos total (ID + user_id + nome + conteúdos)
        int field_count = 3 + list->count;
        char **fields = (char**)malloc(field_count * sizeof(char*));
        
        if (fields == NULL) {
            fclose(file);
            return 0;
        }
        
        char id_str[20], user_id_str[20];
        sprintf(id_str, "%d", list->id);
        sprintf(user_id_str, "%d", list->user_id);
        
        fields[0] = id_str;
        fields[1] = user_id_str;
        fields[2] = list->name;
        
        // Adicionar os IDs dos conteúdos
        char **content_strs = (char**)malloc(list->count * sizeof(char*));
        if (content_strs == NULL) {
            free(fields);
            fclose(file);
            return 0;
        }
        
        for (int j = 0; j < list->count; j++) {
            content_strs[j] = (char*)malloc(20 * sizeof(char));
            if (content_strs[j] == NULL) {
                for (int k = 0; k < j; k++) {
                    free(content_strs[k]);
                }
                free(content_strs);
                free(fields);
                fclose(file);
                return 0;
            }
            
            sprintf(content_strs[j], "%d", list->content_ids[j]);
            fields[3 + j] = content_strs[j];
        }
        
        csv_write_line(file, fields, field_count);
        
        // Liberar memória
        for (int j = 0; j < list->count; j++) {
            free(content_strs[j]);
        }
        free(content_strs);
        free(fields);
    }
    
    fclose(file);
    return 1;
}

int list_create(ListManager *manager, int user_id, const char *name) {
    if (manager == NULL || user_id <= 0 || name == NULL || strlen(name) == 0) {
        return -1;
    }
    
    // Verificar se precisamos aumentar a capacidade do gerenciador
    if (manager->count >= manager->capacity) {
        int new_capacity = manager->capacity * 2;
        CustomList *new_lists = (CustomList*)realloc(manager->lists, new_capacity * sizeof(CustomList));
        
        if (new_lists == NULL) {
            return -1;
        }
        
        manager->lists = new_lists;
        manager->capacity = new_capacity;
    }
    
    // Encontrar o próximo ID disponível
    int next_id = 1;
    for (int i = 0; i < manager->count; i++) {
        if (manager->lists[i].id >= next_id) {
            next_id = manager->lists[i].id + 1;
        }
    }
    
    // Criar a nova lista
    CustomList *list = &manager->lists[manager->count];
    
    list->id = next_id;
    list->user_id = user_id;
    strncpy(list->name, name, MAX_LIST_NAME_LENGTH - 1);
    list->name[MAX_LIST_NAME_LENGTH - 1] = '\0';
    list->count = 0;
    
    manager->count++;
    return next_id;
}

int list_remove(ListManager *manager, int list_id) {
    if (manager == NULL || list_id <= 0) {
        return 0;
    }
    
    // Buscar a lista com o ID especificado
    int index = -1;
    for (int i = 0; i < manager->count; i++) {
        if (manager->lists[i].id == list_id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return 0; // ID não encontrado
    }
    
    // Mover as listas seguintes uma posição para trás
    for (int i = index; i < manager->count - 1; i++) {
        manager->lists[i] = manager->lists[i + 1];
    }
    
    manager->count--;
    return 1;
}

int list_add_content(ListManager *manager, int list_id, int content_id) {
    if (manager == NULL || list_id <= 0 || content_id <= 0) {
        return 0;
    }
    
    CustomList *list = list_get_by_id(manager, list_id);
    if (list == NULL) {
        return 0;
    }
    
    // Verificar se o conteúdo já está na lista
    for (int i = 0; i < list->count; i++) {
        if (list->content_ids[i] == content_id) {
            return 1; // Já está na lista
        }
    }
    
    // Verificar se ainda há espaço na lista
    if (list->count >= MAX_LIST_ITEMS) {
        return 0; // Lista cheia
    }
    
    // Adicionar o conteúdo à lista
    list->content_ids[list->count++] = content_id;
    return 1;
}

int list_remove_content(ListManager *manager, int list_id, int content_id) {
    if (manager == NULL || list_id <= 0 || content_id <= 0) {
        return 0;
    }
    
    CustomList *list = list_get_by_id(manager, list_id);
    if (list == NULL) {
        return 0;
    }
    
    // Buscar o conteúdo na lista
    int index = -1;
    for (int i = 0; i < list->count; i++) {
        if (list->content_ids[i] == content_id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return 0; // Conteúdo não está na lista
    }
    
    // Mover os itens seguintes uma posição para trás
    for (int i = index; i < list->count - 1; i++) {
        list->content_ids[i] = list->content_ids[i + 1];
    }
    
    list->count--;
    return 1;
}

CustomList* list_get_by_id(ListManager *manager, int list_id) {
    if (manager == NULL || list_id <= 0) {
        return NULL;
    }
    
    for (int i = 0; i < manager->count; i++) {
        if (manager->lists[i].id == list_id) {
            return &manager->lists[i];
        }
    }
    
    return NULL;
}

int list_get_by_user(ListManager *manager, int user_id, int *result_ids, int max_results) {
    if (manager == NULL || user_id <= 0 || result_ids == NULL || max_results <= 0) {
        return 0;
    }
    
    int found_count = 0;
    
    for (int i = 0; i < manager->count && found_count < max_results; i++) {
        if (manager->lists[i].user_id == user_id) {
            result_ids[found_count++] = manager->lists[i].id;
        }
    }
    
    return found_count;
}

int list_rename(ListManager *manager, int list_id, const char *new_name) {
    if (manager == NULL || list_id <= 0 || new_name == NULL || strlen(new_name) == 0) {
        return 0;
    }
    
    CustomList *list = list_get_by_id(manager, list_id);
    if (list == NULL) {
        return 0;
    }
    
    strncpy(list->name, new_name, MAX_LIST_NAME_LENGTH - 1);
    list->name[MAX_LIST_NAME_LENGTH - 1] = '\0';
    
    return 1;
}