#ifndef COMET_UTILS_H_
#define COMET_UTILS_H_

#include <stdio.h>
#include <sys/wait.h>

#include "comet/io.h"

/** path restriction needed. i.e. do not allow /usr or ../ */

/**
 * @brief 
 * 
 * @param script 
 * @param args 
 * @return int 
 */
int run_pipe(const char* script, char*** args);

/**
 * @brief NOTE: May not be needed unless bash can't be used. 
 * Can run all init commands i.e. create_dir/mkdir with bash
 * 
 * @return int 
 */
int comet_setup(const char* comet_dir);

/**
 * @brief 
 * 
 * @return int 
 */
int verify_dirs();

void char_arr_free(char** arr, int size);

int parse_flags(int argc, char** args, char*** flags, char*** targets, int* targets_sz);


#endif