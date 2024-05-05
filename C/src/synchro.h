#ifndef SYNCHRO_H
#define SYNCHRO_H

#include "ensitheora.h"
#include <stdbool.h>

extern bool fini;

/* Les extern des variables pour la synchro ici */
extern pthread_mutex_t mutexStreamState;
extern pthread_mutex_t syn_mutex;
extern pthread_mutex_t mutex_fenetre_texture;
extern pthread_mutex_t mutex_producteur_consommateur;
extern pthread_cond_t cond_window_size_ready;
extern pthread_cond_t cond_texture_ready;
extern pthread_cond_t condition_consommation;
extern pthread_cond_t condition_production;
extern int texture_wait_count;
extern int windowsx;
extern int windowsy;
extern bool size_ready;
extern bool texture_ready;
/* Fonctions de synchro à implanter */

void envoiTailleFenetre(th_ycbcr_buffer buffer);
void attendreTailleFenetre();

void attendreFenetreTexture();
void signalerFenetreEtTexturePrete();

void debutConsommerTexture();
void finConsommerTexture();

void debutDeposerTexture();
void finDeposerTexture();

#endif
