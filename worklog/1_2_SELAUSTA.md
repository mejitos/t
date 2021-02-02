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


