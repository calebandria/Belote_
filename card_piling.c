#include "card_piling.h"

void empiler(Pile *pile, Scarte *newCard ){
    Pcarte *new = malloc(sizeof(*new));

    if(pile== NULL || new== NULL){
        exit(EXIT_FAILURE);
    }
    new->valeur = newCard->valeur;
    new->couleur = newCard->couleur;
    new->suivant = pile->premier;
    pile->premier = new;
}

void afficheCarte(Scarte carte){
    printf("Valeur: %d\nCouleur: %c\n", carte.valeur, (int)carte.couleur);
}

void afficherPile(Pile *pile){
    Pcarte *actuel= pile->premier;

    if(pile == NULL){
        printf("Erreur, pile vide");
    }

    while(actuel!=NULL){
        printf("%d %c\n",actuel->valeur, actuel->couleur);
        actuel = actuel->suivant;
    }
}
void afficheCartes(Scarte *carte){
    int i =0;
    for(i=0;i<CARD_NUMBER;i++){
        afficheCarte(carte[i]);
    }
}

void initializePile(Pile *pile){
    pile->premier = NULL;
}

Scarte depiler(Pile *pile){
    if(pile == NULL){
        exit(EXIT_FAILURE);
    }

    Scarte *carteDeLaPile;
    carteDeLaPile = malloc(sizeof(Scarte));
    Pcarte *carteDeLaPileADepiler = pile->premier;

    if(pile!=NULL && pile->premier!=NULL){
        carteDeLaPile->valeur = carteDeLaPileADepiler->valeur;
        carteDeLaPile->couleur = carteDeLaPileADepiler->couleur;
        pile->premier = carteDeLaPileADepiler->suivant;

        free(carteDeLaPileADepiler);
    }

    return *carteDeLaPile;
}
// Construction des cartes à jouer;
Scarte* playableCards(){
    Scarte* playable = malloc(CARD_NUMBER*sizeof(Scarte));
    int i = 0;
    for(i=0;i<8;i++){
        playable[i].valeur = i+1;
        playable[i].couleur = 'T';
    }

    for(i=8;i<16;i++){
        playable[i].valeur = i-7;
        playable[i].couleur = 'K';
    }

    for(i=16;i<24;i++){
        playable[i].valeur = i-15;
        playable[i].couleur = 'C';
    }

    for(i=24;i<32;i++){
        playable[i].valeur = i-23;
        playable[i].couleur = 'P';
    }

    return playable;
}

// Shuffling cards
void shuffleCard(Scarte* cardPlayable){
    srand(time(NULL));

    for(int i=CARD_NUMBER-1; i > 0; i--){
        int j = rand()%(i+1);

        Scarte temp = cardPlayable[i];
        cardPlayable[i] = cardPlayable[j];
        cardPlayable[j] = temp;
    }
}

// Sharing the pile between the four players

void shareCards(Pile* pile,Player* players, int indexPlayerHand){
    int playerLeftWithoutCards=4;

    for(int i=indexPlayerHand; i< PLAYER_NUMBER;i++){
        players[i].cartes = malloc(CARDS_PLAYER*sizeof(Scarte));
        for(int j=0; j<CARDS_PLAYER; j++){
            players[i].cartes[j] = depiler(pile);
        }
        playerLeftWithoutCards--;
    }

    for(int i=0; i<playerLeftWithoutCards; i++){
        players[i].cartes = malloc(CARDS_PLAYER*sizeof(Scarte));
        for(int j=0; j<CARDS_PLAYER;j++){
            players[i].cartes[j] = depiler(pile);
        }
    }

}

void displayCardForEachPlayer(Player player){
    for(int i=0; i<CARDS_PLAYER; i++){
        printf("%d %c\n", player.cartes[i].valeur,player.cartes[i].couleur);
    }
}

void displayCardForAllPlayers(Player *players){
    for(int i=0; i<PLAYER_NUMBER; i++){
        printf("Player %d: \n",i);
        displayCardForEachPlayer(players[i]);
    }
}

int getIndexPlayerhand(Player *players){
    int handPlayerIndex = 0;
    for(int i=0; i<PLAYER_NUMBER; i++){
        if(players[i].main==true){
            handPlayerIndex=i;
            break;
        }
            
    }
    return handPlayerIndex;
}

