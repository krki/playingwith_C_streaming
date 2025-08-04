/**
 * @file main.c
 * @brief Módulo principal do programa Streamflix
 * 
 * Este módulo contém a função main e a lógica de controle principal do programa,
 * orquestrando a utilização dos demais módulos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "csvutil.h"
#include "content.h"
#include "user.h"
#include "list.h"
#include "recommendation.h"
#include "report.h"

// Arquivo padrão de dados
#define CONTENT_FILE "contents.csv"
#define USER_FILE "users.csv"
#define INTERACTION_FILE "interactions.csv"
#define LIST_FILE "lists.csv"

// Capacidades iniciais dos gerenciadores
#define INITIAL_CONTENT_CAPACITY 100
#define INITIAL_USER_CAPACITY 100
#define INITIAL_INTERACTION_CAPACITY 1000
#define INITIAL_LIST_CAPACITY 100

// Tamanho máximo dos arrays de resultados
#define MAX_SEARCH_RESULTS 100
#define MAX_REPORT_RESULTS 20

// Protótipos das funções do menu principal
void show_main_menu();
void content_management_menu(ContentCatalog *catalog);
void user_management_menu(UserManager *user_manager, ContentCatalog *content_catalog);
void list_management_menu(ListManager *list_manager, UserManager *user_manager, ContentCatalog *content_catalog);
void recommendation_menu(UserManager *user_manager, ContentCatalog *content_catalog);
void report_menu(UserManager *user_manager, ContentCatalog *content_catalog, ListManager *list_manager);

// Protótipos das funções auxiliares
void pause_screen();
int get_user_choice();
void clear_screen();
void save_data(ContentCatalog *content_catalog, UserManager *user_manager, ListManager *list_manager);

/**
 * @brief Função principal do programa
 * 
 * @return int Código de saída
 */
int main() {
    // Inicializar seed para geração de números aleatórios
    srand((unsigned int)time(NULL));
    
    // Inicializar catálogo de conteúdos
    ContentCatalog content_catalog;
    if (!content_init_catalog(&content_catalog, INITIAL_CONTENT_CAPACITY)) {
        printf("Erro ao iniciar o catalogo de conteudos.\n");
        return 1;
    }
    
    // Inicializar gerenciador de utilizadores
    UserManager user_manager;
    if (!user_init_manager(&user_manager, INITIAL_USER_CAPACITY, INITIAL_INTERACTION_CAPACITY)) {
        printf("Erro ao inicializar o gerenciador de utilizadores.\n");
        content_free_catalog(&content_catalog);
        return 1;
    }
    
    // Inicializar gerenciador de listas
    ListManager list_manager;
    if (!list_init_manager(&list_manager, INITIAL_LIST_CAPACITY)) {
        printf("Erro ao inicializar o gerenciador de listas.\n");
        content_free_catalog(&content_catalog);
        user_free_manager(&user_manager);
        return 1;
    }
    
    // Carregar dados dos arquivos CSV
    printf("Carregando dados...\n");
    
    int content_count = content_load_from_csv(&content_catalog, CONTENT_FILE);
    if (content_count >= 0) {
        printf("%d conteudos carregados.\n", content_count);
    } else {
        printf("Aviso: Nao foi possivel carregar o arquivo de conteudos. Um novo sera criado.\n");
    }
    
    int user_count = user_load_from_csv(&user_manager, USER_FILE);
    if (user_count >= 0) {
        printf("%d utilizadores carregados.\n", user_count);
    } else {
        printf("Aviso: Nao foi possivel carregar o arquivo de utilizadores. Um novo sera criado.\n");
    }
    
    int interaction_count = user_load_interactions_from_csv(&user_manager, INTERACTION_FILE);
    if (interaction_count >= 0) {
        printf("%d interacoes carregadas.\n", interaction_count);
    } else {
        printf("Aviso: Nao foi possivel carregar o arquivo de interacoes. Um novo sera criado.\n");
    }
    
    int list_count = list_load_from_csv(&list_manager, LIST_FILE);
    if (list_count >= 0) {
        printf("%d listas carregadas.\n", list_count);
    } else {
        printf("Aviso: Nao foi possivel carregar o arquivo de listas. Um novo sera criado.\n");
    }
    
    pause_screen();
    
    // Loop principal do programa
    int option;
    int running = 1;
    
    while (running) {
        clear_screen();
        show_main_menu();
        option = get_user_choice();
        
        switch (option) {
            case 1:
                content_management_menu(&content_catalog);
                break;
            case 2:
                user_management_menu(&user_manager, &content_catalog);
                break;
            case 3:
                list_management_menu(&list_manager, &user_manager, &content_catalog);
                break;
            case 4:
                recommendation_menu(&user_manager, &content_catalog);
                break;
            case 5:
                report_menu(&user_manager, &content_catalog, &list_manager);
                break;
            case 6:
                save_data(&content_catalog, &user_manager, &list_manager);
                break;
            case 0:
                printf("Salvando dados antes de sair...\n");
                save_data(&content_catalog, &user_manager, &list_manager);
                printf("Obrigado por usar o Streamflix!\n");
                running = 0;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pause_screen();
                break;
        }
    }
    
    // Liberar memória
    content_free_catalog(&content_catalog);
    user_free_manager(&user_manager);
    list_free_manager(&list_manager);
    
    return 0;
}

void show_main_menu() {
    printf("========================================\n");
    printf("            STREAMFLIX                  \n");
    printf("========================================\n");
    printf("[1] Gerenciamento de Conteudos\n");
    printf("[2] Gerenciamento de Utilizadores\n");
    printf("[3] Gerenciamento de Listas\n");
    printf("[4] Recomendacoes\n");
    printf("[5] Relatorios\n");
    printf("[6] Salvar Dados\n");
    printf("[0] Sair\n");
    printf("----------------------------------------\n");
    printf("Escolha uma opcao: ");
}

void content_management_menu(ContentCatalog *catalog) {
    int option;
    int running = 1;
    
    while (running) {
        clear_screen();
        printf("========================================\n");
        printf("     GERENCIAMENTO DE CONTEUDOS         \n");
        printf("========================================\n");
        printf("[1] Listar Conteudos\n");
        printf("[2] Adicionar Conteudo\n");
        printf("[3] Editar Conteudo\n");
        printf("[4] Remover Conteudo\n");
        printf("[5] Pesquisar por Titulo\n");
        printf("[6] Pesquisar por Categoria\n");
        printf("[7] Pesquisar por Classificacao Etaria\n");
        printf("[0] Voltar\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");
        
        option = get_user_choice();
        
        switch (option) {
            case 1: {
                // Listar conteúdos
                clear_screen();
                printf("Lista de Conteudos (%d)\n", catalog->count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < catalog->count; i++) {
                    Content *content = &catalog->items[i];
                    printf("[ID: %d] %s\n", content->id, content->title);
                    printf("  Categoria: %s | Duracao: %d min | Classificacao: %d | Visualizacoes: %d\n", 
                           content->category, content->duration, content->age_rating, content->views);
                    printf("----------------------------------------\n");
                }
                
                pause_screen();
                break;
            }
            case 2: {
                // Adicionar conteúdo
                clear_screen();
                printf("Adicionar Novo Conteudo\n");
                printf("----------------------------------------\n");
                
                char title[MAX_TITLE_LENGTH];
                char category[MAX_CATEGORY_LENGTH];
                int duration, age_rating;
                
                printf("Titulo: ");
                fgets(title, MAX_TITLE_LENGTH, stdin);
                title[strcspn(title, "\n")] = 0; // Remover quebra de linha
                
                printf("Categoria: ");
                fgets(category, MAX_CATEGORY_LENGTH, stdin);
                category[strcspn(category, "\n")] = 0;
                
                printf("Duracao (minutos): ");
                scanf("%d", &duration);
                getchar(); // Consumir quebra de linha
                
                printf("Classificacao Etaria: ");
                scanf("%d", &age_rating);
                getchar(); // Consumir quebra de linha
                
                int result = content_add(catalog, title, category, duration, age_rating);
                
                if (result > 0) {
                    printf("Conteudo adicionado com sucesso! ID: %d\n", result);
                } else {
                    printf("Erro ao adicionar o conteudo.\n");
                }
                
                pause_screen();
                break;
            }
            case 3: {
                // Editar conteúdo
                clear_screen();
                printf("Editar Conteudo\n");
                printf("----------------------------------------\n");
                
                int id;
                printf("Digite o ID do conteudo a ser editado: ");
                scanf("%d", &id);
                getchar(); // Consumir quebra de linha
                
                Content *content = content_get_by_id(catalog, id);
                if (content == NULL) {
                    printf("Conteudo nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Conteudo atual: %s\n", content->title);
                printf("Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                       content->category, content->duration, content->age_rating);
                printf("----------------------------------------\n");
                
                char title[MAX_TITLE_LENGTH];
                char category[MAX_CATEGORY_LENGTH];
                int duration, age_rating;
                
                printf("Novo titulo (deixe em branco para manter o atual): ");
                fgets(title, MAX_TITLE_LENGTH, stdin);
                title[strcspn(title, "\n")] = 0;
                
                printf("Nova categoria (deixe em branco para manter a atual): ");
                fgets(category, MAX_CATEGORY_LENGTH, stdin);
                category[strcspn(category, "\n")] = 0;
                
                printf("Nova duracao (0 para manter a atual): ");
                scanf("%d", &duration);
                getchar();
                
                printf("Nova classificacao etaria (0 para manter a atual): ");
                scanf("%d", &age_rating);
                getchar();
                
                int result = content_edit(catalog, id, 
                                         strlen(title) > 0 ? title : NULL, 
                                         strlen(category) > 0 ? category : NULL, 
                                         duration, age_rating);
                
                if (result) {
                    printf("Conteudo atualizado com sucesso!\n");
                } else {
                    printf("Erro ao atualizar o conteudo.\n");
                }
                
                pause_screen();
                break;
            }
            case 4: {
                // Remover conteúdo
                clear_screen();
                printf("Remover Conteudo\n");
                printf("----------------------------------------\n");
                
                int id;
                printf("Digite o ID do conteudo a ser removido: ");
                scanf("%d", &id);
                getchar(); // Consumir quebra de linha
                
                Content *content = content_get_by_id(catalog, id);
                if (content == NULL) {
                    printf("Conteudo nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Tem certeza que deseja remover o conteudo '%s'? (S/N): ", content->title);
                char confirmation;
                scanf("%c", &confirmation);
                getchar();
                
                if (confirmation == 'S' || confirmation == 's') {
                    int result = content_remove(catalog, id);
                    
                    if (result) {
                        printf("Conteudo removido com sucesso!\n");
                    } else {
                        printf("Erro ao remover o conteudo.\n");
                    }
                } else {
                    printf("Operacao cancelada.\n");
                }
                
                pause_screen();
                break;
            }
            case 5: {
                // Pesquisar por título
                clear_screen();
                printf("Pesquisar por Titulo\n");
                printf("----------------------------------------\n");
                
                char title[MAX_TITLE_LENGTH];
                printf("Digite o titulo ou parte dele: ");
                fgets(title, MAX_TITLE_LENGTH, stdin);
                title[strcspn(title, "\n")] = 0;
                
                int results[MAX_SEARCH_RESULTS];
                int count = content_search_by_title(catalog, title, results, MAX_SEARCH_RESULTS);
                
                printf("\nResultados da pesquisa (%d encontrados):\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(catalog, results[i]);
                    printf("[ID: %d] %s\n", content->id, content->title);
                    printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                           content->category, content->duration, content->age_rating);
                    printf("----------------------------------------\n");
                }
                
                pause_screen();
                break;
            }
            case 6: {
                // Pesquisar por categoria
                clear_screen();
                printf("Pesquisar por Categoria\n");
                printf("----------------------------------------\n");
                
                char category[MAX_CATEGORY_LENGTH];
                printf("Digite a categoria: ");
                fgets(category, MAX_CATEGORY_LENGTH, stdin);
                category[strcspn(category, "\n")] = 0;
                
                int results[MAX_SEARCH_RESULTS];
                int count = content_search_by_category(catalog, category, results, MAX_SEARCH_RESULTS);
                
                printf("\nResultados da pesquisa (%d encontrados):\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(catalog, results[i]);
                    printf("[ID: %d] %s\n", content->id, content->title);
                    printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                           content->category, content->duration, content->age_rating);
                    printf("----------------------------------------\n");
                }
                
                pause_screen();
                break;
            }
            case 7: {
                // Pesquisar por classificação etária
                clear_screen();
                printf("Pesquisar por Classificação Etaria\n");
                printf("----------------------------------------\n");
                
                int age_rating;
                printf("Digite a classificacao etaria: ");
                scanf("%d", &age_rating);
                getchar();
                
                int results[MAX_SEARCH_RESULTS];
                int count = content_search_by_age_rating(catalog, age_rating, results, MAX_SEARCH_RESULTS);
                
                printf("\nResultados da pesquisa (%d encontrados):\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(catalog, results[i]);
                    printf("[ID: %d] %s\n", content->id, content->title);
                    printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                           content->category, content->duration, content->age_rating);
                    printf("----------------------------------------\n");
                }
                
                pause_screen();
                break;
            }
            case 0:
                running = 0;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pause_screen();
                break;
        }
    }
}

void user_management_menu(UserManager *user_manager, ContentCatalog *content_catalog) {
    int option;
    int running = 1;
    
    while (running) {
        clear_screen();
        printf("========================================\n");
        printf("     GERENCIAMENTO DE UTILIZADORES          \n");
        printf("========================================\n");
        printf("[1] Listar Utilizadores\n");
        printf("[2] Adicionar Utilizador\n");
        printf("[3] Remover Utilizador\n");
        printf("[4] Registrar Interacao\n");
        printf("[5] Ver Favoritos do Utilizador\n");
        printf("[6] Adicionar aos Favoritos\n");
        printf("[7] Remover dos Favoritos\n");
        printf("[0] Voltar\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");
        
        option = get_user_choice();
        
        switch (option) {
            case 1: {
                // Listar utilizadores
                clear_screen();
                printf("Lista de Utilizadores (%d)\n", user_manager->count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < user_manager->count; i++) {
                    User *user = &user_manager->users[i];
                    printf("[ID: %d] %s\n", user->id, user->username);
                    printf("  Interacoes: %d | Favoritos: %d\n", 
                           user->interaction_count, user->favorite_count);
                    printf("----------------------------------------\n");
                }
                
                pause_screen();
                break;
            }
            case 2: {
                // Adicionar utilizador
                clear_screen();
                printf("Adicionar Novo Utilizador\n");
                printf("----------------------------------------\n");
                
                char username[MAX_USERNAME_LENGTH];
                printf("Nome de utilizador: ");
                fgets(username, MAX_USERNAME_LENGTH, stdin);
                username[strcspn(username, "\n")] = 0;
                
                int result = user_add(user_manager, username);
                
                if (result > 0) {
                    printf("Utilizador adicionado com sucesso! ID: %d\n", result);
                } else {
                    printf("Erro ao adicionar o utilizador.\n");
                }
                
                pause_screen();
                break;
            }
            case 3: {
                // Remover utilizador
                clear_screen();
                printf("Remover Utilizador\n");
                printf("----------------------------------------\n");
                
                int id;
                printf("Digite o ID do utilizador a ser removido: ");
                scanf("%d", &id);
                getchar();
                
                User *user = user_get_by_id(user_manager, id);
                if (user == NULL) {
                    printf("Utilizador não encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Tem certeza que deseja remover o utilizador '%s'? (S/N): ", user->username);
                char confirmation;
                scanf("%c", &confirmation);
                getchar();
                
                if (confirmation == 'S' || confirmation == 's') {
                    int result = user_remove(user_manager, id);
                    
                    if (result) {
                        printf("Utilizador removido com sucesso!\n");
                    } else {
                        printf("Erro ao remover o utilizador.\n");
                    }
                } else {
                    printf("Operacao cancelada.\n");
                }
                
                pause_screen();
                break;
            }
            case 4: {
                // Registrar interação
                clear_screen();
                printf("Registrar Interacao\n");
                printf("----------------------------------------\n");
                
                int user_id, content_id;
                int interaction_type;
                
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                if (user_get_by_id(user_manager, user_id) == NULL) {
                    printf("Utilizador não encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("ID do conteudo: ");
                scanf("%d", &content_id);
                getchar();
                
                if (content_get_by_id(content_catalog, content_id) == NULL) {
                    printf("Conteudo nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Tipo de interacao:\n");
                printf("[1] Reproduzir\n");
                printf("[2] Parar\n");
                printf("[3] Completar Visualizacao\n");
                printf("[4] Marcar como Favorito\n");
                printf("Escolha: ");
                scanf("%d", &interaction_type);
                getchar();
                
                InteractionType type;
                switch (interaction_type) {
                    case 1: type = INTERACTION_PLAY; break;
                    case 2: type = INTERACTION_PAUSE; break;
                    case 3: type = INTERACTION_COMPLETE; break;
                    case 4: type = INTERACTION_FAVORITE; break;
                    default:
                        printf("Tipo de interacao invalido.\n");
                        pause_screen();
                        break;
                }
                
                int result = user_register_interaction(user_manager, user_id, content_id, type);
                
                if (result) {
                    printf("Interacao registrada com sucesso!\n");
                    
                    // Incrementar visualizações se for do tipo PLAY ou COMPLETE
                    if (type == INTERACTION_PLAY || type == INTERACTION_COMPLETE) {
                        content_increment_views(content_catalog, content_id);
                    }
                } else {
                    printf("Erro ao registrar a interacao.\n");
                }
                
                pause_screen();
                break;
            }
            case 5: {
                // Ver favoritos do utilizador
                clear_screen();
                printf("Favoritos do Utilizador\n");
                printf("----------------------------------------\n");
                
                int user_id;
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador não encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Favoritos de %s (%d):\n", user->username, user->favorite_count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < user->favorite_count; i++) {
                    Content *content = content_get_by_id(content_catalog, user->favorite_contents[i]);
                    if (content != NULL) {
                        printf("[%d] %s\n", content->id, content->title);
                        printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                               content->category, content->duration, content->age_rating);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 6: {
                // Adicionar aos favoritos
                clear_screen();
                printf("Adicionar aos Favoritos\n");
                printf("----------------------------------------\n");
                
                int user_id, content_id;
                
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                if (user_get_by_id(user_manager, user_id) == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("ID do conteudo: ");
                scanf("%d", &content_id);
                getchar();
                
                if (content_get_by_id(content_catalog, content_id) == NULL) {
                    printf("Conteudo nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                int result = user_add_favorite(user_manager, user_id, content_id);
                
                if (result) {
                    printf("Conteudo adicionado aos favoritos com sucesso!\n");
                } else {
                    printf("Erro ao adicionar o conteudo aos favoritos.\n");
                }
                
                pause_screen();
                break;
            }
            case 7: {
                // Remover dos favoritos
                clear_screen();
                printf("Remover dos Favoritos\n");
                printf("----------------------------------------\n");
                
                int user_id, content_id;
                
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Favoritos atuais:\n");
                for (int i = 0; i < user->favorite_count; i++) {
                    Content *content = content_get_by_id(content_catalog, user->favorite_contents[i]);
                    if (content != NULL) {
                        printf("[%d] %s\n", content->id, content->title);
                    }
                }
                
                printf("\nID do conteudo a remover: ");
                scanf("%d", &content_id);
                getchar();
                
                int result = user_remove_favorite(user_manager, user_id, content_id);
                
                if (result) {
                    printf("Conteudo removido dos favoritos com sucesso!\n");
                } else {
                    printf("Erro ao remover o conteudo dos favoritos.\n");
                }
                
                pause_screen();
                break;
            }
            case 0:
                running = 0;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pause_screen();
                break;
        }
    }
}

void list_management_menu(ListManager *list_manager, UserManager *user_manager, ContentCatalog *content_catalog) {
    int option;
    int running = 1;
    
    while (running) {
        clear_screen();
        printf("========================================\n");
        printf("     GERENCIAMENTO DE LISTAS            \n");
        printf("========================================\n");
        printf("[1] Listar Todas as Listas\n");
        printf("[2] Listar Listas de um Utilizador\n");
        printf("[3] Criar Nova Lista\n");
        printf("[4] Renomear Lista\n");
        printf("[5] Remover Lista\n");
        printf("[6] Adicionar Conteudo a Lista\n");
        printf("[7] Remover Conteudo da Lista\n");
        printf("[8] Ver Conteudos de uma Lista\n");
        printf("[0] Voltar\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");
        
        option = get_user_choice();
        
        switch (option) {
            case 1: {
                // Listar todas as listas
                clear_screen();
                printf("Todas as Listas (%d)\n", list_manager->count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < list_manager->count; i++) {
                    CustomList *list = &list_manager->lists[i];
                    User *user = user_get_by_id(user_manager, list->user_id);
                    
                    printf("[ID: %d] %s\n", list->id, list->name);
                    printf("  Utilizador: %s | Conteudos: %d\n", 
                           user != NULL ? user->username : "Desconhecido", list->count);
                    printf("----------------------------------------\n");
                }
                
                pause_screen();
                break;
            }
            case 2: {
                // Listar listas de um utilizador
                clear_screen();
                printf("Listas de um Utilizador\n");
                printf("----------------------------------------\n");
                
                int user_id;
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                int list_ids[MAX_SEARCH_RESULTS];
                int count = list_get_by_user(list_manager, user_id, list_ids, MAX_SEARCH_RESULTS);
                
                printf("Listas de %s (%d):\n", user->username, count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    CustomList *list = list_get_by_id(list_manager, list_ids[i]);
                    if (list != NULL) {
                        printf("[ID: %d] %s\n", list->id, list->name);
                        printf("  Conteudos: %d\n", list->count);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 3: {
				
				// Criar nova lista
                clear_screen();
                printf("Criar Nova Lista\n");
                printf("----------------------------------------\n");
                
                int user_id;
                char name[MAX_LIST_NAME_LENGTH];
                
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                if (user_get_by_id(user_manager, user_id) == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                printf("Nome da lista: ");
                fgets(name, MAX_LIST_NAME_LENGTH, stdin);
                name[strcspn(name, "\n")] = 0;
                
                int result = list_create(list_manager, user_id, name);
                
                if (result > 0) {
                    printf("Lista criada com sucesso! ID: %d\n", result);
                } else {
                    printf("Erro ao criar a lista.\n");
                }
                
                pause_screen();
                break;
            }
            case 4: {
                // Renomear lista
                clear_screen();
                printf("Renomear Lista\n");
                printf("----------------------------------------\n");
                
                int list_id;
                char new_name[MAX_LIST_NAME_LENGTH];
                
                printf("ID da lista: ");
                scanf("%d", &list_id);
                getchar();
                
                CustomList *list = list_get_by_id(list_manager, list_id);
                if (list == NULL) {
                    printf("Lista nao encontrada.\n");
                    pause_screen();
                    break;
                }
                
                printf("Nome atual: %s\n", list->name);
                printf("Novo nome: ");
                fgets(new_name, MAX_LIST_NAME_LENGTH, stdin);
                new_name[strcspn(new_name, "\n")] = 0;
                
                int result = list_rename(list_manager, list_id, new_name);
                
                if (result) {
                    printf("Lista renomeada com sucesso!\n");
                } else {
                    printf("Erro ao renomear a lista.\n");
                }
                
                pause_screen();
                break;
            }
            case 5: {
                // Remover lista
                clear_screen();
                printf("Remover Lista\n");
                printf("----------------------------------------\n");
                
                int list_id;
                printf("ID da lista a ser removida: ");
                scanf("%d", &list_id);
                getchar();
                
                CustomList *list = list_get_by_id(list_manager, list_id);
                if (list == NULL) {
                    printf("Lista nao encontrada.\n");
                    pause_screen();
                    break;
                }
                
                printf("Tem certeza que deseja remover a lista '%s'? (S/N): ", list->name);
                char confirmation;
                scanf("%c", &confirmation);
                getchar();
                
                if (confirmation == 'S' || confirmation == 's') {
                    int result = list_remove(list_manager, list_id);
                    
                    if (result) {
                        printf("Lista removida com sucesso!\n");
                    } else {
                        printf("Erro ao remover a lista.\n");
                    }
                } else {
                    printf("Operacao cancelada.\n");
                }
                
                pause_screen();
                break;
            }
            case 6: {
                // Adicionar conteúdo à lista
                clear_screen();
                printf("Adicionar Conteudo à Lista\n");
                printf("----------------------------------------\n");
                
                int list_id, content_id;
                
                printf("ID da lista: ");
                scanf("%d", &list_id);
                getchar();
                
                CustomList *list = list_get_by_id(list_manager, list_id);
                if (list == NULL) {
                    printf("Lista nao encontrada.\n");
                    pause_screen();
                    break;
                }
                
                printf("ID do conteudo: ");
                scanf("%d", &content_id);
                getchar();
                
                if (content_get_by_id(content_catalog, content_id) == NULL) {
                    printf("Conteudo nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                int result = list_add_content(list_manager, list_id, content_id);
                
                if (result) {
                    printf("Conteudo adicionado a lista com sucesso!\n");
                } else {
                    printf("Erro ao adicionar o conteudo a lista.\n");
                }
                
                pause_screen();
                break;
            }
            case 7: {
                // Remover conteúdo da lista
                clear_screen();
                printf("Remover Conteudo da Lista\n");
                printf("----------------------------------------\n");
                
                int list_id, content_id;
                
                printf("ID da lista: ");
                scanf("%d", &list_id);
                getchar();
                
                CustomList *list = list_get_by_id(list_manager, list_id);
                if (list == NULL) {
                    printf("Lista nao encontrada.\n");
                    pause_screen();
                    break;
                }
                
                printf("Conteudos da lista '%s':\n", list->name);
                for (int i = 0; i < list->count; i++) {
                    Content *content = content_get_by_id(content_catalog, list->content_ids[i]);
                    if (content != NULL) {
                        printf("[%d] %s\n", content->id, content->title);
                    }
                }
                
                printf("\nID do conteudo a remover: ");
                scanf("%d", &content_id);
                getchar();
                
                int result = list_remove_content(list_manager, list_id, content_id);
                
                if (result) {
                    printf("Conteudo removido da lista com sucesso!\n");
                } else {
                    printf("Erro ao remover o conteudo da lista.\n");
                }
                
                pause_screen();
                break;
            }
            case 8: {
                // Ver conteúdos de uma lista
                clear_screen();
                printf("Conteudos de uma Lista\n");
                printf("----------------------------------------\n");
                
                int list_id;
                printf("ID da lista: ");
                scanf("%d", &list_id);
                getchar();
                
                CustomList *list = list_get_by_id(list_manager, list_id);
                if (list == NULL) {
                    printf("Lista nao encontrada.\n");
                    pause_screen();
                    break;
                }
                
                User *user = user_get_by_id(user_manager, list->user_id);
                
                printf("Lista: %s\n", list->name);
                printf("Utilizador: %s\n", user != NULL ? user->username : "Desconhecido");
                printf("Conteudos (%d):\n", list->count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < list->count; i++) {
                    Content *content = content_get_by_id(content_catalog, list->content_ids[i]);
                    if (content != NULL) {
                        printf("[ID: %d] %s\n", content->id, content->title);
                        printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                               content->category, content->duration, content->age_rating);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 0:
                running = 0;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pause_screen();
                break;
        }
    }
}

void recommendation_menu(UserManager *user_manager, ContentCatalog *content_catalog) {
    int option;
    int running = 1;
    
    while (running) {
        clear_screen();
        printf("========================================\n");
        printf("            RECOMENDACOES              \n");
        printf("========================================\n");
        printf("[1] Recomendacoes por Conteudos Similares\n");
        printf("[2] Recomendacoes por Categoria\n");
        printf("[3] Conteudos Populares\n");
        printf("[4] Recomendacoes Personalizadas\n");
        printf("[0] Voltar\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");
        
        option = get_user_choice();
        
        switch (option) {
            case 1: {
                // Recomendações por conteúdos similares
                clear_screen();
                printf("Recomendacoes por Conteudos Similares\n");
                printf("----------------------------------------\n");
                
                int user_id;
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                int recommendations[MAX_RECOMMENDATIONS];
                int count = recommendation_by_content_similarity(
                    user_manager, content_catalog, user_id, recommendations, MAX_RECOMMENDATIONS);
                
                printf("\nRecomendacoes para %s (%d):\n", user->username, count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(content_catalog, recommendations[i]);
                    if (content != NULL) {
                        printf("[ID: %d] %s\n", content->id, content->title);
                        printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                               content->category, content->duration, content->age_rating);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 2: {
                // Recomendações por categoria
                clear_screen();
                printf("Recomendacoes por Categoria\n");
                printf("----------------------------------------\n");
                
                int user_id;
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                int recommendations[MAX_RECOMMENDATIONS];
                int count = recommendation_by_category(
                    user_manager, content_catalog, user_id, recommendations, MAX_RECOMMENDATIONS);
                
                printf("\nRecomendacoes para %s (%d):\n", user->username, count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(content_catalog, recommendations[i]);
                    if (content != NULL) {
                        printf("[ID: %d] %s\n", content->id, content->title);
                        printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                               content->category, content->duration, content->age_rating);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 3: {
                // Conteúdos populares
                clear_screen();
                printf("Conteudos Populares\n");
                printf("----------------------------------------\n");
                
                int recommendations[MAX_RECOMMENDATIONS];
                int count = recommendation_by_popularity(
                    content_catalog, recommendations, MAX_RECOMMENDATIONS);
                
                printf("\nConteudos Mais Populares (%d):\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(content_catalog, recommendations[i]);
                    if (content != NULL) {
                        printf("[ID: %d] %s\n", content->id, content->title);
                        printf("  Categoria: %s | Duracao: %d min | Classificacao: %d | Visualizacoes: %d\n", 
                               content->category, content->duration, content->age_rating, content->views);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 4: {
                // Recomendações personalizadas
                clear_screen();
                printf("Recomendacoes Personalizadas\n");
                printf("----------------------------------------\n");
                
                int user_id;
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                int recommendations[MAX_RECOMMENDATIONS];
                int count = recommendation_personalized(
                    user_manager, content_catalog, user_id, recommendations, MAX_RECOMMENDATIONS);
                
                printf("\nRecomendacoes Personalizadas para %s (%d):\n", user->username, count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    Content *content = content_get_by_id(content_catalog, recommendations[i]);
                    if (content != NULL) {
                        printf("[ID: %d] %s\n", content->id, content->title);
                        printf("  Categoria: %s | Duracao: %d min | Classificacao: %d\n", 
                               content->category, content->duration, content->age_rating);
                        printf("----------------------------------------\n");
                    }
                }
                
                pause_screen();
                break;
            }
            case 0:
                running = 0;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pause_screen();
                break;
        }
    }
}
void report_menu(UserManager *user_manager, ContentCatalog *content_catalog, ListManager *list_manager) {
    int option;
    int running = 1;
    
    while (running) {
        clear_screen();
        printf("========================================\n");
        printf("             RELATORIOS                 \n");
        printf("========================================\n");
        printf("[1] Conteudos Mais Assistidos\n");
        printf("[2] Categorias Mais Populares\n");
        printf("[3] Utilizadores Mais Ativos\n");
        printf("[4] Interações de Utilizador\n");
        printf("[5] Exportar Relatorio para CSV\n");
        printf("[0] Voltar\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");
        
        option = get_user_choice();
        
        switch (option) {
            case 1: {
                // Conteúdos mais assistidos
                clear_screen();
                printf("Conteudos Mais Assistidos\n");
                printf("----------------------------------------\n");
                
                ContentReportItem results[MAX_REPORT_RESULTS];
                int count = report_most_viewed_contents(content_catalog, results, MAX_REPORT_RESULTS);
                
                printf("\nTop %d Conteudos Mais Assistidos:\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    printf("%d. [ID: %d] %s - %d visualizacoes\n", 
                           i + 1, results[i].content_id, results[i].title, results[i].count);
                }
                
                pause_screen();
                break;
            }
            case 2: {
                // Categorias mais populares
                clear_screen();
                printf("Categorias Mais Populares\n");
                printf("----------------------------------------\n");
                
                CategoryReportItem results[MAX_REPORT_RESULTS];
                int count = report_most_popular_categories(content_catalog, results, MAX_REPORT_RESULTS);
                
                printf("\nTop %d Categorias Mais Populares:\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    printf("%d. %s - %d visualizacoes\n", 
                           i + 1, results[i].category, results[i].count);
                }
                
                pause_screen();
                break;
            }
            case 3: {
                // Utilizadores mais ativos
                clear_screen();
                printf("Utilizadores Mais Ativos\n");
                printf("----------------------------------------\n");
                
                UserReportItem results[MAX_REPORT_RESULTS];
                int count = report_most_active_users(user_manager, results, MAX_REPORT_RESULTS);
                
                printf("\nTop %d Utilizadores Mais Ativos:\n", count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    printf("%d. [ID: %d] %s - %d interacoes\n", 
                           i + 1, results[i].user_id, results[i].username, results[i].count);
                }
                
                pause_screen();
                break;
            }
            case 4: {
                // Interações de utilizador
                clear_screen();
                printf("Interacoes de Utilizador\n");
                printf("----------------------------------------\n");
                
                int user_id;
                printf("ID do utilizador: ");
                scanf("%d", &user_id);
                getchar();
                
                User *user = user_get_by_id(user_manager, user_id);
                if (user == NULL) {
                    printf("Utilizador nao encontrado.\n");
                    pause_screen();
                    break;
                }
                
                ContentReportItem results[MAX_REPORT_RESULTS];
                int count = report_user_interactions(
                    user_manager, content_catalog, user_id, results, MAX_REPORT_RESULTS);
                
                printf("\nInteracoes de %s (%d):\n", user->username, count);
                printf("----------------------------------------\n");
                
                for (int i = 0; i < count; i++) {
                    printf("%d. [ID: %d] %s - %d interacoes\n", 
                           i + 1, results[i].content_id, results[i].title, results[i].count);
                }
                
                pause_screen();
                break;
            }
            case 5: {
                // Exportar relatório para CSV
                clear_screen();
                printf("Exportar Relatorio para CSV\n");
                printf("----------------------------------------\n");
                
                printf("Tipo de relatorio:\n");
                printf("[1] Conteudos Mais Assistidos\n");
                printf("[2] Categorias Mais Populares\n");
                printf("[3] Utilizadores Mais Ativos\n");
                printf("Escolha: ");
                
                int report_type;
                scanf("%d", &report_type);
                getchar();
                
                char filename[100];
                printf("Nome do arquivo CSV: ");
                fgets(filename, 100, stdin);
                filename[strcspn(filename, "\n")] = 0;
                
                int result = 0;
                
                switch (report_type) {
                    case 1: {
                        // Conteúdos mais assistidos
                        ContentReportItem results[MAX_REPORT_RESULTS];
                        int count = report_most_viewed_contents(content_catalog, results, MAX_REPORT_RESULTS);
                        
                        // Preparar os dados para exportação
                        char *headers[3] = {"ID", "Título", "Visualizações"};
                        
                        char ***data = (char***)malloc(count * sizeof(char**));
                        for (int i = 0; i < count; i++) {
                            data[i] = (char**)malloc(3 * sizeof(char*));
                            
                            data[i][0] = (char*)malloc(20 * sizeof(char));
                            sprintf(data[i][0], "%d", results[i].content_id);
                            
                            data[i][1] = (char*)malloc(MAX_TITLE_LENGTH * sizeof(char));
                            strcpy(data[i][1], results[i].title);
                            
                            data[i][2] = (char*)malloc(20 * sizeof(char));
                            sprintf(data[i][2], "%d", results[i].count);
                        }
                        
                        result = report_export_to_csv(filename, headers, 3, data, count);
                        
                        // Liberar memória
                        for (int i = 0; i < count; i++) {
                            for (int j = 0; j < 3; j++) {
                                free(data[i][j]);
                            }
                            free(data[i]);
                        }
                        free(data);
                        
                        break;
                    }
                    case 2: {
                        // Categorias mais populares
                        CategoryReportItem results[MAX_REPORT_RESULTS];
                        int count = report_most_popular_categories(content_catalog, results, MAX_REPORT_RESULTS);
                        
                        // Preparar os dados para exportação
                        char *headers[2] = {"Categoria", "Visualizações"};
                        
                        char ***data = (char***)malloc(count * sizeof(char**));
                        for (int i = 0; i < count; i++) {
                            data[i] = (char**)malloc(2 * sizeof(char*));
                            
                            data[i][0] = (char*)malloc(MAX_CATEGORY_LENGTH * sizeof(char));
                            strcpy(data[i][0], results[i].category);
                            
                            data[i][1] = (char*)malloc(20 * sizeof(char));
                            sprintf(data[i][1], "%d", results[i].count);
                        }
                        
                        result = report_export_to_csv(filename, headers, 2, data, count);
                        
                        // Libertar memória
                        for (int i = 0; i < count; i++) {
                            for (int j = 0; j < 2; j++) {
                                free(data[i][j]);
                            }
                            free(data[i]);
                        }
                        free(data);
                        
                        break;
                    }
                    case 3: {
                        // Utilizadores mais ativos
                        UserReportItem results[MAX_REPORT_RESULTS];
                        int count = report_most_active_users(user_manager, results, MAX_REPORT_RESULTS);
                        
                        // Preparar os dados para exportação
                        char *headers[3] = {"ID", "Nome de Utilizador", "Interacoes"};
                        
                        char ***data = (char***)malloc(count * sizeof(char**));
                        for (int i = 0; i < count; i++) {
                            data[i] = (char**)malloc(3 * sizeof(char*));
                            
                            data[i][0] = (char*)malloc(20 * sizeof(char));
                            sprintf(data[i][0], "%d", results[i].user_id);
                            
                            data[i][1] = (char*)malloc(MAX_USERNAME_LENGTH * sizeof(char));
                            strcpy(data[i][1], results[i].username);
                            
                            data[i][2] = (char*)malloc(20 * sizeof(char));
                            sprintf(data[i][2], "%d", results[i].count);
                        }
                        
                        result = report_export_to_csv(filename, headers, 3, data, count);
                        
                        // Libertar memória
                        for (int i = 0; i < count; i++) {
                            for (int j = 0; j < 3; j++) {
                                free(data[i][j]);
                            }
                            free(data[i]);
                        }
                        free(data);
                        
                        break;
                    }
                    default:
                        printf("Tipo de relatorio invalido.\n");
                        pause_screen();
                        break;
                }
                
                if (result) {
                    printf("Relatorio exportado com sucesso para '%s'!\n", filename);
                } else {
                    printf("Erro ao exportar o relatorio.\n");
                }
                
                pause_screen();
                break;
            }
            case 0:
                running = 0;
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pause_screen();
                break;
        }
    }
}

void save_data(ContentCatalog *content_catalog, UserManager *user_manager, ListManager *list_manager) {
    clear_screen();
    printf("Salvando Dados\n");
    printf("----------------------------------------\n");
    
    int content_result = content_save_to_csv(content_catalog, CONTENT_FILE);
    if (content_result) {
        printf("Conteudos salvos com sucesso em '%s'.\n", CONTENT_FILE);
    } else {
        printf("Erro ao salvar os conteudos.\n");
    }
    
    int user_result = user_save_to_csv(user_manager, USER_FILE);
    if (user_result) {
        printf("Utilizadores salvos com sucesso em '%s'.\n", USER_FILE);
    } else {
        printf("Erro ao salvar os utilizadores.\n");
    }
    
    int interaction_result = user_save_interactions_to_csv(user_manager, INTERACTION_FILE);
    if (interaction_result) {
        printf("Interacoees salvas com sucesso em '%s'.\n", INTERACTION_FILE);
    } else {
        printf("Erro ao salvar as interacoes.\n");
    }
    
    int list_result = list_save_to_csv(list_manager, LIST_FILE);
    if (list_result) {
        printf("Listas salvas com sucesso em '%s'.\n", LIST_FILE);
    } else {
        printf("Erro ao salvar as listas.\n");
    }
    
    pause_screen();
}

void pause_screen() {
    printf("\nPressione Enter para continuar...");
    getchar();
}

int get_user_choice() {
    int choice;
    scanf("%d", &choice);
    getchar(); // Consumir quebra de linha
    return choice;
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(0, NULL);
}
#endif				