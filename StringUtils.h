// Header File Information ////////////////////////////////////////////
/*
File: StringUtils.h
Brief: Header file for StringUtils code
Details: Specifies functions, constants, and other information
         related to StringUtils code
Version: 1.0
         01 February 2019
         Initial development of StringUtils code
Note: Code based on 'Introduction to C Programming' tutorial
      by Michael E. Leverington
*/

// Precompiler Directives //////////////////////////////////////////
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// Header Files ///////////////////////////////////////////////////
#include <stdio.h> // file operations
#include <stdlib.h> // dynamic memory operations

// Data Structure Definitions (structs, enums, etc.)////////////////
typedef enum Boolean
{
    False,
    True
} Boolean;

typedef enum StringManipCode
{
    NO_ERR,
    INCOMPLETE_FILE_ERR,
    INPUT_BUFFER_OVERRUN_ERR
} StringManipCode;

// Global Constant Definitions /////////////////////////////////////
extern const int MAX_STR_LEN;
extern const int STD_STR_LEN;
extern const int SUBSTRING_NOT_FOUND;
extern const int STR_EQ;
extern const char NULL_CHAR;
extern const char SPACE;
extern const char COLON;
extern const char PERIOD;
extern const char SEMICOLON;
extern const char RIGHT_PAREN;
extern const char LEFT_PAREN;
extern const char DECIMAL;
extern const Boolean IGNORE_LEADING_WS;
extern const Boolean ACCEPT_LEADING_WS;

// Function Prototypes /////////////////////////////////////////////
int getStringLength( char *testStr );
void copyString( char *destination, char *source );
void concatenateString( char *destination, char *source );
int compareString( char *oneStr, char *otherStr );
void getSubString( char *destStr, char *sourceStr,
                                          int startIndex, int endIndex );
int findSubString( char *testStr, char *searchSubStr );
void setStrToLowerCase( char *destStr, char *sourceStr );
char setCharToLowerCase( char testChar );
int getLineTo( FILE *filePtr, int bufferSize, char stopChar,
                          char *buffer, Boolean omitLeadingWhiteSpace );
Boolean isEndOfFile( FILE *filePtr);
float stringToFloat( char *strNum );

// Terminating Precompiler Directives ///////////////////////////////
#endif  // STRING_UTILS_H
