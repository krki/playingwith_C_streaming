/**
 * @file csvutil.h
 * @brief Módulo para operações com arquivos CSV
 * 
 * Este módulo contém funções para leitura e escrita de dados em arquivos CSV,
 * facilitando a manipulação de dados pelo programa Streamflix.
 */

#ifndef CSVUTIL_H
#define CSVUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Lê uma linha de um arquivo CSV
 * 
 * @param file Ponteiro para o arquivo
 * @param buffer Buffer para armazenar a linha
 * @param size Tamanho do buffer
 * @return int 1 se a leitura foi bem-sucedida, 0 se chegou ao fim do arquivo
 */
int csv_read_line(FILE *file, char *buffer, int size);

/**
 * @brief Divide uma linha CSV em campos
 * 
 * @param line Linha CSV a ser dividida
 * @param fields Array para armazenar os campos
 * @param max_fields Número máximo de campos
 * @return int Número de campos encontrados
 */
int csv_parse_line(char *line, char **fields, int max_fields);

/**
 * @brief Escreve uma linha CSV em um arquivo
 * 
 * @param file Ponteiro para o arquivo
 * @param fields Array contendo os campos
 * @param num_fields Número de campos
 * @return int 1 se a escrita foi bem-sucedida, 0 caso contrário
 */
int csv_write_line(FILE *file, char **fields, int num_fields);

#endif /* CSVUTIL_H */