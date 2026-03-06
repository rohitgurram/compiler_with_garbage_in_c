#include <string.h>
#include <stdio.h>

#include"common.h"
#include "scanner.h"

typedef struct 
{
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source){
    scanner.start=source;
    scanner.current=source;
    scanner.line=1;
}

static bool isAlpha(char c){
    return (c>='a'&& c<='z')|| c=='_' || (c>='A'&& c<='Z');
}
 
static bool isDigit(char c){
    return c>='0'&& c<='9';
}

static bool isAtEnd(){
    return *scanner.current=='\0';
}

static char advanceChar(){
    scanner.current++;
    return scanner.current[-1];
}

static char peekChar(){
    return *scanner.current;
}

static char peekNext(){
  if(isAtEnd()) return '\0';
  return scanner.current[1];
}

static bool matched(char expected){
    if(isAtEnd()) return false;
    if(*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

static Token makeToken(TokenType type){
    Token token;
    token.type=type;
    token.start=scanner.start;
    token.length=(int)(scanner.current-scanner.start);
    token.line=scanner.line;
    return token;
}

static Token errorToken(const char* message){
    Token token;
    token.type=TOKEN_ERROR;;
    token.start=message;
    token.length=(int)strlen(message);
    token.line=scanner.line;
    return token;
}

static void skipWhiteSpace(){
    for(;;){
        char c=peekChar();
        switch(c){
            case ' ':
            case '\r':
            case '\t':
             advanceChar();
             break;
            case '\n':
             scanner.line++;
             advanceChar();
             break;
            case '/':
              if (peekNext()=='/'){
                 while(peekChar()!='\n' && !isAtEnd()) advanceChar();
              }else{
                return;
              }
              break;
            default:
            return ;
        }
    }
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type){
    if(scanner.current-scanner.start ==start+length && memcmp(scanner.start+start,rest,length)==0){
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType(){
    switch (scanner.start[0]) {
    case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
          case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
        }
      }
      break;
    case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
          case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
        }
      }
      break;
    case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    
    
  }
    return TOKEN_IDENTIFIER;
}

static Token identifier(){
    while(isAlpha(peekChar()) || isDigit(peekChar())) advanceChar();
    return makeToken(identifierType());
}

static Token numberFun(){
   while( isDigit(peekChar()) ) advanceChar();

   if(peekChar()=='.' && isDigit(peekNext())){
    advanceChar();

    while( isDigit(peekChar()) ) advanceChar();
   }
   return makeToken(TOKEN_NUMBER);

}

static Token string (){
  while(peekChar()!= '"' && !isAtEnd()) {
  if(peekChar()== '\n') scanner.line++;
  advanceChar();
  }

  if(isAtEnd()) return errorToken("Unterminated String");

  advanceChar();

  return makeToken(TOKEN_STRING);
}

Token scanToken(){
    skipWhiteSpace();
    scanner.start=scanner.current;

    if(isAtEnd()) return makeToken(TOKEN_EOF);
    
    char c = advanceChar();
    
    if(isAlpha(c)) return identifier();
    if(isDigit(c))  return  numberFun();
    
   switch (c) {
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case '-': return makeToken(TOKEN_MINUS);
    case '+': return makeToken(TOKEN_PLUS);
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);
    case '!':
      return makeToken(
          matched('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return makeToken(
          matched('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(
          matched('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(
          matched('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
      return string();
    }

    return errorToken("Unexpected character.");
}
