# 3.1. Loppusuora(ko?)

Vähän on tullut nyt taukoa tämän työlokin päivittämisen kanssa. Pitkälti sen takia,
että alkaa olemaan tämä kokonaiskuva nyt sillä tapaa hallussa, että voi keskittyä
pääasiassa vain kovaan koodaamiseen. Kyllähän sieltä jatkuvasti löytyy uusia ongelmia
ja asioita, joita joutuu hetken aikaa miettimään ja selvittelemään, mutta sellaiset
peruskonseptit kuitenkin alkaa kuitenkin olemaan alkeellisesti hallussa. Tosi paljon
tullut myös keskityttyä vain koodin refaktorointiin ja sitä kautta kokonaispaketin
tiukentamiseen. Tietyllä tapaa tämä tällainen hierominen on vähän ulkona kurssin
tavotteista, kun kurssin tavoitteena ei sinänsä ole tehdä sitä mahtavaa kääntäjää,
vaan oppia niitä taustalla olevia tekniikoita ja periaatteita, mutta kyllä sitä väkisin
vain luonne vetää tuohon hieromiseen. On kiva tehdä kivoja juttuja. Kerätään kuitenkin
muutamia vastaan tulleita ongelmia/ajatuksia.


## Oma kääntäjä kullan kallis

Kyllähän sitä lopulta saatiin oma kääntäjä sellaiseen tilaan, että sen avulla pystyi
kääntämään ihan oikeita toimivia ohjelmia. Joskin melko triviaaleja sellaisia, mutta
ohjelmia kuitenkin. Funktioiden kanssa tosin meinasi olla aika paljon ongelmia. Siinä
meni aikalailla kaksi (2) päivää siihen, etten osannut laskea x - 8.

Pinossa ei siis lähtökohtaisesti voi olla kohdassa [rbp-0] mitään, koska tällainen arvo
valuisi pinon ulkopuolelle, vaan tämä pinon pohja löytyy aina kohasta [rbp-8]\*, jolloinka
siinä on tilaa 8 tavun kokoiselle arvolle. Vastaavasti kun funktion parametreja pusketaan
pinoon, löytyvät nämä parametrit positiiviselta etäisyydeltä rbp:hen nähden esim. [rbp+8]
sisältää paluuosoitteen ja sen jälkeen tulee sitten ne pusketut parametrit.

* Minulla on toistaiseksi kaikki tietotyypit yksinkertaistuksen vuoksi 8 tavun kokoisia


## Generoitu assembly

Tällä hetkellä generoidaan assemblyä jo käytännössä kaikelle toiminnallisuudelle , mitä 
kääntäjän tavotteiksi on asetettu. Generoitu assembly on kuitenkin nätistikin sanottuna
todella rumaa ja surkeaa.

Päätin alkuun, että kaikki tietotyypit tulevat olemaan 8 tavun kokoisia ja käsittelemmekin
kaikkea tämän kokoisina palasina, jotta hommat pysyy yksinkertaisena. Toinen yksinkertaistus
oli päättää, että kaikki tilapäismuuttujatkin tallennetaan pinoon. Tässä kohtaa kun generoidulle 
välikielelle ei tehdä minkään sortin optimointeja, on käytetyn muistin määrä per pinokehys
aika valtava.

Nämä yksinkertaistukset ovat kuitenkin olleet todella avuliaita sen suhteen, että pääsee
tekemään ihan oikeasti toimivia asioita ja pikkuhiljaa alkaa se ymmärrys lisääntymään ja
tunnistaa paremmin niitä kohtia, joissa on varaa tehdä pientä optimointia tai muuttaa käskyjä.
Esimerkiksi funktion parametrien poistaminen pinosta tuli heti korjattua siten, että ne
poistetaan suoraan eikä niin, että ne tallennetaan ensin tilapäismuuttujiin. Tämänkaltaisia
pieniä asioita alkaa näkemään ja myös ratkaisuja niille.

En kuitenkaan usko, että isommin tulen tästä luopumaan, koska aika alkaa olemaan aivan
tosissaan kortilla, ja tässä on vielä useampi muukin kurssi mitä pitäisi viimeistellä 
loppukevään aikana. Huonosti toimiva tuote on kuitenkin 100% parempi kuin ei-toimiva tuote.


## Assemblyn ja välikielen välinen suhde

Nyt kun tätä assemblyä on generoitu, luettu ja opiskeltu, niin tuo välikielen funktio on avautunut
koko ajan vain enemmän ja enemmän. Tässä kohtaa en kyllä voisi edes kuvitella generoivani
assemblyä ilman välikieltä. Näin noviisinakin se kyky nähdä asiat suoraan assemblynä, kehittyy
todella nopeasti, kun tämä näkeminen täytyy tehdä välikielen kautta. Lisäksi suoraan silmällä
näkee välikielestä jo paljon tärkeitä asioita todella kompaktissa muodossa. 

Vaikka alkuun jännitin koko välikieltä, sen mahdollista vaikeutta ja yleensäkin tarpeellisuutta,
niin kyllä tämä viime hetken lisävaatimus on ollut kaiken lisätyön arvoinen.


## Testejä, testejä, testejä

Tulipa refaktoroitua koko testien kokonaisuus ja tämän ympärille luotua testejä hallinnoiva
keskitetty tirehtööri. Tavoitteena saada parempaa palautetta testeistä ja lisäksi kyky ajaa
testejä vain osittain eikä niin, että aina tarvitsee ajaa kaikki testit. Tätä tekstiä 
kirjottaessa testejä on ~150, eikä näiden kaikkien testaaminen ole missään tapauksessa
tarpeen, jos kehitetään jotakin tiettyä osaa kääntäjästä.

Luotu Test_Runner moduuli on kuitenkin vielä melko vajaa siihen nähden, mitä haluaisin sen
tekevän, mutta kyseiseen moduulin tuli käytettyä jo aivan liian paljon aikaa ja näin ollen
tyydyn nyt siihen mitä on saatu valmiiksi, ellei sitten ilmene jotain oikeasti merkittäviä
ongelmia/kehitystarpeita.


## Prosessien välinen kommunikointi

Tämä on tunnetustikin melko hankala aihe hallita kunnolla ja näin käytännössä noviisina, se
on todella hankala aihe. Ongelmia alkoi esiintymään kun piti luoda testejä luoduille
esimerkkitiedostoille/-ohjelmille joiden avulla on helpompi testata kääntäjän toimintaa
kokonaisuutena. Ensin ajattelin homman sujuvan oikein näppärästä, kun tekee vain matriisin
olemassa olevista ohjelmista syötteineen ja tuloksineen ja senkus ajelee silmukassa asioita.
Aloin kuitenkin ihmettelemään, kun suoritus pysähtyi aina tietyn rajan jälkeen. Raja meni 
jossain ~20-25 ohjelman kohdalla. Noh, lopulta keksin, että eihän se suoritus mihinkään
pysähdy vaan siitä tulee vain TODELLA hidasta.

Uusi ohjelma avattiin popen- funktiolla, jotta ohjelman syöte saadaan luettua helposti. Tämä
funktiohan käyttää pellin alla fork-komentoa ja tässä oikeastaan onkin se esiin noussut ongelma.
Kun käynnistetään loopissa kymmeniä ohjelmia peräjälkeen käyttämällä fork-komentoa, tuppaa
siinä käymää niin, että 35 ohjelman sijaan käynnistyy 35! ohjelmaa. Johtuen nimenomaan siitä,
että fork tekee kopion käynnissä olevasta prosessista, ajaa sen loppuun ja sitten vasta palaa
takaisin alkuperäiseen isäntäprosessiin. Näin ollen jokainen avattu prosessi ajaa sen saman
loopin loppuun asti, mistä se on alkujaan käynnistetty, käynnistäen myös itse jäljellä
olevat ohjelmat. Näin käynnistettyjen ohjelmien lukumäärä kasvaa lopulta n:n kertomana.

Ongelma ratkaistiin käynnistämällä ohjelmat ilman looppia. Vähäsen se nyt aiheutti tyhmää
copy-paste-koodia, mutta sitäpä riittää muutenkin.


## Komentoriviargumentit

Tämä aiheutti enemmän ongelmia, kuin alkuun ajattelin. Ongelmat kuitenkin johtuivat siitä,
että olin unohtanut käyttäväni GCC:n linkkeriä, jolloinka tietysti ohjelman argumentit eivät
ole pinossa, vaan ne välitetään sitten normaalissa funktion argumenttivektorissa ohjelman
main-funktiolle, tai mikä se aloituspiste nyt sitten sattuukaan olemaan.

Mietin aluksi myös ratkaisua, että jokainen argumentti olisi tallennettu erikseen pinoon,
mutta luovuin tästä ajatuksesta hyvin nopeasti. Tämä ei ole oikein järkevällä tapaa (jos
lainkaan) mahdollista ajon aikana. Komentoriviargumenttien tyypit tarkastetaan aina kun niihin
viitataan ja samalla argumentti muunnetaan kokonaisluvuksi.

Tämän komentoriviargumenttien tyyppitarkastuksen voisi tosin tehdä jo heti ohjelman alussa,
niin mahdollinen virheenkäsittely on helpompaa. Nyt ei virheellisistä argumenteista anneta
mitään virheilmoitusta, vaan atoll-funktion mukaisesti, virhetilanteen jälkeen palautuu vain
arvo 0. Vaikka tämä ei kaada ohjelmaa tai muuten vaikuta olennaisesti toimivuuteen, on se
kivempi antaa kuvaava virheilmoitus käyttäjälle.


## Mitä jäljellä

### Kääntäjän olennaiset toiminnot

Tässä oli alkujaan aika pitkä lista asioita, mutta ne tulikin tehtyä, ennen kuin tämä teksti
puskettiin koodivarastoon. Periaatteessa ainoa olennainen toiminto, joka olisi jäljellä, on
loogiset operaatiot (and/or/not), mutta tämä ei kyllä periaatteessa ole edes pakollinen.
Muuten näitä olennaisia korjauksia ja parannuksia sen sijaan löytyy vielä pitkä lista, mutta
ne kuuluvat pitkälti refaktoroinnin piiriin.


### Muuta

- lisää testejä, testejä, testejä
- refaktorointia (nyt etenkin loppupään koodi on yksi todella iso sotku)
- dokumentaatio
- loppuraportti


## Mitä ehkä

- basic blocks
- constant folding
- pieni optimointi (esim. turhien ohjeiden karsiminen)
- AST-tulkin viimeistely
- välikielen tulkki
