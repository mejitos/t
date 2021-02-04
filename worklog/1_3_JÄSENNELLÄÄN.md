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
