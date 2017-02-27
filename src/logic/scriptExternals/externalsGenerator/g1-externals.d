// *** Phoenix Externals  ***


// *******************************************************************************************
// ***                                       PRINT                                         ***
// *******************************************************************************************

FUNC VOID Print 				( VAR STRING s0 ) { };
// Ausgabebefehl, der Text ins Game schreibt (wie OutputunitTexte)
FUNC VOID PrintMulti 			( VAR STRING s0, VAR STRING s1, VAR STRING s2, VAR STRING s3, VAR STRING s4 ) { };
// Printbefehl, der aus den angegebenen Strings einen Auswählt und auf den Bildschirm schreibt
FUNC VOID PrintDebug			(VAR STRING s)	 { };
// Printausgabe, die nur bei eingeschaltetem Debugmodus (Alt+D) im Spy/logfile ausgegeben wird
func VOID PrintScreen           (var INT dialogNr, VAR STRING msg, VAR INT posx, VAR INT posy, VAR STRING font, VAR INT timeSec) { };
// Gibt den Text 'msg' auf dem Bildschrim aus und benutzt dabei den Font 'font'.
// Die Position ist für jede Koordinate eine Zahl zwischen 0 und 99 und gibt die prozentuale Position an.
// Der Ursprung befindet sich oben links (also 0% X und 0% Y)

// *******************************************************************************************
// ***                                          MISC                                       ***
// *******************************************************************************************

FUNC INT 	Hlp_Random 			( VAR INT n0 ) { return 0; };
// erzeugt einen Zufallswert (wobei n0 Obergrenze : [0..n0-1] ) Rückgabewert integer 
FUNC INT    Hlp_StrCmp			(VAR STRING s1, VAR STRING s2) { return 0; };
// Hilfsfunktion um zwei Strings miteinander zu vergleichen liefert bei Gleichheit eins zurück
FUNC INT  	Hlp_IsValidNpc			(VAR C_NPC self)					{ return 0; };
// Prüfe ob NSC-Instanz gültig und initialisiert ist. True/False
FUNC INT  	Hlp_IsValidItem			(VAR C_ITEM item)					{ return 0; };
// Prüfe ob Item-Instanz gültig und initialisiert ist True/False
// -------------------------------------------------------------------------------------------
FUNC INT 	Hlp_IsItem 				(VAR C_Item item, VAR INT instanceName)
// Prüft, ob der Gegenstand 'item' den Instanznamen 'instanceName' trägt. Will man z.B. testen,
// ob die globale Variable item Pfeile sind (ItMuArrow) ruft man Hlp_IsItem(item,ItMuArrow) auf.
// Gribt bei Gleichheit TRUE, sonst FALSE zurück.
FUNC C_NPC 	Hlp_GetNpc				(VAR INT instanceName) { };
// Ermittle einen NSC über den Instanznamen. Dieser kann einer Variablen zugewiesen werden und ist somit gezielt verfügbar
FUNC INT 	Hlp_GetInstanceID 		( VAR C_NPC npc ) { return 0; };
// liefert die interne ID ( nicht Var aus der Instanz) zurück, um z.B. other mit self vergleichen zu können ( integer Vergleich)
FUNC INT 	Hlp_GetInstanceID 		( VAR C_ITEM item ) { return 0; };
// liefert die interne ID ( nicht Var aus der Instanz) zurück, um zwei items miteinander vergleichen zu können ( integer Vergleich)
// *******************************************************************************************
// ***                                      WARTEN / ZEIT                                  ***
// *******************************************************************************************

FUNC VOID 	AI_Wait 			( VAR C_NPC n0, VAR float n1 ) { };
// Character wird für n1 Sekunden in einen Wait-Zustand versetzt, 
// d.h. er tut nichts, aber Treffer etc.(passive Wahrnehmung) werden registriert

FUNC INT	Npc_GetStateTime		(VAR C_NPC self) { return 0; };
// Liefert Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
FUNC VOID	Npc_SetStateTime		(VAR C_NPC self, VAR INT seconds) { };
// _Setzt_ Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet. 
//--> Das ist dann über Npc_GetStateTime abfragbar.

FUNC INT	Wld_GetDay			() {};
// Liefert den aktuellen Tag zurück (Zähler), hierbei ist der StartTag (Gamestart) = 0.
FUNC INT    Wld_IsTime			(VAR INT hour1, VAR INT min1, VAR INT hour2, VAR INT min2) { return 0; };
// Liefert "1" wenn aktuelle Weltzeit zwischen den beiden angegebenen Zeiten liegt (von - bis)



// *******************************************************************************************
// ***                                       NSC-STEUERUNG                                 ***
// *******************************************************************************************

// --------------------------------------- NSC EINFUEGEN -------------------------------------
FUNC VOID 	Wld_InsertNpc				(VAR INT npcInstance, VAR STRING spawnPoint) 		{ };
// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf.
func void 	Wld_InsertNpcAndRespawn (VAR INT instance, VAR STRING spawnPoint, VAR FLOAT spawnDelay)
// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf. Stirbt dieser NSC wird
// nach "spawnDelay"-Sekunden ein neuer NSC am Spawnpoint erzeugt.

// ------------------------------------- ANIMATIONEN SPIELEN ---------------------------------

FUNC VOID 	AI_PlayAni 				(VAR C_NPC n0, VAR STRING s0 ) { };
// Npc-Instanz spielt die angegebene Animation ab
FUNC VOID	AI_StandUp				(VAR C_NPC self) {};
// - Ist der Nsc in einem Animatinsstate, wird die passende Rücktransition abgespielt.
// - Benutzt der NSC gerade ein MOBSI, poppt er ins stehen.	
func void	AI_StandUpQuick 		(VAR C_NPC self)
// Wie AI_StandUp(), jedoch werden keine Rücktransitionen abgespielt, sondern auch dort wird
// sofort in den Grundzustand "gepoppt". Wichtig für sehr eilige Situationen!
FUNC VOID 	AI_Quicklook			(VAR C_NPC self, VAR C_NPC other) { };
// NSC kurz ( 2 sec) anschauen ( nur Kopf bewegt sich )
FUNC VOID 	AI_LookAt				(VAR C_NPC self, VAR STRING name) { };
// Schaue auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)
FUNC VOID 	AI_LookAtNpc			(VAR C_NPC self, VAR C_NPC other) { };
// Schaue zu einem NSC
FUNC VOID 	AI_StopLookAt			(VAR C_NPC self) { };
// wieder geradeaus schauen
FUNC VOID 	AI_PointAt				(VAR C_NPC self, VAR STRING name) { };
// Zeige auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)
FUNC VOID 	AI_PointAtNpc			(VAR C_NPC self, VAR C_NPC other) { };
// Zeige auf einen NSC
FUNC VOID 	AI_StopPointAt			(VAR C_NPC self) { };
// nicht mehr auf etwas zeigen

// ------------------------------------- NSC NIMMT / LEGT WEG -------------------------------
FUNC VOID 	AI_TakeItem				(VAR C_NPC self, VAR C_ITEM item) {};
// der Npc nimmt mit dieser Funktion die globale Item-Instanz auf 
FUNC VOID	AI_DropItem				(VAR C_NPC self, VAR INT itemid) {};
// Item(itemid) wird auf den Boden fallen gelassen

// ------------------------------------------- USE ITEM -------------------------------------
FUNC VOID 	AI_UseItem				(VAR C_NPC self, VAR INT itemInstance) { };
// Item bis zum Ende benutzen
FUNC VOID 	AI_UseItemToState		(VAR C_NPC self, VAR INT itemInstance, VAR INT state) 	{ };
// Item benutzen bis zum angegebenen State

// ---------------------------------------- NSC BENUTZT MOB ---------------------------------

FUNC INT	AI_UseMob				(VAR C_NPC self, VAR STRING schemeName, VAR INT targetState) { return 0; };
// Benutze Mob mit angegebenen Schema-Namen bis zum Zustand "targetState". Wird diese Funktion aufgerufen
// und der angegebene 'targetState' ist bereits vorhanden, läuft der NSC zwar trotzdem zum MOB, tut dann aber nichts
FUNC INT  	Wld_IsMobAvailable		(VAR C_NPC self, VAR STRING schemeName) {};
// Sucht sich ein Mobsi im Umkreis von 10m und liefert TRUE falls gefunden. MOB wird nur zurückgeliefert, wenn es nicht besetzt ist.
func int	Wld_GetMobState			(var C_NPC self, var string schemeName );
// Liefert den State zum nächsten Mob mit 'schemeName' zurück bzw. '-1' wenn kein solches Mob gefunden wurde 

// -------------------------------------------- GEHEN ----------------------------------------

FUNC VOID 	AI_SetWalkmode 			(VAR C_NPC n, VAR INT n0 ) { };
// gibt an mit welchem Walkmode Run etc der Character durch das Level läuft
//NPC_RUN			: Rennen
//NPC_WALK			: Gehen
//NPC_SNEAK			: Schleichen
//NPC_RUN_WEAPON	: Rennen mit gezogener Waffe
//NPC_WALK_WEAPON	: Gehen mit gezogener Waffe
//NPC_SNEAK_WEAPON	: Schleichen mit gezogener Waffe
// -------------------------------------------------------------------------------------------
FUNC VOID 	AI_GotoWP 				(VAR C_NPC n0, VAR STRING s0 ) { };
// Npc-Instanz läuft zum namentlich angegeben Waypoint
FUNC VOID 	AI_GotoFP				(VAR C_NPC self, VAR STRING fpName) {};
// Sucht sich einen Freepoint im Umkreis von 20m vom NSC, bewegt sich dorthin und richtet sich entsprechend aus.
// Suchkriterium wie bei Wld_IsFPAvailable()
FUNC VOID AI_GotoNextFP 			(var C_NPC self, VAR STRING fpName ) { };
//wie AI_GotoFP() allerdings Suchkriterium wie bei Wld_IsNextFPAvailable()
FUNC VOID 	AI_GotoNpc				(VAR C_NPC self, VAR C_NPC other) {};
// "self" geht zu "other"
FUNC VOID 	AI_GotoItem				(VAR C_NPC self, VAR C_ITEM item) {};
// "self" geht zu "item"
FUNC VOID 	AI_GotoSound 			(VAR C_NPC n0 ) { };
// Npc läuft zum Sound
func void AI_Teleport				(var C_NPC self, var string waypoint);
// teleportiert den NSC zur angegebenene Location

// -------------------------------------------------------------------------------------------
FUNC STRING Npc_GetNearestWP		(VAR C_NPC self) { return ""; };
// liefert den Namen des am nächsten gelegenen Waypoints zurück
FUNC string	Npc_GetNextWP			(VAR C_NPC self) {return ""; };
// Liefert den zweitnahesten WP vom NSC zurück
// -------------------------------------------------------------------------------------------
FUNC INT  Wld_IsFPAvailable			(VAR C_NPC self, VAR STRING fpName) {};
// Sucht einen Freepoint im Umkreis von 20m vom NSC und liefert TRUE falls vorhanden und frei ('self' zählt als Blockierer nicht!) und sichtbar 
func int  Wld_IsNextFPAvailable     (var C_NPC self, VAR STRING fpName)
//  wie Wld_IsFPAvailable(), aber es wird immer der nahegelegenste genommen und 'self' zählt als Blockierer!
FUNC INT  Npc_IsOnFP				(VAR C_NPC self, VAR STRING name)
// Abfrage darauf, ob der Nsc auf einem Freepoint mit name Teilstring steht
FUNC INT  Npc_IsWayBlocked			(VAR C_NPC self) {};
// Liefert "1", falls Weg von NSC durch Hindernis versperrt ist.

// -------------------------------- DREHEN / AUSRICHTEN -------------------------------------

FUNC VOID 	AI_TurnToNpc 			(VAR C_NPC n0, VAR C_NPC n1 ) { };
// drehe Dich zum angegeben (zweiten) Npc um
FUNC VOID 	AI_TurnAway 			(VAR C_NPC n0, VAR C_NPC n1 ) { };
//Der NSC "self" dreht dem NSC "other" den Rücken zu.
FUNC VOID	AI_WhirlAround			(VAR C_NPC self, VAR C_NPC other) {};
// schnelle Drehung zu other
FUNC VOID 	AI_TurnToSound			(VAR C_NPC self) { };
// Charakter dreht sich zur Geräuschquelle
FUNC VOID   AI_AlignToWP			(VAR C_NPC self) { };
// richtet den Nsc am Waypoint aus (im Spacer gesetzte Pfeilrichtung)
FUNC VOID 	AI_Dodge 				(var C_NPC npc ) { };
// Der Nsc weicht ein Stück nach hinten aus

// ----------------------------------- OVERLAY-MDS -------------------------------------------

FUNC VOID 	Mdl_ApplyOverlayMds 	(var C_NPC n0, VAR STRING s1 ) { };
// mit diesem Befehl werden Animationen auf einem höheren Layer gestartet 
// (z.B. hat der Zombie ein Overlay über die normalen Human-Animationen)
FUNC VOID 	Mdl_RemoveOverlayMDS	(VAR C_NPC self, VAR STRING overlayName) {};
// Entferne ein Overlay-MDS
func VOID	Mdl_ApplyOverlayMDSTimed	(VAR C_NPC self, VAR STRING overlayname, VAR FLOAT timeTicks );
// Overlay-MDS wird gestartet und automatisch nach der angegebenen Zeit abgeschaltet

// ----------------------------------- RANDOM-ANIS -------------------------------------------

FUNC VOID 	Mdl_ApplyRandomAni 		(VAR C_NPC n0, VAR STRING s1, VAR STRING s2 ) { };
// Fügt Zufallsanimationen (am Kopf kratzen, an den Sack packen etc (s2)) für bestimmte Animationszustände (s1) ein
FUNC VOID 	Mdl_ApplyRandomAniFreq 	(VAR C_NPC n0, VAR STRING s1, VAR FLOAT f2 ) { };
// hiermit kann die Frequenz betimmt werden, wie oft die für den Animationszustand (s1) deklarierten Randomanis abgespielt werden

// ----------------------------------- FACE-ANIS ----------------------------------------------

FUNC VOID	Mdl_StartFaceAni	(VAR C_NPC self, VAR STRING name, VAR FLOAT intensity, VAR FLOAT holdTime) {};
// Starte Gesichtsanimation // intensity 1 = 100% // holdTime -1 = forever
FUNC VOID	Mdl_ApplyRandomFaceAni	(VAR C_NPC self, VAR STRING name, VAR FLOAT timeMin, VAR FLOAT timeMinVar, VAR FLOAT timeMax, VAR FLOAT timeMaxVar, VAR FLOAT probMin) {};
// Starte zufällige Gesichtsanimation
// Mdl_ApplyRandomFaceAni ( self, ANINAME, minTime, minTimeVar, maxTime, maxTimeVar, probMin)
// minTime  = Minimum an Zeit nachdem Ani startet (in Sekunden)
// maxTime  = Maximum an Zeit nachdem Ani startet (in Sekunden)
// minTimeVar = Abweichung von minTime (in Sekunden)
// maxTimeVar = Abweichung von maxTime (in Sekunden)
// probMin  = Wahrscheinlichkeit [0..1] ob obere oder untere Grenze der Abweichung benutzt wird

// --------------------------------------- BODYSTATES ----------------------------------------

FUNC INT  	Npc_GetBodyState			(VAR C_NPC self)					{ return 0; };
// Ermittle BodyState ( Liefert BS_-Konstanten )
// !!! VORSICHT !!!: Diese Funktion gibt den Bodystate mit allen eventuellen
// ver-oderten Modifiern wie "brennend". Besser 'C_BodyStateContains(self,bodystate)' benutzen
FUNC INT 	Npc_HasBodyFlag				(VAR C_NPC self, VAR INT bodyFlag) 			{};
// Liefert >0, falls BodyFlag gesetzt ist.
func VOID AI_PlayAniBS ( VAR C_NPC npc, VAR STRING aniname, VAR INT bodystate ) { };
// Beim Abspielen einer Ani mit diesem Befehl kann ein Bodystate angemeldet werden

// *******************************************************************************************
// ***                                       KAMPF                                         ***
// *******************************************************************************************

// --------------------------------------- KAMPFMODUS ----------------------------------------

FUNC VOID 	Npc_SetToFistMode 		(VAR C_NPC self ) { };
// Setzt den NSC beim Start in den Faustkampfmodus (zB.Monster)
FUNC VOID 	Npc_SetToFightMode 		(VAR C_NPC self, VAR INT weapon ) { };
// Setzt den NSC beim Start in den der Waffe entsprechenden Kampfmodus (Waffe wird erzeugt)
FUNC INT	Npc_IsInFightMode		(VAR C_NPC self, VAR INT fmode)	  { return 0; };
// liefert eins zurück, wenn der Charakter im angegebenen Fightmode (z.B. FMODE_MAGIC) ist

// ----------------------------------------- WEAPONS -----------------------------------------

FUNC VOID 	AI_DrawWeapon 				(VAR C_NPC n0 ) { };
// Equipte Waffe wird gezogen
FUNC VOID 	AI_ReadyMeleeWeapon			(VAR C_NPC self) {};
// Ziehe equippte Nahkampfwaffe
FUNC VOID 	AI_ReadyRangedWeapon		(var C_NPC self) {};
// Ziehe equippte Fernkampfwaffe
FUNC VOID 	AI_RemoveWeapon 			(VAR C_NPC n0 ) { };
// Gezogene Waffe wird weggesteckt
// -------------------------------------------------------------------------------------------
FUNC C_Item Npc_GetReadiedWeapon		(VAR C_NPC n0 ) { };
// Liefert die gezogene Waffe des NSCs.
// -------------------------------------------------------------------------------------------
FUNC INT  	Npc_HasReadiedWeapon		(VAR C_NPC self) { return 0; };
// gibt eins zurück, wenn die Waffe schon in der Hand ist
FUNC INT  	Npc_HasReadiedMeleeWeapon 	(VAR C_NPC self) { return 0; };
// Nahkampfwaffe in der Hand
FUNC INT  	Npc_HasReadiedRangedWeapon	(VAR C_NPC self) { return 0; };
// Fernkampfwaffe in der Hand
FUNC INT	Npc_HasRangedWeaponWithAmmo	(VAR C_NPC npc)
// Gibt TRUE zurück, wenn 'npc' irgendeine Fernkampfwaffe im Inventory oder in der Hand hat UND
// dazu auch passende Munition vorhanden ist. Sonst FALSE.

// ----------------------------------------- ANGRIFF -----------------------------------------
FUNC INT  	Npc_GetTarget				(VAR C_NPC self)
// Befüllt 'other' mit dem aktuellen Ziel. Das aktuelle Ziel wird intern gespeichert, wird 
// durch Npc_SetTarget() bzw. Npc_GetNextTarget() gesetzt.
// - return:	aktuelles Ziel gespeichert 	-> TRUE
//				kein Ziel gespeichert		-> FALSE
FUNC INT 	Npc_GetNextTarget 			(var C_NPC self ) 
// Aktive Suche nach einem Gegner. Wird ein Gegner gefunden, so wird er als internes Ziel
// übernommen und in 'other' geschrieben, wenn kein Gegner gefunden wurde wird das
// interne Ziel gelöscht und 'other' ungültig gemacht.
// Kriterien:	1. gesetzter Gegner, der nicht tot/bewußtlos ist wird genommen...
//				2. ...dann der nächstbeste aNSC demgegenüber 'self' HOSTILE ist (auch weder tot noch bewußtlos)
// - return:	neues Ziel gefunden	-> TRUE
//				kein Ziel gefunden	-> FALSE
// VORSICHT:	Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
//				Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
//				Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden
FUNC INT 	Npc_IsNextTargetAvailable 	(var C_NPC self )
// Sucht genauso wie Npc_GetNextTarget nach einem neuen Ziel, überschreibt aber weder
// das interne Ziel, noch 'other'
// - return:	neues Ziel gefunden	-> TRUE
//				kein Ziel gefunden	-> FALSE
// VORSICHT:	Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
//				Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
//				Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden
FUNC VOID 	Npc_SetTarget 				(var C_NPC self, VAR C_NPC other )
// Gibt dem Nsc 'self' das interne Ziel 'other'. --> Nur wenn per GetTarget auch der other "geholt" wird ist er vorhanden, da hier interne Variablen, die in den Skripten nicht vorhanden sind verwendet werden
FUNC VOID 	AI_Attack					(VAR C_NPC self)
// Startet Kampf AI (sollte in der ZS_Attack-Loop stehen)
// Es wird das interne Ziel verwendet, das mit Npc_SetTarget() oder Npc_GetNextTarget() gesetzt
// wurde.
FUNC VOID 	AI_FinishingMove 			(var C_NPC self, VAR C_NPC other )
// Führt den logischen Finishing Move inklusive Anis aus den Skripten heraus aus
func void	AI_Defend					(var C_NPC self)
// Der Befehl ist als Overlay-Message implementiert. Das heisst, dass er neben anderen Nachrichten 
// aktiv bleibt. Er wird erst beendet, wenn der NPC eine Parade (ausgeloest durch die Attacke eines
// anderen NPCs) durchgefuert hat.





func void	AI_Flee						(var C_NPC self) {};
// Der Befehl muss, wie AI_Attack(), in der ZS-Loop regelmaessig aufgerufen werden und setzt voraus, dass
// vorher mit Npc_SetTarget( self, <var c_npc enemy> ) ein Gegner gesetzt wurde, vor dem der Npc fliehen soll.
func void  	AI_AimAt					(VAR C_NPC attacker, VAR C_NPC target )	{};
// NPC zielt mit Fernkampfwaffe auf Target-NPC
func void 	AI_ShootAt					(VAR C_NPC attacker, VAR C_NPC target )	{ };
// NPC feuert mit Fernkampfwaffe auf Target-NPC
func void	AI_StopAim					(VAR C_NPC attacker )	{ };
// NPC beendet vorher gestartetes Zielen mit Fernkampfwaffe.
// ------------------------------------- SONSTIGE CHECKS ------------------------------------

func INT  	Npc_AreWeStronger			(VAR C_NPC self, VAR C_NPC other)			{ return 0; };
// ermittelt den stärkeren Nsc, 
//Wenn die Summe der Level aller NPCs (human, Monster), die zu mir feindlich sind und die zu <other> freundlich sind
//MEHR ALS DOPPELT SO HOCH ist (x  > 2*y), wie die Summe der Level aller Leute, die zu mir freundlich sind, und die
//zu <other> feindlich sind, dann NEIN,
//sonst JA (= wir SIND stärker, nicht echt, aber ab doppelter Gegnerstärke zu fliehen ist früh genug, sonst kommen 
//die Jungs aus dem Rennen nicht mehr raus 
//Beachten:
//1) Monster können feindlich zu beiden Menschen sein --> egal, zählen auf beiden Seiten
//2) Jemand, der z.B. zu mir freundlich und zu <other> feindlich ist, wird demnach doppelt gewertet --> ok so
func INT	Npc_IsAiming				(VAR C_NPC self, VAR C_NPC other) { return 0; };
// liefert eins zurück, wenn der zweite Character auf den ersten zielt (Fernkampfwaffen und Zauber)



// *******************************************************************************************
// ***                                       ITEMS                                         ***
// *******************************************************************************************

// ------------------------------------ ITEMS IN DER WELT ------------------------------------
FUNC VOID 	Wld_InsertItem				(VAR INT itemInstance, VAR STRING spawnPoint) 		{ };
// Füge Item in Welt ein entweder an einem WP oder einem FP
// Vorsicht, funktioniert nicht, Items werden immer im Mittelpunkt der Welt inserted
FUNC VOID	AI_LookForItem				(VAR C_NPC self, VAR INT instance) {};
// gibt die Möglichkeit nach bestimmten Items zu suchen (z.B.:Das goldene Schwert der Zerstörung, wenn vorhanden)
func INT  	Wld_RemoveItem          	(VAR C_ITEM item)		{ };
// Hiermit wird das angegebene Item aus der Welt entfernt und gelöscht

// ---------------------------------------- INVENTORY ----------------------------------------
FUNC VOID 	CreateInvItem 				(VAR C_NPC n0, VAR INT n1 ) { };
//  gibt der angegebenen Instanz (self oder other) ein Item ins Inventory
FUNC VOID 	CreateInvItems 				(VAR C_NPC n0, VAR INT n1, VAR INT n2 ) { };
// wie vorherige Funktion, nur das Multi-Items (z.B.: 100 Pfeile) ins Inventory kreiert werden
FUNC C_Item Npc_GetInvItem				(VAR C_NPC self, VAR INT itemInstance) 	{};
// Ermittle ItemInstanz aus Inventory
FUNC INT	Npc_HasItems				(VAR C_NPC n0, VAR INT itemInstance) { return 0; };
// Liefert "1", wenn NSC die angegebene Anzahl von Items besitzt.-> NpcHasItem ist damit obsolete
func INT  	Npc_GetInvItemBySlot    	(VAR C_NPC self, VAR INT category, VAR INT slotNr)
// Mit diesem Befehl läßt sich nachsehen, ob in einem bestimmten Slot einer bestimmten Kategorie ein item vorhanden ist
// ist das der Fall, wird dieses Item in die globale Variable item geschrieben
// gibt jetzt die Anzahl zurueck, wenn das Item stackable ist
//. Den Transfer machst Du dann per Npc_RemoveInvItems() und Npc_CreateInvItems().
func void 	Npc_RemoveInvItem			(VAR C_NPC owner, VAR int itemInstance )	{};
// das angegebene Item wird aus dem Inventory des NSCs entfernt und gelöscht
func void	Npc_RemoveInvItems			(VAR C_NPC owner, VAR int itemInstance, VAR INT amount )	{ };
// das angegebene Anzahl des Multi-Items wird aus dem Inventory des NSCs entfernt und gelöscht

// ------------------------------------------ TRUHEN ----------------------------------------- 
FUNC VOID 	Mob_CreateItems				(VAR STRING mobName, VAR INT itemInstance, VAR INT amount) {};
// Erzeuge "amount" Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen.
FUNC INT  	Mob_HasItems				(var STRING mobName, VAR INT itemInstance ) { return 0; };
// Liefere Anzahl der Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen

// ------------------------------------ EQUIPPEN / UNEQUIPPEN --------------------------------
FUNC VOID 	EquipItem 					(VAR C_NPC n0, VAR INT n1 ) { };
// gibt dem Nsc direkt das angegebene Item an den Gürtel
FUNC VOID 	AI_EquipBestMeleeWeapon		(VAR C_NPC self) {};
// sucht im Inventory nach der besten Nahkampfwaffe und hängt sie an den Gürtel
FUNC VOID 	AI_EquipBestRangedWeapon	(VAR C_NPC self) {};
// sucht im Inventory nach der besten Fernkampfwaffe und ploppt sie auf den Rücken der Instanz
FUNC VOID 	AI_EquipBestArmor			(VAR C_NPC self) {};
// Wunder, Wunder hier wird die beste im Inventory vorhandene Rüstung angezogen
FUNC VOID 	AI_UnequipWeapons			(VAR C_NPC self) {};
// Unequippe alle Waffen
FUNC VOID 	AI_UnequipArmor				(VAR C_NPC self) {};
// Unequippe aktuelle Rüstung
func void	AI_EquipArmor				(VAR C_NPC owner, VAR C_ITEM armor_from_owners_inventory )
// Ziehe die angebene Rüstung dem NSC "owner" an, diese muss sich in seinem Inventory befinden.
// -------------------------------------------------------------------------------------------
FUNC C_Item Npc_GetEquippedMeleeWeapon 	(VAR C_NPC n0 ) { };
// Liefert die gegurtete Nahkampfwaffe des NSCs.
FUNC C_Item Npc_GetEquippedRangedWeapon (VAR C_NPC n0 ) { };
// Liefert die gegurtete Fernkampfwaffe des NSCs.
FUNC C_Item Npc_GetEquippedArmor 		(VAR C_NPC n0 ) { };
// Liefert die angelegte Rüstung des NSCs.
// -------------------------------------------------------------------------------------------
FUNC INT  	Npc_HasEquippedWeapon		(VAR C_NPC self) { return 0; };
// gibt eins zurück, wenn die abgefragte Instanz (self oder other) eine Waffe sichtbar mit sich rumträgt
FUNC INT  	Npc_HasEquippedMeleeWeapon	(VAR C_NPC self) { return 0; };
// gibt eins zurück, wenn eine Nahkampfwaffe Equipped ist
FUNC INT  	Npc_HasEquippedRangedWeapon	(VAR C_NPC self) { return 0; };
// Fernkampwaffe auf dem Rücken True/False
FUNC INT  	Npc_HasEquippedArmor		(VAR C_NPC self) { return 0; };
// Armor angezogen True/False

// ------------------------------------------- BESITZ ---------------------------------------
FUNC INT  	Npc_OwnedByNpc				(VAR C_ITEM item, VAR C_NPC npc) { return 0; };
// Liefert "1", wenn dem NSC das Item gehört ( persönliches Besitzflag )
FUNC INT  	Npc_OwnedByGuild			(VAR C_ITEM item, VAR int guild) { return 0; };
// Liefert "1", wenn der angegebenen Gilde das Item gehört ( Gildenbesitz )
FUNC INT 	Npc_IsDetectedMobOwnedByNpc	(var C_NPC user, VAR C_NPC owner ) { return 0; };
// Liefert >0, falls "owner" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
FUNC INT 	Npc_IsDetectedMobOwnedByGuild(var C_NPC user, VAR INT ownerguild ) { return 0; };
//Liefert >0, falls "ownerguild" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
// ------------------------------------------ TRADE -----------------------------------------
FUNC VOID 	Npc_GiveItem 				(VAR C_NPC n0, VAR C_ITEM n1, VAR C_NPC n2 ) { };
//Der NSC "self" gibt den NSC "other" den angegebenen Gegenstand "item". Der Gegenstand wandert sofort ins Inventory des anderen.
FUNC INT 	Npc_StartItemReactModules	(VAR C_NPC self, VAR C_NPC other, VAR C_ITEM item) { return 0; };
// Prüfe alle ItemReact-Module von "self" auf Gegenstand "item" von Geber "other" und starte passende Reaction-Funktion 
//liefert True beim finden eines Moduls
FUNC INT  	Npc_HasOffered				(VAR C_NPC self, VAR C_NPC other, VAR INT itemInstance) { return 0; };
//Bietet Spieler dem NSC einen Gegenstand übers Trade-Modul an ? True/False

// ---------------------------------------- MAPS & DOCS -------------------------------------
FUNC VOID 	Doc_Open					(VAR STRING picName ) {};
// Öffnet ein Dokument mit dem Bild "picName" als Hintergrund
FUNC VOID 	Doc_Font					(VAR STRING fontName ) {};
// Benutzt den angegebenen Font für nachfolgende "Doc_Print"s.
FUNC VOID 	Doc_Print					(VAR STRING text ) {};
// Angegebener Text wird in das aktuelle (vorher mit Doc_Open geöffnete) Dokument geschrieben
FUNC VOID 	Doc_MapCoordinates			(VAR STRING levelName, VAR FLOAT gamex1, VAR FLOAT gamey1, VAR FLOAT pixelx1, VAR FLOAT pixely1, VAR FLOAT gamex2, VAR FLOAT gamey2, VAR FLOAT pixelx2, VAR FLOAT pixely2) {};
// Karte initialisieren : levelName = Name des Levels (.ZEN) aus dem die Karte einen Ausschnitt zeigt 
// gamex1,gamey1 : Koordinaten im Spiel (linke obere Ecke der Karte) 
// pixelx1,pixely1 : PixelKoordinaten linke obere Ecke // der Rest : alles nochmal für die rechte untere Ecke



// *******************************************************************************************
// ***                                     DIALOGE                                         ***
// *******************************************************************************************

FUNC VOID	AI_Output					(VAR C_NPC self, VAR C_NPC target, VAR STRING outputName) {};
// Cutscene mit entsprechender Id wird abgenudelt
FUNC VOID	AI_OutputSVM				(VAR C_NPC self, VAR C_NPC target, VAR STRING svmname) {};
// Outputbefehl um Svms abzuspielen
FUNC VOID	AI_OutputSVM_Overlay		(VAR C_NPC self, VAR C_NPC target, VAR STRING svmname) {};
// wie AI_OutputSVM, wartet jedoch NICHT mit der Ausführung des nächsten AI_...-Befehls, bis
// das SVM zuende gespielt wird. (Für Kommentare kurz vor und während dem Kampf!)
FUNC VOID	AI_WaitTillEnd				(VAR C_NPC self, VAR C_NPC other) {};
// 'self' wartet bis 'other' seinen laufenden AI-Befehl zu Ende gespielt hat (funzt allerdings nicht bei AI-Overlays!)
// -------------------------------------------------------------------------------------------
FUNC VOID 	AI_Ask 						(VAR C_NPC self, VAR FUNC anserYes, VAR FUNC answerNo ) { };
// die angegeben Instanz (self), gibt eine Frage aus und verweist auf die selbst zu definierenden Funktionen, 
// die für die Fälle Spieler sagt ja (Daumen nach oben) und Spieler sagt nein (Daumen unten) vorhanden sein müssen
FUNC VOID 	AI_AskText              	(VAR C_NPC self, VAR FUNC funcYes, VAR FUNC funcNo, VAR STRING strYes, VAR STRING strNo);
// wie AI_Ask, nur das außer den Funktionen auch noch Strings für die Antworten Ja/Nein mit angegeben werden können
FUNC VOID 	AI_WaitForQuestion			(var C_NPC self, VAR FUNC scriptFunc) { };
// NSC wartet 20 Sekunden, wird er in dieser Zeit vom Spieler angesprochen, wird die angegebene SkriptFunktion ausgeführt.
// -------------------------------------------------------------------------------------------
FUNC VOID 	Npc_SetRefuseTalk			(VAR C_NPC self, VAR INT timeSec) { };
// Dialog Refuse Counter aus "x" Sekunden setzen
FUNC INT  	Npc_RefuseTalk				(VAR C_NPC self) { return 0; };
// Abfrage ob Dialog-Refuse Counter noch aktiv ist True/False



// *******************************************************************************************
// ***                                        NEWS                                         ***
// *******************************************************************************************

FUNC VOID	Npc_MemoryEntry				(VAR C_NPC self, VAR INT source, VAR C_NPC offender, VAR INT newsid, VAR C_NPC victim) { };
// mit dieser Funktion wird eine NSC bezogene News geschrieben newsid		:	News ID source >	0	:	"gossip", ansonsten	"witness",
// self:	NSC, bei dem News eingespeist werden soll, other:	Täter victim :	Opfer
FUNC VOID	Npc_MemoryEntryGuild		(VAR C_NPC self, VAR INT source, VAR C_NPC offender, VAR INT newsid, VAR C_NPC victimguild) { };
// wie MemoryEntry nur, das die Memory für die ganze Gilde kreiert wird
FUNC INT    Npc_HasNews					(VAR C_NPC self, VAR INT newsID, VAR C_NPC offender, VAR C_NPC victim)	{};
// Liefert Newsnummer>0 (für weitere Referenzen) falls entsprechende News vorhanden.
// Nicht benötigte Parameter können mit "NULL" leergelassen werden
FUNC INT    Npc_IsNewsGossip			(VAR C_NPC self, VAR INT newsNumber) {};
// Liefert >0, falls News "Gossip" ist
FUNC C_NPC  Npc_GetNewsWitness			(VAR C_NPC self, VAR INT newsNumber) {};
// Ermittle Zeuge der News und gibt eine INstanz der Klasse C_Npc zurück
FUNC C_NPC  Npc_GetNewsVictim			(VAR C_NPC self, VAR INT newsNumber) {};
// Ermittle Opfer der News und gibt eine INstanz der Klasse C_Npc zurück
FUNC C_NPC 	Npc_GetNewsOffender 		(VAR C_NPC self, VAR INT newsNumber ) { return 0; };
// Ermittelt Täter der News und gibt eine INstanz der Klasse C_Npc zurück

// *******************************************************************************************
// ***                                     MISSIONS                                        ***
// *******************************************************************************************

FUNC INT 	Npc_IsDead 					(VAR C_NPC n0 ) { return 0; };
// Funktion liefert einen Wert zurück, falls Dead true ist
FUNC INT	Npc_KnowsInfo 				(VAR C_NPC self, VAR INT infoInstance) { return 0; };
// Liefert TRUE, wenn der angegebene Spieler die Info schon einmal erhalten hat.
// VORSICHT: auch wenn eine permanente Info schon einmal dem Spieler erzählt wurde, so gibt diese Funktion trotzdem FALSE zurück!
FUNC INT  	Npc_CheckInfo				(VAR C_NPC npc, var int important) { return 0; };
// Überprüft,ob der NSC gültige Infos für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").
FUNC INT  	NPC_GiveInfo				(VAR C_NPC npc, VAR INT important) { return 0; };
// Überprüft,ob der NSC eine (!) gültige Info für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").
FUNC INT  	Npc_CheckAvailableMission	(VAR C_NPC npc, VAR INT missionState, VAR INT important) { return 0; };
// Prüfe ob zwischen dem NSC eine aktuelle Mission (AVAILABLE,RUNNING) besteht und vergleiche den Status mit den angegebenen. 
// Falls dieser identisch ist, wird "1" geliefert.
FUNC INT  	Npc_CheckRunningMission		(VAR C_NPC npc, VAR INT important) { return 0; };
// Überprueft ob zwischen dem NSC und dem Spieler eine laufende Mission besteht und startet den entsprechenden Skriptblock. 
// (Returnwert 1 : Mission aktiv)
FUNC INT  	Npc_CheckOfferMission		(VAR C_NPC npc, VAR INT important) { return 0; };
// Überprueft ob der NSC dem Spieler einen Auftrag anbieten kann, wenn ja, wird der Offer-Block gestartet und "1" zurückgeliefert.
FUNC VOID 	Mis_SetStatus				(VAR INT missionName, VAR INT newStatus ) {};
// Setzt den Status einer Mission ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc. )
FUNC INT  	Mis_GetStatus				(VAR INT missionName ) { return 0; };
// Liefert aktuellen Status einer Mission zurück ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc.
FUNC INT  	Mis_OnTime					(VAR INT missionName ) { return 0; };
// Liefert TRUE, wenn sich Spieler noch innerhalb des Zeitlimits für diese Mission befindet
func void	AI_StopProcessInfos			(var C_NPC npc)
// Der DialogModus wird beendet (Multiple Choice-Dialog)


// *******************************************************************************************
// ***                                     PLAYER                                          ***
// *******************************************************************************************

FUNC INT	Npc_IsPlayer				(VAR C_NPC player)			{ return 0; };
// liefert eins zurück, wenn der geprüfte Charakter der Spieler himself ist
FUNC INT	Wld_DetectPlayer			(VAR C_NPC self) { return 0; };
// liefert eins zurück, wenn der Player in der Nähe ist
FUNC INT	Npc_HasDetectedNpc			(VAR C_NPC self, VAR C_NPC other) { return 0; };
// liefert eins zurück, wenn der Spieler in Sinnesreichweite und entdeckt ist                                     // SPIELER ???



// *******************************************************************************************
// ***                                  ENTFERNUNGEN                                       ***
// *******************************************************************************************

FUNC INT	Npc_IsNear					(VAR C_NPC self, VAR C_NPC  other)	{ return 0; };
// liefert eins zurück, wenn geprüfte Instanz sich im Bereich von drei Metern zur prüfenden Instanz befindet
FUNC INT  	Npc_GetDistToNpc			(VAR C_NPC npc1, VAR C_NPC npc2) { return 0; };
// Liefert Entfernung ( ín cm ! ) zwischend den beiden NSCs
FUNC INT	Npc_GetDistToWP				(VAR C_NPC self, VAR STRING wpName)
// liefert die Entfernung vom NSC 'self' zum angegebenen Waypoint in cm
FUNC INT  	Npc_GetDistToItem			(VAR C_NPC npc, VAR C_ITEM item) { return 0; };
// Liefert Entfernung ( ín cm ! ) zwischend NSC und Gegenstand
FUNC INT  	Npc_GetDistToPlayer			(VAR C_NPC npc1) { return 0; };
// Liefert Entfernung ( ín cm ! ) zwischend den beiden NSCs
FUNC INT  	Snd_GetDistToSource			(VAR C_NPC self) { return 0; };
// Liefert Entfernung ( in cm ! ) zum letzten logischen Sound



// *******************************************************************************************
// ***                             GILDEN / ATTITUEDEN / KENNE SC                          ***
// *******************************************************************************************

FUNC INT    Npc_GetTrueGuild			(VAR C_NPC npc)  { return 0; };
// liefert immer!!! die wahre Gilde zurück, ignoriert also auch ohne die Regeln die Verkleidung
FUNC VOID	Npc_SetAttitude				(VAR C_NPC self, VAR INT att) { };
//  setzt die permanente Attitüde auf einen festen Wert
FUNC VOID	Npc_SetTempAttitude			(VAR C_NPC self, VAR INT att) { };
//   setzt die temporäre Attitüde auf einen Wert (att)
FUNC INT	Npc_GetAttitude				(VAR C_NPC self, VAR C_NPC other) { return 0; };
// Gibt die Attitüde von self zu other zurück (temp. / perm. / Gilden )
FUNC INT  	Npc_SetTrueGuild			(var C_NPC npc, VAR INT guildID)  { };
// Setzt die wahre Gilde des NSCs
FUNC VOID 	Wld_SetGuildAttitude		(var INT guild1, VAR INT attitude, VAR INT guild2) { };
// Setze Gildenattitude neu
FUNC INT  	Wld_GetGuildAttitude		(var INT guild1, VAR INT guild2) { return 0; };
// Ermittle Gildenattitude
FUNC INT  	Npc_GetPermAttitude			(var C_NPC self, VAR C_NPC other) {};
// Ermittle die permanente Attitude von "self" zu "other"
FUNC VOID 	Wld_ExchangeGuildAttitudes	(VAR STRING name) 					{ };
// Tausche Gilden-AttitudenTabelle aus.
FUNC INT 	Npc_GetGuildAttitude 		(VAR C_NPC npc, VAR C_NPC npc ) { return 0; };
// Ermittelt die Gildenattitüde von zwei Nsc´s direkt im Gegensatz zu Wld_GetGuildAttitude
// -------------------------------------------------------------------------------------------
FUNC VOID 	Npc_SetKnowsPlayer			(var C_NPC self, VAR C_NPC player) { };
// NSC kennt SC
FUNC INT  	Npc_KnowsPlayer				(VAR C_NPC self, VAR C_NPC player) { return 0; };
// Kennt der NSC den Spieler? True/False



// *******************************************************************************************
// ***                                     	   TA / ZS                                     ***
// *******************************************************************************************

// ---------------------------------------------- ZS -----------------------------------------
FUNC VOID	AI_StartState				(VAR C_NPC self, VAR FUNC what, VAR INT stateBehaviour, VAR STRING wpName) {};
// Versetzt den Nsc aus den Skripten heraus in den entsprechenden ZS (what), 
// stateBehaviour sagt : "0"-aktuellen Zustand abbrechen "1"-aktuellen Zustand erst ordnungsgemäß beenden (End-Funktion aufrufen) ).
FUNC VOID 	Npc_ClearAIQueue 			(var C_NPC self ) { };
// hiermit werden alle Befehle mit sofortiger Wirkung aus der AI_Queue rausgeworfen
FUNC VOID   AI_SetNpcsToState			(var C_NPC self, VAR FUNC aiStateFunc, VAR INT radius) 	{};
// Setze alle NSCs im Umkreis von x cm in entsprechenden AI-Zustand
// VORSICHT: Diese Funktion ist Buggy, wird aber derzeit auch NICHT verwendet!
// -> FINGER WEG!!! (SN)
// -------------------------------------------------------------------------------------------
FUNC INT	Npc_IsInState				(VAR C_NPC self, VAR FUNC state) { return 0; };
// Abfrage auf den aktuellen Zustand einer Spielfigur True/False
FUNC INT	Npc_WasInState				(VAR C_NPC self, VAR FUNC state) { return 0; };
// Abfrage auf den vorherigen Zustand einer Spielfigur True/False

// ---------------------------------------------- TA -----------------------------------------
FUNC VOID	TA							(var c_npc self, var int start_h, var int stop_h, VAR func state, VAR string waypoint) { };
// Mit _(Zustandsname) wird ein neuer Tagesablauf generiert, siehe TA.d
FUNC VOID 	TA_Min						(var c_npc self, var int start_h, var int start_m, var int stop_h, var int stop_m, VAR func state, VAR string waypoint) { };
// Tagesablaufpunkt minutengenau angeben
FUNC VOID 	AI_ContinueRoutine			(var C_NPC self) 					{ };
// Setze Tagesablauf fort
// Enthält Standup
// -------------------------------------------------------------------------------------------
FUNC INT	Npc_IsInRoutine				(VAR C_NPC self, VAR FUNC state) { return 0; };
// Check ob der angegebene Zustand der aktuelle TA des NSCs ist. True/FAlse
FUNC VOID 	Npc_ExchangeRoutine			(VAR C_NPC self, VAR STRING routineName ) {};
// Tausche meherere Tagesabläufe aus

// ------------------------------------- OBJEKT-TAs ------------------------------------------
FUNC VOID 	Wld_SetObjectRoutine		(var INT hour1, VAR INT min1, VAR STRING objName, VAR INT state) { };
// _Ein_ Objekt mit diesem _Vobnamen_ wird getriggert
FUNC VOID 	Wld_SetMobRoutine			(var INT hour1, VAR INT min1, VAR STRING objName, VAR INT state) { };
// _Alle_ Mobs mit diesem _Schemanamen_ werden getriggert.

// ------------------------------------- Overlay-TAs -----------------------------------------
FUNC VOID 	Rtn_Exchange				(VAR STRING oldRoutine, VAR STRING newRoutine ) {};
// Tausche aktuellen Tagesablauf des NSC "self" gegen angegebenen aus 
// (Name wird automatisch mit "RTN_" am Anfang und NSC-Skript-ID am Ende ergänzt)
FUNC VOID 	TA_BeginOverlay				(VAR C_NPC self ) {};
// Melde einen Overlay-Tagesablauf an
FUNC VOID 	TA_EndOverlay				(VAR C_NPC self ) {};
// Beende einen Overlay-Tagesablauf 
FUNC VOID 	TA_RemoveOverlay			(VAR C_NPC self ) {};
// Entferne aktiven TA-Overlay



// *******************************************************************************************
// ***                                       SCALING                                       ***
// *******************************************************************************************

FUNC VOID	Mdl_SetModelScale			(VAR C_NPC self, VAR FLOAT x, VAR FLOAT y, VAR FLOAT z) {}; 
// zum skalieren des Meshes (Breite,Höhe,Tiefe) 1 = 100%, also Normalgröße!
func VOID 	Mdl_SetModelFatness			(var C_NPC self, VAR FLOAT fatness) 	{};
// Setzt Model-Z-Skalierung



// *******************************************************************************************
// ***                                      ATTRIBUTE                                      ***
// *******************************************************************************************

FUNC VOID	Npc_ChangeAttribute			(var C_NPC self, VAR INT atr, VAR INT value) {};
// ändert den Wert des Attributs (atr) um (value) Einheiten.



// *******************************************************************************************
// ***                                       TALENTE                                       ***
// *******************************************************************************************

FUNC INT 	Npc_HasTalent				(VAR C_NPC self, VAR INT tal) 		{ return 0; };
// liefert eins zurück, wenn der geprüfte Charakter das Talent tal hat
FUNC INT 	Npc_HasFightTalent			(VAR C_NPC self, VAR INT tal) 		{ return 0; };
// Spezialabfrage auf Kampftalente (z.B. 1hSword) ansonsten wie Npc_HasTalent 



// *******************************************************************************************
// ***                                        MAGIE                                        ***
// *******************************************************************************************

FUNC VOID	Npc_CreateSpell				(VAR C_NPC self, VAR INT spellnr) {};
// NSc bekommt Zauberspruch zugewiesen kann diesen aber noch nicht gebrauchen 
// (erscheint ausgegraut im Auswahlkranz)
FUNC VOID	Npc_LearnSpell				(VAR C_NPC self, VAR INT spellnr) {};
// Was könnte das wohl heißen ? Ich glaube damit kann man den (spellnr) Zauber zuweisen
FUNC VOID	Npc_SetTeleportPos			(VAR C_NPC self) {};
// Magie/Zauberstein Teleport Spruch : letzte Position des NSCs merken, zwecks späteren dahinbeamens.
FUNC INT	Npc_GetActiveSpell			(var C_NPC self) { return 0; };
// liefert den Zauber zurück, der auf der Hand ist (self oder other)
// liefert -1 zurück, wenn kein Zauber auf der Hand ist
FUNC INT	Npc_GetActiveSpellCat		(VAR C_NPC self) { return 0; };
// Unterscheidet zwischen den drei Kategorien (Spell_Bad, Spell_neutral,Spell_Good) Spellkat ist Rückgabewert
FUNC INT 	Npc_SetActiveSpellInfo 		(VAR C_NPC npc, VAR INT i1 ) { return 0; };
// Hier kann ein Wert für den Zauberspruch gesetzt werden. Was dieser Wert bewirkt, haengt allein von der Nutzung im
// Skript ab. Das Programm  benutzt diesen nicht.
FUNC INT	Npc_GetActiveSpellLevel		(VAR C_NPC self)
// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist 

func VOID 	AI_ReadySpell  				(VAR C_NPC self, VAR INT spellID, VAR INT investMana);
// Lasse zauberspruch auf Hand erscheinen.
func VOID 	AI_UnreadySpell				(VAR C_NPC self);
// lasse zauberspruch aus Hand verschwinden
func INT  	Npc_HasSpell   				(VAR C_NPC self, VAR INT spellID);
// Kann der NSC den angegebenen Zauberspruch benutzen ?


// *******************************************************************************************
// ***                                   WAHRNEHMUNGEN                                     ***
// *******************************************************************************************

FUNC VOID	Npc_PercEnable				(VAR C_NPC self, VAR INT percID, VAR FUNC function) { }; 
// Aktiviere Perception
FUNC VOID 	Npc_PercDisable				(VAR C_NPC self, VAR INT percID) { }; 
// Deaktiviere Perception
FUNC VOID 	Npc_SetPercTime				(VAR C_NPC self, VAR FLOAT seconds)			{ };
// Setze Zeitdelta für aktive Wahrnehmungen, alle Zeitdelta-Sekunden wird  WN gesendet 
// -------------------------------------------------------------------------------------------
FUNC VOID 	Perc_SetRange				(VAR INT percID, VAR INT range)				{ };
// Setze Reichweite fuer eine passive Wahrnehmung int cm
FUNC VOID 	Npc_SendPassivePerc 		( VAR C_NPC npc1, VAR INT Perc_type, VAR C_NPC npc2, VAR C_NPC npc3 ) { };
// Sende eine passive Wahrnehmung aus.Npc1 = wer schickt Npc2 = Opfer, Npc3 = Täter
FUNC VOID   Npc_SendSinglePerc			(VAR C_NPC self, VAR C_NPC target, VAR INT percID) 	{};
// verschicke Wahrnehmung an einzelnen NSC
// --------------------------------------------------------------------------------------------
FUNC VOID 	Npc_PerceiveAll				(VAR C_NPC self) 		  {};
// Nimm alle Objekte in Wahrnehmungsreichweite wahr, die dann mit WLD_DetectNpc und Wld_DetectItem auswertbar sind
FUNC INT   	Wld_DetectNpc   			(VAR C_NPC self, VAR INT instance, VAR func aiState, VAR INT guild) { return 0; };
// Diese Methode initilisiert die globale Skriptvariable "other" mit einem NSC, den "self" beim letzten Warnehmungscheck wargenommen hat.
// instance      = Name der zu suchenden Instanz							( "-1" angeben, wenn Instanzname unwichtig )
// guild         = Der zu suchende NSC muss Mitglied dieser Gilde sein		( "-1" angeben, wenn Gilde unwichtig )
// aiState       = Der AI-Zustandsname, in dem sich der NSC befinden soll	( NOFUNC angeben, wenn AI-State unwichtig )
// Wenn die Methode einen entsprechenden NSC gefunden hat, liefert diese "1" und 'other' ist initialisiert
// ansonsten wird "0" geliefert und "other" wird nicht verändert.
VAR INT 	Wld_DetectNpcEx ( VAR C_NPC self, VAR INT npcInstance, VAR FUNC aiState, VAR INT guild, VAR INT detectPlayer)
// Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.
FUNC INT 	Wld_DetectItem				(VAR C_NPC self, VAR INT flags) { return 0; };
// liefert eins zurück, wenn ein Item mit dem Entsprechende Flag (z.B.ITEM_KAT_FOOD )gefunden wurde
// Globale Variable 'item' wird mit dem gefundenen Gegenstand initialisiert
FUNC STRING Npc_GetDetectedMob 			(var C_NPC self ) { return 0; };
// liefert den Schemanamen des Mobsi zurück, das der Nsc entdeckt hat. Schemaname ist das String-Kürzel des Mobs, daß im Visualnamen vor dem ersten "_"-Zeichen steht, also z.B. "DOOR" wenn der Visualname "DOOR_OCR__135" ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
// WORKAROUND: zur Zeit werden hiermit nur Tür-MOBSIs zurückgegeben, dies soll aber wieder zurückgebaut werden
// --------------------------------------------------------------------------------------------
FUNC INT  	Npc_CanSeeNpc				(VAR C_NPC npc1, VAR C_NPC npc2) { return 0; };
// Prueft ob Npc1 den Npc2 sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False Blickwinkelabhängig (+,-100°)+LOS von der Hüfte aus
FUNC INT  	Npc_CanSeeNpcFreeLOS		(VAR C_NPC self, VAR C_NPC other) 	{};
// Prüft ob NSC anderen NSC sehen kann, ohne dabei den Winkel zu berücksichtigen (nur LineOfSight-Check)
FUNC INT  	Npc_CanSeeItem				(VAR C_NPC npc1, VAR C_ITEM item) { return 0; };
// Prueft ob Npc1 den Gegenstand sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False
FUNC INT  	Npc_CanSeeSource			(VAR C_NPC self) { return 0; };
// Checkt, ob NSC die Soundquelle sehen kann True/False



// *******************************************************************************************
// ***                                    CUTSCENES                                        ***
// *******************************************************************************************

FUNC VOID   TA_CS 						(VAR C_NPC self, VAR STRING csName, VAR STRING roleName) {};
// Cutscene an den zuletzt angegebenen Tagesablaufpunkt hängen
// csName	: Name der Cutscene ( der Name des "CS" - Files )
// roleName : Die Rolle die der NSC dabei übernehmen soll.
FUNC VOID   AI_PlayCutscene				(VAR C_NPC self, VAR STRING csName) {};
// Eine Cutscene aus den Scripten heraus starten
FUNC INT    Hlp_CutscenePlayed			(VAR STRING csName) { return 0; };
// Abfrage, ob Cutscene schon gespielt wurde (0 = Nein / 1 = Ja)
FUNC INT 	Npc_IsInCutscene 			(var C_NPC self ) { return 0; };
// liefert eins zurück, wenn der entsprechende Nsc in einer Cutscene ist


// *******************************************************************************************
// ***                                      SOUND                                          ***
// *******************************************************************************************

FUNC VOID 	Snd_Play 					(VAR STRING s0 ) { };
// spielt einen Sound ab
FUNC VOID 	Snd_Play3D 					(VAR C_NPC n0, VAR STRING s1 ) { };
// spielt einen 3D-Sound ab.
// -------------------------------------------------------------------------------------------
FUNC INT  	Snd_IsSourceNpc				(VAR C_NPC self) { return 0; };
// Check, ob Quelle des letzten Sound NPC war (Return >0) und setzt "other" auf diesen NPC
FUNC INT  	Snd_IsSourceItem			(var C_NPC self) { return 0; };
// Check, ob Quelle des letzten Sound Item war (Return >0) und setzt "item" auf diesen Gegenstand


// *******************************************************************************************
// ***                                       PORTALRÄUME                                   ***
// *******************************************************************************************
FUNC VOID 	Wld_AssignRoomToGuild 		( VAR STRING s0, VAR INT guild ) { };
// Ordnet den Raum: 's0' der Gilde 'guild' zu
func VOID 	Wld_AssignRoomToNpc 		( VAR STRING s0, VAR C_NPC roomowner ) { };
// Ordnet den Raum: 's0' dem  speziellen Nsc 'roomowner' zu
FUNC C_NPC 	Wld_GetPlayerPortalOwner 	()
// liefert den NSC des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'
FUNC INT 	Wld_GetPlayerPortalGuild 	() 
// liefert Gilde des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert
FUNC C_NPC 	Wld_GetFormerPlayerPortalOwner ()
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'
FUNC INT 	Wld_GetFormerPlayerPortalGuild ()
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert
FUNC int 	Npc_IsPlayerInMyRoom ( VAR C_NPC npc )
// gibt TRUE zurück, wenn sich SC im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
FUNC int 	Npc_WasPlayerInMyRoom ( VAR C_NPC npc )
// gibt TRUE zurück, wenn sich SC vor dem letzten Raumwechsel im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen



// *******************************************************************************************
// ***                               sonstige Hilfsfunktionen                              ***
// *******************************************************************************************
func string	IntToString			( var int x) 	{ };
// !!!Funktion überträgt eine Integer variable in eine Stringvariable (Rückgabewert : string)???

func int	FloatToInt			( var float x)	{ };
// !!! konvertiert einen float in einen int, hier wäre es noch ganz schön zu wissen, wie mit dem float umgesprungen wird --> schneiden oder runden ???

func float	IntToFloat			( var int x)	{ };
// !!! wie float to int, nur daß ein integer in float umgewandelt wird ???
func string ConcatStrings		(var string str1, var string str2)
// !!!Erzeugt eine Kopie von dem ersten angegebenen String, hängt den zweiten an diesen an und gibt den neuen String zurück ???
 

//Die folgenden Methoden geben den Text nur unter bestimmten zusätzlichen Bedingungen aus. Für die Methoden mit dem Suffix Inst trifft dies zu, wenn die Instanz von der die Debug-Methode aufgerufen wird, zum Debuggen aktiviert wurde. Die Methoden mit dem Suffix Ch geben den Text nur aus, wenn der angegebene Channel zum Debuggen aktiviert wurde.

func void PrintDebugInst		(String text) 	{ };
// !!! gibt nur den Debugtext der eingeschalteten (SetDebugFocus --> siehe Intranetpage Erweitertes Skript-Debugging) Instanz aus ???
func void PrintDebugInstCh		(int ch, String text)	{ };
// !!! gibt nur in einem bestimmten channel liegende Debuginfos einer Instanz aus ???
func void PrintDebugCh			(int ch, String text)	{ };
// gibt nur den Text eines bestimmten Channels wieder, unabhängig von der Instanz ???

// *******************************************************************************************
// ***                               Log-Entries (ab v1.01)                                ***
// *******************************************************************************************
func void Log_CreateTopic( VAR STRING name, VAR INT section )
//	Der Befehl fügt unter der Sektion 'section' ein neues Topic mit Namen 'name' ein.
//	Sollte bereits ein Topic dieses Names in irgendeiner Sektion existieren, wird das Topic nicht hinzugefügt.
//
//	Parameter:
//	- name      Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Topics verwendet wird. 
//	- section	Eine, in der Constants.d definierte Konstante, die besagt, in welcher Sektion das Topic angezeigt werden soll.
//				Die Konstante kann folgende Werte annehmen: 
//					- LOG_MISSION
//					- LOG_NOTE 
 
func void Log_SetTopicStatus( VAR STRING name, VAR INT status )
//	Der Befehl ändert den Status des Topics 'name' in 'status'.
//	Er sollte nur für Topics verwendet werden, die für die Sektion LOG_MISSION erzeugt wurden.
//
//	Parameter:
//	- name      Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde. 
//	- status	Eine, in der Constants.d definierte Konstante, die besagt, unter welchem Status die Mission dargestellt werden soll.
//				Folgende Werte sind möglich: 
//					- LOG_RUNNING 
//					- LOG_SUCCESS 
//					- LOG_FAILED 
//					- LOG_OBSOLETE 
 
func void Log_AddEntry( VAR STRING topic, VAR STRING entry )
//	Der Befehl fügt dem Topics topic ein neues Entrz namens entry hinzu. Wie auch bei der Erzeugung von Topics dient hier der
//	Name sowohl zur Identifikation als auch zur Anzeige und muss somit eindeutig sein. Doppelte Einträge werden auch hier ignoriert.
//
//	Parameter
//	topic       Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde. 
//	entry		Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Eintrags verwendet wird. 



// *******************************************************************************************
// ***                               Docs (ab v1.01)		                               ***
// *******************************************************************************************
func int	Doc_Create		()
// erstellet ein Dokument und liefert ein Handle zurück, daß für alle weiteren Doc_...-Befehle nötig ist
func void	Doc_SetPages 	(var int handle, var int pages)
// setzt die Anzahl der Seiten dieses Dokuments. Bei mehr als zwei Seiten wird versucht ALLE
// Seiten auf den Bildschrim zu bekommen	
func void	Doc_SetPage 	(var int handle, var int page, var string pageimage , 0?)	
// setzt für die Seite 'page' die Grafik-Datei 'pageimage'. Diese muß im TGA-Format vorliegen und 
// z.B. "aufgeschlagenesBuch_links.tga"  heißen
// wird bei 'page' -1 übergeben, so bezieht sich der Befehl auf ALLE Seiten des Dokuments
func void	Doc_SetFont		(var int handle, var int page, var string fontname) 	
// setzt den Font für die angegebene Seite 'page' oder für das ganze Dokument ('page':-1)
// Fontname z.B. "Scrolls.tga"
func void	Doc_SetMargins	(var int handle, var int page, var int leftMargin, var int topMargin, var int rightMargin, var int bottomMargin, 1?); 
// setzt die Ränder (gerechnet vom Rand der TGA-Datei, die in Doc_SetPage() gesetzt wird). Die Ränder sind in Pixeln angegeben.
func void	Doc_PrintLine	(var int handle, var int page, var string text)
// Gibt Text für die Seite 'page' an. Der Text wird NICHT umgebrochen 
func void	Doc_PrintLines	(var int handle, var int page, var string text)
// Gibt Text für die Seite 'page' an. Der Text WIRD umgebrochen, falls er nicht auf die in Doc_SetPage() angegebene Grafik passt.
func void	Doc_Show	   	(var int handle)
 // nachdem das Dokument mit den obenstehenden Befehlen befüllt und formatiert wurde, wird es hiermit nun auf den Bildschirm geschrieben!            
