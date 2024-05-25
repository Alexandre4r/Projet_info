#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" //importe les fonctions du fichier lexer.c

//type pour organiser les lexemes par "groupe syntaxique" ex fonction,declaration variable, etc
typedef struct{
  enum{FONCTION, VAR_DECLARATION, VAR_ASSIGNEMENT, FOR, WHILE, IF, ELSE} type_objet; 
  // type de l'objet(!= du lexeme)
  union {//a l'avenir pour stoker les differentes stuct envisageables
    struct {char* nom;char* valeur;} var_declaration;
  };
}objet;

void imprim(maillon* m){
  printf("%d, :%s \n",(m->lexeme), (m->argument));
  if (m->suivant !=NULL){
    imprim(m->suivant);
  }
}


int main(){

  FILE* fichierML = fopen("trad.ml", "r");
  FILE* fichierC = fopen("fichier.c", "r");
  maillon* liste = lexeur(fichierC);
  affiche_liste(liste);

}
