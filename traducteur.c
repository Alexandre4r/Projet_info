#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" //importe les fonctions du fichier lexer.c


typedef struct {char* nom;char* valeur;}var_declaration;
typedef struct {char* body;} commentaire; //a modif
typedef struct {char* nom;char* param; maillon* body;}fonction; //a modif
typedef struct {char* nom;char* valeur;}var_assignement; //a modif
typedef struct {maillon* condition; maillon* body;}b_for; //a modif
typedef struct {maillon* condition; maillon* body;}b_while; //a modif

//type pour organiser les lexemes par "groupe syntaxique" ex fonction,declaration variable, etc
typedef struct{
  char* type_objet; //FONCTION, VAR_DECLARATION, VAR_ASSIGNEMENT, FOR, WHILE, IF, COMMENTAIRE
  // type de l'objet(!= du lexeme)
  union{//a l'avenir pour stoker les differentes stuct envisageables
    var_declaration decl;
    commentaire com;
    fonction f;
    var_assignement agt;
  };
}objet;

void ntm(char* a, char* valeur){
  objet* o =  malloc(sizeof(objet));
  o->type_objet = "VAR_DECLARATION";
  o->decl->nom  = "tadaronne";
  o->decl->valeur = "balec";

}


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
