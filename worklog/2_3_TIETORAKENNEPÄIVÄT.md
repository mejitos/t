# 2.3. Tietorakennepäivät


Tulipa pari päivää aika ahkeraan käytettyä tuon hajautustaulun kanssa. Koko ajan sai lennosta
uusia ideoita ja oppi uusia asioita, jolloin ei lopulta taas oikein tiennyt mikä se suunta on.
Sain kuitenkin idean tehdä hajautustaulusta ns. single header kirjaston, jotta voin sitten
hieroa sitä omalla aikaa ja käyttää jatkossa muihinkin juttuihin. Mietin, että vastaavan 
voisi toteuttaa muillekin tietorakenteille jota käyttää.

Tuli kuitenkin otettua todella syväluotaavaa tarkastelua muistinkäytöstä ja oli hyvin lähellä,
että en alkanut tekemään ratkaisua, jossa käsin asetellaan jokainen yksittäinen tavu varatulle
muistialueelle. Tällä olisi saanut oikein kivat ja nopeat haut taulukosta, mutta sitten taas
taulukkoon sijoitukset olisivat vaatineet vähän turhan paljon työtä. Täytyy varmaan ihan
kuriositeettina vain harjoitella sellaista, vaikka ei tähän projektiin sellaista käyttäisikään.
Tässä nimittäin oppi myös ymmärtämään kuinka järkyttävän hidas toimenpide se on pyytää muistia
käyttöjärjestelmältä ja nyt ymmärrän entistä paremmin miksi esimerkiksi Arena-allokaattoreita
käytetään.


## Hajautustaulu

Hajautustaulu toteutui lopulta oikeastaan niin kuin suunnittelinkin. FNV hajautusfunktio ja
avoin osoitteenmuodostus. Sen sijaan taulusta ei pysty poistamaan mitään. Tulin lopulta siihen
lopputulokseen, että minua kiinnostaa ainoastaan read ja write toiminnot nyt tähän hätään.


## Dynaaminen taulukko

Tämän voisi myös muuntaa tosiaankin single header file kirjastoksi ja samalla lisätä muutamia
apufunktioita helpottamaan elämää.


## Pino

Pohdin myös, että pinolle varmaan voisi myös oman tietorakenteensa tehdä. En kuitenkaan ole
vielä päättänyt mitään, koska minulla ei välttämättä tosiasiallista tarvetta ainakaan nyt
ole pinolle. Minulla kun tulee näkyvyysalueita toistaiseksi olemaan vain kaksi erilaista 
ja siten myös niiden syvyys on korkeintaan 2. Sen sijaan pinolle voi kyllä tulla käyttöä
myöhemmässä vaiheessa, joten tätä voi ainakin harkita.
