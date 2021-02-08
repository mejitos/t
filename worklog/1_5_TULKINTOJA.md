# 1.5. Tulkintoja

Piti pitää välipäivä tai kaksi, koska aika monta päivää työskennelty lujaa + pitäisi
muitakin aineita opiskella, mutta yllättäen mielenkiinto veti tähän suuntaan ja 
päätin alkaa työstämään tulkkia kuten edellisessä päivityksessä päätin tehdä. Heti
alkuun tulikin vastaan asia, joka laittoi miettimään. Välittömästi, kun sain yleisen
kehyksen valmiiksi (= ensimmäiset `evaluate_X` funktiot), tajusin ettei tämä C
toimikaan niinkuin Python ja en voi vaan heittää vapaamielisiä tyyppejä palautusarvoiksi
saati parametreiksi.

Näin ollen jäljelle jää oikeastaan kaksi vaihtoehtoa näin nopealla ajattelulla. Joko
kirjoitetaan jokaiselle käytössä olevalle tyypille oma evaluointifunktionsa, tai sitten
luon oman abstraktion joka kuvaa yleisesti jotakin arvoa. Vaikka itse en sinänsä välitä
koodin kirjoittamisesta, niin tuo oma abstraktio tuo myös sellaisen edun nimeltä 
tyypintarkastus. C-kieli kun ei itsessään omista keinoja jonkun arvon tyypin tarkastamiseen,
ainakaan tällaisenaan. Kaiketi siihenkin joku clang-murre jostain löytyisi, mutta silloinkin
se toimisi vain sen alla, eikä siten ole mielestäni hyvä ratkaisu. Pysyttelen myös melko
tiukasti C99 alaisuudessa. Pitää siis luoda oma abstraktionsa arvolle ja varmaan muutama
apufunktio, joiden avulla voi tarkastaa tyyppejä nopeasti.


## Vastuunjaoista

Tuossa kauppamatkan aikana vähän pohdiskelin näitä eri osien vastuita samalla, kun pohdin,
että kenen tehtävä on esimerkiks luoda noita `Value` tietueita. Tulin siihen lopputulokseen,
että kyllähän se on tarkastajan tehtävä. Tulin samalla siihen lopputulokseen, että selaajan
tehtävä esimerkiksi ei kyllä tarvisi lainkaan olla arvojen päättäminen. Myöskään jäsentäjä
kun ei tee mitään kyseisellä tiedolla. Näin ollen selaajan tehtäväksi jäisi vain poimia se
lekseemi, ja tarkastaja sitten tarkastamisen yhteydessä muuntaa lekseemin arvoksi. Näin
tämä arvoksi muuntaminen tapahtuu vain yhdessä paikassa ja selaajan työtaakka kevenee. Tai
oikeastaan se selaajan tehtävä täsmentyy paremmin siihen, että mikä sen selaajan tehtävä on =
luoda sanasia lekseemeistä, ei päättää onko joku asia kokonaisluku tai boolean. Tämä aiheuttaa
jonkinverran työtä, mutta se on työ joka sitten vain tehdään.


## Tarkastuksia ja tulkintoja?

Totesin paremmaksi ideaksi tehdä molempia alusta alkaen rinnakkain. Johtuen juuri näistä 
selkeistä vastuista, jolloinka tarkastaja tarvitaan väliin, jotta tulkilla on jotain konkreettisia
arvoja mitä tulkita ja käsitellä. Kyllähän nämä kyseiset palikat ovat toisaalta keskenään
todella samanlaisia, joten näiden tekeminen rinnakkain toimii kyllä oikein hyvin.


### Arvot

Arvojen muuntaminen merkkijonosta konkreettisiksi arvoiksi tapahtuu nyt tosiaankin tuolla
tarkastajassa/semanttisen analyysin vaiheessa. Siellä saa samaan rahaan hoitaa sitten kaikki
mahdolliset tarkastukset mitä tulee sen arvon luomiseen ja määrittelyyn. Sen sijaan se on 
vielä vähän hakusessa, että mihin kaikkialle tätä arvo-tietuetta pitäisi laittaa. Toistaiseksi
laitoin sen ainoastaan literaalien yhteyteen, koska ne nyt teknisesti ottaen edustavat näitä
arvoja.

Tuo constant folding jää toistaiseksi toteuttamatta, mutta mikäli se toteutettaisiin, niin
silloin arvo-kenttä tulisi jokaiseen lausekkeeseen tai vaihtoehtoisesti tarkastajassa
liikkuisi joku toinen tietue joka vastaa kyseisestä toimenpiteestä. Alkuun mallailin tuollaista
operandi-tietuetta, joka sitoisi tyypin ja arvon yhteen, jolloinka voisi helposti suorittaa
kyseiseiä toimenpiteitä tarkastajan aikana. En kuitenkaan usko, että sille on käyttöä,
jos lausekkeiden arvoja itsessään ei käsitellä mitenkään.


### Tyypit

Tässä kohtaa joutuu nyt kyllä tosissaan miettimään tätä tyyppiasiaakin ja vielä askelta
pisemmälle. Havahduin täsäs nimittäin siihen, että tarvitaanhan noille tyypeillekin lisää
informaatiota. Meidän pitää tietää minkä kokoisia ne on, minkä kokoinen niiden tasaus (align?)
on, puhumattakaan sitten esimerkiksi funktioista, jotka tarvitsevat vielä enemmän tietoa.
Toisaalta taas meillä kun ei ole mitään käyttäjän määrittelemiä tyyppejä, niin me tiedetään
tyypit ja siten niiden koot ym. tiedot etukäteen.

Toteuttaessani yksinkertaista tyyppi-tietuetta, huomasin myös, että sitä tulee toistettua
pitkin koodia ihan turhaan. Käytännössä kaikille olemassa oleville tyypeille voisi luoda
globaalit muuttujat, joihin voidaan sitten osoittaa muista tietueista. Tulee säästettyä
taas hieman sitä kullanarvoista muistia, mutta myös laskettua ohjelman kompleksisuutta.


### Näkyvyysalueet ja symbolitaulu

Toteutin jo alustavaa mallailua symboleille ja samalla päätin, että ainakin toistaiseksi
kielen näkyvyysalueet tulevat olemaan dynaamiset. Siitä on helpompi lähteä liikkeelle.
Lisäksi tarkastajaan tulen toteuttamaan samanlaisen pinorakenteisen viritelmän noiden
näkyvyysaludeiden hallintaan. Toteutin sellaisen syksyllä tulkin kanssa, ja se toimi
oikein hyvin. Nämäkin asiat täytyy toteuttaa jo tässä kohtaa, jotta tarkastajan saisi
toimimaan jo alusta lähtien mahdollisimman "oikealla" tavalla.


## Yleisesti lopuksi

Tässä on tullut kyllä monen monta haastetta nyt vastaan, koska kuten aikaisemmin jo totesin,
ei asioita voi tehdä ihan samalla tapaa kuin dynaamisesti tyypitetyssä kielessä. Vastaavasti
tämän minun luomani kielen ominaisuudet ovat hyvin erilaisia edelliseen kieleen nähden, niin
sitä on vähän sormi suussa, että mitä abstraktioita sitä nyt oikeasti tarvii ja mitä ei,
kuka/mikä nyt vastaa mistäkin hommasta ja niin edelleen. Tämän takia moni asia onkin nyt
todella leväällää lähdekoodissa. Lisäksi se tarvittavien palikoiden määrä mitä esimerkiksi
tarkastaja tarvitsee toimiakseen, yllätti aivan täysin. Nyt alkaa kuitenkin pikkuhiljaa 
hommat avautumaan jollain tapaa ja alkaa löytymään se suunta tekemiselle.
Todennäköisesti ne voimakoodauspäivät on vietetty ja eteneminen siirtyy siihen "hitaasti, mutta
varmasti"-vaiheeseen.
