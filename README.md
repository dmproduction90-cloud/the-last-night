# THE LAST NIGHT - Etape 1

Prototype PSP en C (PSPSDK/pspdev) : une maison, Ethan (modele provisoire en boites)
et une camera a la troisieme personne.

**Etat : code ecrit mais NON compile et NON teste.** Les premiers retours de
compilation serviront a corriger les erreurs eventuelles.

## Commandes
- Stick analogique : deplacer Ethan (a fond = courir)
- L / R : tourner la camera
- Carre : lampe torche provisoire (repousse le brouillard)

## Compiler depuis un telephone (GitHub Actions)
1. Creer un compte GitHub et un depot vide (ex. `the-last-night`).
2. Y envoyer tous les fichiers (a la racine du depot) : Makefile, main.c, gfx.c, gfx.h,
   world.c, world.h, player.c, player.h, camera.c, camera.h.
3. Creer le fichier `.github/workflows/build.yml` (bouton « Create new file », taper le chemin
   complet dans le nom) et y coller le contenu fourni.
4. Onglet **Actions** : le build demarre tout seul. S'il reussit, ouvrir l'execution et telecharger
   l'artefact **TheLastNight-PSP** (contient EBOOT.PBP).
5. Si le build echoue, copier les dernieres lignes rouges du journal et me les envoyer.

## Tester dans PPSSPP (telephone)
1. Dezipper l'artefact et placer `EBOOT.PBP` dans un dossier `TheLastNight`.
2. PPSSPP > onglet Jeux > ouvrir ce dossier > toucher l'icone.

## Compiler sur PC
Installer pspdev (https://pspdev.github.io), puis `make` dans le dossier.
