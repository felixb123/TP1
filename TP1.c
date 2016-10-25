/*
* @file
* Travail pratique 1
* @author Felix Blier
* @date  07 octobre 2016

Ce programme est un jeu dans lequel un on déplace un robot en essayant de trouver une bombe. Le but du jeu est de trouver la bombe en faisant le moins de déplacement possible.
*/

//Appel des librairies nécessaires à l'exécution du programme
#include <stdio.h>

//Appel des librairies nécessaires le bon fonctionnement du générateur aléatoire <rand()>
#include <stdlib.h>
#include <time.h>

//Appel de la librairie stdtool
#if !defined(_MSC_VER) || _MSC_VER != 1600
#include <stdbool.h>
#endif

//Appel de la librairie curse
#if defined(_MSC_VER) || defined(__MINGW32__)
#include "curses.h"
#else
#include <curses.h>
#endif

// Définition des dimensions de la fenêtre
#ifdef INF_MAIN
#define COLS 80
#define LINES 25 m
#else

//Définition des constantes
#define MINCOLONNES 0
#define MINLIGNES 0
#define MAXCOLONNES 79
#define MAXLIGNES 23
#define S_OUEST 1
#define SUD 2
#define S_EST 3
#define OUEST 4
#define EST 6
#define N_OUEST 7
#define NORD 8
#define N_EST 9
#define ERREUR -1
#define ABANDON 5
#define QUITTER 0
#define LIGNEMESSAGE 24
#define DELAI 500

//Déclaration des fonctions utilisées dans le programme
void afficher_intro_direction();
void attendre();
int saisir_entier_valide(const char * ,int,int);
void afficher_robot(int,int);
unsigned int alea(unsigned int,unsigned int);
bool est_robot_sur_bombe(int,int,int,int);
int distance_bombe(int,int,int,int);
int prochaine_direction(int,int,int,int);
int bouger_colonne(int,int);
int bouger_ligne(int,int);
int saisir_direction();
void afficher_bombe(int,int);
void afficher_abandon(int,int);
void effacer_robot(int,int);
void afficher_trouve_bombe(int,int);
void afficher_stats(int,int,int,int);
void afficher_fin();


//Programme principal
int main (void) {

srand((unsigned int)time(NULL)); //Permet le fonctionnement du générateur aléatoire <rand()>

initscr(); //Permet l'utilisation de ncurse
start_color(); //Permet l'utilisation des couleures de la librairie curse

//Déclaration des variables
bool trouve=FALSE,quitter=FALSE,abandon=FALSE;
int position_ligne=0,position_colonne=0,bombe_ligne=0,bombe_colonne=0,distance_initiale=0,nb_mouvements=0,direction=0,nb_jeux=0,nb_mouvements_totals=0,nb_mouvements_min=0,nb_mouvements_max=0;


afficher_intro_direction();
attendre();

while (quitter==FALSE) //Tant que l'utilisateur ne choisit pas de quitter le jeu le jeu s'exécute
{
    position_ligne=saisir_entier_valide("ligne",MINLIGNES,MAXLIGNES); //Permet d'entrer la position en ligne du robot
    position_colonne=saisir_entier_valide("colonne",MINCOLONNES,MAXCOLONNES);//Permet d'entrer la position en colonne du robot

// Si l'utilisateur entre 0 en ligne et 0 en colonne, le jeu se ferme.
    if (position_ligne==0 && position_colonne==0)
    {
        quitter=TRUE;
    }

    else
    {
        afficher_robot(position_ligne,position_colonne);

        alea(0,0); //Permet de bien faire fonctionner le générateur aléatoire <rand()

        while (est_robot_sur_bombe(position_ligne,position_colonne,bombe_ligne,bombe_colonne)); /*Permet de régénérer des positions de la bombe si cette dernière est sut
                                                                                                la position du robot au départ*/
        {
            bombe_ligne=alea(MINLIGNES,MAXLIGNES); //Génère la position aléatoire de la bombe dans les lignes du tableau
            bombe_colonne=alea(MINCOLONNES,MAXCOLONNES);//Génère la position aléatoire de la bombe dans les colonnes du tableau
            distance_initiale = distance_bombe(position_ligne,position_colonne,bombe_ligne,bombe_colonne); //Permet de connaitre le nombre de déplacements minimum pour arriver à la bombe
        }

        //Au départ, l'utilisateur n'a pas trouvé la bombe et n'a pas abandonné.
        trouve=FALSE;
        abandon=FALSE;

        nb_mouvements=0;

        while (quitter==FALSE && abandon==FALSE && trouve==FALSE) //La partie dure tant que l'utilisateur n'a pas quitté, n'a pas abandonné et n'a pas trouvé la bombe.
        {
            direction=saisir_direction(); //Permet d'associer la direction voulue du robot par l'utilisateur à la variable de direction.
            if (direction==ABANDON) //Si l'utilisateur a choisi la touche pour abandonner soit 5, la partie est abandonnée.
            {
                afficher_abandon(bombe_ligne,bombe_colonne);
                distance_bombe(position_ligne,position_colonne,bombe_ligne,bombe_colonne);
                abandon=TRUE;
                attendre();
            }
            else if (direction==QUITTER) //Si l'utilisateur a choisi la touche pour quitté soit 5, le jeu est quitté.
            {
                quitter=TRUE;
            }
            else if((direction>QUITTER && direction<ABANDON) || (direction>ABANDON && direction<=N_EST)) //Si l'utilisateur a choisi une touche correspondant à un déplacement, ce déplacement est effectué s’il est possible.
            {
                nb_mouvements++; //Un mouvement de plus est effectué
                effacer_robot(position_ligne,position_colonne);
                position_colonne=bouger_colonne(position_colonne,direction); //La position en colonne est changée si le déplacement la fait changer.
                position_ligne=bouger_ligne(position_ligne,direction); //La position en ligne est changée si le déplacement la fait changer.
                afficher_robot(position_ligne,position_colonne);
            }

            if (est_robot_sur_bombe(position_ligne,position_colonne,bombe_ligne,bombe_colonne)) //Si le robot est sur la bombe alors l'utilisateur a trouvé la bombe.
            {
                trouve=TRUE;
                afficher_trouve_bombe(nb_mouvements,distance_initiale);
            }
            else //Sinon, la distance entre le robot et la bombe est recalculée.
            {
                distance_bombe(position_ligne,position_colonne,bombe_ligne,bombe_colonne);
            }
        }

        if (trouve==TRUE) //Si l'utilisateur à trouvé la bombe, les statistiques de la partie sont calculées
        {

            if (nb_jeux==0) //Si c'est le premier tour joué, le nombre de mouvements total est à zéro et le nombre de mouvements minimal et maximal est égale au nombre de mouvements de ce tour.
            {
                nb_mouvements_min=nb_mouvements;
                nb_mouvements_max=nb_mouvements;
            }

            else if(nb_mouvements>nb_mouvements_max)    /*Si le nombre de mouvements de ce tour est plus grand que le nombre de mouvements d'un tour précédent,
                                                        le nombre de mouvements de ce tour est le plus grand.*/
            {
                nb_mouvements_max=nb_mouvements;
            }

            else if (nb_mouvements<nb_mouvements_min)   /*Si le nombre de mouvements de ce tour est plus petit que le nombre de mouvements d'un tour précédent,
                                                    le nombre de mouvements de ce tour est le plus petit.*/
            {
                nb_mouvements_min=nb_mouvements;
            }

        nb_jeux++; //Le joueur viens de faire un tour de plus
        nb_mouvements_totals=nb_mouvements_totals+nb_mouvements; //Le nombre de mouvements de ce tour est ajouté au nombre de mouvements total.
        }
    }
}
afficher_stats(nb_jeux,nb_mouvements_totals,nb_mouvements_min,nb_mouvements_max);
afficher_fin();
endwin(); //Met fin à l'affichage avec la librarie curse.
return 0;
}
#endif


//Définition des fonctions

//Cette fonction décrit les commandes du jeux à l'utilisateur
void afficher_intro_direction() {
move(2,0);
printw("Voici comment diriger le robot, abandonner le tour ou quitter le jeux :");
move(4,0);
printw("2=Sud");
move(5,0);
printw("4=Ouest");
move(6,0);
printw("6=Est");
move(7,0);
printw("8=Nord");
move(8,0);
printw("1=Sud-ouest");
move(9,0);
printw("3=Sud-Est");
move(10,0);
printw("7=Nord-Ouest");
move(11,0);
printw("7=Nord-Est");
move(12,0);
printw("5=Abandonner le tour");
move(13,0);
printw("0=Quitter le jeux");
}

//Cette fonction attend et demande que l'utilisateur appui sur une touche ce qui lui laisse le temps de lire
void attendre() {
move(LIGNEMESSAGE,0);
printw("Appyez sur une touche pour continuer.");
getch();
}

//Cette fonction reçoit l'orientation de la position, la position minimale et la position maximale envoyées et demande à l'utilisateur d'entrée une position entre les limites pour la renvoyer.
int saisir_entier_valide (const char * msg,int min,int max){

int position;

// Permet d'afficher l'instruction comment quitter le programme à cette étape et une seule fois, soit lors de la déclaration de la position des y
if (msg=="ligne")
{
    clear();
    refresh();
    move(1,0);
    printw("Pour quitter le jeu, entrez la position (0,0).\n\n");
}

//Si la position entrée est en dehors des limites d'affichage, le programme va demander d'entrer une autre position.
//L'utilisation du do permet de faire la boucle au moins une fois.
do
{
    printw("Entrer la position en %s du robot suivit de la touche <entrer>: ", msg);
    scanw("%d", &position);


    if(position<min || position>max)
    {
        printw("Erreur la position en %s doit etre entre %d et %d. Recommencez",msg, min, max);
    }

    printw("\n\n");
} while(position<min || position>max);

return position;
}

//Cette fonction affiche le robot à la position de la ligne et de la colonne indiquée par l'utilisateur
void afficher_robot(int position_ligne,int position_colonne) {

//Affiche le robot
clear();
refresh();
move(position_ligne,position_colonne);
printw("+");

//Affiche les coordonnées du robot
move(LIGNEMESSAGE,0);
printw("Position en x:%d ", position_colonne);
move(LIGNEMESSAGE,18);
printw("Position en y:%d ", position_ligne);
}

//Cette fonction génère et retourne un nombre compris entre un maximum et un minimum envoyé
unsigned int alea(unsigned int min,unsigned int max) {

unsigned int r;
const unsigned int range = 1 + max - min;
const unsigned int buckets = RAND_MAX / range;
const unsigned int limit = buckets * range;

do
{
    r = (unsigned int)rand();
}
while (r >= limit);


return min + (r/buckets);
}

//Cette fonction détermine si le robot est sur la bombe en recevant leurs positions. Elle retourne la réponse en format booléen.
bool est_robot_sur_bombe(int position_ligne,int position_colonne,int bombe_ligne,int bombe_colonne) {

if (position_ligne==bombe_ligne && position_colonne==bombe_colonne) //Si le la bombe est sur la même ligne et la même colonne que le robot, elle est à la même place que le robot.
{
    return TRUE;
}
else
{
    return FALSE;
}
}

//Cette fonction calcul le nombre de déplacements minimum à faire pour arriver à la bombe.
int distance_bombe(int position_ligne,int position_colonne,int bombe_ligne,int bombe_colonne) {

int distance=0, direction; //Initialise le compte du nombre de d.placements à zéro et déclare la variable de la direction à prendre.

//Tant que la position du robot n'est pas sur la position de la bombe, la direction à prendre est évaluée, un robot fictif est déplacé selon la direction à prendre
// et le nombre de déplacements est augmenté de 1.
while (est_robot_sur_bombe(position_ligne,position_colonne,bombe_ligne,bombe_colonne)==0)
{
    direction=prochaine_direction(position_ligne,position_colonne,bombe_ligne,bombe_colonne);
    position_colonne=bouger_colonne(position_colonne,direction);
    position_ligne=bouger_ligne(position_ligne,direction);
    distance=distance+1;
}

//Le nombre de déplacements à faire est affiché.
move(LIGNEMESSAGE,40);
printw("Distance entre le robot et la bombe:%d", distance);

return distance;
}

//Cette fonction permet au robot de déterminer la direction à prendre pour atteindre la bombe
int prochaine_direction(int position_ligne,int position_colonne,int bombe_ligne,int bombe_colonne) {

if  (position_colonne==bombe_colonne) //Si le robot est sur la même colonne que la ligne
{
    if (position_ligne<bombe_ligne) //Si le robot est sous la bombe
    {
        return SUD; //Le rebot doit aller vers le sud.
    }
    else if (position_ligne>bombe_ligne) //Si le robot est au-dessus de la bombe
    {
        return OUEST; //Le robot doit aller vest l'ouest.
    }
    else
    {
        return ERREUR; //Sinon, il y a alors une erreur.
    }

}
else
{
    if (position_colonne<bombe_colonne) //Si le robot est à gauche de la bombe
    {
        if (position_ligne==bombe_ligne) //Si le robot est sur la même ligne que la bombe
        {
            return EST; //Le robot soit aller vers l'est.
        }
        else if (position_ligne<bombe_ligne) //Si le robot est en dessous de la bombe
        {
            return S_EST; //Le robot doit aller vers le sud-est.
        }
        else
        {
            return N_EST; //Sinon, le robot doit aller vers le nord-est.
        }
    }
    else
    {
        if (position_ligne==bombe_ligne) //Si le robot est sur la même ligne que la bombe
        {
            return OUEST; //Le robot doit aller vers l'ouest.
        }
        else if (position_ligne<bombe_ligne) //Si le robot est en dessous de la bombe
        {
            return S_OUEST; //Le robot doit aller vers le sud-ouest.
        }
        else
        {
            return N_OUEST; //Sinon, le robot doit aller vers le nord-ouest.
        }
    }

}
}

//Cette fonction sert à faire bouger le robot entre les colonnes selon la direction voulue et à r'envoyer la nouvelle position en colonnes.
int bouger_colonne(int position_colonne,int direction) {

//Si la direction du déplacement est vers le sud-ouest, l'ouest ou le nord-ouest et que le robot est à droite de la limite maximale de gauche, le robot se déplace vers la gauche.
if (direction==S_OUEST || direction==OUEST || direction==N_OUEST)
{
    if (position_colonne>MINCOLONNES)
    {
        position_colonne=position_colonne-1;
    }
}
//Si la direction du déplacement est vers le sud-est, l'est ou le nord-est et que le robot est à gauche de la limite maximale de droite, le robot se déplace vers la droite.
else if (direction==S_EST || direction==EST || direction==N_EST)
{
    if (position_colonne<MAXCOLONNES)
    {
        position_colonne=position_colonne+1;
    }
}
//Si la touche appuyée n'est pas une direction valide, renvoyer une valeur différente que celles des contrôles du jeu.
if (direction<QUITTER || direction>N_EST)
{
    return ERREUR;
}
else
{
return position_colonne;
}
}

//Cette fonction sert à faire bouger le robot entre les lignes selon la direction voulue et à r'envoyer la nouvelle position en lignes.
int bouger_ligne(int position_ligne,int direction) {

//Si la direction du déplacement est vers le nord-ouest, le nord ou le nord-est et que le robot est en bas de la limite maximale du haut, le robot se déplace vers le haut.
if (direction==N_OUEST || direction==NORD || direction==N_EST)
{
    if (position_ligne>MINLIGNES)
    {
        position_ligne=position_ligne-1;
    }
}
//Si la direction du déplacement est vers le sud-ouest, le sud ou le sud-est et que le robot est en haut de la limite maximale du bas, le robot se déplace vers le bas.
else if (direction==S_OUEST || direction==SUD || direction==S_EST)
{
    if (position_ligne<MAXLIGNES)
    {
        position_ligne=position_ligne+1;
    }
}
//Si la touche appuyé n'est pas une direction valide, renvoyer une valeur différente que celles des contrôles du jeu.
if (direction<QUITTER || direction>N_EST)
{
   return ERREUR;
}
else
{
    return position_ligne;
}

}

//Cette fonction sert à saisir et renvoyer la direction demandée par l'utilisateur.
int saisir_direction() {

int saisie;

    noecho(); //Désactivation de l'affichage de la touche entrée
    saisie=getch()-48; //Association de la touche appuyée à la variable de saisie en soustrayant la touche appuyée de 48 qui est la valeur avec laquelle les chiffes du code ASCII commencent.
    echo(); //Réactivation de l'affichage de la touche entrée

    return saisie;
}

//Cette fonction sert à afficher un message lorsque l'utilisateur abandonne la partie.
void afficher_abandon(int bombe_ligne,int bombe_colonne) {

int i;

clear();
refresh();

//Avec cette boucle la bombe clignotera 3 fois
for (i=1;i<=3;i++)
{
    move(LIGNEMESSAGE,0);
    printw("La bombe etait la");

    afficher_bombe(bombe_ligne,bombe_colonne);
}
}

//Cette fonction sert à afficher la bombe.
void afficher_bombe(int bombe_ligne,int bombe_colonne) {
move(bombe_ligne,bombe_colonne);
clrtoeol(); //Efface la bombe
refresh();
napms(DELAI);

init_pair(1, COLOR_RED, COLOR_BLACK); //Définit la couleure
attron(COLOR_PAIR(1)); //Colore la bombe
printw("O");
refresh();
attroff(COLOR_PAIR(1));//Annule la couleure
napms(DELAI);
}

//Cette fonction sert à effacer le robot
void effacer_robot(int position_ligne,int position_colonne) {
move(position_ligne,position_colonne);
clrtoeol();
}

//Cette fonction sert à afficher un message lorsque la bombe est trouvée avec le nombre de déplacements faits et minimaux au départ.
void afficher_trouve_bombe(int nb_mouvements,int distance_initiale) {
clear();
refresh();
move(2,0);
printw("Vous avez trouve la bombe en %d deplacements!", nb_mouvements);
move(4,0);
printw("Le nombre de mouvements minimaux de depart etait de %d deplacements", distance_initiale);

attendre();
}

//Cette fonction sert à afficher les statistiques sur le nombre de tours jouées sans abandonner, le plus petit nombre, le plus grand nombre et la moyenne de déplacement par partie.
void afficher_stats(int nb_jeux,int nb_mouvements_total,int nb_mouvements_min,int nb_mouvements_max) {
clear();
refresh();

if (nb_jeux>0) //La moyenne du nombre de tours est faite si l'utilisateur à fait plus d'un tour ce qui évite de diviser l'équation par un nombre de tours égales à zéro entrainant une erreure.
{
    nb_mouvements_total=nb_mouvements_total/nb_jeux;
}

move(2,0);
printw("Le nombre de tours non abandones joues est %d tours", nb_jeux);
move(4,0);
printw("Le nombre de coups joues le plus petit est %d coups", nb_mouvements_min);
move(6,0);
printw("Le nombre de coups joues le plus grand est %d coups", nb_mouvements_max);
move(8,0);
printw("Le nombre de coups joues en moyenne est %d coups", nb_mouvements_total);

attendre();
}

//Cette fonction affiche un message à l'utilisateur avant que se dernier se ferme.
void afficher_fin(){
clear();
refresh();

move(2,0);
printw("Merci d'avoir joue et a la prochaine!");
attendre();
}
