Ce programme permet de transformer une trame PPM de radiocommande RC (old school certes) en une trame série plus moderne.
Cette trame série peut ensuite être décodée par un controleur de vol pour drone par exemple.

La trame PPM est lu grace à la bibliothèque PPM-Reader 1.20, qui est limitée à 8 canaux. (testé avec 9, en effet on n'a pas la 9eme voie)

On a dans le code source le choix pour transmettre via l'UART de l'ARDUINO une trame au format
- Graupner SUMD (8 canaux)
- MULTIPLEX SRXL V1 (12 canaux)
- MULTIPLEX SRXL V2  (16 canaux)

Les canaux supplémentaires sont définis à 1500 µs (neutre) puisqu'ils ne sont pas transmis dans la trame PPM. Ils ne sont donc pas réellement utilisables.

Ce programme a été conçu pour pouvoir utiliser un récepteur ImmersionRC LongRange qui ne transmet que le signal PPP. Or ce signal n'est pas reconnu par iNav
Je voulais assurer une compatibilité avec les signaux d'un récepteur MLINK mais de même iNav ne gère plus ce protocole depuis la version 5. 
J'ai donc prévu qu'un signal SUMD soit disponible en sortie.

Le montage a été réalisé sur une carte Arduini Mini Pro (5V - 16 MHz)
La pin 2 (qui gère les interruptions) est utilisée pour le signal PPM en entrée)
L'UART TX est utilisé pour le signal de sortie
  
