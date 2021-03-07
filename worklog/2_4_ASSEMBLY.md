# 2.4. Assembly

Tässä kun alkaa pikkuhiljaa kääntäjän luomat asiat olemaan koko ajan lähempänä rautaa,
niin täytyypä hetki vähän pohtia tulevaa. Vähintäänkin tämän kirjoittamisen ajan.
Tajusin tuossa luodessani tuota pipelinea lähdetiedostosta lopputulokseksi, että minun
ajatukseni komentoriviargumenttien välittämisestä oli jäänyt vähän tulkkimaailmaan.
Jotenkin ajattelin päässäni, että komentoriviargumentit käsitellään heti kääntäjän
alussa, mutta eihän se ole lainkaan niin. Komentoriviargumenttien käsittelyhän tapahtuu
siellä assemblytasolla ikäänkuin automaationa, eikä meillä ole mitään hajua käyttäjän
antamista komentoriviargumenteista ennen ajoa.

Näin ollen tulin siihen lopputulokseen, että vaikka tässä on välikielikin melkolailla
vaiheessa (se ei esimerkiksi luo lokaaleja tilapäismuuttujia tai käytä symbolitauluja),
niin minun täytyy kyllä tässä kohtaa vähän tutustua tuohon assemblyyn niiltä osin, että
kuinka saan yleisesti sen ohjelman rungon aikaiseksi ja sitä kautta sitten luon siltaa
sen lopputuloksen ja nykyisen tilanteen välille. Joten vaikka hetki sitten höpöttelin,
että aikaisempi suunnitelmani siitä, että tehdään kääntämisen jokaisesta vaiheesta ikään
kuin vähän kerrallaan, olisi huono, niin nyt joudun toteamaan sen kyllä oikein hyväksi.

Nyt tehdään siis ensin luentovideoita mukaillen pari triviaalia ohjelmaa mallia AJK, 
jotka toteutetaan ihan linkitykseen saakka. Samalla tutustutaan hieman noiden ulkoisten
funktioden käyttämiseen, koska tässä kohtaa kyllä ehdottomasti käytän valmiita C-funktioita
elämääni helpottaakseni. Ensimmäiset ohjelmat olisi siis:

* pääohjelman palautusarvo tulostetaan aina ruudulle
* pääohjelma, joka palauttaa arvon esim. 0
* pääohjelma, joka palauttaa komentoriviargumenttien arvoja
* pääohjelma, joka tulostaa ruudulle syötetyt komentoriviargumentit

Uskoisin, että näillä saadaan se sellainen minimaalinen kehys luotua aivan hyvin ja
samalla saamme myös pienen tuntuman siihen sen kokonaisen pipelinen luomiseen. Tätä
kautta sitten näkee taas vähän paremmin, että mitä asioita täytyy ottaa huomioon
muissa vaiheissa. Ennen kaikkea välikielen generoinnissa. Seuraavakvis se onkin siis
koodigeneroinnin, assemblyn ja linkittämisen aika.

Tammikuun Assembly-leikeistä muistan yleisesti Assemblyyn liittyviä juttuja jonkin
verran, mutta se ei kuitenkaan ole aivan sama nyt sitten generoida Assemblyä
ohjelmoinnillisesti. Tässä kohtaa myös tajuan TODELLA hyvin sen välikielen roolin.
Tuosta välikielestä on todella paljon mukavampi alkaa rakentamaan sitä Assemblyä,
kun saadaan käsitellä samankaltaista lineaarista formaattia.
