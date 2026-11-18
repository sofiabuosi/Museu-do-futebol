#ifndef SCENE_H
#define SCENE_H

void inicializaCena(void);
void desenhaCena(void);
void atualizaAnimacao(void);
void alternaAnimacao(void);

void alternaIluminacao(void);
void usaFlat(void);
void usaSmooth(void);

void desenhaPedestal(float x, float z);
void desenhaPlaca(float x, float y, float z, const char *texto);
void desenhaCamisaCorinthians(void);
void desenhaCamisaAtletico(void);
void desenhaTacaLibertadores(void);

#endif
