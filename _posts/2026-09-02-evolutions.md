---
layout: post
title: Évolutions
date: 2026-09-02 10:00:00 +0200
tags: [roadmap]
---

* TOC
{:toc}

🚧 todo : liste des pistes v2, avec liens vers les posts d'origine plutôt que de dupliquer le contenu.

Teensy pins & bus I2C
Sur le Teensy, les pins sont dédiées à un seul composant, une seule fonction. Mais c'est différent sur le bus I2C = Inter-Integrated Circuit, c'est un protocole de communication série à 2 fils (SDA pin 18 = données, SCL pin 19 = horloge) qui permet à un microcontrôleur de parler à plusieurs composants sur les mêmes 2 pins, chaque composant identifié par une adresse unique (⚠️).