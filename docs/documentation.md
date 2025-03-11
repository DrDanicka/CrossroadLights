# Dokumentácia Crossroad lights

Projekt Crossroad lights slúži na vyrváranie koncigurácií križovatiek s 3 alebo 4 cestami. Užívateľ si môže nastaviť akými smermi cesty smerujú, časový interval zelenej a či má daná cesta aj prechod pre choodcov.

## Konfigurácia:

Konfiguráciu si užívateľ môže vytvoriť v prívetivom UI vytvorenom v Pythone. Po vytvorení a verifikácií ju môže poslať do mikrokontroleru pomocou sériovej linky. Kódovanie konfigurácie pozostáva z 10 bajtov. 
```
Start byte (0xD9)
2 North road bytes
2 East road bytes
2 South road bytes
2 West road bytes
Checksum
```

Zakódovanie jednej cesty teda vyzerá následovne:

```
O = Orientácia
S = Rovno
L = Vľavo
R = Vpravo
Gt = Čas zelenej
P = Prechod pre chodcov

byte1:
| O | O | S | L | R | Gt | Gt | Gt |

byte2:
| Gt | Gt | Gt | Gt | Gt | Gt | Gt | P |
```

## Ukladanie konfigurácie:

Konfigurácia sa po odoslaní do mikrokontrolera skontroluje, či je validná. Ak je všetko vporiadku, tak sa uloží do EEPROM, aby sme mali konfiguráciu k dispozícií aj po odpojení mikrokontrolera. O USART komunikáciu a EEPROM ukladanie sa starajú funkcie v `USART_EEPROM_functions.S`. V prípade, že odoslaná konfigurácia nie je validná, tak sa obnoví predchádzajúca, prípadne sa prepne na defaultnú, ktorá je v kóde zapísaná.

## CrossRoadConfig

Po uložení a validácií sa vytvorí `CrossRoadConfig` objekt, ktorý v sebe zahrňuje 4 `Road` objekty, ktoré reprezentujú cesty. Každá cesta je spojená so svojím expandrom v závislosti na orientácií, ktorú dostal v konfigurácií. Môže to byť `N, E, S, W`.

## Stavy križovatky

Križovatka môže byť v stave `OFF`, kedy je vypnutá a čaká na sériovom porte novú konfiguráciu. V opačnom prípade je zapnutá `ON`, kedy pomocou `startTrafficLights()` funkcie objektu `CrossRoadLogic` vykonáva nahratú konfiguráciu. Stavy môžeme prepínať pomocou tlačidla v strede križovatky. Zapnutie a vypnutie reprezentuje aj LEDka na mikrokontrolery.

## CrossRoadLogic

O ovládanie križovatiek sa stará trieda `CrossRoadLogic`, ktorá môže ovládať križovaky s 3 alebo 4 cestami:

### 3 cesty:

V prípade 3 ciest môže byť križovatka v týchto stavoch:

```
MAIN_STRAIGHT -> zelenú majú autá idúce oproti sebe
SIDE_LEFT -> zelenú majú autá idúce doľava z vedľajšej cesty a doprava z hlavnej
SIDE_RIGHT -> zelenú majú autá idúce doprava z vedľajšej cesty a doľava z hlavnej
```

### 4 cesty:

V prípade 3 ciest môže byť križovatka v týchto stavoch:

```
ONE_STRAIGHT -> zelenú majú autá z N a S idúce rovno
TWO_STRAIGHT -> zelenú majú autá z W a E idúce rovno
ONE_LEFT -> zelenú majú autá z N a S idúce doľava a W a E idúce doprava
TWO_LEFT -> zelenú majú autá z N a S idúce doľava a W a E idúce doprava
```

Ako interval zelenej pre daný stav sa zoberie maximum zo všetkých ciest, ktoré majú v danom stave zelenú.

### Stavy semafórov:

Semafór môže mať nasledujúce stavy:

```
RED
RED_ORANGE
GREEN
ORANGE
```

kedy svietia prílušné farby na smafóre. O rozsvecovanie sa stará modul `Road` a `TWI`, ktorý obstaráva I2C komunikáciu.

## Časovanie:

Časovanie je riešené v module `Timer`. Na počítanie sa používa implementovaná metóda `millis()`. 


## Zapojenie:

Každá cesta má svoj vlastný expander so svojou unikátnou adresov. Adresy pre jedtnolivé smery sú:

```
NORTH_ADDRESS 0x5B
EAST_ADDRESS 0x59
SOUTH_ADDRESS 0x5A
WEST_ADDRESS 0x58
```

Piny každého z expandrov sú zapojené rovnako a to:

```
PA0 -> RED LEFT
PA1 -> ORANGE LEFT
PA2 -> GREEN LEFT
PA3 -> RED STRAIGHT
PA4 -> ORANGE STRAIGHT
PA5 -> GREEN STRAIGHT

PB0 -> RED PEDESTRIANS
PB1 -> GREEN PEDESTRIANS
PB2 -> BUTTON
PB3 -> RED RIGHT
PB4 -> ORANGE RIGHT
PB5 -> GREEN RIGHT
```