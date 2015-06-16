//
//  main.cpp
//  SmartCalc
//
//  Created by Daniel Holst on 2015-06-08.
//  Copyright (c) 2015 Daniel Holst. All rights reserved.
//

#include <iostream>
#include <string>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>


//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//Button constants
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 100;
const int TOTAL_BUTTONS = 16;

//text string
int textSize = 0;
char text[20] = " ";

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
SDL_Surface* Buttons[TOTAL_BUTTONS];

//display surface
SDL_Surface* display = nullptr;

//Load a font
TTF_Font *font = nullptr;

//Text
SDL_Surface* textSurface = nullptr;

//Text location
SDL_Rect textLocation;

//properties for buttons
SDL_Rect prop_buttons[TOTAL_BUTTONS];


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
    void handleEvent( SDL_Event* e, int nr );
    
    
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

void LButton::handleEvent( SDL_Event* e, int nr )
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
                    std::cout << " pressed button: " << nr << std::endl;
                    if(textSize < 30)
                    {
                        char temp[5];
                        temp + std::to_string(nr);
                        std::strcat(text, temp);
                        textSize++;
                    }
                    
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                    
                case SDL_MOUSEBUTTONUP:
//                    text = text + '9';
//                    textSize++;
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
    //text[1] = 3;
    
    // Initialize SDL_ttf library
    if (TTF_Init() != 0)
    {
        std::cout << "TTF_Init() Failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    
    TTF_Font* font = TTF_OpenFont("FreeSans.ttf", 20);
    SDL_Color foregroundColor = { 255, 255, 255 };
    SDL_Color backgroundColor = { 0, 0, 0 };
    textSurface = TTF_RenderText_Shaded(font, text , foregroundColor, backgroundColor );
    
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
            //Init rect for text field
            textLocation.x = 30;
            textLocation.y = 30;
            
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
    
    font = TTF_OpenFont("FreeSans.ttf", 24);
    if (font == NULL)
    {
        std::cout << "TTF_OpenFont() Failed: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    
    //Load background surface
    background = loadSurface( "Images/SmartCalc.png" );
    if( background == nullptr )
    {
        printf( "Failed to load PNG image!\n" );
        success = false;
    }
    
    //load button textures
    for( int i = 0; i < TOTAL_BUTTONS; i++)
    {
        Buttons[i] = loadSurface( "Images/button_" + std::to_string(i + 1) + ".png" );
        setRectProperties(prop_buttons[i], i+1, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    
    return success;
}

void setRectProperties(SDL_Rect& prop_button, int nr, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    int tempX = 0;
    int tempY = 0;
    
    if( nr == 11)
    {
        prop_button.y = 125;
        tempY = 125;
    }
    
    if( nr == 9 || nr == 8 || nr == 7 || nr == 13) // top row
    {
        prop_button.y = 210;
        tempY = 210;
    }
    
    if( nr == 6 || nr == 5 || nr == 4 || nr == 15) // middle row
    {
        prop_button.y = 305;
        tempY = 305;
    }
    
    if( nr == 3 || nr == 2 || nr == 1 || nr == 12) // bottom row
    {
        prop_button.y = 400;
        tempY = 400;
    }
    
    if( nr == 16 || nr == 10 || nr == 14)
    {
        prop_button.y = 495;
        tempY = 495;
    }

    // 30+75+30+75+30
    if( nr == 9 || nr == 6 || nr == 3)  // right column
    {
        prop_button.x = 240;
        tempX = 240;
    }
    
    // 30+75+30
    if( nr == 8 || nr == 5 || nr == 2 || nr == 10)  //middle column
    {
        prop_button.x = 135;
        tempX = 135;
    } 
    
    if( nr == 7 || nr == 4 || nr == 1 || nr == 16)  // left column
    {
        prop_button.x = 30;
        tempX = 30;
    }
    
    if( nr == 11 || nr == 12 || nr == 13  || nr == 14  || nr == 15 )
    {
        prop_button.x = 320;
        tempX = 320;
    }
    
    //set right position for invisible clickable button
    gButtons[nr-1].setPosition( tempX, tempY);
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

    //Start up SDL and create window
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
                SDL_BlitSurface( background, nullptr, gScreenSurface, nullptr );
                for(int i = 0; i < TOTAL_BUTTONS; i++)
                {
                    SDL_BlitSurface( Buttons[i], nullptr, gScreenSurface, &prop_buttons[i] );
                }
                
                SDL_BlitSurface(textSurface, 0, gScreenSurface, &textLocation);
                
                //Handle button events
                for( int i = 0; i < TOTAL_BUTTONS; ++i )
                {
                    gButtons[ i ].handleEvent( &e, i+1 );
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