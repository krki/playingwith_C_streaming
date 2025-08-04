/**
 * @file list.h
 * @brief Módulo para gerenciamento de listas personalizadas
 * 
 * Este módulo contém funções e estruturas para gerenciar listas personalizadas
 * criadas por usuários, permitindo que eles organizem conteúdos em coleções.
 */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "content.h"

#define MAX_LIST_NAME_LENGTH 100
#define MAX_LIST_ITEMS 100

/**
 * @brief Estrutura que representa uma lista personalizada
 */
typedef struct {
    int id;                             /**< ID da lista */
    int user_id;                        /**< ID do usuário proprietário */
    char name[MAX_LIST_NAME_LENGTH];    /**< Nome da lista */
    int content_ids[MAX_LIST_ITEMS];    /**< IDs dos conteúdos na lista */
    int count;                          /**< Número de conteúdos na lista */
} CustomList;

/**
 * @brief Estrutura que gerencia a coleção de listas personalizadas
 */
typedef struct {
    CustomList *lists;      /**< Array dinâmico de listas */
    int count;              /**< Número atual de listas */
    int capacity;           /**< Capacidade máxima do array */
} ListManager;

/**
 * @brief Inicializa o gerenciador de listas
 * 
 * @param manager Ponteiro para o gerenciador a ser inicializado
 * @param initial_capacity Capacidade inicial do gerenciador
 * @return int 1 se a inicialização foi bem-sucedida, 0 caso contrário
 */
int list_init_manager(ListManager *manager, int initial_capacity);

/**
 * @brief Liberta a memória alocada para o gerenciador de listas
 * 
 * @param manager Ponteiro para o gerenciador a ser libertado
 */
void list_free_manager(ListManager *manager);

/**
 * @brief Carrega listas de um arquivo CSV
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param filename Nome do arquivo CSV
 * @return int Número de listas carregadas ou -1 em caso de erro
 */
int list_load_from_csv(ListManager *manager, const char *filename);

/**
 * @brief Salva as listas em um arquivo CSV
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param filename Nome do arquivo CSV
 * @return int 1 se o salvamento foi bem-sucedido, 0 caso contrário
 */
int list_save_to_csv(ListManager *manager, const char *filename);

/**
 * @brief Cria uma nova lista personalizada
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param user_id ID do usuário proprietário
 * @param name Nome da lista
 * @return int ID da lista criada ou -1 em caso de erro
 */
int list_create(ListManager *manager, int user_id, const char *name);

/**
 * @brief Remove uma lista personalizada pelo ID
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param list_id ID da lista a ser removida
 * @return int 1 se a remoção foi bem-sucedida, 0 caso contrário
 */
int list_remove(ListManager *manager, int list_id);

/**
 * @brief Adiciona um conteúdo a uma lista
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param list_id ID da lista
 * @param content_id ID do conteúdo a ser adicionado
 * @return int 1 se a adição foi bem-sucedida, 0 caso contrário
 */
int list_add_content(ListManager *manager, int list_id, int content_id);

/**
 * @brief Remove um conteúdo de uma lista
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param list_id ID da lista
 * @param content_id ID do conteúdo a ser removido
 * @return int 1 se a remoção foi bem-sucedida, 0 caso contrário
 */
int list_remove_content(ListManager *manager, int list_id, int content_id);

/**
 * @brief Obtém uma lista pelo ID
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param list_id ID da lista
 * @return CustomList* Ponteiro para a lista ou NULL se não encontrada
 */
CustomList* list_get_by_id(ListManager *manager, int list_id);

/**
 * @brief Obtém todas as listas de um usuário
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param user_id ID do usuário
 * @param result_ids Array para armazenar os IDs das listas encontradas
 * @param max_results Tamanho máximo do array de resultados
 * @return int Número de listas encontradas
 */
int list_get_by_user(ListManager *manager, int user_id, int *result_ids, int max_results);

/**
 * @brief Renomeia uma lista personalizada
 * 
 * @param manager Ponteiro para o gerenciador de listas
 * @param list_id ID da lista
 * @param new_name Novo nome para a lista
 * @return int 1 se a renomeação foi bem-sucedida, 0 caso contrário
 */
int list_rename(ListManager *manager, int list_id, const char *new_name);

#endif /* LIST_H */