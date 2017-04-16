/** @file */

func void AI_AimAt(var c_npc attacker, var c_npc target)
/// \remarks NPC zielt mit Fernkampfwaffe auf Target-NPC
{};

func void AI_AlignToFP(var c_npc npc)
/// \remarks richtet den Nsc am Freepoint aus (im Spacer gesetzte Pfeilrichtung)
{};

func void AI_AlignToWP(var c_npc npc)
/// \remarks richtet den Nsc am Waypoint aus (im Spacer gesetzte Pfeilrichtung)
{};

func void AI_Ask(var c_npc npc, var func anserYes, var func answerNo)
/// \remarks die angegeben Instanz (npc), gibt eine Frage aus und verweist auf die selbst zu definierenden Funktionen,
/// \remarks die für die Fälle Spieler sagt ja (Daumen nach oben) und Spieler sagt nein (Daumen unten) vorhanden sein müssen
{};

func void AI_AskText(var c_npc npc, var func funcYes, var func funcNo, var string strYes, var string strNo)
/// \remarks wie AI_Ask, nur das außer den Funktionen auch noch Strings für die Antworten Ja/Nein mit angegeben werden können
{};

func void AI_Attack(var c_npc npc)
/// \remarks NPC npc startet Kampf AI (sollte in der ZS_Attack-Loop stehen)
/// \remarks Es wird das interne Ziel verwendet, das mit Npc_SetTarget() oder Npc_GetNextTarget() gesetzt wurde.
{};

func void AI_CanSeeNpc(var c_npc npc, var c_npc other, var func see)
{};

func void AI_CombatReactToDamage(var c_npc npc)
{};

func void AI_ContinueRoutine(var c_npc npc)
/// \remarks Setze Tagesablauf fort
/// \remarks Enthält Standup
{};

func void AI_Defend(var c_npc npc)
/// \remarks Der Befehl ist als Overlay-Message implementiert. Das heisst, dass er neben anderen Nachrichten
/// \remarks aktiv bleibt. Er wird erst beendet, wenn der NPC eine Parade (ausgeloest durch die Attacke eines
/// \remarks anderen NPCs) durchgefuert hat.
{};

func void AI_Dodge(var c_npc npc)
/// \remarks Der Nsc weicht ein Stück nach hinten aus
{};

func void AI_DrawWeapon(var c_npc npc)
/// \remarks Equipte Waffe wird gezogen
{};

func void AI_DropItem(var c_npc npc, var c_item_id itemInstance)
/// \remarks Item wird auf den Boden fallen gelassen
{};

func void AI_DropMob(var c_npc npc)
{};

func void AI_EquipArmor(var c_npc npc, var c_item_id itemInstance)
/// \remarks Ziehe die angebene Rüstung dem NSC "npc" an, diese muss sich in seinem Inventory befinden.
{};

func void AI_EquipBestArmor(var c_npc npc)
/// \remarks Wunder, Wunder hier wird die beste im Inventory vorhandene Rüstung angezogen
{};

func int AI_EquipBestMeleeWeapon(var c_npc npc)
/// \remarks sucht im Inventory nach der besten Nahkampfwaffe und hängt sie an den Gürtel
{};

func int AI_EquipBestRangedWeapon(var c_npc npc)
/// \remarks sucht im Inventory nach der besten Fernkampfwaffe und ploppt sie auf den Rücken der Instanz
{};

func void AI_FinishingMove(var c_npc npc, var c_npc other)
/// \remarks Führt den logischen Finishing Move inklusive Anis aus den Skripten heraus aus
{};

func void AI_Flee(var c_npc npc)
/// \remarks Der Befehl muss, wie AI_Attack(), in der ZS-Loop regelmaessig aufgerufen werden und setzt voraus, dass
/// \remarks vorher mit Npc_SetTarget( self, <var C_NPC enemy> ) ein Gegner gesetzt wurde, vor dem der Npc fliehen soll.
{};

func void AI_GotoFP(var c_npc npc, var string freepoint)
/// \remarks Sucht sich einen Freepoint im Umkreis von 20m vom NSC, bewegt sich dorthin und richtet sich entsprechend aus.
/// \remarks Suchkriterium wie bei Wld_IsFPAvailable()
{};

func void AI_GotoItem(var c_npc npc, var c_item item)
/// \remarks "npc" geht zu "item"
{};

func void AI_GotoNextFP(var c_npc npc, var string freepoint)
/// \remarks wie AI_GotoFP() allerdings Suchkriterium wie bei Wld_IsNextFPAvailable()
{};

func void AI_GotoNpc(var c_npc npc, var c_npc other)
/// \remarks "npc" geht zu "other"
{};

func void AI_GotoSound(var c_npc npc)
/// \remarks Npc npc läuft zum Sound
{};

func void AI_GotoWP(var c_npc npc, var string waypoint)
/// \remarks Npc-Instanz npc läuft zum namentlich angegeben Waypoint
{};

func void AI_LookAt(var c_npc npc, var string wpOrVob)
/// \remarks Schaue auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)
{};

func void AI_LookAtNpc(var c_npc npc, var c_npc other)
/// \remarks Schaue zu einem NSC
{};

func void AI_LookForItem(var c_npc npc, var c_item_id itemInstance)
/// \remarks gibt die Möglichkeit nach bestimmten Items zu suchen (z.B.:Das goldene Schwert der Zerstörung, wenn vorhanden)
{};

func void AI_Output(var c_npc npc, var c_npc target, var string wavName)
/// \remarks Cutscene mit entsprechender Id wird abgenudelt
{};

func void AI_OutputSVM(var c_npc npc, var c_npc target, var string svmName)
/// \remarks Outputbefehl um Svms abzuspielen
{};

func void AI_OutputSVM_Overlay(var c_npc npc, var c_npc target, var string svmName)
/// \remarks wie AI_OutputSVM, wartet jedoch NICHT mit der Ausführung des nächsten AI_...-Befehls, bis
/// \remarks das SVM zuende gespielt wird. (Für Kommentare kurz vor und während dem Kampf!)
{};

func void AI_PlayAni(var c_npc npc, var string animationName)
/// \remarks Npc-Instanz npc spielt die angegebene Animation ab
{};

func void AI_PlayAniBS(var c_npc npc, var string animationName, var int bodystate)
/// \remarks Beim Abspielen einer Ani mit diesem Befehl kann ein Bodystate angemeldet werden
{};

func void AI_PlayCutscene(var c_npc npc, var string csName)
/// \remarks Eine Cutscene aus den Scripten heraus starten
{};

func void AI_PlayFX(var c_npc npc, var c_npc other, var string videoName)
{};

func void AI_PointAt(var c_npc name, var string wpOrVob)
/// \remarks Zeige auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)
{};

func void AI_PointAtNpc(var c_npc npc, var c_npc other)
/// \remarks Zeige auf einen NSC
{};

func int AI_PrintScreen(var string text, var int x, var int y, var string font, var int timeSec)
{};

func void AI_ProcessInfos(var c_npc npc)
{};

func void AI_Quicklook(var c_npc npc, var c_npc other)
/// \remarks NSC kurz ( 2 sec) anschauen ( nur Kopf bewegt sich )
{};

func void AI_ReadyMeleeWeapon(var c_npc npc)
/// \remarks Ziehe equippte Nahkampfwaffe
{};

func void AI_ReadyRangedWeapon(var c_npc npc)
/// \remarks Ziehe equippte Fernkampfwaffe
{};

func void AI_ReadySpell(var c_npc npc, var int spellID, var int investMana)
/// \remarks Lasse zauberspruch auf Hand erscheinen.
{};

func void AI_RemoveWeapon(var c_npc npc)
/// \remarks Gezogene Waffe wird weggesteckt
{};

func void AI_SetNpcsToState(var c_npc npc, var func aiStateFunc, var int radius)
/// \remarks Setze alle NSCs im Umkreis von x cm in entsprechenden AI-Zustand
/// \remarks VORSICHT: Diese Funktion ist Buggy, wird aber derzeit auch NICHT verwendet!
/// \remarks -> FINGER WEG!!! (SN)
{};

func void AI_SetWalkmode(var c_npc npc, var int moveMode)
/// \remarks gibt an mit welchem Walkmode Run etc der Character durch das Level läuft
/// \remarks NPC_RUN               : Rennen
/// \remarks NPC_WALK              : Gehen
/// \remarks NPC_SNEAK             : Schleichen
/// \remarks NPC_RUN_WEAPON        : Rennen mit gezogener Waffe
/// \remarks NPC_WALK_WEAPON       : Gehen mit gezogener Waffe
/// \remarks NPC_SNEAK_WEAPON      : Schleichen mit gezogener Waffe
{};

func void AI_ShootAt(var c_npc attacker, var c_npc target)
/// \remarks NPC feuert mit Fernkampfwaffe auf Target-NPC
{};

func void AI_Snd_Play(var c_npc npc, var string snd)
{};

func void AI_Snd_Play3D(var c_npc npc, var c_npc other, var string snd)
{};

func void AI_StandUp(var c_npc npc)
/// \remarks - Ist der Nsc in einem Animatinsstate, wird die passende Rücktransition abgespielt.
/// \remarks - Benutzt der NSC gerade ein MOBSI, poppt er ins stehen.
{};

func void AI_StandUpQuick(var c_npc npc)
/// \remarks Wie AI_StandUp(), jedoch werden keine Rücktransitionen abgespielt, sondern auch dort wird
/// \remarks sofort in den Grundzustand "gepoppt". Wichtig für sehr eilige Situationen!
{};

func void AI_StartState(var c_npc npc, var func state, var bool waitForCurrentStateEnd, var string waypoint)
/// \remarks Versetzt den Nsc aus den Skripten heraus in den entsprechenden ZS (what),
/// \remarks stateBehaviour sagt : "0"-aktuellen Zustand abbrechen "1"-aktuellen Zustand erst ordnungsgemäß beenden (End-Funktion aufrufen) ).
{};

func void AI_StopAim(var c_npc attacker)
/// \remarks NPC beendet vorher gestartetes Zielen mit Fernkampfwaffe.
{};

func void AI_StopFX(var c_npc npc, var string videoName)
{};

func void AI_StopLookAt(var c_npc npc)
/// \remarks wieder geradeaus schauen
{};

func void AI_StopPointAt(var c_npc npc)
/// \remarks nicht mehr auf etwas zeigen
{};

func void AI_StopProcessInfos(var c_npc npc)
/// \remarks Der DialogModus wird beendet (Multiple Choice-Dialog)
{};

func void AI_TakeItem(var c_npc npc, var c_item item)
/// \remarks der Npc nimmt mit dieser Funktion die globale Item-Instanz auf
{};

func void AI_TakeMob(var c_npc npc, var string mobName)
{};

func void AI_Teleport(var c_npc npc, var string waypoint)
/// \remarks teleportiert den NSC zur angegebenene Location
{};

func void AI_TurnAway(var c_npc turner, var c_npc npc)
/// \remarks Der NSC turner dreht dem NSC npc den Rücken zu.
{};

func void AI_TurnToNpc(var c_npc turner, var c_npc npc)
/// \remarks dreht npc turner zu NSC npc
{};

func void AI_TurnToSound(var c_npc npc)
/// \remarks Charakter dreht sich zur Geräuschquelle
{};

func void AI_UnequipArmor(var c_npc npc)
/// \remarks Unequippe aktuelle Rüstung
{};

func void AI_UnequipWeapons(var c_npc npc)
/// \remarks Unequippe alle Waffen
{};

func void AI_UnreadySpell(var c_npc npc)
/// \remarks lasse zauberspruch aus Hand verschwinden
{};

func void AI_UseItem(var c_npc npc, var c_item_id itemInstance)
/// \remarks Item bis zum Ende benutzen
{};

func void AI_UseItemToState(var c_npc npc, var c_item_id itemInstance, var int state)
/// \remarks Item benutzen bis zum angegebenen State
{};

func int AI_UseMob(var c_npc npc, var string schemeName, var int targetState)
/// \remarks Benutze Mob mit angegebenen Schema-Namen bis zum Zustand "targetState". Wird diese Funktion aufgerufen
/// \remarks und der angegebene 'targetState' ist bereits vorhanden, läuft der NSC zwar trotzdem zum MOB, tut dann aber nichts
{};

func void AI_Wait(var c_npc npc, var float sec)
/// \remarks Character wird für sec Sekunden in einen Wait-Zustand versetzt,
/// \remarks d.h. er tut nichts, aber Treffer etc.(passive Wahrnehmung) werden registriert
{};

func void AI_WaitForQuestion(var c_npc npc, var func scriptFunc)
/// \remarks NSC wartet 20 Sekunden, wird er in dieser Zeit vom Spieler angesprochen, wird die angegebene SkriptFunktion ausgeführt.
{};

func void AI_WaitMS(var c_npc npc, var int milliSeconds)
{};

func void AI_WaitTillEnd(var c_npc npc, var c_npc other)
/// \remarks 'npc' wartet bis 'other' seinen laufenden AI-Befehl zu Ende gespielt hat (funzt allerdings nicht bei AI-Overlays!)
{};

func void AI_WhirlAround(var c_npc npc, var c_npc other)
/// \remarks schnelle Drehung zu other
{};

func void AI_WhirlAroundToSource(var c_npc npc)
{};

func void Apply_Options_Audio()
{};

func void Apply_Options_Controls()
{};

func void Apply_Options_Game()
{};

func void Apply_Options_Performance()
{};

func void Apply_Options_Video()
{};

func string ConcatStrings(var string s1, var string s2)
/// \remarks Erzeugt eine Kopie von dem ersten angegebenen String, hängt den zweiten an diesen an und gibt den neuen String zurück
{};

func void CreateInvItem(var c_npc npc, var c_item_id itemInstance)
/// \remarks gibt der angegebenen Instanz npc ein Item ins Inventory
{};

func void CreateInvItems(var c_npc npc, var c_item_id itemInstance, var int amount)
/// \remarks wie vorherige Funktion, nur das Multi-Items (z.B.: 100 Pfeile) ins Inventory kreiert werden
{};

func int Doc_Create()
/// \remarks G1: erstellet ein Dokument und liefert ein Handle zurück, daß für alle weiteren Doc_...-Befehle nötig ist
/// \remarks Erzeugt ein Dokument.
/// \remarks
/// \remarks      [result]        - Handle auf das Dokument (-1 = konnte nicht erzeugt werden)
{};

func int Doc_CreateMap()
/// \remarks Gothic 2 only function
/// \remarks Erzeugt ein Dokument (Karte).
/// \remarks
/// \remarks      [result]        - Handle auf das Dokument (-1 = konnte nicht erzeugt werden)
{};

func void Doc_Font(var string fontName)
/// \remarks Gothic 1 only function
/// \remarks Benutzt den angegebenen Font für nachfolgende "Doc_Print"s.
{};

func void Doc_MapCoordinates(var string levelName, var float gameX1, var float gameY1, var float pixelX1, var float pixelY1, var float gameX2, var float gameY2, var float pixelX2, var float pixelY2)
/// \remarks Gothic 1 only function
/// \remarks Karte initialisieren : levelName = Name des Levels (.ZEN) aus dem die Karte einen Ausschnitt zeigt
/// \remarks gamex1,gamey1 : Koordinaten im Spiel (linke obere Ecke der Karte)
/// \remarks pixelx1,pixely1 : PixelKoordinaten linke obere Ecke // der Rest : alles nochmal für die rechte untere Ecke
{};

func void Doc_Open(var string texture)
/// \remarks Gothic 1 only function
/// \remarks Öffnet ein Dokument mit dem Bild "texture" als Hintergrund
{};

func void Doc_Print(var string text)
/// \remarks Gothic 1 only function
/// \remarks Angegebener Text wird in das aktuelle (vorher mit Doc_Open geöffnete) Dokument geschrieben
{};

func void Doc_PrintLine(var int document, var int page, var string text)
/// \remarks G1: Gibt Text für die Seite 'page' an. Der Text wird NICHT umgebrochen
/// \remarks Setzt den Text einer Seite. Der Text wird _nicht_ umgebrochen.
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Page            - Index der Seite (beginnt mit 0)
/// \remarks      Text            - Textzeile
{};

func void Doc_PrintLines(var int document, var int page, var string text)
/// \remarks G1: Gibt Text für die Seite 'page' an. Der Text WIRD umgebrochen, falls er nicht auf die in Doc_SetPage() angegebene Grafik passt.
/// \remarks Setzt den Text einer Seite. Der Text wird umgebrochen falls notwendig.
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Page            - Index der Seite (beginnt mit 0)
/// \remarks      Text            - Textzeilen
{};

func void Doc_SetFont(var int document, var int page, var string font)
/// \remarks G1: setzt den Font für die angegebene Seite 'page' oder für das ganze Dokument ('page':-1)
/// \remarks G1: Fontname z.B. "Scrolls.tga"
/// \remarks Setzt den Font einer/aller Seite(n).
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Page            - Index der Seite (beginnt mit 0, -1 = alle)
/// \remarks      Font            - Dateiname der Font-Textur (ohne Pfad - z.B. 'Font_Default.tga')
{};

func void Doc_SetLevel(var int document, var string level)
/// \remarks Gothic 2 only function
/// \remarks Setzt das Level-ZEN zur Anzeige der Spieler-Position (Karten).
/// \remarks
/// \remarks (als Grenze gilt die BBox des statischen Level-Meshes)
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Level           - Dateiname des Level-ZENs (mit Pfad ab WORLDS - z.B. 'NewWorld\NewWorld.zen')
{};

func void Doc_SetLevelCoords(var int document, var int left, var int top, var int right, var int bottom)
/// \remarks Gothic 2 only function
/// \remarks Setzt die Grenzen des Level-Bereichs, der durch die Textur abgedeckt wird (Karten).
/// \remarks
/// \remarks (als Grenzen werden _absolute_ Level-Koordinaten angegeben - z.B.: '..., -28000, 50500, 95500, -42500')
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Left            - linke Grenze
/// \remarks      Top                     - obere Grenze
/// \remarks      Right           - rechte Grenze
/// \remarks      Bottom          - untere Grenze
{};

func void Doc_SetMargins(var int document, var int page, var int left, var int top, var int right, var int bottom, var bool inPixels)
/// \remarks G1: setzt die Ränder (gerechnet vom Rand der TGA-Datei, die in Doc_SetPage() gesetzt wird). Die Ränder sind in Pixeln angegeben.
/// \remarks Setzt den jeweiligen Rand einer/aller Seite(n) (vom Rand der mit Doc_SetPage gesetzten Textur aus gesehen).
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Page            - Index der Seite (beginnt mit 0, -1 = alle)
/// \remarks      Left            - linker Rand
/// \remarks      Top             - oberer Rand
/// \remarks      Right           - rechter Rand
/// \remarks      Bottom          - unterer Rand
/// \remarks      Pixel           - Flag ob Angaben in Pixeln (Bildschirm) oder virtuellen Koordinaten (0-8192)
{};

func void Doc_SetPage(var int document, var int page, var string texture, var int scale)
/// \remarks G1: setzt für die Seite 'page' die Grafik-Datei 'pageimage'. Diese muß im TGA-Format vorliegen und
/// \remarks G1: z.B. "aufgeschlagenesBuch_links.tga"  heißen
/// \remarks G1: wird bei 'page' -1 übergeben, so bezieht sich der Befehl auf ALLE Seiten des Dokuments
/// \remarks Setzt die Hintergrund-Textur der Seite eines Dokuments.
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Page            - Index der Seite (beginnt mit 0, -1 = alle)
/// \remarks      Scale           - Flag ob Textur gestreckt werden soll
{};

func void Doc_SetPages(var int document, var int count)
/// \remarks G1: setzt die Anzahl der Seiten dieses Dokuments. Bei mehr als zwei Seiten wird versucht ALLE
/// \remarks G1: Seiten auf den Bildschirm zu bekommen
/// \remarks Setzt die Anzahl der Seiten eines Dokuments.
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
/// \remarks      Count           - Anzahl der Seiten
{};

func void Doc_Show(var int document)
/// \remarks G1: nachdem das Dokument mit den obenstehenden Befehlen befüllt und formatiert wurde, wird es hiermit nun auf den Bildschirm geschrieben!
/// \remarks Zeigt das Dokument auf dem Bildschirm an.
/// \remarks
/// \remarks      Document        - Handle auf das Dokument
{};

func void EquipItem(var c_npc npc, var c_item_id itemInstance)
/// \remarks gibt dem Nsc direkt das angegebene Item an den Gürtel
{};

func void ExitGame()
/// \remarks Gothic 2 only function
/// \remarks Beendet Gothic.
{};

func void ExitSession()
/// \remarks Gothic 2 only function
/// \remarks Beendet das laufende Spiel.
{};

func int FloatToInt(var float f)
/// \remarks !!! konvertiert einen float in einen int, hier wäre es noch ganz schön zu wissen, wie mit dem float umgesprungen wird --> schneiden oder runden ???
{};

func string FloatToString(var float f)
{};

func bool Game_InitEnglish()
/// \remarks Gothic 2 only function
/// \remarks Setzt die interne Engine-Variablen auf English.
/// \remarks
/// \remarks      [result]        - Zeigt an, ob die Sprache erfolgreich geladen wurde.
{};

func bool Game_InitGerman()
/// \remarks Gothic 2 only function
/// \remarks Setzt die interne Engine-Variablen auf Deutsch.
/// \remarks
/// \remarks      [result]        - Zeigt an, ob die Sprache erfolgreich geladen wurde.
{};

func bool Hlp_CutscenePlayed(var string csName)
/// \remarks Abfrage, ob Cutscene schon gespielt wurde (0 = Nein / 1 = Ja)
{};

func int Hlp_GetInstanceID(var instance itemOrNpcInstance)
/// \remarks liefert die interne ID ( nicht Var aus der Instanz) zurück, um zwei items oder npcs miteinander vergleichen zu können ( integer Vergleich)
/// \remarks itemOrNPC kann vom Typ C_ITEM oder C_NPC sein
{};

func c_npc Hlp_GetNpc(var c_npc_id npcInstance)
/// \remarks Ermittle einen NSC über den Instanznamen. Dieser kann einer Variablen zugewiesen werden und ist somit gezielt verfügbar
{};

func bool Hlp_IsItem(var c_item item, var c_item_id itemInstance)
/// \remarks Prüft, ob der Gegenstand 'item' den Instanznamen 'itemInstance' trägt. Will man z.B. testen,
/// \remarks ob die globale Variable item Pfeile sind (ItMuArrow) ruft man Hlp_IsItem(item,ItMuArrow) auf.
/// \remarks Gribt bei Gleichheit TRUE, sonst FALSE zurück.
{};

func bool Hlp_IsValidItem(var c_item item)
/// \remarks Prüfe ob Item-Instanz gültig und initialisiert ist True/False
{};

func bool Hlp_IsValidNpc(var c_npc npc)
/// \remarks Prüfe ob NSC-Instanz gültig und initialisiert ist. True/False
{};

func int Hlp_Random(var int excludedUpperBound)
/// \remarks erzeugt einen Zufallswert (wobei maxNotIncluded Obergrenze : [0..n0-1] ) Rückgabewert integer
{};

func bool Hlp_StrCmp(var string s1, var string s2)
/// \remarks Hilfsfunktion um zwei Strings miteinander zu vergleichen liefert bei Gleichheit eins zurück
{};

func void Info_AddChoice(var c_info_id infoInstance, var string text, var func f)
/// \remarks Fügt der Dialogoption info eine neue Subdialog-Option hinzu (Reihenfolge: oben einfügen, nicht unten dranhängen)
{};

func void Info_ClearChoices(var c_info_id infoInstance)
/// \remarks Löscht alle Subdialogoptionen, die zur Dialogoption gehören
{};

func bool InfoManager_HasFinished()
{};

func void IntroduceChapter(var string titel, var string subtitle, var string texture, var string wavName, var int waitTimeMS)
/// \remarks Gothic 2 only function
/// \remarks Zeigt den Kapitelwechsel-Bildschirm an.
/// \remarks
/// \remarks      Titel           - Text des Titels
/// \remarks      subtitle        - text des Untertitels
/// \remarks      Texture         - Dateiname der Hintergrundtextur
/// \remarks      Sound           - Dateiname des abgespielten Sounds
/// \remarks      waitTimeMS      - Anzahl in Millisekunden, die der Bildschirm angezeigt wird
{};

func float IntToFloat(var int x)
/// \remarks !!! wie float to int, nur daß ein integer in float umgewandelt wird ???
{};

func string IntToString(var int x)
/// \remarks !!!Funktion überträgt eine Integer variable in eine Stringvariable (Rückgabewert : string)???
{};

func void Log_AddEntry(var string topic, var string entry)
/// \remarks      Der Befehl fügt dem Topics topic ein neues Entry namens entry hinzu. Wie auch bei der Erzeugung von Topics dient hier der
/// \remarks      Name sowohl zur Identifikation als auch zur Anzeige und muss somit eindeutig sein. Doppelte Einträge werden auch hier ignoriert.
/// \remarks
/// \remarks      Parameter
/// \remarks      topic           Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
/// \remarks      entry           Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Eintrags verwendet wird.
{};

func void Log_CreateTopic(var string topic, var int section)
/// \remarks      Der Befehl fügt unter der Sektion 'section' ein neues Topic mit Namen 'topic' ein.
/// \remarks      Sollte bereits ein Topic dieses Names in irgendeiner Sektion existieren, wird das Topic nicht hinzugefügt.
/// \remarks
/// \remarks      Parameter:
/// \remarks      - topic         Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Topics verwendet wird.
/// \remarks      - section       Eine, in der Constants.d definierte Konstante, die besagt, in welcher Sektion das Topic angezeigt werden soll.
/// \remarks                      Die Konstante kann folgende Werte annehmen:
/// \remarks                          - LOG_MISSION
/// \remarks                          - LOG_NOTE
{};

func void Log_SetTopicStatus(var string topic, var int status)
/// \remarks      Der Befehl ändert den Status des Topics 'name' in 'status'.
/// \remarks      Er sollte nur für Topics verwendet werden, die für die Sektion LOG_MISSION erzeugt wurden.
/// \remarks
/// \remarks      Parameter:
/// \remarks      - topic         Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
/// \remarks      - status        Eine, in der Constants.d definierte Konstante, die besagt, unter welchem Status die Mission dargestellt werden soll.
/// \remarks                      Folgende Werte sind möglich:
/// \remarks                              - LOG_RUNNING
/// \remarks                              - LOG_SUCCESS
/// \remarks                              - LOG_FAILED
/// \remarks                              - LOG_OBSOLETE
{};

func void Mdl_ApplyOverlayMds(var c_npc npc, var string mdsName)
/// \remarks mit diesem Befehl werden Animationen auf einem höheren Layer gestartet
/// \remarks (z.B. hat der Zombie ein Overlay über die normalen Human-Animationen)
{};

func void Mdl_ApplyOverlayMDSTimed(var c_npc npc, var string mdsName, var int timeTicks)
/// \remarks Overlay-MDS wird gestartet und automatisch nach der angegebenen Zeit abgeschaltet
{};

func void Mdl_ApplyRandomAni(var c_npc npc, var string animationName, var string alias)
/// \remarks Fügt Zufallsanimationen (am Kopf kratzen, an den Sack packen etc (s2)) für bestimmte Animationszustände (s1) ein
{};

func void Mdl_ApplyRandomAniFreq(var c_npc npc, var string animationName, var float freq)
/// \remarks hiermit kann die Frequenz betimmt werden, wie oft die für den Animationszustand (s1) deklarierten Randomanis abgespielt werden
{};

func void Mdl_ApplyRandomFaceAni(var c_npc npc, var string animationName, var float minTime, var float minTimeVar, var float maxTime, var float maxTimeVar, var float probMin)
/// \remarks Starte zufällige Gesichtsanimation
/// \remarks Mdl_ApplyRandomFaceAni ( npc, animationName, minTime, minTimeVar, maxTime, maxTimeVar, probMin)
/// \remarks minTime      = Minimum an Zeit nachdem Ani startet (in Sekunden)
/// \remarks maxTime      = Maximum an Zeit nachdem Ani startet (in Sekunden)
/// \remarks minTimeVar   = Abweichung von minTime (in Sekunden)
/// \remarks maxTimeVar   = Abweichung von maxTime (in Sekunden)
/// \remarks probMin      = Wahrscheinlichkeit [0..1] ob obere oder untere Grenze der Abweichung benutzt wird
{};

func void Mdl_RemoveOverlayMDS(var c_npc npc, var string mdsName)
/// \remarks Entferne ein Overlay-MDS
{};

func void Mdl_SetModelFatness(var c_npc npc, var float fatness)
/// \remarks Setzt Model-Z-Skalierung
{};

func void Mdl_SetModelScale(var c_npc npc, var float x, var float y, var float z)
/// \remarks zum skalieren des Meshes (Breite,Höhe,Tiefe) 1 = 100%, also Normalgröße!
{};

func void Mdl_SetVisual(var c_npc npc, var string mdsName)
{};

func void Mdl_SetVisualBody(var c_npc npc, var string bodyMesh, var int bodyTex, var int skinColor, var string headMMS, var int headTex, var int toothTex, var int armor)
{};

func void Mdl_StartFaceAni(var c_npc npc, var string animationName, var float intensity, var float holdTime)
/// \remarks Starte Gesichtsanimation // intensity 1 = 100% // holdTime -1 = forever
{};

func void Mis_AddMissionEntry(var instance n0, var string s1)
{};

func int Mis_GetStatus(var int missionName)
/// \remarks Liefert aktuellen Status einer Mission zurück ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc.
{};

func bool Mis_OnTime(var int missionName)
/// \remarks Liefert TRUE, wenn sich Spieler noch innerhalb des Zeitlimits für diese Mission befindet
{};

func void Mis_RemoveMission(var instance n0)
{};

func void Mis_SetStatus(var int missionName, var int newStatus)
/// \remarks Setzt den Status einer Mission ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc. )
{};

func void Mob_CreateItems(var string mobName, var c_item_id itemInstance, var int amount)
/// \remarks Erzeuge "amount" Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen.
{};

func int Mob_HasItems(var string mobName, var c_item_id itemInstance)
/// \remarks Liefere Anzahl der Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen
{};

func bool Npc_AreWeStronger(var c_npc npc, var c_npc other)
/// \remarks ermittelt den stärkeren Nsc,
/// \remarks Wenn die Summe der Level aller NPCs (human, Monster), die zu mir feindlich sind und die zu <other> freundlich sind
/// \remarks MEHR ALS DOPPELT SO HOCH ist (x > 2*y), wie die Summe der Level aller Leute, die zu mir freundlich sind, und die
/// \remarks zu <other> feindlich sind, dann NEIN,
/// \remarks sonst JA (= wir SIND stärker, nicht echt, aber ab doppelter Gegnerstärke zu fliehen ist früh genug, sonst kommen
/// \remarks die Jungs aus dem Rennen nicht mehr raus
/// \remarks Beachten:
/// \remarks 1) Monster können feindlich zu beiden Menschen sein --> egal, zählen auf beiden Seiten
/// \remarks 2) Jemand, der z.B. zu mir freundlich und zu <other> feindlich ist, wird demnach doppelt gewertet --> ok so
{};

func bool Npc_CanSeeItem(var c_npc npc, var c_item item)
/// \remarks Prueft ob Npc den Gegenstand sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False
{};

func bool Npc_CanSeeNpc(var c_npc npc, var c_npc other)
/// \remarks Prueft ob NPC npc NPC other sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False Blickwinkelabhängig (+,-100°)+LOS von der Hüfte aus
{};

func bool Npc_CanSeeNpcFreeLOS(var c_npc npc, var c_npc other)
/// \remarks Prüft ob NSC anderen NSC sehen kann, ohne dabei den Winkel zu berücksichtigen (nur LineOfSight-Check)
{};

func bool Npc_CanSeeSource(var c_npc npc)
/// \remarks Checkt, ob NSC die Soundquelle sehen kann True/False
{};

func void Npc_ChangeAttribute(var c_npc npc, var int attribute, var int value)
/// \remarks ändert den Wert des Attributs (attribute) um (value) Einheiten.
{};

func bool Npc_CheckAvailableMission(var c_npc npc, var int missionState, var bool important)
/// \remarks Prüfe ob zwischen dem NSC eine aktuelle Mission (AVAILABLE,RUNNING) besteht und vergleiche den Status mit den angegebenen.
/// \remarks Falls dieser identisch ist, wird "1" geliefert.
{};

func bool Npc_CheckInfo(var c_npc npc, var bool important)
/// \remarks Überprüft,ob der NSC gültige Infos für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").
{};

func bool Npc_CheckOfferMission(var c_npc npc, var bool important)
/// \remarks Überprueft ob der NSC dem Spieler einen Auftrag anbieten kann, wenn ja, wird der Offer-Block gestartet und "1" zurückgeliefert.
{};

func bool Npc_CheckRunningMission(var c_npc npc, var bool important)
/// \remarks Überprueft ob zwischen dem NSC und dem Spieler eine laufende Mission besteht und startet den entsprechenden Skriptblock.
/// \remarks (Returnwert 1 : Mission aktiv)
{};

func void Npc_ClearAIQueue(var c_npc npc)
/// \remarks hiermit werden alle Befehle mit sofortiger Wirkung aus der AI_Queue rausgeworfen
{};

func void Npc_ClearInventory(var c_npc npc)
/// \remarks leert das Inventar des NPC
{};

func void Npc_CreateSpell(var c_npc npc, var int spellnr)
/// \remarks NSc bekommt Zauberspruch zugewiesen kann diesen aber noch nicht gebrauchen
/// \remarks (erscheint ausgegraut im Auswahlkranz)
{};

func bool Npc_DeleteNews(var c_npc npc, var int newsID)
/// \remarks entfernt Nachricht bei npc, gibt 1 zurück wenn erfolgreich, sonst 0
{};

func void Npc_ExchangeRoutine(var c_npc npc, var string routineName)
/// \remarks Tausche meherere Tagesabläufe aus
{};

func int Npc_GetActiveSpell(var c_npc npc)
/// \remarks liefert den Zauber zurück, der auf der Hand des NPC ist
/// \remarks liefert -1 zurück, wenn kein Zauber auf der Hand ist
{};

func int Npc_GetActiveSpellCat(var c_npc npc)
/// \remarks Unterscheidet zwischen den drei Kategorien (Spell_Bad, Spell_neutral,Spell_Good) Spellkat ist Rückgabewert
{};

func bool Npc_GetActiveSpellIsScroll(var c_npc npc)
/// \remarks Gibt zurück ob der aktive Zauber auf der Hand des NPC npc eine Spruchrolle ist.
/// \remarks Wird benutzt um die korrekten Manakosten des Zaubers zu ermitteln, da sich Manakosten von Spruchrollen und Runen unterscheiden.
{};

func int Npc_GetActiveSpellLevel(var c_npc npc)
/// \remarks liefert den Spell-Level des Zaubers zurück, der auf der Hand ist
{};

func int Npc_GetAttitude(var c_npc npc, var c_npc other)
/// \remarks Gibt die Attitüde von npc zu other zurück (temp. / perm. / Gilden )
{};

func int Npc_GetBodyState(var c_npc npc)
/// \remarks Ermittle BodyState ( Liefert BS_-Konstanten )
/// \remarks !!! VORSICHT !!!: Diese Funktion gibt den Bodystate mit allen eventuellen
/// \remarks ver-oderten Modifiern wie "brennend". Besser 'C_BodyStateContains(self,bodystate)' benutzen
{};

func int Npc_GetComrades(var c_npc npc)
/// \remarks gibt 1 zurück, wenn der NPC npc Freunde hat, sonst - 0.
{};

func string Npc_GetDetectedMob(var c_npc npc)
/// \remarks liefert den Schemanamen des Mobsi zurück, das der Nsc entdeckt hat.
/// \remarks Schemaname ist das String-Kürzel des Mobs, daß im Visualnamen vor dem ersten "_"-Zeichen steht, also z.B. "DOOR" wenn der Visualname "DOOR_OCR__135" ist.
/// \remarks VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
/// \remarks WORKAROUND: zur Zeit werden hiermit nur Tür-MOBSIs zurückgegeben, dies soll aber wieder zurückgebaut werden
{};

func int Npc_GetDistToItem(var c_npc npc, var c_item item)
/// \remarks Liefert Entfernung ( in cm ! ) zwischen NSC und Gegenstand
{};

func int Npc_GetDistToNpc(var c_npc npc, var c_npc other)
/// \remarks Liefert Entfernung ( in cm ! ) zwischen den beiden NSCs
{};

func int Npc_GetDistToPlayer(var c_npc npc)
/// \remarks Liefert Entfernung ( in cm ! ) zwischen npc und Spieler
{};

func int Npc_GetDistToWP(var c_npc npc, var string waypoint)
/// \remarks liefert die Entfernung vom NSC 'npc' zum angegebenen Waypoint in cm
{};

func c_item Npc_GetEquippedArmor(var c_npc npc)
/// \remarks Liefert die angelegte Rüstung des NSCs.
{};

func c_item Npc_GetEquippedMeleeWeapon(var c_npc npc)
/// \remarks Liefert die gegurtete Nahkampfwaffe des NSCs.
{};

func c_item Npc_GetEquippedRangedWeapon(var c_npc npc)
/// \remarks Liefert die gegurtete Fernkampfwaffe des NSCs.
{};

func int Npc_GetGuildAttitude(var c_npc npc, var c_npc other)
/// \remarks Ermittelt die Gildenattitüde von zwei Nsc´s direkt im Gegensatz zu Wld_GetGuildAttitude
{};

func int Npc_GetHeightToItem(var c_npc npc, var c_item item)
/// \remarks Distanz in cm
{};

func int Npc_GetHeightToNpc(var c_npc npc1, var c_npc npc2)
/// \remarks Gothic 2 only function
/// \remarks Liefert Höhendifferenz ( in cm ! ) zwischend den beiden NSCs
{};

func bool Npc_GetInvItem(var c_npc npc, var c_item_id itemInstance)
/// \remarks Ermittle ItemInstanz aus Inventory
/// \remarks befüllt globale Variable "item" mit dem gefundenen C_Item, falls das item gültig ist. Gibt TRUE zurück, wenn item im inventar gefunden wurde
{};

func int Npc_GetInvItemBySlot(var c_npc npc, var int category, var int slotNr)
/// \remarks Mit diesem Befehl läßt sich nachsehen, ob in einem bestimmten Slot einer bestimmten Kategorie ein item vorhanden ist
/// \remarks ist das der Fall, wird dieses Item in die globale Variable item geschrieben
/// \remarks gibt jetzt die Anzahl zurueck, wenn das Item stackable ist.
/// \remarks Den Transfer machst Du dann per Npc_RemoveInvItems() und Npc_CreateInvItems().
{};

func int Npc_GetLastHitSpellCat(var c_npc npc)
/// \remarks Gothic 2 only function
/// \remarks liefert die Category des Zaubers zurück, der den NSC zuletzt getroffen hat
{};

func int Npc_GetLastHitSpellID(var c_npc npc)
/// \remarks Gothic 2 only function
/// \remarks liefert den Zauber zurück, der den NSC zuletzt getroffen hat
{};

func c_npc Npc_GetLookAtTarget(var c_npc npc)
/// \remarks setzt irgendeine globale Variable? target?
{};

func string Npc_GetNearestWP(var c_npc npc)
/// \remarks liefert den Namen des am nächsten gelegenen Waypoints zurück
{};

func c_npc Npc_GetNewsOffender(var c_npc npc, var int newsID)
/// \remarks Ermittelt Täter der News und gibt eine Instanz der Klasse C_NPC zurück
{};

func c_npc Npc_GetNewsVictim(var c_npc npc, var int newsID)
/// \remarks Ermittle Opfer der News und gibt eine Instanz der Klasse C_NPC zurück
{};

func c_npc Npc_GetNewsWitness(var c_npc npc, var int newsID)
/// \remarks Ermittle Zeuge der News und gibt eine Instanz der Klasse C_NPC zurück
{};

func bool Npc_GetNextTarget(var c_npc npc)
/// \remarks Aktive Suche nach einem Gegner. Wird ein Gegner gefunden, so wird er als internes Ziel
/// \remarks übernommen und in 'other' geschrieben, wenn kein Gegner gefunden wurde wird das
/// \remarks interne Ziel gelöscht und 'other' ungültig gemacht.
/// \remarks Kriterien:   1. gesetzter Gegner, der nicht tot/bewußtlos ist wird genommen...
/// \remarks              2. ...dann der nächstbeste aNSC demgegenüber 'npc' HOSTILE ist (auch weder tot noch bewußtlos)
/// \remarks - return:    neues Ziel gefunden     -> TRUE
/// \remarks              kein Ziel gefunden      -> FALSE
/// \remarks VORSICHT:    Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
/// \remarks              Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
/// \remarks              Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden
{};

func string Npc_GetNextWP(var c_npc npc)
/// \remarks Liefert den zweitnahesten WP vom NSC zurück
{};

func int Npc_GetPermAttitude(var c_npc npc, var c_npc other)
/// \remarks Ermittle die permanente Attitude von "npc" zu "other"
{};

func int Npc_GetPortalGuild(var c_npc npc)
{};

func c_npc Npc_GetPortalOwner(var c_npc npc)
{};

func c_item Npc_GetReadiedWeapon(var c_npc npc)
/// \remarks Liefert die gezogene Waffe des NSCs.
{};

func int Npc_GetStateTime(var c_npc npc)
/// \remarks Liefert Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
{};

func int Npc_GetTalentSkill(var c_npc npc, var int talent)
/// \remarks gibt talent level zurück. z.B. 5 für fünter Kreis der Magie. oder 0/1 für Taschendiebstahl gelernt/ungelernt
{};

func int Npc_GetTalentValue(var c_npc npc, var int talent)
/// \remarks gibt talent value zurück. Wird in gothic 1 benutzt für 1h/2h/bow/crossbow/picklock/pickpocket
{};

func bool Npc_GetTarget(var c_npc npc)
/// \remarks Befüllt 'other' mit dem aktuellen Ziel. Das aktuelle Ziel wird intern gespeichert, wird
/// \remarks durch Npc_SetTarget() bzw. Npc_GetNextTarget() gesetzt.
/// \remarks - return: aktuelles Ziel gespeichert     -> TRUE
/// \remarks           kein Ziel gespeichert          -> FALSE
{};

func int Npc_GetTrueGuild(var c_npc npc)
/// \remarks liefert immer!!! die wahre Gilde zurück, ignoriert also auch ohne die Regeln die Verkleidung
{};

func bool NPC_GiveInfo(var c_npc npc, var bool important)
/// \remarks Überprüft, ob der NSC eine (!) gültige Info für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").
{};

func void Npc_GiveItem(var c_npc npc, var c_item_id itemInstance, var c_npc other)
/// \remarks Der NSC "npc" gibt dem NSC "other" den angegebenen Gegenstand "itemInstance". Der Gegenstand wandert sofort ins Inventory des anderen.
{};

func bool Npc_HasBodyFlag(var c_npc npc, var int bodyFlag)
/// \remarks Liefert 1, falls BodyFlag gesetzt ist, sonst 0
{};

func bool Npc_HasDetectedNpc(var c_npc npc, var c_npc other)
/// \remarks liefert zurück, ob der Spieler in Sinnesreichweite und entdeckt ist
{};

func bool Npc_HasEquippedArmor(var c_npc npc)
/// \remarks Armor angezogen True/False
{};

func bool Npc_HasEquippedMeleeWeapon(var c_npc npc)
/// \remarks gibt zurück, ob eine Nahkampfwaffe Equipped ist
{};

func bool Npc_HasEquippedRangedWeapon(var c_npc npc)
/// \remarks Fernkampwaffe auf dem Rücken True/False
{};

func bool Npc_HasEquippedWeapon(var c_npc npc)
/// \remarks gibt eins zurück, wenn der npc eine Waffe sichtbar mit sich rumträgt
{};

func bool Npc_HasFightTalent(var c_npc npc, var int talent)
/// \remarks NICHT IMPLEMENTIERT. Spezialabfrage auf Kampftalente (z.B. 1hSword) ansonsten wie Npc_HasTalent
{};

func int Npc_HasItems(var c_npc npc, var c_item_id itemInstance)
/// \remarks Liefert zurück wie viele Items der NSC vom angegebenen Typ besitzt
{};

func int Npc_HasNews(var c_npc npc, var int newsID, var c_npc offender, var c_npc victim)
/// \remarks Liefert Newsnummer>0 (für weitere Referenzen) falls entsprechende News vorhanden.
/// \remarks Nicht benötigte Parameter können mit "NULL" leergelassen werden
{};

func bool Npc_HasOffered(var c_npc npc, var c_npc other, var c_item_id itemInstance)
/// \remarks Bietet Spieler dem NSC einen Gegenstand übers Trade-Modul an ? True/False
{};

func bool Npc_HasRangedWeaponWithAmmo(var c_npc npc)
/// \remarks Gibt TRUE zurück, wenn 'npc' irgendeine Fernkampfwaffe im Inventory oder in der Hand hat UND
/// \remarks dazu auch passende Munition vorhanden ist. Sonst FALSE.
{};

func bool Npc_HasReadiedMeleeWeapon(var c_npc npc)
/// \remarks Nahkampfwaffe in der Hand
{};

func bool Npc_HasReadiedRangedWeapon(var c_npc npc)
/// \remarks Fernkampfwaffe in der Hand
{};

func bool Npc_HasReadiedWeapon(var c_npc npc)
/// \remarks gibt zurück, ob die Waffe schon in der Hand ist
{};

func bool Npc_HasSpell(var c_npc npc, var int spellID)
/// \remarks Kann der NSC den angegebenen Zauberspruch benutzen ?
{};

func bool Npc_HasTalent(var c_npc npc, var int talent)
/// \remarks NICHT IMPLEMENTIERT. liefert eins zurück, wenn der geprüfte Charakter das Talent talent hat
{};

func bool Npc_IsAiming(var c_npc npc, var c_npc other)
/// \remarks liefert eins zurück, wenn der zweite Character auf den ersten zielt (Fernkampfwaffen und Zauber)
{};

func bool Npc_IsDead(var c_npc npc)
/// \remarks Funktion liefert einen Wert zurück, falls Dead true ist
{};

func int Npc_IsDetectedMobOwnedByGuild(var c_npc user, var int ownerGuild)
/// \remarks Liefert >0, falls "ownerGuild" der Besitzer des vom "user" benutzten Mob ist.
/// \remarks VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
{};

func int Npc_IsDetectedMobOwnedByNpc(var c_npc user, var c_npc owner)
/// \remarks Liefert >0, falls "owner" der Besitzer des vom "user" benutzten Mob ist.
/// \remarks VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
{};

func int Npc_IsDrawingSpell(var c_npc npc)
/// \remarks liefert spellID???
{};

func int Npc_IsDrawingWeapon(var c_npc npc)
/// \remarks ???
{};

func bool Npc_IsInCutscene(var c_npc npc)
/// \remarks liefert eins zurück, wenn der entsprechende Nsc in einer Cutscene ist
{};

func bool Npc_IsInFightMode(var c_npc npc, var int fightMode)
/// \remarks liefert eins zurück, wenn der Charakter im angegebenen Fightmode (z.B. FMODE_MAGIC) ist
{};

func bool Npc_IsInPlayersRoom(var c_npc npc)
/// \remarks liefert zurück ob der npc im Raum des Spielers ist???
{};

func bool Npc_IsInRoutine(var c_npc npc, var func state)
/// \remarks Check ob der angegebene Zustand der aktuelle TA des NSCs ist. True/FAlse
{};

func bool Npc_IsInState(var c_npc npc, var func state)
/// \remarks Abfrage auf den aktuellen Zustand einer Spielfigur True/False
{};

func bool Npc_IsNear(var c_npc npc, var c_npc other)
/// \remarks liefert eins zurück, wenn geprüfte Instanz sich im Bereich von drei Metern zur prüfenden Instanz befindet
{};

func bool Npc_IsNewsGossip(var c_npc npc, var int newsID)
/// \remarks Liefert 1, falls News "Gossip" ist
{};

func bool Npc_IsNextTargetAvailable(var c_npc npc)
/// \remarks Sucht genauso wie Npc_GetNextTarget nach einem neuen Ziel, überschreibt aber weder
/// \remarks das interne Ziel, noch 'other'
/// \remarks - return:    neues Ziel gefunden     -> TRUE
/// \remarks              kein Ziel gefunden      -> FALSE
/// \remarks VORSICHT:    Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
/// \remarks              Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
/// \remarks              Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden
{};

func bool Npc_IsOnFP(var c_npc npc, var string freepoint)
/// \remarks Abfrage darauf, ob der Nsc auf einem Freepoint mit freepoint Teilstring steht
{};

func bool Npc_IsPlayer(var c_npc npc)
/// \remarks liefert eins zurück, wenn der geprüfte Charakter der Spieler himself ist
{};

func bool Npc_IsPlayerInMyRoom(var c_npc npc)
/// \remarks gibt TRUE zurück, wenn sich SC im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
{};

func bool Npc_IsVoiceActive(var c_npc npc)
/// \remarks ???
{};

func bool Npc_IsWayBlocked(var c_npc npc)
/// \remarks Liefert "1", falls Weg von NSC durch Hindernis versperrt ist.
{};

func bool Npc_KnowsInfo(var c_npc npc, var c_info_id instanceInfo)
/// \remarks Liefert TRUE, wenn der angegebene Spieler die Info schon einmal erhalten hat.
/// \remarks VORSICHT: auch wenn eine permanente Info schon einmal dem Spieler erzählt wurde, so gibt diese Funktion trotzdem FALSE zurück!
{};

func bool Npc_KnowsPlayer(var c_npc npc, var c_npc player)
/// \remarks Kennt der NSC den Spieler? True/False
{};

func void Npc_LearnSpell(var c_npc npc, var int spell)
/// \remarks Was könnte das wohl heißen ? Ich glaube damit kann man den (spell) Zauber zuweisen
{};

func void Npc_MemoryEntry(var c_npc npc, var int source, var c_npc offender, var int newsID, var c_npc victim)
/// \remarks mit dieser Funktion wird eine NSC bezogene News geschrieben newsID : News ID source > 0 : "gossip", ansonsten "witness",
/// \remarks npc: NSC, bei dem News eingespeist werden soll, offender: Täter, victim : Opfer
{};

func void Npc_MemoryEntryGuild(var c_npc npc, var int source, var c_npc offender, var int newsID, var c_npc victimGuild)
/// \remarks wie MemoryEntry nur, das die Memory für die ganze Gilde kreiert wird
{};

func bool Npc_OwnedByGuild(var c_item item, var int guild)
/// \remarks Liefert "1", wenn der angegebenen Gilde das Item gehört ( Gildenbesitz )
{};

func bool Npc_OwnedByNpc(var c_item item, var c_npc npc)
/// \remarks Liefert "1", wenn dem NSC das Item gehört ( persönliches Besitzflag )
{};

func void Npc_PercDisable(var c_npc npc, var int percID)
/// \remarks Deaktiviere Perception
{};

func void Npc_PerceiveAll(var c_npc npc)
/// \remarks Nimm alle Objekte in Wahrnehmungsreichweite wahr, die dann mit WLD_DetectNpc und Wld_DetectItem auswertbar sind
{};

func void Npc_PercEnable(var c_npc npc, var int percID, var func action)
/// \remarks Aktiviere Perception
{};

func void Npc_PlayAni(var c_npc npc, var string animationName)
{};

func bool Npc_RefuseTalk(var c_npc npc)
/// \remarks Abfrage ob Dialog-Refuse Counter noch aktiv ist True/False
{};

func bool Npc_RemoveInvItem(var c_npc owner, var c_item_id itemInstance)
/// \remarks das angegebene Item wird aus dem Inventory des NSCs entfernt und gelöscht
/// \remarks True wenn erfolgreich (mindestens 1 Gegenstand vom Typ itemInstance befand sich im Inventar)
{};

func bool Npc_RemoveInvItems(var c_npc owner, var c_item_id itemInstance, var int amount)
/// \remarks das angegebene Anzahl des Multi-Items wird aus dem Inventory des NSCs entfernt und gelöscht
/// \remarks wie Npc_RemoveInvItem, nur das Multislotgegenstände gelöscht werden
/// \remarks True wenn erfolgreich (mindestens 1 Gegenstand vom Typ itemInstance befand sich im Inventar)
{};

func void Npc_SendPassivePerc(var c_npc npc1, var int perc_type, var c_npc victim, var c_npc offender)
/// \remarks Sende eine passive Wahrnehmung aus.Npc1 = wer schickt victim = Opfer, offender = Täter
{};

func void Npc_SendSinglePerc(var c_npc npc, var c_npc target, var int percID)
/// \remarks verschicke Wahrnehmung an einzelnen NSC
{};

func int Npc_SetActiveSpellInfo(var c_npc npc, var int value)
/// \remarks Hier kann ein Wert für den Zauberspruch gesetzt werden.
/// \remarks Was dieser Wert bewirkt, haengt allein von der Nutzung im Skript ab. Das Programm benutzt diesen nicht.
/// \remarks liefert den Spell-Level des Zaubers zurück, der auf der Hand ist
{};

func void Npc_SetAttitude(var c_npc npc, var int attitude)
/// \remarks setzt die permanente Attitüde auf einen festen Wert
{};

func void Npc_SetKnowsPlayer(var c_npc npc, var c_npc player)
/// \remarks NSC kennt SC
{};

func void Npc_SetPercTime(var c_npc npc, var float seconds)
/// \remarks Setze Zeitdelta für aktive Wahrnehmungen, alle Zeitdelta-Sekunden wird WN gesendet
{};

func void Npc_SetRefuseTalk(var c_npc npc, var int seconds)
/// \remarks Dialog Refuse Counter auf seconds Sekunden setzen
{};

func void Npc_SetStateTime(var c_npc npc, var int seconds)
/// \remarks _Setzt_ Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
/// \remarks --> Das ist dann über Npc_GetStateTime abfragbar.
{};

func void Npc_SetTalentSkill(var c_npc npc, var int talent, var int level)
/// \remarks Setzt das Skill-Level des Talents
{};

func void Npc_SetTalentValue(var c_npc npc, var int talent, var int value)
/// \remarks setzt talent value auf den angegebenen Wert. Wird in gothic 1 benutzt für 1h/2h/bow/crossbow/picklock/pickpocket
{};

func void Npc_SetTarget(var c_npc npc, var c_npc other)
/// \remarks Gibt dem Nsc 'npc' das interne Ziel 'other'.
/// \remarks --> Nur wenn per GetTarget auch der other "geholt" wird ist er vorhanden,
/// \remarks da hier interne Variablen, die in den Skripten nicht vorhanden sind verwendet werden
{};

func void Npc_SetTeleportPos(var c_npc npc)
/// \remarks Magie/Zauberstein Teleport Spruch : letzte Position des NSCs merken, zwecks späteren dahinbeamens.
{};

func void Npc_SetTempAttitude(var c_npc npc, var int attitude)
/// \remarks setzt die temporäre Attitüde auf einen Wert (att)
{};

func void Npc_SetToFightMode(var c_npc npc, var int weapon)
/// \remarks Setzt den NSC beim Start in den der Waffe entsprechenden Kampfmodus (Waffe wird erzeugt)
{};

func void Npc_SetToFistMode(var c_npc npc)
/// \remarks Setzt den NSC beim Start in den Faustkampfmodus (zB.Monster)
{};

func int Npc_SetTrueGuild(var c_npc npc, var int guildID)
/// \remarks Setzt die wahre Gilde des NSCs
{};

func bool Npc_StartItemReactModules(var c_npc npc, var c_npc other, var c_item item)
/// \remarks Prüfe alle ItemReact-Module von "npc" auf Gegenstand "item" von Geber "other" und starte passende Reaction-Funktion
/// \remarks liefert True beim finden eines Moduls
{};

func void Npc_StopAni(var c_npc npc, var string animationName)
{};

func bool Npc_WasInState(var c_npc npc, var func state)
/// \remarks Abfrage auf den vorherigen Zustand einer Spielfigur True/False
{};

func bool Npc_WasPlayerInMyRoom(var c_npc npc)
/// \remarks gibt TRUE zurück, wenn sich SC vor dem letzten Raumwechsel im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
/// \remarks Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
{};

func void Perc_SetRange(var int percID, var int range)
/// \remarks Setze Reichweite fuer eine passive Wahrnehmung int cm
{};

func bool PlayVideo(var string bikName)
/// \remarks Gothic 2 only function
/// \remarks Spielt eine Videodatei ab.
/// \remarks
/// \remarks      bikName         - Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
/// \remarks      [result]        - Boolean ob erfolgreich abgespielt
{};

func bool PlayVideoEx(var string bikName, var bool screenBlend, var bool exitSession)
/// \remarks Gothic 2 only function
/// \remarks Spielt eine Videodatei mit erweiterten Optionen ab.
/// \remarks
/// \remarks      bikName         - Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
/// \remarks      ScreenBlend     - Boolean ob Effekt 'BLACK_SCREEN' danach abgespielt wird (nicht bei ExitSession)
/// \remarks      ExitSession     - Boolean ob nach dem Video die Session beendet wird
/// \remarks      [result]        - Boolean ob erfolgreich abgespielt, bei ExitSession immer erfolgreich
{};

func void Print(var string text)
/// \remarks Ausgabebefehl, der Text ins Game schreibt (wie OutputunitTexte)
{};

func void PrintDebug(var string text)
/// \remarks Printausgabe, die nur bei eingeschaltetem Debugmodus (Alt+D) im Spy/logfile ausgegeben wird
{};

func void PrintDebugCh(var int ch, var string text)
/// \remarks gibt nur den Text eines bestimmten Channels wieder, unabhängig von der Instanz
{};

func void PrintDebugInst(var string text)
/// \remarks gibt nur den Debugtext der eingeschalteten (SetDebugFocus --> siehe Intranetpage Erweitertes Skript-Debugging) Instanz aus
{};

func void PrintDebugInstCh(var int ch, var string text)
/// \remarks gibt nur in einem bestimmten channel liegende Debuginfos einer Instanz aus
{};

func int PrintDialog(var int dialogNr, var string text, var int x, var int y, var string font, var int timeSec)
{};

func void PrintMulti(var string text1, var string text2, var string text3, var string text4, var string text5)
/// \remarks Printbefehl, der aus den angegebenen Strings einen Auswählt und auf den Bildschirm schreibt
{};

func bool PrintScreen(var string text, var int posX, var int posY, var string font, var int timeSec)
/// \remarks Gibt den Text 'text' auf dem Bildschrim aus und benutzt dabei den Font 'font'.
/// \remarks Die Position ist für jede Koordinate eine Zahl zwischen 0 und 99 und gibt die prozentuale Position an.
/// \remarks Der Ursprung befindet sich oben links (also 0% X und 0% Y)
/// \remarks Liefert False, wenn font nicht gefunden wurde, sonst True
{};

func void Rtn_Exchange(var string oldRoutine, var string newRoutine)
/// \remarks Tausche aktuellen Tagesablauf des NSC "self" gegen angegebenen aus
/// \remarks (Name wird automatisch mit "RTN_" am Anfang und NSC-Skript-ID am Ende ergänzt)
{};

func void SetPercentDone(var int percentDone)
/// \remarks Gothic 2 only function
/// \remarks Setz die Fortschrittsanzeige im Ladebalken.
/// \remarks
/// \remarks      PercentDone     - Aktueller Fortschritt des Ladevorgangs in Prozent (0-100)
{};

func int Snd_GetDistToSource(var c_npc npc)
/// \remarks Liefert Entfernung ( in cm ! ) zum letzten logischen Sound
{};

func bool Snd_IsSourceItem(var c_npc npc)
/// \remarks Check, ob Quelle des letzten Sound Item war (Return >0) und setzt "item" auf diesen Gegenstand
{};

func bool Snd_IsSourceNpc(var c_npc npc)
/// \remarks Check, ob Quelle des letzten Sound NPC war (Return >0) und setzt "other" auf diesen NPC
{};

func void Snd_Play(var string sfxName)
/// \remarks spielt einen Sound ab
{};

func void Snd_Play3D(var c_npc npc, var string sfxName)
/// \remarks spielt einen 3D-Sound ab.
{};

func void TA(var c_npc npc, var int start_h, var int stop_h, var func state, var string waypoint)
/// \remarks Mit _(Zustandsname) wird ein neuer Tagesablauf generiert, siehe TA.d
{};

func void TA_BeginOverlay(var c_npc npc)
/// \remarks Melde einen Overlay-Tagesablauf an
{};

func void TA_CS(var c_npc npc, var string csName, var string roleName)
/// \remarks Cutscene an den zuletzt angegebenen Tagesablaufpunkt hängen
/// \remarks csName:      Name der Cutscene ( der Name des "CS" - Files )
/// \remarks roleName:    Die Rolle die der NSC dabei übernehmen soll.
{};

func void TA_EndOverlay(var c_npc npc)
/// \remarks Beende einen Overlay-Tagesablauf
{};

func void TA_Min(var c_npc npc, var int start_h, var int start_m, var int stop_h, var int stop_m, var func state, var string waypoint)
/// \remarks Tagesablaufpunkt minutengenau angeben
{};

func void TA_RemoveOverlay(var c_npc npc)
/// \remarks Entferne aktiven TA-Overlay
{};

func void Tal_Configure(var int talent, var int value)
/// \remarks wird nicht benutzt?
{};

func void Update_ChoiceBox(var string s0)
{};

func void Wld_AssignRoomToGuild(var string room, var int guild)
/// \remarks Ordnet den Raum: 'room' der Gilde 'guild' zu
{};

func void Wld_AssignRoomToNpc(var string room, var c_npc roomOwner)
/// \remarks Ordnet den Raum: 'room' dem speziellen Nsc 'roomOwner' zu
{};

func bool Wld_DetectItem(var c_npc npc, var int flags)
/// \remarks liefert eins zurück, wenn ein Item mit dem Entsprechende Flag (z.B.ITEM_KAT_FOOD )gefunden wurde
/// \remarks Globale Variable 'item' wird mit dem gefundenen Gegenstand initialisiert
{};

func bool Wld_DetectNpc(var c_npc npc, var c_npc_id npcInstance, var func aiState, var int guild)
/// \remarks Diese Methode initilisiert die globale Skriptvariable "other" mit einem NSC, den "npc" beim letzten Warnehmungscheck wargenommen hat.
/// \remarks npcInstance  = Name der zu suchenden Instanz                                 ( "-1" angeben, wenn Instanzname unwichtig )
/// \remarks guild        = Der zu suchende NSC muss Mitglied dieser Gilde sein           ( "-1" angeben, wenn Gilde unwichtig )
/// \remarks aiState      = Der AI-Zustandsname, in dem sich der NSC befinden soll        ( NOFUNC angeben, wenn AI-State unwichtig )
/// \remarks Wenn die Methode einen entsprechenden NSC gefunden hat, liefert diese "1" und 'other' ist initialisiert
/// \remarks ansonsten wird "0" geliefert und "other" wird nicht verändert.
{};

func bool Wld_DetectNpcEx(var c_npc npc, var c_npc_id npcInstance, var func aiState, var int guild, var bool detectPlayer)
/// \remarks Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.
{};

func int Wld_DetectNpcExAtt(var c_npc npc, var c_npc_id npcInstance, var func aiState, var int guild, var bool detectPlayer, var int Attitude)
{};

func bool Wld_DetectPlayer(var c_npc npc)
/// \remarks liefert eins zurück, wenn der Player in der Nähe ist
{};

func void Wld_ExchangeGuildAttitudes(var string tableName)
/// \remarks Tausche Gilden-AttitudenTabelle aus.
{};

func int Wld_GetDay()
/// \remarks Liefert den aktuellen Tag zurück (Zähler), hierbei ist der StartTag (Gamestart) = 0.
{};

func int Wld_GetFormerPlayerPortalGuild()
/// \remarks liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
/// \remarks Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
/// \remarks - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
/// \remarks - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert
{};

func c_npc Wld_GetFormerPlayerPortalOwner()
/// \remarks liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
/// \remarks Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
/// \remarks - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
/// \remarks - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'
{};

func int Wld_GetGuildAttitude(var int guild1, var int guild2)
/// \remarks Ermittle Gildenattitude
{};

func int Wld_GetMobState(var c_npc npc, var string schemeName)
/// \remarks Liefert den State zum nächsten Mob mit 'schemeName' zurück bzw. '-1' wenn kein solches Mob gefunden wurde
{};

func int Wld_GetPlayerPortalGuild()
/// \remarks liefert Gilde des aktiven Raums, in dem sich der SC gerade befindet, zurück
/// \remarks - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
/// \remarks - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert
{};

func c_npc Wld_GetPlayerPortalOwner()
/// \remarks liefert den NSC des aktiven Raums, in dem sich der SC gerade befindet, zurück
/// \remarks - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
/// \remarks - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'
{};

func void Wld_InsertItem(var c_item_id itemInstance, var string fpOrWPName)
/// \remarks Füge Item in Welt ein entweder an einem WP oder einem FP
/// \remarks Vorsicht, funktioniert nicht, Items werden immer im Mittelpunkt der Welt inserted
{};

func void Wld_InsertNpc(var c_npc_id npcInstance, var string fpOrWPName)
/// \remarks Füge NSC in Welt ein. Wobei Spawnpoint entweder ein WP oder ein FP sein darf.
{};

func void Wld_InsertNpcAndRespawn(var c_npc_id npcInstance, var string fpOrWPName, var int spawnDelaySec)
/// \remarks Füge NSC in Welt ein. Wobei Spawnpoint entweder ein WP oder ein FP sein darf. Stirbt dieser NSC wird
/// \remarks nach "spawnDelay"-Sekunden ein neuer NSC am Spawnpoint erzeugt.
{};

func void Wld_InsertObject(var string objName, var string fpOrWPName)
{};

func bool Wld_IsFPAvailable(var c_npc npc, var string freepoint)
/// \remarks Sucht einen Freepoint im Umkreis von 20m vom NSC und liefert TRUE falls vorhanden und frei ('npc' zählt als Blockierer nicht!) und sichtbar
{};

func bool Wld_IsMobAvailable(var c_npc npc, var string schemeName)
/// \remarks Sucht sich ein Mobsi im Umkreis von 10m und liefert TRUE falls gefunden. MOB wird nur zurückgeliefert, wenn es nicht besetzt ist.
{};

func bool Wld_IsNextFPAvailable(var c_npc npc, var string freepoint)
/// \remarks wie Wld_IsFPAvailable(), aber es wird immer der nahegelegenste genommen und 'npc' zählt als Blockierer!
{};

func bool Wld_IsRaining()
{};

func bool Wld_IsTime(var int hour1, var int min1, var int hour2, var int min2)
/// \remarks Liefert zurück ob die aktuelle Weltzeit zwischen den beiden angegebenen Zeiten liegt (von - bis)
{};

func void Wld_PlayEffect(var string effectVFXName, var instance originvob, var instance targetvob, var int effectLevel, var int damage, var int damageType, var bool bIsProjectile)
/// \remarks effectVFXName: Name der VisualFX-Instanz
/// \remarks originvob: Ursprung/Verursacher (muss existieren!) NPC, item, freepoint, ...
/// \remarks targetvob: Ziel fuer Effekt + Schaden
/// \remarks effectLevel: Bei Angabe von effectLevel wird nach einer VisualFX-Instanz mit dem Namen _L gesucht und verwendet falls sie
/// \remarks gefunden wurde. (es muss trotzdem eine VisualFX-Instanz mit dem Namen definiert worden sein!)
/// \remarks damage: Hoehe des Schadens
/// \remarks damageType: Schadensart (DAM_Xxx)
/// \remarks bIsProjectile: Effekt ist Projektil
{};

func bool Wld_RemoveItem(var c_item item)
/// \remarks Hiermit wird das angegebene Item aus der Welt entfernt und gelöscht, gibt TRUE zurück wenn erfolgreich
{};

func void Wld_RemoveNpc(var c_npc_id npcInstance)
{};

func void Wld_SendTrigger(var string vobName)
/// \remarks Sendet eine Trigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.
{};

func void Wld_SendUntrigger(var string vobName)
/// \remarks Sendet eine UnTrigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.
{};

func void Wld_SetGuildAttitude(var int guild1, var int attitude, var int guild2)
/// \remarks Setze Gildenattitude neu
{};

func void Wld_SetMobRoutine(var int hour, var int min, var string objName, var int state)
/// \remarks _Alle_ Mobs mit diesem _Schemanamen_ werden getriggert.
{};

func void Wld_SetObjectRoutine(var int hour, var int min, var string objName, var int state)
/// \remarks _Ein_ Objekt mit diesem _Vobnamen_ wird getriggert
{};

func void Wld_SetTime(var int hour, var int min)
/// \remarks Setzt die Uhrzeit auf hour:min. hour kann größer als 23 sein, um zum nächsten Tag zu springen.
{};

func void Wld_SpawnNpcRange(var c_npc summoner, var c_npc_id npcInstance, var int count, var float radius)
/// \remarks NPC summoner beschwört die Diener npcInstance der Anzahl count. Die Diener erscheinen in einer Entfernung von radius (in cm).
{};

func void Wld_StopEffect(var string vfxName)
{};

