/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <string>
#include <iostream>
#include "Personaje.h"
#include "SDL_mixer.h"

using namespace std;

//Screen attributes
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *upMessage = NULL;
SDL_Surface *downMessage = NULL;
SDL_Surface *leftMessage = NULL;
SDL_Surface *rightMessage = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;
Mix_Music * music = NULL;

//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

SDL_Surface *load_image( std::string filename )
{
    return IMG_Load( filename.c_str() );
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blitz
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Press an Arrow Key", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "background.png" );

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 72 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return false;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );
    SDL_FreeSurface( upMessage );
    SDL_FreeSurface( downMessage );
    SDL_FreeSurface( leftMessage );
    SDL_FreeSurface( rightMessage );

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

bool puedoLLegar(char tablero[8][8],int x_actual, int y_actual,int pasos, int x_final, int y_final)
{
    //Casos base
    if(pasos<0)
        return false;

    if(x_actual>=8
       || y_actual>=8
       || x_actual<0
       || y_actual<0)
       return false;

    if(tablero[y_actual][x_actual]=='#')
       return false;

    if(x_actual==x_final
       && y_actual==y_final)
       return true;

    if(tablero[y_actual][x_actual]=='O')
        pasos--;

    if(tablero[y_actual][x_actual]=='V')
        pasos++;
      if(tablero[y_actual][x_actual]=='V'
           && pasos<=0)
            pasos=1;
    pasos--;



    return puedoLLegar(tablero,x_actual+1,y_actual,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual-1,y_actual,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual,y_actual+1,pasos,x_final,y_final)
            || puedoLLegar(tablero,x_actual,y_actual-1,pasos,x_final,y_final);
}

void marcar(char tablero[8][8],char tablero_de_pasos[8][8],char ataca[8][8],int x_actual, int y_actual,int pasos)
{
    //Casos base
    if(pasos<0)
        return;

    if(x_actual>=8
       || y_actual>=8
       || x_actual<0
       || y_actual<0)
       return;

    if(tablero[y_actual][x_actual]=='#')
       return;

    tablero_de_pasos[y_actual][x_actual]='P';
    if (pasos>=1&&tablero[y_actual][x_actual]==' ')
        ataca[y_actual][x_actual]='A';

    if(tablero[y_actual][x_actual]=='O')
        pasos--;

    if(tablero[y_actual][x_actual]=='V')
        pasos++;
    if(tablero[y_actual][x_actual]=='V'
           && pasos<=0)
            pasos=1;
    pasos--;



    marcar(tablero,tablero_de_pasos,ataca,x_actual+1,y_actual,pasos);
    marcar(tablero,tablero_de_pasos,ataca,x_actual-1,y_actual,pasos);
    marcar(tablero,tablero_de_pasos,ataca,x_actual,y_actual+1,pasos);
    marcar(tablero,tablero_de_pasos,ataca,x_actual,y_actual-1,pasos);
}




void limpiar(char tablero_de_pasos1[8][8],char ataca[8][8])
{

    for(int x=0;x<8;x++)
        for(int y=0;y<8;y++){
            tablero_de_pasos1[y][x]=' ';
            ataca[y][x]=' ';
        }
}

int main( int argc, char* args[] )
{if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return 1;
    }

    music = Mix_LoadMUS( "cancion.mp3" );

    if( music == NULL )
    {
        return 1;
    }

  //  bool first_time = true, first_it = true;
      Mix_PlayMusic(music, -1);

    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Generate the message surfaces
    upMessage = TTF_RenderText_Solid( font, "Up was pressed.", textColor );
    downMessage = TTF_RenderText_Solid( font, "Down was pressed.", textColor );
    leftMessage = TTF_RenderText_Solid( font, "Left was pressed", textColor );
    rightMessage = TTF_RenderText_Solid( font, "Right was pressed", textColor );



    SDL_Surface* pasillo = load_image("tablero/pasillo.png");
    SDL_Surface* muro = load_image("tablero/muro.png");
    SDL_Surface* cursor = load_image("tablero/cursor.png");
    SDL_Surface* obstaculo = load_image("tablero/obstaculo.png");
    SDL_Surface* ventaja = load_image("tablero/ventaja.png");
    SDL_Surface* ATACAR = load_image("tablero/ATACAR.png");
     SDL_Surface*Vida = load_image("life.png");
     SDL_Surface*Gandor1 = load_image("ganador1.png");
     SDL_Surface*Gandor2 = load_image("ganador2.png");
     SDL_Surface*Menu  = load_image("menu.png");
     SDL_Surface*Play = load_image("play.png");
     SDL_Surface*intrucciones  = load_image("instrucciones.png");
     SDL_Surface*imagenes_intrucciones  = load_image("imagenes_intrucciones.png");
     SDL_Surface*salir  = load_image("salir.png");
     SDL_Surface*cursor_menu  = load_image("curso_menu.png");
    int cursor_menu_y;
    char tablero[8][8]={{' ','#',' ',' ',' ','#','#','O',},
                        {' ','#',' ','#',' ',' ',' ',' ',},
                        {' ','#',' ','#',' ','V',' ',' ',},
                        {' ','V',' ','#',' ','#','#',' ',},
                        {' ',' ',' ',' ',' ','#','#',' ',},
                        {' ',' ',' ','O',' ','V','#',' ',},
                        {'#','#','#',' ','O',' ',' ','O',},
                        {'#','V',' ',' ',' ','O',' ','O',}};

    char tablero_de_pasos1[8][8]={{' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '}};

    char tablero_de_pasos2[8][8]={{' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '},
                                {' ',' ',' ',' ',' ',' ',' ',' '}};

     char ataca[8][8]={{' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '},
                       {' ',' ',' ',' ',' ',' ',' ',' '}};

    SDL_Surface* pasos_surface = load_image("pasos.png");
    SDL_Surface* personaje_surface = load_image("Personaje1.png");
    Personaje personaje(0,0,personaje_surface);

    SDL_Surface* personaje_surface2 = load_image("mojojojo.png");
    Personaje mojojo(7,7,personaje_surface2);

    SDL_Surface* cursor_surface = load_image("cursor.png");
    SDL_Surface* turno1 = load_image("turno1.png");
    SDL_Surface* turno2 = load_image("turno2.png");
    int cursor_x=0;
    int cursor_y=0;
    int life1=3;
    int life2=3;
    bool termino=false;
    bool turno =true;
    bool menu=true;



    //While the user hasn't quit
    while( quit == false )
    {
        //If there's an event to handle
        if( SDL_PollEvent( &event ) )
        {
            //If a key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                //Set the proper message surface

                switch( event.key.keysym.sym )
                {
                    case SDLK_SPACE:
                        if (termino==false){
                        if (ataca[mojojo.y][mojojo.x]=='A'&&turno==true){
                            life2--;
                            turno=false;
                        }else{
                        if (ataca[personaje.y][personaje.x]=='A'&&turno==false){
                            life1--;
                             turno=true;
                        }
                        }
                        }
                    break;
                    case SDLK_UP:
                        if (menu==true)
                        cursor_menu_y--;
                        if (menu==false)
                        cursor_y--;
                    break;
                    case SDLK_ESCAPE:
                        if (cursor_menu_y==1){
                            menu=true;
                        }
                    break;
                    case SDLK_DOWN:
                        if (menu==true)
                        cursor_menu_y++;
                        if (menu==false)
                        cursor_y++;
                    break;
                    case SDLK_LEFT:
                        cursor_x--;
                    break;
                    case SDLK_RIGHT:
                        cursor_x++;
                    break;
                    case SDLK_RETURN:
                        if (menu==true){
                            menu=false;
                        }else{

                        if(menu==false&&puedoLLegar(tablero,personaje.x,personaje.y,3,cursor_x,cursor_y)&&turno==true)
                        {   personaje.x=cursor_x;
                            personaje.y=cursor_y;
                            limpiar(tablero_de_pasos1,ataca);
                            marcar(tablero,tablero_de_pasos1,ataca,personaje.x,personaje.y,3);
                            turno=false;

                        }else{
                         if(menu==false&&puedoLLegar(tablero,mojojo.x,mojojo.y,3,cursor_x,cursor_y)&&turno==false)
                        {   mojojo.x=cursor_x;
                            mojojo.y=cursor_y;
                            limpiar(tablero_de_pasos2,ataca);
                            marcar(tablero,tablero_de_pasos2,ataca,mojojo.x,mojojo.y,3);
                            turno=true;

                        }
                        }
                        }
                    break;

                        }
            }

            //If the user has Xed out the window
            else if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
        if (cursor_menu_y>2)
            cursor_menu_y=0;
        if (cursor_menu_y<0)
            cursor_menu_y=0;

        //Apply the background

        //If a message needs to be displayed
        if( message != NULL )
        {
            //Apply the background to the screen
            apply_surface( 0, 0, background, screen );

            //Apply the message centered on the screen
            apply_surface( ( SCREEN_WIDTH - message->w ) / 2, ( SCREEN_HEIGHT - message->h ) / 2, message, screen );

            //Null the surface pointer
            message = NULL;
        }
        if (menu==true&&termino==false){
            apply_surface(0,0, Menu, screen );
            apply_surface(0,200, Play, screen );
            apply_surface(0,300, intrucciones, screen );
            apply_surface(0,400, salir, screen );
            apply_surface(0,(cursor_menu_y*57)+220, cursor_menu, screen );
        }
        if (cursor_menu_y==2&&menu==false){
            exit(0);
        }
        if(cursor_menu_y==1&&menu==false){
              menu=false;
            apply_surface(0,0, imagenes_intrucciones, screen );
        }
        if (termino==false&&menu==false&&cursor_menu_y==0){
        for(int x=0;x<10;x++)
            for(int y=0;y<10;y++)
            {
                if(tablero[y][x]==' ')
                    apply_surface(x*75,y*75,pasillo,screen);
                if(tablero[y][x]=='#')
                    apply_surface(x*75,y*75,muro,screen);
                if(tablero[y][x]=='O')
                    apply_surface(x*75,y*75,obstaculo,screen);
                if(tablero[y][x]=='V')
                    apply_surface(x*75,y*75,ventaja,screen);

            }

         for(int x=0;x<8;x++){
            for(int y=0;y<8;y++){
            if(tablero_de_pasos1[y][x]=='P'){
            apply_surface(x*75,y*75,pasos_surface,screen);
            }
             if(ataca[y][x]=='A'){
            apply_surface(x*75,y*75,ATACAR,screen);
             }
            }
         }
          for(int x=0;x<8;x++){
            for(int y=0;y<8;y++){
            if(tablero_de_pasos2[y][x]=='P'){
            apply_surface(x*75,y*75,pasos_surface,screen);
            }
             if(ataca[y][x]=='A'){
            apply_surface(x*75,y*75,ATACAR,screen);
             }
            }
         }
          for(int x=0;x<life1;x++){
            apply_surface(x*40,0,Vida,screen);
         }
         for(int x=0;x<life2;x++){
            apply_surface((x*40)+480,0,Vida,screen);
         }


        apply_surface(cursor_x*75,cursor_y*75,cursor_surface,screen);
         personaje.dibujar(screen);
        mojojo.dibujar(screen);

        }
        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }
    if (cursor_x>7)
        cursor_x=7;
     if (cursor_y>7)
        cursor_y=7;
    if (cursor_x<0)
        cursor_x=0;
     if (cursor_y<0)
        cursor_y=0;



       if (life1==0){
            termino=true;
        apply_surface(-120,-100,Gandor2,screen);
        }
        if (life2==0){
            termino=true;
        apply_surface(-120,-100,Gandor1,screen);
        }

    }

    //Clean up
    clean_up();

    return 0;
}
