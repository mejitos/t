# 2.2. Välikielenhuoltoa


Tämä välikieli on tosiaankin vallan täysin uusi kokonaisuus tässä viime syksynä alkaneessa
kääntäjätekniikan matkassa. Näin hyvin nukutun yön (=päivän) jälkeen tajusin heti ainakin
yhden asian, jonka olen ymmärtänyt täysin väärin ja sen takia asiat tuntui vaikealta.
Kolmiosoitekoodin esitykseen kun on näitä erinäisiä muotoja ja ainoa oikea olemassa oleva
ohje kolmiosoitekoodiin on, että koodissa ei saa olla enempää kuin kolme operandia per
ohje. Tämän voidaan sitten tietenkin esittää muodoissa, joissa on 0-3 operandia tai
osoitetta. Minulla on mennyt kolmikot ja nelikot aivan täysin sekaisin koko viime viikon
ajan, kun olen asiaan perehtynyt. 

Toinen asia, jonka tajusin eilisen kirjoitteluni jälkeen, niin minullahan on tosiaan 
täysi vapaus oman välikielen luomisessa. Näin ollen se pään raapiminen jonkun Dragon
Bookin tai muiden surkeiden esimerkkien pohjalta, saa loppua. Niin kauan, kuin minun
välikieleni seuraa tuota edellä mainittua yksinkertaista ohjetta (=maksimissaan kolme
operandia / osoitetta per ohje), niin voin tehdä oikeastaan mitän vain.

Välikielikin varmaan tarvitsisi jonkinsorttisen formaalin kieliopin?


## Symbolitaulut

Tässä välikieltä ihmetellessäni tein myös toisen päätöksen miettiessäni seuraavia
askeleita. Välikieltä (kuten myös tarkastajan viimeistelyä) ajatellen, symbolitaulu
sekä näkyvyysalueet pitäisi saada käyttöön täydellä voimalla. Päätin, että minulla
on alkuun käytössä vain kaksi näkyvyysaluetta: globaali ja funktion sisäinen lokaali.
Näin ollen tulee vähän erikoinen järjestely, jossa globaali näkyvyysalue on leksikaalinen
näkyvyysalue ja funktioiden sisäiset näkyvyysalueet ovat dynaamisia näkyvyysalueita.
Tämä yksinkertaistus auttaa kuitenkin laittamaan todella paljon mutkia suoraksi,
säilyttäen kuitenkin kurssin tavoitteet, jonka mukaan jokaisella aliohjelmalla tulee
olla oma lokaali näkyvyysalueensa. Myöhempänä tavoitteena on sitten lisätä tähänkin
kieleen leksikaaliset näkyvyysalueet kaikkialle.

Toinen iso ratkaisu symbolitaulujen kanssa liittyy välikieleen. Olen tässä taas yhden
yön pähkäillyt ja selvitellyt asioita sekä tietenkin selaillut tonneittain kirjallisuutta.
Näin sivuhuomiona: huomasin, että loppujen lopuksi aika moni aihealueen kirja/paperi 
on käytännössä vain uudelleenmuotoiltu kopio Dragon Bookista. Niin tai näin, nämä
kirjoissa esitetyt vajaat esimerkit ovat todella surkeita ja ne lähes poikkeuksetta
kannustavat jopa luomaan suoraan erillisiä attribuutteja suoraan syntaksipuun solmuihin
välikielen generoinnin yhteydessä. Siinä kohtaa totesin, että näissä esimerkeissä
ei ole mitään tolkkua, koska minun silmissäni tämä välikielen generointi on nimenomaan
tapa päästä eroon siitä syntaksipuusta ja puurakenteesta ja siten lähemmäksi sitä
kohdekoneen assemblyä. Näin ollen tavoitteeni on symbolitaulujen lopullisen toteutuksen
yhteydessä myös saada välikielen generointi tapahtumaan siten, että syntaksipuu voidaan
käytännössä hylätä kyseisen prosessin yhteydessä. Kaikki tarvittava tieto tarvittavista
solmuista löytyy sitten symbolitaulusta, mikäli kolmiosoitekoodit itsessään ei 
tarvittavaa infoa omista.


## Hajautustaulu 

Nykyinen symbolitaulu on toteutettu ihan vain raakana taulukkona, josta iteroidaan
haluttuja tuloksia merkkijonoja vertailemalla. Tämä on kuitenkin aika huono ratkaisu,
joten ajattelin toteuttaa oman yleisemmön hajautustaulun. Tulee taas algoritmikurssien
oppeja hyödynnettyä ihan "oikeassa elämässä". Kuten minun toteutus yleiselle dynamiselle
taulukolle, tulee myös tämä toteutus olemaan melko yksinkertainen ja suoraviivainen.
Valitaan vain sopiva täyttösuhde esim 0.7-0.75, hajautusfunktioksi jokin simppeli 
funktio esimerkiksi FNV (Fowler-Noll-Vo) 1 tai FNV-1a ja törmäykset käsitellään 
avoimella osoitteenmuodostuksella. Eiköhän se aika nopeasti tule tehtyä. Nämä omat
tietorakenteetkin olisi varmaan hyvä testata jollain tapaa.
