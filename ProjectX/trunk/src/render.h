#ifndef RENDER_INCLUDED
#define RENDER_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

#include "main.h"
#include "new3d.h"
#include <windows.h>

#ifndef OPENGL
#include <d3d9.h>
#endif

BOOL  bSquareOnly;

typedef struct {
    DWORD       X;
    DWORD       Y;            /* Viewport Top left */
    DWORD       Width;
    DWORD       Height;       /* Viewport Dimensions */
    float       MinZ;         /* Min/max of clip Volume */
    float       MaxZ;
    float	ScaleX; 
    float	ScaleY; 
} render_viewport_t;

typedef struct {
    int     w;                /* width */
    int     h;                /* height */
    int     bpp;              /* bits per pixel */
	int		rate;			  /* refresh rate (HZ) */
} render_display_mode_t;

typedef struct
{
    long cx;
    long cy;
} CLIENTSIZE;

typedef struct {
    int                     NumModes;				/* number of available display modes */
    int                     CurrMode;				/* number of current display mode (only when fullscreen) */
    render_display_mode_t * Mode;					/* desc avail modes */
    render_display_mode_t   ThisMode;				/* description of this mode, identical to Mode[CurrMode] */
    render_display_mode_t   WindowsDisplay;			/* current Windows disply mode */
    render_display_mode_t   default_mode;			/* current Windows disply mode */
    BOOL                    bFullscreen;			/* in fullscreen exclusive mode? */
    CLIENTSIZE              szClient;				/* dimensions of client win */
    BOOL                    bPaused;				/* the app is paused */
    BOOL                    bAppActive;				/* the app is active */
    BOOL                    bMinimized;				/* app window is minimized */
    BOOL                    bRenderingIsOK;			/* All objects etc. necessary rendering are in ok */
	BOOL					vsync;					/* vertical sync */
#ifdef WIN32
	HWND					window;			        /* application window handle */
#else
	// linux ?
#endif
} render_info_t;

#undef RELEASE
#ifndef __cplusplus
#define RELEASE(x) {if (x != NULL) {x->lpVtbl->Release(x); x = NULL;}}
#else
#define RELEASE(x) {if (x != NULL) {x->Release(); x = NULL;}}
#endif

#undef MAX
#define MAX(x, y) ((x) > (y)) ? (x) : (y)

#undef MIN
#define MIN(x, y) ((x) > (y)) ? (y) : (x)

#undef ZEROMEM
#define ZEROMEM(x) memset(&x, 0, sizeof(x))

void cull_none( void );
void cull_cw( void );
void reset_cull( void );
void disable_zbuff( void );
void set_alpha_states( void );
void set_normal_states( void );
void reset_zbuff( void );
void reset_filtering( void );
void set_alpha_fx_states( void );

const char * render_error_description( HRESULT hr );

typedef struct {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
} XYRECT;

#define FSColourKeyBlack 0xFF000000 // pass this as colour key for black as transparent
BOOL FSClearBlack(void);
BOOL FSClear(DWORD Count, XYRECT* rect, DWORD Flags, DWORD Color, float Z, DWORD Stencil);

#define MAX_LEVEL_TEXTURE_GROUPS 8

#define MAX_TEXTURE_GROUPS 600

#define INCREASE_TEXTURE_GROUPS( group ) \
	assert( group->numTextureGroups < MAX_TEXTURE_GROUPS ); \
	group->numTextureGroups++;

typedef struct TEXTUREGROUP
{
	int startVert;
	int startIndex;
	int numVerts;
	int numTriangles;
	BOOL colourkey;
	LPDIRECT3DTEXTURE9 texture;
} TEXTUREGROUP;

typedef struct RENDEROBJECT
{
	LPDIRECT3DVERTEXBUFFER9 lpD3DVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	lpD3DIndexBuffer;
	D3DMATERIAL9 material;
	BOOL			vbLocked;
	int numTextureGroups;
	TEXTUREGROUP textureGroups[MAX_TEXTURE_GROUPS];
} RENDEROBJECT;

typedef struct LEVELRENDEROBJECT
{
	LPDIRECT3DVERTEXBUFFER9 lpD3DVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	lpD3DIndexBuffer;
	D3DMATERIAL9 material;
	BOOL				vbLocked;
	int numTextureGroups;
	TEXTUREGROUP textureGroups[MAX_LEVEL_TEXTURE_GROUPS];
} LEVELRENDEROBJECT;

BOOL render_flip( render_info_t * info );
HRESULT FSGetViewPort(render_viewport_t *returnViewPort);
HRESULT FSBeginScene();
HRESULT FSEndScene();
HRESULT FSCreateDynamicVertexBuffer(RENDEROBJECT *renderObject, int numVertices);
HRESULT FSCreateVertexBuffer(RENDEROBJECT *renderObject, int numVertices);
HRESULT FSLockVertexBuffer(RENDEROBJECT *renderObject, LVERTEX **verts);
HRESULT FSUnlockVertexBuffer(RENDEROBJECT *renderObject);
HRESULT FSCreateDynamicIndexBuffer(RENDEROBJECT *renderObject, int numIndices);
HRESULT FSCreateIndexBuffer(RENDEROBJECT *renderObject, int numIndices);
HRESULT FSLockIndexBuffer(RENDEROBJECT *renderObject, WORD **indices);
HRESULT FSUnlockIndexBuffer(RENDEROBJECT *renderObject);
void FSReleaseRenderObject(RENDEROBJECT *renderObject);
HRESULT draw_object(RENDEROBJECT *renderObject);
HRESULT FSSetViewPort(render_viewport_t *newViewPort);
HRESULT FSGetViewport(render_viewport_t *returnViewPort);
HRESULT FSSetViewPort(render_viewport_t *newViewPort);
HRESULT FSSetMatrix(D3DTRANSFORMSTATETYPE type, const D3DMATRIX *matrix);
HRESULT FSGetMatrix(D3DTRANSFORMSTATETYPE type, D3DMATRIX *matrix);
HRESULT FSSetMaterial(const D3DMATERIAL9 *material);
HRESULT FSCreateTexture(LPDIRECT3DTEXTURE9 *texture, const char *fileName, uint16 *width, uint16 *height, int numMips, BOOL * colourkey);
HRESULT update_texture_from_file(LPDIRECT3DTEXTURE9 dstTexture, const char *fileName, uint16 *width, uint16 *height, int numMips, BOOL * colourkey);
HRESULT draw_line_vertex_buffer(RENDEROBJECT *renderObject);
HRESULT FSUnlockPretransformedVertexBuffer(RENDEROBJECT *renderObject);
HRESULT FSLockPretransformedVertexBuffer(RENDEROBJECT *renderObject, LPTLVERTEX **verts);
HRESULT FSCreateDynamic2dVertexBuffer(RENDEROBJECT *renderObject, int numVertices);
HRESULT draw_line_object(RENDEROBJECT *renderObject);
HRESULT draw_object(RENDEROBJECT *renderObject);
HRESULT draw_2d_object(RENDEROBJECT *renderObject);

#ifdef __cplusplus
};
#endif

// this is used to read the file because the file is encoded with dwReserved
typedef struct {
    union {
		float     x;             /* Homogeneous coordinates */
		float     dvX;
    };
    union {
		float     y;
		float     dvY;
    };
    union {
		float     z;
		float     dvZ;
    };
    DWORD            dwReserved;
    union {
		COLOR     color;         /* Vertex color */
		COLOR     dcColor;
    };
    union {
		COLOR     specular;      /* Specular component of vertex */
		COLOR     dcSpecular;
    };
    union {
		float     tu;            /* Texture coordinates */
		float     dvTU;
    };
    union {
		float     tv;
		float     dvTV;
    };
} *LPOLDLVERTEX;

/* bjd - remove when done. taken from SDK */
#define D3DCLIP_LEFT                0x00000001L
#define D3DCLIP_RIGHT               0x00000002L
#define D3DCLIP_TOP					0x00000004L
#define D3DCLIP_BOTTOM              0x00000008L
#define D3DCLIP_FRONT               0x00000010L
#define D3DCLIP_BACK                0x00000020L

#endif // RENDER_INCLUDED