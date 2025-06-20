#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h> // basename

#include <sys/time.h> // gettimeofday localtime
#include <time.h>

#include "gfx_ptBR.h"

#include "readMdl.h"
#include "3d.h"

#include "render.h"

char paleta[256][3];
float oldTS = 0;

void msg(char *out) {
    struct timeval tv;
    struct tm *timeinfo;
    char buffDataHora[64];
	float newTS;
	float tempoDesdeUltimaMsg = 0.0;

    gettimeofday(&tv, NULL);
    timeinfo = localtime(&tv.tv_sec);
    
    strftime(buffDataHora, 64, "%H:%M:%S", timeinfo);

	newTS = (timeinfo->tm_hour * 3600) + (timeinfo->tm_min * 60) + timeinfo->tm_sec + 
			((float)tv.tv_usec / 1000000.0);

	if (oldTS > 0) {
		tempoDesdeUltimaMsg = newTS - oldTS;
	}
	oldTS = newTS;

	printf("[%s.%03ld] [%f / %f] > [%s]\n", buffDataHora, tv.tv_usec / 1000, tempoDesdeUltimaMsg, 1.0/tempoDesdeUltimaMsg, out);
}

int main(int argc, char **argv)
{
	int janX = 720, janY = 720, totAnims;
	camera_t cam;

	msg("Quake MDL Viewer");

	if (argc < 4 || strlen(argv[1]) < 4) {
		msg("Uso: mdlViewer ARQUIVO1.mdl ARQUIVO2.mdl ARQUIVO3.mdl");
		exit(1);
	}

	obj3d_t *chao = obj_plano(10, 10);

	obj3d_t *obj = readMdl(argv[1]);
	if (!obj) {
		msg("Falha ao carregar ARQUIVO.mdl");
		exit(2);
	}

	obj3d_t *obj2 = readMdl(argv[2]);
	if (!obj2) {
		msg("Falha ao carregar ARQUIVO2.mdl");
		exit(2);
	}

	obj3d_t *obj3 = readMdl(argv[3]);
	if (!obj3) {
		msg("Falha ao carregar ARQUIVO3.mdl");
		exit(2);
	}

	// Use current time as
	// seed for random generator
	srand(time(0));

	FILE *fpPal = fopen("data/paleta", "rb");
	if(!fpPal) {
		msg("Paleta nao encontrada!");
		exit(99);
	}
	int pRead = fread(&paleta, 1, 768, fpPal);
	fclose(fpPal);

	char tituloJanela[128];
	sprintf(tituloJanela, "MDL VIEWER - %s", basename(argv[1]));

	int erro = grafico_init(janX, janY, tituloJanela);
	if (erro) {
		msg("Erro ao inicializar graficos");
		exit(33);
	}

	int numAnimSel = 0;
	int numAnimSelAuto = 2;
	int numFrameSel = 0;
	int numFrameSel2 = 0;
	int numFrameSel3 = 0;
	char out[256];

	chao->rotacao.y = 55;

	obj->posicao.x  = 0;
	obj->posicao.y  = -45;
	obj->posicao.z  = 0;
	obj->rotacao.x = 0;

	obj2->posicao.x  = 40;
	obj2->posicao.y  = -46;
	obj2->posicao.z  = 0;
	obj2->rotacao.x = 0;

	obj3->posicao.x  = -40;
	obj3->posicao.y  = -27;
	obj3->posicao.z  = 0;
	obj3->rotacao.x = 0;

	cam.pos.x = 0;
	cam.pos.y = 30;
	cam.pos.z = 100;

	cam.ang.x = 0;
	cam.ang.y = 0;
	cam.ang.z = 0;

	// obj->tipo = OBJ_TIPO_WIRE;

	printf("Init!\n");
	// grafico_tecla_espera();
	// printf("FOI!\n");

	grafico_desenha_objeto(&cam, obj, numFrameSel, paleta);

	grafico_desenha_objeto(&cam, chao, 0, NULL);

	// grafico_tecla_espera();
	// printf("FOI 2!\n");

	grafico_mostra();

	grafico_tecla_espera();
	
	while (1)
	{
		grafico_desenha_objeto(&cam, chao, 0, NULL);


		char *framename = &obj->framenames[numFrameSel * 16];
		grafico_desenha_objeto(&cam, obj, numFrameSel, paleta);

		grafico_desenha_objeto(&cam, obj2, numFrameSel2, paleta);
		numFrameSel2++;
		if(numFrameSel2 >= obj2->numframes -1)
			numFrameSel2 = 0;

		grafico_desenha_objeto(&cam, obj3, numFrameSel3, paleta);
		numFrameSel3++;
		if(numFrameSel3 >= 8)
			numFrameSel3 = 0;

		obj->rotacao.y++;
		if(obj->rotacao.y >= 360)
			obj->rotacao.y = 0;
		obj2->rotacao.y--;
		if(obj2->rotacao.y <= 0)
			obj2->rotacao.y = 360;
		// obj3->rotacao.z--;
		// if(obj3->rotacao.z <= 0)
		// 	obj3->rotacao.z = 360;
		chao->rotacao.y--;
		if(chao->rotacao.y <= 0)
			chao->rotacao.y = 360;

		grafico_mostra();

		sprintf(out, "Mostrando frame[%d]: %s > [%d]", numFrameSel, framename, (int)obj3->posicao.y);
		msg(out);

		numFrameSel++;

		int naSel = (numAnimSel == -1) ? numAnimSelAuto : numAnimSel;
		if (numFrameSel >= obj->framesanims[naSel].frameF) {
			if (numAnimSel == -1) {
				numAnimSelAuto = rand() % obj->totAnims;
				numFrameSel = obj->framesanims[numAnimSelAuto].frameI;
			} else {
				numFrameSel = obj->framesanims[naSel].frameI;
			}
		}

		// Wait for the user to press a character.
		char c = grafico_tecla();

		// Quit if it is the letter q.
		if (c == 'q')
			break;

		if (c == '\\') {
			numAnimSel--;
			if (numAnimSel < -1)
				numAnimSel = -1;
			
			numFrameSel = obj->framesanims[numAnimSel].frameI;
		} else if (c == 'z') {
			numAnimSel++;
			if (numAnimSel >= obj->totAnims)
				numAnimSel = obj->totAnims - 1;

			numFrameSel = obj->framesanims[numAnimSel].frameI;
		} else if (c == 'y') {
			cam.pos.z++;
		} else if (c == 'h') {
			cam.pos.z--;
		} else if (c == 't') {
			obj3->posicao.y++;
		} else if (c == 'g') {
			obj3->posicao.y--;
		} else if (c == 'u') {
			cam.pos.y++;
		} else if (c == 'j') {
			cam.pos.y--;
		}

		if (numFrameSel < 0)
			numFrameSel = 0;
		if (numFrameSel >= obj->numframes)
			numFrameSel = obj->numframes - 1;
		
		//usleep(20000);
	}

	msg("Free Myke Tyson FREE");

	freeObj3D(obj3);
	freeObj3D(obj2);
	freeObj3D(obj);

	freeObj3D(chao);

	grafico_desliga();

	return 0;
}
