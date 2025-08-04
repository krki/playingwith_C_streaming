/**
 * @file test.c
 * @brief Testes unitários para os módulos do programa Streamflix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "csvutil.h"
#include "content.h"
#include "user.h"
#include "list.h"
#include "recommendation.h"
#include "report.h"

// Protótipos das funções de teste
void test_csvutil();
void test_content();
void test_user();
void test_list();
void test_recommendation();
void test_report();
void test_integration();

/**
 * @brief Função principal dos testes
 * 
 * @return int Código de saída
 */
/*int main() {
    printf("Iniciando testes unitários...\n\n");
    
    test_csvutil();
    test_content();
    test_user();
    test_list();
    test_recommendation();
    test_report();
    test_integration();
    
    printf("\nTodos os testes foram executados com sucesso!\n");
    return 0;
}*/

/**
 * @brief Testes para o módulo de utilitários CSV
 */
void test_csvutil() {
    printf("Testando módulo csvutil...\n");
    
    // Teste de leitura e escrita de CSV
    FILE *test_file = fopen("test_csv.csv", "w");
    assert(test_file != NULL);
    
    char *test_line1[3] = {"campo1", "campo2", "campo3"};
    char *test_line2[3] = {"valor1", "valor com vírgula", "valor3"};
    
    assert(csv_write_line(test_file, test_line1, 3) == 1);
    assert(csv_write_line(test_file, test_line2, 3) == 1);
    
    fclose(test_file);
    
    // Ler o arquivo criado
    test_file = fopen("test_csv.csv", "r");
    assert(test_file != NULL);
    
    char buffer[1024];
    char *fields[10];
    
    // Ler a primeira linha
    assert(csv_read_line(test_file, buffer, sizeof(buffer)) == 1);
    int field_count = csv_parse_line(buffer, fields, 10);
    assert(field_count == 3);
    assert(strcmp(fields[0], "campo1") == 0);
    assert(strcmp(fields[1], "campo2") == 0);
    assert(strcmp(fields[2], "campo3") == 0);
    
    // Ler a segunda linha
    assert(csv_read_line(test_file, buffer, sizeof(buffer)) == 1);
    field_count = csv_parse_line(buffer, fields, 10);
    assert(field_count == 3);
    assert(strcmp(fields[0], "valor1") == 0);
    assert(strcmp(fields[1], "valor com vírgula") == 0);
    assert(strcmp(fields[2], "valor3") == 0);
    
    fclose(test_file);
    remove("test_csv.csv"); // Limpar arquivo de teste
    
    printf("Módulo csvutil testado com sucesso!\n");
}

/**
 * @brief Testes para o módulo de conteúdos
 */
void test_content() {
    printf("Testando módulo content...\n");
    
    ContentCatalog catalog;
    assert(content_init_catalog(&catalog, 10) == 1);
    
    // Testar adição de conteúdos
    int id1 = content_add(&catalog, "Filme 1", "Ação", 120, 16);
    int id2 = content_add(&catalog, "Série 1", "Drama", 45, 12);
    int id3 = content_add(&catalog, "Documentário", "Educacional", 90, 0);
    
    assert(id1 > 0);
    assert(id2 > 0);
    assert(id3 > 0);
    assert(catalog.count == 3);
    
    // Testar busca por ID
    Content *content = content_get_by_id(&catalog, id1);
    assert(content != NULL);
    assert(strcmp(content->title, "Filme 1") == 0);
    assert(strcmp(content->category, "Ação") == 0);
    assert(content->duration == 120);
    assert(content->age_rating == 16);
    
    // Testar edição de conteúdo
    assert(content_edit(&catalog, id1, "Filme 1 - Edição Especial", NULL, 130, 0) == 1);
    content = content_get_by_id(&catalog, id1);
    assert(strcmp(content->title, "Filme 1 - Edição Especial") == 0);
    assert(content->duration == 130);
    assert(content->age_rating == 16); // Não foi alterado
    
    // Testar busca por título
    int results[10];
    int count = content_search_by_title(&catalog, "filme", results, 10);
    assert(count == 1);
    assert(results[0] == id1);
    
    // Testar busca por categoria
    count = content_search_by_category(&catalog, "Drama", results, 10);
    assert(count == 1);
    assert(results[0] == id2);
    
    // Testar busca por classificação etária
    count = content_search_by_age_rating(&catalog, 0, results, 10);
    assert(count == 1);
    assert(results[0] == id3);
    
    // Testar incremento de visualizações
    assert(content_increment_views(&catalog, id1) == 1);
    content = content_get_by_id(&catalog, id1);
    assert(content->views == 1);
    
    // Testar remoção de conteúdo
    assert(content_remove(&catalog, id2) == 1);
    assert(catalog.count == 2);
    assert(content_get_by_id(&catalog, id2) == NULL);
    
    // Testar salvamento e carregamento
    assert(content_save_to_csv(&catalog, "test_content.csv") == 1);
    
    ContentCatalog loaded_catalog;
    assert(content_init_catalog(&loaded_catalog, 10) == 1);
    assert(content_load_from_csv(&loaded_catalog, "test_content.csv") == 2);
    
    content = content_get_by_id(&loaded_catalog, id1);
    assert(content != NULL);
    assert(strcmp(content->title, "Filme 1 - Edição Especial") == 0);
    assert(content->views == 1);
    
    // Limpar recursos
    content_free_catalog(&catalog);
    content_free_catalog(&loaded_catalog);
    remove("test_content.csv");
    
    printf("Módulo content testado com sucesso!\n");
}

/**
 * @brief Testes para o módulo de utilizadores
 */
void test_user() {
    printf("Testando módulo user...\n");
    
    UserManager manager;
    assert(user_init_manager(&manager, 10, 100) == 1);
    
    // Testar adição de utilizador
    int id1 = user_add(&manager, "Utilizador1");
    int id2 = user_add(&manager, "Utilizador2");
    
    assert(id1 > 0);
    assert(id2 > 0);
    assert(manager.count == 2);
    
    // Testar busca por ID
    User *user = user_get_by_id(&manager, id1);
    assert(user != NULL);
    assert(strcmp(user->username, "Utilizador1") == 0);
    
    // Testar busca por nome de utilizador
    user = user_get_by_username(&manager, "Utilizador2");
    assert(user != NULL);
    assert(user->id == id2);
    
    // Testar interações
    assert(user_register_interaction(&manager, id1, 101, INTERACTION_PLAY) == 1);
    assert(user_register_interaction(&manager, id1, 102, INTERACTION_FAVORITE) == 1);
    assert(user_register_interaction(&manager, id2, 101, INTERACTION_COMPLETE) == 1);
    
    assert(manager.interaction_count == 3);
    assert(user_get_by_id(&manager, id1)->interaction_count == 2);
    assert(user_get_by_id(&manager, id2)->interaction_count == 1);
    
    // Testar favoritos
    user = user_get_by_id(&manager, id1);
    assert(user->favorite_count == 1); // Devido à interação FAVORITE
    assert(user->favorite_contents[0] == 102);
    
    // Testar remoção de favoritos
    assert(user_remove_favorite(&manager, id1, 102) == 1);
    user = user_get_by_id(&manager, id1);
    assert(user->favorite_count == 0);
    
    // Testar adição manual de favoritos
    assert(user_add_favorite(&manager, id1, 103) == 1);
    user = user_get_by_id(&manager, id1);
    assert(user->favorite_count == 1);
    assert(user->favorite_contents[0] == 103);
    
    // Testar salvamento e carregamento
    assert(user_save_to_csv(&manager, "test_user.csv") == 1);
    assert(user_save_interactions_to_csv(&manager, "test_interaction.csv") == 1);
    
    UserManager loaded_manager;
    assert(user_init_manager(&loaded_manager, 10, 100) == 1);
    assert(user_load_from_csv(&loaded_manager, "test_user.csv") == 2);
    assert(user_load_interactions_from_csv(&loaded_manager, "test_interaction.csv") == 3);
    
    user = user_get_by_id(&loaded_manager, id1);
    assert(user != NULL);
    assert(strcmp(user->username, "Utilizador1") == 0);
    assert(user->favorite_count == 1);
    
    // Testar remoção de utilizador
    assert(user_remove(&loaded_manager, id2) == 1);
    assert(loaded_manager.count == 1);
    assert(user_get_by_id(&loaded_manager, id2) == NULL);
    
    // Limpar recursos
    user_free_manager(&manager);
    user_free_manager(&loaded_manager);
    remove("test_user.csv");
    remove("test_interaction.csv");
    
    printf("Módulo user testado com sucesso!\n");
}

/**
 * @brief Testes para o módulo de listas
 */
void test_list() {
    printf("Testando módulo list...\n");
    
    ListManager manager;
    assert(list_init_manager(&manager, 10) == 1);
    
    // Testar criação de listas
    int id1 = list_create(&manager, 1, "Minha Lista");
    int id2 = list_create(&manager, 2, "Assistir Depois");
    
    assert(id1 > 0);
    assert(id2 > 0);
    assert(manager.count == 2);
    
    // Testar busca por ID
    CustomList *list = list_get_by_id(&manager, id1);
    assert(list != NULL);
    assert(strcmp(list->name, "Minha Lista") == 0);
    assert(list->user_id == 1);
    
    // Testar procurar por utilizador
    int results[10];
    int count = list_get_by_user(&manager, 1, results, 10);
    assert(count == 1);
    assert(results[0] == id1);
    
    // Testar adição de conteúdos à lista
    assert(list_add_content(&manager, id1, 101) == 1);
    assert(list_add_content(&manager, id1, 102) == 1);
    assert(list_add_content(&manager, id2, 103) == 1);
    
    list = list_get_by_id(&manager, id1);
    assert(list->count == 2);
    assert(list->content_ids[0] == 101);
    assert(list->content_ids[1] == 102);
    
    // Testar remoção de conteúdos da lista
    assert(list_remove_content(&manager, id1, 101) == 1);
    list = list_get_by_id(&manager, id1);
    assert(list->count == 1);
    assert(list->content_ids[0] == 102);
    
    // Testar renomeação de lista
    assert(list_rename(&manager, id1, "Nova Minha Lista") == 1);
    list = list_get_by_id(&manager, id1);
    assert(strcmp(list->name, "Nova Minha Lista") == 0);
    
    // Testar salvamento e carregamento
    assert(list_save_to_csv(&manager, "test_list.csv") == 1);
    
    ListManager loaded_manager;
    assert(list_init_manager(&loaded_manager, 10) == 1);
    assert(list_load_from_csv(&loaded_manager, "test_list.csv") == 2);
    
    list = list_get_by_id(&loaded_manager, id1);
    assert(list != NULL);
    assert(strcmp(list->name, "Nova Minha Lista") == 0);
    assert(list->count == 1);
    assert(list->content_ids[0] == 102);
    
    // Testar remoção de lista
    assert(list_remove(&loaded_manager, id2) == 1);
    assert(loaded_manager.count == 1);
    assert(list_get_by_id(&loaded_manager, id2) == NULL);
    
    // Limpar recursos
    list_free_manager(&manager);
    list_free_manager(&loaded_manager);
    remove("test_list.csv");
    
    printf("Módulo list testado com sucesso!\n");
}

/**
 * @brief Testes para o módulo de recomendações
 */
void test_recommendation() {
    printf("Testando módulo recommendation...\n");
    
    // Criar catálogo de conteúdos
    ContentCatalog catalog;
    assert(content_init_catalog(&catalog, 10) == 1);
    
    // Adicionar conteúdos
    int id1 = content_add(&catalog, "Filme de Ação 1", "Ação", 120, 16);
    int id2 = content_add(&catalog, "Filme de Ação 2", "Ação", 110, 14);
    int id3 = content_add(&catalog, "Comédia 1", "Comédia", 100, 12);
    int id4 = content_add(&catalog, "Drama 1", "Drama", 140, 16);
    int id5 = content_add(&catalog, "Documentário", "Documentário", 90, 0);
    
    // Adicionar visualizações
    for (int i = 0; i < 10; i++) content_increment_views(&catalog, id1);
    for (int i = 0; i < 20; i++) content_increment_views(&catalog, id2);
    for (int i = 0; i < 5; i++) content_increment_views(&catalog, id3);
    for (int i = 0; i < 15; i++) content_increment_views(&catalog, id4);
    for (int i = 0; i < 3; i++) content_increment_views(&catalog, id5);
    
    // Criar gerenciador de utilizador
    UserManager user_manager;
    assert(user_init_manager(&user_manager, 10, 100) == 1);
    
    // Adicionar utilizador
    int user_id1 = user_add(&user_manager, "Utilizador1");
    int user_id2 = user_add(&user_manager, "Utilizador2");
    
    // Registrar interações
    user_register_interaction(&user_manager, user_id1, id1, INTERACTION_COMPLETE);
    user_register_interaction(&user_manager, user_id1, id3, INTERACTION_COMPLETE);
    user_register_interaction(&user_manager, user_id2, id2, INTERACTION_COMPLETE);
    user_register_interaction(&user_manager, user_id2, id4, INTERACTION_COMPLETE);
    
    // Testar recomendações por popularidade
    int recommendations[10];
    int count = recommendation_by_popularity(&catalog, recommendations, 3);
    
    assert(count == 3);
    assert(recommendations[0] == id2); // 20 visualizações
    assert(recommendations[1] == id4); // 15 visualizações
    assert(recommendations[2] == id1); // 10 visualizações
    
    // Testar recomendações por categoria
    count = recommendation_by_category(&user_manager, &catalog, user_id1, recommendations, 5);
    assert(count > 0);
    
    // Testar recomendações personalizadas
    count = recommendation_personalized(&user_manager, &catalog, user_id1, recommendations, 5);
    assert(count > 0);
    
    // Testar similaridade
    Content *c1 = content_get_by_id(&catalog, id1);
    Content *c2 = content_get_by_id(&catalog, id2);
    Content *c3 = content_get_by_id(&catalog, id3);
    
    float sim1 = recommendation_calculate_similarity(c1, c2);
    float sim2 = recommendation_calculate_similarity(c1, c3);
    
    assert(sim1 > sim2); // Filmes da mesma categoria devem ter maior similaridade
    
    // Testar função has_watched
    assert(recommendation_has_watched(&user_manager, user_id1, id1) == 1);
    assert(recommendation_has_watched(&user_manager, user_id1, id2) == 0);
    assert(recommendation_has_watched(&user_manager, user_id2, id2) == 1);
    
    // Limpar recursos
    content_free_catalog(&catalog);
    user_free_manager(&user_manager);
    
    printf("Módulo recommendation testado com sucesso!\n");
}

/**
 * @brief Testes para o módulo de relatórios
 */
void test_report() {
    printf("Testando módulo report...\n");
    
    // Criar catálogo de conteúdos
    ContentCatalog catalog;
    assert(content_init_catalog(&catalog, 10) == 1);
    
    // Adicionar conteúdos
    int id1 = content_add(&catalog, "Filme de Ação 1", "Ação", 120, 16);
    int id2 = content_add(&catalog, "Filme de Ação 2", "Ação", 110, 14);
    int id3 = content_add(&catalog, "Comédia 1", "Comédia", 100, 12);
    int id4 = content_add(&catalog, "Drama 1", "Drama", 140, 16);
    
    // Adicionar visualizações
    for (int i = 0; i < 10; i++) content_increment_views(&catalog, id1);
    for (int i = 0; i < 20; i++) content_increment_views(&catalog, id2);
    for (int i = 0; i < 5; i++) content_increment_views(&catalog, id3);
    for (int i = 0; i < 15; i++) content_increment_views(&catalog, id4);
    
    // Criar gerenciador de utilizadores
    UserManager user_manager;
    assert(user_init_manager(&user_manager, 10, 100) == 1);
    
    // Adicionar utilizadores
    int user_id1 = user_add(&user_manager, "Utilizador1");
    int user_id2 = user_add(&user_manager, "Utilizador2");
    
    // Registrar interações
    for (int i = 0; i < 5; i++) user_register_interaction(&user_manager, user_id1, id1, INTERACTION_PLAY);
    for (int i = 0; i < 3; i++) user_register_interaction(&user_manager, user_id1, id3, INTERACTION_COMPLETE);
    for (int i = 0; i < 8; i++) user_register_interaction(&user_manager, user_id2, id2, INTERACTION_PAUSE);
    for (int i = 0; i < 4; i++) user_register_interaction(&user_manager, user_id2, id4, INTERACTION_FAVORITE);
    
    // Testar relatório de conteúdos mais vistos
    ContentReportItem content_results[10];
    int count = report_most_viewed_contents(&catalog, content_results, 10);
    
    assert(count == 4);
    assert(content_results[0].content_id == id2);
    assert(content_results[0].count == 20);
    assert(content_results[1].content_id == id4);
    assert(content_results[1].count == 15);
    
    // Testar relatório de categorias mais populares
    CategoryReportItem category_results[10];
    count = report_most_popular_categories(&catalog, category_results, 10);
    
    assert(count == 3); // Ação, Drama, Comédia
    assert(strcmp(category_results[0].category, "Ação") == 0);
    assert(category_results[0].count == 30); // 10 + 20
    
    // Testar relatório de utilizadores mais ativos
    UserReportItem user_results[10];
    count = report_most_active_users(&user_manager, user_results, 10);
    
    assert(count == 2);
    assert(user_results[0].user_id == user_id2);
    assert(user_results[0].count == 12); // 8 + 4
    assert(user_results[1].user_id == user_id1);
    assert(user_results[1].count == 8); // 5 + 3
    
    // Testar relatório de interações de utilizador
    ContentReportItem interaction_results[10];
    count = report_user_interactions(&user_manager, &catalog, user_id1, interaction_results, 10);
    
    assert(count == 2);
    assert(interaction_results[0].content_id == id1);
    assert(interaction_results[0].count == 5);
    assert(interaction_results[1].content_id == id3);
    assert(interaction_results[1].count == 3);
    
    // Testar exportação de relatório
    char *headers[3] = {"ID", "Título", "Visualizações"};
    char ***data = (char***)malloc(2 * sizeof(char**));
    
    for (int i = 0; i < 2; i++) {
        data[i] = (char**)malloc(3 * sizeof(char*));
        
        data[i][0] = (char*)malloc(20 * sizeof(char));
        sprintf(data[i][0], "%d", i + 1);
        
        data[i][1] = (char*)malloc(100 * sizeof(char));
        sprintf(data[i][1], "Conteúdo %d", i + 1);
        
        data[i][2] = (char*)malloc(20 * sizeof(char));
        sprintf(data[i][2], "%d", (i + 1) * 10);
    }
    
    assert(report_export_to_csv("test_report.csv", headers, 3, data, 2) == 1);
    
    // Limpar recursos
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
    
    content_free_catalog(&catalog);
    user_free_manager(&user_manager);
    remove("test_report.csv");
    
    printf("Módulo report testado com sucesso!\n");
}

/**
 * @brief Testes de integração
 */
void test_integration() {
    printf("Executando testes de integração...\n");
    
    // Inicializar estruturas
    ContentCatalog catalog;
    UserManager user_manager;
    ListManager list_manager;
    
    assert(content_init_catalog(&catalog, 10) == 1);
    assert(user_init_manager(&user_manager, 10, 100) == 1);
    assert(list_init_manager(&list_manager, 10) == 1);
    
    // Adicionar dados
    int film_id = content_add(&catalog, "Matrix", "Sci-Fi", 136, 14);
    int series_id = content_add(&catalog, "Breaking Bad", "Drama", 45, 16);
    int user_id = user_add(&user_manager, "TestUser");
    int list_id = list_create(&list_manager, user_id, "Favoritos");
    
    // Testar fluxo completo
    // 1. Utilizador assiste a um filme
    assert(user_register_interaction(&user_manager, user_id, film_id, INTERACTION_PLAY) == 1);
    assert(content_increment_views(&catalog, film_id) == 1);
    
    // 2. Utilizador completa a visualização
    assert(user_register_interaction(&user_manager, user_id, film_id, INTERACTION_COMPLETE) == 1);
    
    // 3. Utilizador adiciona o conteúdo à sua lista de favoritos
    assert(list_add_content(&list_manager, list_id, film_id) == 1);
    
    // 4. Utilizador recebe recomendações
    int recommendations[5];
    int count = recommendation_personalized(&user_manager, &catalog, user_id, recommendations, 5);
    assert(count > 0);
    
    // 5. Gerar relatório de conteúdos mais vistos
    ContentReportItem content_results[10];
    count = report_most_viewed_contents(&catalog, content_results, 10);
    assert(count == 2);
    assert(content_results[0].content_id == film_id);
    
    // 6. Salvar todos os dados
    assert(content_save_to_csv(&catalog, "integration_content.csv") == 1);
    assert(user_save_to_csv(&user_manager, "integration_user.csv") == 1);
    assert(user_save_interactions_to_csv(&user_manager, "integration_interaction.csv") == 1);
    assert(list_save_to_csv(&list_manager, "integration_list.csv") == 1);
    
    // 7. Carregar todos os dados em novas estruturas
    ContentCatalog new_catalog;
    UserManager new_user_manager;
    ListManager new_list_manager;
    
    assert(content_init_catalog(&new_catalog, 10) == 1);
    assert(user_init_manager(&new_user_manager, 10, 100) == 1);
    assert(list_init_manager(&new_list_manager, 10) == 1);
    
    assert(content_load_from_csv(&new_catalog, "integration_content.csv") == 2);
    assert(user_load_from_csv(&new_user_manager, "integration_user.csv") == 1);
    assert(user_load_interactions_from_csv(&new_user_manager, "integration_interaction.csv") == 2);
    assert(list_load_from_csv(&new_list_manager, "integration_list.csv") == 1);
    
    // 8. Verificar se os dados foram carregados corretamente
    Content *content = content_get_by_id(&new_catalog, film_id);
    assert(content != NULL);
    assert(strcmp(content->title, "Matrix") == 0);
    assert(content->views == 1);
    
    User *user = user_get_by_id(&new_user_manager, user_id);
    assert(user != NULL);
    assert(strcmp(user->username, "TestUser") == 0);
    
    CustomList *list = list_get_by_id(&new_list_manager, list_id);
    assert(list != NULL);
    assert(strcmp(list->name, "Favoritos") == 0);
    assert(list->count == 1);
    assert(list->content_ids[0] == film_id);
    
    // Limpar recursos
    content_free_catalog(&catalog);
    user_free_manager(&user_manager);
    list_free_manager(&list_manager);
    
    content_free_catalog(&new_catalog);
    user_free_manager(&new_user_manager);
    list_free_manager(&new_list_manager);
    
    remove("integration_content.csv");
    remove("integration_user.csv");
    remove("integration_interaction.csv");
    remove("integration_list.csv");
    
    printf("Testes de integração executados com sucesso!\n");
}