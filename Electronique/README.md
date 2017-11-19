# Dossier Electronique

Ce dossier contient les fichiers relatifs à l'électronique du robot : PCB et cablage.
Pour la réalisation de circuits imprimés, le logiciel utilisé est  [Designspark 8.0](https://www.rs-online.com/designspark/pcb-download-and-installation).
Attention, un fichier créé avec une version ultérieure de Designspark ne peut être lu par les versions précédentes. Attention donc à ne pas utiliser Designspark 8.1 (actuellement en beta).

### Bibliothèques
Ce dossier contient deux bibliothèques de composants pour Designspark PCB, à ajouter aux composants présents par défaut :
 - pinheader est une bibliothèque dédiée aux picots 2.54mm (utilisés partout, par exemple pour les connexions des Arduino, Beaglebone, Raspberry...)
 - Eurobot : une bibliothèque un peu fourre-tout contenant des composants utilisées, actuellement ou pour des versions passées, pour la Coupe de France.

Pour ajouter ces bibliothèques à Designspark : cliquez sur l'icon Libraries (Ctrl + L), Folders, Add et ajoutez le chemin vers ce dossier.

### BeageboneCape

Le schéma (BeagleboneCape.sch) et le layout (BeagleboneCape.pcb) d'un cape couvrant la Beaglebone (l'équivalent d'un shield pour l'Arduino). Ce circuit est responsable de toute l'interface entre cette carte et le reste du robot, son role est notamment de limiter au maximum le cablage et les jonctions en l'air.
Note : c'est actuellement la version précédente (2017) du circuit qui est présente.
