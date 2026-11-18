#ifndef CAMERA_H
#define CAMERA_H

void inicializaCamera(void);
void aplicaCamera(void);
void configuraProjecao(int largura, int altura);

void usaPerspectiva(void);
void usaOrtografica(void);

void moveFrente(float distancia);
void moveLateral(float distancia);
void moveVertical(float distancia);
void giraYaw(float graus);
void giraPitch(float graus);

#endif
