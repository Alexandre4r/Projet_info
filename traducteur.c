#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" //importe les fonctions du fichier lexer.c

void imprim(maillon* m){
  printf("%s",m->argument);
}

int main(){

  FILE* fichier = fopen("trad.ml", "r");
  maillon* liste = lexeur(fichier);
  imprim(liste);

}
