/**
 * @file content.h
 * @brief Módulo para gerenciamento de conteúdos do Streamflix
 * 
 * Este módulo contém funções e estruturas para gerenciar os conteúdos
 * disponíveis no catálogo da Streamflix, incluindo adição, remoção,
 * edição e pesquisa de conteúdos.
 */

#ifndef CONTENT_H
#define CONTENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE_LENGTH 100
#define MAX_CATEGORY_LENGTH 50
#define MAX_FIELD_COUNT 10

/**
 * @brief Estrutura que representa um conteúdo no catálogo
 */
typedef struct {
    int id;                            /**< Identificador único do conteúdo */
    char title[MAX_TITLE_LENGTH];      /**< Título do conteúdo */
    char category[MAX_CATEGORY_LENGTH]; /**< Categoria do conteúdo */
    int duration;                      /**< Duração em minutos */
    int age_rating;                    /**< Classificação etária */
    int views;                         /**< Número de visualizações */
} Content;

/**
 * @brief Estrutura que gerencia a coleção de conteúdos
 */
typedef struct {
    Content *items;        /**< Array dinâmico de conteúdos */
    int count;             /**< Número atual de conteúdos */
    int capacity;          /**< Capacidade máxima do array */
} ContentCatalog;

/**
 * @brief Inicializa o catálogo de conteúdos
 * 
 * @param catalog Ponteiro para o catálogo a ser inicializado
 * @param initial_capacity Capacidade inicial do catálogo
 * @return int 1 se a inicialização foi bem-sucedida, 0 caso contrário
 */
int content_init_catalog(ContentCatalog *catalog, int initial_capacity);

/**
 * @brief Liberta a memória alocada para o catálogo de conteúdos
 * 
 * @param catalog Ponteiro para o catálogo a ser libertado
 */
void content_free_catalog(ContentCatalog *catalog);

/**
 * @brief Carrega conteúdos de um arquivo CSV para o catálogo
 * 
 * @param catalog Ponteiro para o catálogo onde os conteúdos serão armazenados
 * @param filename Nome do arquivo CSV
 * @return int Número de conteúdos carregados ou -1 em caso de erro
 */
int content_load_from_csv(ContentCatalog *catalog, const char *filename);

/**
 * @brief Salva o catálogo de conteúdos em um arquivo CSV
 * 
 * @param catalog Ponteiro para o catálogo a ser salvo
 * @param filename Nome do arquivo CSV
 * @return int 1 se o salvamento foi bem-sucedido, 0 caso contrário
 */
int content_save_to_csv(ContentCatalog *catalog, const char *filename);

/**
 * @brief Adiciona um novo conteúdo ao catálogo
 * 
 * @param catalog Ponteiro para o catálogo
 * @param title Título do conteúdo
 * @param category Categoria do conteúdo
 * @param duration Duração em minutos
 * @param age_rating Classificação etária
 * @return int ID do conteúdo adicionado ou -1 em caso de erro
 */
int content_add(ContentCatalog *catalog, const char *title, const char *category, 
               int duration, int age_rating);

/**
 * @brief Remove um conteúdo do catálogo pelo ID
 * 
 * @param catalog Ponteiro para o catálogo
 * @param id ID do conteúdo a ser removido
 * @return int 1 se a remoção foi bem-sucedida, 0 caso contrário
 */
int content_remove(ContentCatalog *catalog, int id);

/**
 * @brief Edita as informações de um conteúdo pelo ID
 * 
 * @param catalog Ponteiro para o catálogo
 * @param id ID do conteúdo a ser editado
 * @param title Novo título (NULL para manter o atual)
 * @param category Nova categoria (NULL para manter a atual)
 * @param duration Nova duração (0 para manter a atual)
 * @param age_rating Nova classificação etária (0 para manter a atual)
 * @return int 1 se a edição foi bem-sucedida, 0 caso contrário
 */
int content_edit(ContentCatalog *catalog, int id, const char *title, 
                const char *category, int duration, int age_rating);

/**
 * @brief Busca conteúdos por título
 * 
 * @param catalog Ponteiro para o catálogo
 * @param title Título a ser buscado (parcial ou completo)
 * @param results Array para armazenar os IDs dos conteúdos encontrados
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de conteúdos encontrados
 */
int content_search_by_title(ContentCatalog *catalog, const char *title, 
                            int *results, int max_results);

/**
 * @brief Busca conteúdos por categoria
 * 
 * @param catalog Ponteiro para o catálogo
 * @param category Categoria a ser buscada
 * @param results Array para armazenar os IDs dos conteúdos encontrados
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de conteúdos encontrados
 */
int content_search_by_category(ContentCatalog *catalog, const char *category, 
                              int *results, int max_results);

/**
 * @brief Busca conteúdos por classificação etária
 * 
 * @param catalog Ponteiro para o catálogo
 * @param age_rating Classificação etária a ser buscada
 * @param results Array para armazenar os IDs dos conteúdos encontrados
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de conteúdos encontrados
 */
int content_search_by_age_rating(ContentCatalog *catalog, int age_rating, 
                                int *results, int max_results);

/**
 * @brief Incrementa o contador de visualizações de um conteúdo
 * 
 * @param catalog Ponteiro para o catálogo
 * @param id ID do conteúdo
 * @return int 1 se a operação foi bem-sucedida, 0 caso contrário
 */
int content_increment_views(ContentCatalog *catalog, int id);

/**
 * @brief Obtém um conteúdo pelo ID
 * 
 * @param catalog Ponteiro para o catálogo
 * @param id ID do conteúdo
 * @return Content* Ponteiro para o conteúdo ou NULL se não encontrado
 */
Content* content_get_by_id(ContentCatalog *catalog, int id);

#endif /* CONTENT_H */