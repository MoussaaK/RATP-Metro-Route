***************************************************************************
* Projet Langage C Telecom 1, KONATE Moussa et GUEYE Serigne Abdou Khadre *
***************************************************************************


***************************************************************************************************************************
1 - Choix des structures :

Pour choisir nos structures, on s'est inspir� de la mani�re dont les lignes de m�tro sont faites dans la vie r�elle
On a definit la structures station qui elle contient les champs : nom de la station, heures du premier et dernier train, les adresses des stations suivante et pr�cedente, le temps qu'il faut pour aller � ces stations, le parent et le poids de la station, en fin on a rajout� � la fin du projet un champs suppl�mentaire qui est le num�ro de la ligne sur laquelle se trouve cette station ce qui nous a permis de distinguer les changements de lignes.
On a aussi defini la structure ligne de m�tro qui contient la premi�re et derni�re station de la ligne et le num�ro de la ligne. Puis une derni�re structure permet de g�rer le temps.

***************************************************************************************************************************
2 - Liste des fonctionnalit�s :

+ Utilisation des arguments en ligne de commande pour l'ex�cution
+ L'interaction avec l'utilisateur et gestion des cas particuliers qui peuvent faire plenter le programme
+ La recherche d'itin�raire
+ Affichage de l'itin�raire avec specification claire des temps de trajet

***************************************************************************************************************************
3 - Bilan :

On a r�ussi � faire pratiquement tout ce qui �tait demand� dans le cahier des charges, on a malheureusement pas pu d�sallouer par soucis de temps les m�moires qui ont �t� allou�es et on n'a pas rajout� au trajet le temps d'une correspondance
 
***************************************************************************************************************************
4 - Utilisation :
 
- Compilation : gcc -Wall ItineraireMetro.c -o ItineraireMetro.exe

- Execution   : ItineraireMetro.exe pathFinder metro_modifiee.csv

Pour l'�xecution, il faut s'assurer que le repertoire dans lequel se trouve l'ex�cutable contient bien le fichier metro_modifiee.csv

Au demarrage le programme demande le nom de la station de d�part et d'arriv�e, vous avez une flexibilit� � ce niveau, vous pouvez entrer le nom en majuscule ou en minuscule, parcontre les accents ne sont pas pris en compte ni dans le fichier metro_modifiee ni dans le programme.

Si vos stations sont valides, appuyez "1 FOIS" sur la touche ENTRER pour pouvoir afficher l'itineraire.

Enfin si vous souhaitez continuer tapez, ou quitter en tappant q.

***************************************************************************************************************************