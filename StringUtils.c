// Code Implementation File Information ///////////////////////////////
/*
File: StringUtils.c
Brief: Implementation file for StringUtils code
Details: Implements all functions of the StringUtils utilities
Version: 1.0
         01 February 2019
         Initial development of StringUtils code
Note: Code based on 'Introduction to C Programming' tutorial
      by Michael E. Leverington
*/

// Header Files ///////////////////////////////////////////////////
#include "StringUtils.h"

// global constants
const int MAX_STR_LEN = 200;
const int STD_STR_LEN = 80;
const int SUBSTRING_NOT_FOUND = -101;
const int STR_EQ = 0;
const char NULL_CHAR = '\0';
const char SPACE = ' ';
const char COLON = ':';
const char PERIOD = '.';
const char SEMICOLON = ';';
const char RIGHT_PAREN = ')';
const char LEFT_PAREN = '(';
const char DECIMAL = '.';
const Boolean IGNORE_LEADING_WS = True;
const Boolean ACCEPT_LEADING_WS = False;

/*
Function name: getStringLength
Algorithm: Find the length of the string, up to the null character
Precondition: Given C-style with null character at end
Postcondition: Return the number of characters (int) from the beginning
               up to the null character
Exceptions: None
Note: Limit test loop to maximum characters for safety
*/
int getStringLength( char *testStr )
{
    // Initialize function/variables
    int index = 0;

    while ( index < MAX_STR_LEN && testStr[ index ] != NULL_CHAR )
    {
        index++;
    }

    return index;
}

/*
Function name: copyString
Algorithm: Copies contents of one string into another
Precondition: Given C-style source string, having a null character ('\0')
              at end of string; destination string is passed in
              as a parameter with enough memory to accept the source string
Postcondition: Destination string contains an exact copy
               of the source string
Exceptions: None
Note: Limit test loop to maximum characters for safety
*/
void copyString( char *destination, char *source )
{
    // Initialize function/variables
    int index = 0;

    while( index < MAX_STR_LEN && source[ index ] != NULL_CHAR )
    {
        destination[ index ] = source[ index ];
        index++;
        destination[ index ] = NULL_CHAR;
    }
}

/*
Function name: concatenateString
Algorithm: Concatenates or appends contents of one string
           onto the end of another
Precondition: Given C-style source string, having a null character ('\0')
              at end of string; destination string is passed in
              as a parameter with enough memory to accept the source string
Postcondition: Destination string contains its original string with
               the source string appended or concatenated to the end of it
Exceptions: None
Note: Limit test loop to maximum characters for safety
*/
void concatenateString( char *destination, char *source )
{
    // Initialize function/variables
    int destIndex = getStringLength( destination );
    int sourceIndex = 0;

    while( sourceIndex < MAX_STR_LEN && source[ sourceIndex ] != NULL_CHAR )
    {
        destination[ destIndex ] = source[ sourceIndex ];
        sourceIndex++; destIndex++;
        destination[ destIndex ] = NULL_CHAR;
    }
}

/*
Function name: compareString
Algorithm: Compares two strings alphabetically such that:
           if oneStr < otherStr, the function returns a value < 0
           if oneStr > otherStr, the function returns a value > 0
           if oneStr == otherStr, the function returns a 0
           if two strings are identical up to the point that one is longer,
           the difference in lengths will be returned
Precondition: Given two C-style strings, having a null character ('\0')
              at end of string
Postcondition: Integer value returned as specified
Exceptions: None
Note: Limit test loop to maximum characters for safety
*/
int compareString( char *oneStr, char *otherStr )
{
    // Initialize function/variables
    int index = 0;
    int difference;

    while( index < MAX_STR_LEN
        && oneStr[ index ] != NULL_CHAR
        && otherStr[ index ] != NULL_CHAR )
        {
            difference = oneStr[ index ] - otherStr[ index ];
            if( difference != 0 )
            {
                return difference;
            }
            index++;
        }

    return getStringLength( oneStr ) - getStringLength( otherStr );
}

/*
Function name: getSubString
Algorithm: Captures sub string within larger string
           between two inclusive indices
Precondition: Given a C-style source string, having a null character ('\0')
              at end of string, and another string parameter
              with enough memory to accept the resulting substring
Postcondition: Substring is returned as a parameter
Exceptions: Empty string returned if any of the index parameters
            are out of range
Note: Copy of source string is made internally to protect from aliasing
*/
void getSubString( char *destStr, char *sourceStr,
    int startIndex, int endIndex )
{
    // Initialize function/variables
    int sourceStrLen = getStringLength( sourceStr );
    int destIndex = 0;
    int sourceIndex = startIndex;
    char *tempSourceStr;

    if( startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen )
    {
        tempSourceStr = (char *) malloc( sourceStrLen + 1 );
        copyString( tempSourceStr, sourceStr );

        while( sourceIndex <= endIndex )
        {
            destStr[ destIndex ] = tempSourceStr[ sourceIndex ];
            destIndex++; sourceIndex++;
            destStr[ destIndex ] = NULL_CHAR;
        }

        free( tempSourceStr );
    }

    else
    {
        destStr[ 0 ] = NULL_CHAR;
    }
}

/*
Function name: findSubString
Algorithm: Linear seach for a given substring within a given test string
Precondition: Given a C-style source string, having a null character ('\0')
              at end of string, and given a search string with
              a null character ('\n') at the end of that string
Postcondition: Index of substring location returned,
               or SUBSTRING_NOT_FOUND constant is returned
Exceptions: None
Note: None
*/
int findSubString( char *testStr, char *searchSubStr )
{
    // Initialize function/variables
    int testStrLen = getStringLength( testStr );
    int masterIndex = 0;
    int searchIndex, internalIndex;

    while( masterIndex < testStrLen )
    {
        internalIndex = masterIndex;
        searchIndex = 0;

        while( internalIndex <= testStrLen
            && testStr[ internalIndex ] == searchSubStr[ searchIndex ] )
        {
            internalIndex++; searchIndex++;

            if( searchSubStr[ searchIndex ] == NULL_CHAR )
            {
                return masterIndex;
            }
        }

        masterIndex++;
    }

    return SUBSTRING_NOT_FOUND;
}

/*
Function name: setStrToLowerCase
Algorithm: Iterates through string, set all upper case letters
           to lower case without changing any other characters
Precondition: Given C-style source string, having a null character ('\0')
              at end of string, and destination string parameter
              is passed with enough memory to hold the lower case string
Postcondition: All upper case letters in given string are set
               to lower case; no change to any other characters
Exceptions: Limit on string loop in case incorrect string format
Note: Copy of source string is made internally to protect from aliasing
*/
void setStrToLowerCase( char *destStr, char *sourceStr )
{
    // initialize function/variables
    int strLen = getStringLength( sourceStr );
    char *tempStr = (char *) malloc( strLen + 1 );
    int index = 0;

    copyString( tempStr, sourceStr );

    while( index < MAX_STR_LEN && tempStr[ index ] != NULL_CHAR )
    {
        destStr[ index ] = setCharToLowerCase( tempStr[ index ] );
        index++;
        destStr[ index ] = NULL_CHAR;
    }

    free( tempStr );
}

/*
Function name: setCharToLowerCase
Algorithm: Tests character parameter for upper case, changes it to lower case,
           makes no changes if not upper case
Precondition: Given character value
Postcondition: Upper case letter is set to lower case,
               otherwise, character is returned unchanged
Exceptions: None
Note: None
*/
char setCharToLowerCase( char testChar )
{
    if( testChar >= 'A' && testChar <= 'Z' )
    {
        testChar = (char) ( testChar - 'A' + 'a' );
    }

    return testChar;
}

/*
Function name: getLineTo
Algorithm: Finds given text in file, skipping white space if specified,
           stops searching at given character or length
Precondition: File is open with valid file pointer;
              char buffer has adequate memory for data;
              stop character and length are valid
Postcondition: Ignores leading white space if specified;
               captures all printable characters and stores in string buffer
Exceptions: Returns INCOMPLETE_FILE_ERR if no valid data found;
            returns NO_ERR if sucessful operation
Note: None
*/
int getLineTo( FILE *filePtr, int bufferSize, char stopChar,
    char *buffer, Boolean omitLeadingWhiteSpace )
{
    // Initialize function/variables
    int charIndex = 0;
    int statusReturn = NO_ERR;
    Boolean bufferSizeAvailable = True;
    int charAsInt;

    charAsInt = fgetc( filePtr );
    while( omitLeadingWhiteSpace == True
        && charAsInt != (int) stopChar
        && charIndex < bufferSize
        && charAsInt <= (int) SPACE )
    {
        charAsInt = fgetc( filePtr );
    }

    while( charAsInt != (int) stopChar && bufferSizeAvailable == True )
    {
        if( isEndOfFile( filePtr ) == True )
        {
            return INCOMPLETE_FILE_ERR;
        }

        if( charAsInt >= (int) SPACE )
        {
            buffer[ charIndex ] = (char) charAsInt;
            charIndex++;
        }

        buffer[ charIndex ] = NULL_CHAR;

        if( charIndex < bufferSize -1 )
        {
            charAsInt = fgetc( filePtr );
        }

        else
        {
            bufferSizeAvailable = False;
            statusReturn = INPUT_BUFFER_OVERRUN_ERR;
        }
    }

    return statusReturn;
}

/*
Function name: isEndOfFile
Algorithm: Reports end of file, using feof
Precondition: File is open with valid file pointer
Postcondition: Reports end of file
Exceptions: None
Note: None
*/
Boolean isEndOfFile( FILE *filePtr)
{
    if( feof( filePtr ) != 0 )
    {
        return True;
    }

    return False;
}

/*
Function name: stringToFloat
Algorithm: Converts a sting value representing a number and returns a float
Precondition: Given a string representing a positive number
Postcondition: Floating point number returned
Exceptions: None
Note: None
*/
float stringToFloat( char *str )
{
    int digit;
    Boolean decimal;
    float result = 0, factor = 1;

    for( ; *str; str++ )
    {
        if( *str == PERIOD )
        {
          decimal = True;
          continue;
        }
        digit = *str - '0';

        if( digit >= 0 && digit <= 9 )
        {
            if( decimal )
            {
                factor /= 10.0f;
            }
            result = result * 10.0f + (float)digit;
        }
    }
    result *= factor;
    return result;
}
