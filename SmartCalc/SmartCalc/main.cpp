//
//  main.cpp
//  SmartCalc
//
//  Created by Daniel Holst on 2015-06-08.
//  Copyright (c) 2015 Daniel Holst. All rights reserved.
//

#include <iostream>
#include <sstream>
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
const int TOTAL_BUTTONS = 18;

//text string
int textSize = 0;
char text[30] = "";

//saved values
const int SAVED_VALUES_SLOTS = 6;
int SAVED_VALUES = 0;
bool SAVE_BUTTON_ACTIVE = false;
char variableName[15] = "";
char *composition;
Sint32 cursor;
Sint32 selection_len;

//Starts up SDL and creates window
bool init();

//update text
void updateText();
//save answer
void saveAnswer(char answer[]);

//calculate answer
double getAnswer(char equation[]);

//remove white spaces from text string
void removeSpaces(std::string& eq);

//Loads media
bool loadMedia();

//set properties of rect
void setRectProperties(SDL_Rect& prop_button_9, int nr);

//get special char
char getSpecialChar(int nr);

//Frees media and shuts down SDL
void close();

//update and print out the save value name
void saveValue(std::string inputText);

//Loads individual image
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = nullptr;

//The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;

//Popup window
SDL_Surface* popupSurface = nullptr;

//Current displayed PNG image
SDL_Surface* background = nullptr;

//Surface for logo
SDL_Surface* logo = nullptr;

//button (9)
SDL_Surface* Buttons[TOTAL_BUTTONS];

//display surface
SDL_Surface* display = nullptr;

//Load a font
TTF_Font *font  = nullptr;

//Text
SDL_Surface* textSurface = nullptr;

//saved answers
SDL_Surface* savedAnswerSurface[SAVED_VALUES_SLOTS];

//saved answer names
SDL_Surface* savedAnswersNameSurface[SAVED_VALUES_SLOTS];

//text colors
SDL_Color textForegroundColor;
SDL_Color textBackgroundColor;
SDL_Color textBackgroundColor2;

// display ext location
SDL_Rect textLocation;

// saved answers value location
SDL_Rect savedAnswersLocation[SAVED_VALUES_SLOTS];

// saved answers name location
SDL_Rect savedAnswersNameLocation[SAVED_VALUES_SLOTS];

//Logo location
SDL_Rect logoLocation;

//properties for buttons
SDL_Rect prop_buttons[TOTAL_BUTTONS];

// properties for popup
SDL_Rect popUpLocation;



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
//                    std::cout << " pressed button: " << nr << std::endl;
                    
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                    
                case SDL_MOUSEBUTTONUP:
                    //check if pressed down before
                    if(mCurrentSprite == BUTTON_SPRITE_MOUSE_DOWN)
                    {
                        if(textSize < 30)
                        {
                            std::stringstream ss;
                            //get special char for operations
                            if( nr >= 10 && nr <= 14)
                            {
                                if( nr == 11 || nr == 13)
                                    ss <<" " << getSpecialChar(nr) << " ";
                                else
                                    ss << getSpecialChar(nr);
                            }
                            
                            else if(nr == 15)   // =
                            {
                                //TODO (answer)
                                double answer = getAnswer(text);
                                ss << answer;
                            }
                            else if(nr == 16)   // clear
                            {
                                ss << "";
                                
                            }
                            else if(nr == 17)   // save
                            {
                                //TODO (save answer)
                                if(SAVED_VALUES != SAVED_VALUES_SLOTS)
                                {
                                    saveAnswer(text);
                                    SAVED_VALUES++;
                                    //add popup surface to layout
                                    //Enable text input
                                    SDL_StartTextInput();
                                    SAVE_BUTTON_ACTIVE = true;
                                    std::cout << "used slots: " <<  SAVED_VALUES << std::endl;
                                }
                            }
                            else
                            {
                                ss << nr;
                            }
                        
                            ss.str().c_str();
                            size_t size = ss.str().length() + 1;
                            char* message = new char[size];
                            memcpy(message, ss.str().c_str(), size);
                            message[size - 1] = char(0);
                            if(nr == 15 || nr == 16)
                            {
                                std::strcpy(text, message);
                            }
                            else
                                std::strcat(text, message);
                            textSize++;
                            updateText();
                            delete [] message;

                        }
                    }

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
    // Initialize SDL_ttf library
    if (TTF_Init() != 0)
    {
        std::cout << "TTF_Init() Failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    
    font  = TTF_OpenFont("FreeSans.ttf", 18);
    
    //text colors
    textForegroundColor = { 0, 0, 0 };
    textBackgroundColor = { 221, 243, 130 };
    textBackgroundColor2 = { 255, 255, 255 };
    
    textSurface = TTF_RenderText_Shaded(font, text , textForegroundColor, textBackgroundColor );
    
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
            textLocation.x = 40;
            textLocation.y = 40;
            
            //init rect for logo field
            logoLocation.x = 400;
            logoLocation.y = 0;
            
            //init rect for popup
            popUpLocation.x = 200;
            popUpLocation.y = 100;
            
            for(int i = 0; i < SAVED_VALUES_SLOTS; i++)
            {
                savedAnswersLocation[i].x = 670;
                savedAnswersLocation[i].y = 250 + i * 50;
                
                savedAnswersNameLocation[i].x = 450;
                savedAnswersNameLocation[i].y = 250 + i * 50;
            }
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
    
    //Load popup surface    TODO fix texture for this
    popupSurface = loadSurface("Images/popUp.png");
    if( popupSurface == nullptr )
    {
        printf( "Failed to load popup image!\n" );
        success = false;
    }
    
    //Load logo texture
    logo = loadSurface("Images/savedValues.png");
    if( logo == nullptr )
    {
        printf( "Failed to load logo texture!\n" );
        success = false;
    }
    
    //load button textures
    for( int i = 0; i < TOTAL_BUTTONS; i++)
    {
        Buttons[i] = loadSurface( "Images/button_" + std::to_string(i) + ".png" );
        setRectProperties(prop_buttons[i], i);
    }
    
    return success;
}

//save answer
void saveAnswer(char answer[])
{
    savedAnswerSurface[SAVED_VALUES] = TTF_RenderText_Shaded(font, answer , textForegroundColor, textBackgroundColor2 );
}

//remove spaces in string
void removeSpaces(std::string& eq)
{
    std::string temp = "";
    for(size_t i = 0; i < eq.length(); i++)
        if(eq[i] != ' ')
            temp += eq[i];
    eq = temp;
}

double getAnswer(char eq[])
{
    double answer = 0;
    std::string equation(eq);
    
    //remove empty spaces
    removeSpaces(equation);
    std::cout << "answer = " << equation << std::endl;  //debug
    
    //divide numbers and operations
//    int tokensInString = equation.size();
    
    //do operations
    
    //return answer
    return answer;
}

void updateText()
{
    textSurface = TTF_RenderText_Shaded(font, text , textForegroundColor, textBackgroundColor );
}

char getSpecialChar(int nr)
{
    char operation = '\0';
    if(nr == 10)
        operation = '.';
    if( nr == 11)
        operation = '+';
    if( nr == 12)
        operation = '/';
    if( nr == 13)
        operation = '-';
    if( nr == 14)
        operation = '*';
    
    return operation;
}

void setRectProperties(SDL_Rect& prop_button, int nr)
{
    int tempX = 0;
    int tempY = 0;
    
    if( nr == 11 || nr == 16 || nr == 17)
    {
        prop_button.y = 125;
        tempY = 125;
    }
    
    if( nr == 9 || nr == 8 || nr == 7 || nr == 13) // top row
    {
        prop_button.y = 210;
        tempY = 210;
    }
    
    if( nr == 6 || nr == 5 || nr == 4 || nr == 14) // middle row
    {
        prop_button.y = 305;
        tempY = 305;
    }
    
    if( nr == 3 || nr == 2 || nr == 1 || nr == 12) // bottom row
    {
        prop_button.y = 400;
        tempY = 400;
    }
    
    if( nr == 0 || nr == 10 || nr == 15)
    {
        prop_button.y = 495;
        tempY = 495;
    }

    // 30+75+30+75+30
    if( nr == 9 || nr == 6 || nr == 3 )  // right column
    {
        prop_button.x = 220;
        tempX = 240;
    }
    
    if( nr == 17)
    {
        prop_button.x = 160;
        tempX = 135;
    }
    // 30+75+30
    if( nr == 8 || nr == 5 || nr == 2 || nr == 10 )  //middle column
    {
        prop_button.x = 125;
        tempX = 135;
    } 
    
    if( nr == 7 || nr == 4 || nr == 1 || nr == 0)  // left column
    {
        prop_button.x = 30;
        tempX = 30;
    }
    
    if( nr == 11 || nr == 12 || nr == 13  || nr == 14  || nr == 15 )
    {
        prop_button.x = 320;
        tempX = 320;
    }
    
    if(nr == 16)
        prop_button.x = 25;
    
    //set right position for invisible clickable button
    gButtons[nr].setPosition( tempX, tempY);
}

void saveValue(std::string inputText)
{
    SAVE_BUTTON_ACTIVE = false;
    std::cout << inputText << std::endl;
    //Disable text input
    SDL_StopTextInput();
    savedAnswersNameSurface[SAVED_VALUES] = TTF_RenderText_Shaded(font, inputText.c_str(), textForegroundColor, textBackgroundColor2 );
    
}

void close()
{
    //Free loaded image
    SDL_FreeSurface( background );
    background = nullptr;
    SDL_FreeSurface( textSurface );
    textSurface = nullptr;
    SDL_FreeSurface( popupSurface );
    popupSurface = nullptr;
    
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
            
//            gInputTextTexture.loadFromRenderedText( inputText.c_str(), textForegroundColor );
            
            //While application is running
            while( !quit )
            {
                //The rerender text flag
                bool renderText = false;
                
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    //Special key input
                    else if(SAVE_BUTTON_ACTIVE == true)
                    {
                        //The input text.
                        std::string inputText = "";
                        
                        SDL_BlitSurface(popupSurface, 0, gScreenSurface, &popUpLocation);
                        if( e.type == SDL_KEYDOWN )
                        {
                            //Handle backspace
                            if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
                            {
                                //lop off character
                                inputText.pop_back();
                                renderText = true;
                            }
                        
                            //if enter is pressed
                            else if ( e.key.keysym.sym == SDLK_RETURN)
                            {
                                saveValue(inputText);
                                inputText = "";
                            }
                            //Handle copy
                            else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                            {
                                SDL_SetClipboardText( inputText.c_str() );
                            }
                            //Handle paste
                            else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                            {
                                inputText = SDL_GetClipboardText();
                                renderText = true;
                            }

                        }
                        //Special text input event
                        else if( e.type == SDL_TEXTINPUT )
                        {

                            //Not copy or pasting
                            if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL ) )
                            {
                                //Append character
                                inputText += e.text.text;
                                renderText = true;
                                std::cout << inputText << std::endl;
                            }
                        }
                                          
                            popupSurface = TTF_RenderText_Shaded(font, inputText.c_str(), textForegroundColor, textBackgroundColor);
                    }
                
                
                
                else
                {
                    //Apply the PNG image
                    SDL_BlitSurface( background, nullptr, gScreenSurface, nullptr );
                    //add logo surface to layout
                    SDL_BlitSurface(logo, 0, gScreenSurface, &logoLocation);
                    
                    //add buttons
                    for(int i = 0; i < TOTAL_BUTTONS; i++)
                    {
                        SDL_BlitSurface( Buttons[i], nullptr, gScreenSurface, &prop_buttons[i] );
                    }
                    
                    //Handle button events
                    for( int i = 0; i < TOTAL_BUTTONS; ++i )
                    {
                        gButtons[ i ].handleEvent( &e, i );
                    }
                    
                    //add display surface to layout
                    SDL_BlitSurface(textSurface, 0, gScreenSurface, &textLocation);
                    
                    //surface for saved values
                    for(int i = 0; i < SAVED_VALUES_SLOTS; i++)
                    {
                        SDL_BlitSurface(savedAnswerSurface[i], 0, gScreenSurface, &savedAnswersLocation[i]);
                        SDL_BlitSurface(savedAnswersNameSurface[i], 0, gScreenSurface, &savedAnswersNameLocation[i]);
                    }
                    
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
}