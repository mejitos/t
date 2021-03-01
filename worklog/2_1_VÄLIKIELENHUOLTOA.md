# 2.1. Välikielenhuoltoa

Aika aloittaa kokonaan uusi jakso tässä projektissa ja lyödä tavotteita seuraavaksi kuukaudeksi.
Viime viikon loppu lähti vähän käsistä, kun piti keskittyä olemassa olevien palikoiden 
hiomiseen ja parantamiseen, mutta välikieleen perehtyminen vei mennessään ja lähes kaikki mitä 
piti tehdä, jäi sitten tekemättä.

Hieman kyllä yllätyin siitä, miten vähän tähän välikieleen liittyvää konkreettista asiaa tai
kirjallisuutta löytyy. Kaikki on pitkälti vain symbolista suuntien antamista ja monissa
kirjoissa ei välikieliä käsitellä lainkaan. Erityisesti mitä vanhemmasta kirjasta/paperista
puhutaan, sitä vähemmän välikielillä on mitään osaa asioissa. Yksi paperi, jonka nimeä en nyt
tätä kirjottaessani muista (selasin viime yön aikana läpi noin tusinan verran kirjoja) kyllä
tiivisti asian siten, että nämä välikielet ovat enemmänkin taiteeseen kuin tieteeseen
verrattavia asioita. Näin ollen tämä on osa-alue on alue, joka jättää todella paljon sen
artistin itsensä vastuulle. 

Tämä on vain ollut vähän ahdistavaa näin oppimisen näkökulmasta, kun on 0 kykyä ymmärtää
tällaisia abstraktioita ja sen lisäksi on tyhmä kuin saapas. Sitten taas, kun alkaa
ymmärtämään näitä abstraktioita, niin sitten osaa ratkaista ongelmia symbolisesti siinä
muodossa kuin ne esim. kirjassa esitellään, mutta sitten sen sen siirtäminen käytäntöön
iskee vain aivot lukkoon.

Nyt kuitenkin onnistuu jollakin tapaa kohtuullisen järkevän kolmiosoitekoodin luominen.
Vaikkakin koodi tulostetaan vain konsolille, eikä mitään abstraktioita ole vielä tehty,
niin tästä ei kyllä ole pitkä matka siihen, että saadaan abstraktiot käyttöön ja koko
pipeline taas toimimaan.


## Maaliskuun tavoitteet

Tarkastellessani alkuperäistä suunnitelmaa, totesin vähän surkeaksi. Tässä kun on nyt
ilmennyt, että näitä väliin tulevia abstraktioita on aivan hirveä määrä, niin en suinkaan
kyllä ala toteuttamaan ensin vain pientä osaa toiminnallisuudesta lähdekoodista 
kohdekieleen asti, vaan kyllä tässä siirrytään taas sellaiseen toteutustapaan, jossa
keskitytään yhteen osa-alueeseen kerralla koko kielen laajuudelta.

### Tarkastaja

Tarkalleen ottaen kaikki tarkastajan ympärillä olevat asiat pitäisi saada maaliin. Eli
symbolit, symbolitaulu, näkyvyysalueet ym. Ne on tällä hetkellä vähän leväällään, jotta
on saatu vain asiat toimimaan jollain tapaa. Nyt kuitenkin alkaa olemaan jo kohtuu hyvä
kuva siitä, mitä oikeasti tarvitaan koko prosessin aikana, niin tämä on oikein hyvä hetki
laittaa nämä ympäröivät asiat kuntoon.

Toinen iso asia, joka ei yksinään ole tarkastajaan liittyvä, mutta tarkastajassa se
korostuu erityisesti: diagnostiikka. Tällä hetkellä kääntäjä kyllä generoi jonkin verran
diagnostiikkaa, mutta sitä ei tällä hetkellä pysty testaamaan mitenkään. Testaamista varten
pitäisi luoda jokin yksinkertainen diagnostiikka tietue, joka ottaa vastaan virheviestin
ja sijaintitiedot.

### Tulkki

Vähän ymmärsin väärin sen suoran tulkin kätevyyden. Syntaksipuuta kävelevän tulkin sijaan
se tärkeämpi suora tulkki olisi tulkki välikielelle. Eipä siitä silti varmaan haittaakaan
ole tehdä ns. treewalkkeria, koska siinä kohtaa, kun tarkastaja on täydessä terässä ei
puutulkin toteuttaminen vaadi oikeastaan työtä lainkaan. Tämä ei nyt kuitenkaan ole mikään
ykkösprioriteetti, joten se tehdään jos tehdään.

### Välikielen generointi ja ajaminen

Tämä tulee olemaan se kuukauden pääteema. Luodaan toiminnallisuus välikielen generoinnille
ja välikielen tulkkaamiselle. Välikieltä varten pyritään luomaan sopivat abstraktiot,
tässä tapauksessa tarkoittaen sitä, että saadaan ns. basic blockit käyttöön valmiina
optimointia varten. Tuskin tällä kurssilla ihan hirveästi tulen optimointiin keskittymään,
mutta jonkinlaiset puitteet sitä varten olisi kiva luoda.

### Muistinhallinta

Muistista ei ole viime aikoina juuri välitetty. Samaan aikaan sitä varten nyt ei ole ollut
mieltä tehdä puitteita, koska moni asia on sen verran leväällään, että siitä olisi tullut
vain turhaa refaktorointityötä. Toisaalta, jos ihan tarkasti alkaa asiaa ajattelemaan,
niin eipä koko muistista tarvitsisi tässä käyttötarkoituksessa isommin edes välittää.
Ohjelma on lyhytaikainen ja kaikki muisti vapautuu käyttöjärjestelmälle ohjelman ajamisen
jälkeen kyllä aivan itsestäänkin. Tämä on kuitenkin hyvää harjoitusta tällekin osa-alueelle,
kun ei aikaisemmin ole tullut tehtyä minkään valtakunnan muistinhallintaa.

### Testit

Ja tämä kaikki tietenkin testataan
