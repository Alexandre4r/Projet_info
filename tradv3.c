#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" //importe les fonctions du fichier lexer.c

//j'ai r commenté mais ntm theo il est 2h t'avais qu'a venir sale alsacien


//Ca marche pas dcp c'est en commentaire

/*
typedef struct {char* nom;char* valeur;}var_declaration;
typedef struct {char* body;} commentaire; //a modif
typedef struct {char* nom;char* param; maillon* body;}fonction; //a modif
typedef struct {char* nom;char* valeur;}var_assignement; //a modif
typedef struct {maillon* condition; maillon* body;}b_for; //a modif
typedef struct {maillon* condition; maillon* body;}b_while; //a modif

typedef union{//a l'avenir pour stoker les differentes stuct envisageables
    var_declaration decl;
    commentaire com;
    fonction f;
    var_assignement agt;
}union_type;

typedef enum{
  FONCTION,
  VAR_DECLARATION, 
  VAR_ASSIGNEMENT, 
  FOR, 
  WHILE, 
  IF
  }type_objet; 

//type pour organiser les lexemes par "groupe syntaxique" ex fonction,declaration variable, etc
typedef struct{
  type_objet  type; // type de l'objet(!= du lexeme)
  union_type corps;
}objet;

*/
void imprim(maillon* m){
  printf("%c, :%s \n",(m->lexeme), (m->argument));
  if (m->suivant !=NULL){
    imprim(m->suivant);
  }
}

bool est_fonction(maillon* m){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,"(") == 0){return true;};
  if(m->lexeme == 'E'){return false;};
  return est_fonction(m->suivant);
}

maillon* creer_declaration(maillon* m){
  if(m == NULL){return NULL;};
  if(m->lexeme == 'T'){printf("let ");return creer_declaration(m->suivant);};
  if(strcmp(m->argument,";") == 0){printf(";;\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_declaration(m->suivant);};
  if(strcmp(m->argument," ") != 0){printf("%s", m->argument);return creer_declaration(m->suivant);};
  return creer_declaration(m->suivant);
}

maillon* creer_assignement(maillon* m){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,";") == 0){printf(";;\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_assignement(m->suivant);};
  printf("%s ", m->argument);
  return creer_assignement(m->suivant);
}





/*j'avais pas lu  le pdf faut changer  ca  dcp(traduction:t'as interet a le faire theo)*/
maillon* creer_commentaire(maillon* m){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,"/") == 0){printf("(*");return creer_commentaire(m->suivant);}
  if(strcmp(m->argument, "\n") == 0){printf("*)\n");return m->suivant;};
  if(m->lexeme  == 'V'){printf("%s", m->argument);return creer_commentaire(m->suivant);}
  return creer_commentaire(m->suivant);

}


maillon* creer_assignement_fonction(maillon* m){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,";") == 0){printf(";\n");return m->suivant;};
  if(strcmp(m->argument,"=") == 0){printf(":= ");return creer_assignement_fonction(m->suivant);};
  if(strcmp(m->argument," ") == 0){return creer_assignement_fonction(m->suivant);};
  if(m->lexeme=='V'){printf("!%s ", m->argument);return creer_assignement_fonction(m->suivant);}
  printf("%s ", m->argument);
  return creer_assignement_fonction(m->suivant);
}

maillon* creer_declaration_fonction(maillon* m){
  //printf("(%c, :'%s')",(m->lexeme), (m->argument));
  if(m == NULL){return NULL;};
  if(m->lexeme == 'T'){printf("let ");return creer_declaration_fonction(m->suivant);};
  if(strcmp(m->argument,";") == 0){printf(" in\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_declaration_fonction(m->suivant);};
  if(strcmp(m->argument,"=") == 0){printf(" = ref "); return creer_declaration_fonction(m->suivant);};
  if(strcmp(m->argument," ") != 0){printf("%s", m->argument);return creer_declaration_fonction(m->suivant);};
  return creer_declaration_fonction(m->suivant);
}

maillon* return_fonction(maillon* m){
  //printf("(%c, :'%s')",(m->lexeme), (m->argument));
  if(m == NULL){return NULL;};
  if(m->lexeme == 'T'){printf("let ");return creer_declaration_fonction(m->suivant);};
  if(strcmp(m->argument,";") == 0){printf(" in\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_declaration_fonction(m->suivant);};
  if(strcmp(m->argument,"=") == 0){printf(" = ref "); return creer_declaration_fonction(m->suivant);};
  if(strcmp(m->argument," ") != 0){printf("%s", m->argument);return creer_declaration_fonction(m->suivant);};
  return creer_declaration_fonction(m->suivant);
}

maillon* parcours_fonction(maillon* m){
  if(m == NULL){return NULL;}
  else if(strcmp(m->argument,"}") == 0){
    return m->suivant;}
  else if(m->lexeme == 'T'){
    if(est_fonction(m)){
      return NULL;
    }else
    return parcours_fonction(creer_declaration_fonction(m));}
  else if(m->lexeme == 'V'){return parcours_fonction(creer_assignement_fonction(m));}
  else if(strcmp(m->argument,"/") == 0){return parcours_fonction(creer_commentaire(m->suivant));}
  else{return parcours_fonction(m->suivant);};
}
maillon* creer_fonction(maillon* m){
  if(m == NULL){return NULL;};
  bool is_main = false;
  bool nom_fct = true;
  while(strcmp(m->argument,"{") != 0){
    if (strcmp(m->argument,"main") == 0){
      is_main = true;
    }
    if(m->lexeme=='V'){
      if (nom_fct){printf("let %s ", m->argument);nom_fct=false;}
      else {printf("%s ", m->argument);}
    }
    
    m = m->suivant;
  }
  if (is_main){
    return m;
  }
  else
  printf("= \n");
  m= parcours_fonction(m); 
  printf(";;\n");
  return m;
}

void parcours(maillon* m){
  if(m == NULL){return;}
  else if(m->lexeme =='D'){return parcours(m->suivant);}
  else if(m->lexeme == 'T'){
    if(est_fonction(m)){
      return parcours(creer_fonction(m));
    }else
    return parcours(creer_declaration(m));}
  else if(m->lexeme == 'V'){return parcours(creer_assignement(m));}
  else if(strcmp(m->argument,"/") == 0){return parcours(creer_commentaire(m->suivant));}
  else{return parcours(m->suivant);};
}

int main(){
  FILE* fichierML = fopen("trad.ml", "r");
  FILE* fichierC = fopen("fichier.c", "r");
  maillon* liste = lexeur(fichierC);
  parcours(liste);
}

