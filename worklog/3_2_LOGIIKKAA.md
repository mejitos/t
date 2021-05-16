# 3.2. Logiikkaa

Loogisten operaattoreiden lisääminen oli hyvin mielenkiintoisella tavalla
haastava osa. Tämä oli niin hyvä esimerkki siitä, kuinka sitä ottaa 
itsestäänselvyytenä monia korkeamman tason ohjelmointikielten ominaisuuksia.
Ajattelin nimittäin alkuun ihan tosissani, että x86 käskyt and, or ja not
hoitaisi nämä loogiset operaatiot suoraan sellaisenaan.

Kielessä true ja false on toteutettu kokonaislukuina siten, että true vastaa
arvoa 1 ja false arvoa 0.

## and / or

Nämä oli hyvin simppelit, koska nämä evaluoituu aina "oikein" seuraten
myös loogisen andin ja orin mukaista totuustaulua. Nämä pystyi toteuttamaan
ihan normaalina binäärilausekkeena, jossa operandeina on 0 tai 1.

## not

Tämä sen sijaan oli eri tarina. Tässä kohtaa aloin heräämään siihen, että
nämä on tosiaankin ns. bitwise-operaatioita eli nämä tosiaankin pyörittelee
bittejä ja siten omistaa ihan eri käytöksen kuin nämä loogiset operaattorit.
Nimittäin `not 1` ei ole yhtä kuin 0 ja vastaavasti `not 0` ei ole yhtä kuin 1.
Käsky not kääntää jokaisen bitin, jolloin nämä tulokset eivät tietenkään
pidä paikkaansa.

Käytetään esimerkkeinä 8 bittisiä lukuja

not true

```
0000 0001
--------- not
1111 1110
```

not false

```
0000 0000
--------- not
1111 1111
```

Kyseessä on jonnin verran eri kokoisia lukuja kuin 0 ja 1. Näin ollen loogisen
not operaation suorittaminen vaatii hivenen toisenlaisen toteutuksen. Ensimmäinen
ratkaisu joka tuli kynän ja paperin kanssa veivaamisen tuloksena kehiteltyä,
oli ratkaisu jossa notin jälkeen, otetaan and luvusta 1 ja näin saadaan
maskattua ensimmäiset bitit pois ja otettua huomioon ainoastaan LSB bitin tulos.

Esimerkkinä `not true = false`

```
0000 0001
--------- not
1111 1110
0000 0001
--------- and
0000 0000
```

Ja näin lopputuloksena on 0, joka vastaa meidän kielessä boolean arvoa `false`.
Sitten taas kun näin tämän paperilla, niin tajusin, että tässähän on yksi
ainoa xor operaatio luvusta 1, jolloinka looginen not voidaan esittää yhdellä 
xor-käskyllä.

Esimerkkinä taas `not true = false`

```
0000 0001
0000 0001
--------- xor
0000 0000
```

ja `not false = true`

```
0000 0000
0000 0001
--------- xor
0000 0001
```


## Short circuit

Samaa vauhtia tajusin, että and ja or toteuttaa vertailun tosiaan bittitasolla,
ja siten esimerkiksi lausekkeessa

```
if (false and true) or (false or true) {...
```

jokainen and- ja or- operaatio suoritetaan. Vaikka tämä ei tuota väärää
lopputulosta, tuottaa tämä vääränlaista käytöstä. Loogisissa operaatioissa
pitäisi kuitenkin olla mahdollista evaluoinnin keskeyttäminen ensimmäisellä
mahdollisella hetkellä. Vaikka tämä on konseptina hyvinkin tuttu jo 
ohjelmoinnin perusteista lähtien, niin nyt sitä ensimmäistä kertaa joutui
ihan konkreettisesti "kohtaamaan" tämän toiminnallisuuden. 

Ei ole koskaan tullut mieleenkään, kuinka monimutkaisista operaatioista
näissäkin on tavallaan kyse ja niitä varten joutuu miettimään ihan
algoritmin, että kuinka se evaluointi lopetetaan oikein oikealla hetkellä.
Tämä tarkoittaa tietenkin vertailuja ja hyppyjä, jotta evaluoinnista
voidaan hypätä pois oikealla hetkellä oikeaan paikkaan. Käytännössä 
nämä ovat vähän monimutkaisempia ehtolauseita.

### and

and-operaatiossa molempien operandien tulee olla arvoltaan true, jotta
evaluointi jatkuu. Toisinsanoen jos ensimmäinen (vasemman puoleinen)
operandi on heti arvoltaan false, ei evaluointia tarvitse jatkaa enempää,
koska lauseke ei enää voi evaluoitua todeksi. Tästä voidaan hypätä suoraan
seuraavaan lohkoon.

Käyttämäni algoritmi pseudokoodina and-operaation toteuttamiseksi:

```
    if left false goto false
    if right false goto false
    condition := true
    goto exit
false:
    condition := false
exit:
    condition and 1
```

### or

or-operaatiossa riittää, että toinen operandeista on arvoltaan true,
jotta evaluointi jatkuu. Näin ollen or-operaatiossa jokainen operandi on
pakko evaluoida/tarkastaa siltä varalta, että arvo on tosi.

Käyttämäni algoritmi pseudokoodina and-operaation toteuttamiseksi:

```
    if left false goto next
    goto true
next:
    if right false goto false
true:
    condition := true
    goto exit
false:
    condition := false
exit:
    condition and 1
```
