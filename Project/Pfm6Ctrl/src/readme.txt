
>p(ulse)  T,PW          ... 105us,0.54
>b(urst)  N,len,per     ... 0,0ms,500ms
>p(ulse)  T,PW          ... 175us,0.54
>b(urst)  N,len,per     ... 0,0ms,500ms
>p(ulse)  T,PW          ... 350us,0.54
>b(urst)  N,len,per     ... 0,0ms,500ms
>p(ulse)  T,PW          ... 600us,0.54
>b(urst)  N,len,per     ... 0,0ms,500ms
>p(ulse)  T,PW          ... 1000us,0.54
>b(urst)  N,len,per     ... 0,0ms,500ms
>p(ulse)  T,PW          ... 50us,0.54
>b(urst)  N,len,per     ... 5,1ms,500ms
>
>
>p(ulse)  T,PW          ... 150us,0.43
>b(urst)  N,len,per     ... 0,0ms,100ms
>p(ulse)  T,PW          ... 250us,0.43
>b(urst)  N,len,per     ... 0,0ms,100ms
>p(ulse)  T,PW          ... 750us,0.43
>b(urst)  N,len,per     ... 0,0ms,100ms
>
>
>p(ulse)  T,PW          ... 150us,0.44
>b(urst)  N,len,per     ... 0,0ms,15ms
>p(ulse)  T,PW          ... 250us,0.44
>b(urst)  N,len,per     ... 0,0ms,15ms
>p(ulse)  T,PW          ... 750us,0.43
>b(urst)  N,len,per     ... 0,0ms,15ms
>
>
>p(ulse)  T,PW          ... 2000us,0.60
>b(urst)  N,len,per     ... 4,15ms,2000ms
>p(ulse)  T,PW          ... 2000us,0.60
>b(urst)  N,len,per     ... 4,25ms,2000ms
__________________________________________________________________________________________

- sw tools req. Keil uvision 4.70 or higher
- ext. open source sw
	- STM peripheral library
	- STM USB library
	- FatFs file system

- file structure app only

- output file, hash management
__________________________________________________________________________________________

	: FW generira dva sklopa (4) PWM signalov za pogon IGBT stikal. Signali se prekrivajo s faznim zamikom 90 st. (slika ...)
	: mozna stanja generatorja so <simmer>, <burst> ali izklop. 
	: <simmer> je stanje s frekvenco 25kHz in sirino od 16 do max. 500 ns
	: <burst> je stanje s frekvenco 100kHz in z vlakom pulzov s skladu s protokolom xxx (slika ...)
	: <izklop> je neaktivno stanje na vseh 4 linijah
__________________________________________________________________________________________
	- funkc. PFM6 v skladu  z zahtevami ..
		. komunikacija s host sistemom
			: vnos parametrov za obliko flesa
			: zahteva za simmer stanje
			: zahteva za pulz
			: porocanje o izmerjeni energiji
		
		. komunikacija in kontrola napajalnika	
			: nastavitev vrsne napetosti (i2c)
			: zahteva za vklop/izklop	(i2c)
			: kont. merjenje napetosti na obeh kondenzatorjih banke
			: kont. merjenje temp IGBT
				
		. formiranje flash pulza glede na zahtevane parametre
			: stabilizacija napetosti na flesu
			: merjenje U/I in izracun energije
			
		. nadzor delovanja in vzdrzevanje  simmer stanja
__________________________________________________________________________________________
		- selftest
		- sw update
		- debugging
__________________________________________________________________________________________

RS232 konektor na PFM_CTRL
 __________      __________
|          ------          |
|   GND     BOOT    5V     |
|   TX      RX      RST    |
|__________________________|

lampe na PFM6
-l d0,d1,d2,d3,,d4,d5,d6,d7

lampe na discovery
-usb serial
-l ,,,,d12,d13,d14,d15
-l
-u s
+m 9
u 700
s 160,160
q 250,250,350,70

172.3.0.1  Murata IP

TODO:

- tokovna zanka 
- izbira kanala z izbiro triggerja, done
- error status, add. errors, simmer
- kontrola simmerja s frekvenco
- bootloader
- wifi bootloader
- ungetch
- parse, yay
- poenostavi wifi getch, putch

- nove trigerje ugasati po tem, ko simmer deluje !!!
- paralelni mode

7.2.14
- dodan burst v SetPwmTab - parametri qN,qTime,qLength in qPmax
14.2.14

- reentrant blokada v Parse
- ungetch, ungets
- TODO
- IgbtError implementacija
- prepreci modifikacije parametrov med intervalom (..incident ob prehodi SP  - VLP)
- WiFi je obcutljiv na echo iz Cfg !!!

21.2.2014 PREHOD NA NOVI HW, STARE VERZIJE SE NE PODPIRAJO VEC !!
-------------------------------------------------------------------------

SW version 2, podpora 0 serije (ali prototipov wtf, na prvi serijski verziji HW)
- Cfg izveden z ungets
- spremembe v razporeditvi AD, povprecje ADC3 so 4 vzorci
- implementacija izkljucno PFM_ENABLE_SENSE in PFM_FAULT_SENSE signalov
- interrupt implementacija za crowbar in igbt driver error

TODO: 
- error diagnostika za 20 in -5V
- ADC watchdogs
- brisanje igbt & crowbar error

24.2.2014
- ADC watchdog na toku rabi fiksen offset! Najnizja spodnja meja le lahko samo 000, 
  ne mores je izklopit in ce prileti v adc prozi konstantne interrupte pri hitrosti simmer DMA !!!
- za USB host driver se USB_OTG_BSP_mDelay zamenja z Wait in klicem App_Loop !!!

27.2. 2014
- Ce pri ADC3 zaporedje konverzij ni enako zaporedju kanalov, dobis periodine motnje na vseh kanalih ?!?!? sic
- senzor 20 in -5V zdaj deluje

28.2.2014
- popravil check chargerja, prej sploh ni delal !!!!
- obrnil logiko IGBT drive error, preveri lock LED !!!
- preveri ADC watchdogs, nastavitve, logika error statusa
- error status za i2c ???
- preimenuj CROWBAR 

5.3.2014
- LW spet prvic zalaufal
- IGBTdriveErr mora met pulldown
- EXTI_Line8 vkljuci se samo med pulzom (glej adc.c) .. zaradi motnje triggerja
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Jankotov mejl

- CROWBAR pride iz ennergometra po fibru, gre na Morherboard kjer Enable-driverje za IGBT. Poleg tega ga senza procesor preko 33k upora na  PD14. Normalno stanje je LOW. Ko izhodna energija preseže 2x vrednost energometer ugasne fiber in ostane v tem stanju zalecan, dokler ga ne resetira Tomaž.
Na vhod bi dodali filter s casovno konstanto cca ???ns. Kljub temu, da napaka direktno ugasne IGBTje, bi bilo pametno zapreti PWM izhode (3 state) in pocakati na reset napake s strani Tomaža.
 
- PFM FAULT SENSE pride iz Motherboarda. Sproži se,ko je napetost nasicenja na prižganem IGBTju višja kot 6V za vec kot približno 5us. Normalno se to lahko zgodi samo med pulzi, ko je duty cycle nad 40%. Ce se sproži izven tega okvira je motnja. Signal pelje na PE8 procesorja. Normalno stanje je LOW, ce pride do napake, gre signal HIGH za cca 2us, nato se napaka resetira avtomatsko. Procesor naj bi ugasnil PWMje in cakal na reset s strani Tomaža. Na vhod bi dodali filter cca 500ns. Ce želimo daljšo casovno konstanto filtra, je potrebno na Motherboardu dodati kondenzator, ki bo podaljšal cas  napake na vec kot 2us. 

Analog Watchdog, Naslednjic
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

max vrednost Er ( leskovar)

10Hz
 
SSP 105u, 640V,42J, max 50 -625A
MSP 175u, 540V,47J, max 55
SP	350u, 480V,72J, max 72  -428A
LP	600,  420V,86J, max 86J
VLP 1000, 360V,92J, max 100J -255A
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TODO:

poskusi sfiltrirat igbt drive error

7.3.2014
- nastavljena prioriteta IRQ
- povecanan CAN buffer
- umaknjeno pinganje na 3 sekunde
- zacasno ima USART1 tx realiziran z IRQ in simmer error se ne uposteva v Error statusu
hash 02B19B85

10.3.2014

- kiks na ScopeDumpBinary popravljen - koncno, m in p se ne smeta spreminjat po "-? 1\r"
- kiks na putVCP, return value je _buffer_push, ne USBD_OK, sic!
- veckratni event, mode

11.3.2014
- nastavitev nizjega simmerja izza vsakega Eack!!! Ce host prekine dolgopulzno sekvenco ostane simmer previsok ***11314
- ADC watchdog bo problem, ce vrednost presega obseg ADC! Potreben selektivni DISABLE ???

12.3.2014

- _PFM_Iap = AA, nova koda komande dodana v can parser ia _ID_SYS2PFM, sprozi IWDG reset in prehod v IAP
- izklop chargerja v primeru kriticnih napak	***120314

13.3.2014	
- parse ima dodan +/-E set,reset error status
- 
- hash 2A3EFF4C
- realiz. vstopna rampa v SSP, predhodna izvedba (vlak pulzov) zakomentirana, korenska funkcija brisana
- v emerg. izlop chargerja dodan disble ADC3 watchdoga

14.3.2014
- blokiran CAN interrupt med vpisom v CAN buffer iz konzole !
- IWDG vrze ven ce napol stisnes footswitch !!!!????
- v q(erefeke) dodan parameter qLength za modifikacijo dol~ine glavnega pulza

E = k*t*U*U
dE/dt = k*U*U
dE/dU = 2*k*t*U
k*U*U*dt=2*k*t*U*dU
dt/dU=2*k*t*U / k*U*U

dt/dU = 2*t/U
dt/t=2*du/U

400V, 100u, 10J 

404V 10.201J

20.2.2014

- umaknit disable/enable igbt ker pri slabih energijah simer ugasa --- spica je manjse zlo
- raziskat jebo s SetCommand in rekurzijo pri ParseCan ...

1.4.2014
Zadnje nastavitve za LW

-l d0,d1,d2,d3,,d4,d5,d6,d7
-l
-u serial
+m 9
u 700
s 180,180,50
q 270,300,365,50,105
q 350,220,300,100,175

- dodan filter za 20V in -5V
- vnos za q je imel kiks, ni preracunal iz V v pwm
- CAN se skine iz irq...

3.4.2014

- pri single channel mode disable tudi vseh senzorjev na tistem kanalu! zaenkrat termistor ...*

cist tazadne nastavitve za shaping

q 200,339,40,50,105
q 200,339,40,100,175
q 200,330,40,50,50

6.6.2014

- jeba z ADC koncno adacta
	TODO
- temp senzor pri enokanalni varianti
- snemanje offseta in kontrola napetosti pri IDLE simmer

13.5

- jeba z ADC watchdog - mal vec meritev, pocakat na nove omejitve, 
- sicer se ISR enostavno blokira al pa da vrze ven sele po nekaj kiksih

TODO
- stabilizacija toka (mode 10) pri shape pulzu
- IAP iz klucka
- razmislit o zagonskem SW in diagnostiki na klucek
- preglej, kje vse lahko zajebe zacetek FatFs na 6 sektorju...

19.5.2014

boot iz klucka se dela tako, da se najprej kopira hex file na flash, fat sistem je 
po novem v sklopu bootloaderja! Pogledat, ce se ga da spravit pod 16 kB, secer se 
vrze ven COM port!
Pri identifikaciji lokalno vs. can v CanProgHex namesto null v argumentu uporabit 
naslov v TxMessage objektu-bolj elegantno ! Vse sprobat !!!!

27.5.2014
TODO
- popravljen bug v putCAN, com port ne blokira vec, problem se vedno pri dolgih izpisih
- pri single channel lucke bliskajo tudi na neaktivnem kanalu ....????
- izpis status iz ADP

27.5.2014
- zaradi tezav pri kalibraciji sprememba v SetPwmTab - ne preracunavam vec pulza iz tref v to ! 
  Napetost je zato nekaj nizja kot na konzoli, ne more pa vec preseci max. vrednosti!
  
- default meje za u 700 popraviti na 500-780 !!! Spodnja meja -20% previsoka, ker pri dolgopulznem 
  vrze ven pri 500J, zgornja prenizka, ker pri Er na max. rezimu PFC odpizdi cez 750, ko spustis 
  footswitch !!! Zaenkrat dela pri "u 700,500,780" v cfg.ini !!!
- 2.6.2014

karakteristika ventilatorja
%		ms

10		110	
20		23	
40		16
60		10	
80		8
100		7	

19.6.2014

inic. Pref1 in Pref2 (current loop reset ...)  se prestavi iz PFM_set komande na posiljanje energije Eack

24.7.2014

Na testiranjih max. mode vrze ven po cca 10-15 min, gui sier javlja 0081-0006, kar je rezultat zaustavitve PFM
- error 0x2000 (fan ...??)
- verjetno zaradi rekurzivne kasnitve pri kom. z chargerjem
- po odstranitvi 10Hz pollinga za charger status stvar deluje, prihaja pa do izpadov  syscontrol 

25.7.2014

major. verzija 100

- odpravljen bug z fan error na max mode
- skrajsan cakalni cas na SetPwmTab
- CAN debug output premaknjen na izhod (ParseCan)


verzija 101

- error margin za -5 V napajac 0d -4 do -6
- max temperatura za IGBT je 50% nad max. obrati ventilatorja
- dodan CAN command za nastavitev DAC iz GUI, sprememba GUI 
- dodan konzolni ukaz za nastavitev int izpis DAC vrednosti
- na konzolni izpis za (f)an dodana temperatura (samo pri izpisu...)
- debug izpis energije na desetinko J


