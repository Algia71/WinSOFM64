# WinSOFM64

Applicazione della quantizzazione vettoriale operata dalla [Self Organizing Features Map (SOFM) di Kohonen](/doc/1990-Kohonen-PIEEE.pdf) alla distribuzione omogenea di punti nel piano. Il programma è stato realizzato nell'ambito del corso di Cibernetica, Laurea in Scienze dell'Informazione dell'Università di Bari, Italia.

_Application of vector quantization performed by Kohonen's Self Organizing Features Map (SOFM) to the homogeneous distribution of points in the plane. The program was developed as part of the Cybernetics course, Degree in Information Science at the University of Bari, Italy._


## Alcuni screenshot

Ecco di seguito alcuni screenshot catturati durante diverse fasi di utilizzo del programma

_Some screenshots. Here are some screenshots taken during different stages of using the application_


### Avvio del programma

All'avvio, il programma presenta graficamente lo stato della mappa di Kohonen, coi pesi inizializzati a valori di partenza.

_Starting the program. At startup, the program graphically presents the state of the Kohonen map, with the weights initialized to starting values._

![Avvio dell'applicazione](/img/winsofm-1.png)


### Impostazione dei parametri generali di esecuzione della simulazione

C'è un primo livello di configurazione che permette di impostare i parametri generali dell'esecuzione della simulazione.

_Setting the general simulation execution parameters. There is a first level of configuration that allows you to set the general parameters of the simulation execution._

![Parametri generali della simulazione](/img/winsofm-2.png)


### Parametri specifici dell'algoritmo di Kohonen

C'è poi un secondo livello di conofigurazione, relativo a parametri specifici di come lavorerà l'algoritmo di Kohonen.

_Specific parameters of the Kohonen algorithm. Then there is a second level of configuration, relating to specific parameters of how Kohonen's algorithm will work._

![Parametri dell'algoritmo di Kohonen](/img/winsofm-3.png)


### Fine della simulazione

Alla fine della simulazione, il programma presenta lo stato finale dei pesi della mappa di Kohonen.

_End of simulation. At the end of the simulation, the program presents the final state of the Kohonen map weights._

![Fine della simulazione](/img/winsofm-4.png)


## Dettagli tecnici

Il programma, scritto in linguaggio C nel 1996,  fu realizzato originariamente con Microsoft QuickC For Windows, sistema opeativo target Microsoft Windows 3.0. Questa è una ricompilazione a 64 bit realizzata con Microsoft Visual Studio Community 2022. Nella sua trasposizione da 16 a 64 bit, il programma conserva il modello cooperativo di multi tasking, originariamente necessario per permettere alla GUI di continuare a rispondere durante la fase di simulazione. E' interessante notare come non sia stato necessario apportare alcuna modifica al codice sorgente C al fine di effettuare il porting da 16 a 64 bit, al netto ovviamente dei file di progetto.

_Technical details. The program, written in C language in 1996, was originally developed with Microsoft QuickC For Windows; the target operating system was Microsoft Windows 3.0. This is a 64-bit recompilation made with Microsoft Visual Studio Community 2022. In its port from 16 to 64 bits, the program retains the cooperative multitasking model, originally necessary to allow the GUI to remain responsive during the simulation phase. Interestingly, no changes were needed to the C source code to port from 16 to 64 bit, apart from the project files, of course._
