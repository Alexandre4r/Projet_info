#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h" //importe les fonctions du fichier lexer.c

FILE *fichierML;
void imprim(maillon *m)
{
    printf("%c :%s \n", (m->lexeme), (m->argument));
    /*fonction de debug qui affiche pour chaque maillon de la liste chainée son argument et son lexeme*/
    if (m->suivant != NULL)
    {
        imprim(m->suivant);
    }
}

// test si une chaine de caractere est apres m(on ne prends pas en  compte les espaces)
bool test_maillon_suivant(maillon *m, char *s)
{
    while (strcmp(m->argument, " ") == 0)
    {
        m = m->suivant;
    }
    return (strcmp(m->argument, s) == 0);
}

bool est_fonction(maillon *m)
{
    /*fonction qui renvoie true si l'argument fait partie d'une declaration/appel de fonction et false sinon*/
    if (m == NULL)
    {
        return NULL;
    };
    if (strcmp(m->argument, "(") == 0)
    {
        return true;
    }
    else if ((m->lexeme == 'P' && strcmp(m->argument, " ") != 0) || m->lexeme == 'O')
    {
        return false;
    }
    if (m->lexeme == 'E')
    {
        return false;
    }
    if (strcmp(m->argument, ";") == 0)
    {
        return false;
    }
    return est_fonction(m->suivant);
}
bool is_next_else(maillon *m)
{
    if (m == NULL)
    {
        return NULL;
    };
    if (strcmp(m->argument, "else") == 0)
    {
        return true;
    }
    else if (strcmp(m->argument, " ") == 0)
    {
        return is_next_else(m->suivant);
    }
    return false;
}

maillon *appel_fonction(maillon *m, int parentheses, char *end)
{
    /*fonction qui traduit un appel de fonction*/
    if (m == NULL)
    {
        return NULL;
    }
    if (parentheses == 0)
    {
        fprintf(fichierML,"))%s", end);
        return m;
    }
    if (strcmp(m->argument, "(") == 0 && parentheses == -1)
    {
        if (strcmp(m->suivant->argument, ")") == 0)
        {
            fprintf(fichierML,"()");
            return m->suivant;
        } // cas fonction sans arguments
        fprintf(fichierML,"(ref (");
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
        fprintf(fichierML,"), ref (");
        return appel_fonction(m->suivant, parentheses, end);
    }
    if (m->lexeme == 'V' && parentheses == -1)
    {
        fprintf(fichierML,"%s", m->argument);
        return appel_fonction(m->suivant, parentheses, end);
    }
    if (m->lexeme == 'V' && parentheses != -1)
    {
        fprintf(fichierML,"(!%s)", m->argument);
        return appel_fonction(m->suivant, parentheses, end);
    }
    fprintf(fichierML,"%s", m->argument);
    return appel_fonction(m->suivant, parentheses, end);
}

maillon *creer_declaration(maillon *m, int after_egal /*0 si avantle =, 1 si apres*/, char *end)
{
    /*fonction qui traduit une declaration de variable*/
    if (m == NULL)
    {
        return NULL;
    }
    if (m->lexeme == 'T')
    {
        fprintf(fichierML,"let ");
        return creer_declaration(m->suivant, after_egal, end);
    }
    if (strcmp(m->argument, ";") == 0)
    {
        fprintf(fichierML,"%s \n", end);
        return m->suivant;
    }
    if (strcmp(m->argument, " ") == 0)
    {
        return creer_declaration(m->suivant, after_egal, end);
    }
    if (strcmp(m->argument, "=") == 0)
    {
        fprintf(fichierML," = ref ");
        return creer_declaration(m->suivant, 1, end);
    }
    if (m->lexeme == 'V' && after_egal == 1)
    {
        if (est_fonction(m))
        {
            return creer_declaration(appel_fonction(m, -1, ""), after_egal, end);
        }
        else
        {
            fprintf(fichierML,"(!%s)", m->argument);
            return creer_declaration(m->suivant, after_egal, end);
        }
    }
    // prise en charge des booleens
    if (strcmp(m->argument, "==") == 0)
    {
        fprintf(fichierML,"=");
        return creer_declaration(m->suivant, 1, end);
    }
    if (strcmp(m->argument, "!=") == 0)
    {
        fprintf(fichierML,"<>");
        return creer_declaration(m->suivant, 1, end);
    }
    if (strcmp(m->argument, "!") == 0)
    {
        if (test_maillon_suivant(m->suivant, "("))
        {
            fprintf(fichierML,"not");
            return creer_declaration(m->suivant, 1, end);
        }
        else
        {
            if (m->suivant->lexeme == 'V')
            {
                fprintf(fichierML,"not(!%s)", m->suivant->argument);
            }
            else
            {
                fprintf(fichierML,"not(%s)", m->suivant->argument);
            }

            return creer_declaration(m->suivant->suivant, 1, end);
        }
    }
    //--------------------
    if (strcmp(m->argument, " ") != 0)
    {
        fprintf(fichierML,"%s", m->argument);
        return creer_declaration(m->suivant, after_egal, end);
    };
    return creer_declaration(m->suivant, after_egal, end);
}

maillon *creer_assignement(maillon *m, int after_egal, char *end)
{
    /*fonction qui traduit une affectaion de variable*/
    if (m == NULL)
    {
        return NULL;
    }
    if (strcmp(m->argument, ";") == 0)
    {
        fprintf(fichierML,"%s\n", end);
        return m->suivant;
    }
    if (strcmp(m->argument, " ") == 0)
    {
        return creer_assignement(m->suivant, after_egal, end);
    }
    if (strcmp(m->argument, "=") == 0)
    {
        fprintf(fichierML," := ");
        return creer_assignement(m->suivant, 1, end);
    }
    if (m->lexeme == 'V' && after_egal == 1)
    {
        if (est_fonction(m))
        {
            return creer_assignement(appel_fonction(m, -1, ""), after_egal, end);
        }
        else
        {
            fprintf(fichierML,"(!%s)", m->argument);
            return creer_assignement(m->suivant, after_egal, end);
        }
    }
    // prise  en charge des booleens
    if (strcmp(m->argument, "==") == 0)
    {
        fprintf(fichierML,"=");
        return creer_assignement(m->suivant, 1, end);
    }
    if (strcmp(m->argument, "!=") == 0)
    {
        fprintf(fichierML,"<>");
        return creer_assignement(m->suivant, 1, end);
    }
    if (strcmp(m->argument, "!") == 0)
    {
        if (test_maillon_suivant(m->suivant, "("))
        {
            fprintf(fichierML,"not");
            return creer_assignement(m->suivant, 1, end);
        }
        else
        {
            if (m->suivant->lexeme == 'V')
            {
                fprintf(fichierML,"not(!%s)", m->suivant->argument);
            }
            else
            {
                fprintf(fichierML,"not(%s)", m->suivant->argument);
            }

            return creer_assignement(m->suivant->suivant, 1, end);
        }
    }

    fprintf(fichierML,"%s", m->argument);
    return creer_assignement(m->suivant, after_egal, end);
}

// typecom a  modifier : reconnaitre les commentaire  // et /**/ pour les \n
maillon *creer_commentaire(maillon *m, int typecom)
{
    /*fonction qui traduit un commentaire (typecom = 0 pour les commentaires // et typecom = 0 pour les commentaires /**./)*/
    if (typecom == 0)
    {
        fprintf(fichierML,"(*%s*)\n", m->argument);
        return m->suivant;
    }
    else
    {
        fprintf(fichierML,"(*%s*)", m->argument);
        return m->suivant;
    }
}

maillon *return_fonction(maillon *m)
{
    /*fonction qui  traduit un return*/
    if (strcmp(m->argument, ";") == 0)
    {
        fprintf(fichierML,";\n");
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
            fprintf(fichierML,"(!%s)", m->argument);
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
    fprintf(fichierML,"%s", m->argument);
    return return_fonction(m->suivant);
}

maillon *printf_(maillon *m, char *end)
{
    /*fonction qui  traduit un printf*/
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
            fprintf(fichierML,"Printf.printf");
        }
        // On détecte des parenthèses, si c'est la prenthèses du printf, on l'écrit pas
        else if ((strcmp(m->argument, "(") == 0) && first_parent == true)
        {
            fprintf(fichierML," ");
            first_parent = false;
        }
        else if ((strcmp(m->argument, "(") == 0))
        {
            parent_ouv += 1;
            fprintf(fichierML,"(");
        }

        else if (strcmp(m->argument, ")") == 0 && parent_ferm == (parent_ouv - 1))
        {
            parent_ferm = parent_ferm + 1;
        }
        else if (strcmp(m->argument, ")") == 0)
        {
            fprintf(fichierML,")");
            parent_ferm = parent_ferm + 1;
        }

        // On fait en sorte que les virgules entre les arguments en C ne soit pas écrits, on déctecte si ces virgules sont dans une chaîne de caractère
        else if (strcmp((m->argument), ",") == 0 && m->lexeme == 'P')
        {
            fprintf(fichierML," ");
        }
        else if (m->lexeme == 'V' && est_fonction(m))
        {
            fprintf(fichierML,"(");
            m = appel_fonction(m, -1, ")");
            continue;
        }
        else if (m->lexeme == 'V')
        {
            fprintf(fichierML,"(!%s)", m->argument);
        }
        else if (m->lexeme == 'A')
        {
            fprintf(fichierML,"(*%s*)", m->argument);
        }
        else
        {
            fprintf(fichierML,"%s", m->argument);
        }
        m = m->suivant;
    }

    fprintf(fichierML,"%s\n", end);
    return m->suivant;
}

maillon *parcours_conditionnelle(maillon *m, int type_condition, bool dans_accolades, char *end)
{
    /*fonction qui traduit les if(type_condition=0), else(type_condition=1), while(type_condition=2) et for(type_condition=3)*/
    
    //boucles FOR
    if (type_condition == 3 && !dans_accolades)
    {
        fprintf(fichierML,"for ");
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
                    fprintf(fichierML,"(!%s)", m->argument);
                }
                else
                {
                    iterateur = m->argument;
                    fprintf(fichierML,"%s", iterateur);
                }
                m = m->suivant;
            }
            else if (m->lexeme == 'E')
            {
                after_egal = true;
                fprintf(fichierML," = ");
                m = m->suivant;
            }
            else
            {
                if (after_egal)
                {
                    fprintf(fichierML,"%s", m->argument);
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
                fprintf(fichierML," downto ");
            }
            else if (strcmp(m->argument, "+") == 0)
            {
                pas = 1;
                fprintf(fichierML," to ");
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
                    fprintf(fichierML,"(!%s)", m->argument);
                }
                else
                {
                    fprintf(fichierML,"%s", m->argument);
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
                fprintf(fichierML,"+1");
            }
            else if (pas == 1)
            {
                fprintf(fichierML,"-1");
            }
        }
        m = toReturn->suivant;
        fprintf(fichierML," do\n");
        m = parcours_conditionnelle(m, type_condition, true, end);
        fprintf(fichierML," done%s\n", end);
        return m;
    }
    //IF, ELSE et boucles WHILE
    else if (!dans_accolades)
    {
        bool else_if = false;
        while (strcmp(m->argument, "{") != 0)
        {
            if (strcmp(m->argument, "!=") == 0)
            {
                fprintf(fichierML,"<>");
            }
            else if (strcmp(m->argument, "==") == 0)
            {
                fprintf(fichierML,"=");
            }
            else if (strcmp(m->argument, "!") == 0)
            {
                if (test_maillon_suivant(m->suivant, "("))
                {
                    fprintf(fichierML,"not");
                    return creer_assignement(m->suivant, 1, end);
                }
                else
                {
                    if (m->suivant->lexeme == 'V')
                    {
                        fprintf(fichierML,"not(!%s)", m->suivant->argument);
                    }
                    else
                    {
                        fprintf(fichierML,"not(%s)", m->suivant->argument);
                    }
                    m = m->suivant; // car on a print 2 maillons
                }
            }
            else if (strcmp(m->argument, "if") == 0)
            {
                fprintf(fichierML,"%s", m->argument);
                else_if = true;
            }

            else if (m->lexeme == 'V')
            {
                fprintf(fichierML,"(!%s)", m->argument);
            }
            else
            {
                fprintf(fichierML,"%s", m->argument);
            }
            m = m->suivant;
        }
        if (type_condition == 0 || else_if)
        {
            fprintf(fichierML," then begin \n");
            m = parcours_conditionnelle(m, type_condition, true, end);
            if (is_next_else(m)){
                fprintf(fichierML," end\n");
            }
            else {
                fprintf(fichierML," end%s\n",end);
            }
            return m;
        }
        else if (type_condition == 1)
        {
            fprintf(fichierML," begin \n");
            m = parcours_conditionnelle(m, type_condition, true, end);
            if (is_next_else(m)){
                fprintf(fichierML," end\n");
            }
            else {
                fprintf(fichierML," end%s\n",end);
            }
            return m;
        }
        else if (type_condition == 2)
        {
            fprintf(fichierML," do \n");
            m = parcours_conditionnelle(m, type_condition, true, end);
            fprintf(fichierML," done%s\n", end);
            return m;
        }
    }
    //traduit l'interieur des accolades
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
                return parcours_conditionnelle(parcours_conditionnelle(m, 0, false, ";"), type_condition, dans_accolades, end);
            }
            else if (strcmp(m->argument, "else") == 0)
            {
                return parcours_conditionnelle(parcours_conditionnelle(m, 1, false, ";"), type_condition, dans_accolades, end);
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
    /*Fonction qui traduit l'interieur des accolades d'une fonction*/
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
            return parcours_fonction(parcours_conditionnelle(m, 0, false, ";"));
        }
        else if (strcmp(m->argument, "else") == 0)
        {
            return parcours_fonction(parcours_conditionnelle(m, 1, false, ";"));
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
    /*Fonction qui traduit la declaration d'une fonction*/
    if (m == NULL)
    {
        return NULL;
    }
    bool is_main = false;
    bool avant_nom_fct = true;
    while (strcmp(m->argument, "{") != 0)
    {
        if (strcmp(m->argument, "main") == 0)
        {
            is_main = true;
        }
        else if (m->lexeme == 'V' && !is_main)
        {
            if (avant_nom_fct)
            {
                fprintf(fichierML,"let %s", m->argument);
                avant_nom_fct = false;
            }
            else
            {
                fprintf(fichierML,"%s", m->argument);
            }
        }
        else if (m->lexeme != 'T' && !is_main && strcmp(m->argument, " ") != 0)
        {
            fprintf(fichierML,"%s", m->argument);
        }

        m = m->suivant;
    }
    if (is_main)
    {
        return m;
    }
    else
        fprintf(fichierML," = \n");
    m = parcours_fonction(m);
    fprintf(fichierML,";;\n");
    return m;
}

void parcours(maillon *m)
{
    /*Fonction qui parcours la liste chainee afin de tout traduire*/
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
            return parcours(parcours_conditionnelle(m, 0, false, ";;"));
        }
        else if (strcmp(m->argument, "else") == 0)
        {
            return parcours(parcours_conditionnelle(m, 1, false, ";;"));
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
    if(argc>1){
        //Creer le nom de fichier en .ml
        int len  = strlen(argv[1]);
        char fichierml[len + 1];
        for(int i=0; i<len -1; i+=1){
            fichierml[i]  = argv[1][i];
        }
        fichierml[len-1] =  'm';
        fichierml[len] =  'l';
        fichierml[len+1] =  '\0';


        fichierML = fopen(/*fichier traduit*/fichierml, "w");
        FILE *fichierC = fopen(/*fichier_a_traduire */ argv[1], "r");
        maillon *liste = lexeur(fichierC);
        imprim(liste);
        parcours(liste);
    }
    
}
