/**
 * @file recommendation.c
 * @brief Implementação do módulo para sistema de recomendação de conteúdos
 */

#include "recommendation.h"
#include <math.h>

// Estrutura auxiliar para ordenação de conteúdos por score
typedef struct {
    int content_id;
    float score;
} ContentScore;

// Função de comparação para ordenação de scores (ordem decrescente)
static int compare_scores(const void *a, const void *b) {
    const ContentScore *score_a = (const ContentScore *)a;
    const ContentScore *score_b = (const ContentScore *)b;
    
    if (score_b->score > score_a->score) return 1;
    if (score_b->score < score_a->score) return -1;
    return 0;
}

int recommendation_by_content_similarity(UserManager *user_manager, 
                                       ContentCatalog *content_catalog,
                                       int user_id, 
                                       int *recommendations, 
                                       int max_recommendations) {
    if (user_manager == NULL || content_catalog == NULL || 
        user_id <= 0 || recommendations == NULL || max_recommendations <= 0) {
        return 0;
    }
    
    // Encontrar conteúdos que o utilizador assistiu (interação COMPLETE)
    int watched_ids[100];
    int watched_count = 0;
    
    for (int i = 0; i < user_manager->interaction_count && watched_count < 100; i++) {
        Interaction *interaction = &user_manager->interactions[i];
        
        if (interaction->user_id == user_id && 
            interaction->type == INTERACTION_COMPLETE) {
            
            // Verificar se o conteúdo já está na lista
            int exists = 0;
            for (int j = 0; j < watched_count; j++) {
                if (watched_ids[j] == interaction->content_id) {
                    exists = 1;
                    break;
                }
            }
            
            if (!exists) {
                watched_ids[watched_count++] = interaction->content_id;
            }
        }
    }
    
    if (watched_count == 0) {
        // Se o utilizador não assistiu nenhum conteúdo, retornar recomendações por popularidade
        return recommendation_by_popularity(content_catalog, recommendations, max_recommendations);
    }
    
    // Calcular similaridade de todos os conteúdos não assistidos com os assistidos
    ContentScore scores[1000]; // Assumindo no máximo 1000 conteúdos
    int score_count = 0;
    
    for (int i = 0; i < content_catalog->count && score_count < 1000; i++) {
        Content *candidate = &content_catalog->items[i];
        
        // Verificar se o utilizador já assistiu este conteúdo
        int already_watched = 0;
        for (int j = 0; j < watched_count; j++) {
            if (watched_ids[j] == candidate->id) {
                already_watched = 1;
                break;
            }
        }
        
        if (!already_watched) {
            float total_similarity = 0.0f;
            
            for (int j = 0; j < watched_count; j++) {
                Content *watched = content_get_by_id(content_catalog, watched_ids[j]);
                if (watched != NULL) {
                    total_similarity += recommendation_calculate_similarity(watched, candidate);
                }
            }
            
            // Calcular score médio
            float avg_similarity = watched_count > 0 ? total_similarity / watched_count : 0;
            
            scores[score_count].content_id = candidate->id;
            scores[score_count].score = avg_similarity;
            score_count++;
        }
    }
    
    // Ordenar os scores em ordem decrescente
    qsort(scores, score_count, sizeof(ContentScore), compare_scores);
    
    // Copiar as recomendações para o array de resultado
    int recommendation_count = 0;
    for (int i = 0; i < score_count && recommendation_count < max_recommendations; i++) {
        recommendations[recommendation_count++] = scores[i].content_id;
    }
    
    return recommendation_count;
}

int recommendation_by_category(UserManager *user_manager, 
                              ContentCatalog *content_catalog,
                              int user_id, 
                              int *recommendations, 
                              int max_recommendations) {
    if (user_manager == NULL || content_catalog == NULL || 
        user_id <= 0 || recommendations == NULL || max_recommendations <= 0) {
        return 0;
    }
    
    // Contar a frequência de cada categoria assistida pelo utilizador
    char categories[100][MAX_CATEGORY_LENGTH];
    int category_counts[100] = {0};
    int category_count = 0;
    
    for (int i = 0; i < user_manager->interaction_count; i++) {
        Interaction *interaction = &user_manager->interactions[i];
        
        if (interaction->user_id == user_id && 
            (interaction->type == INTERACTION_PLAY || 
             interaction->type == INTERACTION_COMPLETE)) {
            
            Content *content = content_get_by_id(content_catalog, interaction->content_id);
            if (content != NULL) {
                // Verificar se a categoria já está na lista
                int category_index = -1;
                for (int j = 0; j < category_count; j++) {
                    if (strcmp(categories[j], content->category) == 0) {
                        category_index = j;
                        break;
                    }
                }
                
                if (category_index >= 0) {
                    category_counts[category_index]++;
                } else if (category_count < 100) {
                    strncpy(categories[category_count], content->category, MAX_CATEGORY_LENGTH - 1);
                    categories[category_count][MAX_CATEGORY_LENGTH - 1] = '\0';
                    category_counts[category_count] = 1;
                    category_count++;
                }
            }
        }
    }
    
    if (category_count == 0) {
        // Se o utilizador não tem histórico de categorias, retornar recomendações por popularidade
        return recommendation_by_popularity(content_catalog, recommendations, max_recommendations);
    }
    
    // Ordenar as categorias por frequência (ordenação simples por ser poucos itens)
    for (int i = 0; i < category_count - 1; i++) {
        for (int j = i + 1; j < category_count; j++) {
            if (category_counts[j] > category_counts[i]) {
                // Trocar contagens
                int temp_count = category_counts[i];
                category_counts[i] = category_counts[j];
                category_counts[j] = temp_count;
                
                // Trocar nomes de categorias
                char temp_name[MAX_CATEGORY_LENGTH];
                strncpy(temp_name, categories[i], MAX_CATEGORY_LENGTH);
                strncpy(categories[i], categories[j], MAX_CATEGORY_LENGTH);
                strncpy(categories[j], temp_name, MAX_CATEGORY_LENGTH);
            }
        }
    }
    
    // Criar uma lista dos conteúdos não assistidos nas categorias populares
    ContentScore scores[1000]; // Assumindo no máximo 1000 conteúdos
    int score_count = 0;
    
    for (int i = 0; i < content_catalog->count && score_count < 1000; i++) {
        Content *content = &content_catalog->items[i];
        
        // Verificar se o utilizador já assistiu este conteúdo
        if (!recommendation_has_watched(user_manager, user_id, content->id)) {
            // Encontrar a posição da categoria deste conteúdo na lista de categorias populares
            int category_index = -1;
            for (int j = 0; j < category_count; j++) {
                if (strcmp(categories[j], content->category) == 0) {
                    category_index = j;
                    break;
                }
            }
            
            if (category_index >= 0) {
                scores[score_count].content_id = content->id;
                
                // O score é a posição inversa na lista (mais popular = maior score)
                scores[score_count].score = (float)(category_count - category_index) +
                                           // Adicionar uma fração da contagem normalizada
                                           (float)category_counts[category_index] / 
                                           (category_counts[0] > 0 ? category_counts[0] : 1);
                
                score_count++;
            }
        }
    }
    
    // Ordenar os scores em ordem decrescente
    qsort(scores, score_count, sizeof(ContentScore), compare_scores);
    
    // Copiar as recomendações para o array de resultado
    int recommendation_count = 0;
    for (int i = 0; i < score_count && recommendation_count < max_recommendations; i++) {
        recommendations[recommendation_count++] = scores[i].content_id;
    }
    
    return recommendation_count;
}

int recommendation_by_popularity(ContentCatalog *content_catalog, 
                                int *recommendations, 
                                int max_recommendations) {
    if (content_catalog == NULL || recommendations == NULL || max_recommendations <= 0) {
        return 0;
    }
    
    // Criar array de scores com base nas visualizações
    ContentScore scores[1000]; // Assumindo no máximo 1000 conteúdos
    int score_count = 0;
    
    for (int i = 0; i < content_catalog->count && score_count < 1000; i++) {
        Content *content = &content_catalog->items[i];
        
        scores[score_count].content_id = content->id;
        scores[score_count].score = (float)content->views;
        score_count++;
    }
    
    // Ordenar os scores em ordem decrescente
    qsort(scores, score_count, sizeof(ContentScore), compare_scores);
    
    // Copiar as recomendações para o array de resultado
    int recommendation_count = 0;
    for (int i = 0; i < score_count && recommendation_count < max_recommendations; i++) {
        recommendations[recommendation_count++] = scores[i].content_id;
    }
    
    return recommendation_count;
}

int recommendation_personalized(UserManager *user_manager, 
                              ContentCatalog *content_catalog,
                              int user_id, 
                              int *recommendations, 
                              int max_recommendations) {
    if (user_manager == NULL || content_catalog == NULL || 
        user_id <= 0 || recommendations == NULL || max_recommendations <= 0) {
        return 0;
    }
    
    // Obter recomendações por diferentes métodos
    int similarity_recs[MAX_RECOMMENDATIONS];
    int category_recs[MAX_RECOMMENDATIONS];
    int popularity_recs[MAX_RECOMMENDATIONS];
    
    int similarity_count = recommendation_by_content_similarity(
        user_manager, content_catalog, user_id, similarity_recs, MAX_RECOMMENDATIONS);
    
    int category_count = recommendation_by_category(
        user_manager, content_catalog, user_id, category_recs, MAX_RECOMMENDATIONS);
    
    int popularity_count = recommendation_by_popularity(
        content_catalog, popularity_recs, MAX_RECOMMENDATIONS);
    
    // Combinar as recomendações com pesos
    ContentScore combined_scores[3 * MAX_RECOMMENDATIONS];
    int combined_count = 0;
    
    // Adicionar recomendações por similaridade (peso alto)
    for (int i = 0; i < similarity_count; i++) {
        combined_scores[combined_count].content_id = similarity_recs[i];
        combined_scores[combined_count].score = 3.0f * (float)(similarity_count - i) / similarity_count;
        combined_count++;
    }
    
    // Adicionar recomendações por categoria (peso médio)
    for (int i = 0; i < category_count; i++) {
        // Verificar se já foi adicionado
        int exists = 0;
        for (int j = 0; j < combined_count; j++) {
            if (combined_scores[j].content_id == category_recs[i]) {
                exists = 1;
                // Se já existe, aumentar o score
                combined_scores[j].score += 2.0f * (float)(category_count - i) / category_count;
                break;
            }
        }
        
        if (!exists) {
            combined_scores[combined_count].content_id = category_recs[i];
            combined_scores[combined_count].score = 2.0f * (float)(category_count - i) / category_count;
            combined_count++;
        }
    }
    
    // Adicionar recomendações por popularidade (peso baixo)
    for (int i = 0; i < popularity_count; i++) {
        // Verificar se já foi adicionado
        int exists = 0;
        for (int j = 0; j < combined_count; j++) {
            if (combined_scores[j].content_id == popularity_recs[i]) {
                exists = 1;
                // Se já existe, aumentar o score
                combined_scores[j].score += 1.0f * (float)(popularity_count - i) / popularity_count;
                break;
            }
        }
        
        if (!exists) {
            combined_scores[combined_count].content_id = popularity_recs[i];
            combined_scores[combined_count].score = 1.0f * (float)(popularity_count - i) / popularity_count;
            combined_count++;
        }
    }
    
    // Ordenar os scores combinados em ordem decrescente
    qsort(combined_scores, combined_count, sizeof(ContentScore), compare_scores);
    
    // Copiar as recomendações para o array de resultado
    int recommendation_count = 0;
    for (int i = 0; i < combined_count && recommendation_count < max_recommendations; i++) {
        recommendations[recommendation_count++] = combined_scores[i].content_id;
    }
    
    return recommendation_count;
}

int recommendation_has_watched(UserManager *user_manager, int user_id, int content_id) {
    if (user_manager == NULL || user_id <= 0 || content_id <= 0) {
        return 0;
    }
    
    for (int i = 0; i < user_manager->interaction_count; i++) {
        Interaction *interaction = &user_manager->interactions[i];
        
        if (interaction->user_id == user_id && 
            interaction->content_id == content_id && 
            (interaction->type == INTERACTION_COMPLETE || interaction->type == INTERACTION_PLAY)) {
            return 1; // Utilizador já assistiu
        }
    }
    
    return 0; // Utilizador não assistiu
}

float recommendation_calculate_similarity(Content *content1, Content *content2) {
    if (content1 == NULL || content2 == NULL) {
        return 0.0f;
    }
    
    float similarity = 0.0f;
    
    // Similaridade por categoria (0.6 se for a mesma categoria)
    if (strcmp(content1->category, content2->category) == 0) {
        similarity += 0.6f;
    }
    
    // Similaridade por classificação etária (até 0.2)
    float age_diff = abs(content1->age_rating - content2->age_rating);
    float max_age_diff = 18.0f; // Assumindo máxima diferença possível
    float age_similarity = 0.2f * (1.0f - (age_diff / max_age_diff));
    similarity += age_similarity;
    
    // Similaridade por duração (até 0.2)
    float duration_diff = abs(content1->duration - content2->duration);
    float max_duration = (content1->duration > content2->duration) ? 
                         content1->duration : content2->duration;
    
    if (max_duration > 0) {
        float duration_similarity = 0.2f * (1.0f - (duration_diff / max_duration));
        similarity += duration_similarity;
    }
    
    return similarity; // Valor entre 0 e 1
}