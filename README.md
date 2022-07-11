# PS2_2021_Sfrangeu: Smart Home Control System using Arduino

<img src="https://github.com/BogdanSfrangeu/PS2-Project/blob/2ed52213b791911996e9ebbd889f0ca665510084/Proiect%20Final%20.png" width=100% height=100%>

**Se va realiza un sistem de conducere pe bază de micro-controller care va efectua următoarele funcții:**
- Citirea temperaturii sistemului de la distanță.
- Controlul luminilor LED de la distanță. ( 1 LED - control on/off), 1 LED RGB - controlul culorii.
- Transmiterea mesajelor prin internet către sistem. 
- Detecția și alertarea inundațiilor.
    
**Interfața locală**
- Sistemul va dispune de un LCD 16x2 pe care va fi afișat meniul, iar în modul blocat se vor afișa ora, temperatura curentă și numărul mesajelor necitite. (ex. “    18:02    “, “16°C - 2 mesaje”)
- Navigarea prin meniul echipamentului se va face prin patru butoane: “OK”, “Cancel”, “+”, “-”.
- Accesul în meniu se face pe baza unei parole prestabilite (ex. “--++--”, “+++---”)
- Meniul va permite citirea și ștergerea mesajelor primite precum și citirea evenimentelor de tip inundație.

**Interfața web**
- Sistemul va dispune de o interfață web ce va afișa temperatura curentă(citită de la un senzor LM35) și va oferi posibilitatea de transmitere a unui mesaj către sistem.
- Interfața va dispune de un buton de tip ON/OFF pentru controlul unui LED conectat la micro-controller.
- Evenimentele de tip inundație vor fi transmise prin e-mail la o adresa prestabilită.  

**Dezvoltarea Software**
- Se vor stoca în memoria nevolatilă ultimele 10 mesaje primite.
- Se vor stoca în memoria nevolatilă datele ultimelor 10 evenimente de tip inundație.
- Pentru conectarea la internet a sistemului se poate folosi o comunicație serială cu un PC sau o conexiune Wi-Fi/Ethernet/3G.
- Se pot folosi orice librării open-source.

**Teme opționale**
- Reglarea temperaturii de la distanță. Regulator format din senzorul de temperatură și o sursă de căldură controlată prin PWM.
- Stabilirea unui orar zilnic de funcționare din interfața web pentru luminile LED.

# Etape de dezvoltare:

1. LCD
    - Descriere: Pe LCDul sistemului (ex. LCD 16x2) se va afișa temperatura curenta de la un senzor de temperatura (ex. LM35).

2. Meniu
    - Descriere: Pe LCDul sistemului (ex. LCD 16x2) se va afișa un meniu conform schemei de mai jos, iar navigarea prin acesta se va face de la 4 butoane de tip push-button.
    
3. Mesaje
    - Descriere: Prin comunicație seriala de la un PC se transfera mesaje text către microcontroller, iar ultimele 10 mesaje vor fi stocate în memoria nevolatila. Acestea vor fi afișate inițial în meniul 1.1(mesaje necitite), iar în urma citirii unui mesaj acesta va fi mutat în meniul 1.2(mesaje citite). Ștergerea tuturor mesajelor salvate se poate face de la tastatura sistemului din meniul 1.3(ștergere). 

4. Jurnal evenimente
    - Descriere: Dezvoltarea și implementarea unui senzor de inundații. Se vor salva în memoria nevolatila data și ora ultimelor 10 evenimente de tip inundație. Aceste eveniment vor putea fi accesate și din meniul 4(inundații). De asemenea la detecția unui eveniment de tip inundație se va transmite un e-mail către o adresa prestabilita.

5. Interfața Web
    - Descriere: Implementarea serverului ce comunică cu microcontrollerul printr-o interfață web accesată din browser. Din această interfață se vor putea trimite mesaje către microcontroller (vezi 3), se va putea controla un LED prin acțiuni ON/OFF sau acțiuni  pentru alegerea culorii LED-ului RGB și de asemenea se va putea citi temperatura curentă a sistemului.

6. Control Acces (optional)
    - Descriere: În modul blocat LCD-ul va afișa ora, temperatura curentă și numărul mesajelor necitite. (ex. “    18:02    “, “16°C - 2 mesaje”). Accesul la meniu este permis după introducerea unei secvente de control (ex. “2 x OK, 2 x Back, 2 x OK”).
