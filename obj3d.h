#ifndef OBJ3D_H
#define OBJ3D_H

#include "QuakeViewer.h"
#include "3d.h"
#include "obj3d.h"

typedef struct
{
    int r, g, b;
} cor_t;

typedef struct {
	int		onseam;
	int		s;
	int		t;
} skinvert_t;

typedef struct
{
    int v[2];
} edge_t;

typedef struct
{
    int v[3];
    vetor3d_t normal;
    char isFront;
    cor_t cor;
    int planenum;
    int texinfo;
} triangulo_t;

typedef struct
{
	char		name[16];
	unsigned	width, height;
    char        *data;
} texture_t;

typedef struct
{
    vetor3d_t   vetorS;
    float       distS;
    
    vetor3d_t   vetorT;
    float       distT;

	int			miptex;
	int			flags;
} textureinfo_t;

typedef struct
{
    int         id;
	short		side;

	int		   *firstledge;
	short		numedges;

    textureinfo_t   *texinfo;
	texture_t       *texture;

    plano_t    *plano;

    byte       *light;
    short       light_width, light_height;
    short       light_mins_s, light_mins_t;

    int         drawn;
} face_t;

typedef struct
{
	int frameI;
    int frameF;
} animationframes_t;

typedef struct mnode_s
{
// common with leaf
	int			contents;		// 0, to differentiate from leafs
	int			visofs;		// node needs to be traversed if current

	vetor3d_t	min;            // for bounding box culling
    vetor3d_t   max;

	struct mnode_s	*parent;

// node specific
	plano_t         *plane;
	struct mnode_s	*children[2];

	unsigned int		firstsurface;
	unsigned int		numsurfaces;
} node_t;



typedef struct mleaf_s
{
// common with node
	int			contents;		// wil be a negative contents number
	int			visofs;		// node needs to be traversed if current

	vetor3d_t	min;            // for bounding box culling
    vetor3d_t   max;

	struct mnode_s	*parent;

// leaf specific
	byte		*compressed_vis;
	// efrag_t		*efrags;

	face_t	**firstmarksurface;
	int       nummarksurfaces;

	int     key;			    // BSP sequence number for leaf's contents
	// byte		ambient_sound_level[NUM_AMBIENTS];
} leaf_t;

typedef struct
{
    char nome[64];

    int numverts;
    int numtris;
    int numframes;
    int totAnims;

    int numFrameSel;
    int numAnimSel;
    int numAnimSelAuto;

    int   skinwidth;
    int   skinheight;

    char       *skin;
    skinvert_t *skinmap;

    char        *framenames;
    vetor3d_t   *frames;
    triangulo_t *tris;

    ponto_t       *verts;

    animationframes_t *framesanims;

    vetor3d_t *trisnormals;

    // run
    vetor3d_t posicao;
    vetor3d_t rotacao;
} obj3d_t;

typedef struct
{
    char nome[64];

    vetor3d_t   player_start;
    int         player_start_angle;

    int tipo;
    int numverts;
    int numedges;
    int numledges;
    int numplanes;
    int numfaces;
    int nummarksurfaces;
    int numnodes;
    int numtextures;
    int numtexinfo;
    int numleafs;

    int numTextureTrigger;

    vetor3d_t bbMin, bbMax;

    vetor3d_t   *base;

    edge_t      *edges;
    int         *ledges;

    plano_t       *planes;
    face_t        *faces;
    face_t       **marksurfaces;
    texture_t     *textures;
    textureinfo_t *texinfo;
    node_t        *nodes;
    leaf_t        *leafs;

    char *lighting;
    int   lightinglen;

    char *entities;
    int   entitieslen;

    char *visibility;
    int   visibilitylen;

    ponto_t *verts;
} mapa_t;

void obj_inc_frame(obj3d_t *obj);
void obj_inc_anim(obj3d_t *obj);
void obj_dec_anim(obj3d_t *obj);

void freeObj3D(obj3d_t *obj);
void freeMapa3D(mapa_t *mapa);

void obj_calculate_face_normals(obj3d_t *obj);

void obj_projecao3D(camera_t *cam, obj3d_t *obj);

void mapa_projecao3D(camera_t *cam, mapa_t *mapa);

obj3d_t *obj_plano(int sizeX, int sizeY);

#endif
