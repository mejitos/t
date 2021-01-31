# Kuvaus 

Ohjelmointikieli T on Jyväskylän yliopiston kurssin TIES448 Kääntäjätekniikat kurssiprojekti.
Projektin tarkoituksena on laajentaa aikaisemmalla TIEA304 kurssilla saavutettua osaamista ja
ymmärrystä kääntäjätekniikasta. TIEA304 kurssilla toteutin tulkin omalle ohjelmointikielelle
ja nyt olisi tarkoitus nostaa panoksia tekemällä kääntäjä, joka tuottaa lähdekoodista ajettavan 
ohjelman. 

Tavotteina on lisäksi syventää ymmärrystä C-ohjelmointikielestä sekä saavuttaa perusymmärrys 
x86-arkkitehtuurin assembly-kielestä. Näin ollen luotavan ohjelmointikielen isäntäkielenä on C ja
kohdekielenä x86 assembly. Tuotettu assembly käännetään NASM:lla objektitiedostoksi ja linkitetään
ld:llä suoritettavksi ohjelmaksi. Testaus toteutetaan itsekirjoitetuilla testeillä ja testejä
kirjoitetaan sitä mukaan, kun ominaisuuksia lisätään. Jokainen lisätty ominaisuus testataan samalla
kun se lisätään. Kuten tulkkiprojektissa, myös tässä projektissa kaikki kirjoitetaan käsin, eli
mitään ulkopuolisia koodigeneraattoreita ei käytetä.

Toteutettavalla kielellä on vähintään seuraavat ominaisuudet:

* kokonaislukuaritmetiikka (infix-syntaksilla)
* valintojen tekeminen (if - lauseet)
* toisto (while - silmukat)
* muuttujat
* syötteen välitys vähintään ohjelman alussa
* pääohjelman paluuarvon tulostus ruudulle
* staattinen tyypintarkastus

Jos jää aikaa, toteutetaan lisäksi yksi tai useampi seuraavista:

* merkkijonosyöte ja -tulostus
* taulukot
* yksinkertainen merkkijonojen formatointi

Toteutettava lähdekieli muistuttaa syntaksiltaan JavaScriptiä/TypeScriptiä, pienellä mausteella
C:tä ja Pascalia. Kieli tulee olemaan staattisesti tyypitetty. Vaikka tuo tyypintarkastus on
ikäänkuin "ekstraominaisuuksissa", tulen alusta alkaen luomaan kielen sillä ajatuksella, että
se on staattisesti tyypitetty. Näin ollen tähtään kaiken kaikkiaan vähintään 7op suoritukseen,
jossa 3 op koostuu ns. vähimmäisvaatimuksista, 3 op kohdekielen valinnasta (rekisterikoneen
konekieli) ja 1 op staattisesta tyypityksestä.

Esimerkki lähdekielen syntaksista

```
# Program to calculate fibonacci numbers
# 
# Date: xx.xx.xxxx
# Author: John Doe


main: int = (argc: int, argv: int) => {
    if argc != 1 or n == 0 then return 0;

    n: int = argv;
    temp_1: int = 0;
    temp_2: int = 1;
    next: int = 0;
    i: int = 2;

    while i <= n do {
        next := temp_1 + b;
        temp_1 := temp_2;
        temp_2 := next;
        i := i + 1;
    }

    return b;
};
```


# Yleinen suunnitelma 

Karkeasti suunnitelma jakautuu ensimmäisen puoliskon aikana noin viikon mittaisiin 
sprintteihin ja toisen puoliskon aikana noin kahden viikon mittaisiin sprintteihin,
jolloinka projektin kokonaiskestoksi tulee noin 12 viikkoa. Tavoitteena on silti
tehdä projekti valmiiksi niin nopeasti kuin mahdollista, mutta edellisen kurssin tulkin
kanssa opin sen, että monta asiaa voi mennä pieleen ja vaikka tässä ollaan tietyllä
tapaa vanhan asian äärellä, on tässä silti yhtä iso määrä tuntematonta kuin syksylläkin.


## Vaihe 0 - Alustava suunnitelma

Alustava suunnitelma koostuu karkeasti tämän suunnitelman ja siten ääriviivojen luomisesta.
Luotuani tulkin aikaisemmalla kurssilla, minulla on aika hyvä idea tulkin peruspalikoista.
Tällä kertaa olen kuitenkin lähes uudella alueella C-kielen kanssa ja erityisesti x86
assemblyn kanssa. Näin ollen luotava kieli on todella paljon yksinkertaisempi kuin edellinen
kieli. Näin ollen ei lisätä turhaa kompleksisuutta kieleen/kääntäjään. Tällä tapaa voidaan
keskittyminen pitää vanhan opin kertaamisessa ja hiomisessa, mutta ennen kaikkea taas sen
uuden asian oppimisessa.


## Vaihe 1 - Alkusysäys

Ensimmäinen askel on puskea kääntäjän peruspalikat nopealla aikataululla. Luodaan selaajan
ja parsijan toiminnallisuudet seuraten samaa periaatetta kuin tulkin kanssa. Lisäksi luodaan
alustava testisysteemi näiden osien testaamiselle. Tehdään alustava tarkastaja, joka tekee
hyvin pintapuolista tarkastelua ja luo symbolitaulun. Symbolitauluja tulee olemaan vain
yksi globaali symbolitaulu. Lopuksi luodaan jonkinnäköinen tulkki/evaluoija, jonka avulla
voidaan testata kieltä ja käyttää kyseistä rakennetta koodigeneraattorin luomiseen. Tämä
osa tulee olemaan todella suoraviivainen.


## Vaihe 2 - Vedetään happea ja mietitään

Aikaisemmassa projektissa opin, että pitäisi useammin pysähtyä tarkastelemaan tehtyä työtä
ja miettimään tarkemmin tulevia liikkeitä. Annetaan oikeasti yhden sprintin ajan pääasiassa
vain ajatusta tulevalle, eli koodin generoinnille. Ohessa voidaan tehdä esimerkiksi testien
luomista edellisen vaiheen asioille, mutta pääpaino on koodigeneroinnin suunnittelussa.


## Vaihe 3 - Toimiva käännös

Tavoitteena saada ohjelma tuottamaan toimivia x86-arkkitehtuurin mukaisia komentoja ja 
saada ohjelma oikeasti kääntymään lähdekoodista ajettavaksi ohjelmaksi asti. Alustava
ohjelma, joka halutaan saada käännettyä, näyttäisi jota kuinkin tältä:

```
main: int = (argc: int, argv: int) => {
    return 0;
};
```

Käytännössä se on vain pääohjelma, joka palauttaa arvon 0. Lisäksi tässä kohtaa luodaan jo
puitteet sille, että pääohjelmasta palautettava arvo tulostetaan ruudulle. Näin ollen tässä
vaiheessa on kaksi tavoitetta:

* luoda oikeita ja toimivia x86 komentoja pääohjelmalle ja pääohjelman palautusarvon tulostaminen
* luodaan kokonaisuus, joka mahdollistaa ohjelman kääntämisen lähdekoodista ajettavaksi
ohjelmaksi. Tämä tarkoittaa lähdekooditiedoston muuntamista syntaksipuuksi, x86 komentojen
generointi, generoidun assembly tiedoston kokoaminen objektitiedostoksi ja lopuksi
objektitiedoston linkittäminen ajettavaksi ohjelmaksi.


## Vaihe 4 - Komentoriviargumentit

Parametrien tai yleensäkin syötteen välittäminen ohjelmille tulee alustavasti olemaan mahdollista
vain komentoriviargumenttien kautta, joten haluan saadan sen toimimaan alusta lähtien. Tästä
syystä se otetaan työn alle heti, kun ensimmäiset onnistuneet käännökset on saatu luotua. Tämä
on myöskin oikein hyvä johdanto esimerkiksi muuttujien lisäämiselle, kun joudutaan luomaan asioita
jotka ohjelman tulee pystyä muistamaan.


## Vaihe 5 - Kääntämistä, kääntämistä 

Kaikki tähän asti tehty työ mahdollistaa nyt sen, että voimme alkaa tehtaan lailla testaamaan
erinäisiä lausekkeita, niiden generointia ja kääntämistä.

Aritmetiikka

```
main: int = (argc: int, argv: [int]) => {
    return 1 - 1;
};
```

tai

```
main: int = (argc: int, argv: [int]) => {
    return 10 / 5;
};
```

tai aritmetiikkaa sulkeiden kanssa

```
main: int = (argc: int, argv: [int]) => {
    return (2 + 3) * 4;
};
```

Vertailuoperaatiot

```
main: bool = (argc: int, argv: [int]) => {
    return 2 >= 2;
};
```

Loogiset operaatiot

```
main: bool = (argc: int, argv: [int]) => {
    return true and 3 < 10;
};
```

Ja niin edelleen. Tämä on helppo muuntaa testeiksi, jossa käännetään näitä useita pieniä
ohjelmia ja tarkastellaan ohjelmien palautusarvoja. Generoidun assembly-koodin tarkastelu
sen sijaan jää ainakin tässä kohtaa käsityöksi, mutta se on hyväksyttävää. Tällä tapaa saa
myös vahvistettua tuntumaa ja intuitiota assemblyn kanssa.


## Vaihe 6 - Muuttujat 

Aika lisätä muuttujat mukaan, jotta ohjelmamme pystyy muistamaan jotain ja niistä saadaan
heti vähän hyödyllisempiä. Tämä on todennäköisesti melko helppo osuus sen jälkeen, kun on
saatu kohtuullinen pohja luotua.


## Vaihe 7 - Toisto ja valintojen tekeminen 

Tässä kohtaa asiat muuttuvat taas mielenkiintoiseksi. Vaikka näiden rakenteiden tekeminen
onnistuu käsin myös assemblyllä (hyvin yksinkertaisissa tapauksissa), tulee näiden luominen
täysin vapaamielisestä lähdekoodista olemaan varmasti melko haasteellinen. 


## Vaihe 8 - Kiilloitusta

Lopuksi voimme vain keskittyä testitapausten luomiseen ja kääntäjän lujittamiseen niin paljon
kuin se on mahdollista rajoitetussa aikaikkunassa ja ennen kaikkea rajoitetulla ymmärryksellä.
Lisäksi alussa mainittujen lisäominaisuuksien lisäämistä voidaan harkita.
