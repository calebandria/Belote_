#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CARD 32

struct carte {
    char* valeur;
    char* couleur;
};

struct pile {
    struct carte carte;
    struct pile* suivant;
};
void initialiser_cartes(struct carte* jeu) {
    int i, j, k = 0;
    char* valeurs[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    char* couleurs[] = { "Pique","Carreau","Coeur","Trefle" };

    for (i = 0; i < 13; i++) {
        for (j = 0; j < 4; j++) {
            jeu[k].valeur = valeurs[i];
            jeu[k].couleur = couleurs[j];
            k++;
        }
    }
}
struct pile* ajouter_carte(struct carte* carte, struct pile* sommet) {
    struct pile* nouveau_sommet = (struct pile*)malloc(sizeof(struct pile));
    nouveau_sommet->carte = *carte;
    nouveau_sommet->suivant = sommet;
    return nouveau_sommet;
}

void melanger_cartes(struct pile** sommet) {
    int i, j, k;
    struct carte tmp;
    srand(time(NULL));

    for (i = 0; i < CARD; i++) {
        j = rand() % CARD;
        k = rand() % CARD;
        struct pile* ptr_j = *sommet;
        struct pile* ptr_k = *sommet;
        for (int l = 0; l < j; l++) {
            ptr_j = ptr_j->suivant;
        }
        for (int l = 0; l < k; l++) {
            ptr_k = ptr_k->suivant;
        }
        tmp = ptr_j->carte;
        ptr_j->carte = ptr_k->carte;
        ptr_k->carte = tmp;
    }
}

int main() {
    struct carte jeu[CARD];
    struct pile* sommet = NULL;
    int i;

    initialiser_cartes(jeu);
    printf("Cartes avant melange :\n");

    for (i = 0; i < CARD; i++) {
        sommet = ajouter_carte(&jeu[i], sommet);
        printf("%s %s\n", sommet->carte.valeur, sommet->carte.couleur);
    }

    melanger_cartes(&sommet);
    printf("\nCartes apres melange :\n");

    for (i = 0; i < CARD; i++) {
        printf("%s %s\n", sommet->carte.valeur, sommet->carte.couleur);
        sommet = sommet->suivant;
    }

    return 0;
}


