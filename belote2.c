#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBR_CARTES 32
#define NBR_JOUEURS 4

typedef struct carte {
    int valeur;
    char couleur;
} Carte;

typedef struct pile {
    Carte cartes[NBR_CARTES];
    int sommet;
} Pile;
void initialiser_pile(Pile *pile) {
    pile->sommet = -1;
}

void empiler(Pile *pile, Carte carte) {
    pile->sommet++;
    pile->cartes[pile->sommet] = carte;
}

Carte depiler(Pile *pile) {
    Carte carte = pile->cartes[pile->sommet];
    pile->sommet--;
    return carte;
}

void melanger_cartes(Pile *pile) {
    srand(time(NULL));
    for (int i = 0; i < NBR_CARTES; i++) {
        int j = rand() % NBR_CARTES;
        Carte temp = pile->cartes[i];
        pile->cartes[i] =pile->cartes[j];
        pile->cartes[j] = temp;
    }
}
void afficher_pile(Pile pile) {
    for (int i = 0; i <= pile.sommet; i++) {
        printf("%d%c ", pile.cartes[i].valeur, pile.cartes[i].couleur);
    }
    printf("\n");
}
void partager_cartes(Pile *pile, Pile *joueurs[NBR_JOUEURS], int nb_cartes) {
    for (int i = 0; i < nb_cartes; i++) {
        for (int j = 0; j < NBR_JOUEURS; j++) {
            Carte carte = depiler(pile);
            empiler(joueurs[j], carte);
        }
    }
}
int main() {
    Pile pile;
    Pile *joueurs[NBR_JOUEURS];

    for (int i = 0; i < NBR_JOUEURS; i++) {
        joueurs[i] = (Pile*) malloc(sizeof(Pile));
        initialiser_pile(joueurs[i]);
    }
initialiser_pile(&pile);

    // Initialisation de la pile
    for (int i = 0; i < 8; i++) {
        for (int j = 1; j <= 4; j++) {
            Carte carte;
            carte.valeur = i+1;
            switch (j) {
                case 1:
                    carte.couleur = 'C';
                    break;
                case 2:
                    carte.couleur = 'P';
                    break;
                case 3:
                    carte.couleur = 'C';
                    break;
                case 4:
                    carte.couleur = 'T';
                    break;
            }
            empiler(&pile, carte);
        }
    }

    printf("Cartes avant melange : ");
    afficher_pile(pile);

    melanger_cartes(&pile);
     printf("Cartes apres melange : ");
    afficher_pile(pile);

    // Partage des cartes 3 à 3
    partager_cartes(&pile, joueurs, 3);

    printf("Cartes apres partage 3 a 3 : \n");
    for (int i = 0; i < NBR_JOUEURS; i++) {
        printf("Joueur %d : ", i+1);
        afficher_pile(*joueurs[i]);
    }
    
    // Affichage des cartes de chaque joueur après le partage 3 à 3
    for (int i = 0; i < NBR_JOUEURS; i++) {
        printf("Joueur %d : ", i+1);
        afficher_pile(*joueurs[i]);
    }
    
    // Partage des cartes 2 à 2
    for (int i = 0; i < NBR_JOUEURS; i++) {
        initialiser_pile(joueurs[i]);
    }
    partager_cartes(&pile, joueurs, 2);
    
    printf("Cartes apres partage 2 a 2 : \n");
    for (int i = 0; i < NBR_JOUEURS; i++) {
        printf("Joueur %d : ", i+1);
        afficher_pile(*joueurs[i]);
    }
    
    // Libération de la mémoire allouée pour les piles des joueurs
    for (int i = 0; i < NBR_JOUEURS; i++) {
        free(joueurs[i]);
    }

    return 0;
}


