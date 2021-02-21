# 1.6. You only return once

10 päivän hiljaiselon jälkeen, on taas aika palata kääntäjätekniikan maailmaan. Vähän
ikävä ja samalla odottamaton tuo 10 päivän tauko, mutta minkäs teet. Ensimmäisenä olikin
ihan hyvä vaihtoehto keskittyä helppojen hedelmien poimimiseen, eli tehtiin tulkille
evaluointifunktioita ja samalla vähän katseltiin, että missä mennään. Huomaa kyllä, että
koodi tuli kirjoitettua noin viikossa yhtenä jatkuvana ajatuksen virtana. Lisäksi moni
asia siinä lähtökohtaisessa ajattelutavassakin on jo hieman muuttunut.


## Kääntäjätekniikat vs kääntäjä

Kurssin tavoitteenahan on opetella kääntäjätekniikoihin liittyviä asioita, eikä suinkaan
ainoastaan kirjoittaa kääntäjää ohjelmointikielelle. Oma focus on todella lujaa ollut 
kiinni tässä ajatuksessa, että kirjoitetaan ennen kaikkea kääntäjää. Tämä on kuitenkin
jo näin alkuvaiheessa osottautunut valtavaksi ongelmaksi, kun kyseessä on TODELLA laaja
kokonaisuus verrattuna syksyllä tehtyyn tulkkiin ja tähän lisättynä se, että seikkailen
käytännössä itselleni uuden ohjelmointikielen maailmassa. Tämä saikin minut lopulta
tarkastelemaan kokonaisuutta sillä silmällä, että nyt on aika suoraviivaistaa asioita
isolla kädellä, jotta pääsen oikeasti eteenpäin.


## YORO

Käytin jonkin verran aikaa siihen, että pohdin sallitaanko ne yksi vai useampi paluulause
funktiosta/alirutiinista. Tämä oli oikeastaan se käännekohta kun ymmärsin sen loputtoman 
suon jossa kohta ollaan ja että nyt on aika suoraviivaistaa asioita. Vaikka en ole ideoitani
kovinkaan tarkasti suunnitelmaan kirjannut, niin minun ideani siitä, mitä ominaisuuksia
kieli omistaa, on niitä kuitenkin suoraviivaistettava. Vaikka alkuun ajattelin tekeväni
suoraviivaisempaa ja helpompaa kieltä, kuin syksyllä, niin asia ei nyt tosiaankaan ole aivan
niin.

Vaikka tämä sallittujen paluulauseiden lukumäärän pohtiminen oli ohjelmointikielten 
periaatteita ajatellen oikein tärkeä asia, jota en ole koskaan aikaisemmin edes tietoisesti
pohtinut, ei tässä pitäisi kuitenkaan olla kahta sanaa sen suhteen, mitä toteutetaan.
Tietenkin se yksinkertaisempi vaihtoehto toteutetaan. Vastaavasti ajatukseni vapaamielisesti
esiteltävistä muuttujista, tulee tuottamaan vain enemmän päänvaivaa kuin mitää muuta,
tullaan tämäkin muuttamaan muotoon, jossa muuttuja tulee esitellä ennen käyttöä, niin
globaalilla kuin lokaalillakin tasolla. Lokaalilla tasolla tämä nyt on melko itsestäänselvyys,
mutta globaalilla tasolla tällä nyt ei pitäisi olla mitään väliä. Esim.

```
foo: int = bar;
bar: int = 42;
```

Pitäisi olla aivan sallittua globaalilla tasolla. Tätä ei nyt kuitenkaan aleta toteuttamaan.


## Hengähdysviikko

Aikataulullisesti ollaan jo vähintään viikko jäljessä. Ensimmäisen 1-2 viikon aikana oli
tarkoitus saada valmiiksi tuo pipeline lähdetiedostosta tulkattuun lopputulokseen.
Tämän jälkeen sitten pitää "hengähdysviikko" jonka aikana suunnittelisi sitten tuota
välikielen generointia ja kirjottaisi testejä jo tehdyille osille. Viikossa ei saatu
lopulta valmiiksi kuin selaaja ja jäsentäjä, sekä hieman aloiteltiin mallailemaan
tarkastajaa ja tulkkia. En nyt kuitenkaan ala liikaa hötkyilemään, koska tässä täytyy
nyt tosissaan palauttaa itseään kartalle tämän asian suhteen, joten pidän sen tietynlaisen
hengähdysviikon nyt tulevalla viikolla.

Ajatus viikon kulusta on sellainen, että käyn nykyisen kokonaisuuden läpi selaajasta lähtien
ja siistin sen mitä on siistittävää ja korjaan sen mitä on korjattavaa. Sen lisäksi, että
pääsen taas vahvasti sisään tähän itse työhön ja aiheeseen, viikon lopussa minulla on sitten 
valmiina se kokonaisuus, joka nyt olisi pitänyt olla valmiina. Lisäksi viikon lopuksiolen 
täydentänyt suunnitelmaa/README:ta ajantasalle sen suhteen, että mitä nyt on tehty ja
mitä tullaan tekemään.
