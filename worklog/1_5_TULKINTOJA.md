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
