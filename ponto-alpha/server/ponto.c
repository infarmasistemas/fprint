//
// Created by leticia on 14/09/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "utils.h"
#include "remote_database.h"



int do_point();

int do_point(){

    //lista de digitais
    int num_digitais = 0;
    unsigned char *ret;
    int num_ret = 0;


    //free this later on
    char *json = get_full_user_list();

    int number_of_users;
    get_number_of_users(json, &number_of_users);

    //Allocating structs to fill with user data
    //PS: Free this later on
    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
    num_digitais = deal_with_json(json, list_of_users);

    //criando lista de digitais
    unsigned char digitais[num_digitais][12050];
    int ids_list[num_digitais];
    for (int i = 0; i < number_of_users; i++){

        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
//            printf("Id: %d\n", (list_of_users)[i].user_id);
//            printf("Name: %s\n", (list_of_users)[i].name);
            ids_list[num_ret] = (list_of_users)[i].user_id;
            string_to_fprint((list_of_users)[i].fingerprint, digitais[num_ret]);
            num_ret++;
        }
    }


    ///*Iniciando device*///

    int r = 1;
    struct fp_dscv_dev *ddev;
    struct fp_dscv_dev **discovered_devs;
    struct fp_dev *dev;
    // struct fp_print_data *data;


    r = fp_init();

    if (r < 0) {
        fprintf(stderr, "Failed to initialize libfprint\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao inicializar a libfprint", "");
        return 1;
    }

    fp_set_debug(3);

    discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
        fprintf(stderr, "Could not discover devices\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao descobrir dispositivos", "");
        return 1;
    }
    ddev = discover_device(discovered_devs);
    if (!ddev) {
        fprintf(stderr, "No devices detected.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Nenhum dispositivo encontrado", "");
        return 1;
    }
    dev = fp_dev_open(ddev);
    fp_dscv_devs_free(discovered_devs);
    if (!dev) {
        fprintf(stderr, "Could not open device.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao abrir dispositivo", "");
        return 1;
    }

    printf("Opened device. It's now time to enroll your finger.\n");
    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "do_point", "Dispositivo inicializado. Cadastre sua digital.", "");

    ///Fim inicialização device


    int result = compare_digital(dev, digitais, num_digitais, ids_list); //chamada em data.c
    if(result > -1){
        printf("index_match: %d | id_user: %d\n", result);
        compose_json_answer("SCREEN_UPDATE", "SUCCESS", "do_point", "User matches",  result);
    }else {
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "User doesn't match", "-1");
    }



    printf("Result do_point:%d\n", result);
    post_ponto(result);

    out_close:
    fp_dev_close(dev);
    out:

    fp_exit();

    return 0;
}

///verifica a digital capturada contra uma lista de digitais
int identify(struct fp_dev *dev, struct fp_print_data **print_gallery){

    int r;
    size_t match_offset = 0;

    do {
        sleep(1);
        printf("\nScan your finger now.\n");

        r = fp_identify_finger(dev, print_gallery, &match_offset);

        if (r < 0) {
            printf("verification failed with error %d :(\n", r);
            return r;
        }
        switch (r) {
            case FP_VERIFY_NO_MATCH:
                printf("NO MATCH!\n");
                return -1;
            case FP_VERIFY_MATCH:
                printf("MATCH!\n");
                return match_offset;
            case FP_VERIFY_RETRY:
                printf("Scan didn't quite work. Please try again.\n");
                break;
            case FP_VERIFY_RETRY_TOO_SHORT:
                printf("Swipe was too short, please try again.\n");
                break;
            case FP_VERIFY_RETRY_CENTER_FINGER:
                printf("Please center your finger on the sensor and try again.\n");
                break;
            case FP_VERIFY_RETRY_REMOVE_FINGER:
                printf("Please remove finger from the sensor and try again.\n");
                break;
        }
    } while (1);
}

int compare_digital(struct fp_dev *dev, unsigned char digitais[][12050], int num_digitais, int * id_list){

    struct fp_dscv_dev *ddev;
    struct fp_dscv_dev **discovered_devs;
    struct fp_print_data *data_user;
    struct fp_print_data **print_gallery = NULL;

    //criando print_gallery
    print_gallery = malloc(sizeof(*print_gallery) * (num_digitais+1));
    print_gallery[num_digitais] = NULL;

    ///print_gallery 	NULL-terminated array of pointers to the prints to identify against. Each one must have been previously enrolled with a device compatible to the device selected to perform the scan. *///



    for (int i = 0; i<num_digitais; i++) {
        printf("\nnum_digitais: %d | i: %d\n", num_digitais, i);
        print_gallery[i] = fp_print_data_from_data(digitais[i], 12050);

    }

    ///for one by one verification
    //verify(dev, print_gallery[1]);


    ///for 1 by many verification
    int index_match = identify(dev, print_gallery);
    int id_user_matched = -1;
    char str_user_id[12];

    //fp_print_data_free(print_gallery);

    return id_user_matched;
}