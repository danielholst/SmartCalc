//
//  main.cpp
//  SmartCalc
//
//  Created by Daniel Holst on 2015-06-08.
//  Copyright (c) 2015 Daniel Holst. All rights reserved.
//

#include <iostream>
#include <stdio.h>
//#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
//#include "LButton.h"

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//set properties of rect
void setRectProperties(SDL_Rect& prop_button_9, int nr, int SCREEN_WIDTH, int SCREEN_HEIGHT);

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = nullptr;

//The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;

//Current displayed PNG image
SDL_Surface* background = nullptr;

//button (9)
SDL_Surface* button_9 = nullptr;



//Button constants
const int BUTTON_WIDTH = 80;
const int BUTTON_HEIGHT = 80;
const int TOTAL_BUTTONS = 1;

enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};


//The mouse button
class LButton
{
public:
    //Initializes internal variables
    LButton();
    
    //Sets top left position
    void setPosition( int x, int y );
    
    //Handles mouse event
    void handleEvent( SDL_Event* e );
    
    
private:
    //Top left position
    SDL_Point mPosition;
    
    //Currently used global sprite
    LButtonSprite mCurrentSprite;
};

//#endif /* defined(__SmartCalc__LButton__) */



LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;
    
    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition( int x, int y )
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent( SDL_Event* e )
{
    //If mouse event happened
    if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );
        
        //Check if mouse is in button
        bool inside = true;
        
        //Mouse is left of the button
        if( x < mPosition.x )
        {
            inside = false;
        }
        //Mouse is right of the button
        else if( x > mPosition.x + BUTTON_WIDTH )
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < mPosition.y )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > mPosition.y + BUTTON_HEIGHT )
        {
            inside = false;
        }
        
        //Mouse is outside button
        if( !inside )
        {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        //Mouse is inside button
        else
        {
            //Set mouse over sprite
            switch( e->type )
            {
                case SDL_MOUSEMOTION:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    std::cout << " pressed button" << std::endl;
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                    
                case SDL_MOUSEBUTTONUP:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                    break;
            }
        }
    }
}

//button object
LButton gButtons[TOTAL_BUTTONS];

bool init()
{
    
    //Initialization flag
    bool success = true;
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SmartCalc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == nullptr )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) )
            {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }
            else
            {
                //Get window surface
                gScreenSurface = SDL_GetWindowSurface( gWindow );
            }
        }
    }
    
    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;
    
    //Load background surface
    background = loadSurface( "Images/SmartCalc.png" );
    if( background == nullptr )
    {
        printf( "Failed to load PNG image!\n" );
        success = false;
    }
    
    //load button image
    button_9 = loadSurface( "Images/button_9.png" );
    if( background == nullptr )
    {
        printf( "Failed to load PNG image!\n" );
        success = false;
    }
    
    //Load clickable transparent buttons
    gButtons[0].setPosition(SCREEN_WIDTH/4, SCREEN_HEIGHT/4);

    
    return success;
}

void setRectProperties(SDL_Rect& prop_button_9, int nr, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    prop_button_9.x = SCREEN_WIDTH/4;
    prop_button_9.y = SCREEN_HEIGHT/4;
    prop_button_9.w = 16;
    prop_button_9.h = 16;
}

void close()
{
    //Free loaded image
    SDL_FreeSurface( background );
    background = nullptr;
    
    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface( std::string path )
{
    //The final optimized image
    SDL_Surface* optimizedSurface = nullptr;
    
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == nullptr )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == nullptr )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    
    return optimizedSurface;
}


int main( int argc, char* args[] )
{
    //Background layout
//    Graphics backgroundLayout(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    
//    Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Main loop flag
            bool quit = false;
            
            //Event handler
            SDL_Event e;
            
            //While application is running
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                }
                
                //Apply the PNG image
                SDL_Rect prop_button_9;
                setRectProperties(prop_button_9, 9, SCREEN_WIDTH, SCREEN_HEIGHT);
                
                SDL_BlitSurface( background, nullptr, gScreenSurface, nullptr );
                SDL_BlitSurface( button_9, nullptr, gScreenSurface, &prop_button_9);
                
                //Handle button events
                for( int i = 0; i < TOTAL_BUTTONS; ++i )
                {
                    gButtons[ i ].handleEvent( &e );
                }
                
                //Update the surface
                SDL_UpdateWindowSurface( gWindow );
            }
        }
    }
    
    //Free resources and close SDL
    close();
    
    return 0;
}