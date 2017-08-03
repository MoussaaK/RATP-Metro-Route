#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>              // Pour printf, scanf, getchar, 
#include <string.h>            // Pour strlen, strcmp,
#include <stdlib.h>           // Pour EXIT_SUCCESS, EXIT_FAILLURE, 
#include <time.h>            // Pour localtime, time_t, struct tm;
#define SIZE 1412           // Nombre de ligne du fichier metro.csv
#define NombreLigne 14     // Nombre de ligne de metro
#define INFINI 100000     // Valeur très grande utilisé dans shortestPath

typedef struct _HORAIRE
{
  int heure;
  int minute;
} HORAIRE;

/* structure representant une station ie 
un arret donné contenu dans une ligne*/
typedef struct _STATION
{
  char *nomStation;
  struct _STATION *suiv;
  struct _STATION *prec;
  HORAIRE premierTrain;
  HORAIRE dernierTrain;

  /* champ  sup pour mesurer le temps 
  entre deux station avec DJIKSTRA */
  double poids;
  struct _STATION *parent;
  int tempsstationSuiv;    // le temps qu'il faut pour aller à la prochaine station
  int tempsstationPrec;   // le temps qu'il faut pour aller à la station precedente
  int numLigneStation;   // la ligne sur laquelle se trouve la station

} STATION;

/*structure representant une ligne */
typedef struct _LIGNE
{
  STATION *premiereStation;
  STATION *derniereStation;
  int numLigne;   
} LIGNE;

/*Declaration des fonctionnamlitées supplémentaires*/

void printStation(STATION* station);
void printLigne(LIGNE * maLigne);
void initialiseTemps(LIGNE maLigne);
void ajouteDevantLigne(LIGNE *ligne, STATION *station);
void appendToLigne(LIGNE *ligne, STATION nouvelleStation);
void loadStationInLigne(LIGNE *pLigne, FILE *pFile, char *ligneStart, char *ligneEnd);
void insert(LIGNE *pLigne, STATION *station);
void loadGraphe(LIGNE metro[], FILE *pFile);
void printPath(LIGNE *pLigne);
void handleCorrespondance(LIGNE *shortestPath);
void keyBordInteraction(FILE *pFile);  // Menu d'interaction avec l'utilisateur
int getSizeBeetweenStartAndEnd(FILE *pFile, char *ligneStart, char *ligneEnd);
int nombreDelement(LIGNE *ligne);
int listeVide(LIGNE* maLigne);
int convertHeureToMin(HORAIRE time);
int findStationInLigne(LIGNE ligne, char *nomStation);
char *handleEncoding(char *string);
STATION *removeTailFromList(LIGNE *maLigne);
STATION *removeHeadFromList(LIGNE *maLigne);
STATION *removeStationX(LIGNE *ligne, char* nomStation);
STATION *getStation(FILE *file);
LIGNE reserve(FILE *file);
LIGNE *shortestPath(LIGNE metro[], LIGNE reserve, char *stationDepart, char *stationDarrivee);



int main(int argc, char *argv[])
{
  // Utilisation des arguments en ligne de commande
  if (argc != 3)
  {
    //Au debut le programme va rentrer ici obligatoirement vu qu'on pas donne les arguments 
    printf("\nTapez windows + R, tapez cmd puis entrer pour ouvrir le cmd puis aller dans :\n %s\n", argv[0]);
    printf("\nAssurez vous que le dossier contient bien le fichier metro_modifiee.csv !");
    printf("\nPuis tapez ProjetMetro.exe pathFinder metro_modifiee.csv pour executer le programme\n");
    getchar();
    exit(1);
  }
  while (strcmp(argv[1], "pathFinder"))
  {
    printf("Utilistion: %s %s %s\n ", "ProjetMetro.exe", "pathFinder", "metro_modifiee.csv");
    getchar();
  }
  FILE *pFile = fopen(argv[2], "r");
  //FILE *pFile = fopen("metro_modifiee.csv", "r");
  if (pFile == NULL)
  {
    printf(" Erreur a l'ouverture de %s\n ", argv[1]);
    getchar();
    exit(1);
  }
  //interaction clavier
  keyBordInteraction(pFile);
  fclose(pFile);
  getchar();

  return EXIT_SUCCESS;
}

/***
Fonction listeVide retourne 1 si la liste est vide, 0 sinon
**/
int listeVide(LIGNE* maLigne)
{
  if (maLigne->premiereStation == NULL)
    return 1; //liste Vide
  else
    return 0;
}

/***
Fonction qui prend en paramètre une station (maillon d'une liste) et l'affiche
--> Situation : Reste des modifications à ajouter pour afficher toutes les informations necessaires
**/
void printStation(STATION *station)
{
  printf("%s(poid : %lg)", station->nomStation, station->poids);
}

/***
Fonction qui prend en paramètre une ligne (liste) et l'affiche
**/
void printLigne(LIGNE * maLigne)
{
  if (listeVide(maLigne))
    printf("Ligne inexistant\n");
  else
  {
    STATION *station;
    station = maLigne->premiereStation;
    while (station != maLigne->derniereStation)
    {
      printStation(station); printf("-->");
      station = station->suiv;
    }
    printStation(maLigne->derniereStation);
  }
}

/***
Fonction qui affichera le trajet à suivre dans la console
**/
void printPath(LIGNE *pLigne)
{
  // temps du système en utilisant time.h
  time_t temps;
  time(&temps);
  struct tm *tempsPC = localtime(&temps);

  //Affichage
  int i, n = strlen(pLigne->premiereStation->nomStation), m = strlen(pLigne->derniereStation->nomStation);
  printf("\n+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
  printf("|\t\tDepart : %d:%d\t\t\t Arrive : %d:%d\t\t       |", tempsPC->tm_hour, tempsPC->tm_min, tempsPC->tm_hour, tempsPC->tm_min + (int)pLigne->derniereStation->poids);
  printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +|");
  printf(" -> Station de depart : %s", pLigne->premiereStation->nomStation); for (i = 0; i < 80 - (26 + n); i++) printf(" ");
  printf("|+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +|");
  printf(" -> Station d'arrivee : %s", pLigne->derniereStation->nomStation); for (i = 0; i < 80 - (26 + m); i++) printf(" ");
  printf("|+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +|");
  printf("\t\tOn vous propose  l'itineraire suivant (%lg min):\t\t       |", pLigne->derniereStation->poids);
  printf("| - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - |");

  // Si la station de depart est la meme que la station darrive on le siganale et arrete le programme
  if (strcmp(pLigne->premiereStation->nomStation, pLigne->derniereStation->nomStation) == 0)
  {
    printf("|\t\t\t\tArrive = Depart\t\t\t\t       |FIN");
    printf("   - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - |");
    getchar();
    exit(EXIT_SUCCESS);
  }

  printf("| --> %d:%d Quitter la station %s : prendre la ligne %d\n|\n|", tempsPC->tm_hour, tempsPC->tm_min, pLigne->premiereStation->nomStation, pLigne->premiereStation->numLigneStation);
  STATION *vparcours = pLigne->premiereStation->suiv;
  printf("  # %s \n|", vparcours->nomStation);
  int curseur = 0, curseur2 = 0;
  while (vparcours->suiv != NULL)
  {
    if (vparcours->suiv->numLigneStation == vparcours->numLigneStation)
      printf("  # %s \n|", vparcours->suiv->nomStation);
    else
    {
      //if (curseur < 1)
      //{
        printf("\n| --> %d:%d Changement de ligne :\n|\tAller sur la ligne %d depuis la station %s\n|\n|", tempsPC->tm_hour, (tempsPC->tm_min + (int)vparcours->poids), vparcours->suiv->numLigneStation, vparcours->nomStation);
        printf("  # %s \n|", vparcours->suiv->nomStation);
      //}
      //curseur++;
    }
    vparcours = vparcours->suiv;
  }
  //printf("  # %s \n|", pLigne->derniereStation->nomStation);
  printf("\n+ --> %d:%d Arrive a : %s\n", tempsPC->tm_hour, (tempsPC->tm_min + (int)vparcours->poids), pLigne->derniereStation->nomStation);
  printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +\n");
}

/***
Fonction qui lit les données d'une station dans la ligne de métro
On lit une ligne d'un fichier, on utilise strtok pour retourner les chaines de caractères avant ":"
puis on convertit cette chaine quand il le faut et on le stocke dans une structure qu'on retourne
**/
STATION *getStation(FILE *file)
{
  STATION *station = (STATION*)calloc(1, sizeof(STATION));
  if (file == NULL)
    printf("Impossible d'ouvrir le fichier metro.csv");
  else
  {
    char *str = (char*)calloc(SIZE, sizeof(char));
    int n = 0, i = 0;
    while (n < 1)
    {
      fgets(str, SIZE, file);
      char *ptr = strtok(str, ":");
      while (ptr != NULL)
      {
        if (i == 0)
          station->numLigneStation = atoi(ptr); //printf("%d", station->numLigneStation);
        else if (i == 1) {/*on recupère le sens du trajet*/ }
        else if (i == 2)
        {
          station->nomStation = (char*)calloc(strlen(ptr) + 1, sizeof(char));
          station->nomStation[strlen(ptr)] = '\0';
          strcpy(station->nomStation, handleEncoding(ptr));
        }
        else if (i == 3) {}
        else if (i == 4) {}
        else if (i == 5)
        {
          double heureDecimale = atof(ptr);
          int heure = (int)heureDecimale + 0.00000001; //on ajoute cette valeur pour arrondir
          int minute = (heureDecimale - heure) * 100.0;
          station->premierTrain.heure = heure;
          station->premierTrain.minute = minute;
        }
        else if (i == 6)
        {
          double heureDecimale = atof(ptr);
          int heure = (int)heureDecimale;
          int minute = (heureDecimale - heure) * 100.0;
          station->dernierTrain.heure = heure;
          station->dernierTrain.minute = minute;
        }
        else{}
          //printf("Erreur au niveau de l'accès au champs\n");
        i++;
        ptr = strtok(NULL, ":");
      }
      n++;
    }
  }
  return station;
}

/***
Fonction qui supprime à partir de la tête de la ligne
**/
STATION *removeTailFromList(LIGNE *maLigne)
{
  //initialisation à 0
  STATION *stationToRemove = NULL;
  if (listeVide(maLigne))
    printf("Impossible de supprimer une liste vide !");
  else
  {
    stationToRemove = maLigne->derniereStation;
    maLigne->derniereStation = maLigne->derniereStation->prec;
    if (listeVide(maLigne))
    {
      maLigne->premiereStation = NULL;
      maLigne->derniereStation = NULL;
    }
    else
      maLigne->derniereStation->suiv = NULL;
  }
  return stationToRemove;
}

/***
Fonction qui supprime à partir de la queue de la ligne
**/
STATION *removeHeadFromList(LIGNE *maLigne)
{
  //initialisation à 0
  STATION *stationToRemove = NULL;
  if (listeVide(maLigne))
    printf("Impossible d'enlever au debut si la liste est vide ");
  else
  {
    stationToRemove = maLigne->premiereStation;
    maLigne->premiereStation = maLigne->premiereStation->suiv;
    if (listeVide(maLigne)) { /*printf("ErreUr liste Vide");*/ }
    else
      maLigne->premiereStation->prec = NULL;
  }
  return  stationToRemove;
}

/***
Fonction ajoute une station à partir de la queue de la ligne
**/
void appendToLigne(LIGNE *ligne, STATION nouvelleStation)
{
  STATION *station = (STATION *)calloc(1, sizeof(STATION));

  //on initialise les champs de station par les valeurs passées en paramètre
  station->nomStation = malloc(SIZE * sizeof(char));
  strcpy(station->nomStation, nouvelleStation.nomStation);
  station->premierTrain.heure = nouvelleStation.premierTrain.heure;
  station->premierTrain.minute = nouvelleStation.premierTrain.minute;
  station->dernierTrain.heure = nouvelleStation.dernierTrain.heure;
  station->dernierTrain.minute = nouvelleStation.dernierTrain.minute;
  station->tempsstationSuiv = nouvelleStation.tempsstationSuiv;
  station->poids = nouvelleStation.poids;
  station->parent = nouvelleStation.parent;
  station->numLigneStation = nouvelleStation.numLigneStation;

  if (listeVide(ligne))
  {
    ligne->premiereStation = station;
    ligne->derniereStation = station;
  }
  else
  {
    ligne->derniereStation->suiv = station;
    station->prec = ligne->derniereStation;
    ligne->derniereStation = station;
  }
}

/***
Fonction ajoute une station à partir de la tete de ligne
**/
void ajouteDevantLigne(LIGNE *ligne, STATION *station)
{
  if (listeVide(ligne))
  {
    ligne->premiereStation = station;
    ligne->derniereStation = station;
  }
  else
  {
    ligne->premiereStation->prec = station;
    station->suiv = ligne->premiereStation;
    ligne->premiereStation = station;
  }
}

/***
Fonction qui donne le nombre de saut à la ligne entre 2 lignes de métro en
lisant le fichier et en utilisant les délimiteurs qui identifient les lignes
arguments: la ligne, le fichier, les delimiteurs du debut et de la fin de ligne
**/
int getSizeBeetweenStartAndEnd(FILE *pFile, char *ligneStart, char *ligneEnd)
{
  int nbSizeOfLine = 0, startposition = 0;
  char *str = (char*)calloc(SIZE, sizeof(char));

  // On calcule le nombre de lignes entre ####i et ####j 
  fgets(str, SIZE, pFile);
  startposition = ftell(pFile);
  char *ptr = strtok(str, ":");
  if (strcmp(ligneStart, ptr) != 0)
  {
    do
    {
      fgets(str, SIZE, pFile);
      ptr = strtok(str, ":");
    } while (strcmp(ligneStart, ptr) != 0);
    startposition = ftell(pFile); //on sauvegarde la position à la quelle on s'est déplacé
  }
  if (strcmp(ligneStart, ptr) == 0)
    do
    {
      fgets(str, SIZE, pFile);
      ptr = strtok(str, ":");
      nbSizeOfLine++;
    } while (strcmp(ligneEnd, ptr) != 0);
  fseek(pFile, startposition, SEEK_SET); // on remet le curseur au niveau du ####i de départ pour commencer à afficher à partir de cette position
  return nbSizeOfLine;
}

/***
Fonction qui permet de remplir une ligne de métro avec les stations qui correspondent
arguments : la ligne, le fichier, le debut et la fin de la ligne
**/
void loadStationInLigne(LIGNE *pLigne, FILE *pFile, char *ligneStart, char *ligneEnd)
{
  int i = 0, nbSizeOfLine;
  STATION *stations = (STATION*)calloc(SIZE, sizeof(STATION));
  nbSizeOfLine = getSizeBeetweenStartAndEnd(pFile, ligneStart, ligneEnd);
  if (pFile == NULL) printf("Erreur d'ouverture du fichier");
  else
  {
    while (i < nbSizeOfLine - 1)
    {
      stations[i] = *getStation(pFile);
      appendToLigne(pLigne, stations[i]);
      i++;
    }
    // On renseigne le numéro de la ligne
    if (ligneStart == "####1")
      pLigne->numLigne = 1;
    else if (ligneStart == "####2")
      pLigne->numLigne = 2;
    else if (ligneStart == "####3" || ligneStart == "####3bis")
      pLigne->numLigne = 3;
    else if (ligneStart == "####4")
      pLigne->numLigne = 4;
    else if (ligneStart == "####5")
      pLigne->numLigne = 5;
    else if (ligneStart == "####6")
      pLigne->numLigne = 6;
    else if (ligneStart == "####7" || ligneStart == "####7bis")
      pLigne->numLigne = 7;
    else if (ligneStart == "####8")
      pLigne->numLigne = 8;
    else if (ligneStart == "####9")
      pLigne->numLigne = 9;
    else if (ligneStart == "####10")
      pLigne->numLigne = 10;
    else if (ligneStart == "####11")
      pLigne->numLigne = 11;
    else if (ligneStart == "####12")
      pLigne->numLigne = 12;
    else if (ligneStart == "####13" || ligneStart == "####13bis")
      pLigne->numLigne = 13;
    else
      pLigne->numLigne = 14;
  }
}

/***
Fonction qui insert une station dans la ligne en fonction du poids
**/
void insert(LIGNE *pLigne, STATION *station)
{
  STATION *vParcours = pLigne->premiereStation;
  vParcours = pLigne->premiereStation;
  if (pLigne->premiereStation == NULL)
  {
    station->suiv = NULL;
    pLigne->premiereStation = station;
    pLigne->derniereStation = station;
  }
  while (vParcours != NULL)
  {
    if (station->poids < vParcours->poids)
    {
      if (vParcours == pLigne->premiereStation)
      {
        vParcours->prec = station;
        pLigne->premiereStation = station;
        station->suiv = vParcours;
        break;
      }
      else
      {
        station->prec = vParcours->prec;
        vParcours->prec->suiv = station;
        vParcours->prec = station;
        station->suiv = vParcours;
        break;
      }
    }
    vParcours = vParcours->suiv;
  }
  vParcours = pLigne->derniereStation;
  if (station->poids >= vParcours->poids)
    appendToLigne(pLigne, *station);
}

/***
Fonction qui supprime une station à partir de n'importe quelle position
dans la ligne puis retourne cette station
**/
STATION *removeStationX(LIGNE *ligne, char *nomStation)
{
  STATION *stationToRemove = NULL;
  if (listeVide(ligne))
    printf("On ne peux retirer d'une quelconque position si la ligne est vide !");
  else
  {
    // Liste Update
    if (!strcmp(ligne->premiereStation->nomStation, nomStation))
      stationToRemove = removeHeadFromList(ligne);
    else if (!strcmp(ligne->derniereStation->nomStation, nomStation))
      stationToRemove = removeTailFromList(ligne);
    else
    {
      stationToRemove = ligne->premiereStation->suiv;
      while (stationToRemove != ligne->derniereStation && strcmp(stationToRemove->nomStation, nomStation))
        stationToRemove = stationToRemove->suiv;

      stationToRemove->prec->suiv = stationToRemove->suiv;
      stationToRemove->suiv->prec = stationToRemove->prec;
    }
  }
  return stationToRemove;
}

/***
Fonction qui permet de compter toutes les stations d'une ligne
**/
int nombreDelement(LIGNE *ligne)
{
  int n = 0;
  if (listeVide(ligne))
    n = 0;
  else
  {
    STATION *station;
    station = ligne->premiereStation;
    while (station != NULL)
    {
      n++;
      station = station->suiv;
    }
  }
  return n;
}

/***
Fonction qui permet de remplir un tableau de ligne qui correspond au
graphe et d'initialiser les temps pour aller aux stations suivantes
arguments : un tableau de ligne et un fichier
**/
void loadGraphe(LIGNE metro[], FILE *pFile)
{
  //On charge toutes les stations 
  loadStationInLigne(&metro[0], pFile, "####1", "####1");
  loadStationInLigne(&metro[1], pFile, "####2", "####2");
  loadStationInLigne(&metro[2], pFile, "####3", "####3");
  loadStationInLigne(&metro[2], pFile, "####3bis", "####3bis");
  loadStationInLigne(&metro[3], pFile, "####4", "####4");
  loadStationInLigne(&metro[4], pFile, "####5", "####5");
  loadStationInLigne(&metro[5], pFile, "####6", "####6");
  loadStationInLigne(&metro[6], pFile, "####7", "####7");
  loadStationInLigne(&metro[6], pFile, "####7bis", "####7bis");
  loadStationInLigne(&metro[7], pFile, "####8", "####8");
  loadStationInLigne(&metro[8], pFile, "####9", "####9");
  loadStationInLigne(&metro[9], pFile, "####10", "####10");
  loadStationInLigne(&metro[10], pFile, "####11", "####11");
  loadStationInLigne(&metro[11], pFile, "####12", "####12");
  loadStationInLigne(&metro[12], pFile, "####13", "####13");
  loadStationInLigne(&metro[12], pFile, "####13bis", "####13bis");
  loadStationInLigne(&metro[13], pFile, "####14", "####14");

  //on initialise pour chaque station le temps pour aller à la station suivante
  //et on initialise le numéro de la ligne
  for (int j = 0; j < NombreLigne; j++)
    initialiseTemps(metro[j]);
    
}

/***
Fonction qui permet de remplir une ligne contenant de toutes les stations possibles
contenu dans le fichier metro.csv sans repétition (la reserve)
**/
LIGNE reserve(FILE *file)
{
  file = fopen("metro_modifiee.csv", "r");
  LIGNE reserve = { NULL, NULL, 0 };

  //on charge toutes les lignes de metro sens a
  loadStationInLigne(&reserve, file, "####1", "####1");
  loadStationInLigne(&reserve, file, "####2", "####2");
  loadStationInLigne(&reserve, file, "####3", "####3");
  loadStationInLigne(&reserve, file, "####3bis", "####3bis");
  loadStationInLigne(&reserve, file, "####4", "####4");
  loadStationInLigne(&reserve, file, "####5", "####5");
  loadStationInLigne(&reserve, file, "####6", "####6");
  loadStationInLigne(&reserve, file, "####7", "####7");
  loadStationInLigne(&reserve, file, "####7bis", "####7bis");
  loadStationInLigne(&reserve, file, "####8", "####8");
  loadStationInLigne(&reserve, file, "####9", "####9");
  loadStationInLigne(&reserve, file, "####10", "####10");
  loadStationInLigne(&reserve, file, "####11", "####11");
  loadStationInLigne(&reserve, file, "####12", "####12");
  loadStationInLigne(&reserve, file, "####13", "####13");
  loadStationInLigne(&reserve, file, "####13bis", "####13bis");
  loadStationInLigne(&reserve, file, "####14", "####14");
  reserve.numLigne = 0;

  STATION *position = reserve.premiereStation;
  STATION *vparcours = reserve.derniereStation;
  //on verifie si une ligne apparait deux fois on la supprime
  while (position != NULL)
  {
    while (vparcours != position && vparcours != reserve.premiereStation)
    {
      if (!strcmp(position->nomStation, vparcours->nomStation))
        removeStationX(&reserve, vparcours->nomStation);
      vparcours = vparcours->prec;
    }
    vparcours = reserve.derniereStation;
    position = position->suiv;
  }
  return reserve;
}

/***
Fonction qui permet de convertir des heures en minutes
**/
int convertHeureToMin(HORAIRE time)
{
  return ((time.heure * 60) + time.minute);
}

/***
Fonction qui permet d'initialiser le champs tempsstationSuiv qu'il
faut pour aller à la station suivante lorsqu'on est sur une ligne
**/
void initialiseTemps(LIGNE maLigne)
{

  STATION* vparcour = maLigne.premiereStation;
  HORAIRE time2, time1;

  while (vparcour != maLigne.derniereStation)
  {
    time2.heure = vparcour->suiv->dernierTrain.heure - vparcour->dernierTrain.heure;
    time2.minute = vparcour->suiv->dernierTrain.minute - vparcour->dernierTrain.minute;

    vparcour->tempsstationSuiv = convertHeureToMin(time2);
    if (vparcour->tempsstationSuiv == 0)
      vparcour->tempsstationSuiv = 1;//correction de l'erreur
    vparcour = vparcour->suiv;
  }

  vparcour = maLigne.derniereStation;
  while (vparcour != maLigne.premiereStation)
  {
    time1.heure = vparcour->dernierTrain.heure - vparcour->prec->dernierTrain.heure;
    time1.minute = vparcour->dernierTrain.minute - vparcour->prec->dernierTrain.minute;

    vparcour->tempsstationPrec = convertHeureToMin(time1);
    if (vparcour->tempsstationPrec == 0)
      vparcour->tempsstationPrec = 1;//correction de l'erreur
    vparcour = vparcour->prec;
  }

  maLigne.premiereStation->tempsstationPrec = 0;
  maLigne.derniereStation->tempsstationSuiv = 0;
}
/***
Fonction qui permet de convertir d'initialiser les poids
**/
void initialisePoids(LIGNE maLigne)
{
  maLigne.premiereStation->poids = 0;
  STATION* vparcour = maLigne.premiereStation;
  HORAIRE time2;

  while (vparcour != maLigne.derniereStation)
  {
    time2.heure = vparcour->suiv->dernierTrain.heure - vparcour->dernierTrain.heure;
    time2.minute = vparcour->suiv->dernierTrain.minute - vparcour->dernierTrain.minute;
    vparcour->tempsstationSuiv = convertHeureToMin(time2);
    if (vparcour->tempsstationSuiv == 0) //pour suprimer l'erreur ie l'arrondi de l'ordinateur
      vparcour->tempsstationSuiv = 1;
    vparcour = vparcour->suiv;
  }
}

/***
Fonction qui permet de trouver une station dans une ligne
return 1 si la station est presente, 0 sinon
**/
int findStationInLigne(LIGNE ligne, char *nomStation)
{
  STATION *vparcour = ligne.premiereStation;
  while (vparcour->suiv != NULL)
  {
    if (strcmp(vparcour->nomStation, nomStation) == 0)
      return 1;
    vparcour = vparcour->suiv;
  }
  return 0;
}

/***
Fonction qui convertit la chaine saisie en majuscule s'il y a des espaces, apostrophes, des 
virgules ou des tirets de 8, on les laisse intacte, cela assure la reconnaissance de la station 
**/
char *handleEncoding(char *string)
{
  int stringSize = strlen(string), i;
  char *newString = (char*)calloc(stringSize + 1, sizeof(char));
  for (i = 0; i < stringSize; i++)
  {
    // les noms des stations ne contiennent que ces 4 caracteres speciaux on les gère ici
    if (string[i] == ' ' || string[i] == '\'' || string[i] == ',' || string[i] == '-')
      newString[i] = string[i];

    //conversion des minuscules en majuscules
    if (string[i] <= 'z' && string[i] >= 'a')
      newString[i] = string[i] - 32;
    else
      newString[i] = string[i];
  }
  return newString;
}
/***
Fonction qui gère les correspondances en ajoutant 5 min de plus
quand il y a correspondance
**/
void handleCorrespondance(LIGNE *shortestPath)
{
  STATION *vparcour = shortestPath->premiereStation->suiv;
  while (vparcour->suiv != NULL)
  {
    if (vparcour->numLigneStation != vparcour->prec->numLigneStation)
      vparcour->poids = vparcour->poids + 5;
  }
}

/***
Fonction qui permet d'interagir avec l'utilsateur en prenant compte tout
ce que l'utilisateur pourra saisir ou presque qui fera planter le programme
**/
void keyBordInteraction(FILE *pFile)
{
  //FILE *pFile = fopen("metro_modifiee.csv", "r"); //ouverture du fichier metro.csv en mode lecture
  if (pFile == NULL)
    printf("Impossible d'ouvrir le fichier metro_modifiee.csv dans keyboardInteraction");
  else
  {
    //test Djikstra
    LIGNE metro[NombreLigne] = { NULL, NULL, 0 };
    loadGraphe(metro, pFile);
    
    char *stationDepart = (char*)calloc(1, sizeof(char)), *stationDarrive = (char*)calloc(1, sizeof(char));
    char choix;
    int curseur = 0;
    do
    {
      printf("\n+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
      printf("|       Tapez 1 pour lancer la recherche d'itineraire, q pour quittez : ");
      choix = getchar();
    } while (choix != '1' && choix != 'q');

    if (choix == 'q')
    {
      printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
      printf("+ - - - - - - - - - - - - - - - - - -  FIN  - - - - - - - - - - - - - - - - -  +");
      exit(EXIT_SUCCESS);
    }

    do {
      LIGNE laReserve = reserve(pFile);
      int dejaRentre = 0;
      do {
        if (!dejaRentre)
        {
          printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
          printf("| Entrez la station de depart : ");
          // le buffer contient '\n' donc le programme va sauter la ligne du fgets
          fseek(stdin, 0, SEEK_END); //on retourne au debut du stdin pour eviter de sauter l'appel à fgets
          fgets(stationDepart, SIZE, stdin);
          strtok(stationDepart, "\n"); //fgets rajoute \n aux données saisies : donc on supprime le \n
          stationDepart = handleEncoding(stationDepart); //la station seras a coup sure reconnue sauf si elle n'existe pas
          printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        }
        else
        {
          printf("| Entrez une station de depart Valide SVP : ");
          // le buffer contient '\n' donc le programme va sauter la ligne du fgets
          fseek(stdin, 0, SEEK_END); //on retourne au debut du stdin pour eviter de sauter l'appel à fgets
          fgets(stationDepart, SIZE, stdin);
          strtok(stationDepart, "\n"); //fgets rajoute \n aux données saisies : donc on supprime le \n
          stationDepart = handleEncoding(stationDepart); //la station seras a coup sure reconnue sauf si elle n'existe pas
          printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        }
      dejaRentre++;
      } while (!findStationInLigne(laReserve, stationDepart));

      dejaRentre = 0;
      do {
        if (!dejaRentre)
        {
          printf("| Entrez la station d'arrive  : ");
          fseek(stdin, 0, SEEK_END);
          fgets(stationDarrive, SIZE, stdin);
          strtok(stationDarrive, "\n");
          stationDarrive = handleEncoding(stationDarrive); //la station seras a coup sure reconnue sauf si elle n'existe pas
          printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        }
        else
        {
          printf("| Entrez une station d'arrive Valide SVP  : ");
          fseek(stdin, 0, SEEK_END);
          fgets(stationDarrive, SIZE, stdin);
          strtok(stationDarrive, "\n");
          stationDarrive = handleEncoding(stationDarrive); //la station seras a coup sure reconnue sauf si elle n'existe pas
          printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        }
      } while (!findStationInLigne(laReserve, stationDarrive));

      LIGNE *path = shortestPath(metro, laReserve, stationDepart, stationDarrive);
      //handleCorrespondance(path);
      printPath(path);

      choix = '1';
      do {
        printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        printf("|\t\t\t     Voulez Vous continuer ?\t\t\t       |");
        printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        printf("|     Tapez 1 pour relancer la recherche d'itineraire, q pour quittez : ");
        choix = getchar();
      } while (choix != '1' && choix != 'q');

      if (choix == 'q')
      {
        printf("+ - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - +");
        printf("+ - - - - - - - - - - - - - - - - - -  FIN  - - - - - - - - - - - - - - - - -  +");
        exit(EXIT_SUCCESS);
      }
    } while (choix == '1');
  }
  fclose(pFile);
}

/***
Fonction qui permet de trouver le plus court chemin en utilisant l'algorithme de Djikstra
arguments: le graphe, la reserve, le nom de la station de départ et le nom de la station d'arrivée
**/
LIGNE *shortestPath(LIGNE metro[], LIGNE Reserve, char *stationDepart, char *stationDarrivee)
{
  LIGNE itineraire = { NULL, NULL, 0 }, *itineraireOrdonne = (LIGNE*)calloc(1, sizeof(LIGNE));
  STATION *varParcour = Reserve.premiereStation;
  while (varParcour != NULL)
  {
    varParcour->parent = NULL;
    varParcour->poids = INFINI;
    varParcour = varParcour->suiv;
  }

  STATION *tmp = removeStationX(&Reserve, stationDepart);
  tmp->poids = 0.0;
  insert(&Reserve, tmp);

  STATION *varParcour1Reserve = (STATION*)calloc(1, sizeof(STATION));
  int arrive = 1;
  while ((listeVide(&Reserve) != 1) && arrive)
  {
    varParcour1Reserve = removeHeadFromList(&Reserve);
    insert(&itineraire, varParcour1Reserve);

    if (!strcmp(stationDarrivee, varParcour1Reserve->nomStation))
    {
      STATION *varParcourAffichage = itineraire.derniereStation;
      while (varParcourAffichage != NULL)
      {
        ajouteDevantLigne(itineraireOrdonne, varParcourAffichage);
        varParcourAffichage = varParcourAffichage->parent;
      }
      arrive = 0; //condition de sorti lorsque le chemin le plus court sera trouvé
    }

    //recherche de la station dans le graphe
    double tempsParcours = 0;
    for (int i = 0; i < NombreLigne; i++)
    {
      STATION *varParcour1metro = metro[i].premiereStation;
      STATION *varParcour2Reserve = NULL;
      STATION *toRemove2 = NULL;
      while (varParcour1metro->suiv != NULL)
      {
        if (!strcmp(varParcour1metro->nomStation, varParcour1Reserve->nomStation))
        {
          tempsParcours = (double)varParcour1metro->tempsstationSuiv;
          double nouveauPoids = varParcour1Reserve->poids + tempsParcours;

          // Recherche de la station suivante dans la reserve
          varParcour2Reserve = Reserve.premiereStation;
          while (varParcour2Reserve != NULL)
          {
            if (!strcmp(varParcour2Reserve->nomStation, varParcour1metro->suiv->nomStation))
              if (nouveauPoids < varParcour2Reserve->poids)
              {
                toRemove2 = removeStationX(&Reserve, varParcour2Reserve->nomStation);
                toRemove2->poids = nouveauPoids;
                toRemove2->parent = varParcour1Reserve;  //on donne a son parent l'adresse de la station ????
                insert(&Reserve, toRemove2);
              }
            varParcour2Reserve = varParcour2Reserve->suiv;
          }

          // Dans le sens b
          if (varParcour1metro->prec != NULL)
          {
            tempsParcours = (double)varParcour1metro->tempsstationPrec;
            double nouveauPoids = varParcour1Reserve->poids + tempsParcours;

            // Recherche de la station suivante dans la reserve
            varParcour2Reserve = Reserve.premiereStation;
            while (varParcour2Reserve != NULL)
            {
              if (!strcmp(varParcour2Reserve->nomStation, varParcour1metro->prec->nomStation))
                /*if (varParcour2Reserve->poids > INFINI)
                varParcour2Reserve->poids = varParcour2Reserve->poids - INFINI;*/
                if (nouveauPoids < varParcour2Reserve->poids)
                {
                  toRemove2 = removeStationX(&Reserve, varParcour2Reserve->nomStation);
                  toRemove2->poids = nouveauPoids;
                  toRemove2->parent = varParcour1Reserve;  //on donne a son parent l'adresse de la station ????
                  insert(&Reserve, toRemove2);
                }
               varParcour2Reserve = varParcour2Reserve->suiv;
             }
           }
         }
        varParcour1metro = varParcour1metro->suiv;
      }
    }
  }
  return itineraireOrdonne;
}