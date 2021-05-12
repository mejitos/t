// Stringbuilder to build strings Java/C# style.
//
// As of now, only functionality is concatenating the strings.
//
// TODO(timo): sb_init() could take the initial capacity as argument
//
// Author: Timo Mehto
// Date: 2021/05/12

#ifndef t_stringbuilder_h
#define t_stringbuilder_h


// Structure for stringbuilder and it's data.
//
// Fields
//      string: Pointer to the start of the current string.
//      length: Current length of the string.
//      capacity: Maximum length of the string.
typedef struct stringbuilder
{
    char* string;
    int length;
    int capacity;
} stringbuilder;


// Initializes new stringbuilder structure.
//
// Returns
//      Pointer to the newly created stringuilder.
stringbuilder* sb_init();


// Appends new piece of string to the end of the current string.
//
// Arguments
//      sb: Stringbuilder where the string is being appended to.
//      string: String being appended to the stringbuilder.
void sb_append(stringbuilder* sb, const char* string);


// Frees the memory allocated for a stringbuilder structure.
//
// Arguments
//      sb: Stringuilder to be freed.
void sb_free(stringbuilder* sb);


// Turns the stringbuilder to string.
//
// Arguments
//      sb: Stringbuilder being changed to string.
// Returns
//      Pointer to start of the string representation.
char* sb_to_string(stringbuilder* sb);


#endif
