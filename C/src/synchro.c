#include "ensitheora.h"
#include "synchro.h"
#include <pthread.h>
/* les variables pour la synchro, ici */
pthread_mutex_t syn_mutex;
pthread_mutex_t mutex_fenetre_texture;
pthread_mutex_t mutex_producteur_consommateur;
pthread_cond_t cond_window_size_ready;
pthread_cond_t cond_texture_ready;
pthread_cond_t condition_consommation;
pthread_cond_t condition_production;
int texture_wait_count = 0;  // Counter for available textures
int windowsx;
int windowsy;
bool size_ready = false;
bool texture_ready = false;
/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    pthread_mutex_lock(&syn_mutex);
    windowsx = buffer[0].width;
    windowsy = buffer[0].height;
    size_ready = true;
    pthread_cond_broadcast(&cond_window_size_ready);
    pthread_mutex_unlock(&syn_mutex);
}

void attendreTailleFenetre() {
    pthread_mutex_lock(&syn_mutex);
    while(size_ready==false){
        pthread_cond_wait(&cond_window_size_ready,&syn_mutex);
    }
    //TODO shouldn't i add size_ready = false ?
    pthread_mutex_unlock(&syn_mutex);

}

void signalerFenetreEtTexturePrete() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    texture_ready = true;
    pthread_cond_broadcast(&cond_texture_ready);
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void attendreFenetreTexture() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    while(texture_ready == false) {
        pthread_cond_wait(&cond_texture_ready, &mutex_fenetre_texture);
    }
    //TODO shouldn't i add texture_ready = false ?
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void debutConsommerTexture() {
    pthread_mutex_lock(&mutex_producteur_consommateur);
    while(texture_wait_count==0)
    {
        pthread_cond_wait(&condition_consommation, &mutex_producteur_consommateur);
    }
    pthread_mutex_unlock(&mutex_producteur_consommateur);
}

void finConsommerTexture() {
    pthread_mutex_lock(&mutex_producteur_consommateur);
    texture_wait_count--;
    pthread_cond_signal(&condition_production);
    pthread_mutex_unlock(&mutex_producteur_consommateur);
}

void debutDeposerTexture() {
    pthread_mutex_lock(&mutex_producteur_consommateur);
    while(texture_wait_count==NBTEX)
   {
        pthread_cond_wait(&condition_production, &mutex_producteur_consommateur);
   }
    pthread_mutex_unlock(&mutex_producteur_consommateur);
}

void finDeposerTexture() {
    pthread_mutex_lock(&mutex_producteur_consommateur);
    texture_wait_count++;
    pthread_cond_signal(&condition_consommation);
    pthread_mutex_unlock(&mutex_producteur_consommateur);
}
