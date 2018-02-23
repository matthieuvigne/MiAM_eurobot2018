# ProgrammationBBB
Ce dossier contient le code de la Beaglebone embarquée sur le robot. Il contient un dossier BBBEurobot, contenant une
bibliothèque de fonctions utilitaires pour la Beaglebone, et d'un ensemble de projets indépendants, le plus souvent
utilisant cette bibliothèque :
 - TestADNS : un simple code de test du capteur de souris ADNS9800.


## Prérequis

Ce code est concu pour être cross-compilé (compilé par un PC pour une exécution par le processeur ARM de la Beaglebone).
Cross-compiler le code est en effet beaucoup plus pratique que devoir copier à chaque fois les sources sur la carte
pour les compiler, et permet de le faire même en l'absence de Beaglebone.
Pour cela, il est nécessaire d'avoir:
 - un cross-compilateur C vers arm, arm-linux-gnueabihf-gcc (à utiliser à la place du gcc installé sur votre ordinateur)
 - la glib-2.0 cross-compilé pour arm (avec le compilateur précédent). La [glib](https://developer.gnome.org/glib/) est 4
 une bibliothèque bas-niveau fournissant un ensemble de fonctions très diverses et très utiles ! L'utiliser permet de
 limiter les dépendances externes (c'est même la seule pour le moment).

Pour installer et configurer ces deux éléments, référez-vous au document SetupCrossCompilation.pdf.

*Note: a priori cette configuration fonctionnera aussi pour la Raspberry Pi si on développe aussi en C/C++. A vérifier...*

## La bibliothèque BBBEurobot

Lors du développement, plusieurs projets vont être ammenés à évoluer (par exemple, des codes de tests de capteurs
individuels) en plus du code principal pour la Coupe. Tous ces projets utilisent une grande partie de code commun, notamment
tout ce qui concerne la communication avec les différents capteurs et moteurs. Pour éviter de dupliquer le code, et donc
d'augmenter fortement la probabilité de bug, toutes ces fonctions sont rassembleés dans une bibliothèque, nommée
BBBEurobot, afin de les partager. Les fonctions dans cette bibliothèque doivent être le plus fiable possible...

Pour l'utiliser, il est nécessaire de compiler et d'installer cette bibliothèque. Pour cela, il faut choisir un chemin
d'installation : un chemin par défaut existe (souvent `/usr/local`). Pour ne pas mélanger cette bibliothèque avec le reste
de votre système (si vous voulez la supprimer facilement plus tard), je vous conseil de créer un dossier spécifique pour
l'y installer (par exemple dans `/home/<username>/eurobotInstall`) : soit `<installPath>` ce chemin d'installation.
Pour compiler et installer la bibliothèque, cmake est utilisé. Créez un répertoire `build` pour la compiler, puis faites:

`
cmake /path/to/BBBEurobot -DCMAKE_INSTALL_PREFIX=<installPath>
make
make install
`

La bibliothèque est désormais installée à l'endroit choisi. Cependant, pour pouvoir l'utiliser ailleurs, il faut indiquer
aux autres projets comment la trouver. Ceci est fait grace à l'utilitaire `pkg-config`, qui se base sur un fichier
`BBBEurobot.pc` installé avec la bibliothèque. La seule modification à faire est donc d'indiquer au système où se trouve
ce fichier (cette étape n'est pas nécessaire si la bibliothèque a été installée à l'emplacement par défaut du système) :
pour cela, il suffit d'ajouter ce chemin à la varible d'environement PKG_CONFIG_PATH :

`
gedit ~/.bashrc
`

Ajoutez à la fin de ce fichier la ligne :

`
export PKG_CONFIG_PATH=<installPath>/lib/pkgconfig
`

Il faut ensuite relancer le terminal pour que le changement prenne effet (ou bien exécuter `source ~/.bashrc`).

## Usage

BBB-Eurobot relies on a device tree overlay (DTO) file to configure the Beaglebone serial ports and GPIO.
This file is called Eurobot-00A0.dts, and can be found in the library folder (BBB-Eurobot).
To use is, copy this file to the Beaglebone, compile it and copy it to /lib/firmware.
dtc -O dtb -o Eurobot-00A0.dtbo -b 00 -@ Eurobot-00A0.dts
cp Eurobot-00A0.dtbo /lib/firmware
This file is persitent on the Beaglebone, so this only needs to be done once (as long as the file isn't updated.
