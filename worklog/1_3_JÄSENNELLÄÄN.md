# 1.3. Jäsennellään


## Niin hyvää syntaksipuuta

Toistaiseksi kaikki tärkeimmät solmut syntaksipuulle alkaisi olemaan tähän hätään kirjoitettu
(ehkä). Mitään hirveitä muutoksia ei aikaisempaan Python-tulkkiin nähden tullut. Ainoa
merkittävämpi muutos on tähän kieleen valitsemani syntaksin aiheuttama tilanne, jossa
funktioita käsitellään, muuttujiin sijoitettuina lausekkeina JavaScriptin tapaan. 
Toinen merkittävämpi muutos edelliseen kieleen nähden on `let` ja `const` avainsanojen
poistaminen. Mitä enemmän tässä on näitä asioita veivannut ja miettinyt, niin sitä enemmän
nuo avainsanat asioiden esittelyssä ahdistaa. Näin ollen päätin ottaa tällaisenkin kokeilun,
että kyseiset avainsanat otetaan pois käytöstä ja muuttujan esittely ja olemassaolo
varmistetaan sitten tarkastajan toimesta. Toistaiseksi ei ole suunnitelmissa toteuttaa
minkäänlaisia `const`-tyyppejä.

En tiedä vielä kuinka lujaa nämä valinnat tulevat puraisemaan takapuolesta, mutta sittenhän
se nähdään. Pienempänä lisänä on sitten noiden tyyppien kuvaajat. Niin yli en kuitenkaan aio ampua, 
että tyyppejä alettaisiin selvittelemään erikseen, vaan ne täytyy aina antaa käyttäjän
toimesta.

En kuitenkaan aivan ymmärrä, että miksi luennoilla opettajan esimerkkikääntäjässä on joka
ikinen lauseke tehtynä erikseen jokaiselle yksittäiselle operaatiolle. Ehkä se selviää
ajan myötä.


## Komentoriviargumentit

Syntaksipuuta kirjoittaessani huomasin yhden aika olennaisen asian, joka täytyy ottaa
tulevaisuudessa huomioon. Minulla on ohjelmassa pääohjelma, jonka kautta voidaan käyttää
komentoriviargumentteja. Komentoriviargumentit sen sijaan, ovat oletuksena merkkijonoja ja
merkkijonot taas ovat tyyppi, jota en ole toteuttamassa kieleeni (ainakaan tällä hetkellä).
Näin ollen pitää joko ottaa merkkijonot mukaan kieleen, tai sitten tekee oman laajennosfunktion,
joka käsittelee tulevat komentoriviargumentit muuntaen ne oikeantyyppisiksi.

En myöskään vielä tiedä kuinka käsitellä taulukkotyyppejä jäsentämisessä, joten toistaiseksi
komentoriviargumenttien kohdalla käytän kokonaislukutaulukon sijaan pelkkää kokonaislukua.


## Kielioppia

Seuraavaksi pitäisi alkaa sitten puskemaan jäsentäjää kasaan ja tämä on asia johon tarvitaan
tietenkin kielioppia, jonka mukaan jäsennellä. Kielioppi on kirjoitettu EBNF-notaatiolla ja
toistaiseksi käsittää vain pienen osan. Täydennellään sitä mukaan, kuin on tarpeen.

```
declaration             = IDENTIFIER ':' type_specifier '=' expression ';' ;

statement               = expression_statement
                        | block_statement
                        | while_statement
                        | if_statement
                        | return_statement ;

expression_statement    = expression ';' ;
block_statement         = '{' declaration* '}' ;
while_statement         = 'while' expression 'do' statement ;
if_statement            = 'if' expression 'then' statement ('else' statement)? ;
return_statement        = 'return' expression ';' ;

expression              = assignment ;
assignment              = IDENTIFIER ':=' assignment
                        | or ;
or                      = and ( 'or' and )* ;
and                     = equality ( 'and' equality )* ;
equality                = ordering ( ( '==' | '!=' ) ordering )* ;
ordering                = term ( ( '<' | '<=' | '>' | '>=' ) term )* ;
term                    = factor ( ( '+' | '-' ) factor )* ;
factor                  = unary ( ( '/' | '*' ) unary )* ;
unary                   = ( 'not' | '-' | '+' ) unary
                        | primary ;
primary                 = '(' expression ')'
                        | '(' parameter_list? ')' '=>' statement ';' ;
                        | literal ;
literal                 = IDENTIFIER
                        | INTEGER
                        | BOOLEAN ;

type_specifier          = 'int' | 'bool' ;
parameter_list          = IDENTIFIER ':' type_specifier ( ',' IDENTIFIER ':' type_specifier )* ;

```


## Jäsentelyä

Ensimmäinen tavoite olisi saada jäsenneltyä ns. pääohjelma oikein. Tosin sillä poikkeuksella
(näin alkuun) että pääohjelman komentoriviargumentteihin ei laiteta taulukkoa argv muuttujaan,
vaan siihen tulee ihan yksittäinen kokonaisluku. En vielä tiedä kuinka käsittelen taulukkoita,
mutta eiköhän sekin aukene tässä jäsennellessä.


### Ongelmia

Tai ei nyt varsinaisesti ongelmia, mutta ensimmäiset ajatuksia herättävät asiat nousivat esiin,
kun raavin kasaan ensimmäistä osaa jäsentäjästä. Tarvitsen kyllä kipeästi jonkun yleisemmän
tietorakenteen dynaamisille taulukoille. Sellaisen tekee aika nopeasti tällaiseen tarkoitukseen,
mihin sitä tällä hetkellä tarvitsen. Sekin on parempi tehdä nyt tässä kohtaa, kun ei varsinaista
koodia ole vielä niin paljoa.

Merkittävämpi vastaan tullut asia on muistinhallinta. Nyt tosiaan koko selaajan vapautusfunktio
muuttui käytännössä turhaksi, koska nyt myös jäsentäjä käyttää näitä sanasia. Sen sijaan selaajan
testejä varten mokoma funktio on aivan hyvä olla olemassa, joten jos ei muuta, niin sen voi siirtää
selaajan testimoduuliin jahka testejä alkaa moduuleihin jakamaan. En tässä kohtaa kuitenkaan tee
asialle mitään, vaan teen rauhassa jäsentäjän loppuun ja mietin sen jälkeen taas, että kuinka
muistia saadaan vapauteltua nätisti, kun on isompi kokonaisuus kasassa. Täytyy kuitenkin muistaa 
myös se, että tässä on monta palikkaa tekemättä, jotka käyttävät näitä tietoja.


## Jäsentäjän testaaminen

Tämä onkin asia, jolle en toistaiseksi tiedä kunnon ratkaisua. Aikaisemmin Pythonilla tein rekursiivisesti
sellaisen Scheme-tyylisen merkkijonon joka edusti jäsennyspuuta ja sitten pystyi vain vertailemaan
merkkijonoja. Minulla ei ole aavistustakaan kuinka C:llä voisi rakentaa tuollaisia merkkijonoja
kätevästi, mutta empä kyllä tiedä parempaakaan tapaa testata jäsentäjää kuin tuo. Siinä tulee
testattua sitä rakennetta mitä se tuottaa, todella kätevällä tavalla. Muussa tapauksessa täytyisi
käsin käydä läpi jokainen luodun puun solmu ja se ei kyllä tule kuuloonkaan. Tässä kyllä tekemistä riittää,
enkä tosiaan ajatellut, että sitä ihan näin paljoa on. 


### Stringbuilder

Mietin tuossa, että testaamista varten varmaan olisi hyvä tehä jonkinsorttinen stringbuilder-systeemi,
jonka avulla voi rakentaa noita parsimistapuita (?), parsintapuita (?) merkkijonoksi testaamista
varten. Kuten edellä totesin, niin tällä tapaa se testaaminen käy huomattavasti kätevämmin, kun 
voidaan muuttaa esimerkiksi lauseke `1 + 2 * 3` muotoon `(1 + (2 * 3))` ja sitä kautta sitten
tarkastella jäsentääkö jäsentäjämme oikein vai ei. En ole aikaisemmin tehnyt mitään stringbuilderin
tyylistäkään, mutta mietin sellaista (yllättäen) suoraviivaista ratkaisua siihenkin. Käytännössä
lähes identtinen toteutus tuon dynaamisen taulukon kanssa, mutta asioiden lisääminen tapahtuu 
muistia/puskureita kopioimalla. Tämä onnistuisi ehkä nytkin ovelalla `snprintf` käytöllä, mutta
uskon tämän olevan parempi vaihtoehto.
