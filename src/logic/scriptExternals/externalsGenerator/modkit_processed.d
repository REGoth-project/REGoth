void AI_AimAt(c_npc attacker, c_npc target);
// NPC zielt mit Fernkampfwaffe auf Target-NPC

void AI_AlignToWP(c_npc self);
// richtet den Nsc am Waypoint aus (im Spacer gesetzte Pfeilrichtung)

void AI_Ask(c_npc self, func anserYes, func answerNo);
// die angegeben Instanz (self), gibt eine Frage aus und verweist auf die selbst zu definierenden Funktionen,
// die für die Fälle Spieler sagt ja (Daumen nach oben) und Spieler sagt nein (Daumen unten) vorhanden sein müssen

void AI_AskText(c_npc self, func funcYes, func funcNo, string strYes, string strNo);
// wie AI_Ask, nur das außer den Funktionen auch noch Strings für die Antworten Ja/Nein mit angegeben werden können

void AI_Attack(c_npc self);
// Startet Kampf AI (sollte in der ZS_Attack-Loop stehen)
// Es wird das interne Ziel verwendet, das mit Npc_SetTarget() oder Npc_GetNextTarget() gesetzt
// wurde.

void AI_ContinueRoutine(c_npc self);
// Setze Tagesablauf fort
// Enthält Standup

void AI_Defend(c_npc self);
// Der Befehl ist als Overlay-Message implementiert. Das heisst, dass er neben anderen Nachrichten
// aktiv bleibt. Er wird erst beendet, wenn der NPC eine Parade (ausgeloest durch die Attacke eines
// anderen NPCs) durchgefuert hat.

void AI_Dodge(c_npc npc);
// Der Nsc weicht ein Stück nach hinten aus

void AI_DrawWeapon(c_npc n0);
// Equipte Waffe wird gezogen

void AI_DropItem(c_npc self, int itemid);
// Item(itemid) wird auf den Boden fallen gelassen

void AI_EquipArmor(c_npc owner, c_item armor_from_owners_inventory);
// G1: Ziehe die angebene Rüstung dem NSC "owner" an, diese muss sich in seinem Inventory befinden.
// G2: !!! was ist diese armor_from_owners_inventory ???

void AI_EquipBestArmor(c_npc self);
// Wunder, Wunder hier wird die beste im Inventory vorhandene Rüstung angezogen

void AI_EquipBestMeleeWeapon(c_npc self);
// sucht im Inventory nach der besten Nahkampfwaffe und hängt sie an den Gürtel

void AI_EquipBestRangedWeapon(c_npc self);
// sucht im Inventory nach der besten Fernkampfwaffe und ploppt sie auf den Rücken der Instanz

void AI_FinishingMove(c_npc self, c_npc other);
// Führt den logischen Finishing Move inklusive Anis aus den Skripten heraus aus

void AI_Flee(c_npc self);
// Der Befehl muss, wie AI_Attack(), in der ZS-Loop regelmaessig aufgerufen werden und setzt voraus, dass
// vorher mit Npc_SetTarget( self, <var c_npc enemy> ) ein Gegner gesetzt wurde, vor dem der Npc fliehen soll.

void AI_GotoFP(c_npc self, string fpName);
// Sucht sich einen Freepoint im Umkreis von 20m vom NSC, bewegt sich dorthin und richtet sich entsprechend aus.
// Suchkriterium wie bei Wld_IsFPAvailable()

void AI_GotoItem(c_npc self, c_item item);
// "self" geht zu "item"

void AI_GotoNextFP(c_npc self, string fpName);
//wie AI_GotoFP() allerdings Suchkriterium wie bei Wld_IsNextFPAvailable()

void AI_GotoNpc(c_npc self, c_npc other);
// "self" geht zu "other"

void AI_GotoSound(c_npc n0);
// Npc läuft zum Sound

void AI_GotoWP(c_npc n0, string s0);
// Npc-Instanz läuft zum namentlich angegeben Waypoint

void AI_LookAt(c_npc self, string name);
// Schaue auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)

void AI_LookAtNpc(c_npc self, c_npc other);
// Schaue zu einem NSC

void AI_LookForItem(c_npc self, int instance);
// gibt die Möglichkeit nach bestimmten Items zu suchen (z.B.:Das goldene Schwert der Zerstörung, wenn vorhanden)

void AI_Output(c_npc self, c_npc target, string outputName);
// Cutscene mit entsprechender Id wird abgenudelt

void AI_OutputSVM(c_npc self, c_npc target, string svmname);
// Outputbefehl um Svms abzuspielen

void AI_OutputSVM_Overlay(c_npc self, c_npc target, string svmname);
// wie AI_OutputSVM, wartet jedoch NICHT mit der Ausführung des nächsten AI_...-Befehls, bis
// das SVM zuende gespielt wird. (Für Kommentare kurz vor und während dem Kampf!)

void AI_PlayAni(c_npc n0, string s0);
// Npc-Instanz spielt die angegebene Animation ab

void AI_PlayAniBS(c_npc npc, string aniname, int bodystate);
// Beim Abspielen einer Ani mit diesem Befehl kann ein Bodystate angemeldet werden

void AI_PlayCutscene(c_npc self, string csName);
// Eine Cutscene aus den Scripten heraus starten

void AI_PointAt(c_npc self, string name);
// Zeige auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)

void AI_PointAtNpc(c_npc self, c_npc other);
// Zeige auf einen NSC

void AI_Quicklook(c_npc self, c_npc other);
// NSC kurz ( 2 sec) anschauen ( nur Kopf bewegt sich )

void AI_ReadyMeleeWeapon(c_npc self);
// Ziehe equippte Nahkampfwaffe

void AI_ReadyRangedWeapon(c_npc self);
// Ziehe equippte Fernkampfwaffe

void AI_ReadySpell(c_npc self, int spellID, int investMana);
// Lasse zauberspruch auf Hand erscheinen.

void AI_RemoveWeapon(c_npc n0);
// Gezogene Waffe wird weggesteckt

void AI_SetNpcsToState(c_npc self, func aiStateFunc, int radius);
// Setze alle NSCs im Umkreis von x cm in entsprechenden AI-Zustand
// VORSICHT: Diese Funktion ist Buggy, wird aber derzeit auch NICHT verwendet!
// -> FINGER WEG!!! (SN)

void AI_SetWalkmode(c_npc n, int n0);
// gibt an mit welchem Walkmode Run etc der Character durch das Level läuft
//NPC_RUN			: Rennen
//NPC_WALK			: Gehen
//NPC_SNEAK			: Schleichen
//NPC_RUN_WEAPON	: Rennen mit gezogener Waffe
//NPC_WALK_WEAPON	: Gehen mit gezogener Waffe
//NPC_SNEAK_WEAPON	: Schleichen mit gezogener Waffe

void AI_ShootAt(c_npc attacker, c_npc target);
// NPC feuert mit Fernkampfwaffe auf Target-NPC

void AI_StandUp(c_npc self);
// - Ist der Nsc in einem Animatinsstate, wird die passende Rücktransition abgespielt.
// - Benutzt der NSC gerade ein MOBSI, poppt er ins stehen.

void AI_StandUpQuick(c_npc self);
// Wie AI_StandUp(), jedoch werden keine Rücktransitionen abgespielt, sondern auch dort wird
// sofort in den Grundzustand "gepoppt". Wichtig für sehr eilige Situationen!

void AI_StartState(c_npc self, func what, int stateBehaviour, string wpName);
// Versetzt den Nsc aus den Skripten heraus in den entsprechenden ZS (what),
// stateBehaviour sagt : "0"-aktuellen Zustand abbrechen "1"-aktuellen Zustand erst ordnungsgemäß beenden (End-Funktion aufrufen) ).

void AI_StopAim(c_npc attacker);
// NPC beendet vorher gestartetes Zielen mit Fernkampfwaffe.

void AI_StopLookAt(c_npc self);
// wieder geradeaus schauen

void AI_StopPointAt(c_npc self);
// nicht mehr auf etwas zeigen

void AI_StopProcessInfos(c_npc npc);
// Der DialogModus wird beendet (Multiple Choice-Dialog)

void AI_TakeItem(c_npc self, c_item item);
// der Npc nimmt mit dieser Funktion die globale Item-Instanz auf

void AI_Teleport(c_npc self, string waypoint);
// teleportiert den NSC zur angegebenene Location

void AI_TurnAway(c_npc n0, c_npc n1);
//Der NSC "self" dreht dem NSC "other" den Rücken zu.

void AI_TurnToNpc(c_npc n0, c_npc n1);
// drehe Dich zum angegeben (zweiten) Npc um

void AI_TurnToSound(c_npc self);
// Charakter dreht sich zur Geräuschquelle

void AI_UnequipArmor(c_npc self);
// Unequippe aktuelle Rüstung

void AI_UnequipWeapons(c_npc self);
// Unequippe alle Waffen

void AI_UnreadySpell(c_npc self);
// lasse zauberspruch aus Hand verschwinden

void AI_UseItem(c_npc self, int itemInstance);
// Item bis zum Ende benutzen

void AI_UseItemToState(c_npc self, int itemInstance, int state);
// Item benutzen bis zum angegebenen State

int AI_UseMob(c_npc self, string schemeName, int targetState);
// Benutze Mob mit angegebenen Schema-Namen bis zum Zustand "targetState". Wird diese Funktion aufgerufen
// und der angegebene 'targetState' ist bereits vorhanden, läuft der NSC zwar trotzdem zum MOB, tut dann aber nichts

void AI_Wait(c_npc n0, float n1);
// Character wird für n1 Sekunden in einen Wait-Zustand versetzt,
// d.h. er tut nichts, aber Treffer etc.(passive Wahrnehmung) werden registriert

void AI_WaitForQuestion(c_npc self, func scriptFunc);
// NSC wartet 20 Sekunden, wird er in dieser Zeit vom Spieler angesprochen, wird die angegebene SkriptFunktion ausgeführt.

void AI_WaitTillEnd(c_npc self, c_npc other);
// 'self' wartet bis 'other' seinen laufenden AI-Befehl zu Ende gespielt hat (funzt allerdings nicht bei AI-Overlays!)

void AI_WhirlAround(c_npc self, c_npc other);
// schnelle Drehung zu other

string ConcatStrings(string str1, string str2);
// !!!Erzeugt eine Kopie von dem ersten angegebenen String, hängt den zweiten an diesen an und gibt den neuen String zurück ???

void CreateInvItem(c_npc n0, int n1);
//  gibt der angegebenen Instanz (self oder other) ein Item ins Inventory

void CreateInvItems(c_npc n0, int n1, int n2);
// wie vorherige Funktion, nur das Multi-Items (z.B.: 100 Pfeile) ins Inventory kreiert werden

int Doc_Create();
// G1: erstellet ein Dokument und liefert ein Handle zurück, daß für alle weiteren Doc_...-Befehle nötig ist
// Erzeugt ein Dokument.
//
//	[result]	- Handle auf das Dokument (-1 = konnte nicht erzeugt werden)

int Doc_CreateMap();
// Gothic 2 only function
// Erzeugt ein Dokument (Karte).
//
//	[result]	- Handle auf das Dokument (-1 = konnte nicht erzeugt werden)

void Doc_Font(string fontName);
// Gothic 1 only function
// Benutzt den angegebenen Font für nachfolgende "Doc_Print"s.

void Doc_MapCoordinates(string levelName, float gamex1, float gamey1, float pixelx1, float pixely1, float gamex2, float gamey2, float pixelx2, float pixely2);
// Gothic 1 only function
// Karte initialisieren : levelName = Name des Levels (.ZEN) aus dem die Karte einen Ausschnitt zeigt
// gamex1,gamey1 : Koordinaten im Spiel (linke obere Ecke der Karte)
// pixelx1,pixely1 : PixelKoordinaten linke obere Ecke // der Rest : alles nochmal für die rechte untere Ecke

void Doc_Open(string picName);
// Gothic 1 only function
// Öffnet ein Dokument mit dem Bild "picName" als Hintergrund

void Doc_Print(string text);
// Gothic 1 only function
// Angegebener Text wird in das aktuelle (vorher mit Doc_Open geöffnete) Dokument geschrieben

void Doc_PrintLine(int Document, int Page, string Text);
// G1: Gibt Text für die Seite 'page' an. Der Text wird NICHT umgebrochen
// Setzt den Text einer Seite. Der Text wird _nicht_ umgebrochen.
//
//	Document	- Handle auf das Dokument
//	Page		- Index der Seite (beginnt mit 0)
//	Text		- Textzeile

void Doc_PrintLines(int Document, int Page, string Text);
// G1: Gibt Text für die Seite 'page' an. Der Text WIRD umgebrochen, falls er nicht auf die in Doc_SetPage() angegebene Grafik passt.
// Setzt den Text einer Seite. Der Text wird umgebrochen falls notwendig.
//
//	Document	- Handle auf das Dokument
//	Page		- Index der Seite (beginnt mit 0)
//	Text		- Textzeilen

void Doc_SetFont(int Document, int Page, string Font);
// G1: setzt den Font für die angegebene Seite 'page' oder für das ganze Dokument ('page':-1)
// G1: Fontname z.B. "Scrolls.tga"
// Setzt den Font einer/aller Seite(n).
//
//	Document	- Handle auf das Dokument
//	Page		- Index der Seite (beginnt mit 0, -1 = alle)
//	Font		- Dateiname der Font-Textur (ohne Pfad - z.B. 'Font_Default.tga')

void Doc_SetLevel(int Document, string Level);
// Gothic 2 only function
// Setzt das Level-ZEN zur Anzeige der Spieler-Position (Karten).
//
// (als Grenze gilt die BBox des statischen Level-Meshes)
//	Document	- Handle auf das Dokument
//	Level		- Dateiname des Level-ZENs (mit Pfad ab WORLDS - z.B. 'NewWorld\NewWorld.zen')

void Doc_SetLevelCoords(int Document, int Left, int Top, int Right, int Bottom);
// Gothic 2 only function
// Setzt die Grenzen des Level-Bereichs, der durch die Textur abgedeckt wird (Karten).
//
// (als Grenzen werden _absolute_ Level-Koordinaten angegeben - z.B.: '..., -28000, 50500, 95500, -42500')
//	Document	- Handle auf das Dokument
//	Left		- linke Grenze
//	Top			- obere Grenze
//	Right		- rechte Grenze
//	Bottom		- untere Grenze

void Doc_SetMargins(int Document, int Page, int Left, int Top, int Right, int Bottom, int Pixels);
// G1: setzt die Ränder (gerechnet vom Rand der TGA-Datei, die in Doc_SetPage() gesetzt wird). Die Ränder sind in Pixeln angegeben.
// Setzt den jeweiligen Rand einer/aller Seite(n) (vom Rand der mit Doc_SetPage gesetzten Textur aus gesehen).
//
//	Document	- Handle auf das Dokument
//	Page		- Index der Seite (beginnt mit 0, -1 = alle)
//	Left		- linker Rand
//	Top		- oberer Rand
//	Right		- rechter Rand
//	Bottom		- unterer Rand
//	Pixel		- Flag ob Angaben in Pixeln (Bildschirm) oder virtuellen Koordinaten (0-8192)

void Doc_SetPage(int Document, int Page, string Texture, int Scale);
// G1: setzt für die Seite 'page' die Grafik-Datei 'pageimage'. Diese muß im TGA-Format vorliegen und
// G1: z.B. "aufgeschlagenesBuch_links.tga"  heißen
// G1: wird bei 'page' -1 übergeben, so bezieht sich der Befehl auf ALLE Seiten des Dokuments
// Setzt die Hintergrund-Textur der Seite eines Dokuments.
//
//	Document	- Handle auf das Dokument
//	Page		- Index der Seite (beginnt mit 0, -1 = alle)
//	Scale		- Flag ob Textur gestreckt werden soll

void Doc_SetPages(int Document, int Count);
// G1: setzt die Anzahl der Seiten dieses Dokuments. Bei mehr als zwei Seiten wird versucht ALLE
// G1: Seiten auf den Bildschirm zu bekommen
// Setzt die Anzahl der Seiten eines Dokuments.
//
//	Document	- Handle auf das Dokument
//	Count		- Anzahl der Seiten

void Doc_Show(int Document);
// G1: nachdem das Dokument mit den obenstehenden Befehlen befüllt und formatiert wurde, wird es hiermit nun auf den Bildschirm geschrieben!
// Zeigt das Dokument auf dem Bildschirm an.
//
//	Document	- Handle auf das Dokument

void EquipItem(c_npc n0, int n1);
// gibt dem Nsc direkt das angegebene Item an den Gürtel

void ExitGame();
// Gothic 2 only function
// Beendet Gothic.

void ExitSession();
// Gothic 2 only function
// Beendet das laufende Spiel.

int FloatToInt(float x);
// !!! konvertiert einen float in einen int, hier wäre es noch ganz schön zu wissen, wie mit dem float umgesprungen wird --> schneiden oder runden ???

void Game_InitEnglish();
// Gothic 2 only function
// Setzt die interne Engine-Variablen auf English.
//
//	[result]	- Zeigt an, ob die Sprache erfolgreich geladen wurde.

void Game_InitGerman();
// Gothic 2 only function
// Setzt die interne Engine-Variablen auf Deutsch.
//
//	[result]	- Zeigt an, ob die Sprache erfolgreich geladen wurde.

int Hlp_CutscenePlayed(string csName);
// Abfrage, ob Cutscene schon gespielt wurde (0 = Nein / 1 = Ja)

int Hlp_GetInstanceID(c_npc npc);
// liefert die interne ID ( nicht Var aus der Instanz) zurück, um z.B. other mit self vergleichen zu können ( integer Vergleich)

int Hlp_GetInstanceID2(c_item item);
// liefert die interne ID ( nicht Var aus der Instanz) zurück, um zwei items miteinander vergleichen zu können ( integer Vergleich)
// *******************************************************************************************
// ***                                      WARTEN / ZEIT                                  ***
// *******************************************************************************************

c_npc Hlp_GetNpc(int instanceName);
// Ermittle einen NSC über den Instanznamen. Dieser kann einer Variablen zugewiesen werden und ist somit gezielt verfügbar

int Hlp_IsItem(c_item item, int instanceName);
// Prüft, ob der Gegenstand 'item' den Instanznamen 'instanceName' trägt. Will man z.B. testen,
// ob die globale Variable item Pfeile sind (ItMuArrow) ruft man Hlp_IsItem(item,ItMuArrow) auf.
// Gribt bei Gleichheit TRUE, sonst FALSE zurück.

int Hlp_IsValidItem(c_item item);
// Prüfe ob Item-Instanz gültig und initialisiert ist True/False

int Hlp_IsValidNpc(c_npc self);
// Prüfe ob NSC-Instanz gültig und initialisiert ist. True/False

int Hlp_Random(int n0);
// erzeugt einen Zufallswert (wobei n0 Obergrenze : [0..n0-1] ) Rückgabewert integer

int Hlp_StrCmp(string s1, string s2);
// Hilfsfunktion um zwei Strings miteinander zu vergleichen liefert bei Gleichheit eins zurück

void IntroduceChapter(string Titel, string Untertitel, string Texture, string Sound, int WaitTime);
// Gothic 2 only function
// Zeigt den Kapitelwechsel-Bildschirm an.
//
//	Titel		- Text des Titels
//	Untertitel	- text des Untertitels
//	Texture		- Dateiname der Hintergrundtextur
//	Sound		- Dateiname des abgespielten Sounds
//	WaitTime	- Anzahl in Millisekunden, die der Bildschirm angezeigt wird

float IntToFloat(int x);
// !!! wie float to int, nur daß ein integer in float umgewandelt wird ???

string IntToString(int x);
// !!!Funktion überträgt eine Integer variable in eine Stringvariable (Rückgabewert : string)???

void Log_AddEntry(string topic, string entry);
//	Der Befehl fügt dem Topics topic ein neues Entrz namens entry hinzu. Wie auch bei der Erzeugung von Topics dient hier der
//	Name sowohl zur Identifikation als auch zur Anzeige und muss somit eindeutig sein. Doppelte Einträge werden auch hier ignoriert.
//
//	Parameter
//	topic       Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
//	entry		Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Eintrags verwendet wird.

void Log_CreateTopic(string name, int section);
//	Der Befehl fügt unter der Sektion 'section' ein neues Topic mit Namen 'name' ein.
//	Sollte bereits ein Topic dieses Names in irgendeiner Sektion existieren, wird das Topic nicht hinzugefügt.
//
//	Parameter:
//	- name      Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Topics verwendet wird.
//	- section	Eine, in der Constants.d definierte Konstante, die besagt, in welcher Sektion das Topic angezeigt werden soll.
//				Die Konstante kann folgende Werte annehmen:
//					- LOG_MISSION
//					- LOG_NOTE

void Log_SetTopicStatus(string name, int status);
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

void Mdl_ApplyOverlayMds(c_npc n0, string s1);
// mit diesem Befehl werden Animationen auf einem höheren Layer gestartet
// (z.B. hat der Zombie ein Overlay über die normalen Human-Animationen)

void Mdl_ApplyOverlayMDSTimed(c_npc self, string overlayname, float timeTicks);
// Overlay-MDS wird gestartet und automatisch nach der angegebenen Zeit abgeschaltet

void Mdl_ApplyRandomAni(c_npc n0, string s1, string s2);
// Fügt Zufallsanimationen (am Kopf kratzen, an den Sack packen etc (s2)) für bestimmte Animationszustände (s1) ein

void Mdl_ApplyRandomAniFreq(c_npc n0, string s1, float f2);
// hiermit kann die Frequenz betimmt werden, wie oft die für den Animationszustand (s1) deklarierten Randomanis abgespielt werden

void Mdl_ApplyRandomFaceAni(c_npc self, string name, float timeMin, float timeMinVar, float timeMax, float timeMaxVar, float probMin);
// Starte zufällige Gesichtsanimation
// Mdl_ApplyRandomFaceAni ( self, ANINAME, minTime, minTimeVar, maxTime, maxTimeVar, probMin)
// minTime  = Minimum an Zeit nachdem Ani startet (in Sekunden)
// maxTime  = Maximum an Zeit nachdem Ani startet (in Sekunden)
// minTimeVar = Abweichung von minTime (in Sekunden)
// maxTimeVar = Abweichung von maxTime (in Sekunden)
// probMin  = Wahrscheinlichkeit [0..1] ob obere oder untere Grenze der Abweichung benutzt wird

void Mdl_RemoveOverlayMDS(c_npc self, string overlayName);
// Entferne ein Overlay-MDS

void Mdl_SetModelFatness(c_npc self, float fatness);
// Setzt Model-Z-Skalierung

void Mdl_SetModelScale(c_npc self, float x, float y, float z);
// zum skalieren des Meshes (Breite,Höhe,Tiefe) 1 = 100%, also Normalgröße!

void Mdl_StartFaceAni(c_npc self, string name, float intensity, float holdTime);
// Starte Gesichtsanimation // intensity 1 = 100% // holdTime -1 = forever

int Mis_GetStatus(int missionName);
// Liefert aktuellen Status einer Mission zurück ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc.

int Mis_OnTime(int missionName);
// Liefert TRUE, wenn sich Spieler noch innerhalb des Zeitlimits für diese Mission befindet

void Mis_SetStatus(int missionName, int newStatus);
// Setzt den Status einer Mission ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc. )

void Mob_CreateItems(string mobName, int itemInstance, int amount);
// Erzeuge "amount" Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen.

int Mob_HasItems(string mobName, int itemInstance);
// Liefere Anzahl der Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen

int Npc_AreWeStronger(c_npc self, c_npc other);
// ermittelt den stärkeren Nsc,
//Wenn die Summe der Level aller NPCs (human, Monster), die zu mir feindlich sind und die zu <other> freundlich sind
//MEHR ALS DOPPELT SO HOCH ist (x  > 2*y), wie die Summe der Level aller Leute, die zu mir freundlich sind, und die
//zu <other> feindlich sind, dann NEIN,
//sonst JA (= wir SIND stärker, nicht echt, aber ab doppelter Gegnerstärke zu fliehen ist früh genug, sonst kommen
//die Jungs aus dem Rennen nicht mehr raus
//Beachten:
//1) Monster können feindlich zu beiden Menschen sein --> egal, zählen auf beiden Seiten
//2) Jemand, der z.B. zu mir freundlich und zu <other> feindlich ist, wird demnach doppelt gewertet --> ok so

int Npc_CanSeeItem(c_npc npc1, c_item item);
// Prueft ob Npc1 den Gegenstand sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False

int Npc_CanSeeNpc(c_npc npc1, c_npc npc2);
// Prueft ob Npc1 den Npc2 sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False Blickwinkelabhängig (+,-100°)+LOS von der Hüfte aus

int Npc_CanSeeNpcFreeLOS(c_npc self, c_npc other);
// Prüft ob NSC anderen NSC sehen kann, ohne dabei den Winkel zu berücksichtigen (nur LineOfSight-Check)

int Npc_CanSeeSource(c_npc self);
// Checkt, ob NSC die Soundquelle sehen kann True/False

void Npc_ChangeAttribute(c_npc self, int atr, int value);
// ändert den Wert des Attributs (atr) um (value) Einheiten.

int Npc_CheckAvailableMission(c_npc npc, int missionState, int important);
// Prüfe ob zwischen dem NSC eine aktuelle Mission (AVAILABLE,RUNNING) besteht und vergleiche den Status mit den angegebenen.
// Falls dieser identisch ist, wird "1" geliefert.

int Npc_CheckInfo(c_npc npc, int important);
// Überprüft,ob der NSC gültige Infos für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").

int Npc_CheckOfferMission(c_npc npc, int important);
// Überprueft ob der NSC dem Spieler einen Auftrag anbieten kann, wenn ja, wird der Offer-Block gestartet und "1" zurückgeliefert.

int Npc_CheckRunningMission(c_npc npc, int important);
// Überprueft ob zwischen dem NSC und dem Spieler eine laufende Mission besteht und startet den entsprechenden Skriptblock.
// (Returnwert 1 : Mission aktiv)

void Npc_ClearAIQueue(c_npc self);
// hiermit werden alle Befehle mit sofortiger Wirkung aus der AI_Queue rausgeworfen

void Npc_CreateSpell(c_npc self, int spellnr);
// NSc bekommt Zauberspruch zugewiesen kann diesen aber noch nicht gebrauchen
// (erscheint ausgegraut im Auswahlkranz)

void Npc_ExchangeRoutine(c_npc self, string routineName);
// Tausche meherere Tagesabläufe aus

int Npc_GetActiveSpell(c_npc self);
// liefert den Zauber zurück, der auf der Hand ist (self oder other)
// liefert -1 zurück, wenn kein Zauber auf der Hand ist

int Npc_GetActiveSpellCat(c_npc self);
// Unterscheidet zwischen den drei Kategorien (Spell_Bad, Spell_neutral,Spell_Good) Spellkat ist Rückgabewert

int Npc_GetActiveSpellLevel(c_npc self);
// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist

int Npc_GetAttitude(c_npc self, c_npc other);
// Gibt die Attitüde von self zu other zurück (temp. / perm. / Gilden )

int Npc_GetBodyState(c_npc self);
// Ermittle BodyState ( Liefert BS_-Konstanten )
// !!! VORSICHT !!!: Diese Funktion gibt den Bodystate mit allen eventuellen
// ver-oderten Modifiern wie "brennend". Besser 'C_BodyStateContains(self,bodystate)' benutzen

string Npc_GetDetectedMob(c_npc self);
// liefert den Schemanamen des Mobsi zurück, das der Nsc entdeckt hat. Schemaname ist das String-Kürzel des Mobs, daß im Visualnamen vor dem ersten "_"-Zeichen steht, also z.B. "DOOR" wenn der Visualname "DOOR_OCR__135" ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
// WORKAROUND: zur Zeit werden hiermit nur Tür-MOBSIs zurückgegeben, dies soll aber wieder zurückgebaut werden

int Npc_GetDistToItem(c_npc npc, c_item item);
// Liefert Entfernung ( in cm ! ) zwischend NSC und Gegenstand

int Npc_GetDistToNpc(c_npc npc1, c_npc npc2);
// Liefert Entfernung ( in cm ! ) zwischend den beiden NSCs

int Npc_GetDistToPlayer(c_npc npc1);
// Liefert Entfernung ( in cm ! ) zwischend den beiden NSCs

int Npc_GetDistToWP(c_npc self, string wpName);
// liefert die Entfernung vom NSC 'self' zum angegebenen Waypoint in cm

c_item Npc_GetEquippedArmor(c_npc n0);
// Liefert die angelegte Rüstung des NSCs.

c_item Npc_GetEquippedMeleeWeapon(c_npc n0);
// Liefert die gegurtete Nahkampfwaffe des NSCs.

c_item Npc_GetEquippedRangedWeapon(c_npc n0);
// Liefert die gegurtete Fernkampfwaffe des NSCs.

int Npc_GetGuildAttitude(c_npc npc, c_npc npc);
// Ermittelt die Gildenattitüde von zwei Nsc´s direkt im Gegensatz zu Wld_GetGuildAttitude

int Npc_GetHeightToNpc(c_npc npc1, c_npc npc2);
// Gothic 2 only function
// Liefert Höhendifferenz ( in cm ! ) zwischend den beiden NSCs

c_item Npc_GetInvItem(c_npc self, int itemInstance);
// Ermittle ItemInstanz aus Inventory

int Npc_GetInvItemBySlot(c_npc self, int category, int slotNr);
// Mit diesem Befehl läßt sich nachsehen, ob in einem bestimmten Slot einer bestimmten Kategorie ein item vorhanden ist
// ist das der Fall, wird dieses Item in die globale Variable item geschrieben
// gibt jetzt die Anzahl zurueck, wenn das Item stackable ist
//. Den Transfer machst Du dann per Npc_RemoveInvItems() und Npc_CreateInvItems().

int Npc_GetLastHitSpellCat(c_npc self);
// Gothic 2 only function
// liefert die Category des Zaubers zurück, der den NSC zuletzt getroffen hat

int Npc_GetLastHitSpellID(c_npc self);
// Gothic 2 only function
// liefert den Zauber zurück, der den NSC zuletzt getroffen hat

string Npc_GetNearestWP(c_npc self);
// liefert den Namen des am nächsten gelegenen Waypoints zurück

c_npc Npc_GetNewsOffender(c_npc self, int newsNumber);
// Ermittelt Täter der News und gibt eine INstanz der Klasse C_Npc zurück

c_npc Npc_GetNewsVictim(c_npc self, int newsNumber);
// Ermittle Opfer der News und gibt eine INstanz der Klasse C_Npc zurück

c_npc Npc_GetNewsWitness(c_npc self, int newsNumber);
// Ermittle Zeuge der News und gibt eine INstanz der Klasse C_Npc zurück

int Npc_GetNextTarget(c_npc self);
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

string Npc_GetNextWP(c_npc self);
// Liefert den zweitnahesten WP vom NSC zurück

int Npc_GetPermAttitude(c_npc self, c_npc other);
// Ermittle die permanente Attitude von "self" zu "other"

c_item Npc_GetReadiedWeapon(c_npc n0);
// Liefert die gezogene Waffe des NSCs.

int Npc_GetStateTime(c_npc self);
// Liefert Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.

int Npc_GetTarget(c_npc self);
// Befüllt 'other' mit dem aktuellen Ziel. Das aktuelle Ziel wird intern gespeichert, wird
// durch Npc_SetTarget() bzw. Npc_GetNextTarget() gesetzt.
// - return:	aktuelles Ziel gespeichert 	-> TRUE
//				kein Ziel gespeichert		-> FALSE

int Npc_GetTrueGuild(c_npc npc);
// liefert immer!!! die wahre Gilde zurück, ignoriert also auch ohne die Regeln die Verkleidung

int NPC_GiveInfo(c_npc npc, int important);
// Überprüft,ob der NSC eine (!) gültige Info für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").

void Npc_GiveItem(c_npc n0, c_item n1, c_npc n2);
//Der NSC "self" gibt den NSC "other" den angegebenen Gegenstand "item". Der Gegenstand wandert sofort ins Inventory des anderen.

int Npc_HasBodyFlag(c_npc self, int bodyFlag);
// Liefert >0, falls BodyFlag gesetzt ist.

int Npc_HasDetectedNpc(c_npc self, c_npc other);
// liefert eins zurück, wenn der Spieler in Sinnesreichweite und entdeckt ist                                     // SPIELER ???

int Npc_HasEquippedArmor(c_npc self);
// Armor angezogen True/False

int Npc_HasEquippedMeleeWeapon(c_npc self);
// gibt eins zurück, wenn eine Nahkampfwaffe Equipped ist

int Npc_HasEquippedRangedWeapon(c_npc self);
// Fernkampwaffe auf dem Rücken True/False

int Npc_HasEquippedWeapon(c_npc self);
// gibt eins zurück, wenn die abgefragte Instanz (self oder other) eine Waffe sichtbar mit sich rumträgt

int Npc_HasFightTalent(c_npc self, int tal);
// Spezialabfrage auf Kampftalente (z.B. 1hSword) ansonsten wie Npc_HasTalent

int Npc_HasItems(c_npc n0, int itemInstance);
// Liefert "1", wenn NSC die angegebene Anzahl von Items besitzt.-> NpcHasItem ist damit obsolete

int Npc_HasNews(c_npc self, int newsID, c_npc offender, c_npc victim);
// Liefert Newsnummer>0 (für weitere Referenzen) falls entsprechende News vorhanden.
// Nicht benötigte Parameter können mit "NULL" leergelassen werden

int Npc_HasOffered(c_npc self, c_npc other, int itemInstance);
//Bietet Spieler dem NSC einen Gegenstand übers Trade-Modul an ? True/False

int Npc_HasRangedWeaponWithAmmo(c_npc npc);
// Gibt TRUE zurück, wenn 'npc' irgendeine Fernkampfwaffe im Inventory oder in der Hand hat UND
// dazu auch passende Munition vorhanden ist. Sonst FALSE.

int Npc_HasReadiedMeleeWeapon(c_npc self);
// Nahkampfwaffe in der Hand

int Npc_HasReadiedRangedWeapon(c_npc self);
// Fernkampfwaffe in der Hand

int Npc_HasReadiedWeapon(c_npc self);
// gibt eins zurück, wenn die Waffe schon in der Hand ist

int Npc_HasSpell(c_npc self, int spellID);
// Kann der NSC den angegebenen Zauberspruch benutzen ?

int Npc_HasTalent(c_npc self, int tal);
// liefert eins zurück, wenn der geprüfte Charakter das Talent tal hat

int Npc_IsAiming(c_npc self, c_npc other);
// liefert eins zurück, wenn der zweite Character auf den ersten zielt (Fernkampfwaffen und Zauber)

int Npc_IsDead(c_npc n0);
// Funktion liefert einen Wert zurück, falls Dead true ist

int Npc_IsDetectedMobOwnedByGuild(c_npc user, int ownerguild);
//Liefert >0, falls "ownerguild" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!

int Npc_IsDetectedMobOwnedByNpc(c_npc user, c_npc owner);
// Liefert >0, falls "owner" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!

int Npc_IsInCutscene(c_npc self);
// liefert eins zurück, wenn der entsprechende Nsc in einer Cutscene ist

int Npc_IsInFightMode(c_npc self, int fmode);
// liefert eins zurück, wenn der Charakter im angegebenen Fightmode (z.B. FMODE_MAGIC) ist

int Npc_IsInRoutine(c_npc self, func state);
// Check ob der angegebene Zustand der aktuelle TA des NSCs ist. True/FAlse

int Npc_IsInState(c_npc self, func state);
// Abfrage auf den aktuellen Zustand einer Spielfigur True/False

int Npc_IsNear(c_npc self, c_npc other);
// liefert eins zurück, wenn geprüfte Instanz sich im Bereich von drei Metern zur prüfenden Instanz befindet

int Npc_IsNewsGossip(c_npc self, int newsNumber);
// Liefert >0, falls News "Gossip" ist

int Npc_IsNextTargetAvailable(c_npc self);
// Sucht genauso wie Npc_GetNextTarget nach einem neuen Ziel, überschreibt aber weder
// das interne Ziel, noch 'other'
// - return:	neues Ziel gefunden	-> TRUE
//				kein Ziel gefunden	-> FALSE
// VORSICHT:	Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
//				Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
//				Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden

int Npc_IsOnFP(c_npc self, string name);
// Abfrage darauf, ob der Nsc auf einem Freepoint mit name Teilstring steht

int Npc_IsPlayer(c_npc player);
// liefert eins zurück, wenn der geprüfte Charakter der Spieler himself ist

int Npc_IsPlayerInMyRoom(c_npc npc);
// gibt TRUE zurück, wenn sich SC im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE

int Npc_IsWayBlocked(c_npc self);
// Liefert "1", falls Weg von NSC durch Hindernis versperrt ist.

int Npc_KnowsInfo(c_npc self, int infoInstance);
// Liefert TRUE, wenn der angegebene Spieler die Info schon einmal erhalten hat.
// VORSICHT: auch wenn eine permanente Info schon einmal dem Spieler erzählt wurde, so gibt diese Funktion trotzdem FALSE zurück!

int Npc_KnowsPlayer(c_npc self, c_npc player);
// Kennt der NSC den Spieler? True/False

void Npc_LearnSpell(c_npc self, int spellnr);
// Was könnte das wohl heißen ? Ich glaube damit kann man den (spellnr) Zauber zuweisen

void Npc_MemoryEntry(c_npc self, int source, c_npc offender, int newsid, c_npc victim);
// mit dieser Funktion wird eine NSC bezogene News geschrieben newsid		:	News ID source >	0	:	"gossip", ansonsten	"witness",
// self:	NSC, bei dem News eingespeist werden soll, other:	Täter victim :	Opfer

void Npc_MemoryEntryGuild(c_npc self, int source, c_npc offender, int newsid, c_npc victimguild);
// wie MemoryEntry nur, das die Memory für die ganze Gilde kreiert wird

int Npc_OwnedByGuild(c_item item, int guild);
// Liefert "1", wenn der angegebenen Gilde das Item gehört ( Gildenbesitz )

int Npc_OwnedByNpc(c_item item, c_npc npc);
// Liefert "1", wenn dem NSC das Item gehört ( persönliches Besitzflag )

void Npc_PercDisable(c_npc self, int percID);
// Deaktiviere Perception

void Npc_PerceiveAll(c_npc self);
// Nimm alle Objekte in Wahrnehmungsreichweite wahr, die dann mit WLD_DetectNpc und Wld_DetectItem auswertbar sind

void Npc_PercEnable(c_npc self, int percID, func function);
// Aktiviere Perception

int Npc_RefuseTalk(c_npc self);
// Abfrage ob Dialog-Refuse Counter noch aktiv ist True/False

void Npc_RemoveInvItem(c_npc owner, int itemInstance);
// G1: das angegebene Item wird aus dem Inventory des NSCs entfernt und gelöscht
// G2: !!! das globale Item wird gelöscht ???

void Npc_RemoveInvItems(c_npc owner, int itemInstance, int amount);
// G1: das angegebene Anzahl des Multi-Items wird aus dem Inventory des NSCs entfernt und gelöscht
// G2: !!! wie Npc_RemoveInvItem, nur das Multislotgegenstände gelöscht werden ???

void Npc_SendPassivePerc(c_npc npc1, int Perc_type, c_npc npc2, c_npc npc3);
// Sende eine passive Wahrnehmung aus.Npc1 = wer schickt Npc2 = Opfer, Npc3 = Täter

void Npc_SendSinglePerc(c_npc self, c_npc target, int percID);
// verschicke Wahrnehmung an einzelnen NSC

int Npc_SetActiveSpellInfo(c_npc npc, int i1);
// G2: liefert den Spell-Level des Zaubers zurück, der auf der Hand ist
// G1: Hier kann ein Wert für den Zauberspruch gesetzt werden. Was dieser Wert bewirkt, haengt allein von der Nutzung im
// G1: Skript ab. Das Programm  benutzt diesen nicht.

void Npc_SetAttitude(c_npc self, int att);
//  setzt die permanente Attitüde auf einen festen Wert

void Npc_SetKnowsPlayer(c_npc self, c_npc player);
// NSC kennt SC

void Npc_SetPercTime(c_npc self, float seconds);
// Setze Zeitdelta für aktive Wahrnehmungen, alle Zeitdelta-Sekunden wird  WN gesendet

void Npc_SetRefuseTalk(c_npc self, int timeSec);
// Dialog Refuse Counter aus "x" Sekunden setzen

void Npc_SetStateTime(c_npc self, int seconds);
// _Setzt_ Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
//--> Das ist dann über Npc_GetStateTime abfragbar.

void Npc_SetTarget(c_npc self, c_npc other);
// Gibt dem Nsc 'self' das interne Ziel 'other'.
// --> Nur wenn per GetTarget auch der other "geholt" wird ist er vorhanden,
// da hier interne Variablen, die in den Skripten nicht vorhanden sind verwendet werden

void Npc_SetTeleportPos(c_npc self);
// Magie/Zauberstein Teleport Spruch : letzte Position des NSCs merken, zwecks späteren dahinbeamens.

void Npc_SetTempAttitude(c_npc self, int att);
//   setzt die temporäre Attitüde auf einen Wert (att)

void Npc_SetToFightMode(c_npc self, int weapon);
// Setzt den NSC beim Start in den der Waffe entsprechenden Kampfmodus (Waffe wird erzeugt)

void Npc_SetToFistMode(c_npc self);
// Setzt den NSC beim Start in den Faustkampfmodus (zB.Monster)

int Npc_SetTrueGuild(c_npc npc, int guildID);
// Setzt die wahre Gilde des NSCs

int Npc_StartItemReactModules(c_npc self, c_npc other, c_item item);
// Prüfe alle ItemReact-Module von "self" auf Gegenstand "item" von Geber "other" und starte passende Reaction-Funktion
//liefert True beim finden eines Moduls

int Npc_WasInState(c_npc self, func state);
// Abfrage auf den vorherigen Zustand einer Spielfigur True/False

int Npc_WasPlayerInMyRoom(c_npc npc);
// gibt TRUE zurück, wenn sich SC vor dem letzten Raumwechsel im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen

void Perc_SetRange(int percID, int range);
// Setze Reichweite fuer eine passive Wahrnehmung int cm

int PlayVideo(string Filename);
// Gothic 2 only function
// Spielt eine Videodatei ab.
//
//	Filename	- Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
//	[result]	- Boolean ob erfolgreich abgespielt

int PlayVideoEx(string Filename, int ScreenBlend, int ExitSession);
// Gothic 2 only function
// Spielt eine Videodatei mit erweiterten Optionen ab.
//
//	Filename	- Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
//	ScreenBlend	- Boolean ob Effekt 'BLACK_SCREEN' danach abgespielt wird (nicht bei ExitSession)
//	ExitSession	- Boolean ob nach dem Video die Session beendet wird
//	[result]	- Boolean ob erfolgreich abgespielt, bei ExitSession immer erfolgreich

void Print(string s0);
// Ausgabebefehl, der Text ins Game schreibt (wie OutputunitTexte)

void PrintDebug(string s);
// Printausgabe, die nur bei eingeschaltetem Debugmodus (Alt+D) im Spy/logfile ausgegeben wird

void PrintDebugCh(int ch, string text);
// gibt nur den Text eines bestimmten Channels wieder, unabhängig von der Instanz ???

void PrintDebugInst(string text);
// !!! gibt nur den Debugtext der eingeschalteten (SetDebugFocus --> siehe Intranetpage Erweitertes Skript-Debugging) Instanz aus ???

void PrintDebugInstCh(int ch, string text);
// !!! gibt nur in einem bestimmten channel liegende Debuginfos einer Instanz aus ???

void PrintMulti(string s0, string s1, string s2, string s3, string s4);
// Printbefehl, der aus den angegebenen Strings einen Auswählt und auf den Bildschirm schreibt

void PrintScreen(int dialogNr, string msg, int posx, int posy, string font, int timeSec);
// Gibt den Text 'msg' auf dem Bildschrim aus und benutzt dabei den Font 'font'.
// Die Position ist für jede Koordinate eine Zahl zwischen 0 und 99 und gibt die prozentuale Position an.
// Der Ursprung befindet sich oben links (also 0% X und 0% Y)

void Rtn_Exchange(string oldRoutine, string newRoutine);
// Tausche aktuellen Tagesablauf des NSC "self" gegen angegebenen aus
// (Name wird automatisch mit "RTN_" am Anfang und NSC-Skript-ID am Ende ergänzt)

void SetPercentDone(int PercentDone);
// Gothic 2 only function
// Setz die Fortschrittsanzeige im Ladebalken.
//
//	PercentDone	- Aktueller Fortschritt des Ladevorgangs in Prozent (0-100)

int Snd_GetDistToSource(c_npc self);
// Liefert Entfernung ( in cm ! ) zum letzten logischen Sound

int Snd_IsSourceItem(c_npc self);
// Check, ob Quelle des letzten Sound Item war (Return >0) und setzt "item" auf diesen Gegenstand

int Snd_IsSourceNpc(c_npc self);
// Check, ob Quelle des letzten Sound NPC war (Return >0) und setzt "other" auf diesen NPC

void Snd_Play(string s0);
// spielt einen Sound ab

void Snd_Play3D(c_npc n0, string s1);
// spielt einen 3D-Sound ab.

void TA(c_npc self, int start_h, int stop_h, func state, string waypoint);
// Mit _(Zustandsname) wird ein neuer Tagesablauf generiert, siehe TA.d

void TA_BeginOverlay(c_npc self);
// Melde einen Overlay-Tagesablauf an

void TA_CS(c_npc self, string csName, string roleName);
// Cutscene an den zuletzt angegebenen Tagesablaufpunkt hängen
// csName	: Name der Cutscene ( der Name des "CS" - Files )
// roleName : Die Rolle die der NSC dabei übernehmen soll.

void TA_EndOverlay(c_npc self);
// Beende einen Overlay-Tagesablauf

void TA_Min(c_npc self, int start_h, int start_m, int stop_h, int stop_m, func state, string waypoint);
// Tagesablaufpunkt minutengenau angeben

void TA_RemoveOverlay(c_npc self);
// Entferne aktiven TA-Overlay

void Wld_AssignRoomToGuild(string s0, int guild);
// Ordnet den Raum: 's0' der Gilde 'guild' zu

void Wld_AssignRoomToNpc(string s0, c_npc roomowner);
// Ordnet den Raum: 's0' dem  speziellen Nsc 'roomowner' zu

int Wld_DetectItem(c_npc self, int flags);
// liefert eins zurück, wenn ein Item mit dem Entsprechende Flag (z.B.ITEM_KAT_FOOD )gefunden wurde
// Globale Variable 'item' wird mit dem gefundenen Gegenstand initialisiert

int Wld_DetectNpc(c_npc self, int instance, func aiState, int guild);
// Diese Methode initilisiert die globale Skriptvariable "other" mit einem NSC, den "self" beim letzten Warnehmungscheck wargenommen hat.
// instance      = Name der zu suchenden Instanz							( "-1" angeben, wenn Instanzname unwichtig )
// guild         = Der zu suchende NSC muss Mitglied dieser Gilde sein		( "-1" angeben, wenn Gilde unwichtig )
// aiState       = Der AI-Zustandsname, in dem sich der NSC befinden soll	( NOFUNC angeben, wenn AI-State unwichtig )
// Wenn die Methode einen entsprechenden NSC gefunden hat, liefert diese "1" und 'other' ist initialisiert
// ansonsten wird "0" geliefert und "other" wird nicht verändert.

int Wld_DetectNpcEx(c_npc self, int npcInstance, func aiState, int guild, int detectPlayer);
// Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.

int Wld_DetectPlayer(c_npc self);
// liefert eins zurück, wenn der Player in der Nähe ist

void Wld_ExchangeGuildAttitudes(string name);
// Tausche Gilden-AttitudenTabelle aus.

int Wld_GetDay();
// Liefert den aktuellen Tag zurück (Zähler), hierbei ist der StartTag (Gamestart) = 0.

int Wld_GetFormerPlayerPortalGuild();
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert

c_npc Wld_GetFormerPlayerPortalOwner();
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'

int Wld_GetGuildAttitude(int guild1, int guild2);
// Ermittle Gildenattitude

int Wld_GetMobState(c_npc self, string schemeName);
// Liefert den State zum nächsten Mob mit 'schemeName' zurück bzw. '-1' wenn kein solches Mob gefunden wurde

int Wld_GetPlayerPortalGuild();
// liefert Gilde des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert

c_npc Wld_GetPlayerPortalOwner();
// liefert den NSC des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'

void Wld_InsertItem(int itemInstance, string spawnPoint);
// Füge Item in Welt ein entweder an einem WP oder einem FP
// Vorsicht, funktioniert nicht, Items werden immer im Mittelpunkt der Welt inserted

void Wld_InsertNpc(int npcInstance, string spawnPoint);
// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf.

void Wld_InsertNpcAndRespawn(int instance, string spawnPoint, float spawnDelay);
// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf. Stirbt dieser NSC wird
// nach "spawnDelay"-Sekunden ein neuer NSC am Spawnpoint erzeugt.

int Wld_IsFPAvailable(c_npc self, string fpName);
// Sucht einen Freepoint im Umkreis von 20m vom NSC und liefert TRUE falls vorhanden und frei ('self' zählt als Blockierer nicht!) und sichtbar

int Wld_IsMobAvailable(c_npc self, string schemeName);
// Sucht sich ein Mobsi im Umkreis von 10m und liefert TRUE falls gefunden. MOB wird nur zurückgeliefert, wenn es nicht besetzt ist.

int Wld_IsNextFPAvailable(c_npc self, string fpName);
//  wie Wld_IsFPAvailable(), aber es wird immer der nahegelegenste genommen und 'self' zählt als Blockierer!

int Wld_IsTime(int hour1, int min1, int hour2, int min2);
// Liefert "1" wenn aktuelle Weltzeit zwischen den beiden angegebenen Zeiten liegt (von - bis)

int Wld_RemoveItem(c_item item);
// G1: Hiermit wird das angegebene Item aus der Welt entfernt und gelöscht
// G2: !!! hiermit wird das globale Item aus der Welt gelöscht ???

void Wld_SetGuildAttitude(int guild1, int attitude, int guild2);
// Setze Gildenattitude neu

void Wld_SetMobRoutine(int hour1, int min1, string objName, int state);
// _Alle_ Mobs mit diesem _Schemanamen_ werden getriggert.

void Wld_SetObjectRoutine(int hour1, int min1, string objName, int state);
// _Ein_ Objekt mit diesem _Vobnamen_ wird getriggert

