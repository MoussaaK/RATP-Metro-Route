
## Itineraire du metro parisien en Langage C utilisant l'algorithme de djikstra pour rechercher le chemin optimale entre la station de départ et la station d'arrivée.


***************************************************************************
* Projet Langage C Telecom 1, KONATE Moussa et GUEYE Serigne Abdou Khadre *
***************************************************************************


***************************************************************************************************************************
1 - Choix des structures :

Pour choisir nos structures, on s'est inspiré de la manière dont les lignes de métro sont faites dans la vie réelle
On a definit la structures station qui elle contient les champs : nom de la station, heures du premier et dernier train, les adresses des stations suivante et précedente, le temps qu'il faut pour aller à ces stations, le parent et le poids de la station, en fin on a rajouté à la fin du projet un champs supplémentaire qui est le numéro de la ligne sur laquelle se trouve cette station ce qui nous a permis de distinguer les changements de lignes.
On a aussi defini la structure ligne de métro qui contient la première et dernière station de la ligne et le numéro de la ligne. Puis une dernière structure permet de gérer le temps.

***************************************************************************************************************************
2 - Liste des fonctionnalités :

+ Utilisation des arguments en ligne de commande pour l'exécution
+ L'interaction avec l'utilisateur et gestion des cas particuliers qui peuvent faire plenter le programme
+ La recherche d'itinéraire
+ Affichage de l'itinéraire avec specification claire des temps de trajet

***************************************************************************************************************************
3 - Bilan :

On a réussi à faire pratiquement tout ce qui était demandé dans le cahier des charges, on a malheureusement pas pu désallouer par soucis de temps les mémoires qui ont été allouées et on n'a pas rajouté au trajet le temps d'une correspondance
 
***************************************************************************************************************************
4 - Utilisation :
 
- Compilation : gcc -Wall ItineraireMetro.c -o ItineraireMetro.exe

- Execution   : ItineraireMetro.exe pathFinder metro_modifiee.csv

Pour l'éxecution, il faut s'assurer que le repertoire dans lequel se trouve l'exécutable contient bien le fichier metro_modifiee.csv

Au demarrage le programme demande le nom de la station de départ et d'arrivée, vous avez une flexibilité à ce niveau, vous pouvez entrer le nom en majuscule ou en minuscule, parcontre les accents ne sont pas pris en compte ni dans le fichier metro_modifiee ni dans le programme.

Si vos stations sont valides, appuyez "1 FOIS" sur la touche ENTRER pour pouvoir afficher l'itineraire.

Enfin si vous souhaitez continuer tapez, ou quitter en tappant q.

***************************************************************************************************************************
