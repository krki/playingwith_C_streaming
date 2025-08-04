/**
 * @file recommendation.h
 * @brief Módulo para sistema de recomendação de conteúdos
 * 
 * Este módulo contém funções e estruturas para recomendar conteúdos
 * aos utilizadores com base em seu histórico de visualização e interações.
 */

#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "content.h"
#include "user.h"

#define MAX_RECOMMENDATIONS 10

/**
 * @brief Estrutura que representa uma recomendação com seu score
 */
typedef struct {
    int content_id;        /**< ID do conteúdo recomendado */
    float score;           /**< Pontuação de relevância da recomendação */
} Recommendation;

/**
 * @brief Gera recomendações com base em conteúdos similares aos assistidos pelo utilizador
 * 
 * @param user_manager Ponteiro para o gerenciador de utilizadores
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param user_id ID do utilizador
 * @param recommendations Array para armazenar os IDs dos conteúdos recomendados
 * @param max_recommendations Tamanho máximo do array de recomendações
 * @return int Número de recomendações geradas
 */
int recommendation_by_content_similarity(UserManager *user_manager, 
                                       ContentCatalog *content_catalog,
                                       int user_id, 
                                       int *recommendations, 
                                       int max_recommendations);

/**
 * @brief Gera recomendações com base nas categorias mais assistidas pelo utilizador
 * 
 * @param user_manager Ponteiro para o gerenciador de utilizadores
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param user_id ID do utilizador
 * @param recommendations Array para armazenar os IDs dos conteúdos recomendados
 * @param max_recommendations Tamanho máximo do array de recomendações
 * @return int Número de recomendações geradas
 */
int recommendation_by_category(UserManager *user_manager, 
                              ContentCatalog *content_catalog,
                              int user_id, 
                              int *recommendations, 
                              int max_recommendations);

/**
 * @brief Gera recomendações com base nos conteúdos populares (mais visualizados)
 * 
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param recommendations Array para armazenar os IDs dos conteúdos recomendados
 * @param max_recommendations Tamanho máximo do array de recomendações
 * @return int Número de recomendações geradas
 */
int recommendation_by_popularity(ContentCatalog *content_catalog, 
                                int *recommendations, 
                                int max_recommendations);

/**
 * @brief Gera recomendações personalizadas combinando vários métodos
 * 
 * @param user_manager Ponteiro para o gerenciador de utilizadores
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param user_id ID do utilizador
 * @param recommendations Array para armazenar os IDs dos conteúdos recomendados
 * @param max_recommendations Tamanho máximo do array de recomendações
 * @return int Número de recomendações geradas
 */
int recommendation_personalized(UserManager *user_manager, 
                              ContentCatalog *content_catalog,
                              int user_id, 
                              int *recommendations, 
                              int max_recommendations);

/**
 * @brief Verifica se um utilizador já assistiu a um conteúdo
 * 
 * @param user_manager Ponteiro para o gerenciador de utilizadores
 * @param user_id ID do utilizador
 * @param content_id ID do conteúdo
 * @return int 1 se o utilizador já assistiu, 0 caso contrário
 */
int recommendation_has_watched(UserManager *user_manager, int user_id, int content_id);

/**
 * @brief Calcula a similaridade entre dois conteúdos (0 a 1)
 * 
 * @param content1 Ponteiro para o primeiro conteúdo
 * @param content2 Ponteiro para o segundo conteúdo
 * @return float Valor de similaridade entre 0 (nada similar) e 1 (idêntico)
 */
float recommendation_calculate_similarity(Content *content1, Content *content2);

#endif /* RECOMMENDATION_H */