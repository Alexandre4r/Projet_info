#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" //importe les fonctions du fichier lexer.c

// j'ai r commenté mais ntm theo il est 2h t'avais qu'a venir sale alsacien

// Ca marche pas dcp c'est en commentaire

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
void imprim(maillon *m)
{
    printf("%c :%s \n", (m->lexeme), (m->argument));
    if (m->suivant != NULL)
    {
        imprim(m->suivant);
    }
}

//test si une chaine de caractere est apres m(onne prends pas en  compte les espaces)
bool test_maillon_suivant(maillon* m, char* s){
  while(strcmp(m->argument," ") == 0){
    m=m->suivant;
    printf("/%s/", m->argument);
  }
  return(strcmp(m->argument,s) == 0);
}

bool est_fonction(maillon* m){
  if(m == NULL){return NULL;};
  if(strcmp(m->argument,"(") == 0){return true;}
  else if((m->lexeme == 'P' &&  strcmp(m->argument," ") != 0) || m->lexeme == 'O'){return false;}
  if(m->lexeme == 'E'){return false;}
  if(strcmp(m->argument,";") == 0){return false;}
  return est_fonction(m->suivant);
}

maillon* appel_fonction(maillon *m, int parentheses, char *end)
{
    if (m == NULL)
    {
        return NULL;
    }
    if (parentheses == 0)
    {
        printf("))%s", end);
        return m;
    }
    if (strcmp(m->argument, "(") == 0 && parentheses == -1)
    {
        if (strcmp(m->suivant->argument, ")") == 0)
        {
            printf("()");
            return m->suivant;
        } // cas fonction sans arguments
        printf("(ref (");
        return appel_fonction(m->suivant, 1, end);
    }
    if (strcmp(m->argument, "(") == 0 && parentheses != -1)
    {
        return appel_fonction(m->suivant, parentheses + 1, end);
    }
    if (strcmp(m->argument, ")") == 0)
    {
        return appel_fonction(m->suivant, parentheses - 1, end);
    }
    if (strcmp(m->argument, ",") == 0)
    {
        printf("), ref (");
        return appel_fonction(m->suivant, parentheses, end);
    }
    if (m->lexeme == 'V' && parentheses == -1)
    {
        printf("%s", m->argument);
        return appel_fonction(m->suivant, parentheses, end);
    }
    if (m->lexeme == 'V' && parentheses != -1)
    {
        printf("(!%s)", m->argument);
        return appel_fonction(m->suivant, parentheses, end);
    }
    printf("%s", m->argument);
    return appel_fonction(m->suivant, parentheses, end);
}

maillon *creer_declaration(maillon *m, int after_egal /*0 si avantle =, 1 si apres*/, char *end)
{
    if (m == NULL)
    {
        return NULL;
    }
    if (m->lexeme == 'T')
    {
        printf("let ");
        return creer_declaration(m->suivant, after_egal, end);
    }
    if (strcmp(m->argument, ";") == 0)
    {
        printf("%s \n", end);
        return m->suivant;
    }
  if(strcmp(m->argument," ") == 0){return creer_declaration(m->suivant,after_egal,end);}
  if(strcmp(m->argument,"=") == 0){printf(" = ref ");return creer_declaration(m->suivant,1,end);}
  if(m->lexeme == 'V' && after_egal ==  1){
    if(est_fonction(m)){
      return creer_declaration(appel_fonction(m,-1,""),after_egal, end);
    }else {printf("(!%s)", m->argument);return creer_declaration(m->suivant,after_egal,end);}}
  //prise en charge des booleens  
  if(strcmp(m->argument,"==") == 0){printf("=");return creer_declaration(m->suivant,1,end);}
  if(strcmp(m->argument,"!=") == 0){printf("<>");return creer_declaration(m->suivant,1,end);}
  if(strcmp(m->argument,"!") == 0){
    if(test_maillon_suivant(m->suivant, "(")){
      printf("not");return creer_declaration(m->suivant,1,end);
    }
    else{
      if(m->suivant->lexeme == 'V'){
        printf("not(!%s)", m->suivant->argument);
      }else{
        printf("not(%s)", m->suivant->argument);
      }
      
      return creer_declaration(m->suivant->suivant,1,end);
    }
  }
  //--------------------  
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
    }else {printf("(!%s)", m->argument);return creer_assignement(m->suivant,after_egal, end);}}
  //prise  en charge des booleens
  if(strcmp(m->argument,"==") == 0){printf("=");return creer_assignement(m->suivant,1,end);}
  if(strcmp(m->argument,"!=") == 0){printf("<>");return creer_assignement(m->suivant,1,end);}
  if(strcmp(m->argument,"!") == 0){
    if(test_maillon_suivant(m->suivant, "(")){
      printf("not");return creer_assignement(m->suivant,1,end);
    }
    else{
      if(m->suivant->lexeme == 'V'){
        printf("not(!%s)", m->suivant->argument);
      }else{
        printf("not(%s)", m->suivant->argument);
      }
      
      return creer_assignement(m->suivant->suivant,1,end);
    }
  }

  printf("%s", m->argument);
  return creer_assignement(m->suivant, after_egal, end);
}

// typecom a  modifier : reconnaitre les commentaire  // et /**/ pour les \n
maillon *creer_commentaire(maillon *m, int typecom)
{
    if (typecom == 0)
    {
        printf("(*%s*)\n", m->argument);
        return m->suivant;
    }
    else
    {
        printf("(*%s*)", m->argument);
        return m->suivant;
    }
}

maillon *return_fonction(maillon *m)
{
    if (strcmp(m->argument, ";") == 0)
    {
        printf(";\n");
        return m->suivant;
    }
    if (m->lexeme == 'V')
    {
        if (est_fonction(m))
        {
            return return_fonction(appel_fonction(m, -1, ""));
        }
        else
        {
            printf("(!%s)", m->argument);
            return return_fonction(m->suivant);
        }
    }

    if (strcmp(m->argument, "return") == 0)
    {
        return return_fonction(m->suivant);
    }
    if (strcmp(m->argument, " ") == 0)
    {
        return return_fonction(m->suivant);
    }
    printf("%s", m->argument);
    return return_fonction(m->suivant);
}

maillon *printf_(maillon *m, char *end)
{
    int parent_ouv = 1;
    int parent_ferm = 0;
    bool first_parent = true;
    if (m == NULL)
    {
        return NULL;
    }

    while (parent_ferm != parent_ouv)
    {

        if (strcmp(m->argument, "printf") == 0)
        {
            printf("Printf.printf");
        }
        // On détecte des parenthèses, si c'est la prenthèses du printf, on l'écrit pas
        else if ((strcmp(m->argument, "(") == 0) && first_parent == true)
        {
            printf(" ");
            first_parent = false;
        }
        else if ((strcmp(m->argument, "(") == 0))
        {
            parent_ouv += 1;
            printf("(");
        }

        else if (strcmp(m->argument, ")") == 0 && parent_ferm == (parent_ouv - 1))
        {
            parent_ferm = parent_ferm + 1;
        }
        else if (strcmp(m->argument, ")") == 0)
        {
            printf(")");
            parent_ferm = parent_ferm + 1;
        }

        // On fait en sorte que les virgules entre les arguments en C ne soit pas écrits, on déctecte si ces virgules sont dans une chaîne de caractère
        else if (strcmp((m->argument), ",") == 0 && m->lexeme == 'P')
        {
            printf(" ");
        }
        else if (m->lexeme == 'V' && est_fonction(m))
        {   
            printf("(");
            m=appel_fonction(m, -1, ")");
            continue;
        }
        else if (m->lexeme == 'V')
        {
            printf("(!%s)", m->argument);
        }
        else if (m->lexeme == 'A')
        {
            printf("(*%s*)", m->argument);
        }
        else
        {
            printf("%s", m->argument);
        }
        m = m->suivant;
    }

    printf("%s\n", end);
    return m->suivant;
}

maillon *parcours_conditionnelle(maillon *m, int type_condition, bool dans_accolades, char *end)
{
    // printf("|%d|",type_condition);
    if (type_condition == 3 && !dans_accolades)
    {
        printf("for ");
        char *iterateur;
        maillon *condition = NULL;
        bool after_egal = false;
        bool strict = false;
        int pas;
        maillon *toReturn = NULL;
        // On va au debut de la parenthese
        while (m != NULL && strcmp(m->argument, "(") != 0)
        {
            m = m->suivant;
        }
        m = m->suivant;
        // Initialisation
        while (m != NULL && strcmp(m->argument, ";") != 0)
        {
            if (strcmp(m->argument, " ") == 0)
            {
                m = m->suivant;
            }
            else if (m->lexeme == 'V')
            {
                if (after_egal)
                {
                    printf("(!%s)", m->argument);
                }
                else
                {
                    iterateur = m->argument;
                    printf("%s", iterateur);
                }
                m = m->suivant;
            }
            else if (m->lexeme == 'E')
            {
                after_egal = true;
                printf(" = ");
                m = m->suivant;
            }
            else
            {
                if(after_egal){
                    printf("%s", m->argument);
                }
                m = m->suivant;
            }
        }
        // On cherche le pas, pour ca on stocke la partie condition pour etre capable d'y retourner et on va a la partie incrementation
        m = m->suivant;
        condition = m;
        while (m != NULL && strcmp(m->argument, ";") != 0)
        {
            m = m->suivant;
        }
        // incremenatation

        while (m != NULL && strcmp(m->argument, "{") != 0)
        {
            if (strcmp(m->argument, "-") == 0)
            {
                pas = -1;
                printf(" downto ");
            }
            else if (strcmp(m->argument, "+") == 0)
            {
                pas = 1;
                printf(" to ");
            }
            m = m->suivant;
        }
        // on cherche maintenant la condition
        toReturn = m;
        m = condition;
        while (strcmp(m->argument, ";") != 0)
        {
            if (strcmp(m->argument, " ") == 0)
            {
                m = m->suivant;
            }
            else if (strcmp(m->argument, "<") == 0 || strcmp(m->argument, ">") == 0)
            {
                strict = true;
                m = m->suivant;
            }
            else if (strcmp(m->argument, "<=") == 0 || strcmp(m->argument, ">=") == 0)
            {
                m = m->suivant;
            }
            else if (strcmp(m->argument, iterateur) != 0)
            {
                if (m->lexeme == 'V')
                {
                    printf("(!%s)", m->argument);
                }
                else
                {
                    printf("%s", m->argument);
                }
                m = m->suivant;
            }
            else
            {
                m = m->suivant;
            }
        }
        // On rajoute maintenant +1 ou -1 en fonction de si l'egalitée est stricte et du pas
        if (strict)
        {
            if (pas == -1)
            {
                printf("+1");
            }
            else if (pas == 1)
            {
                printf("-1");
            }
        }
        m = toReturn->suivant;
        printf(" do\n");
        m = parcours_conditionnelle(m, type_condition, true, end);
        printf(" done%s\n", end);
        return m;
    }
    else if (!dans_accolades)
    {
        bool else_if = false;
        while (strcmp(m->argument, "{") != 0)
        {
            if (strcmp(m->argument, "!=") == 0)
            {
                printf("<>");
            }
            else if(strcmp(m->argument,"==") == 0)
            {
                printf("=");
            }
            else if(strcmp(m->argument,"!") == 0){
                if(test_maillon_suivant(m->suivant, "(")){
                printf("not");return creer_assignement(m->suivant,1,end);
                }
                else{
                    if(m->suivant->lexeme == 'V'){
                        printf("not(!%s)", m->suivant->argument);
                    }else{
                        printf("not(%s)", m->suivant->argument);
                    }
                    m=m->suivant;//car on a print 2 maillons
                }
            }
            else if (strcmp(m->argument, "if") == 0)
            {
                printf("%s", m->argument);
                else_if = true;
            }

            else if (m->lexeme == 'V')
            {
                printf("(!%s)", m->argument);
            }
            else
            {
                printf("%s", m->argument);
            }
            m = m->suivant;
        }
        if (type_condition == 0 || else_if)
        {
            printf(" then begin \n");
            m = parcours_conditionnelle(m, type_condition, true, end);
            printf(" end\n");
            return m;
        }
        else if (type_condition == 1)
        {
            printf(" begin \n");
            m = parcours_conditionnelle(m, type_condition, true, end);
            printf(" end\n");
            return m;
        }
        else if (type_condition == 2)
        {
            printf(" do \n");
            m = parcours_conditionnelle(m, type_condition, true, end);
            printf(" done%s\n", end);
            return m;
        }
    }
    else
    {
        if (m == NULL)
        {
            return NULL;
        }
        else if (strcmp(m->argument, "}") == 0)
        {
            return m->suivant;
        }
        else if (m->lexeme == 'T')
        {
            if (est_fonction(m))
            {
                return NULL;
            }
            else
                return parcours_conditionnelle(creer_declaration(m, 0, ";\n"), type_condition, dans_accolades, end);
        }
        else if (m->lexeme == 'V')
        {
            if (est_fonction(m))
            {
                return parcours_conditionnelle(appel_fonction(m, -1, ";\n"), type_condition, dans_accolades, end);
            }
            else
            {
                return parcours_conditionnelle(creer_assignement(m, 0, ";"), type_condition, dans_accolades, end);
            }
        }
        //  else if(strcmp(m->argument,"/") == 0){return parcours_fonction(creer_commentaire(m->suivant));}  ancienne ligne dcp jsp si j'ai modif comme de la merde
        else if (m->lexeme == 'C')
        {
            return parcours_conditionnelle(creer_commentaire(m, 0), type_condition, dans_accolades, end);
        } // commentaires //
        else if (m->lexeme == 'A')
        {
            return parcours_conditionnelle(creer_commentaire(m, 1), type_condition, dans_accolades, end);
        } // commentaires  /**/
        else if (m->lexeme == 'M')
        {
            if (strcmp(m->argument, "return") == 0)
            {
                return parcours_conditionnelle(return_fonction(m), type_condition, dans_accolades, end);
            }
            if (strcmp(m->argument, "printf") == 0)
            {
                return parcours_conditionnelle(printf_(m, ";"), type_condition, dans_accolades, end); // Fonction Printf
            }
            else if (strcmp(m->argument, "return") == 0)
            {
                return parcours_conditionnelle(m->suivant, type_condition, dans_accolades, end);
            }
            else if (strcmp(m->argument, "if") == 0)
            {
                return parcours_conditionnelle(parcours_conditionnelle(m, 0, false, ""), type_condition, dans_accolades, end);
            }
            else if (strcmp(m->argument, "else") == 0)
            {
                return parcours_conditionnelle(parcours_conditionnelle(m, 1, false, ""), type_condition, dans_accolades, end);
            }
            else if (strcmp(m->argument, "while") == 0)
            {
                return parcours_conditionnelle(parcours_conditionnelle(m, 2, false, ";"), type_condition, dans_accolades, end);
            }
            else if (strcmp(m->argument, "for") == 0)
            {
                return parcours_conditionnelle(parcours_conditionnelle(m, 3, false, ";"), type_condition, dans_accolades, end);
            }

            else
            {
                return parcours_conditionnelle(m->suivant, type_condition, dans_accolades, end);
            }
        }

        else
        {
            return parcours_conditionnelle(m->suivant, type_condition, dans_accolades, end);
        };
    }
}

maillon *parcours_fonction(maillon *m)
{
    if (m == NULL)
    {
        return NULL;
    }
    else if (strcmp(m->argument, "}") == 0)
    {
        return m->suivant;
    }
    else if (m->lexeme == 'T')
    {
        if (est_fonction(m))
        {
            return NULL;
        }
        else
            return parcours_fonction(creer_declaration(m, 0, " in"));
    }
    else if (m->lexeme == 'V')
    {
        if (est_fonction(m))
        {
            return parcours_fonction(appel_fonction(m, -1, ";\n"));
        }
        else
        {
            return parcours_fonction(creer_assignement(m, 0, ";"));
        }
    }
    //  else if(strcmp(m->argument,"/") == 0){return parcours_fonction(creer_commentaire(m->suivant));}  ancienne ligne dcp jsp si j'ai modif comme de la merde
    else if (m->lexeme == 'C')
    {
        return parcours_fonction(creer_commentaire(m, 0));
    } // commentaires //
    else if (m->lexeme == 'A')
    {
        return parcours_fonction(creer_commentaire(m, 1));
    } // commentaires  /**/
    else if (m->lexeme == 'M')
    {
        if (strcmp(m->argument, "return") == 0)
        {
            return parcours_fonction(return_fonction(m));
        }
        else if (strcmp(m->argument, "printf") == 0)
        {
            return parcours_fonction(printf_(m, ";")); // Fonction Printf
        }
        else if (strcmp(m->argument, "if") == 0)
        {
            return parcours_fonction(parcours_conditionnelle(m, 0, false, ""));
        }
        else if (strcmp(m->argument, "else") == 0)
        {
            return parcours_fonction(parcours_conditionnelle(m, 1, false, ""));
        }
        else if (strcmp(m->argument, "while") == 0)
        {
            return parcours_fonction(parcours_conditionnelle(m, 2, false, ";"));
        }
        else if (strcmp(m->argument, "for") == 0)
        {
            return parcours_fonction(parcours_conditionnelle(m, 3, false, ";"));
        }
        else
        {
            return parcours_fonction(m->suivant);
        }
    }

    else
    {
        return parcours_fonction(m->suivant);
    };
}

maillon *creer_fonction(maillon *m)
{
    if (m == NULL)
    {
        return NULL;
    }
    bool is_main = false;
    bool nom_fct = true;
    while (strcmp(m->argument, "{") != 0)
    {
        if (strcmp(m->argument, "main") == 0)
        {
            is_main = true;
        }
        else if (m->lexeme == 'V' && !is_main)
        {
            if (nom_fct)
            {
                printf("let %s", m->argument);
                nom_fct = false;
            }
            else
            {
                printf("%s", m->argument);
            }
        }
        else if (m->lexeme != 'T' && !is_main && strcmp(m->argument, " ") != 0)
        {
            printf("%s", m->argument);
        }

        m = m->suivant;
    }
    if (is_main)
    {
        return m;
    }
    else
        printf(" = \n");
    m = parcours_fonction(m);
    printf(";;\n");
    return m;
}

void parcours(maillon *m)
{
    if (m == NULL)
    {
        return;
    }
    else if (m->lexeme == 'D')
    {
        return parcours(m->suivant);
    }
    else if (m->lexeme == 'T')
    {
        if (est_fonction(m))
        {
            return parcours(creer_fonction(m));
        }
        else
        {
            return parcours(creer_declaration(m, 0, ";;"));
        }
    }
    else if (m->lexeme == 'V')
    {
        if (est_fonction(m))
        {
            return parcours(appel_fonction(m, -1, ";;\n"));
        }
        else
        {
            return parcours(creer_assignement(m, 0, ";;"));
        }
    }
    else if (m->lexeme == 'C')
    {
        return parcours(creer_commentaire(m, 0));
    } // commentaires //
    else if (m->lexeme == 'A')
    {
        return parcours(creer_commentaire(m, 1));
    } // commentaires  /**/
    else if (m->lexeme == 'M')
    {
        if (strcmp(m->argument, "printf") == 0)
        {
            return parcours(printf_(m, ";;")); // Fonction Printf
        }
        else if (strcmp(m->argument, "return") == 0)
        {
            return parcours(m->suivant);
        }
        else if (strcmp(m->argument, "if") == 0)
        {
            return parcours(parcours_conditionnelle(m, 0, false, ""));
        }
        else if (strcmp(m->argument, "else") == 0)
        {
            return parcours(parcours_conditionnelle(m, 1, false, ""));
        }
        else if (strcmp(m->argument, "while") == 0)
        {
            return parcours(parcours_conditionnelle(m, 2, false, ";;"));
        }
        else if (strcmp(m->argument, "for") == 0)
        {
            return parcours(parcours_conditionnelle(m, 3, false, ";;"));
        }
        else
        {
            return parcours(m->suivant);
        }
    }
    else
    {
        return parcours(m->suivant);
    }
}

int main(int argc, char *argv[])
{
    if(argc>=2){
        FILE *fichierML = fopen(argv[2], "r");
        FILE *fichierC = fopen(/*fichier_a_traduire */ argv[1], "r");
        maillon *liste = lexeur(fichierC);
        imprim(liste);
        parcours(liste);
    }
    
}
