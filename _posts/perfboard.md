Ah, je crois qu'il y a une confusion sur ce qu'est une perfboard — et c'est important. Si, tu y mets tous les petits composants : tu les soudes dessus. 🙂

Ce qu'est une perfboard
C'est une plaque pleine de trous (espacés de 2,54 mm) avec des pastilles de cuivre. Tu soudes tout dedans :

des supports (headers femelles) pour y enficher les modules (Teensy, driver, MP1584…) — amovibles,
les petits composants discrets soudés directement dans les trous : résistances/condos des filtres RC, découplage 100 nF, condo 470 µF, connecteurs, fusible, interrupteur,
les liaisons = des fils soudés au dos, point à point.
Donc la perfboard porte l'ensemble du circuit, pas seulement les modules.

Comment ça fonctionne, concrètement

   DESSUS (composants)              DESSOUS (câblage)
   ┌───────────────────┐           ┌───────────────────┐
   │ [Teensy sur support]│         │  fils soudés qui   │
   │ [Driver] [MP1584]   │   ←→    │  relient les points │
   │ R  C  (filtre RC)   │         │  (point à point)    │
   │ ▢ 470µF   ┃fusible  │         │                     │
   └───────────────────┘           └───────────────────┘
Tu plantes le composant par le dessus, ses pattes ressortent dessous.
Tu soudes les pattes aux pastilles.
Tu relies deux points en soudant un fil au dos (ou un pont de soudure).
Et les Dupont alors ?
Les fils Dupont servent surtout pour ce qui est hors de la carte :

les moteurs (sur le châssis),
la batterie,
les modules AS5600 des moustaches (montés sur le châssis, près des aimants). Ils se branchent à la perfboard via des connecteurs / headers.
Résumé de la chaîne complète

Batterie ─Dupont→ [perfboard: fusible, interrupteur, MP1584, 470µF,
                    Teensy, driver, filtres RC, découplage]
              │
    Dupont ───┼──→ Moteurs (châssis)
    Dupont ───┴──→ AS5600 ×2 (châssis, sous les aimants)
👉 La perfboard = ton robot en version "tout soudé mais bidouillable". Les petits composants y sont soudés, les modules s'enfichent, et Dupont relie les éléments mobiles (moteurs, batterie, moustaches).

💡 Si tu veux zéro soudure pour le tout premier test : une breadboard (platine sans soudure) accepte aussi les petits composants (on les enfonce). Mais c'est encombrant et ça bouge → pas pour un robot qui roule. La perfboard soudée, c'est le proto qui monte sur la souris.

Ça clarifie ? Veux-tu un petit schéma d'implantation (quoi mettre où sur la perfboard) pour ton cas précis ?

d:\github\ratatouille\_posts\2026-07-10-encoder-imu-fusion.md
et même le MP1584 qui est vraiment tout petit
Oui, même le MP1584 tout petit va sur la perfboard — il a juste 4 pastilles à relier, c'est tout.

Ce qu'a le MP1584
C'est un mini-module avec 4 trous/pastilles (souvent aux coins) :


   IN+  ──▢        ▢── OUT+
        [ MP1584 ]
   IN−  ──▢   🔧   ▢── OUT−
              ↑ trimmer (petit potentiomètre)
IN+ / IN− = entrée (7,4 V de la batterie)
OUT+ / OUT− = sortie (5 V vers le Teensy VIN)
Comment le poser sur la perfboard
Comme ses 4 pastilles ne sont pas alignées au pas 2,54 mm (contrairement au Teensy), tu ne l'« enfiches » pas. À la place :

soit tu soudes 4 petits fils de ses pastilles vers 4 trous de la perfboard,
soit tu le fixes à côté (scotch double-face / colle) et tu soudes 4 fils vers le circuit.
Il est minuscule et léger → 4 fils suffisent, il « flotte » à côté sans souci.

⚠️ LE point critique avant de brancher
Le MP1584 a un petit trimmer (vis) qui règle sa tension de sortie, et à la sortie d'usine elle n'est PAS à 5 V (souvent bien plus haut).

👉 Avant de le relier au Teensy :

Branche seulement l'entrée (7,4 V) et un multimètre sur OUT+/OUT−.
Tourne la vis jusqu'à lire 5,0 V pile.
Ensuite seulement, connecte OUT au VIN du Teensy.
Si tu sautes cette étape et qu'il sort du 12 V, tu grilles le Teensy instantanément (rappelle-toi : VIN max 5,5 V dans ta BOM).

Résumé
Le MP1584 = 4 fils soudés vers la perfboard, posé à côté. Sa petite taille n'est pas un problème.
Règle-le à 5 V au multimètre AVANT de toucher au Teensy.
💡 Astuce : une fois réglé à 5 V, mets une goutte de vernis à ongles sur le trimmer pour qu'il ne bouge pas avec les vibrations du robot.

Tu veux que je note ce « régler MP1584 à 5 V avant branchement » comme ⚠️ dans la BOM (section Brain & power) ? 