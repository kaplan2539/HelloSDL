#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <fstream>
#include <string>

//---------------------------------------------------------------------------

const int     g_w     = 480;
const int     g_h     = 272;
int           g_scale = 1;
int           g_xoff  = 0;
int           g_yoff  = 0;

int           g_cell_size   = 4; //cell size in pixels
int           g_cell_border = 1; //cell border in pixels

int           g_player_col  = 0;
int           g_player_row  = 0;

int           g_world_cols = 0;
int           g_world_rows = 0;
int          *g_world;
int          *g_world_next;

SDL_Window*   g_pWindow   = 0;
SDL_Renderer* g_pRenderer = 0;
bool          g_bQuit     = false;
bool          g_bPause    = true;
int           g_iSteps    = 0;

typedef struct Stamp {
  int w;
  int h;
  int *d;
} Stamp;

int   g_cur_stamp=1;
const int   n_stamps = 3;
Stamp g_stamps[n_stamps];

int g_glider[] ={ 0, 1, 0,
                  0, 0, 1,
                  1, 1, 1 };

int g_block[] ={ 1, 1,
                 1, 1 };

int g_single[] ={ 1 };


int g_color=1;

//---------------------------------------------------------------------------

int readCells( const std::string &fname, Stamp &s )
{
  std::string line;
  std::ifstream f(fname.c_str());
  if(f.is_open())
  {
    while ( getline(f,line) )
    {
    }
    f.close();
  }
  else {
    std::cout << "Unable to open file\n"; 
    return 0;
  }

  return 1;
}

//---------------------------------------------------------------------------

void initialize()
{
  // initialize SDL
  if(SDL_Init(SDL_INIT_EVERYTHING) >= 0)
  {
    // if succeeded create our window
    g_pWindow = SDL_CreateWindow("HelloSDL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        g_w, g_h,
        SDL_WINDOW_SHOWN);
    // if the window creation succeeded create our renderer
    if(g_pWindow != 0)
    {
      g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
    }

    //initialize PNG loading
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
      exit(1);
    }
  }
  else
  {
    exit(1); // sdl could not initialize
  }

  g_world_cols =  g_w / (g_cell_size + g_cell_border);
  g_xoff       = (g_w % (g_cell_size + g_cell_border)) / 2;

  g_world_rows =  g_h / (g_cell_size + g_cell_border);
  g_yoff       = (g_h % (g_cell_size + g_cell_border)) / 2;

  g_world      = new int[g_world_cols*g_world_rows];
  g_world_next = new int[g_world_cols*g_world_rows];

  for(int i=0; i<g_world_cols*g_world_rows; i++ )
  {
    g_world[i] = 0;
    g_world_next[i] = 0;
  }

  g_stamps[0].w = 3;
  g_stamps[0].h = 3;
  g_stamps[0].d = g_glider;

  g_stamps[1].w = 2;
  g_stamps[1].h = 2;
  g_stamps[1].d = g_block;
  
  g_stamps[2].w = 1;
  g_stamps[2].h = 1;
  g_stamps[2].d = g_single;


}

//---------------------------------------------------------------------------

bool quit()
{
  return g_bQuit;
}

//---------------------------------------------------------------------------

bool pause()
{
  return g_bPause;
}

//---------------------------------------------------------------------------

void rotate( Stamp *s )
{
  int c=s->w, r=s->h;
  int *tmp = new int[c * r];

  for(int i=0; i<c; i++) {
    for(int j=0; j<r; j++) {
      tmp[i + j*c] = s->d[i + j*c];
    }
  }

  for(int i=0; i<c; i++) {
    for(int j=0; j<r; j++) {
      s->d[(r-j-1) + i*c] = tmp[i + j*c];
    }
  }

  s->h = c;
  s->w = r;  
  free(tmp);
}

//---------------------------------------------------------------------------

void input()
{
  static unsigned last = 0, last2 = 0;
    
  SDL_Event e;

  if(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_QUIT:
        g_bQuit=true;
        break;

      default:
        break;
    }
  }
  const unsigned char *keys = SDL_GetKeyboardState(0);

 if( SDL_GetTicks() > last+100 )
 {
   last = SDL_GetTicks();


   if( keys[SDL_SCANCODE_RIGHT]==1 ||
       keys[SDL_SCANCODE_L]==1      )
   {
     g_player_col+=1;
   }

   if( keys[SDL_SCANCODE_LEFT]==1 ||
       keys[SDL_SCANCODE_H]==1 )
   {
     g_player_col-=1;
   }

   if( keys[SDL_SCANCODE_UP]==1 ||
       keys[SDL_SCANCODE_K]==1   )
   {
     g_player_row-=1;
   }

   if( keys[SDL_SCANCODE_DOWN]==1 ||
       keys[SDL_SCANCODE_J]==1  )
   {
     g_player_row+=1;
   }

   if( g_player_row<1 )             g_player_row = 1;
   if( g_player_row>=g_world_rows-g_stamps[g_cur_stamp].h ) g_player_row = g_world_rows-g_stamps[g_cur_stamp].h-1;

   if( g_player_col<1 )             g_player_col = 1;
   if( g_player_col>=g_world_cols-g_stamps[g_cur_stamp].w ) g_player_col = g_world_cols-g_stamps[g_cur_stamp].w-1;
 } 

 if( SDL_GetTicks() > last2+200 ) {
   last2=SDL_GetTicks();

   if( keys[SDL_SCANCODE_SPACE]==1 )
   {
//     g_world[g_player_col + g_player_row*g_world_cols] = 
//       !g_world[g_player_col + g_player_row*g_world_cols];
   
     Stamp s = g_stamps[g_cur_stamp];

       for(int i=0; i<s.w; i++) {
         for(int j=0; j<s.h; j++) {
             g_world[(g_player_col+i) + ((j+g_player_row)* g_world_cols)] = s.d[i+s.w*j] * g_color;
         }
       }
   }

   if( keys[SDL_SCANCODE_P]==1 )
   {
     g_bPause = !g_bPause;
     if(g_iSteps) {
       g_iSteps= 0;
     }
     else
     {
       g_iSteps = -1;
     }
   }

   if( keys[SDL_SCANCODE_S]==1 )
   {
     g_bPause = !g_bPause;
     g_iSteps = 1;
   }

   if( keys[SDL_SCANCODE_F]==1 )
   {
     g_cur_stamp++;
     if(g_cur_stamp >= n_stamps) g_cur_stamp=0;
   }

   if( keys[SDL_SCANCODE_D]==1 )
   {
     rotate( &g_stamps[g_cur_stamp] );
   }

   if( keys[SDL_SCANCODE_A]==1 )
   {
     g_color++;
     if(g_color > 2) g_color=1;
   }



 }
}

//---------------------------------------------------------------------------
int value( int col, int row )
{
  return g_world[col + (g_world_cols* row)];
}

//---------------------------------------------------------------------------
int set( int col, int row, int val )
{
  return g_world_next[col + (g_world_cols* row)] = val;
}


//---------------------------------------------------------------------------

int neighbors( int i, int j, int* avg=0 )
{
  int r=0,v;
  float a=0;

  if(v=value(i-1,(j-1))) r++; a+=v;
  if(v=value(i  ,(j-1))) r++; a+=v;
  if(v=value(i+1,(j-1))) r++; a+=v;
  if(v=value(i-1,    j)) r++; a+=v;
  if(v=value(i+1,    j)) r++; a+=v;
  if(v=value(i-1,(j+1))) r++; a+=v;
  if(v=value(i  ,(j+1))) r++; a+=v;
  if(v=value(i+1,(j+1))) r++; a+=v;

  a/=r;
  a+=0.5;
  if(avg) *avg=int(a);

  return r;
}

//---------------------------------------------------------------------------

void update()
{
  static unsigned last = 0;
  int *p;


//  if( SDL_GetTicks() > last + 500 ) {
//    last = SDL_GetTicks();

    for(int i=1; i<g_world_cols-1; i++)
    {
      for(int j=1; j<g_world_rows-1; j++)
      {
        int a,v;
        int n=neighbors(i,j,&a);
        if( v=value(i,j) )
        {
          if((n<2) || (n>3))
          {
            set(i,j,0);
          }
          else
          {
            set(i,j,a);
          }
        }
        else
        {
          if(n==3) set(i,j,a);
          else set(i,j,0);
        }
      }
    }

    p = g_world;
    g_world = g_world_next;
    g_world_next = p;
//  }
}

//---------------------------------------------------------------------------

void draw()
{
  SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
  SDL_RenderClear(g_pRenderer);

  SDL_Rect r;
  r.w=r.h=g_cell_size;
  for(int i=0;i<g_world_cols; i++)
  {
    for(int j=0; j<g_world_rows; j++)
    {
      int v;
      if(v=value(i,j)) {
        if(v==1) SDL_SetRenderDrawColor( g_pRenderer, 255, 0, 0, 255);
        else SDL_SetRenderDrawColor( g_pRenderer, 0, 0, 255, 255);

        r.x = g_xoff + g_cell_border + i * (g_cell_size+g_cell_border);
        r.y = g_yoff + g_cell_border + j * (g_cell_size+g_cell_border);
        SDL_RenderFillRects( g_pRenderer, &r, 1 );
      }
    } 
  }

  // draw stamp
  Stamp s = g_stamps[g_cur_stamp];
  SDL_SetRenderDrawColor( g_pRenderer, 0, 255, 0, 255);

  r.w=r.h=g_cell_size;
  for(int i=0; i<s.w; i++) {
    for(int j=0; j<s.h; j++) {

      if(s.d[i+s.w*j]) {
        r.x = g_xoff + g_cell_border + (g_player_col+i) * (g_cell_size+g_cell_border);
        r.y = g_yoff + g_cell_border + (g_player_row+j) * (g_cell_size+g_cell_border);
        SDL_RenderFillRects( g_pRenderer, &r, 1 );
      }
      //g_world[(g_player_col+i) + ((j+g_player_row)* g_world_cols)] = g_stamp[i+g_stamp_w*j];
    }
  }

  SDL_RenderPresent(g_pRenderer);

  SDL_Delay(20);
}

//---------------------------------------------------------------------------

void cleanup()
{
   SDL_DestroyWindow(g_pWindow);
   SDL_DestroyRenderer(g_pRenderer);
   SDL_Quit();
}

//---------------------------------------------------------------------------

int main(int argc, char* args[])
{
   initialize();

   while( !quit() )
   {
      input();
      if(g_iSteps) { update(); g_iSteps--; }
      draw();
   }

   cleanup();
}
