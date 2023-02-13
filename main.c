#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
* Ohjelmointi 1
* Kotitehtävä: Osa 6
*
* RAHA-BOTTI 1.0.0
*
* @date: 2022/12/29
* @author: Johanna Tiikkainen
*
*/


int login(FILE * fp);
int open_account(FILE * fp, int balance[1]);
int main_menu(FILE * fp, int balance[1], char account[128]);
int withdraw_menu(FILE * fp, int balance[1], char account[128]);
void withdraw(FILE * fp, int balance[1], char account[128]);
void banknotes(int amount);
void display_balance(int balance[0]);
void recent_events(FILE * fp);
void edit_file(FILE * fp, int balance[1], int amount, char account[128]);
void read_str(char * str, int str_size);
int read_int(void);
void clear_buffer(void);


int main()
{
    FILE * fp;
    char account[128] = {0};
    int balance[1] = {0};

    printf("\nTervetuloa RAHA-BOTTIIN. Syota tilinumero > ");
    read_str(account, 128);
    strcat(account, ".txt");

    if ((fp = fopen(account, "r+")) == NULL) {
        printf("\nVirhe: Tilia ei ole olemassa");
        return 0;
    }

    if (login(fp) != 1) {
        return 0;
    }

    if (open_account(fp, balance) != 1) {
        return 0;
    }

    while (main_menu(fp, balance, account) == 1);

    fclose(fp);
    fp = NULL;

    return 0;
}


/*
*
* open_account()
*
* Lukee tiedostosta rivin, joka sisältää tilin saldon. Lukee saldomerkkijonon
* kokonaislukumuuttujaan sscanf()-funktiolla. Käsittelee mahdolliset tiedostonlukuvirheet.
*
* Parametrit: tiedoston osoitin fp (FILE *), taulukko balance[] (int)
* Palauttaa: (int) 1, jos saldon luku tiedostosta onnistui tai 0, jos ei
*
*/

int open_account(FILE * fp, int balance[1]) {

    char str[128] = {0};
    int status = 0;

    fseek(fp, 0, SEEK_SET);

    fgets(str, 128, fp); // luetaan ensimmäinen rivi tiedostosta

    fseek(fp, strlen(str)+1, SEEK_SET); // tiedosto-osoitin tiedoston toisen rivin alkuun

    if (fgets(str, 128, fp) == NULL) {
        printf("\nVirhe: Saldoa ei voitu lukea\n");
        return 0;
    }

    status = sscanf(str, "%d", &balance[0]);

    if (balance[0] < 0) {
        printf("\nVirhe: Tilin saldo on negatiivinen\n");
        return 0;
    }

    if (status == 0 || status == -1) {
        printf("\nVirhe: Saldoa ei voitu lukea\n");
        return 0;
    }

    return 1;
}


/*
*
* login()
*
* Kysyy käyttäjältä tilin tunnuslukua ja vertaa syötettyä lukua tiedostossa olevaan
* oikeaan tunnuslukuun. Jos käyttäjä syöttää väärän tunnusluvun kolme kertaa,
* sisäänkirjautuminen hylätään.
*
* Parametrit: tiedoston osoitin fp (FILE *)
* Palauttaa: (int) 1 (onnistunut sisäänkirjautuminen) tai 0 (hylätty)
*
*/

int login(FILE * fp) {

    char user_pin[128];
    char correct_pin[128];
    char option = ' ';
    int counter = 0;

    printf("\nSyota tunnusluku > ");

    read_str(user_pin, 128);

    fseek(fp, 0, SEEK_SET);

    if (fgets(correct_pin, 128, fp) != 0) {

        if (correct_pin[strlen(correct_pin)-1] == '\n') {
            correct_pin[strlen(correct_pin)-1] = '\0';
        }
        else {
            clear_buffer();
        }
    }

    else if (fgets(correct_pin, 128, fp) == 0) {
        printf("\nVirhe: Tiedostoa ei voitu lukea\n");
        return 0;
    }

    if (correct_pin[0] == '\0') {
        printf("\nVirhe: Tunnuslukua ei voitu lukea\n");
        return 0;
    }

    while (strcmp(user_pin, correct_pin) != 0) {

        counter += 1;

        printf("\nVaara tunnusluku \nSyota \"Y\" yrittaaksesi uudelleen tai \"L\" poistuaksesi RAHA-BOTISTA > ");

        while ((scanf(" %c", &option)) && !(option == 'L' || option == 'l' || option == 'Y' || option == 'y')) {

            printf("\nVirheellinen komento");
            printf("\nSyota \"Y\" yrittaaksesi uudelleen tai \"L\" poistuaksesi RAHA-BOTISTA > ");
            clear_buffer();
        }

        if (option == 'L' || option == 'l') {
            printf("\nKiitos RAHA-BOTIN kaytosta! Tervetuloa uudestaan\n");
            return 0;
        }

        else if (option == 'Y' || option == 'y') {

            clear_buffer();

            printf("\nSyota tunnusluku > ");
            read_str(user_pin, 128);

            while (strcmp(user_pin, correct_pin) != 0) {

                counter += 1;

                if (counter == 3) {
                    printf("\nSyotit vaaran tunnusluvun 3 kertaa. Sisaankirjautuminen hylatty\n");
                    return 0;
                }

                printf("\nVaara tunnusluku\nSyota tunnusluku > ");
                read_str(user_pin, 128);
            }

            return 1;
        }
    }

    return 1;
}


/*
*
* main_menu()
* Päävalikko, josta valitaan automaatin toiminnot.
*
* Parametrit: tiedoston osoitin fp (FILE *), taulukko balance[] (int)
* Palauttaa: (int) 1, jos käyttäjä jatkaa automaatin käyttöä tai 0, jos käyttäjä poistuu automaatista
*
*/

int main_menu(FILE * fp, int balance[1], char account[128]) {

    char main_menu_option = ' ';

    printf("\n*** PAAVALIKKO ***\n");
    printf("\n[ N ]: Nosta \n[ S ]: Saldo \n[ T ]: Tilitapahtumat \n[ L ]: Lopeta \n\n> ");

    scanf("%c", &main_menu_option);
    clear_buffer();

    if (main_menu_option == 'L' || main_menu_option == 'l') {
        printf("\nKiitos RAHA-BOTIN kaytosta! Tervetuloa uudestaan\n");
        return 0;
    }

    else if (main_menu_option == 'N' || main_menu_option == 'n') {
         if (withdraw_menu(fp, balance, account) == 0) {
            return 0;
         }
    }

    else if (main_menu_option == 'S' || main_menu_option == 's') {
        display_balance(balance);
    }

    else if (main_menu_option == 'T' || main_menu_option == 't') {
        recent_events(fp);
    }

    else {
        printf("\nVirheellinen komento\nValitse paavalikon toiminto\n");
    }

    return 1;
}


/*
*
* withdraw_menu()
* Nosto-valikko, josta käyttäjä voi valita toiminnot Nosto, Päävalikko ja Lopeta.
* Tulostaa tilin senhetkisen saldon ja kuinka paljon rahaa tilistä on nostettavissa.
*
* Parametrit: tiedoston osoitin fp (FILE *), taulukko balance[] (int)
* Palauttaa: (int) 1, jos jatketaan automaatin käyttöä tai 0, jos lopetetaan
*
*/

int withdraw_menu(FILE * fp, int balance[1], char account[128]) {

    char withdraw_option = ' ';

    printf("\n*** NOSTO ***\n");
    printf("\nTilin saldo on %d.%02d euroa\n", balance[0]/100, balance[0]%100);
    printf("\n[ N ] Nosta \n[ P ] Paavalikko \n[ L ] Lopeta \n\n> ");
    scanf("%c", &withdraw_option);
    clear_buffer();

    if (withdraw_option == 'N' || withdraw_option == 'n') {
        withdraw(fp, balance, account);
    }

    else if (withdraw_option == 'P' || withdraw_option == 'p') {
        printf("\nPalataan paavalikkoon\n");
        return 1;
    }

    else if (withdraw_option == 'L' || withdraw_option == 'l') {
        printf("\nKiitos RAHA-BOTIN kaytosta! Tervetuloa uudestaan\n");
        return 0;
    }

    else {
        printf("\nVirheellinen komento\nPalataan paavalikkoon\n");
        return 1;
    }

    return 1;
}


/*
*
* withdraw()
* Kysyy käyttäjältä nostettavan rahan määrän. Automaatista voi nostaa 50 ja 20 euron seteleinä
* 20 euroa tai 40-1000 euroa kymmenen euron välein. Vähentää nostetun määrän tilin saldosta.
*
* Parametrit: tiedoston osoitin fp (FILE *), taulukko balance[] (int)
*
*/


void withdraw(FILE * fp, int balance[1], char account[128]) {

    int amount = 0;

    printf("\nVoit nostaa 20 tai 40-1000 euroa kymmenen euron valein");
    printf("\nSyota 0, jos haluat palata paavalikkoon\n");

    printf("\nSyota nostettava maara euroina > ");

    amount = read_int() * 100;

    while ((amount < 0 || amount > 1000*100) || (amount % 1000 != 0) || (amount == 1000 || amount == 3000)) {
        printf("\nVirhe: Voit nostaa 20 tai 40-1000 euroa kymmenen euron valein > ");
        amount = read_int() * 100;
    }

    if (amount == 0) {
        printf("\nPalataan paavalikkoon\n");
        return;
    }

    if (amount > balance[0]) {
        printf("\nTililla ei ole tarpeeksi nostettavaa rahaa\n");
        return;
    }

    balance[0] -= amount;

    edit_file(fp, balance, amount, account);

    printf("\nNostit %d euroa", amount/100);

    banknotes(amount);

    printf("\nTilin saldo on %d.%02d euroa\n", balance[0]/100, balance[0]%100);

    return;
}


/*
*
* banknotes()
* Laskee, kuinka monesta 50 ja 20 euron setelistä käyttäjän nostama rahamäärä koostuu.
* Tulostaa setelien lukumäärän.
*
* Parametrit: tiedoston osoitin fp (FILE *), taulukko balance[] (int)
*
*/

void banknotes(int amount) {

    int notes[2] = {0}; // taulukko 50 ja 20 euron setelien määrälle: indeksi 0 50:t, indeksi 1 20:t
    int euros = amount/100;


    // jos nostettava rahamäärä koostuu pelkästään 50 euron seteleistä

    if (euros % 50 == 0) {
        notes[0] = euros/50;
    }

    else if (euros % 50 != 0) {

        // jos 50 euron seteleiden suurimman mahdollisen määrän jälkeen loppusumma koostuu 20 euron seteleistä

        if ((euros % 50) % 20 == 0) {
            notes[0] = euros/50;
            euros -= notes[0]*50;
            notes[1] = euros/20;
        }

        // jos ei, 50:n euron seteleitä täytyy olla yksi vähemmän

        else {
            notes[0] = (euros/50)-1;
            euros -= notes[0]*50;
            notes[1] = euros/20;
        }
    }

    printf("\n50 euron seteleita %d kpl ja 20 euron seteleita %d kpl", notes[0], notes[1]);

    return;
}


/*
*
* display_balance()
* Tulostaa tilin senhetkisen saldon näytölle.
*
* Parametrit: taulukko balance[] (int)
*
*/

void display_balance(int balance[1]) {

    printf("\n*** SALDO ***\n");
    printf("\nTilin saldo on %d.%02d euroa\n", balance[0]/100, balance[0]%100);

    return;
}


/*
*
* recent_events()
* Tulostaa näytölle tiedostoon tallennetun viimeisimmän tilitapahtuman.
*
* Parametrit: tiedoston osoitin fp (FILE *)
*
*/

void recent_events(FILE * fp) {

    char event[128] = {0};
    int counter = 0;

    printf("\n*** TILITAPAHTUMAT ***\n");

    printf("\nTulostetaan viimeisin tilitapahtuma\n");

    fseek(fp, 0, SEEK_SET);

    while (fgets(event, 128, fp) != NULL) {
        counter += 1;
        if (counter == 3) {
            break;
        }
    }

    if (counter != 3) {
        printf("\nVirhe: Tilitapahtumia ei voitu lukea\n");
        return;
    }

    if (event[strlen(event)-1] == '\n') {
        event[strlen(event)-1] = '\0';
    }

    printf("\n%s\n", event);

    return;
}


/*
*
* edit_file()
* Muokkaa tekstitiedostossa olevia saldo- ja tilitapahtumatietoja. Lukee tiedostosta säilytettävän
* tiedon merkkijonoon ja lisää merkkijonoon muutetut tiedot. Tiedosto pyyhitään tyhjäksi ja uusi sisältö
* kirjoitetaan tyhjään tiedostoon.
*
* Parametrit: tiedoston osoitin fp (FILE *), taulukko balance[] (int), nostetun rahan määrä amount (int),
* tilitiedoston nimimerkkijono account[] (char)
*/

void edit_file(FILE * fp, int balance[1], int amount, char account[128]) {

    char balance_str[128] = {0};
    char event[128] = {0};
    char full[256] = {0};
    char line[128] = {0};

    snprintf(balance_str, 128, "%d", balance[0]);
    strcat(balance_str, "\n");

    snprintf(event, 128, "Nostettiin %d euroa", amount/100);

    fseek(fp, 0, SEEK_SET); // tiedosto-osoitin tiedoston alkuun

    fgets(line, 128, fp); // luetaan ensimmäinen rivi (pin-koodi) tiedostosta

    strcat(full, line);

    strcat(full, balance_str);

    strcat(full, event);

    fclose(fp);
    fp = NULL;

    if ((fp = fopen(account, "w+")) == NULL) {
        perror("\nVirhe");
        return;
    }

    fclose(fp);
    fp = NULL;

    if ((fp = fopen(account, "r+")) == NULL) {
        perror("\nVirhe");
        return;
    }

    fseek(fp, 0, SEEK_SET);

    fprintf(fp, "%s", full);

    return;
}


/*
*
* read_str()
* Lukee käyttäjän syöttämän merkkijonon.
*
* Parametrit: merkkijono-osoitin str (char *), merkkijonon koko str_size (int)
*
*/

void read_str(char * str, int str_size) {

    fgets(str, str_size, stdin);

    if (str[strlen(str)-1] == '\n') {
        str[strlen(str)-1] = '\0';
    }
    else {
        clear_buffer();
    }

    return;
}


/*
*
* read_int()
* Lukee käyttäjän syöttämän kokonaisluvun.
*
* Palauttaa: (int) käyttäjän syöttämä luku
*
*/

int read_int(void) {

    int num = 0;
    char c = ' ';
    int status = 0;

    while (((status = scanf("%d%c", &num, &c)) == 0)  || (2 == status && c != '\n')) {

        clear_buffer();
        printf("\nEt syottanyt kokonaislukua > ");
    }

    return (num);
}


/*
*
* clear_buffer()
* Tyhjentää näppäimistöpuskurin.
*
*/

void clear_buffer(void) {

    while (fgetc(stdin) != '\n');

    return;
}
