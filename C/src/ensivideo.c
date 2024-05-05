#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "oggstream.h"
#include "stream_common.h"
#include "synchro.h"

int main(int argc, char *argv[]) {
  int res;
  pthread_cond_init(&cond_window_size_ready,NULL);
  pthread_cond_init(&cond_texture_ready,NULL);
  pthread_cond_init(&condition_consommation,NULL);
  pthread_cond_init(&condition_production,NULL);


        
  pthread_mutex_init(&syn_mutex, NULL);
  pthread_mutex_init(&mutex_fenetre_texture, NULL);
  pthread_mutex_init(&mutex_producteur_consommateur, NULL);
  pthread_mutex_init(&mutexStreamState,NULL);

  if (argc != 2) {
    fprintf(stderr, "Usage: %s FILE", argv[0]);
    exit(EXIT_FAILURE);
  }
  assert(argc == 2);

  // Initialisation de la SDL
  res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  atexit(SDL_Quit);
  assert(res == 0);

  // Your code HERE
  // start the two stream readers (theoraStreamReader and vorbisStreamReader)
  // each in a thread
  pthread_t theoraThread;
  pthread_t vorbisThread;
  pthread_create(&theoraThread, NULL, theoraStreamReader, &argv[1]);//TODO (void *)  of (char *)instead of &
  pthread_create(&vorbisThread, NULL, vorbisStreamReader, &argv[1]);//(void *) instead of &

  // wait for vorbis thread
  pthread_join(vorbisThread, NULL);
  // 1 seconde of sound in advance, thus wait 1 seconde
  // before leaving
  sleep(1);

  // Wait for theora and theora2sdl threads
    pthread_cancel(theoraThread);
    //pthread_cancel(draw2SDLThread);
    pthread_join(theoraThread, NULL);
    //pthread_join(draw2SDLThread, NULL);
  // TODO
  /* liberer des choses ? */
  pthread_mutex_destroy(&mutex_fenetre_texture);
  pthread_mutex_destroy(&mutex_producteur_consommateur);
  pthread_mutex_destroy(&mutexStreamState);
  pthread_cond_destroy(&cond_window_size_ready);
  pthread_cond_destroy(&cond_texture_ready);
  pthread_cond_destroy(&condition_consommation);
  pthread_cond_destroy(&condition_production);

  exit(EXIT_SUCCESS);
}
