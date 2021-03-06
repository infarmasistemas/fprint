//
// Created by root on 05/10/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

char * compose_json_answer_user_matches(char id_user[], char name_user[]);

void compose_json_answer(char type[], char status[], char method_name[], char message[], char data[]);

void string_to_fprint(char fprint_string[], unsigned char file[]);

int size_of_file(char fprint_string[]);

void get_number_of_users(char* json_str, int* number_of_users);

void fprint_to_string(char * ret, int length, char digital[]);

int get_length_digital(char * ret, int length);

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif //UNTITLED_UTILS_H
