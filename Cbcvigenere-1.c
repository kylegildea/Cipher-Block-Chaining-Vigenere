//------------------------------------------------------------------
// University of Central Florida
// COP3360 - Fall 2016
// Program Author: Kyle Gildea
//------------------------------------------------------------------

#include <stdio.h> 
#include <string.h> 
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>
#define BUFFERSIZE 5000
#define ASCII_DIFFERENCE 97

typedef struct cipher_info
{
  char *originalText;
  char *originalTextWithPadding;
  char *convertedText;
  char *fileName;
  char *vigenereKeyword;
  char *initializationVector;
  int blockSize;
  int totalChars;
  int paddedChars;
  
}cipher_info;

void errorPrint(char *error);
void print(cipher_info *cipher);
void freeLineBuffer(char *line);
void readFile(char *fileName, char *key, char *iv);
void encrypt(cipher_info *cipher);
void padStrLen(cipher_info *cipher);
void xor(char *plaintextBlock, char *cipherBlock, int blockLength);
void vigenere(char *key, char *cipherBlock, int blockLength);
void freeCipher(cipher_info *cipher);


int main(int argc, char **argv)
{

  if(strlen(argv[2]) != strlen(argv[3]))
  {
    errorPrint("ERROR Key and Initialization Vector Must Be The Same Number of Characters!\n");
    return -1; 
  }
  
  readFile(argv[1], argv[2], argv[3]);
  
  return 0;
}

void errorPrint(char *error) 
{ 
  fprintf(stderr, "%s", error);
}

void readFile(char *fileName, char *key, char *iv)
{
  FILE *inputFile = fopen(fileName, "r");
  char* inChar;
  char *error;
  cipher_info *cipher;
  int blockLen = strlen(key);
  
  if (inputFile == NULL)
  {
    error = malloc(BUFFERSIZE * sizeof(char));
    strcpy(error, "ERROR: could not open file ");
    strcat(error, fileName);
    strcat(error, "\n\n");
    errorPrint(error);
    free(error);
  }
  else
  {
    cipher = malloc(sizeof(cipher_info));
    if (cipher == NULL) 
    {   
      errorPrint("Cipher Malloc Failed!\n");   
    }
    
    cipher->totalChars = 0;
    cipher->blockSize = blockLen;
    
    cipher->fileName = malloc(strlen(fileName) * sizeof(char));
    if (cipher->fileName == NULL) 
    {   
      errorPrint("fileName Malloc Failed!\n");   
    }
    else
    {
      strcpy(cipher->fileName, fileName);
    }
    
    cipher->initializationVector = malloc(cipher->blockSize * sizeof(char));
    if (cipher->initializationVector == NULL) 
    {   
      errorPrint("initializationVector Malloc Failed!\n");   
    }
    else
    {
      strcpy(cipher->initializationVector, iv);
    }
    
    cipher->vigenereKeyword = malloc(cipher->blockSize * sizeof(char));
    if (cipher->vigenereKeyword == NULL) 
    {   
      errorPrint("vigenereKeyword Malloc Failed!\n");   
    }
    else
    {
      strcpy(cipher->vigenereKeyword, key);
    }
    cipher->originalText = malloc(BUFFERSIZE * sizeof(char));
    inChar = malloc(2 * sizeof(char));
    
    if (cipher->originalText == NULL) 
    {   
      errorPrint("originalText Malloc Failed!\n");   
    }
    
    else
    {  
      
      do
      {
        inChar[0] = fgetc(inputFile);
        inChar[1] = '\0';
      
        if(isalpha(inChar[0]))
        {
          inChar[0] = tolower(inChar[0]);
          strcat(cipher->originalText, inChar);
          cipher->totalChars++;
        }
          
      }while(inChar[0] != EOF);
    
    free(inChar);
    fclose(inputFile);
    padStrLen(cipher);  
    }
  encrypt(cipher);
  }
  
}

void padStrLen(cipher_info *cipher)
{
  char *padX = "x";
  int i;
  int padChars;
  
  
  if(strlen(cipher->originalText) % cipher->blockSize !=0)
  {
    padChars= cipher->blockSize - (strlen(cipher->originalText) % cipher->blockSize);
    cipher->originalTextWithPadding = malloc(strlen(cipher->originalText) * sizeof(char) + padChars + 1);
    strcpy(cipher->originalTextWithPadding, cipher->originalText);
    
    for(i = 0; i < padChars; i++)
    {
     strcat(cipher->originalTextWithPadding, padX);
    }
  }
  
  cipher->paddedChars = padChars;
}

void encrypt(cipher_info *cipher)
{
  int i;
  char *encryptedString;
  
  char *cipherBlock;
  char *plaintextBlock;
  char *bookmark;
  int loops = strlen(cipher->originalTextWithPadding) / cipher->blockSize;
  char *p;
  
  p = cipher->originalTextWithPadding;
  
  plaintextBlock = malloc(cipher->blockSize * sizeof(char));
  cipherBlock = malloc(cipher->blockSize * sizeof(char));
  
  strcpy(cipherBlock, cipher->initializationVector);
  strncpy(plaintextBlock, cipher->originalTextWithPadding, cipher->blockSize);
 
  xor(plaintextBlock, cipherBlock, cipher->blockSize);
  vigenere(cipher->vigenereKeyword, cipherBlock, cipher->blockSize);
  
  cipher->convertedText = malloc(strlen(cipher->originalTextWithPadding) * sizeof(char));
  strcpy(cipher->convertedText, cipherBlock);
  
  for(i = 1; i < loops; i++)
  {
    strncpy(plaintextBlock, p + i * (cipher->blockSize), cipher->blockSize);
    xor(plaintextBlock, cipherBlock, cipher->blockSize);
    vigenere(cipher->vigenereKeyword, cipherBlock, cipher->blockSize);
    
    strcat(cipher->convertedText, cipherBlock);            
  }
  free(plaintextBlock);
  free(cipherBlock);
  print(cipher);
  freeCipher(cipher);
}

void xor(char *plaintextBlock, char *cipherBlock, int blockLength)
{
  char *xorString;
  int i, letters = 26;  
  
  xorString = malloc(blockLength * sizeof(char));
  
  for(i = 0; i < blockLength; i++)
  {
    char toEnc = (((int)plaintextBlock[i]- ASCII_DIFFERENCE) + ((int)cipherBlock[i]- ASCII_DIFFERENCE));
    toEnc = (int)(toEnc) % letters;
    toEnc = (int)(toEnc) + ASCII_DIFFERENCE;
    xorString[i] = toEnc;
  }
  xorString[i] = '\0';
  strcpy(cipherBlock, xorString);
  
  free(xorString);
}

void vigenere(char *key, char *cipherBlock, int blockLength)
{
  char *vignereString;
  int i, letters = 26;  
  
  vignereString = malloc(blockLength * sizeof(char));
  
  for(i = 0; i < blockLength; i++)
  {
    char toEnc = (((int)key[i]- ASCII_DIFFERENCE) + ((int)cipherBlock[i]- ASCII_DIFFERENCE));
    toEnc = (int)(toEnc) % letters;
    toEnc = (int)(toEnc) + ASCII_DIFFERENCE;
    vignereString[i] = toEnc;
  }
  vignereString[i] = '\0';
  strcpy(cipherBlock, vignereString); 
  
  free(vignereString);
}

void print(cipher_info *cipher)
{
  int i;
  printf("\nCBC Vignere by Kyle Gildea\n");
  printf("Plaintext filename: %s\n", cipher->fileName);
  printf("Vigenere keyword: %s\n", cipher->vigenereKeyword);
  
  
  printf("\nClean Plaintext: \n\n");
  for(i = 0; i < strlen(cipher->originalText); i++)
  {
    if(i % 80 == 0 && i != 0)
      printf("\n%c", cipher->convertedText[i]);
    else
      printf("%c", cipher->originalText[i]);
  }
  
   printf("\n\nCiphertext: \n\n");
   for(i = 0; i < strlen(cipher->convertedText); i++)
  {
    if(i % 80 == 0 && i != 0)
      printf("\n%c", cipher->convertedText[i]);
    else
      printf("%c", cipher->convertedText[i]);
  }
  
  printf("\n\nNumber of characters in clean plaintext file: %d\n", cipher->totalChars);
  printf("Block size = %d\n", cipher->blockSize);
  printf("Number of pad characters added: %d\n\n", cipher->paddedChars);
}

void freeCipher(cipher_info *cipher)
{
  free(cipher->originalText);
  free(cipher->originalTextWithPadding);
  free(cipher->convertedText);
  free(cipher->fileName);
  free(cipher->vigenereKeyword);
  free(cipher->initializationVector);
  free(cipher);
}