#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t _strlen(const char* str);
int _strcpybxy(char **dest, const char *src, int x, int y);
int stringToInt(char* str,int* res);
int tokenisation(const char* src,size_t* nbrSize,int** tabOfNum,size_t* opeSize,char** tabOfOpe);
int calcule(size_t numberOfOpe,char* tabOfOpe,size_t numberOfNbr,int* tabOfNum,double* res);
int addCharToCalculus(char c,char** str,size_t *sizeOfStr);
int isOperator(char c);
int isNumber(char c);
int valideCalcule(const char* src);
int calcule(size_t numberOfOpe,char* tabOfOpe,size_t numberOfNbr,int* tabOfNum,double* res);
int decalage(void **liste, size_t *size, size_t elem_size, int x);
int doubleToString(double num,char** dest);

// 1 > Yes, 0 > No
int isNumber(char c){
    if(c>='0' && c<='9'){
        return 1;
    }else{
        return 0;
    }
}

// 1 > Yes, 0 > No
int isOperator(char c){
    switch (c)
    {
    case '/':
    case '*':
    case '-':
    case '+':
        return 1;
    }
    return 0;
}

int addCharToCalculus(char c,char** str,size_t *sizeOfStr){
    if(*str == NULL){
        *str = malloc(2);
        if(*str == NULL){
            return 1;
        }
        *(sizeOfStr) = 2;
        (*str)[0] = c;
        (*str)[1] = '\0';
        return 0;
    }else{
        char* tmp = realloc(*str,(*sizeOfStr)+1);
        if(tmp == NULL){
            return 1;
        }
        *str = tmp;
        (*sizeOfStr)++;
    }

    (*str)[*sizeOfStr-2] = c;
    (*str)[*sizeOfStr-1] = '\0';

    return 0;
}


int tokenisation(const char* src,size_t* nbrSize,int** tabOfNum,size_t* opeSize,char** tabOfOpe){
    if(*tabOfNum == NULL){
        *tabOfNum = malloc(1*sizeof(int));
    }
    if(*tabOfOpe == NULL){
        *tabOfOpe = malloc(1*sizeof(int));
    }
    *nbrSize = 0;
    *opeSize = 0;
    size_t sizeStr = _strlen(src);
    if(sizeStr == (size_t)-1){
        return 1;
    }

    char* buffer = malloc(sizeStr*sizeof(char));
    if(buffer == NULL){
        return 1;
    }
    int bufferNBR;
    size_t lastStart = 0;
    for(size_t i=0;i<sizeStr;i++){
        for(size_t j=i;j<sizeStr;j++){
            if(src[j] < '0' || src[j] > '9'){
                _strcpybxy(&buffer,src,i,j-1);
                stringToInt(buffer,&bufferNBR);
                int* tmpNum = realloc(*tabOfNum, (*nbrSize + 1) * sizeof(int));
                if(tmpNum == NULL){
                    return 1;
                }
                *tabOfNum = tmpNum;
                (*tabOfNum)[*nbrSize] = bufferNBR;
                (*nbrSize)++;


                if(isOperator(src[j])){
                    char* tmpOpe = realloc(*tabOfOpe, (*opeSize  + 1) * sizeof(char));
                    if(tmpOpe == NULL){
                        return 1;
                    }
                    *tabOfOpe = tmpOpe;
                    (*tabOfOpe)[*opeSize] = src[j];
                    (*opeSize)++;
                }

                lastStart = j + 1;
                i=j;
                break;
            }
        }
    }

    if(lastStart < sizeStr){
        _strcpybxy(&buffer, src, lastStart, sizeStr - 1);
        stringToInt(buffer, &bufferNBR);
        int* tmpNum = realloc(*tabOfNum, (*nbrSize + 1) * sizeof(int));
        if(tmpNum == NULL){
            return 1;
        }
        *tabOfNum = tmpNum;
        (*tabOfNum)[*nbrSize] = bufferNBR;
        (*nbrSize)++;
    }


    return 0;
}

int valideCalcule(const char* src){
    size_t sizeStr = _strlen(src);
    if(sizeStr == (size_t)-1){
        return 1;
    }

    if(sizeStr == 0){
        return 1;
    }else if(!isNumber(src[0])){
        return 1;
    }else if(src[sizeStr-1] == '*' || src[sizeStr-1] == '/' || src[sizeStr-1] == '+' || src[sizeStr-1] == '-'){
        return 1;
    }

    for(size_t i=1;i<sizeStr;i++){
        if(!isOperator(src[i]) && !isNumber(src[i])){
            return 1;
        }
        if(isOperator(src[i])){
            if(isOperator(src[i-1])){
                return 1;
            }
        }
    }

    return 0;
}

int calcule(size_t numberOfOpe,char* tabOfOpe,size_t numberOfNbr,int* tabOfNum,double* res){
    size_t sizeOfBufferNbr = numberOfNbr;
    double* tabBufferNumber = malloc(sizeof(double) * numberOfNbr);
    if(tabBufferNumber == NULL){
        return 1;
    }
    for(size_t i=0;i<numberOfNbr;i++){
        tabBufferNumber[i] = (double)tabOfNum[i];
    }
    size_t sizeOfBufferOpe = numberOfOpe;
    char* tabBufferOperator = malloc(sizeof(char) * numberOfOpe);
    if(tabBufferOperator == NULL){
        free(tabBufferNumber);
        return 1;
    }
    for(size_t i=0;i<numberOfOpe;i++){
        tabBufferOperator[i] = tabOfOpe[i];
    }

    for(size_t i=0;i<sizeOfBufferOpe;){
        if(tabBufferOperator[i] == '*' || tabBufferOperator[i] == '/'){
            if(tabBufferOperator[i] == '/' && tabBufferNumber[i+1] == 0.0){
                free(tabBufferNumber);
                free(tabBufferOperator);
                return 1;
            }
            tabBufferNumber[i] = (tabBufferOperator[i] == '*')
                ? tabBufferNumber[i] * tabBufferNumber[i+1]
                : tabBufferNumber[i] / tabBufferNumber[i+1];
            decalage((void**)&tabBufferNumber, &sizeOfBufferNbr, sizeof(double), (int)(i+1));
            decalage((void**)&tabBufferOperator, &sizeOfBufferOpe, sizeof(char), (int)i);
        }else{
            i++;
        }
    }

    double total = tabBufferNumber[0];
    for(size_t i=0;i<sizeOfBufferOpe;i++){
        total = (tabBufferOperator[i] == '+') ? total + tabBufferNumber[i+1] : total - tabBufferNumber[i+1];
    }

    *res = total;
    free(tabBufferNumber);
    free(tabBufferOperator);
    return 0;
}

int decalage(void **liste, size_t *size, size_t elem_size, int x) {
    if (x < 0 || (size_t)x >= *size){
        return 1;
    }

    char *base = *liste;  // char* pour pouvoir avancer octet par octet
    memmove(base + x * elem_size,base + (x + 1) * elem_size,(*size - x - 1) * elem_size);
    (*size)--;

    if (*size == 0) { 
        free(*liste); 
        *liste = NULL; return 0; 
    }

    void *tmp = realloc(*liste, *size * elem_size);
    if (tmp){
        *liste = tmp;  // si échec, l'ancien bloc reste valide
    }
    return 0;
}

int stringToInt(char* str,int* res){
    if(str == NULL){
        return 1;
    }
    int result = 0;
    int boolNeg = 0;
    int i = 0;
    int len = (int)_strlen(str);
    if (len == 0) {
        return 1;
    }

    if(str[len-1] == '\n'){
        len--;
    } 
    if(str[0] == '-') {
        boolNeg = 1;
        i++;
    }

    for(; i < len; i++){
        if(str[i] < '0' || str[i] > '9'){
            return 1; // caractère invalide
        }
        result = result * 10 + (str[i] - '0');
    }
    if(boolNeg == 1){
        *res = -result;
    }else{
        *res = result;
    }
    return 0;
}

int _strcpybxy(char **dest, const char *src, int x, int y){
    if(src == NULL){
        return 1;
    }
    int lenSrc = _strlen(src);
    if (x < 0 || y >= lenSrc || y < x) {
        return 1;
    }

    size_t size = y - x + 2;
    if (*dest == NULL) {
        *dest = malloc(size);
    } else {
        *dest = realloc(*dest, size);
    }
    if (*dest == NULL) {
        return 1;
    }

    int index = 0;
    for (int i = x; i <= y; i++) {
        (*dest)[index] = src[i];
        index++;
    }

    (*dest)[index] = '\0';
    return 0;
}

size_t _strlen(const char* str){
    if(str == NULL){
        return -1;
    }
    size_t i =0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

// Écrit num dans *dest (alloué/réalloué ici, à free par l'appelant). 0 > OK, 1 > erreur
int doubleToString(double num,char** dest){
    if(dest == NULL){
        return 1;
    }
    // %.12g : pas de zéros inutiles (22107 et non 22107.000000) et évite les 0.30000000000000004
    int len = snprintf(NULL,0,"%.12g",num);
    if(len < 0){
        return 1;
    }
    char* tmp = realloc(*dest,(size_t)len + 1); // realloc(NULL,...) se comporte comme malloc
    if(tmp == NULL){
        return 1;
    }
    *dest = tmp;
    snprintf(*dest,(size_t)len + 1,"%.12g",num);
    return 0;
}