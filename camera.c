#include <GL/glut.h>
#include <math.h>
#include "camera.h"

#define GRAUS_PARA_RADIANOS 0.01745329251994329577f
#define LIMITE_X 9.0f
#define LIMITE_Z 7.0f
#define ALTURA_MINIMA_CAMERA 1.0f
#define ALTURA_MAXIMA_CAMERA 6.5f
/* A camera comeca dentro da sala, olhando para -Z, com pequena inclinacao para baixo.
   O alvo e posicao + frente; o vetor up do gluLookAt permanece (0, 1, 0). */
static float camX = 0.0f;
static float camY = 2.5f;
static float camZ = 7.0f;
static float yaw = -90.0f;
static float pitch = -7.0f;
static int perspectiva = 1;

static float limita(float valor, float minimo, float maximo)
{
    if (valor < minimo)
        return minimo;
    if (valor > maximo)
        return maximo;
    return valor;
}

static void calculaFrente(float *frenteX, float *frenteY, float *frenteZ)
{
    const float yawRad = yaw * GRAUS_PARA_RADIANOS;
    const float pitchRad = pitch * GRAUS_PARA_RADIANOS;
    const float cosPitch = cosf(pitchRad);

    *frenteX = cosf(yawRad) * cosPitch;
    *frenteY = sinf(pitchRad);
    *frenteZ = sinf(yawRad) * cosPitch;
}

static void limitaPosicao(void)
{
    camX = limita(camX, -LIMITE_X, LIMITE_X);
    camY = limita(camY, ALTURA_MINIMA_CAMERA, ALTURA_MAXIMA_CAMERA);
    camZ = limita(camZ, -LIMITE_Z, LIMITE_Z);
}

void inicializaCamera(void)
{
    camX = 0.0f;
    camY = 2.5f;
    camZ = 7.0f;
    yaw = -90.0f;
    pitch = -7.0f;
    perspectiva = 1;
}

void aplicaCamera(void)
{
    float frenteX;
    float frenteY;
    float frenteZ;

    calculaFrente(&frenteX, &frenteY, &frenteZ);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
              camX + frenteX, camY + frenteY, camZ + frenteZ,
              0.0f, 1.0f, 0.0f);
}

void configuraProjecao(int largura, int altura)
{
    const float aspecto = (altura > 0) ? (float)largura / (float)altura : 1.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (perspectiva)
    {
        /* Perspectiva: objetos distantes parecem menores e ha profundidade visual. */
        gluPerspective(60.0, aspecto, 0.1, 100.0);
    }
    else
    {
        /* Ortografica: mantem o tamanho aparente sem convergencia das linhas. */
        const float metadeAltura = 8.5f;
        const float metadeLargura = metadeAltura * aspecto;

        glOrtho(-metadeLargura, metadeLargura,
                -metadeAltura, metadeAltura,
                -50.0, 50.0);
    }

    glMatrixMode(GL_MODELVIEW);
}

void usaPerspectiva(void)
{
    perspectiva = 1;
}

void usaOrtografica(void)
{
    perspectiva = 0;
}

void moveFrente(float distancia)
{
    float frenteX;
    float frenteY;
    float frenteZ;
    float comprimento;

    calculaFrente(&frenteX, &frenteY, &frenteZ);
    comprimento = sqrtf(frenteX * frenteX + frenteZ * frenteZ);
    if (comprimento > 0.0f)
    {
        camX += (frenteX / comprimento) * distancia;
        camZ += (frenteZ / comprimento) * distancia;
    }
    limitaPosicao();
}

void moveLateral(float distancia)
{
    float frenteX;
    float frenteY;
    float frenteZ;
    float direitaX;
    float direitaZ;
    float comprimento;

    calculaFrente(&frenteX, &frenteY, &frenteZ);

    /* frente x up (0,1,0) produz direita = (frenteZ, 0, -frenteX). */
    direitaX = frenteZ;
    direitaZ = -frenteX;
    comprimento = sqrtf(direitaX * direitaX + direitaZ * direitaZ);
    if (comprimento > 0.0f)
    {
        camX += (direitaX / comprimento) * distancia;
        camZ += (direitaZ / comprimento) * distancia;
    }
    limitaPosicao();
}

void moveVertical(float distancia)
{
    camY += distancia;
    limitaPosicao();
}

void giraYaw(float graus)
{
    yaw += graus;
}

void giraPitch(float graus)
{
    pitch = limita(pitch + graus, -89.0f, 89.0f);
}
