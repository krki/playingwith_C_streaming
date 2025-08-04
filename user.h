/**
 * @file user.h
 * @brief Módulo para gerenciamento de utilizadores e suas interações
 * 
 * Este módulo contém funções e estruturas para gerenciar os utilizadores
 * do sistema Streamflix e suas interações com os conteúdos disponíveis.
 */

#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "content.h"

#define MAX_USERNAME_LENGTH 50
#define MAX_INTERACTIONS 1000
#define MAX_INTERACTION_TYPE_LENGTH 20

/**
 * @brief Tipos de interação do utilizador com conteúdos
 */
typedef enum {
    INTERACTION_PLAY,       /**< Iniciar reprodução */
    INTERACTION_PAUSE,      /**< Pausar reprodução */
    INTERACTION_COMPLETE,   /**< Completar visualização */
    INTERACTION_FAVORITE    /**< Marcar como favorito */
} InteractionType;

/**
 * @brief Estrutura que representa uma interação do utilizador com um conteúdo
 */
typedef struct {
    int user_id;            /**< ID do utilizador */
    int content_id;         /**< ID do conteúdo */
    InteractionType type;   /**< Tipo de interação */
    time_t timestamp;       /**< Timestamp da interação */
} Interaction;

/**
 * @brief Estrutura que representa um utilizador
 */
typedef struct {
    int id;                            /**< ID do utilizador */
    char username[MAX_USERNAME_LENGTH]; /**< Nome de utilizador */
    int favorite_contents[100];        /**< IDs dos conteúdos favoritos */
    int favorite_count;                /**< Número de conteúdos favoritos */
    int interaction_count;             /**< Número de interações do utilizador */
} User;

/**
 * @brief Estrutura que gerencia a coleção de utilizadores
 */
typedef struct {
    User *users;            /**< Array dinâmico de utilizadores */
    int count;              /**< Número atual de utilizadores */
    int capacity;           /**< Capacidade máxima do array */
    Interaction *interactions; /**< Array dinâmico de interações */
    int interaction_count;  /**< Número atual de interações */
    int interaction_capacity; /**< Capacidade máxima do array de interações */
} UserManager;

/**
 * @brief Inicializa o gerenciador de utilizadores
 * 
 * @param manager Ponteiro para o gerenciador a ser inicializado
 * @param initial_user_capacity Capacidade inicial para utilizadores
 * @param initial_interaction_capacity Capacidade inicial para interações
 * @return int 1 se a inicialização foi bem-sucedida, 0 caso contrário
 */
int user_init_manager(UserManager *manager, int initial_user_capacity, 
                     int initial_interaction_capacity);

/**
 * @brief Liberta a memória alocada para o gerenciador de utilizadores
 * 
 * @param manager Ponteiro para o gerenciador a ser libertado
 */
void user_free_manager(UserManager *manager);

/**
 * @brief Carrega utilizadores de um arquivo CSV
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param filename Nome do arquivo CSV
 * @return int Número de utilizadores carregados ou -1 em caso de erro
 */
int user_load_from_csv(UserManager *manager, const char *filename);

/**
 * @brief Salva os utilizadores em um arquivo CSV
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param filename Nome do arquivo CSV
 * @return int 1 se o salvamento foi bem-sucedido, 0 caso contrário
 */
int user_save_to_csv(UserManager *manager, const char *filename);

/**
 * @brief Carrega interações de um arquivo CSV
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param filename Nome do arquivo CSV
 * @return int Número de interações carregadas ou -1 em caso de erro
 */
int user_load_interactions_from_csv(UserManager *manager, const char *filename);

/**
 * @brief Salva as interações em um arquivo CSV
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param filename Nome do arquivo CSV
 * @return int 1 se o salvamento foi bem-sucedido, 0 caso contrário
 */
int user_save_interactions_to_csv(UserManager *manager, const char *filename);

/**
 * @brief Adiciona um novo utilizador
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param username Nome de utilizador
 * @return int ID do utilizador adicionado ou -1 em caso de erro
 */
int user_add(UserManager *manager, const char *username);

/**
 * @brief Remove um utilizador pelo ID
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param user_id ID do utilizador a ser removido
 * @return int 1 se a remoção foi bem-sucedida, 0 caso contrário
 */
int user_remove(UserManager *manager, int user_id);

/**
 * @brief Registra uma interação de um utilizador com um conteúdo
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param user_id ID do utilizador
 * @param content_id ID do conteúdo
 * @param type Tipo de interação
 * @return int 1 se o registro foi bem-sucedido, 0 caso contrário
 */
int user_register_interaction(UserManager *manager, int user_id, int content_id, 
                              InteractionType type);

/**
 * @brief Adiciona um conteúdo aos favoritos de um utilizador
 * 
 * @param manager Ponteiro para o gerenciador de utilizador
 * @param user_id ID do utilizador
 * @param content_id ID do conteúdo
 * @return int 1 se a adição foi bem-sucedida, 0 caso contrário
 */
int user_add_favorite(UserManager *manager, int user_id, int content_id);

/**
 * @brief Remove um conteúdo dos favoritos de um utilizador
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param user_id ID do utilizador
 * @param content_id ID do conteúdo
 * @return int 1 se a remoção foi bem-sucedida, 0 caso contrário
 */
int user_remove_favorite(UserManager *manager, int user_id, int content_id);

/**
 * @brief Obtém um utilizador pelo ID
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param user_id ID do utilizador
 * @return User* Ponteiro para o utilizador ou NULL se não encontrado
 */
User* user_get_by_id(UserManager *manager, int user_id);

/**
 * @brief Obtém um utilizador pelo nome de utilizador
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param username Nome de utilizador
 * @return User* Ponteiro para o utilizador ou NULL se não encontrado
 */
User* user_get_by_username(UserManager *manager, const char *username);

/**
 * @brief Obtém o número de interações de um utilizador
 * 
 * @param manager Ponteiro para o gerenciador de utilizadores
 * @param user_id ID do utilizador
 * @return int Número de interações do utilizador
 */
int user_get_interaction_count(UserManager *manager, int user_id);

/**
 * @brief Converte um tipo de interação em string
 * 
 * @param type Tipo de interação
 * @param buffer Buffer para armazenar a string
 * @param size Tamanho do buffer
 */
void user_interaction_type_to_string(InteractionType type, char *buffer, size_t size);

/**
 * @brief Converte uma string em tipo de interação
 * 
 * @param str String representando o tipo de interação
 * @return InteractionType Tipo de interação correspondente
 */
InteractionType user_interaction_type_from_string(const char *str);

#endif /* USER_H */