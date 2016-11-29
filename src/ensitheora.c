#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "ensitheora.h"
#include "synchro.h"
#include "stream_common.h"

int windowsx = 0;
int windowsy = 0;

int tex_iaff= 0;
int tex_iwri= 0;


static SDL_Window *screen = NULL;
static SDL_Renderer *renderer = NULL;
struct TextureDate texturedate[NBTEX] = {}; 

struct streamstate *theorastrstate=NULL;

void *draw2SDL(void *arg) {
    int serial = (int) (long long int) arg;
    struct streamstate *s= NULL;
    SDL_Texture* texture = NULL;

    attendreTailleFenetre();
    
    // create SDL window (if not done) and renderer
    screen = SDL_CreateWindow("Ensimag lecteur ogg/theora/vorbis",
			      SDL_WINDOWPOS_UNDEFINED,
			      SDL_WINDOWPOS_UNDEFINED,
			      windowsx,
			      windowsy,
			      0);
    renderer = SDL_CreateRenderer(screen, -1, 0);
	    
    assert(screen);
    assert(renderer);
    // affichage en noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);


    // la texture
    texture = SDL_CreateTexture(renderer,
					   SDL_PIXELFORMAT_YV12,
					   SDL_TEXTUREACCESS_STREAMING,
					   windowsx,
					   windowsy);

    assert(texture);

    signalerFenetreEtTexturePrete();

    /* Protéger l'accès à la hashmap */

    HASH_FIND_INT( theorastrstate, &serial, s );



    assert(s->strtype == TYPE_THEORA);
    
    while(! fini) {
	// récupérer les évenements de fin
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
	  // handle your event here
	  if (event.type == SDL_QUIT) {
	    fini = true;
	    break;
	  }
	}

	debutConsommerTexture();

	SDL_UpdateYUVTexture(texture, &rect,
			       texturedate[tex_iaff].buffer[0].data,
			       texturedate[tex_iaff].buffer[0].stride,
			       texturedate[tex_iaff].buffer[1].data,
			       texturedate[tex_iaff].buffer[1].stride,
			       texturedate[tex_iaff].buffer[2].data,
			       texturedate[tex_iaff].buffer[2].stride);
  
	// Copy the texture with the renderer
	SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	double timemsfromstart = msFromStart();
	
	int delaims = (int) (texturedate[tex_iaff].timems - timemsfromstart);
	
	tex_iaff = (tex_iaff + 1) % NBTEX;

	finConsommerTexture();

	if (delaims > 0.0)
	    SDL_Delay(delaims);
    }

    SDL_DestroyTexture(texture);

    return 0;   
}


void theora2SDL(struct streamstate *s) {
    assert(s->strtype == TYPE_THEORA);
    
    ogg_int64_t granulpos = -1;
    double framedate; // framedate in seconds
    int res = th_decode_packetin( s->th_dec.ctx,
				  & s->packet,
				  & granulpos);
    framedate = th_granule_time( s->th_dec.ctx, granulpos);
    if (res == TH_DUPFRAME) // 0 byte duplicated frame
	return;
	
    assert(res == 0);
    th_ycbcr_buffer buffer;
	    
    res =  th_decode_ycbcr_out(s->th_dec.ctx, buffer);

    // Envoyer la taille de la fenêtre
    envoiTailleFenetre(buffer);
    
    attendreFenetreTexture();

    // copy the buffer
    SDL_Rect rect = {};
    rect.w = buffer[0].width;
    rect.h = buffer[0].height;

    // 1 seul producteur/un seul conso => synchro sur le nb seulement

    debutDeposerTexture();

    for(unsigned i = 0; i < 3; ++i)
        texturedate[tex_iwri].buffer[i] = buffer[i];

    texturedate[tex_iwri].timems = framedate * 1000;
    assert(res == 0);
    tex_iwri = (tex_iwri + 1) % NBTEX;

    finDeposerTexture();		
}