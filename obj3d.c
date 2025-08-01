#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "3d.h"
#include "grafico.h"
#include "mapa.h"
#include "readMdl.h"
#include "render.h"

void obj_calculate_offsetChao(obj3d_t *obj)
{
    // TODO calcular do 1o frame!!
    obj->offsetChao = 25;
}

void obj_calculate_face_normals(obj3d_t *obj)
{
    obj->trisnormals = malloc(obj->numframes * obj->numtris * sizeof(vetor3d_t));
    if (!obj->trisnormals) {
        // TODO
        return;
    }

    for (int numFrame=0; numFrame < obj->numframes; numFrame++) {
        int offsetFrameVert = numFrame * obj->numverts;
        int offsetFrameTri  = numFrame * obj->numtris;

        for (int numTri=0; numTri < obj->numtris; numTri++) {
            triangulo_t *tri = &obj->tris[numTri];

            vetor3d_t *v1 = &obj->frames[offsetFrameVert + tri->v[0]];
            vetor3d_t *v2 = &obj->frames[offsetFrameVert + tri->v[1]];
            vetor3d_t *v3 = &obj->frames[offsetFrameVert + tri->v[2]];

            vetor3d_t a = { v3->x - v1->x, v3->y - v1->y, v3->z - v1->z };
            vetor3d_t b = { v2->x - v1->x, v2->y - v1->y, v2->z - v1->z };

            vetor3d_t normal = vetor_cross_product(a, b);
            vetor_normalize(&normal);

            vetor3d_t *normalObj = &obj->trisnormals[offsetFrameTri + numTri];
            normalObj->x = normal.x;
            normalObj->y = normal.y;
            normalObj->z = normal.z;
        }
    }
}

void mapa_projecao3D(camera_t *cam, mapa_t *mapa)
{
    vetor3d_t *base = mapa->base;
    ponto_t   *pnt  = mapa->verts;

    for (int v=0; v < mapa->numverts; v++, base++, pnt++) {
        // Reset - Coordenadas de Objeto
        pnt->rot.x = base->x - cam->pos.x;
        pnt->rot.y = base->y - cam->pos.y;
        pnt->rot.z = base->z - cam->pos.z;

        // Rotacao de Camera - coordenadas de camera
        vetor_rotacao2DEixoX(&pnt->rot, cam->ang.x);
        vetor_rotacao2DEixoY(&pnt->rot, cam->ang.y);
        vetor_rotacao2DEixoZ(&pnt->rot, cam->ang.z);

        // Projecao para 2D
        // TODO projetar apenas depois do near clipping
        grafico_projecao3D(pnt);
    }

    // Projetar as normais dos planos e fazer backface culling
    float dot;
    vetor3d_t view_vector;
    plano_t *plano = mapa->planes;
    for (int p = 0; p < mapa->numplanes; p++, plano++) {
        // vetor do ponto da câmera até o ponto do plano
        view_vector.x = plano->ponto.x - cam->pos.x;
        view_vector.y = plano->ponto.y - cam->pos.y;
        view_vector.z = plano->ponto.z - cam->pos.z;

        // produto escalar no espaço do mundo
        plano->ON = (vetor_dot_product(plano->normal, view_vector) < 0) ? 1 : 0;  // visível se a câmera vê a frente do plano
    }

    // face_t *face = mapa->faces;
    // for (int f=0; f < mapa->numfaces; f++, face++) {
    //     // Reset - Coordenadas de Objeto
    //     face->normal.x = face->plano->normal.x - cam->pos.x;
    //     face->normal.y = face->plano->normal.y - cam->pos.y;
    //     face->normal.z = face->plano->normal.z - cam->pos.z;

    //     // Rotacao de Camera - coordenadas de camera
    //     rotacao2DEixoX(&face->normal, cam->ang.x);
    //     rotacao2DEixoY(&face->normal, cam->ang.y);
    //     rotacao2DEixoZ(&face->normal, cam->ang.z);

    //     normalize(&face->normal);
    // }
}

obj3d_t *obj_plano(int sizeX, int sizeY)
{
    obj3d_t *ret;

    int numtris  = (sizeX - 1) * 2 * (sizeY - 1);
    int numverts = (sizeX * sizeY);

    int totMemObj = sizeof(obj3d_t) +                 // ret
                    (numtris * sizeof(triangulo_t)) + // ret->tris
                    (sizeof(frameinfo_t)) +           // ret->frameinfo
                    (numverts * sizeof(vetor3d_t)) +  // ret->frames
                    (numverts * sizeof(ponto_t));       // ret->verts

    ret = calloc(1, totMemObj);
    if (!ret) {
        printf("Erro malloc!\n\n");
        return NULL;
    }

    strcpy(ret->nome, "Plano");
    ret->numframes = 1;
    ret->numverts  = numverts;
    ret->numtris   = numtris;
    ret->offsetChao= 0;

    ret->tris       = (triangulo_t *) &ret[1];
    ret->frameinfo  = (frameinfo_t *) &ret->tris[numtris];
    ret->frames     = (vetor3d_t *)   &ret->frameinfo[1];
    ret->verts      = (ponto_t *)     &ret->frames[numverts];
    ret->framesanims= NULL;

    strcpy(ret->frameinfo->nome, "Plano01");

    // VXTs
    int PX, PZ = 0 - ((sizeY / 2) * 10);
    for (int Y=0; Y < sizeY; Y++) {
        PX = 0 - ((sizeX / 2) * 10);
        for (int X=0; X < sizeX; X++) {
            vetor3d_t *p = &ret->frames[X + Y * sizeX];

            p->x = PX;
            p->y = 0;
            p->z = PZ;

            PX += 10;
        }
        PZ += 10;
    }

    // TRIs
    cor_t cor_branco = { 255,255,255 };
    cor_t cor_cinza  = { 155,155,155 };
    for (int Y=0; Y < sizeY - 1; Y++) {
        for (int X=0; X < sizeX - 1; X++) {
            triangulo_t *tri1 = &ret->tris[(X*2) + Y * ((sizeX - 1) * 2)];
            triangulo_t *tri2 = &ret->tris[(X*2) + Y * ((sizeX - 1) * 2) + 1];

            tri1->cor  = cor_branco;
            tri1->v[0] = X + Y * sizeX;
            tri1->v[1] = X + Y * sizeX + 1;
            tri1->v[2] = X + (Y + 1) * sizeX;

            tri2->cor  = cor_cinza;
            tri2->v[0] = X + Y * sizeX + 1;
            tri2->v[1] = X + (Y + 1) * sizeX + 1;
            tri2->v[2] = X + (Y + 1) * sizeX;
        }
    }

    // Normals
    obj_calculate_face_normals(ret);

    return ret;
}

void freeObj3D(obj3d_t *obj)
{
    if (!obj) return;
    
    if(obj->trisnormals)
        free(obj->trisnormals);
    
    if(obj->framesanims)
        free(obj->framesanims);
    
    free(obj);
}

void freeMapa3D(mapa_t *mapa)
{
    if (!mapa) return;

    if (mapa->numtextures && mapa->textures) {
        texture_t *tex = mapa->textures;
        for (int i=0; i < mapa->numtextures; i++, tex++)
            if (tex->data) free(tex->data);
        free(mapa->textures);
    }
    
    if(mapa->base)
        free(mapa->base);
    
    if(mapa->edges)
        free(mapa->edges);
    
    if(mapa->ledges)
        free(mapa->ledges);

    if(mapa->planes)
        free(mapa->planes);
    
    if(mapa->faces)
        free(mapa->faces);
    
    if(mapa->nodes)
        free(mapa->nodes);
    
    if(mapa->lighting)
        free(mapa->lighting);
    
    if(mapa->texinfo)
        free(mapa->texinfo);

    if(mapa->verts)
        free(mapa->verts);

    if(mapa->entities)
        free(mapa->entities);

    if(mapa->leafs)
        free(mapa->leafs);
    
    if(mapa->visibility)
        free(mapa->visibility);
    
    free(mapa);
}
