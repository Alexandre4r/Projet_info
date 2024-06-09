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
  if(strcmp(m->argument,"(") == 0){return true;}
  else if((m->lexeme == 'P' &&  strcmp(m->argument," ") != 0) || m->lexeme == 'O'){return false;}
  if(m->lexeme == 'E'){return false;}
  if(strcmp(m->argument,";") == 0){return false;}
  return est_fonction(m->suivant);
}
maillon* appel_fonction(maillon* m, int parentheses, char* end){
  if(m == NULL){return NULL;}
  if(parentheses==0){printf("))%s",end);return m;}
  if(strcmp(m->argument,"(") == 0 && parentheses==-1){
    if (strcmp(m->suivant->argument,")") == 0){printf("()");return m->suivant;}//cas fonction sans arguments
    printf("(ref (");return appel_fonction(m->suivant, 1, end);}
  if(strcmp(m->argument,"(") == 0 && parentheses!=-1){return appel_fonction(m->suivant, parentheses+1, end);}
  if(strcmp(m->argument,")") == 0){return appel_fonction(m->suivant, parentheses-1, end);}
  if(strcmp(m->argument,",") == 0){printf("), ref (");return appel_fonction(m->suivant, parentheses, end);}
  if(m->lexeme == 'V'&& parentheses==-1){printf("%s", m->argument);return appel_fonction(m->suivant, parentheses, end);}
  if(m->lexeme == 'V'&& parentheses!=-1){printf("!%s", m->argument);return appel_fonction(m->suivant, parentheses, end);}
  printf("%s", m->argument);
  return appel_fonction(m->suivant, parentheses, end);
}


maillon* creer_declaration(maillon* m  ,int after_egal /*0 si avantle =, 1 si apres*/, char* end){
  if(m == NULL){return NULL;}
  if(m->lexeme == 'T'){printf("let ");return creer_declaration(m->suivant,after_egal,end);}
  if(strcmp(m->argument,";") == 0){
    printf("%s \n",end);return m->suivant;
    }
  if(strcmp(m->argument," ") == 0){return creer_declaration(m->suivant,after_egal,end);}
  if(strcmp(m->argument,"=") == 0){printf(" = ref ");return creer_declaration(m->suivant,1,end);}
  if(m->lexeme == 'V' && after_egal ==  1){
    if(est_fonction(m)){
      return creer_declaration(appel_fonction(m,-1,""),after_egal, end);
    }else {printf("!%s", m->argument);return creer_declaration(m->suivant,after_egal,end);}}
  if(strcmp(m->argument," ") != 0){printf("%s", m->argument);return creer_declaration(m->suivant,after_egal,end);};
  return creer_declaration(m->suivant,after_egal,end);
}

maillon* creer_assignement(maillon* m, int after_egal, char* end){
  if(m == NULL){return NULL;}
  if(strcmp(m->argument,";") == 0){printf("%s\n", end);return m->suivant;}
  if(strcmp(m->argument," ") == 0){return creer_assignement(m->suivant, after_egal, end);}
  if(strcmp(m->argument,"=") == 0){printf(" := ");return creer_assignement(m->suivant,1, end);}
  if(m->lexeme == 'V' && after_egal == 1){
    if(est_fonction(m)){
      return creer_assignement(appel_fonction(m,-1,""),after_egal, end);
    }else {printf("!%s", m->argument);return creer_assignement(m->suivant,after_egal, end);}}
  printf("%s", m->argument);
  return creer_assignement(m->suivant, after_egal, end);
}


//typecom a  modifier : reconnaitre les commentaire  // et /**/ pour les \n 
maillon* creer_commentaire(maillon* m, int typecom){
  if(typecom == 0){printf("(*%s*)\n", m->argument);return m->suivant;}
  else{printf("(*%s*)", m->argument);return m->suivant;}

}
maillon* return_fonction(maillon* m){
  if(strcmp(m->argument,";") == 0){printf(";\n");return m->suivant;}
  if(m->lexeme == 'V'){    
    if(est_fonction(m)){
      return return_fonction(appel_fonction(m,-1,""));
    }else {
        printf("!%s", m->argument);
        return return_fonction(m->suivant);
        }
  }

  if(strcmp(m->argument,"return") == 0){return return_fonction(m->suivant);}
  if(strcmp(m->argument," ") == 0){return return_fonction(m->suivant);}
  printf("%s", m->argument);
  return return_fonction(m->suivant);
}
maillon* parcours_fonction(maillon* m){
  if(m == NULL){return NULL;}
  else if(strcmp(m->argument,"}") == 0){
    return m->suivant;}
  else if(m->lexeme == 'T'){
    if(est_fonction(m)){
      return NULL;
    }else
    return parcours_fonction(creer_declaration(m,0," in"));}
  else if(m->lexeme == 'V'){    
    if(est_fonction(m)){
      return parcours_fonction(appel_fonction(m,-1,";\n"));
    }else {
        return parcours_fonction(creer_assignement(m, 0,";"));}}
//  else if(strcmp(m->argument,"/") == 0){return parcours_fonction(creer_commentaire(m->suivant));}  ancienne ligne dcp jsp si j'ai modif comme de la merde
  else if(m->lexeme == 'C'){return parcours_fonction(creer_commentaire(m, 0));} // commentaires //
  else if(m->lexeme == 'A'){return parcours_fonction(creer_commentaire(m, 1));} // commentaires  /**/
  else if(m->lexeme == 'M'){
    if(strcmp(m->argument,"return") == 0){
      return parcours_fonction(return_fonction(m)); //Fonction Printf
    }
  }

  else{return parcours_fonction(m->suivant);};
}

maillon* creer_fonction(maillon* m){
  if(m == NULL){return NULL;}
  bool is_main = false;
  bool nom_fct = true;
  while(strcmp(m->argument,"{") != 0){
    if (strcmp(m->argument,"main") == 0){
      is_main = true;
    }
    else if(m->lexeme=='V' && !is_main){
      if (nom_fct){printf("let %s", m->argument);nom_fct=false;}
      else {printf("%s", m->argument);}
    }
    else if(m->lexeme!='T' && !is_main && strcmp(m->argument," ") != 0){printf("%s", m->argument);}
    
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
maillon* parcours_conditionnelle(maillon* m){
  if(m == NULL){return NULL;}
  else if(strcmp(m->argument,"}") == 0){
    return m->suivant;}
  else if(m->lexeme == 'T'){
    if(est_fonction(m)){
      return NULL;
    }else
    return parcours_conditionnelle(creer_declaration(m,0,";;\n"));}
  else if(m->lexeme == 'V'){    
    if(est_fonction(m)){
      return parcours_conditionnelle(appel_fonction(m,-1,";;\n"));
    }else {
        return parcours_conditionnelle(creer_assignement(m, 0,";;"));}}
//  else if(strcmp(m->argument,"/") == 0){return parcours_fonction(creer_commentaire(m->suivant));}  ancienne ligne dcp jsp si j'ai modif comme de la merde
  else if(m->lexeme == 'C'){return parcours_conditionnelle(creer_commentaire(m, 0));} // commentaires //
  else if(m->lexeme == 'A'){return parcours_conditionnelle(creer_commentaire(m, 1));} // commentaires  /**/
  else if(m->lexeme == 'M'){
    if(strcmp(m->argument,"return") == 0){
      return parcours_conditionnelle(return_fonction(m)); 
    }
  }

  else{return parcours_fonction(m->suivant);};
}

maillon* creer_conditionnelle(maillon* m, int if_or_while){
  if(m == NULL){return NULL;}
  while(strcmp(m->argument,"{") != 0){
    if(strcmp(m->argument,"!=") == 0){
        printf("<>");
    }
    else {
    printf("%s", m->argument);
    }
    m = m->suivant;
  }
  if (if_or_while==1){
    printf(" do \n");
    m= parcours_conditionnelle(m); 
    printf("done;;\n");
  }
  else{
    printf(" then begin \n");
    m= parcours_conditionnelle(m); 
    printf(" end;;\n");
  
  }
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
    }else {
    return parcours(creer_declaration(m, 0,";;"));}}
  else if(m->lexeme == 'V'){    
    if(est_fonction(m)){
      return parcours(appel_fonction(m,-1,";;\n"));
    }else {
        return parcours(creer_assignement(m, 0,";;"));}}
  else if(m->lexeme == 'C'){return parcours(creer_commentaire(m, 0));} // commentaires //
  else if(m->lexeme == 'A'){return parcours(creer_commentaire(m, 1));} // commentaires  /**/
  else if(m->lexeme == 'M'){
    if(strcmp(m->argument,"printf") == 0){
      return parcours(printf_(m)); //Fonction Printf
    }
    if(strcmp(m->argument,"return") == 0){
      return parcours(m->suivant);
    }
    if(strcmp(m->argument,"if") == 0){
      return parcours(creer_conditionnelle(m,0));
    }
    if(strcmp(m->argument,"while") == 0){
      return parcours(creer_conditionnelle(m,1)); 
    }
    else{return parcours(m->suivant);}
  }
  else{return parcours(m->suivant);}
}

int main(){
  FILE* fichierML = fopen("trad.ml", "r");
  FILE* fichierC = fopen("fichier.c", "r");
  maillon* liste = lexeur(fichierC);
  imprim(liste);
  parcours(liste);
}

