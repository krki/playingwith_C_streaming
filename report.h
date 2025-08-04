/**
 * @file report.h
 * @brief Módulo para geração de relatórios estatísticos
 * 
 * Este módulo contém funções para gerar relatórios sobre os conteúdos
 * mais assistidos, categorias mais populares e utilizadores mais ativos.
 */

#ifndef REPORT_H
#define REPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "content.h"
#include "user.h"

/**
 * @brief Estrutura que representa um item em um relatório de conteúdos
 */
typedef struct {
    int content_id;        /**< ID do conteúdo */
    char title[MAX_TITLE_LENGTH]; /**< Título do conteúdo */
    int count;             /**< Contagem (visualizações, etc.) */
} ContentReportItem;

/**
 * @brief Estrutura que representa um item em um relatório de categorias
 */
typedef struct {
    char category[MAX_CATEGORY_LENGTH]; /**< Nome da categoria */
    int count;             /**< Contagem (visualizações, etc.) */
} CategoryReportItem;

/**
 * @brief Estrutura que representa um item em um relatório de utilizadores
 */
typedef struct {
    int user_id;           /**< ID do utilizador */
    char username[MAX_USERNAME_LENGTH]; /**< Nome do utilizador */
    int count;             /**< Contagem (interações, etc.) */
} UserReportItem;

/**
 * @brief Gera um relatório dos conteúdos mais assistidos
 * 
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param results Array para armazenar os itens do relatório
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de itens gerados no relatório
 */
int report_most_viewed_contents(ContentCatalog *content_catalog, 
                               ContentReportItem *results, 
                               int max_results);

/**
 * @brief Gera um relatório das categorias mais populares
 * 
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param results Array para armazenar os itens do relatório
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de itens gerados no relatório
 */
int report_most_popular_categories(ContentCatalog *content_catalog, 
                                  CategoryReportItem *results, 
                                  int max_results);

/**
 * @brief Gera um relatório dos utilizadores mais ativos
 * 
 * @param user_manager Ponteiro para o gerenciador de utilizadores
 * @param results Array para armazenar os itens do relatório
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de itens gerados no relatório
 */
int report_most_active_users(UserManager *user_manager, 
                            UserReportItem *results, 
                            int max_results);

/**
 * @brief Gera um relatório das interações de um utilizador específico
 * 
 * @param user_manager Ponteiro para o gerenciador de utilizadores
 * @param content_catalog Ponteiro para o catálogo de conteúdos
 * @param user_id ID do utilizador
 * @param results Array para armazenar os itens do relatório
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de itens gerados no relatório
 */
int report_user_interactions(UserManager *user_manager, 
                            ContentCatalog *content_catalog,
                            int user_id, 
                            ContentReportItem *results, 
                            int max_results);

/**
 * @brief Exporta um relatório para um arquivo CSV
 * 
 * @param filename Nome do arquivo CSV
 * @param headers Array de strings com os cabeçalhos das colunas
 * @param header_count Número de cabeçalhos
 * @param data Array bidimensional de strings com os dados
 * @param row_count Número de linhas de dados
 * @return int 1 se a exportação foi bem-sucedida, 0 caso contrário
 */
int report_export_to_csv(const char *filename, 
                        char **headers, int header_count,
                        char ***data, int row_count);

/**
 * @brief Imprime um relatório no console
 * 
 * @param title Título do relatório
 * @param headers Array de strings com os cabeçalhos das colunas
 * @param header_count Número de cabeçalhos
 * @param data Array bidimensional de strings com os dados
 * @param row_count Número de linhas de dados
 */
void report_print(const char *title, 
                 char **headers, int header_count,
                 char ***data, int row_count);

#endif /* REPORT_H */