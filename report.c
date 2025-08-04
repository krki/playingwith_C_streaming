/**
 * @file report.c
 * @brief Implementação do módulo para geração de relatórios estatísticos
 */

#include "report.h"
#include "csvutil.h"

// Função de comparação para ordenação de conteúdos por visualizações (ordem decrescente)
static int compare_content_views(const void *a, const void *b) {
    const ContentReportItem *item_a = (const ContentReportItem *)a;
    const ContentReportItem *item_b = (const ContentReportItem *)b;
    
    return item_b->count - item_a->count;
}

// Função de comparação para ordenação de categorias por popularidade (ordem decrescente)
static int compare_category_popularity(const void *a, const void *b) {
    const CategoryReportItem *item_a = (const CategoryReportItem *)a;
    const CategoryReportItem *item_b = (const CategoryReportItem *)b;
    
    return item_b->count - item_a->count;
}

// Função de comparação para ordenação de utilizadores por atividade (ordem decrescente)
static int compare_user_activity(const void *a, const void *b) {
    const UserReportItem *item_a = (const UserReportItem *)a;
    const UserReportItem *item_b = (const UserReportItem *)b;
    
    return item_b->count - item_a->count;
}

int report_most_viewed_contents(ContentCatalog *content_catalog, 
                               ContentReportItem *results, 
                               int max_results) {
    if (content_catalog == NULL || results == NULL || max_results <= 0) {
        return 0;
    }
    
    // Copiar os dados para o array de resultados
    int count = content_catalog->count < max_results ? content_catalog->count : max_results;
    
    for (int i = 0; i < count; i++) {
        Content *content = &content_catalog->items[i];
        
        results[i].content_id = content->id;
        strncpy(results[i].title, content->title, MAX_TITLE_LENGTH - 1);
        results[i].title[MAX_TITLE_LENGTH - 1] = '\0';
        results[i].count = content->views;
    }
    
    // Ordenar por número de visualizações
    qsort(results, count, sizeof(ContentReportItem), compare_content_views);
    
    return count;
}

int report_most_popular_categories(ContentCatalog *content_catalog, 
                                  CategoryReportItem *results, 
                                  int max_results) {
    if (content_catalog == NULL || results == NULL || max_results <= 0) {
        return 0;
    }
    
    // Contar visualizações por categoria
    CategoryReportItem categories[100]; // Assumindo no máximo 100 categorias
    int category_count = 0;
    
    for (int i = 0; i < content_catalog->count; i++) {
        Content *content = &content_catalog->items[i];
        
        // Verificar se a categoria já está na lista
        int category_index = -1;
        for (int j = 0; j < category_count; j++) {
            if (strcmp(categories[j].category, content->category) == 0) {
                category_index = j;
                break;
            }
        }
        
        if (category_index >= 0) {
            categories[category_index].count += content->views;
        } else if (category_count < 100) {
            strncpy(categories[category_count].category, content->category, MAX_CATEGORY_LENGTH - 1);
            categories[category_count].category[MAX_CATEGORY_LENGTH - 1] = '\0';
            categories[category_count].count = content->views;
            category_count++;
        }
    }
    
    // Ordenar por popularidade
    qsort(categories, category_count, sizeof(CategoryReportItem), compare_category_popularity);
    
    // Copiar para os resultados
    int result_count = category_count < max_results ? category_count : max_results;
    
    for (int i = 0; i < result_count; i++) {
        results[i] = categories[i];
    }
    
    return result_count;
}

int report_most_active_users(UserManager *user_manager, 
                            UserReportItem *results, 
                            int max_results) {
    if (user_manager == NULL || results == NULL || max_results <= 0) {
        return 0;
    }
    
    // Contar interações por utilizadores
    UserReportItem users[1000]; // Assumindo no máximo 1000 utilizadores
    int user_count = 0;
    
    for (int i = 0; i < user_manager->count && user_count < 1000; i++) {
        User *user = &user_manager->users[i];
        
        users[user_count].user_id = user->id;
        strncpy(users[user_count].username, user->username, MAX_USERNAME_LENGTH - 1);
        users[user_count].username[MAX_USERNAME_LENGTH - 1] = '\0';
        users[user_count].count = user->interaction_count;
        user_count++;
    }
    
    // Ordenar por atividade
    qsort(users, user_count, sizeof(UserReportItem), compare_user_activity);
    
    // Copiar para os resultados
    int result_count = user_count < max_results ? user_count : max_results;
    
    for (int i = 0; i < result_count; i++) {
        results[i] = users[i];
    }
    
    return result_count;
}

int report_user_interactions(UserManager *user_manager, 
                            ContentCatalog *content_catalog,
                            int user_id, 
                            ContentReportItem *results, 
                            int max_results) {
    if (user_manager == NULL || content_catalog == NULL || 
        user_id <= 0 || results == NULL || max_results <= 0) {
        return 0;
    }
    
    // Procurar o utilizador
    User *user = user_get_by_id(user_manager, user_id);
    if (user == NULL) {
        return 0;
    }
    
    // Contar interações por conteúdo
    ContentReportItem interactions[1000]; // Assumindo no máximo 1000 conteúdos
    int interaction_count = 0;
    
    for (int i = 0; i < user_manager->interaction_count; i++) {
        Interaction *interaction = &user_manager->interactions[i];
        
        if (interaction->user_id == user_id) {
            // Verificar se o conteúdo já está na lista
            int content_index = -1;
            for (int j = 0; j < interaction_count; j++) {
                if (interactions[j].content_id == interaction->content_id) {
                    content_index = j;
                    break;
                }
            }
            
            if (content_index >= 0) {
                interactions[content_index].count++;
            } else if (interaction_count < 1000) {
                Content *content = content_get_by_id(content_catalog, interaction->content_id);
                if (content != NULL) {
                    interactions[interaction_count].content_id = content->id;
                    strncpy(interactions[interaction_count].title, content->title, MAX_TITLE_LENGTH - 1);
                    interactions[interaction_count].title[MAX_TITLE_LENGTH - 1] = '\0';
                    interactions[interaction_count].count = 1;
                    interaction_count++;
                }
            }
        }
    }
    
    // Ordenar por número de interações
    qsort(interactions, interaction_count, sizeof(ContentReportItem), compare_content_views);
    
    // Copiar para os resultados
    int result_count = interaction_count < max_results ? interaction_count : max_results;
    
    for (int i = 0; i < result_count; i++) {
        results[i] = interactions[i];
    }
    
    return result_count;
}

int report_export_to_csv(const char *filename, 
                        char **headers, int header_count,
                        char ***data, int row_count) {
    if (filename == NULL || headers == NULL || header_count <= 0 || 
        data == NULL || row_count <= 0) {
        return 0;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    
    // Escrever cabeçalhos
    csv_write_line(file, headers, header_count);
    
    // Escrever dados
    for (int i = 0; i < row_count; i++) {
        csv_write_line(file, data[i], header_count);
    }
    
    fclose(file);
    return 1;
}

void report_print(const char *title, 
                 char **headers, int header_count,
                 char ***data, int row_count) {
    if (title == NULL || headers == NULL || header_count <= 0 || 
        data == NULL || row_count <= 0) {
        return;
    }
    
    // Encontrar a largura de cada coluna
    int column_widths[100]; // Assumindo no máximo 100 colunas
    
    for (int i = 0; i < header_count; i++) {
        column_widths[i] = strlen(headers[i]);
        
        for (int j = 0; j < row_count; j++) {
            int data_len = strlen(data[j][i]);
            if (data_len > column_widths[i]) {
                column_widths[i] = data_len;
            }
        }
    }
    
    // Calcular a largura total da tabela
    int total_width = 1; // Iniciar com 1 para o caractere '|' inicial
    
    for (int i = 0; i < header_count; i++) {
        total_width += column_widths[i] + 3; // Espaço para o conteúdo e para '|'
    }
    
    // Imprimir título
    printf("\n%s\n", title);
    for (int i = 0; i < total_width; i++) {
        printf("=");
    }
    printf("\n");
    
    // Imprimir cabeçalhos
    printf("|");
    for (int i = 0; i < header_count; i++) {
        printf(" %-*s |", column_widths[i], headers[i]);
    }
    printf("\n");
    
    // Linha separadora
    for (int i = 0; i < total_width; i++) {
        printf("-");
    }
    printf("\n");
    
    // Imprimir dados
    for (int i = 0; i < row_count; i++) {
        printf("|");
        for (int j = 0; j < header_count; j++) {
            printf(" %-*s |", column_widths[j], data[i][j]);
        }
        printf("\n");
    }
    
    // Linha de fechamento
    for (int i = 0; i < total_width; i++) {
        printf("=");
    }
    printf("\n\n");
}