#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void toEOL(FILE* file) {
    int ch;
    do{
        ch=fgetc(file);
    } while (ch!='\n' && ch!=EOF);
}

void newgen(int nl, int nc, char current[nl][nc], char next[nl][nc]) {
    int i, j, vecini;
    for (i=0; i<nl; i++) {
        for (j=0; j<nc; j++) {
            vecini=0;
            if (i>0) vecini+=(current[i-1][j]=='X');
            if (j>0) vecini+=(current[i][j-1]=='X');
            if (i<nl-1) vecini+=(current[i+1][j]=='X');
            if (j<nc-1) vecini+=(current[i][j+1]=='X');
            if (i>0 && j>0) vecini+=(current[i-1][j-1]=='X');
            if (i>0 && j<nc-1) vecini+=(current[i-1][j+1]=='X');
            if (i<nl-1 && j>0) vecini+=(current[i+1][j-1]=='X');
            if (i<nl-1 && j<nc-1) vecini+=(current[i+1][j+1]=='X');

            if (current[i][j]=='X') {
                next[i][j]=(vecini<2 || vecini>3) ? '+' : 'X';
            }
            else {
                next[i][j]=(vecini==3) ? 'X' : '+';
            }
        }
    }
}

void newgenB(int nl, int nc, char current[nl][nc], char next[nl][nc]) {
    // Mai întâi copiem matricea current în next
    memcpy(next, current, sizeof(char) * nl * nc);
    
    int i, j, vecini;
    for (i = 0; i < nl; i++) {
        for (j = 0; j < nc; j++) {
            vecini = 0;
            if (i > 0) vecini += (current[i - 1][j] == 'X');
            if (j > 0) vecini += (current[i][j - 1] == 'X');
            if (i < nl-1) vecini += (current[i + 1][j] == 'X');
            if (j < nc-1) vecini += (current[i][j + 1] == 'X');
            if (i > 0 && j > 0) vecini += (current[i - 1][j - 1] == 'X');
            if (i > 0 && j < nc-1) vecini += (current[i - 1][j + 1] == 'X');
            if (i < nl-1 && j>0) vecini += (current[i + 1][j - 1] == 'X');
            if (i < nl-1 && j < nc-1) vecini += (current[i + 1][j + 1] == 'X');

            // Modificăm doar dacă avem exact 2 vecini
            if (vecini == 2) {
                next[i][j] = 'X';
            } else if (current[i][j] != 'X') {
                next[i][j] = '+';
            }
        }
    }
}

void print_grid(int nl,int nc,const char grid[nl][nc], FILE* file) {
    for (int i=0; i<nl;i++) {
        for (int j=0;j<nc;j++) {
            fprintf(file,"%c", grid[i][j]);
        }
        fprintf(file,"\n");
    }
    fprintf(file,"\n");
}

struct Element {
    int l, c;
    struct Element* next;
};

typedef struct Element node;

struct Stiva {
    node* head;
    struct Stiva* next;
};

typedef struct Stiva gen;

void push(gen** top, node** v) {
    gen* newGen = (gen*)malloc(sizeof(gen));
    newGen->head = *v;
    newGen->next = *top;
    *top = newGen;
}



void addAtEnd(node** head, int li, int ci) {
    node* newNode = (node*)malloc(sizeof(node));
    newNode->l = li;
    newNode->c = ci;
    newNode->next = NULL;  // Inițializăm next cu NULL

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    node* aux = *head;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    aux->next = newNode;
}

struct gens {
    char grid[500][500];
    struct gens* left, * right;
};

typedef struct gens Arbore;

void citire(FILE* file, int nl, int nc, char current[nl][nc]) {
    for (int i = 0; i < nl; i++) {
        for (int j = 0; j < nc; j++) {
            int ch = fgetc(file);
            if (ch == '\n' || ch == EOF) {
                break;
            }
            current[i][j] = (char)ch;
        }
        if (fgetc(file) != '\n') toEOL(file);
    }
}

void generare(Arbore* root, int nl, int nc, int ng) {
    if (ng != 0) {
        char gridB[nl][nc], gridS[nl][nc];

        newgen(nl, nc, root->grid, gridS);
        newgenB(nl, nc, root->grid, gridB);

        root->right = malloc(sizeof(Arbore));
        root->left = malloc(sizeof(Arbore));
        memcpy(root->right->grid, gridS, sizeof(char) * nl * nc);
        memcpy(root->left->grid, gridB, sizeof(char) * nl * nc);

        generare(root->left, nl, nc, ng - 1);
        generare(root->right, nl, nc, ng - 1);
    }
}

void preordine(Arbore* root, int nl, int nc, FILE* fout) {
    if (root == NULL) return;

    print_grid(nl, nc, root->grid, fout);
    preordine(root->left, nl, nc, fout);
    preordine(root->right, nl, nc, fout);
}

void freeList(node* head) {
    while (head) {
        node* temp = head;
        head = head->next;
        free(temp);
    }
}

void freeStack(gen* top) {
    while (top) {
        gen* temp = top;
        top = top->next;
        freeList(temp->head);
        free(temp);
    }
}

void freeTree(Arbore* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main(int argc, char const *argv[]) {
    FILE *file;
    FILE *fout;

    if ((file = fopen(argv[1], "rt")) == NULL){
        printf("fisierul de intrare nu poate fi deschis \n");
        exit(1);
    }

    if ((fout = fopen(argv[2], "w")) == NULL){
        printf("fisierul de iesire nu poate fi deschis \n");
        exit(1);
    }
    int tp,nl,nc,ng; //tp=tip problema nl=nr linii nc=nr col ng=nr gen
    fscanf(file,"%d %d %d %d",&tp,&nl,&nc,&ng);
    
    char grid1[nl][nc], grid2[nl][nc];
    memset(grid1, 0, sizeof(grid1));
    memset(grid2, 0, sizeof(grid2));
    char(*grid_current)[nc] = grid1, (*grid_next)[nc] = grid2;

    toEOL(file);
    char(*current)[nc] = grid_current, (*next)[nc] = grid_next;
    if (tp == 1) {
        citire(file, nl, nc, current);
        print_grid(nl, nc, current, fout); // Scrie matricea inițială
        for (int l = 0; l < ng; l++) {
            newgen(nl, nc, current, next);
            print_grid(nl, nc, next, fout);

            char(*temp)[nc] = current;
            current = next;
            next = temp;
        }
    }

    else if (tp == 2) {
        citire(file, nl, nc, current);
        gen* top = NULL;

        // Generăm toate generațiile și stocăm modificările
        for (int l = 0; l < ng; l++) {
            node* aux = NULL;
            newgen(nl, nc, current, next);
            
            // Găsim celulele modificate și le adăugăm în ordine
            for (int i = 0; i < nl; i++) {
                for (int j = 0; j < nc; j++) {
                    if (current[i][j] != next[i][j]) {
                        addAtEnd(&aux, i, j);
                    }
                }
            }
            push(&top, &aux);
            
            // Copiem next în current pentru următoarea generație
            memcpy(current, next, sizeof(char) * nl * nc);
        }

        // Inversăm stiva pentru a afișa în ordinea corectă (1 la ng)
        gen* reversed = NULL;
        gen* temp = top;
        while (temp != NULL) {
            node* list_copy = NULL;
            node* curr_node = temp->head;
            
            while (curr_node != NULL) {
                addAtEnd(&list_copy, curr_node->l, curr_node->c);
                curr_node = curr_node->next;
            }
            
            // Adăugăm în stiva inversată
            gen* newGen = (gen*)malloc(sizeof(gen));
            newGen->head = list_copy;
            newGen->next = reversed;
            reversed = newGen;
            
            temp = temp->next;
        }

        // Afișăm stiva în formatul cerut
        gen* current_gen = reversed;
        int gen_num = 1;
        while (current_gen != NULL) {
            fprintf(fout, "%d", gen_num);
            node* coord = current_gen->head;
            while (coord != NULL) {
                fprintf(fout, " %d %d", coord->l, coord->c);
                coord = coord->next;
            }
            fprintf(fout, "\n");
            gen_num++;
            current_gen = current_gen->next;
        }

        // Eliberăm memoria
        freeStack(top);
        freeStack(reversed);
    }
    else if (tp == 3) {
        Arbore* root = malloc(sizeof(Arbore));
        citire(file, nl, nc, root->grid);
        generare(root, nl, nc, ng);
        preordine(root, nl, nc, fout);
        freeTree(root);
    }
    fclose(file);  // Adaugă această linie
    fclose(fout);  // E bine să închizi și fișierul de ieșire
    return 0;
}

