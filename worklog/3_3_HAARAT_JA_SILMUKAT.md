# 3.3. Haarat ja silmukat

If- ja while-lauseiden generointi välikielessä aiheutti yhden ongelman, joka
veti huomion useammaksi päiväksi. Huomasin, että minun toteutukseni johti
siihen, että esimerkiksi if-lauseille generoitiin "ylimääräisiä" nimikkeitä

```
if 1 < 0 then
    1 + 2;
else if 1 == 0 then
    3 - 4;
else if 1 != 0 then
    5 * 6;
else
    7 / 8;
```

on välikielenä

```
0       _t0 := 1
1       _t1 := 0
2       _t2 := _t0 < _t1
3       if_false _t2 goto _l0
4       _t3 := 1
5       _t4 := 2
6       _t5 := _t3 + _t4
7       goto _l1
8  _l0:
9       _t6 := 1
10      _t7 := 0
11      _t8 := _t6 == _t7
12      if_false _t8 goto _l2
13      _t9 := 3
14      _t10 := 4
15      _t11 := _t9 - _t10
16      goto _l3
17  _l2:
18      _t12 := 1
19      _t13 := 0
20      _t14 := _t12 != _t13
21      if_false _t14 goto _l4
22      _t15 := 5
23      _t16 := 6
24      _t17 := _t15 * _t16
25      goto _l5
26  _l4:
27      _t18 := 7
28      _t19 := 8
29      _t20 := _t18 / _t19
30  _l5:
31  _l3:
32  _l1:
```

Tämä ei sinänsä toimintaan vaikuta millään tapaa, koska hypyt ovat aivan
oikeita. Tämä on lähinnä rumaa ja todennäköisesti myöskin virhealtista,
kun yhdellä if-kontekstilla on tällä tapaa useampia poistumisnimikkeitä.
Vastaavasti ongelma on myös while-lauseilla, jotka tarvitsevat myös tietoa
ympäröivästä kontekstista break- ja continue-lauseiden varalta, jotta
niillä osataan vaikuttaa oikeaan silmukkaan.

Tämä oli ongelma, joka tuli vastaan melko myöhäisessä vaiheessa, joten
tarvittiin nopeita ja näyttäviä ratkaisuja. Nopea ratkaisu oli esitellä
mukaan kontekstipino, jossa ylläpidetään erinäisten kontekstien tietoja.
Tällä tapaa while-silmukan "sitominen" tiettyyn kontekstiin oli todella
helppoa, mutta ongelmat tulivat sitten if-lauseen kanssa.

If-lauseella on kuitenkin aika laaja konteksti kun siinä on se aloittava
if-lause, mielivaltainen määrä else-if osia ja sitten vapaaehtoinen else-osa.
Näin ollen uutta if-kontekstia ei voi avata aivan mielivaltaisesti, vaan
sillä on selvät ehdot. Vastaavasti if-kontekstia ei voida poistaa, kuin
hyvin spesifeissä tilanteissa.

Tämä sitten aiheutti melko ruman ratkaisun, jonka varmaan peruslohkojen
esittely ratkaisisi todella paljon nätimmin ja helpommin. If-lauseet saatiin
kuitenkin toteuttamaan oikeanlaista välikoodia tämän ratkaisun avulla, jolloin
aikaisemmin esitelty esimerkki taipuu välikielenä muotoon

```
0       _t0 := 1
1       _t1 := 0
2       _t2 := _t0 < _t1
3       if_false _t2 goto _l1
4       _t3 := 1
5       _t4 := 2
6       _t5 := _t3 + _t4
7       goto _l0
8  _l1:
9       _t6 := 1
10      _t7 := 0
11      _t8 := _t6 == _t7
12      if_false _t8 goto _l3
13      _t9 := 3
14      _t10 := 4
15      _t11 := _t9 - _t10
16      goto _l0
17  _l3:
18      _t12 := 1
19      _t13 := 0
20      _t14 := _t12 != _t13
21      if_false _t14 goto _l5
22      _t15 := 5
23      _t16 := 6
24      _t17 := _t15 * _t16
25      goto _l0
26  _l5:
27      _t18 := 7
28      _t19 := 8
29      _t20 := _t18 / _t19
30  _l0:
```

Tässä ratkaisussa on tosin edelleen vastaanlaisia ongelmia, jossa syntyy turhia
goto-lauseita ja se alkoikin olemaan hyvin selvä, että peruslohkot on pitkälti
se ensimmäinen askel tämän välikielen siistimiseen. Esimerkiksi jos silmukassa
olevan ehtolauseen jälkeen tulee break-lause, syntyy puolestaan ylimääräisiä
hyppylauseita, koska tarvitaan hyppylause if-kontekstista poistumiselle sekä
sitten while-konteksista poistumiselle. Esim koodi

```
main: int = () => {
    result: int = 0;
    i: int = 0;

    while true do {
        if result < 100 then {
            if result >= 42 then
                break;
            else
                result := result + i;
        }

        i := i + 1;
    }

    return result;
};
```

tuottaa välikoodina

```
0  main:
1       function_begin 144
2       _t0 := 0
3       result := _t0
4       _t1 := 0
5       i := _t1
6  _l0:
7       _t2 := true
8       if_false _t2 goto _l1
9       _t3 := result
10      _t4 := 100
11      _t5 := _t3 < _t4
12      if_false _t5 goto _l2
13      _t6 := result
14      _t7 := 42
15      _t8 := _t6 >= _t7
16      if_false _t8 goto _l4
17      goto _l1
18      goto _l2
19  _l4:
20      _t9 := result
21      _t10 := i
22      _t11 := _t9 + _t10
23      result := _t11
24  _l2:
25      _t12 := i
26      _t13 := 1
27      _t14 := _t12 + _t13
28      i := _t14
29      goto _l0
30  _l1:
31      _t15 := result
32      return _t15
33      function_end
```

Tästä nähdään, että käskyn 18 mukainen goto-lause on turha, koska sitä ei
voida koskaan saavuttaa. Sen lisäksi, että tajusin tässä kohtaa peruslohkojen
olevan oikea ratkaisu (joskaan ei ollut aikaa toteuttaa niitä) tajusin myös,
että tässä kohtaa ei enää lähdetä monimutkaistamaan nykyistä IR_Contextien
avulla luotua tilapäisratkaisua yhtään. Siinä alkaisi tulemaan sellainen
loputtoman ruma ehtolauseiden viidakko, että sitä ei siisti kyllä enää kukaan
jälkeenpäin. Näin ollen jätin tämän(kin) sitten tulevaisuuden pulmaksi.
