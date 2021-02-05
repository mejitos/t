# 1.4. Testejä ja muistinhallintahommia

Tuli sitä muutakin tehtyä, mutta keskeisin iso työ oli testaamisessa sekä muistinhallinnassa.
Jäsentäjän itsensä funktiot tuli syljettyä todella nopeasti. Tuo algoritmi kun ei niin kovin
monimutkainen ole ja seuraa tiukasti kielioppia. Se onkin pääasiassa copy-pastea. Sama kuvio
oikeastaan jatkui jäsentäjän testien kanssa. Nyt kun alkaa löytymään sellainen perusvarma 
tapa luoda testitapauksia ja jonkinlaista kehystä alkaa syntymään, on testitapauksien tekeminen
sekin pitkälti copy-pastea. 

Testien copy-paste sen sijaan on sellaista, että siitä on tulevaisuudessa päästävä eroon. Se
kuitenkin tapahtuu vasta, kun ainakin perusosat (selaaja, jäsentäjä, tarkastaja ja jokin
tulkki/evaluaattori) on valmiina, jolloin sitä on varmaan aika hyvä näkemys kokonaisuudesta.
Tämän pohjalta voi sitten tehdä jonkun järkevämmän kokonaisuuden, jossa voisi vain syöttää
funktiolle läjän testitapauksia ja suorittaa ne silmukassa sen sijaan että ne on nyt yksi
iso sekvenssi samaa koodia, jossa vain funktioille syötetyt arvot muuttuvat.
Luomaani stringbuilderia en myöskään tällä hetkellä utilisoi mitenkään kovinkaan hyvin,
mutta eipä sitä olekaan ns. perustapauksien testaamiseen luotu.

Muistinhallinta olikin sitten se asia, joka aiheutti eniten työtä. Huomasin, että tuon muistin
vapauttamiseen on oikeastaan kaksi vaihtoehtoa: joko muistia vapautetaan sitä mukaan, kuin se
on mahdollista tai sitten vapautetaan kaikki kerralla aivan lopuksi. Muistia kun pystyy 
näköjään vapauttamaan aivan nätisti sitä mukaan, kuin asioita käyttää. Esimerkiksi, kun
jäsentäjä käsittelee avainsanoja, voidaan tämä muisti vapauttaa, jos niin halutaan. Tulin
kuitenkin siihen tulokseen, että vapautan kaiken muistin kerralla aina lopuksi ihan vaan
siksi, että saadaan taas pidettyä yksi kokonaisuus/toiminto tavallaan omana kokonaisuutenaan.
Se taas helpottaa omaa ymmärrystä kokonaisuudesta pitää myös koodin kokonaisuudessaan 
luettavampana ja helpommin ymmärrettävänä (ainakin minulle, mutta miksei muillekin).

> Valgrind on tosiystävä 
>
>   \- Gandhi

Mitä tulee jäsentäjän tulevaisuuteen, niin luulen, että teen tässä kohtaa sellaisen tempun
etten toistaiseksi lisääkään kieleen sen enempää lausekkeita tai lauseita, koska en niitä
varsinaisesti tarvitse juuri nyt. Todennäköisesti jatkan tätä hyvää vauhtia joka on päällä,
ja teen samalla vauhdilla jonkun alustavan suoran tulkin, joka tulkkaa syntaksipuuta. Siitä
kun on ilmeisesti ihan hyötyä tuleviakin askelia ajatellen, niin miksipä ei laittaisi sitä
aluille samantien. Voi sitten viimeistellä jäsentäjän samalla kuin viimeistelee kyseisen
tulkin.

Tästä tulkista tulisi kuitenkin todella yksinkertaistettu versio, koska välistä puuttuisi
se tarkastaja, ja en ole vielä 100% varma minkälaisen linkin nuo kaksi asiaa tarvitsevat
välilleen, jos tarvitsevat mitään. Haluaisin kuitenkin, että tulkin tehtävä olisi tosiaankin
lähtökohtaisesti vain ajaa lujaa, eikä sen tarvitsisi murehtia esimerkiksi tyypeistä tjms.
vaan ihannetilanteessa tarkastaja on jo hoitanut sellaiset. Näin ollen tulkki olisikin aika
ohut "evaluaattori" tässä kohtaa eikä muuta. Syksyn tulkkiprojektissa tuli vähän hämärä
suhde tarkastajan ja tulkin välille, jolloinka vastuut jakautui melko epäsymmetrisesti, mutta
se taitaa olla se hinta siitä, että on tulkattava ja dynaamisesti tyypitetty kieli.

Yleisesti ottaen TODO-lista kasvaa melko terveellä lailla koko ajan. Vaikka tällainen 
tietynlainen teknisen velan kerryttäminen on vähän hölmöä, on nyt tärkeämpää saada
jollain tapaa toimivaa kokonaisuutta pihalle ja alkaa sitten myöhemmin iteroimaan sitä
kokonaisuutta läpi, kun on oikeasti isoa kuvaa käsillä ja tietää mitä tosiasiassa tarvitsee.
