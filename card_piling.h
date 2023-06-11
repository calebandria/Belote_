#ifndef CARD_PILING_H
#define CARD_PILING_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define CARD_NUMBER 32
#define PLAYER_NUMBER 4
#define CARDS_PLAYER 8


typedef struct Pcarte Pcarte;
typedef struct Pile Pile;
typedef struct Scarte Scarte;
typedef struct Player Player;


/* Structure de données pour l'entassement */

/* Carte non appartenant à une pile */
struct Scarte{
    int valeur;
    char couleur;
};
/* Carte appartenant à une pile*/
struct Pcarte{
    int valeur;
    char couleur;
    Pcarte *suivant;
};

struct Pile{
    Pcarte *premier;
};

struct Player{
    Scarte *cartes;
    bool main;
    int jeuChoisi;
};

void empiler(Pile *pile, Scarte *newCard);
void afficheCarte(Scarte carte);
void afficherPile(Pile *pile);
void afficheCartes(Scarte *carte);
void initializePile(Pile *pile);
Scarte depiler(Pile *pile);
Scarte* playableCards();
void shuffleCard(Scarte* cardPlayable);
void shareCards(Pile* pile,Player* players, int indexPlayerHand);
void displayCardForEachPlayer(Player player);
void displayCardForAllPlayers(Player *players);
int getIndexPlayerhand(Player *players);

#endif







