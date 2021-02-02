# 1.1. Alkusysäys

Lyötiin projekti käyntiin selaajan tekemisellä. Melko paljon veivannut selaajaan liittyvää
asiaa, kun ensinnäkin syksyllä tulkkiprojektin aikana tein mokoman ensimmäistä kertaa, niin
joululomalla tuli harrastelun vuoksi suoriteltua MOOC Haskell kurssi ja sen oppien mukaan
tein pienehkön selaajan Haskellilla, joten melko hyvin alkaa olemaan hallussa tämän selaajan
toteuttaminen. Tämän osan tekeminen on sujunut oikeastaan juuri niin mukavasti kuin ajattelinkin 
sen sujuvan. Netflixiä katsellessa, melko olemattomalla aivotoiminnalla on saanut asioita aikaiseksi.

Yleisesti koodi on todella suoraviivaista ja "tyhmää", mutta se on oikeastaan se tarkoituskin.
En tässä kohtaa ala tekemään mitään hienoja temppuja, koska A: en osaa C-kielellä juurikaan
hienoja temppuja ja B: nyt on tärkeämpää saada ymmärrettävää tavaraa nopeasti, joten tämä
toimii oikein hyvin näin. Huomasin, että pikkuhiljaa kaikki selaajan varsinainen toiminnallisuus
alkoi valumaan switch-casen sisään, päätin pyrkiä ratkaisuun, jossa oikeastaan koko funktio
löytyy kyseisen switch-casen sisältä. Ihan vaan, koska miksi ei.


## Muistinhallinta

Varsinaista muistinhallintastrategiaa ei tällä hetkellä ole. Käytän hyvin tavallisia K&R
kirjasta poimittuja allokointifunktioita ja vapautus tapahtuu free-funktiolla. Olen kyllä
tutustunut eri näköisiin strategioihin kuten arena-allocatoreihin, mutta ihan ei kyllä vielä
ymmärrys riitä sellaisten käyttämiseen järkevästi oikeassa projektissa, joten parempi antaa
olla.


## Kokonaislukujen skannaaminen

Tuntui jotenkin tylsältä käyttää konversiofunktiota, joten halusin jonkin vaihtoehtoisen
ratkaisun. Muistinkin hyvin nopeasti algoritmin jota käytin hiljan assemblyn kanssa pelatessani.
Assemblyssä kun ei lukujen ruudulle tulostaminen ole niin itsestäänselvä asia. Käyttämäni
algoritmi muuntaa ensin kirjaimen vastaavaksi numeroksi kikalla, jossa kirjaimesta vähennetään
kirjaimen '0' arvo, joka ASCII-taulun mukaan, kääntää numerokirjaimen vastaavaksi numeroksi.
Tämän jälkeen luku kerrotaan 10:llä, jolloin aikaisemmin saatu luku siirtyy pykälän
vasemmalle, ja tämän jälkeen voimme lisätä tähän uuden luvun, jolloin luku päivittyy.

Parsitaan esimerkiksi vaikka merkkijono "123" kokonaisluvuksi.

```
int value = 0;          // value = 0
int digit = '1' - '0';  // 49 - 48 = 1

value *= 10;            // value = 0
value += digit          // value = 1

// seuraava kirjain

digit = '2' - '0';      // 50 - 48 = 2
value *= 10;            // value = 10
value += 2;             // value = 12

// seuraava kirjain

digit = '3' - '0';      // 51 - 48 = 3
value *= 10;            // value = 120
value += 3;             // value 123
```

Ja näin muuttui merkkijono "123", kokonaisluvuksi 123.

### Kokonaisluvun ylivuoto

Ylivuoto tarkastetaan myös luvun skannaamisen yhteydessä eli pidetään huoli ettei käyttäjän
koodiin kirjoittama luku ole sallittua isompi. Tätä kirjoittaessani mietin kyllä, että tuo
tavallinen int-tyyppi on kyllä vähän turhan pieni, joten tässä voisi oikeastaan ehkä käyttää
`uint64_t` tyyppiä.


## Tunnisteet ja avainsanat

Niinpä oikeastaan tullaankin asiaan, jota joutui hetken pohtimaan, eli kuinka hoidetaan
tunnisteiden ja avainsanojen skannaaminen tilanteessa, jossa halutaan säilyttää se yksi
valtava switch-case. Tässä kun tulee se, että jokainen löydetty lekseemi voi olla joko
avainsana tai sitten vapaasti muotoiltu tunniste. Näin ollen saman koodin copy-pasteaminen
+50 kertaa ei oikein toimi. Tätä kirjoittaessani mietin sellaista ratkaisua, että tekisi
apufunktion tunnisteen skannaamiselle ja jonkunsorttisen trie-rakenteen avainsanojen
etsimiselle. Eli eri casejen kautta pystytään kulkemaan avainsanan merkkejä yksitellen
eteenpäin ja jos löytyy avainsana => luodaan avainsana, muussa tapauksessa luodaan tunniste.

### Huono idea

Edellä mainittu suunnitelma oli aika hölmö, koska siinä ikäänkuin koitetaan koko ajan "lotota"
avainsanaa, ja mikäli avainsanaa ei löydy, palataan tavallaan n-määrä kirjaimia taaksepäin
ja sitten skannataan tunniste. Päädyin lopulta suoraviivaisimpaan toteutukseen, eli ensin
skannataan lekseemi ja sitten trie-rakenteen avulla tarkastetaan onko kyseessä avainsana
ja jos ei ole => kyseessä on tunniste. Nykyinen ratkaisu on todellakin suoraviivainen ja
jahka tämä vaihe saadaan päätökseen ja päästään hengähdysvaiheeseen, niin tuo on varmaan
ensimmäisiä asioita jota korjaan selaajasta. Sen verran hirveä se on.


## Päivä paketissa

Ensimmäinen kunnon koodauspäivä paketissa, ja kyllähän sitä saavutettiin aivan riittävästi
tälle päivää. Enemmänkin olisi voinut saada aikaiseksi, jos ei olisi katsellut elokuvia
koko aikaa samalla, mutta tämä osa nyt oli muutenkin sellainen, että tähän hätään se piti
vain saada pois alta, eikä sen takia aivan 100% aivotyötä sille jaksanut antaa. Siivota
ehtii myöhemminkin ja oikeastaan nyt selaaja on sellaisessa vaiheessa, että jäljellä ei
oikeastaan olekaan kuin sitä siivoilua ja pientä lisätestailua ennen jäsentäjän kirjoittamista.


## Jäsentäjä ja syntaksipuu

Jäsentäjä tulee todennäköisesti menemään vielä suoraviivaisemmalla tavalla kuin selaaja.
Selaajassa sentää tuli käytettyä aika paljon uutta opittua asiaa mitä tulee C-kieleen, joten
ei tullut kopioitua käytännössä mitään syksyn tulkkiprojektistakaan, eikä se ei olisi edes
mahdollista kielten välisten suuren eron takia. Jäsentäjä sen sijaan on aivan eri asia.
Sen pystyn todennäköisesti kirjoittamaan hyvin pitkälti side by side tulkin jäsentäjän
kanssa, mutta koitan ensisijaisesti silti kirjoittaa mokoman ihan ulkomuistista, jotta saa
toistoja ja tuntumaa tällaisten juttujen veivaamiseen.

Sama juttu oikeastaan tuon syntaksipuun kanssa. Tulkkiprojektissa on melko toimivat
olemassa oleva abstraktiot, niin suotta alkaa hirveästi niitä miettimään uudelleen. 
Joitain muutoksia täytyy tietty tehdä esim. unioneja tulee varmasti käytettyä, jolloin
asioita saa yhden tietueen sisään paremmin. Lisäski toisin kuin tulkkiprojektissa, nyt
noille enumeraatioilla on jotain oikeaa käyttöäkin.

Jahka selaajasta saa pahimmat roskat siivottua, niin eipä siinä varmaan mene taas kuin se
ilta ja jäsentäjä ja syntaksipuu on tehtynä ja alustavasti testattuna. Tai ainakin 
toivottavasti näin, koska sen jälkeen alkaa sitten se oikeasti mielenkiintoinen osuus, jonka
kanssa tulkin kanssa tuli hakattua päätä seinään aika lahjakkaasti. Etenkin tuon tarkastajan
kanssa. Mielenkiinnolla siis odotan kuinka se luonnistuu.
