---
layout: post
title: Takeaways, component
date: 2026-07-08 10:00:00 +0200
tags: [electronics]
---

⚠️ pour ne pas tout griller on va vérifier la sorite réel de chaque composant du circuit avant de tout relier ensemble

info 
info qui agit = ordre
puissance
les communications : A, V

ce qui passe dans les fils = A & V, qui est le power & l'information ?

couleur fil :
. noir- /rouge+
. autre couleur pour les commandes

. Pour chaque composant :
  une photo avec sur la photo situer l'in/out avec les cristères
  in-/in+ : 1 ou + à respecter pour ne pas griller le truc
  out-/out+ : 1 ou + attention au prochain composant
  une photo de la vérif avec un multimètre
  quel composant avant et quel composant aprés

. Faire un cercle avec les composant pour avoir une vue d'ensembl

🚧 : puissance vs signal
 le signal et la puissance sont deux mondes séparés, avec chacun sa tension.
. garder l'analogie de l'eau et présenter tous les composants
. c'est quoi input et output, l'unité, les valeurs
. avec quel autre composant il est connecté

A confusion I had: in the chain **Teensy → driver → motor**, what does the **battery** send? A PWM too?

## No — the battery sends *power*, not a *signal*

The battery gives **constant DC power**: **7.4 V, all the time**. It's raw energy,
not information — it never "blinks" and never "says" anything.

Two different things reach the driver:

```
POWER :  Battery 7.4 V (constant) ─────────────┐
                                                ▼
                                        [ DRIVER TB6612FNG ] ──► Motor
                                                ▲
SIGNAL : Teensy ── PWM 3.3 V (on/off) ──────────┘
```

- **Battery → power**: 7.4 V constant, big current available. *(the energy)*
- **Teensy → signal**: PWM at 3.3 V, tiny current. *(the order)*
- **Driver**: takes the battery's 7.4 V and **chops** it on/off, following the
  Teensy's PWM order. The motor gets **chopped** 7.4 V.

## Water analogy 💧

- **Battery** = the pressurised **tank** (always full, constant).
- **Teensy (PWM)** = the **hand** on the tap, saying "open / close" very fast.
- **Driver** = the **tap** that opens and closes, following the hand.

The tank doesn't decide anything — it just supplies water. The hand commands, the
tap executes.

## Takeaway

The battery sends power (constant 7.4 V), not a signal. The only signal (PWM) comes from the Teensy. The driver marries the two.


Pourquoi 2 tensions différentes
3,3 V = la tension du signal (l'ordre). Le Teensy « parle » en 3,3 V (c'est son niveau logique). Un signal a juste besoin d'être « haut » ou « bas » → 3,3 V suffit pour dire « on ». Ça ne transporte quasi aucune énergie (courant minuscule).
7,4 V = la tension de la puissance (l'énergie). Les moteurs, eux, ont besoin de force pour tourner → il leur faut la vraie puissance de la batterie (7,4 V, gros courant).
Le driver fait le pont entre les deux
Le TB6612FNG a exprès deux entrées séparées :

VCC = côté logique, alimenté en 3,3 V (il écoute l'ordre du Teensy).
VM = côté moteur, alimenté en 7,4 V (la puissance de la batterie).

Il lit le petit signal 3,3 V et l'utilise pour commuter le gros 7,4 V vers le moteur. Le 3,3 V ne « devient » jamais du 7,4 V : ce sont deux circuits différents que le driver relie.

L'analogie de l'interrupteur 💡
Pense à un interrupteur mural :

Ton doigt (faible, sans danger) appuie sur l'interrupteur = le signal 3,3 V.
La lampe est alimentée par le 230 V du mur = la puissance 7,4 V.
Ton doigt ne transporte pas le 230 V ! Il commande juste. L'interrupteur (= le driver) relie ta commande faible au circuit puissant.

En une phrase
Un signal n'a pas besoin d'avoir la même tension que la puissance qu'il commande. Le Teensy commande en 3,3 V, le driver commute la puissance en 7,4 V vers le moteur. Deux côtés, deux tensions, un seul composant qui les relie.

💡 C'est exactement ce que dit ta BOM : « logic VCC 2.7–5.5 V → 3.3 V from Teensy » (le signal) et « VM 2.7–13.5 V from 7.4 V » (la puissance).


(1) BRAIN

**Power path** (energy, thick wires) — with the current in each wire:
```
LiPo 2S 7.4 V ─► Fuse ─► Switch ─┬─► TB6612FNG (VM) ─► Motors ×2      (~3.0 A peak)
  (~3.1 A peak total)            │
                                 └─► MP1584 ─ 5 V ─► Teensy (VIN)     (~0.10 A @ 5 V)
                                                      └─► LDO 3.3 V ─► AS5600 ×2 + MPU-6050 + encoders   (~54 mA)
```
> The current in a wire = **what the downstream asks for**, not what the part itself uses.
> So the fuse + switch + top wires carry the **full ~3 A motor peak** (→ fuse 2 A slow-blow, switch ≥ 3 A, thick wire), while the Teensy/sensor branch is tiny.
> ⚠️ The **MP1584 changes the current**: power is conserved, so `7.4 V × I_in ≈ 5 V × I_out` → the ~0.10 A drawn at 5 V is only ~0.07 A pulled from the 7.4 V rail.

**Signal path** (orders, tiny current):
```
Teensy ─ PWM + direction (3.3 V) ─► TB6612FNG      (tells the motors how fast)
AS5600 ×2 ─ analog OUT (0–3.3 V) ─► Teensy ADC     (whisker angle)
MPU-6050 ─ I2C (3.3 V) ──────────► Teensy          (heading)
Encoders ─ quadrature (3.3 V) ───► Teensy          (distance)
```

### MOUVEMENT

2️⃣ « STBY must be HIGH »
STBY = Standby = veille. C'est une broche de contrôle du driver :

STBY = LOW (0 V) → le driver dort : peu importe tes PWM, les moteurs ne tournent pas.
STBY = HIGH (3,3 V) → le driver est réveillé et actif.
👉 Dans ton code, tu dois mettre cette broche à HIGH (digitalWrite(STBY, HIGH);) avant de commander les moteurs.

Pourquoi c'est dans les ⚠️ limites : c'est LE piège n°1 avec ce driver. Des débutants perdent des heures — tout est bien câblé, le PWM est envoyé, mais rien ne bouge… parce que STBY est resté à LOW. Le driver reste en veille.

Les MOTEURS =
6 V = la tension de fonctionnement.
~50–100:1 = le réducteur (échange vitesse ↔ force ; à fixer, ça impacte l'odométrie).
hall encoder = le compteur de tours magnétique intégré, qui donne la distance parcourue.

Torque = couple = la force de rotation que le moteur produit. C'est son travail principal : il envoie ce couple aux roues pour faire avancer le robot.
→ C'est le côté puissance (l'énergie qui bouge le robot).

& quadrature 3.3 V → Teensy » — la sortie signal (retour d'info)
En même temps, l'encodeur intégré (le hall encoder) produit un signal qu'il renvoie au Teensy.
quadrature = le type de signal : 2 canaux (A et B), décalés l'un de l'autre. Ça permet 2 choses :
compter les impulsions → de combien la roue a tourné (distance),
connaître le sens de rotation (avant/arrière) grâce au décalage A/B.
3,3 V = c'est un signal logique en 3,3 V (compatible Teensy).
→ Teensy = il part vers le Teensy pour l'odométrie.

Le point clé : un moteur = 2 sorties
Sortie	Nature	Va vers
torque (couple)	puissance (mécanique)	les roues
quadrature (impulsions)	signal (information)	le Teensy

**Power path** (energy):
```
Switch 7.4 V ─► TB6612FNG (VM) ─ chopped 7.4 V ─► N20 motors ×2 ─► Wheels Ø32 mm ─► motion
```

**Signal path** (orders & feedback):
```
Teensy ─ 1 PWM + 2 direction pins (3.3 V) ─► TB6612FNG   (how fast, which way)
N20 encoders ─ quadrature (3.3 V) ─────────► Teensy       (distance + speed back)
```


### MOUSTACHE
**Sensing path** (mechanical → magnetic → electrical):
```
Wall ─► tip ─► rod ─► pivot (MR63) ─► magnet Ø6 ─► AS5600 ─ analog 0–3.3 V ─► Teensy ADC
                        ▲
                 spring + rubber (pulls back to centre)
```

**Power path** (energy):
```
Teensy onboard 3.3 V ─► AS5600 ×2   (0.03 A total)
```

Bore = l'alésage = le diamètre du trou central d'un roulement (le trou par où passe l'axe).

Sur ton MR63
Rappelle-toi les 3 dimensions du roulement MR63 = 3 × 6 × 2,5 mm :


        ┌─────────┐
        │  ╭───╮  │   ← 6 mm = diamètre extérieur
        │  │ ● │  │   ← 3 mm = bore (trou intérieur)
        │  ╰───╯  │
        └─────────┘
        épaisseur 2,5 mm
bore = 3 mm = le trou au centre.

magnet = 0.5–3 mm above the chip centre


### ODOMETRIE
**Sensing path** (what comes back to the brain):
```
Wheel rotation ─► encoders ─ quadrature (3.3 V) ─► Teensy   (distance + speed)
Robot rotation ─► MPU-6050 gyro ─ I2C (3.3 V) ───► Teensy   (heading)
```

**Power path** (energy):
```
Teensy onboard 3.3 V ─► encoders ×2 + MPU-6050   (0.024 A total)
```

3️⃣ « the gyro drifts slowly over time » — le point le plus subtil
Le gyroscope ne mesure pas un angle, il mesure une vitesse de rotation (des degrés par seconde, °/s).

Pour obtenir le cap (l'angle), il faut additionner cette vitesse au fil du temps (l'intégrer) :


cap = somme de (vitesse × petit temps), en continu
Le problème : le gyro a un minuscule biais (il lit « 0,1 °/s » alors que le robot est immobile). Ce petit défaut, additionné sans arrêt, s'accumule :


0,1 °/s pendant 60 s  →  erreur de 6°  →  au bout de quelques minutes, le cap est faux
C'est la dérive (drift) : même robot à l'arrêt, la valeur de cap s'éloigne lentement de la vérité.

Le lien avec ton projet
C'est le défaut du gyro, à l'opposé du défaut de l'encodeur :

Capteur	Donne	Défaut
Encodeur	distance	ment au patinage (roue qui glisse)
Gyro (MPU-6050)	cap	dérive dans le temps (biais accumulé)
→ Les deux se compensent → c'est exactement pourquoi tu les fusionnes (ton post « Where is Ratatouille ? »).

En une phrase
I2C only = ce capteur se câble en I2C (2 fils SDA/SCL), pas en SPI.
feed 3.3 V = l'alimenter en 3,3 V pour que ses signaux ne grillent pas le Teensy.
drifts over time = le gyro mesure une vitesse qu'on additionne pour avoir l'angle → un petit biais s'accumule, donc le cap dérive lentement → d'où la fusion avec l'encodeur.

#### CLEAN

**Noise path** (where each defense sits):
```
Motors ── current spikes ──► VM rail ──[ 470 µF ]── absorbs the spikes
AS5600 ── noisy analog ────► [ 1 kΩ + 100 nF ] ───► clean analog ──► Teensy ADC
3.3 V rail ────────────────► [ 100 nF ] at each chip's VDD/GND pins
All GND returns ───────────► one single star point (motor GND never through sensor GND)
```

#### CHASSIS

**Assembly path** (what holds what):
```
Chassis (PLA)
   ├─► Motor brackets ×2 ─► N20 motors ─► wheels
   ├─► Whisker module ×2 ─► pivot + magnet + AS5600
   ├─► Perfboard ─► Teensy, driver, MP1584, capacitors
   └─► Rear skid
        (all fixed with M2 screws + nyloc nuts)
```


### coherence

**Power chain**

```
Battery (LiPo 2S) ── 7.4 V ──► Fuse (2 A) ──► Switch ──┐
                                                       ├─► Driver (TB6612FNG, VM) ──► Motors ×2
                                                       └─► Regulator (MP1584) ── 5 V ──► MCU (Teensy 4.0, VIN)
                                                              └─► Teensy onboard LDO ── 3.3 V ──► AS5600 ×2, MPU-6050, encoders
```