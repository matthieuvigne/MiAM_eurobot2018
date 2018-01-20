# Dossier Electronique

Ce dossier contient les fichiers relatifs à l'électronique du robot : PCB et cablage.
Pour la réalisation de circuits imprimés, le logiciel utilisé est  [Designspark 8.0](https://www.rs-online.com/designspark/pcb-download-and-installation).
Attention, un fichier créé avec une version ultérieure de Designspark ne peut être lu par les versions précédentes. Attention donc à ne pas utiliser Designspark 8.1 (actuellement en beta).

### Libraries
Ce dossier contient deux bibliothèques de composants pour Designspark PCB, à ajouter aux composants présents par défaut :
 - pinheader est une bibliothèque dédiée aux picots 2.54mm (utilisés partout, par exemple pour les connexions des Arduino, Beaglebone, Raspberry...)
 - Eurobot : une bibliothèque un peu fourre-tout contenant des composants utilisées, actuellement ou pour des versions passées, pour la Coupe de France.

Pour ajouter ces bibliothèques à Designspark : cliquez sur l'icon Libraries (Ctrl + L), Folders, Add et ajoutez le chemin vers ce dossier.

### BeageboneCape

Le schéma (BeagleboneCape.sch) et le layout (BeagleboneCape.pcb) d'un cape couvrant la Beaglebone (l'équivalent d'un shield pour l'Arduino). Ce circuit est responsable de toute l'interface entre cette carte et le reste du robot (cablage, conversion de tension...)


### SensorDriver

Le schéma (SensorDriver.sch) et le layout (SensorDriver.pcb) d'une carte permettant de commander des servomoteurs à partir d´une liaison I2C. Cette carte se base sur le PCA9685 pour générer des impulsions de servo (avec une précision de 5us seulement). Le composant peut être alimenté en 3.3V ou 5V - une entrée 5V est alors nécessaire pour que le signal pour les servos soit à 5V. Cette carte inclus aussi son propre convertisseur de tension ajustable, pour passer d´une entrée batterie 12V à 7V. Le courant max est de 5A - 16 sorties servos sont exposées, mais utiliser autant de servos en même temps risque de dépasser ce seuil de courant : un régulateur externe serait alors nécessaire. 8 des sorties ont en effet une ligne Vin séparée par un cavalier, afin d´être facilement relié à une autre source d´alimentation , les 8 autres restant sur le régulateur embarqué.


### ArchitectureElec

Le schéma de cablage ¨haut niveau¨ du robot, représentant les liaisions entre les cartes (en terme de protocole de communication : chaque fil n´est pas représenté).
