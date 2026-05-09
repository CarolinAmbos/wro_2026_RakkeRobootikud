# wro_2026_RakkeRobootikud
<div align="center"><img width="180" height="180" alt="OIP" src="https://github.com/user-attachments/assets/1443445b-a98f-4459-b23a-449399de9a8b" alt="alt text"></div>

## Roboti dokumentatsioon

---
## Info struktuur

* `code` - roboti kood
* `models` - roboti põhja laserlõikus fail ja 3D mudelid osadest
---

## Sisukord

- [Meie robot](#meie-robot)  
- [Roboti ehitus](#roboti-ehitus)  
- [Kood](#kood)  
- [Platsid](#platsid)  

---
## Meie robot

Andu (roboti nimi) on iseseisvalt sõitev Arduino kontrolleriga autonoomne robot, kes kasutab takistuste nägemiseks ühte Huskylens-i ja kahte ultra sonic sensorit. Huskylensiga jälgib Andu punast- ja rohelist torni ning jälgib ka jooni mattil, et õigel ajal keerata. Kahte ultra sonic sensoriga vaatab ta, et ta seina ei sõidaks.

Meie robot on lahe.

---
## Roboti ehitus

Alljärgnevalt on välja toodud erinevad roboti osad koos lahedate ilustreerivate pitidega.

### Roboti põhi

Andu põhja disaini inspiratsiooniks võtsime F1 autod kuna need on kiired, pööravad piisavalt hästi ja on lahedad.
Roboti põhjaks valisime 3mm akrüüli, mille lõikaśime välja laserlõikuriga. 
Falili lõikamiseks disainisime ja valmistasime ise. Väga lahe.

<img width="120" height="160" alt="671161621_1410152190862382_1362296107343678027_n" src="https://github.com/user-attachments/assets/008d98f4-a86f-4676-abd2-33cc9cea1f6b" />

### Roboti sõitmine
keeramiseks kasutame servo mootorit. Kahjuks pole teada selle allikat. Ikkagi on lahe.

<img width="120" height="160" alt="685604522_987521793944900_6633324361740464517_n" src="https://github.com/user-attachments/assets/4a3a4b3f-b79f-42fc-afdd-915e96e3883b" />

Robot kasutab sõitmiseks ühte DC mootorit, mille küljes on robootika musta ja kolast värvi rattad. Vahetamise ära ratta kummid, et velgesid kasutada, mis sobivad mootori ülekandele paremini. Vinge.

<img width="120" height="160" alt="670490473_1500242458431624_8726044778614137334_n" src="https://github.com/user-attachments/assets/f3e11e91-512e-46e6-8ec0-1c61d51dddc7" />

### Elektroonika kinnitus
elektroonika on roboti küljes velkroga, et me saaksime vajadusel need eemaldada. Väga äge lisa.

<img width="120" height="160" alt="683883200_1640995473895392_6839198560260087392_n" src="https://github.com/user-attachments/assets/702736c2-b459-4581-8926-b72d8bd888ed" />

### Takistuste nägemine
kaamera mida kasutame takistuste nägemiseks on huskylens kaamera, millele lisasime 3D prinditud kaane lootes et kaamera on seljuhul täpsem. Kahjuks see ei avaldanud piisavat mõju, kuid siiski otsustasime selle jätta kuna see näeb lahe välja.

<img width="120" height="160" alt="684203068_1957390701678020_2866031155544642137_n" src="https://github.com/user-attachments/assets/d3589d42-e404-46ac-8ab5-eb94f74f2e44" />

Seinte tuvastamiseks kasutame ultra sonic andureid, millel on ka kaaned. Kaaned on kassi kõrva kujulised ja me valisime just need kuna nendega oli hea sensoreid roboti külge lisada ja need näevad lahedad välja.

<img width="120" height="140" alt="690528576_1940574493260800_8525633376845302870_n" src="https://github.com/user-attachments/assets/4087414a-18b6-4833-8883-484ac21fbc10" />

### Energia allikas

Andu energia allikas on liitium patarei, mis on laaditav. Lahe.

<img width="120" height="160" alt="685454294_1702758247567863_4734858499360150202_n" src="https://github.com/user-attachments/assets/dd6d665b-4174-4588-ae54-3fff884823ce" />

ROBOTI PILDID:

<img width="120" height="160" alt="671161621_1410152190862382_1362296107343678027_n" src="https://github.com/user-attachments/assets/9a9f98a5-b68e-4d6a-978f-80f85bed622c" />
<img width="120" height="160" alt="694320351_847030255109772_6624946750258791365_n" src="https://github.com/user-attachments/assets/195f1810-2407-4247-9ab0-9c3db3caf031" />
<img width="120" height="160" alt="694780677_1391318369683750_4913826632039349728_n" src="https://github.com/user-attachments/assets/d4359eeb-930a-4226-9a0d-7d9ee424cc28" />
<img width="120" height="160" alt="688959090_951857090819986_1265937365051198215_n" src="https://github.com/user-attachments/assets/27a2f238-5459-4eb5-94f4-8e8298c5b9e5" />

---

## Kood

Meie kood on kirjutatud Arduino id ja programeerimis keeleks on C++. Alustasime Huskylens-i tööle saamisega. Katsetasime kuidas tema süsteem töötab ja uurisime, kas Huskylens sobib arduinoga. Seejärel
coodi loogika:
<img width="120" height="160" alt="692331747_1239695948017261_4997291549142741630_n" src="https://github.com/user-attachments/assets/e1e68d34-452c-4ff6-948a-7e776e2f20bb" />

---

## Platsid

Aja nappuse ja vahendite puuduse tõttu kasutasime oma loovust ja käepäraseid vahendeid, et ise ehitada endale harjutamiseks sõidu platsid. Ehitasime neid lausa kahte erinevasse asukohta, kus tegutseme, et maksimaalselt ära kasutada meie aega ja soovi robotit katsetada.

Plats 1 - Lahu kool

<img width="120" height="213,25" alt="674908625_986323177073322_5922061411484058443_n" src="https://github.com/user-attachments/assets/13cf3a90-2827-4788-93da-70838a753ec6" />

Plats 2 - Koeru Keskkool

<img width="120" height="160" alt="689525032_1342303084378687_1405793184664111852_n" src="https://github.com/user-attachments/assets/21078264-5b34-45c5-a72a-df597a8e7efc" />
