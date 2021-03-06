//
//  main.cpp
//  SmartCalc
//
//  Created by Daniel Holst on 2015-06-08.
//  Copyright (c) 2015 Daniel Holst. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "EquationSolver.h"


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
void saveAnswer(std::string answer);

//calculate answer
std::string getAnswer(char equation[]);

//remove white spaces from text string
void removeSpaces(std::string& eq);

//Loads media
bool loadMedia();

//read from file to saved values
void readFromFile();

//set properties of rect
void setRectProperties(SDL_Rect& prop_button_9, int nr);

//get special char
char getSpecialChar(int nr);

//remove saved value
void removeSavedValue(int nr);

//Frees media and shuts down SDL
void close();

//draw delete buttons
void drawDeleteButtons(bool choice);

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
SDL_Rect popupLocation;

//text surface for popup
SDL_Surface* popupTextSurface = nullptr;
SDL_Rect popupTextLocation;

//Current displayed PNG image
SDL_Surface* background = nullptr;

//Surface for logo
SDL_Surface* logo = nullptr;
SDL_Rect logoLocation;

//button (9)
SDL_Surface* Buttons[TOTAL_BUTTONS];
SDL_Rect prop_buttons[TOTAL_BUTTONS];

//delete button for saved values
SDL_Surface* deleteButton[SAVED_VALUES_SLOTS];
SDL_Surface* deleteButtonActive[SAVED_VALUES_SLOTS];
SDL_Rect deleteButtonLocation[SAVED_VALUES_SLOTS];

//display surface
SDL_Surface* display = nullptr;

//Load a font
TTF_Font *font  = nullptr;

//Text
SDL_Surface* textSurface = nullptr;
SDL_Rect textLocation;

//saved answers
SDL_Surface* savedAnswerSurface[SAVED_VALUES_SLOTS];
SDL_Rect savedAnswersLocation[SAVED_VALUES_SLOTS];

//saved answer names
SDL_Surface* savedAnswersNameSurface[SAVED_VALUES_SLOTS];
SDL_Rect savedAnswersNameLocation[SAVED_VALUES_SLOTS];

//text colors
SDL_Color textForegroundColor;
SDL_Color textBackgroundColor;
SDL_Color textBackgroundColor2;


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
            drawDeleteButtons(true);
            inside = false;
        }
        //Mouse is right of the button
        else if( x > mPosition.x + BUTTON_WIDTH )
        {
            drawDeleteButtons(true);
            inside = false;
        }
        //Mouse above the button
        else if( y < mPosition.y )
        {
            drawDeleteButtons(true);
            inside = false;
        }
        //Mouse below the button
        else if( y > mPosition.y + BUTTON_HEIGHT )
        {
            drawDeleteButtons(true);
            inside = false;
        }
        
        //Mouse is outside button
        if( !inside )
        {
            drawDeleteButtons(true);
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        //Mouse is inside button
        else
        {
            drawDeleteButtons(false);
            
            //Set mouse over sprite
            switch( e->type )
            {
                case SDL_MOUSEMOTION:

                    mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
        
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
                                std::string answer = getAnswer(text);
                                ss << answer;
                            }
                            else if(nr == 16)   // clear
                            {
                                ss << "";
                                
                            }
                            else if(nr == 17)   // save
                            {
                                if(SAVED_VALUES != SAVED_VALUES_SLOTS)
                                {
                                    
                                    saveAnswer(getAnswer(text));
                                    SAVED_VALUES++;
                                    
                                    SDL_StartTextInput();   //Enable text input
                                    SAVE_BUTTON_ACTIVE = true;
                                }
                            }
                            else if(nr >= 20 && nr <= 26)   //delete saved value
                            {
                                removeSavedValue( nr - 20 );
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

//button objects
LButton gButtons[TOTAL_BUTTONS];
LButton deleteButtons[SAVED_VALUES_SLOTS];

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
            popupLocation.x = 200;
            popupLocation.y = 100;
            
            popupTextLocation.x = 270;
            popupTextLocation.y = 240;
            
            for(int i = 0; i < SAVED_VALUES_SLOTS; i++)
            {
                savedAnswersLocation[i].x = 630;
                savedAnswersLocation[i].y = 250 + i * 50;
                
                savedAnswersNameLocation[i].x = 450;
                savedAnswersNameLocation[i].y = 250 + i * 50;
                
                deleteButtonLocation[i].x = 750;
                deleteButtonLocation[i].y = 250 + i * 50;
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
    
    font = TTF_OpenFont("FreeSans.ttf", 22);
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
    
    //Load popup surface
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
    
    //load delete button textures
    for (int i = 0; i < SAVED_VALUES_SLOTS; i++)
    {
        deleteButton[i] = loadSurface( "Images/deletebuttonInactive.png");
        deleteButtonActive[i] = loadSurface( "Images/deletebuttonActive.png");
        setRectProperties(deleteButtonLocation[i], 20+i);
    }
    
    //load button textures
    for( int i = 0; i < TOTAL_BUTTONS; i++)
    {
        Buttons[i] = loadSurface( "Images/button_" + std::to_string(i) + ".png" );
        setRectProperties(prop_buttons[i], i);
    }
    
    readFromFile();
    
    return success;
}

void drawDeleteButtons(bool choice)
{
    if(choice)
    {
        //delete buttons
        for(int i = 0; i < SAVED_VALUES; i++)
        {
            SDL_BlitSurface(deleteButton[i], 0, gScreenSurface, &deleteButtonLocation[i]);
        }
    }
    else
        //delete buttons active
        for(int i = 0; i < SAVED_VALUES; i++)
        {
            SDL_BlitSurface(deleteButtonActive[i], 0, gScreenSurface, &deleteButtonLocation[i]);
        }
}
void readFromFile()
{
    //load saved values from file
    std::ifstream infile("SavedValues.txt");
    double value;
    std::string name;
    while ( infile >> value >> name)
    {
        //format text value
        std::string str = std::to_string (value);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        std::string zeros = "0";
        str.append(zeros);
        
        savedAnswerSurface[SAVED_VALUES] = TTF_RenderText_Shaded(font, str.c_str(), textForegroundColor, textBackgroundColor2 );
        
        savedAnswersNameSurface[SAVED_VALUES]  = TTF_RenderText_Shaded(font, name.c_str() , textForegroundColor, textBackgroundColor2 );
        
        SAVED_VALUES++;
        
    }
    
}
void removeSavedValue(int nr)
{
    //open file
    std::ifstream infile("SavedValues.txt");
    std::string value;
    std::string name;
    int counter1 = 0;
    int counter2 = 0;
    while ( infile >> value >> name)
    {
        if(counter1 == nr)
        {
            break;
        }
        counter1++;
    }
    infile.close();
    
    std::ofstream outfile;
    outfile.open("SavedValues.txt");
    
    if(nr == 0)
        outfile << " " << " ";
    else
    {
        while ( outfile << value << name)
        {
            counter2++;
            
            if(counter2 == nr)
            {
                break;
            }
        }
        outfile << " " << " ";
    }
    outfile.close();
    
    SAVED_VALUES--;
    
    readFromFile();
}

//save answer
void saveAnswer(std::string answer)
{
    savedAnswerSurface[SAVED_VALUES] = TTF_RenderText_Shaded(font, answer.c_str() , textForegroundColor, textBackgroundColor2);
    
    // add to file
    std::stringstream converter;
    converter << answer;
    double value;
    converter >> value;
    
    std::ofstream myfile;
    myfile.open("SavedValues.txt", std::ios::app);
    myfile << value;
    
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

std::string getAnswer(char eq[])
{
    std::string equation(eq);
    
    //remove empty spaces
    removeSpaces(equation);
    std::cout << "answer = " << equation << std::endl;  //debug
    
    equation = ES::solve(equation);
    
    //return answer
    return equation;
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
        tempY = prop_button.y = 125;
        
    if( nr == 9 || nr == 8 || nr == 7 || nr == 13) // top row
        tempY = prop_button.y = 210;
 
    if( nr == 6 || nr == 5 || nr == 4 || nr == 14) // middle row
        tempY = prop_button.y = 305;

    if( nr == 3 || nr == 2 || nr == 1 || nr == 12) // bottom row
        tempY = prop_button.y = 400;
    
    if( nr == 0 || nr == 10 || nr == 15)
        tempY = prop_button.y = 495;

    if( nr == 9 || nr == 6 || nr == 3 )  // right column
        tempX = prop_button.x = 220;

    if( nr == 8 || nr == 5 || nr == 2 || nr == 10 )  //middle column
        tempX = prop_button.x = 125;
    
    if( nr == 7 || nr == 4 || nr == 1 || nr == 0)  // left column
        tempX = prop_button.x = 30;

    if( nr == 11 || nr == 12 || nr == 13  || nr == 14  || nr == 15 )
        tempX = prop_button.x = 320;
    
    if(nr == 16)
        tempX = prop_button.x = 25;
    if( nr == 17)
        tempX = prop_button.x = 160;
    
    if(nr >= 20 && nr <= 26)
    {
        tempX = prop_button.x = 750;
        tempY = prop_button.y = 250 + (nr-20) * 50;
    }
    
    if( nr > 19 )
        deleteButtons[nr-20].setPosition(tempX, tempY);
    else
        gButtons[nr].setPosition( tempX, tempY);
}

void saveValue(std::string inputText)
{
    SAVE_BUTTON_ACTIVE = false;
    //Disable text input
    SDL_StopTextInput();
    savedAnswersNameSurface[SAVED_VALUES - 1] = TTF_RenderText_Shaded(font, inputText.c_str(), textForegroundColor, textBackgroundColor2 );
    
    //save to file
    std::ofstream myfile;
    myfile.open("SavedValues.txt", std::ios::app);
    
    myfile << " " << inputText << std::endl;
          
    myfile.close();
    
    
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
    SDL_FreeSurface(popupTextSurface);
    popupTextSurface = nullptr;
    
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
            
            //The input text.
            std::string inputText = "";
            
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
                        SDL_BlitSurface(popupSurface, 0, gScreenSurface, &popupLocation);
                        SDL_BlitSurface(popupTextSurface, 0, gScreenSurface, &popupTextLocation);
                        
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
                            }
                        }
                                          
                            popupTextSurface = TTF_RenderText_Shaded(font, inputText.c_str(), textForegroundColor, textBackgroundColor);
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
                    for(int i = 0; i < SAVED_VALUES; i++)
                    {
                        deleteButtons[i].handleEvent(&e, 20+i);
                    }
                    //add display surface to layout
                    SDL_BlitSurface(textSurface, 0, gScreenSurface, &textLocation);
                    
                    //surface for saved values
                    for(int i = 0; i < SAVED_VALUES; i++)
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