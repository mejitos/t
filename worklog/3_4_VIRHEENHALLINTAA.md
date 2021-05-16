# 3.4. Virheenhallintaa

Tästä on pakko tehdä ensimmäinen käänteiskronologinen kirjoitus. Käytin nimittäin
monta päivää funktion parametrilistauksen jäsentämiseen ja tarkemmin ottaen
sen jäsentämisen virheenhallintaan. T:ssä funktiot ovat JavaScriptin tavoin
lausekkeita, ja funktio määritellään syntaksilla

```
main: int = (argc: int, argv: [int]) => {
    return 0;
};
```

Tämä taas on rakenteeltaan täysin sama kuin muuttujan määrittely

```
foo: int = 0;
```

Eli nimi, tyyppi ja sitten nimen alustaja. Ongelma tulee siinä, että alustaja
on mikä tahansa lauseke, tarkemmin ottaen primäärilauseke (?). Näin ollen
parametrilistaus ja tavallinen sulkulauseke muistuttaa todella paljon toisiaan,
ja ongelma on valmis.

Näiden kahden osan erotteluun ja hyvään virheidenhallintaan (=hyvien virheilmoitusten
luomiseen) tuli käytettyä 3-4 päivää ja lopputuloksena 0 riviä toimivaa koodia.
Kokeilin kaikenlaisia ratkaisuja monimutkaisista tilakoneista aina joukko-oppia
mukaileviin ratkaisuihin joissa tarkastetana mihin joukkoon mikäkin sananen kuuluu.
Tajusin lopulta, että tässä on oikeastaan kaksi asiaa, jotka estää löytämästä 
hyvää ratkaisua:

1. Minun yleinen virheenkäsittely jäsentäjässä on todella suoraviivaista ja
alkeellista. Se on muutenkin toteutettu melko huonosti. Tämä on todennäköisesti
tilanne, jossa fraasikohtainen virheenkäsittely alkaisi olemaan huomattavasti
parempi vaihtoehto. Tämä taas on asia, jonka toteutus vaatisi koko jäsentäjän
ympäri kääntämisen.

2. Minun sivistykseni ohjelmointikielten periaatteeista on käytännössä 0, jolloin
tuosta on tosi vaikea luoda mitään formaaliakaan mallia/arviota jonka perusteella
sitä voisi analysoida ja keksiä paremman ratkaisun. Tai edes ratkaisun.

Tässä kohtaa alkoi myös TODELLA lujaa iskemään sellainen taisteluväsymys lukuvuoden
osalta. Tämä oli se hetki kun päätin, että nyt saa yksinkertaisesti riittää ja on
aika vain palauttaa työ. Vaikuttaisi muutenkin siltä, että tämä on se hetki, kun
sen formaalin osaamisen kehittäminen tulisi kysymykseen, jotta projektin kehittäminen
onnistuisi sujuvammin. Lisäksi virheenhallinta yleensäkin on asiana melko haastava
asia tehdä hyvin. Erityisesti hyvien virheviestin generointi. Näin ollen tämä on
oikein hyvä hetki muutenkin laittaa paussi tälle projektille, ettei hyppää 
tähän loputtomaan suohon enää tässä kohtaa.
