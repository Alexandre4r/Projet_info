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
  printf("%c :%s \n",(m->lexeme), (m->argument));
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

maillon* creer_declaration(maillon* m  ,int after_egal /*0 si avantle =, 1 si apres*/){
  if(m == NULL){return NULL;};
  if(m->lexeme == 'T'){printf("let ");return creer_declaration(m->suivant,after_egal);};
  if(strcmp(m->argument,";") == 0){
    if(m->suivant->suivant != NULL){
         printf(" in\n");return m->suivant;
      }else{
        printf(";;\n");return m->suivant;
      }
    };
  if(strcmp(m->argument," ") == 0){return creer_declaration(m->suivant,after_egal);};
  if(strcmp(m->argument,"=") == 0){printf(" = ref ");return creer_declaration(m->suivant,1);};
  if(m->lexeme == 'V' && after_egal ==  1){printf("!%s", m->argument);return creer_declaration(m->suivant,after_egal);};
  if(strcmp(m->argument," ") != 0){printf("%s", m->argument);return creer_declaration(m->suivant,after_egal);};
  return creer_declaration(m->suivant,after_egal);
}

maillon* creer_assignement(maillon* m, int after_egal){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,";") == 0){printf(";;\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_assignement(m->suivant, after_egal);};
  if(strcmp(m->argument,"=") == 0){printf(" := ");return creer_assignement(m->suivant,1);};
  if(m->lexeme == 'V' && after_egal == 1){printf("!%s", m->argument);return creer_assignement(m->suivant,after_egal);};
  printf("%s ", m->argument);
  return creer_assignement(m->suivant, after_egal);
}


//typecom a  modifier : reconnaitre les commentaire  // et /**/ pour les \n 
maillon* creer_commentaire(maillon* m, int typecom){
  if(typecom == 0){printf("(*%s*)\n", m->argument);return m->suivant;}
  else{printf("(*%s*)", m->argument);return m->suivant;}

}


maillon* creer_assignement_fonction(maillon* m, int after_egal){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,";") == 0){printf(";\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_assignement_fonction(m->suivant, after_egal);};
  if(strcmp(m->argument,"=") == 0){printf(" := ");return creer_assignement_fonction(m->suivant,1);};
  if(m->lexeme == 'V' && after_egal == 1){printf("!%s", m->argument);return creer_assignement_fonction(m->suivant,after_egal);};
  printf("%s ", m->argument);
  return creer_assignement_fonction(m->suivant, after_egal);
}

maillon* creer_declaration_fonction(maillon* m,int after_egal){
  if(m == NULL){return NULL;};
  if(m->lexeme == 'T'){printf("let ");return creer_declaration_fonction(m->suivant,after_egal);};
  if(strcmp(m->argument,";") == 0){printf(" in\n");return m->suivant;};
  if(strcmp(m->argument," ") == 0){return creer_declaration_fonction(m->suivant,after_egal);};
  if(strcmp(m->argument,"=") == 0){printf(" = ref ");return creer_declaration_fonction(m->suivant,1);};
  if(m->lexeme == 'V' && after_egal ==  1){printf("!%s", m->argument);return creer_declaration_fonction(m->suivant,after_egal);};
  if(strcmp(m->argument," ") != 0){printf("%s", m->argument);return creer_declaration_fonction(m->suivant,after_egal);};
  return creer_declaration_fonction(m->suivant,after_egal);
}


maillon* parcours_fonction(maillon* m){
  if(m == NULL){return NULL;}
  else if(strcmp(m->argument,"}") == 0){
    return m->suivant;}
  else if(m->lexeme == 'T'){
    if(est_fonction(m)){
      return NULL;
    }else
    return parcours_fonction(creer_declaration_fonction(m,0));}
  else if(m->lexeme == 'V'){return parcours_fonction(creer_assignement_fonction(m,0));}
//  else if(strcmp(m->argument,"/") == 0){return parcours_fonction(creer_commentaire(m->suivant));}  ancienne ligne dcp jsp si j'ai modif comme de la merde
  else if(m->lexeme == 'C'){return parcours_fonction(creer_commentaire(m, 0));} // commentaires //
  else if(m->lexeme == 'A'){return parcours_fonction(creer_commentaire(m, 1));} // commentaires  /**/
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
    else if(m->lexeme=='P' && !is_main && strcmp(m->argument," ") != 0){printf("%s", m->argument);}
    else if(m->lexeme=='V' && !is_main){
      if (nom_fct){printf("let %s", m->argument);nom_fct=false;}
      else {printf("%s", m->argument);}
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


maillon* printf_(maillon* m){
  int parent_ouv = 1;
  int parent_ferm = 0;
  bool first_parent = true;
  if(m == NULL){return NULL;}

  while(parent_ferm != parent_ouv){

     if(strcmp(m->argument, "printf") == 0){
      printf("Printf.printf");
    }
    //On détecte des parenthèses, si c'est la prenthèses du printf, on l'écrit pas
    else if((strcmp(m->argument, "(") == 0) && first_parent == true){printf(" "); first_parent = false;}
    else if((strcmp(m->argument, "(") == 0)){
      parent_ouv += 1;
      printf("(");
    }

    else if(strcmp(m->argument,")") == 0 && parent_ferm == (parent_ouv-1)){
      parent_ferm = parent_ferm+1;
    }
    else if(strcmp(m->argument,")") == 0){
      printf(")");
      parent_ferm = parent_ferm+1;
    }

    //On fait en sorte que les virgules entre les arguments en C ne soit pas écrits, on déctecte si ces virgules sont dans une chaîne de caractère
    else if(strcmp((m->argument), ",") == 0 && m->lexeme == 'P'){
      printf(" ");
    }
    else if(m->lexeme == 'V'){
      printf("!%s", m->argument);
    }else{
      printf("%s", m->argument);
    }
    m = m->suivant;

  }

  printf(";;\n");
  return m->suivant;

}

void parcours(maillon* m){
  if(m == NULL){return;}
  else if(m->lexeme =='D'){return parcours(m->suivant);}
  else if(m->lexeme == 'T'){
    if(est_fonction(m)){
      return parcours(creer_fonction(m));
    }else{
    return parcours(creer_declaration(m,0));}
  }
  else if(m->lexeme == 'V'){return parcours(creer_assignement(m,0));}
  else if(m->lexeme == 'M'){
    if(strcmp(m->argument,"printf") == 0){
      return parcours(printf_(m)); //Fonction Printf
    }
  }
  else if(strcmp(m->argument,"/") == 0){return parcours(creer_commentaire(m->suivant,0));}
  else if(m->lexeme == 'V'){return parcours(creer_assignement(m, 0));}
  else if(m->lexeme == 'C'){return parcours(creer_commentaire(m, 0));} // commentaires //
  else if(m->lexeme == 'A'){return parcours(creer_commentaire(m, 1));} // commentaires  /**/
  else{return parcours(m->suivant);}
}


int main(){
  FILE* fichierML = fopen("trad.ml", "r");
  FILE* fichierC = fopen("fichier.c", "r");
  maillon* liste = lexeur(fichierC);
  imprim(liste);
  parcours(liste);
}

