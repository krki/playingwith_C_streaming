/**
 * @file user.c
 * @brief Implementação do módulo para gerenciamento de utilizadores e suas interações
 */

#include "user.h"
#include "csvutil.h"
#include <ctype.h>

int user_init_manager(UserManager *manager, int initial_user_capacity, 
                     int initial_interaction_capacity) {
    if (manager == NULL || initial_user_capacity <= 0 || initial_interaction_capacity <= 0) {
        return 0;
    }
    
    manager->users = (User*)malloc(initial_user_capacity * sizeof(User));
    if (manager->users == NULL) {
        return 0;
    }
    
    manager->interactions = (Interaction*)malloc(initial_interaction_capacity * sizeof(Interaction));
    if (manager->interactions == NULL) {
        free(manager->users);
        return 0;
    }
    
    manager->count = 0;
    manager->capacity = initial_user_capacity;
    manager->interaction_count = 0;
    manager->interaction_capacity = initial_interaction_capacity;
    
    return 1;
}

void user_free_manager(UserManager *manager) {
    if (manager == NULL) {
        return;
    }
    
    free(manager->users);
    free(manager->interactions);
    manager->users = NULL;
    manager->interactions = NULL;
    manager->count = 0;
    manager->capacity = 0;
    manager->interaction_count = 0;
    manager->interaction_capacity = 0;
}

int user_load_from_csv(UserManager *manager, const char *filename) {
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
        
        if (field_count >= 2) {  // ID, nome de utilizador
            // Verificar se precisamos aumentar a capacidade do gerenciador
            if (manager->count >= manager->capacity) {
                int new_capacity = manager->capacity * 2;
                User *new_users = (User*)realloc(manager->users, new_capacity * sizeof(User));
                
                if (new_users == NULL) {
                    fclose(file);
                    return -1;
                }
                
                manager->users = new_users;
                manager->capacity = new_capacity;
            }
            
            User *user = &manager->users[manager->count];
            
            user->id = atoi(fields[0]);
            strncpy(user->username, fields[1], MAX_USERNAME_LENGTH - 1);
            user->username[MAX_USERNAME_LENGTH - 1] = '\0';
            
            user->favorite_count = 0;
            user->interaction_count = 0;
            
            // Carregar favoritos se houver (campo 2 em diante)
            for (int i = 2; i < field_count && user->favorite_count < 100; i++) {
                user->favorite_contents[user->favorite_count++] = atoi(fields[i]);
            }
            
            manager->count++;
            loaded_count++;
        }
    }
    
    fclose(file);
    return loaded_count;
}

int user_save_to_csv(UserManager *manager, const char *filename) {
    if (manager == NULL || filename == NULL) {
        return 0;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    
    // Escrever cabeçalho
    fprintf(file, "ID,Nome de Utilizador,Favoritos\n");
    
    // Escrever dados
    for (int i = 0; i < manager->count; i++) {
        User *user = &manager->users[i];
        
        // Calcular o número de campos total (ID + username + favoritos)
        int field_count = 2 + user->favorite_count;
        char **fields = (char**)malloc(field_count * sizeof(char*));
        
        if (fields == NULL) {
            fclose(file);
            return 0;
        }
        
        char id_str[20];
        sprintf(id_str, "%d", user->id);
        
        fields[0] = id_str;
        fields[1] = user->username;
        
        // Adicionar os favoritos
        char **favorite_strs = (char**)malloc(user->favorite_count * sizeof(char*));
        if (favorite_strs == NULL) {
            free(fields);
            fclose(file);
            return 0;
        }
        
        for (int j = 0; j < user->favorite_count; j++) {
            favorite_strs[j] = (char*)malloc(20 * sizeof(char));
            if (favorite_strs[j] == NULL) {
                for (int k = 0; k < j; k++) {
                    free(favorite_strs[k]);
                }
                free(favorite_strs);
                free(fields);
                fclose(file);
                return 0;
            }
            
            sprintf(favorite_strs[j], "%d", user->favorite_contents[j]);
            fields[2 + j] = favorite_strs[j];
        }
        
        csv_write_line(file, fields, field_count);
        
        // Libertar memória
        for (int j = 0; j < user->favorite_count; j++) {
            free(favorite_strs[j]);
        }
        free(favorite_strs);
        free(fields);
    }
    
    fclose(file);
    return 1;
}

int user_load_interactions_from_csv(UserManager *manager, const char *filename) {
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
        
        if (field_count >= 4) {  // user_id, content_id, type, timestamp
            // Verificar se precisamos aumentar a capacidade do gerenciador
            if (manager->interaction_count >= manager->interaction_capacity) {
                int new_capacity = manager->interaction_capacity * 2;
                Interaction *new_interactions = (Interaction*)realloc(manager->interactions, 
                                                new_capacity * sizeof(Interaction));
                
                if (new_interactions == NULL) {
                    fclose(file);
                    return -1;
                }
                
                manager->interactions = new_interactions;
                manager->interaction_capacity = new_capacity;
            }
            
            Interaction *interaction = &manager->interactions[manager->interaction_count];
            
            interaction->user_id = atoi(fields[0]);
            interaction->content_id = atoi(fields[1]);
            interaction->type = user_interaction_type_from_string(fields[2]);
            interaction->timestamp = (time_t)atoll(fields[3]);
            
            // Atualizar o contador de interações do utilizador
            User *user = user_get_by_id(manager, interaction->user_id);
            if (user != NULL) {
                user->interaction_count++;
            }
            
            manager->interaction_count++;
            loaded_count++;
        }
    }
    
    fclose(file);
    return loaded_count;
}

int user_save_interactions_to_csv(UserManager *manager, const char *filename) {
    if (manager == NULL || filename == NULL) {
        return 0;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    
    // Escrever cabeçalho
    fprintf(file, "ID_Utilizador,ID_Conteudo,Tipo,Timestamp\n");
    
    // Escrever dados
    for (int i = 0; i < manager->interaction_count; i++) {
        Interaction *interaction = &manager->interactions[i];
        
        char user_id_str[20], content_id_str[20], timestamp_str[30];
        char type_str[MAX_INTERACTION_TYPE_LENGTH];
        
        sprintf(user_id_str, "%d", interaction->user_id);
        sprintf(content_id_str, "%d", interaction->content_id);
        sprintf(timestamp_str, "%ld", (long)interaction->timestamp);
        
        user_interaction_type_to_string(interaction->type, type_str, MAX_INTERACTION_TYPE_LENGTH);
        
        char *fields[4] = {
            user_id_str,
            content_id_str,
            type_str,
            timestamp_str
        };
        
        csv_write_line(file, fields, 4);
    }
    
    fclose(file);
    return 1;
}

int user_add(UserManager *manager, const char *username) {
    if (manager == NULL || username == NULL || strlen(username) == 0) {
        return -1;
    }
    
    // Verificar se o nome de utilizador já existe
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->users[i].username, username) == 0) {
            return -1; // Nome de utilizador já existe
        }
    }
    
    // Verificar se precisamos aumentar a capacidade do gerenciador
    if (manager->count >= manager->capacity) {
        int new_capacity = manager->capacity * 2;
        User *new_users = (User*)realloc(manager->users, new_capacity * sizeof(User));
        
        if (new_users == NULL) {
            return -1;
        }
        
        manager->users = new_users;
        manager->capacity = new_capacity;
    }
    
    // Encontrar o próximo ID disponível
    int next_id = 1;
    for (int i = 0; i < manager->count; i++) {
        if (manager->users[i].id >= next_id) {
            next_id = manager->users[i].id + 1;
        }
    }
    
    // Adicionar o novo utilizador
    User *user = &manager->users[manager->count];
    
    user->id = next_id;
    strncpy(user->username, username, MAX_USERNAME_LENGTH - 1);
    user->username[MAX_USERNAME_LENGTH - 1] = '\0';
    user->favorite_count = 0;
    user->interaction_count = 0;
    
    manager->count++;
    return next_id;
}

int user_remove(UserManager *manager, int user_id) {
    if (manager == NULL || user_id <= 0) {
        return 0;
    }
    
    // Buscar o utilizador com o ID especificado
    int index = -1;
    for (int i = 0; i < manager->count; i++) {
        if (manager->users[i].id == user_id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return 0; // ID não encontrado
    }
    
    // Remover as interações do utilizador
    int i = 0;
    while (i < manager->interaction_count) {
        if (manager->interactions[i].user_id == user_id) {
            // Mover a última interação para a posição atual
            manager->interactions[i] = manager->interactions[manager->interaction_count - 1];
            manager->interaction_count--;
        } else {
            i++;
        }
    }
    
    // Mover os utilizadores seguintes uma posição para trás
    for (int i = index; i < manager->count - 1; i++) {
        manager->users[i] = manager->users[i + 1];
    }
    
    manager->count--;
    return 1;
}

int user_register_interaction(UserManager *manager, int user_id, int content_id, 
                              InteractionType type) {
    if (manager == NULL || user_id <= 0 || content_id <= 0) {
        return 0;
    }
    
    // Verificar se o utilizador existe
    User *user = user_get_by_id(manager, user_id);
    if (user == NULL) {
        return 0;
    }
    
    // Verificar se precisamos aumentar a capacidade do gerenciador de interações
    if (manager->interaction_count >= manager->interaction_capacity) {
        int new_capacity = manager->interaction_capacity * 2;
        Interaction *new_interactions = (Interaction*)realloc(manager->interactions, 
                                        new_capacity * sizeof(Interaction));
        
        if (new_interactions == NULL) {
            return 0;
        }
        
        manager->interactions = new_interactions;
        manager->interaction_capacity = new_capacity;
    }
    
    // Adicionar a nova interação
    Interaction *interaction = &manager->interactions[manager->interaction_count];
    
    interaction->user_id = user_id;
    interaction->content_id = content_id;
    interaction->type = type;
    interaction->timestamp = time(NULL);
    
    manager->interaction_count++;
    user->interaction_count++;
    
    // Se a interação for do tipo FAVORITE, adicionar o conteúdo aos favoritos
    if (type == INTERACTION_FAVORITE) {
        user_add_favorite(manager, user_id, content_id);
    }
    
    return 1;
}

int user_add_favorite(UserManager *manager, int user_id, int content_id) {
    if (manager == NULL || user_id <= 0 || content_id <= 0) {
        return 0;
    }
    
    User *user = user_get_by_id(manager, user_id);
    if (user == NULL) {
        return 0;
    }
    
    // Verificar se o conteúdo já está nos favoritos
    for (int i = 0; i < user->favorite_count; i++) {
        if (user->favorite_contents[i] == content_id) {
            return 1; // Já está nos favoritos
        }
    }
    
    // Verificar se ainda há espaço nos favoritos
    if (user->favorite_count >= 100) {
        return 0; // Lista de favoritos cheia
    }
    
    // Adicionar aos favoritos
    user->favorite_contents[user->favorite_count++] = content_id;
    return 1;
}

int user_remove_favorite(UserManager *manager, int user_id, int content_id) {
    if (manager == NULL || user_id <= 0 || content_id <= 0) {
        return 0;
    }
    
    User *user = user_get_by_id(manager, user_id);
    if (user == NULL) {
        return 0;
    }
    
    // Buscar o conteúdo na lista de favoritos
    int index = -1;
    for (int i = 0; i < user->favorite_count; i++) {
        if (user->favorite_contents[i] == content_id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return 0; // Conteúdo não está nos favoritos
    }
    
    // Mover os itens seguintes uma posição para trás
    for (int i = index; i < user->favorite_count - 1; i++) {
        user->favorite_contents[i] = user->favorite_contents[i + 1];
    }
    
    user->favorite_count--;
    return 1;
}

User* user_get_by_id(UserManager *manager, int user_id) {
    if (manager == NULL || user_id <= 0) {
        return NULL;
    }
    
    for (int i = 0; i < manager->count; i++) {
        if (manager->users[i].id == user_id) {
            return &manager->users[i];
        }
    }
    
    return NULL;
}

User* user_get_by_username(UserManager *manager, const char *username) {
    if (manager == NULL || username == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->users[i].username, username) == 0) {
            return &manager->users[i];
        }
    }
    
    return NULL;
}

int user_get_interaction_count(UserManager *manager, int user_id) {
    if (manager == NULL || user_id <= 0) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < manager->interaction_count; i++) {
        if (manager->interactions[i].user_id == user_id) {
            count++;
        }
    }
    
    return count;
}

void user_interaction_type_to_string(InteractionType type, char *buffer, size_t size) {
    const char *type_str = "";
    
    switch (type) {
        case INTERACTION_PLAY:
            type_str = "PLAY";
            break;
        case INTERACTION_PAUSE:
            type_str = "PAUSE";
            break;
        case INTERACTION_COMPLETE:
            type_str = "COMPLETE";
            break;
        case INTERACTION_FAVORITE:
            type_str = "FAVORITE";
            break;
    }
    
    strncpy(buffer, type_str, size - 1);
    buffer[size - 1] = '\0';
}

InteractionType user_interaction_type_from_string(const char *str) {
    if (str == NULL) {
        return INTERACTION_PLAY; // Valor padrão
    }
    
    // Converter para maiúsculas para comparação
    char upper_str[MAX_INTERACTION_TYPE_LENGTH];
    size_t len = strlen(str);
    
    if (len >= MAX_INTERACTION_TYPE_LENGTH) {
        len = MAX_INTERACTION_TYPE_LENGTH - 1;
    }
    
    for (size_t i = 0; i < len; i++) {
        upper_str[i] = toupper(str[i]);
    }
    upper_str[len] = '\0';
    
    if (strcmp(upper_str, "PLAY") == 0) {
        return INTERACTION_PLAY;
    } else if (strcmp(upper_str, "PAUSE") == 0) {
        return INTERACTION_PAUSE;
    } else if (strcmp(upper_str, "COMPLETE") == 0) {
        return INTERACTION_COMPLETE;
    } else if (strcmp(upper_str, "FAVORITE") == 0) {
        return INTERACTION_FAVORITE;
    }
    
    return INTERACTION_PLAY; // Valor padrão
}