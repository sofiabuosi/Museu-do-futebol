#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "scene.h"

#define SALA_LIMITE_X 10.0f
#define SALA_FUNDO_Z (-8.0f)
#define SALA_FRENTE_Z 8.0f
#define SALA_ALTURA 7.0f
#define ALTURA_PEDESTAL 1.35f
#define POS_CORINTHIANS_X (-5.2f)
#define POS_ATLETICO_X 0.0f
#define POS_TACA_X 5.2f
#define POS_OBRAS_Z (-1.6f)
#define PASSOS_ANIMACAO 30.0f

static int animacaoLigada = 0;
static float anguloAnimacao = 0.0f;
static int ultimoTempoAnimacao = 0;
static GLUquadric *quadric = NULL;

static void desenhaBloco(float x, float y, float z,
                         float largura, float altura, float profundidade,
                         float rotacaoZ, float vermelho, float verde, float azul)
{
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(rotacaoZ, 0.0f, 0.0f, 1.0f);
        glScalef(largura, altura, profundidade);
        glColor3f(vermelho, verde, azul);
        glutSolidCube(1.0);
    glPopMatrix();
}

static void desenhaCilindroY(float x, float y, float z,
                             float raioInferior, float raioSuperior, float altura)
{
    if (quadric == NULL)
        return;

    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(quadric, raioInferior, raioSuperior, altura, 24, 4);
        gluDisk(quadric, 0.0, raioInferior, 24, 1);
        glTranslatef(0.0f, 0.0f, altura);
        gluDisk(quadric, 0.0, raioSuperior, 24, 1);
    glPopMatrix();
}

static void desenhaParede(float x1, float z1, float x2, float z2,
                          float nx, float ny, float nz,
                          float vermelho, float verde, float azul)
{
    glPushMatrix();
        glColor3f(vermelho, verde, azul);
        glBegin(GL_QUADS);
            glNormal3f(nx, ny, nz);
            glVertex3f(x1, 0.0f, z1);
            glVertex3f(x2, 0.0f, z2);
            glVertex3f(x2, SALA_ALTURA, z2);
            glVertex3f(x1, SALA_ALTURA, z1);
        glEnd();
    glPopMatrix();
}

static void desenhaPlacaTitulo(void)
{
    const GLboolean luzLigada = glIsEnabled(GL_LIGHTING);

    glPushMatrix();
        glTranslatef(0.0f, 5.75f, SALA_FUNDO_Z + 0.08f);
        glScalef(12.0f, 1.35f, 1.0f);
        glColor3f(0.10f, 0.16f, 0.22f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.5f, -0.5f, 0.0f);
            glVertex3f( 0.5f, -0.5f, 0.0f);
            glVertex3f( 0.5f,  0.5f, 0.0f);
            glVertex3f(-0.5f,  0.5f, 0.0f);
        glEnd();
    glPopMatrix();

    if (luzLigada)
        glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.82f, 0.48f);
    glRasterPos3f(-4.75f, 5.70f, SALA_FUNDO_Z + 0.095f);
    {
        const char *texto = "MUSEU DA HISTORIA DO FUTEBOL";
        while (*texto != '\0')
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *texto);
            ++texto;
        }
    }
    if (luzLigada)
        glEnable(GL_LIGHTING);
}

static void desenhaAmbiente(void)
{
    glPushMatrix();
        glColor3f(0.34f, 0.32f, 0.29f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-SALA_LIMITE_X, 0.0f, SALA_FUNDO_Z);
            glVertex3f(-SALA_LIMITE_X, 0.0f, SALA_FRENTE_Z);
            glVertex3f( SALA_LIMITE_X, 0.0f, SALA_FRENTE_Z);
            glVertex3f( SALA_LIMITE_X, 0.0f, SALA_FUNDO_Z);
        glEnd();
    glPopMatrix();

    /* Tapete central: pequeno deslocamento vertical evita conflito com o piso. */
    glPushMatrix();
        glTranslatef(0.0f, 0.012f, 0.8f);
        glScalef(13.0f, 1.0f, 4.2f);
        glColor3f(0.37f, 0.12f, 0.10f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-0.5f, 0.0f, -0.5f);
            glVertex3f(-0.5f, 0.0f,  0.5f);
            glVertex3f( 0.5f, 0.0f,  0.5f);
            glVertex3f( 0.5f, 0.0f, -0.5f);
        glEnd();
    glPopMatrix();

    /* Paredes de fundo, esquerda e direita; todas voltadas para o interior. */
    desenhaParede(-SALA_LIMITE_X, SALA_FUNDO_Z,
                   SALA_LIMITE_X, SALA_FUNDO_Z,
                   0.0f, 0.0f, 1.0f, 0.24f, 0.27f, 0.30f);
    desenhaParede(-SALA_LIMITE_X, SALA_FRENTE_Z,
                  -SALA_LIMITE_X, SALA_FUNDO_Z,
                   1.0f, 0.0f, 0.0f, 0.19f, 0.22f, 0.25f);
    desenhaParede(SALA_LIMITE_X, SALA_FUNDO_Z,
                  SALA_LIMITE_X, SALA_FRENTE_Z,
                  -1.0f, 0.0f, 0.0f, 0.19f, 0.22f, 0.25f);

    /* Parede frontal com abertura central para a porta. */
    glPushMatrix();
        glColor3f(0.19f, 0.22f, 0.25f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(-SALA_LIMITE_X, 0.0f, SALA_FRENTE_Z);
            glVertex3f(-SALA_LIMITE_X, 5.5f, SALA_FRENTE_Z);
            glVertex3f(-1.35f, 5.5f, SALA_FRENTE_Z);
            glVertex3f(-1.35f, 0.0f, SALA_FRENTE_Z);

            glVertex3f(1.35f, 0.0f, SALA_FRENTE_Z);
            glVertex3f(1.35f, 5.5f, SALA_FRENTE_Z);
            glVertex3f(SALA_LIMITE_X, 5.5f, SALA_FRENTE_Z);
            glVertex3f(SALA_LIMITE_X, 0.0f, SALA_FRENTE_Z);

            glVertex3f(-SALA_LIMITE_X, 5.5f, SALA_FRENTE_Z);
            glVertex3f(-SALA_LIMITE_X, SALA_ALTURA, SALA_FRENTE_Z);
            glVertex3f(SALA_LIMITE_X, 5.5f, SALA_FRENTE_Z);
            glVertex3f(SALA_LIMITE_X, SALA_ALTURA, SALA_FRENTE_Z);
        glEnd();
    glPopMatrix();

    /* Faixas de rodape nas tres paredes solidas. */
    desenhaBloco(0.0f, 0.16f, SALA_FUNDO_Z + 0.06f,
                 20.0f, 0.32f, 0.12f, 0.0f, 0.46f, 0.35f, 0.20f);
    desenhaBloco(-SALA_LIMITE_X + 0.06f, 0.16f, 0.0f,
                 0.12f, 0.32f, 16.0f, 0.0f, 0.46f, 0.35f, 0.20f);
    desenhaBloco(SALA_LIMITE_X - 0.06f, 0.16f, 0.0f,
                 0.12f, 0.32f, 16.0f, 0.0f, 0.46f, 0.35f, 0.20f);

    desenhaPlacaTitulo();
}

void configuraIluminacao(void)
{
    const GLfloat ambiente[] = {0.28f, 0.28f, 0.28f, 1.0f};
    const GLfloat difusa[] = {0.85f, 0.85f, 0.82f, 1.0f};
    const GLfloat especular[] = {0.75f, 0.75f, 0.75f, 1.0f};
    const GLfloat posicao[] = {0.0f, 6.5f, 1.0f, 1.0f};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao);
    // TODO Encontro 3: adicionar segunda fonte, spot e materiais refinados.
}

void desenhaPedestal(float x, float z)
{
    /* Cada bloco e transladado para a posicao indicada e escalado a sua dimensao. */
    glPushMatrix();
        glTranslatef(x, 0.0f, z);
        desenhaBloco(0.0f, 0.12f, 0.0f, 2.45f, 0.24f, 1.65f,
                     0.0f, 0.22f, 0.23f, 0.24f);
        desenhaBloco(0.0f, 0.68f, 0.0f, 1.72f, 0.92f, 1.10f,
                     0.0f, 0.30f, 0.31f, 0.32f);
        desenhaBloco(0.0f, 1.24f, 0.0f, 2.30f, 0.22f, 1.48f,
                     0.0f, 0.46f, 0.35f, 0.20f);
    glPopMatrix();
}

void desenhaPlaca(float x, float y, float z, const char *texto)
{
    const GLboolean luzLigada = glIsEnabled(GL_LIGHTING);
    const char *letra;

    glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(2.15f, 0.38f, 1.0f);
        glColor3f(0.08f, 0.12f, 0.16f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.5f, -0.5f, 0.0f);
            glVertex3f( 0.5f, -0.5f, 0.0f);
            glVertex3f( 0.5f,  0.5f, 0.0f);
            glVertex3f(-0.5f,  0.5f, 0.0f);
        glEnd();
    glPopMatrix();

    if (luzLigada)
        glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.82f, 0.48f);
    glRasterPos3f(x - 0.72f, y - 0.045f, z + 0.012f);
    letra = texto;
    while (*letra != '\0')
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *letra);
        ++letra;
    }
    if (luzLigada)
        glEnable(GL_LIGHTING);
}

static void desenhaCabide(void)
{
    /* A haste cilindrica e a barra em cubo formam o cabide atras da camisa. */
    desenhaBloco(0.0f, 1.60f, -0.14f, 1.42f, 0.07f, 0.07f,
                 0.0f, 0.50f, 0.34f, 0.16f);
    glColor3f(0.50f, 0.34f, 0.16f);
    desenhaCilindroY(0.0f, 1.62f, -0.14f, 0.035f, 0.035f, 0.28f);
}

static void desenhaEscudo(void)
{
    glPushMatrix();
        glTranslatef(0.0f, 1.10f, 0.145f);
        glScalef(0.20f, 0.20f, 0.045f);
        glColor3f(0.08f, 0.08f, 0.08f);
        glutSolidSphere(1.0, 20, 12);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 1.10f, 0.158f);
        glScalef(0.145f, 0.145f, 0.035f);
        glColor3f(0.90f, 0.88f, 0.82f);
        glutSolidSphere(1.0, 20, 12);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, 1.10f, 0.169f);
        glScalef(0.065f, 0.065f, 0.025f);
        glColor3f(0.78f, 0.08f, 0.07f);
        glutSolidSphere(1.0, 16, 10);
    glPopMatrix();
}

void desenhaCamisaCorinthians(void)
{
    glPushMatrix();
        /* Transformacao local: centra a camisa e preserva a escala do modelo. */
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);

        desenhaCabide();
        desenhaBloco(0.0f, 0.88f, 0.0f, 1.18f, 1.38f, 0.24f,
                     0.0f, 0.94f, 0.93f, 0.88f);
        desenhaBloco(-0.78f, 1.34f, 0.0f, 0.48f, 0.62f, 0.25f,
                     -28.0f, 0.94f, 0.93f, 0.88f);
        desenhaBloco(0.78f, 1.34f, 0.0f, 0.48f, 0.62f, 0.25f,
                     28.0f, 0.94f, 0.93f, 0.88f);
        desenhaBloco(-0.78f, 1.08f, 0.132f, 0.43f, 0.09f, 0.025f,
                     -28.0f, 0.08f, 0.08f, 0.08f);
        desenhaBloco(0.78f, 1.08f, 0.132f, 0.43f, 0.09f, 0.025f,
                     28.0f, 0.08f, 0.08f, 0.08f);
        desenhaBloco(-0.53f, 0.88f, 0.13f, 0.08f, 1.24f, 0.025f,
                     0.0f, 0.08f, 0.08f, 0.08f);
        desenhaBloco(0.53f, 0.88f, 0.13f, 0.08f, 1.24f, 0.025f,
                     0.0f, 0.08f, 0.08f, 0.08f);

        glPushMatrix();
            glTranslatef(0.0f, 1.58f, 0.0f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.08f, 0.08f, 0.08f);
            glutSolidTorus(0.035, 0.20, 12, 24);
        glPopMatrix();
        desenhaEscudo();
    glPopMatrix();
}

void desenhaCamisaAtletico(void)
{
    int i;

    glPushMatrix();
        /* A camisa fica no plano XY; a face frontal aponta para +Z. */
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);

        desenhaCabide();
        desenhaBloco(0.0f, 0.88f, 0.0f, 1.18f, 1.38f, 0.24f,
                     0.0f, 0.95f, 0.94f, 0.90f);

        /* Cinco faixas pretas estreitas sobre a frente branca do corpo. */
        for (i = 0; i < 5; ++i)
        {
            const float faixaX = -0.42f + (float)i * 0.21f;
            desenhaBloco(faixaX, 0.88f, 0.132f, 0.105f, 1.30f, 0.025f,
                         0.0f, 0.06f, 0.06f, 0.07f);
        }

        desenhaBloco(-0.78f, 1.34f, 0.0f, 0.48f, 0.62f, 0.25f,
                     -28.0f, 0.95f, 0.94f, 0.90f);
        desenhaBloco(0.78f, 1.34f, 0.0f, 0.48f, 0.62f, 0.25f,
                     28.0f, 0.95f, 0.94f, 0.90f);
        desenhaBloco(-0.78f, 1.08f, 0.132f, 0.43f, 0.09f, 0.025f,
                     -28.0f, 0.06f, 0.06f, 0.07f);
        desenhaBloco(0.78f, 1.08f, 0.132f, 0.43f, 0.09f, 0.025f,
                     28.0f, 0.06f, 0.06f, 0.07f);

        glPushMatrix();
            glTranslatef(0.0f, 1.58f, 0.0f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.06f, 0.06f, 0.07f);
            glutSolidTorus(0.035, 0.20, 12, 24);
        glPopMatrix();
    glPopMatrix();
}

void desenhaTacaLibertadores(void)
{
    /* Os cilindros GLU ficam verticais; a copa e as alcas compoem a taca. */
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);

        glColor3f(0.45f, 0.28f, 0.08f);
        desenhaCilindroY(0.0f, 0.05f, 0.0f, 0.36f, 0.36f, 0.18f);
        glColor3f(0.78f, 0.55f, 0.16f);
        desenhaCilindroY(0.0f, 0.23f, 0.0f, 0.26f, 0.26f, 0.16f);
        desenhaCilindroY(0.0f, 0.39f, 0.0f, 0.12f, 0.12f, 0.48f);

        glColor3f(0.86f, 0.65f, 0.20f);
        desenhaCilindroY(0.0f, 0.86f, 0.0f, 0.22f, 0.49f, 0.65f);

        glPushMatrix();
            glTranslatef(-0.44f, 1.22f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            glutSolidTorus(0.055, 0.24, 12, 24);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.44f, 1.22f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            glutSolidTorus(0.055, 0.24, 12, 24);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.0f, 1.51f, 0.0f);
            glColor3f(0.96f, 0.75f, 0.26f);
            glutSolidTorus(0.06, 0.48, 12, 28);
        glPopMatrix();
    glPopMatrix();
}

static void desenhaExposicoes(void)
{
    desenhaPedestal(POS_CORINTHIANS_X, POS_OBRAS_Z);
    desenhaPedestal(POS_ATLETICO_X, POS_OBRAS_Z);
    desenhaPedestal(POS_TACA_X, POS_OBRAS_Z);

    desenhaPlaca(POS_CORINTHIANS_X, 1.03f, POS_OBRAS_Z + 0.84f,
                 "CORINTHIANS");
    desenhaPlaca(POS_ATLETICO_X, 1.03f, POS_OBRAS_Z + 0.84f,
                 "ATLETICO MINEIRO");
    desenhaPlaca(POS_TACA_X, 1.03f, POS_OBRAS_Z + 0.84f,
                 "LIBERTADORES");

    /* Ordem: posiciona, gira em torno do eixo vertical e escala a obra. */
    glPushMatrix();
        glTranslatef(POS_CORINTHIANS_X, ALTURA_PEDESTAL, POS_OBRAS_Z);
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        desenhaCamisaCorinthians();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(POS_ATLETICO_X, ALTURA_PEDESTAL, POS_OBRAS_Z);
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        desenhaCamisaAtletico();
    glPopMatrix();

    glPushMatrix();
        /* O deslocamento antes/depois da rotacao define o pivô no meio da taca. */
        glTranslatef(POS_TACA_X, ALTURA_PEDESTAL + 0.82f, POS_OBRAS_Z);
        glRotatef(anguloAnimacao, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, -0.82f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        desenhaTacaLibertadores();
    glPopMatrix();
}

void desenhaCena(void)
{
    glMatrixMode(GL_MODELVIEW);
    configuraIluminacao();
    desenhaAmbiente();
    desenhaExposicoes();
}

void inicializaCena(void)
{
    glShadeModel(GL_SMOOTH);

    if (quadric == NULL)
    {
        quadric = gluNewQuadric();
        if (quadric != NULL)
            gluQuadricNormals(quadric, GLU_SMOOTH);
        else
            fprintf(stderr, "Erro: nao foi possivel criar o GLUquadric.\n");
    }

    ultimoTempoAnimacao = glutGet(GLUT_ELAPSED_TIME);
}

void alternaAnimacao(void)
{
    animacaoLigada = !animacaoLigada;
    ultimoTempoAnimacao = glutGet(GLUT_ELAPSED_TIME);
}

void atualizaAnimacao(void)
{
    const int tempoAtual = glutGet(GLUT_ELAPSED_TIME);

    if (animacaoLigada)
    {
        const int deltaMs = tempoAtual - ultimoTempoAnimacao;
        anguloAnimacao += PASSOS_ANIMACAO * (float)deltaMs / 1000.0f;
        if (anguloAnimacao >= 360.0f)
            anguloAnimacao = fmodf(anguloAnimacao, 360.0f);
    }

    ultimoTempoAnimacao = tempoAtual;
}

void alternaIluminacao(void)
{
    // TODO Encontro 3: alternar a iluminacao global sem remover a luz base.
}

void usaFlat(void)
{
    // TODO Encontro 3: implementar GL_FLAT.
}

void usaSmooth(void)
{
    // TODO Encontro 3: implementar GL_SMOOTH.
}
