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
/*
void ntm(char* a, char* valeur){
  objet* o =  malloc(sizeof(objet));
  o->type_objet = "VAR_DECLARATION";
  o->decl->nom  = "tadaronne";
  o->decl->valeur = "balec";

}
*/

void imprim(maillon* m){
  printf("%c, :%s \n",(m->lexeme), (m->argument));
  if (m->suivant !=NULL){
    imprim(m->suivant);
  }
}
char* compare(char* c, char** t, int n){
  for (int i =0; i<=n; i++){
   if (t[i]==c){
    return c;
   }
  }
  return "";
}
maillon* next (maillon* m){
  maillon* a = m->suivant;
  while (a->argument = " ") {
    a = a->suivant;
  }
  return a;
}

maillon* assign (maillon* m){
  printf("let");
  maillon* a = next(m);
  printf(" %s",m->argument);
  a=next(next(a));
  printf(" %s",m->argument);
  return a->suivant;
}


void traitement(maillon* m){
  // comparer m->argument a un tableau pour trouver mot cle
  //printf("stephanie la catin");
  if (m->suivant == NULL){
    printf("\n END");
  }
  else if (m->argument == "int" || m->argument == "bool"){
    //verifie si pas une fct
    traitement(assign(m));
    
  }
  else if (m->argument == ";;"){
    printf(";;");
  }
  else {
    printf("%s",(m->argument));
    traitement(m->suivant);
  }
}





int main(){

  FILE* fichierML = fopen("trad.ml", "w");
  FILE* fichierC = fopen("fichier.c", "r");
  maillon* liste = lexeur(fichierC);
  traitement(liste->suivant);

}
