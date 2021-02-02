# 1.2. Selausta


## Tunnisteet ja avainsanat

Tuo tekemäni ratkaisu tunnisteille ja avainsanoille on kyllä koko ajan vain huonompi.
Tulee ihan turhaan allokoitua muistia avainsanojen lekseemille, jota en kuitenkaan
tallenna ja sitten sitä pitää vapautella koko ajan. Nämä eivät tunnetusti ole mitään
halpoja operaatioita. Ei siinä, että mikään nykyisessä ohjelmassa olisi erityisen halpaa,
mutta hyvä pitää mielessä tällaiset asiat. Avainsanojen ja tunnisteiden selaamiseksi
pitäisi joka tapauksessa keksiä joku vähän järkevämpi tapa. Yksi vaihtoehto olisi varmaan
tekniikka "string interning" (mikä tämä on suomeksi?) jolloinka kaikki avainsanat vain
ladataan muistiin ja voidaan tehdä pelkästään osoitinvertailuja. Samaan rahaan oikeastaan
kaikki tunnisteet voitaisiin tehdä kyseisellä tekniikalla.

Tietenkin voisi myös esittää kysymyksen, että miksi emme vain tallentaisi lekseemiä
jokaisesta sanasesta, jolloinka kyseinen muisti voidaan vapauttaa aina oletuksena jokaisesta
sanasesta? Noh, en tiedä miksi varaisin muistia, jota en välttämättä tosiasiassa tarvitse?
Vaikka tavoitteenani ei ole luoda läpeensä optimoitua tuotetta, niin kuten edellä totesin,
mielelläni annan vähän ajatusta tällaisille jutuille. Ne sitten osottautuvat hyviksi ja
tärkeiksi tai hyvin todennäköisesti sitten turhiksi.


## Sijainti ja virheilmoitukset

Tässä kohtaa ollaan oikeastaan kahden viimeisen vaiheen ääreellä mitä tulee selaajaan:
sanasien sijaintitiedot ja paremmat virheilmoitukset. Sijaintitieto on sinänsä yksinkertanen
juttu. Luodaan vain tietue jolla on rivi ja saraketiedot ja sitä voidaan sitten käyttää
jokaisella syntaksipuun solmulla. Virheilmoitukset sen sijaan vaatii vähän ajatusta. Paino
sanalla vähän, koska en tässä kohtaa ihan liikaa halua sille antaa energiaa. Tulin kuitenkin
siihen tulokseen, että tällä kertaa en ala luomaan sellaisia virheilmoituksia, jota tein
tulkin kanssa, että konsolille tulostetaan ilmoituksen lisäksi pala lähdekoodia ja osoitetaan
tarkka paikka virheelle. Tämä jää myöhäisempään kiillotukseen. Tähän hätään riittänee vain
yksinkertainen funktio joka ottaa sijaintitietueen ja ilmoituksen, jolloinka saamme kuitenkin
rivi- ja saraketiedon sekä pienen viestin virheestä, joka on aivan riittävä tähän hätään.


## Selaajan viimeistelyä

Sittempä on oikeastaan vuorossa enää tätä "viimeistelytyötä". Eipä se selaaja missään tapauksessa
tule valmis olemaan, mutta tavoitteena on kuitenkin saada se sillä tapaa viimeisteltyä, että
minun ei tarvitsisi liiemmin ajatella sen olemassaoloa kun siirryn jäsentäjän pariin. Voin
jäsentäjää kirjoittaessani vain nauttia loputtomasta sanasten virrasta jotka tulevat käskettäessä.
Tämä viimeistely pitää sisällään juuri nuo edellä mainitut asiat, mutta lisäksi ainakin:

* Parempi muistinvapautustekniikka -> todennäköisesti selaajalle oma lexer_free() funktio, joka
vapauttaa kaiken muistin mitä selaaja on varannut. Tällä hetkellä se on aivan toimiva ratkaisu, 
mutta jatkossa tätäkin täytyy miettiä vähän enemmän, kun tulee kyseeseen näiden "objektien"
todelliset elinkaaret, jolloinka muistia voisi olla järkevämpää vapauttaa muulloinkin.
* Lisää testejä -> tällä hetkellä ei juuri mitään reunatapauksia testailla, joten tätä testausta
pitäisi vähintäänkin aloittaa kaikkein ilmiselvimpien tapauksien osalta esim. peräkkäiset 
operaattorit poimitaan oikein tai annetaan virheilmoitus, avainsanat ja tunnisteet pystytään
erottelemaan esim. "\_while" on täysin tapaasti käytettävissä oleva tunniste kuten myös "integer",
kokonaislukujen ylivuotokin pitäisi varmasti testata ja niin edelleen.
* Virheiden testaaminen -> kuten jo aiemmin puhuttiin virheiden testaamisesta, niin tätä varten
täytyy luoda jokin mekanismi, että kuinka se onnistuu. Toistaiseksi mietin ratkaisua, jossa
virheilmoitus ohjataan normaalisti stdouttiin tai stderroriin ja testiohjelma vain lukee sen
viestin sieltä varmistaen että ilmoitus on sitä mitä halutaan. En tässä kohtaa tai ehkä lainkaan
ala luomaan ratkaisua, jossa keräisin diagnostiikkaa kootusti yhteen paikkaan ja sitten aika
ajoin tulostelisin niitä sieltä. Ehkä sitten myöhemmässä vaiheessa.


## Selaustekniikka

Tulipa mieleen tapa, jolla ehkä pystyn ratkaisemaan nykysen avainsanojen ja tunnisteiden ruman
toteutuksen. Voisin aina heti iteraation alkuun vain etsiä kokonaisen lekseemin, ja sitten tätä
lekseemiä ajetaan tuon valtavan switch-casen läpi, joka sitten päättää, että mistä sanasesta
onkaan tosiasiassa kyse. Tässä voidaan kuitenkin pitkälti ajella osoittimia, eikä tarvitse 
tehdä mitään muistivertailuja tai muutakaan. Lekseemi kuitenkin katkaistaan aina tyhjän tilan tai
kommentin kohdalta, joten voimme ihan yhtälailla myös vertailla vain lekseemin osoittimien rajoja 
ilman, että tarvitsee puskea osamarkkijonoja erillisiin funktioihin ja tehdä muistivertailuja.
Tämä vaatii kuitenkin aika ison refaktoroinnin, joten tämä jää nyt ainoastaan idealaatikkoon.

### Päivitystä

Kyllähän sitä tuli lennosta keksittyä "siisti" tapa tuon toteuttamiseksi. Siisti siinä mielessä,
että sai poistaa yhden melko turhalta tuntuneen funktion. Vaikka onhan se tietyllä tapaa aika rumaa
tehdä käsin merkki merkiltä vertailua, mutta se on ainakin selvä ja suoraviivainen. Tässä kohtaa
varmaan moni eristäisi mokoman trie-hirvityksen omaksi funktiokseen, mutta minä en oikein ymmärrä
miksi siirtää tällaisia asioita omaksi funktiokseen, jos niitä käytetään vain kyseisessä kohdassa.
Onhan tuo switch-case aika hirviö, mutta se on silti yksittäinen kokonaisuutensa. Kyllähän tuossa
on silti niin paljon turhaa toistoa, että eiköhän se siitä siistitä. Nyt se kuitenkin toistaiseksi
ajaa asiansa todella hyvin.


## Muistinhallintaa

Kirjoittaessani tuota selaajan muistinvapautustekniikkaa, tuli vastaan taas pieni ongelma siinä,
että kuinka tuo sanaset nyt sitten vapautetaan, koska tuo malli jossa ne vapautetaan vain lopuksi,
tekee tällä hetkellä oletuksen siitä, että kyseinen taulukko on "kelattu" alkuun, jotta kyseinen
taulukko voidaan sitten iteroida alusta loppuun. Toistaiseksi se nyt on helppo laittaa tämä
"kelausvastuu" kutsujalle, mutta varmaan aika pian tilanne voi muuttua karvaiseksi. Täytyy siis
harkita linkitettyä listaa, ensimmäisen osoittimen tallentamista erikseen tai vastaavaa ratkaisua.


## Sijaintitiedot

Alkuun tuntui hieman overkilliltä testata jokainen testattu sananen ja sen positio, mutta kyllähän 
sieltä alkoi löytymään, jos nyt ei virheitä, niin ainakain jotain mitä ajatella. Eli onko lekseemin
sijainnin viimeinen merkki inclusive vai exclusive? Tämä on todennäköisesti asia, joka on täysin
minun päätettävissäni, mutta tämä on asia joka tulee päättää, koska se taas vaikuttaa melko olennaisesti
selaajan toimintaan. Tällä hetkellä viimeinen merkki on inclusive, mutta ainakin tällä hetkellä se
aiheuttaa aika ruman workaround ratkaisun kokonaislukujen ja tunnisteiden kanssa. Pitääpä laittaa
tämäkin pohdintalistalle myöhempää varten.
