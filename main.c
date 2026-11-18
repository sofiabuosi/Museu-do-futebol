#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "camera.h"
#include "scene.h"

static int largura = 1000;
static int altura = 700;

static void desenho(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    configuraProjecao(largura, altura);
    aplicaCamera();
    desenhaCena();

    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    largura = (w > 0) ? w : 1;
    altura = (h > 0) ? h : 1;
    glViewport(0, 0, largura, altura);
}

static void idle(void)
{
    atualizaAnimacao();
    glutPostRedisplay();
}

static void teclado(unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla)
    {
        case 'w':
        case 'W':
            moveFrente(0.35f);
            break;
        case 's':
        case 'S':
            moveFrente(-0.35f);
            break;
        case 'a':
        case 'A':
            moveLateral(-0.35f);
            break;
        case 'd':
        case 'D':
            moveLateral(0.35f);
            break;
        case 'q':
        case 'Q':
            moveVertical(0.30f);
            break;
        case 'e':
        case 'E':
            moveVertical(-0.30f);
            break;
        case 'p':
        case 'P':
            usaPerspectiva();
            break;
        case 'o':
        case 'O':
            usaOrtografica();
            break;
        case ' ':
            alternaAnimacao();
            break;
        case 27:
            exit(0);
            break;
        default:
            break;
    }

    // TODO Encontro 3: usar L para alternar os controles de iluminacao.
    glutPostRedisplay();
}

static void tecladoEspecial(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla)
    {
        case GLUT_KEY_LEFT:
            giraYaw(-4.0f);
            break;
        case GLUT_KEY_RIGHT:
            giraYaw(4.0f);
            break;
        case GLUT_KEY_UP:
            giraPitch(4.0f);
            break;
        case GLUT_KEY_DOWN:
            giraPitch(-4.0f);
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

static void inicializa(void)
{
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    inicializaCamera();
    inicializaCena();
}

static void imprimeComandos(void)
{
    printf("Controles do Museu 3D:\n");
    printf("W/S: frente/tras | A/D: esquerda/direita | Q/E: subir/descer\n");
    printf("Setas: girar a camera | P: perspectiva | O: ortografica\n");
    printf("Espaco: iniciar/pausar a animacao | ESC: sair\n");
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(largura, altura);
    glutCreateWindow("Museu da Historia do Futebol");

    inicializa();
    imprimeComandos();

    glutDisplayFunc(desenho);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(tecladoEspecial);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
