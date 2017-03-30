func void AI_AimAt(C_NPC attacker, C_NPC target);
// NPC zielt mit Fernkampfwaffe auf Target-NPC

func void AI_AlignToFP(C_NPC npc);
// richtet den Nsc am Freepoint aus (im Spacer gesetzte Pfeilrichtung)

func void AI_AlignToWP(C_NPC npc);
// richtet den Nsc am Waypoint aus (im Spacer gesetzte Pfeilrichtung)

func void AI_Ask(C_NPC npc, func anserYes, func answerNo);
// die angegeben Instanz (npc), gibt eine Frage aus und verweist auf die selbst zu definierenden Funktionen,
// die für die Fälle Spieler sagt ja (Daumen nach oben) und Spieler sagt nein (Daumen unten) vorhanden sein müssen

func void AI_AskText(C_NPC npc, func funcYes, func funcNo, string strYes, string strNo);
// wie AI_Ask, nur das außer den Funktionen auch noch Strings für die Antworten Ja/Nein mit angegeben werden können

func void AI_Attack(C_NPC npc);
// NPC npc startet Kampf AI (sollte in der ZS_Attack-Loop stehen)
// Es wird das interne Ziel verwendet, das mit Npc_SetTarget() oder Npc_GetNextTarget() gesetzt wurde.

func void AI_CanSeeNpc(C_NPC npc, C_NPC other, func see);

func void AI_CombatReactToDamage(C_NPC npc);

func void AI_ContinueRoutine(C_NPC npc);
// Setze Tagesablauf fort
// Enthält Standup

func void AI_Defend(C_NPC npc);
// Der Befehl ist als Overlay-Message implementiert. Das heisst, dass er neben anderen Nachrichten
// aktiv bleibt. Er wird erst beendet, wenn der NPC eine Parade (ausgeloest durch die Attacke eines
// anderen NPCs) durchgefuert hat.

func void AI_Dodge(C_NPC npc);
// Der Nsc weicht ein Stück nach hinten aus

func void AI_DrawWeapon(C_NPC npc);
// Equipte Waffe wird gezogen

func void AI_DropItem(C_NPC npc, C_Item_ID itemID);
// Item wird auf den Boden fallen gelassen

func void AI_DropMob(C_NPC npc);

func void AI_EquipArmor(C_NPC owner, C_Item_ID armorFromOwnersInventory);
// Ziehe die angebene Rüstung dem NSC "owner" an, diese muss sich in seinem Inventory befinden.

func void AI_EquipBestArmor(C_NPC npc);
// Wunder, Wunder hier wird die beste im Inventory vorhandene Rüstung angezogen

func void AI_EquipBestMeleeWeapon(C_NPC npc);
// sucht im Inventory nach der besten Nahkampfwaffe und hängt sie an den Gürtel

func void AI_EquipBestRangedWeapon(C_NPC npc);
// sucht im Inventory nach der besten Fernkampfwaffe und ploppt sie auf den Rücken der Instanz

func void AI_FinishingMove(C_NPC npc, C_NPC other);
// Führt den logischen Finishing Move inklusive Anis aus den Skripten heraus aus

func void AI_Flee(C_NPC npc);
// Der Befehl muss, wie AI_Attack(), in der ZS-Loop regelmaessig aufgerufen werden und setzt voraus, dass
// vorher mit Npc_SetTarget( self, <var C_NPC enemy> ) ein Gegner gesetzt wurde, vor dem der Npc fliehen soll.

func void AI_GotoFP(C_NPC npc, string freepointName);
// Sucht sich einen Freepoint im Umkreis von 20m vom NSC, bewegt sich dorthin und richtet sich entsprechend aus.
// Suchkriterium wie bei Wld_IsFPAvailable()

func void AI_GotoItem(C_NPC npc, C_ITEM item);
// "npc" geht zu "item"

func void AI_GotoNextFP(C_NPC npc, string freepointName);
// wie AI_GotoFP() allerdings Suchkriterium wie bei Wld_IsNextFPAvailable()

func void AI_GotoNpc(C_NPC npc, C_NPC other);
// "npc" geht zu "other"

func void AI_GotoSound(C_NPC npc);
// Npc npc läuft zum Sound

func void AI_GotoWP(C_NPC npc, string waypointName);
// Npc-Instanz npc läuft zum namentlich angegeben Waypoint

func void AI_LookAt(C_NPC npc, string wpOrVob);
// Schaue auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)

func void AI_LookAtNpc(C_NPC npc, C_NPC other);
// Schaue zu einem NSC

func void AI_LookForItem(C_NPC npc, int instanceItem);
// gibt die Möglichkeit nach bestimmten Items zu suchen (z.B.:Das goldene Schwert der Zerstörung, wenn vorhanden)

func void AI_Output(C_NPC npc, C_NPC target, string wavName);
// Cutscene mit entsprechender Id wird abgenudelt

func void AI_OutputSVM(C_NPC npc, C_NPC target, string svmName);
// Outputbefehl um Svms abzuspielen

func void AI_OutputSVM_Overlay(C_NPC npc, C_NPC target, string svmName);
// wie AI_OutputSVM, wartet jedoch NICHT mit der Ausführung des nächsten AI_...-Befehls, bis
// das SVM zuende gespielt wird. (Für Kommentare kurz vor und während dem Kampf!)

func void AI_PlayAni(C_NPC npc, string animationName);
// Npc-Instanz npc spielt die angegebene Animation ab

func void AI_PlayAniBS(C_NPC npc, string animationName, int bodystate);
// Beim Abspielen einer Ani mit diesem Befehl kann ein Bodystate angemeldet werden

func void AI_PlayCutscene(C_NPC npc, string csName);
// Eine Cutscene aus den Scripten heraus starten

func void AI_PlayFX(C_NPC npc, C_NPC other, string videoName);

func void AI_PointAt(C_NPC name, string wpOrVob);
// Zeige auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)

func void AI_PointAtNpc(C_NPC npc, C_NPC other);
// Zeige auf einen NSC

func int AI_PrintScreen(string text, int x, int y, string font, int timeSec);

func void AI_ProcessInfos(C_NPC npc);

func void AI_Quicklook(C_NPC npc, C_NPC other);
// NSC kurz ( 2 sec) anschauen ( nur Kopf bewegt sich )

func void AI_ReadyMeleeWeapon(C_NPC npc);
// Ziehe equippte Nahkampfwaffe

func void AI_ReadyRangedWeapon(C_NPC npc);
// Ziehe equippte Fernkampfwaffe

func void AI_ReadySpell(C_NPC npc, int spellID, int investMana);
// Lasse zauberspruch auf Hand erscheinen.

func void AI_RemoveWeapon(C_NPC npc);
// Gezogene Waffe wird weggesteckt

func void AI_SetNpcsToState(C_NPC npc, func aiStateFunc, int radius);
// Setze alle NSCs im Umkreis von x cm in entsprechenden AI-Zustand
// VORSICHT: Diese Funktion ist Buggy, wird aber derzeit auch NICHT verwendet!
// -> FINGER WEG!!! (SN)

func void AI_SetWalkmode(C_NPC npc, int moveMode);
// gibt an mit welchem Walkmode Run etc der Character durch das Level läuft
// NPC_RUN               : Rennen
// NPC_WALK              : Gehen
// NPC_SNEAK             : Schleichen
// NPC_RUN_WEAPON        : Rennen mit gezogener Waffe
// NPC_WALK_WEAPON       : Gehen mit gezogener Waffe
// NPC_SNEAK_WEAPON      : Schleichen mit gezogener Waffe

func void AI_ShootAt(C_NPC attacker, C_NPC target);
// NPC feuert mit Fernkampfwaffe auf Target-NPC

func void AI_Snd_Play(C_NPC npc, string snd);

func void AI_Snd_Play3D(C_NPC npc, C_NPC other, string snd);

func void AI_StandUp(C_NPC npc);
// - Ist der Nsc in einem Animatinsstate, wird die passende Rücktransition abgespielt.
// - Benutzt der NSC gerade ein MOBSI, poppt er ins stehen.

func void AI_StandUpQuick(C_NPC npc);
// Wie AI_StandUp(), jedoch werden keine Rücktransitionen abgespielt, sondern auch dort wird
// sofort in den Grundzustand "gepoppt". Wichtig für sehr eilige Situationen!

func void AI_StartState(C_NPC npc, func state, bool waitForCurrentStateEnd, string waypointName);
// Versetzt den Nsc aus den Skripten heraus in den entsprechenden ZS (what),
// stateBehaviour sagt : "0"-aktuellen Zustand abbrechen "1"-aktuellen Zustand erst ordnungsgemäß beenden (End-Funktion aufrufen) ).

func void AI_StopAim(C_NPC attacker);
// NPC beendet vorher gestartetes Zielen mit Fernkampfwaffe.

func void AI_StopFX(C_NPC npc, string videoName);

func void AI_StopLookAt(C_NPC npc);
// wieder geradeaus schauen

func void AI_StopPointAt(C_NPC npc);
// nicht mehr auf etwas zeigen

func void AI_StopProcessInfos(C_NPC npc);
// Der DialogModus wird beendet (Multiple Choice-Dialog)

func void AI_TakeItem(C_NPC npc, C_ITEM item);
// der Npc nimmt mit dieser Funktion die globale Item-Instanz auf

func void AI_TakeMob(C_NPC npc, string mobName);

func void AI_Teleport(C_NPC npc, string waypointName);
// teleportiert den NSC zur angegebenene Location

func void AI_TurnAway(C_NPC turner, C_NPC npc);
// Der NSC turner dreht dem NSC npc den Rücken zu.

func void AI_TurnToNpc(C_NPC turner, C_NPC npc);
// dreht npc turner zu NSC npc

func void AI_TurnToSound(C_NPC npc);
// Charakter dreht sich zur Geräuschquelle

func void AI_UnequipArmor(C_NPC npc);
// Unequippe aktuelle Rüstung

func void AI_UnequipWeapons(C_NPC npc);
// Unequippe alle Waffen

func void AI_UnreadySpell(C_NPC npc);
// lasse zauberspruch aus Hand verschwinden

func void AI_UseItem(C_NPC npc, C_Item_ID itemID);
// Item bis zum Ende benutzen

func void AI_UseItemToState(C_NPC npc, C_Item_ID itemID, int state);
// Item benutzen bis zum angegebenen State

func int AI_UseMob(C_NPC npc, string schemeName, int targetState);
// Benutze Mob mit angegebenen Schema-Namen bis zum Zustand "targetState". Wird diese Funktion aufgerufen
// und der angegebene 'targetState' ist bereits vorhanden, läuft der NSC zwar trotzdem zum MOB, tut dann aber nichts

func void AI_Wait(C_NPC npc, float sec);
// Character wird für sec Sekunden in einen Wait-Zustand versetzt,
// d.h. er tut nichts, aber Treffer etc.(passive Wahrnehmung) werden registriert

func void AI_WaitForQuestion(C_NPC npc, func scriptFunc);
// NSC wartet 20 Sekunden, wird er in dieser Zeit vom Spieler angesprochen, wird die angegebene SkriptFunktion ausgeführt.

func void AI_WaitMS(C_NPC npc, int milliSeconds);

func void AI_WaitTillEnd(C_NPC npc, C_NPC other);
// 'npc' wartet bis 'other' seinen laufenden AI-Befehl zu Ende gespielt hat (funzt allerdings nicht bei AI-Overlays!)

func void AI_WhirlAround(C_NPC npc, C_NPC other);
// schnelle Drehung zu other

func void AI_WhirlAroundToSource(C_NPC npc);

func void Apply_Options_Audio();

func void Apply_Options_Controls();

func void Apply_Options_Game();

func void Apply_Options_Performance();

func void Apply_Options_Video();

func string ConcatStrings(string s1, string s2);
// Erzeugt eine Kopie von dem ersten angegebenen String, hängt den zweiten an diesen an und gibt den neuen String zurück

func void CreateInvItem(C_NPC npc, C_Item_ID itemID);
// gibt der angegebenen Instanz npc ein Item ins Inventory

func void CreateInvItems(C_NPC npc, C_Item_ID itemID, int amount);
// wie vorherige Funktion, nur das Multi-Items (z.B.: 100 Pfeile) ins Inventory kreiert werden

func int Doc_Create();
// G1: erstellet ein Dokument und liefert ein Handle zurück, daß für alle weiteren Doc_...-Befehle nötig ist
// Erzeugt ein Dokument.
//
//      [result]        - Handle auf das Dokument (-1 = konnte nicht erzeugt werden)

func int Doc_CreateMap();
// Gothic 2 only function
// Erzeugt ein Dokument (Karte).
//
//      [result]        - Handle auf das Dokument (-1 = konnte nicht erzeugt werden)

func void Doc_Font(string fontName);
// Gothic 1 only function
// Benutzt den angegebenen Font für nachfolgende "Doc_Print"s.

func void Doc_MapCoordinates(string levelName, float gameX1, float gameY1, float pixelX1, float pixelY1, float gameX2, float gameY2, float pixelX2, float pixelY2);
// Gothic 1 only function
// Karte initialisieren : levelName = Name des Levels (.ZEN) aus dem die Karte einen Ausschnitt zeigt
// gamex1,gamey1 : Koordinaten im Spiel (linke obere Ecke der Karte)
// pixelx1,pixely1 : PixelKoordinaten linke obere Ecke // der Rest : alles nochmal für die rechte untere Ecke

func void Doc_Open(string texture);
// Gothic 1 only function
// Öffnet ein Dokument mit dem Bild "texture" als Hintergrund

func void Doc_Print(string text);
// Gothic 1 only function
// Angegebener Text wird in das aktuelle (vorher mit Doc_Open geöffnete) Dokument geschrieben

func void Doc_PrintLine(int document, int page, string text);
// G1: Gibt Text für die Seite 'page' an. Der Text wird NICHT umgebrochen
// Setzt den Text einer Seite. Der Text wird _nicht_ umgebrochen.
//
//      Document        - Handle auf das Dokument
//      Page            - Index der Seite (beginnt mit 0)
//      Text            - Textzeile

func void Doc_PrintLines(int document, int page, string text);
// G1: Gibt Text für die Seite 'page' an. Der Text WIRD umgebrochen, falls er nicht auf die in Doc_SetPage() angegebene Grafik passt.
// Setzt den Text einer Seite. Der Text wird umgebrochen falls notwendig.
//
//      Document        - Handle auf das Dokument
//      Page            - Index der Seite (beginnt mit 0)
//      Text            - Textzeilen

func void Doc_SetFont(int document, int page, string font);
// G1: setzt den Font für die angegebene Seite 'page' oder für das ganze Dokument ('page':-1)
// G1: Fontname z.B. "Scrolls.tga"
// Setzt den Font einer/aller Seite(n).
//
//      Document        - Handle auf das Dokument
//      Page            - Index der Seite (beginnt mit 0, -1 = alle)
//      Font            - Dateiname der Font-Textur (ohne Pfad - z.B. 'Font_Default.tga')

func void Doc_SetLevel(int document, string level);
// Gothic 2 only function
// Setzt das Level-ZEN zur Anzeige der Spieler-Position (Karten).
//
// (als Grenze gilt die BBox des statischen Level-Meshes)
//      Document        - Handle auf das Dokument
//      Level           - Dateiname des Level-ZENs (mit Pfad ab WORLDS - z.B. 'NewWorld\NewWorld.zen')

func void Doc_SetLevelCoords(int document, int left, int top, int right, int bottom);
// Gothic 2 only function
// Setzt die Grenzen des Level-Bereichs, der durch die Textur abgedeckt wird (Karten).
//
// (als Grenzen werden _absolute_ Level-Koordinaten angegeben - z.B.: '..., -28000, 50500, 95500, -42500')
//      Document        - Handle auf das Dokument
//      Left            - linke Grenze
//      Top                     - obere Grenze
//      Right           - rechte Grenze
//      Bottom          - untere Grenze

func void Doc_SetMargins(int document, int page, int left, int top, int right, int bottom, bool inPixels);
// G1: setzt die Ränder (gerechnet vom Rand der TGA-Datei, die in Doc_SetPage() gesetzt wird). Die Ränder sind in Pixeln angegeben.
// Setzt den jeweiligen Rand einer/aller Seite(n) (vom Rand der mit Doc_SetPage gesetzten Textur aus gesehen).
//
//      Document        - Handle auf das Dokument
//      Page            - Index der Seite (beginnt mit 0, -1 = alle)
//      Left            - linker Rand
//      Top             - oberer Rand
//      Right           - rechter Rand
//      Bottom          - unterer Rand
//      Pixel           - Flag ob Angaben in Pixeln (Bildschirm) oder virtuellen Koordinaten (0-8192)

func void Doc_SetPage(int document, int page, string texture, int scale);
// G1: setzt für die Seite 'page' die Grafik-Datei 'pageimage'. Diese muß im TGA-Format vorliegen und
// G1: z.B. "aufgeschlagenesBuch_links.tga"  heißen
// G1: wird bei 'page' -1 übergeben, so bezieht sich der Befehl auf ALLE Seiten des Dokuments
// Setzt die Hintergrund-Textur der Seite eines Dokuments.
//
//      Document        - Handle auf das Dokument
//      Page            - Index der Seite (beginnt mit 0, -1 = alle)
//      Scale           - Flag ob Textur gestreckt werden soll

func void Doc_SetPages(int document, int count);
// G1: setzt die Anzahl der Seiten dieses Dokuments. Bei mehr als zwei Seiten wird versucht ALLE
// G1: Seiten auf den Bildschirm zu bekommen
// Setzt die Anzahl der Seiten eines Dokuments.
//
//      Document        - Handle auf das Dokument
//      Count           - Anzahl der Seiten

func void Doc_Show(int document);
// G1: nachdem das Dokument mit den obenstehenden Befehlen befüllt und formatiert wurde, wird es hiermit nun auf den Bildschirm geschrieben!
// Zeigt das Dokument auf dem Bildschirm an.
//
//      Document        - Handle auf das Dokument

func void EquipItem(C_NPC npc, C_Item_ID itemID);
// gibt dem Nsc direkt das angegebene Item an den Gürtel

func void ExitGame();
// Gothic 2 only function
// Beendet Gothic.

func void ExitSession();
// Gothic 2 only function
// Beendet das laufende Spiel.

func int FloatToInt(float f);
// !!! konvertiert einen float in einen int, hier wäre es noch ganz schön zu wissen, wie mit dem float umgesprungen wird --> schneiden oder runden ???

func string FloatToString(float f);

func bool Game_InitEnglish();
// Gothic 2 only function
// Setzt die interne Engine-Variablen auf English.
//
//      [result]        - Zeigt an, ob die Sprache erfolgreich geladen wurde.

func bool Game_InitGerman();
// Gothic 2 only function
// Setzt die interne Engine-Variablen auf Deutsch.
//
//      [result]        - Zeigt an, ob die Sprache erfolgreich geladen wurde.

func bool Hlp_CutscenePlayed(string csName);
// Abfrage, ob Cutscene schon gespielt wurde (0 = Nein / 1 = Ja)

func int Hlp_GetInstanceID(C_Instance itemOrNPC);
// liefert die interne ID ( nicht Var aus der Instanz) zurück, um zwei items oder npcs miteinander vergleichen zu können ( integer Vergleich)
// itemOrNPC kann vom Typ C_ITEM oder C_NPC sein

func C_NPC Hlp_GetNpc(int npcInstance);
// Ermittle einen NSC über den Instanznamen. Dieser kann einer Variablen zugewiesen werden und ist somit gezielt verfügbar

func bool Hlp_IsItem(C_ITEM item, int instanceName);
// Prüft, ob der Gegenstand 'item' den Instanznamen 'instanceName' trägt. Will man z.B. testen,
// ob die globale Variable item Pfeile sind (ItMuArrow) ruft man Hlp_IsItem(item,ItMuArrow) auf.
// Gribt bei Gleichheit TRUE, sonst FALSE zurück.

func bool Hlp_IsValidItem(C_ITEM item);
// Prüfe ob Item-Instanz gültig und initialisiert ist True/False

func bool Hlp_IsValidNpc(C_NPC npc);
// Prüfe ob NSC-Instanz gültig und initialisiert ist. True/False

func int Hlp_Random(int excludedUpperBound);
// erzeugt einen Zufallswert (wobei maxNotIncluded Obergrenze : [0..n0-1] ) Rückgabewert integer

func bool Hlp_StrCmp(string s1, string s2);
// Hilfsfunktion um zwei Strings miteinander zu vergleichen liefert bei Gleichheit eins zurück

func void Info_AddChoice(C_INFO_ID infoID, string text, func f);
// Fügt der Dialogoption info eine neue Subdialog-Option hinzu (Reihenfolge: oben einfügen, nicht unten dranhängen)

func void Info_ClearChoices(C_INFO_ID infoID);
// Löscht alle Subdialogoptionen, die zur Dialogoption info gehören

func bool InfoManager_HasFinished();

func void IntroduceChapter(string titel, string untertitel, string texture, string wavName, int waitTimeMS);
// Gothic 2 only function
// Zeigt den Kapitelwechsel-Bildschirm an.
//
//      Titel           - Text des Titels
//      Untertitel      - text des Untertitels
//      Texture         - Dateiname der Hintergrundtextur
//      Sound           - Dateiname des abgespielten Sounds
//      waitTimeMS      - Anzahl in Millisekunden, die der Bildschirm angezeigt wird

func float IntToFloat(int x);
// !!! wie float to int, nur daß ein integer in float umgewandelt wird ???

func string IntToString(int x);
// !!!Funktion überträgt eine Integer variable in eine Stringvariable (Rückgabewert : string)???

func void Log_AddEntry(string topic, string entry);
//      Der Befehl fügt dem Topics topic ein neues Entry namens entry hinzu. Wie auch bei der Erzeugung von Topics dient hier der
//      Name sowohl zur Identifikation als auch zur Anzeige und muss somit eindeutig sein. Doppelte Einträge werden auch hier ignoriert.
//
//      Parameter
//      topic           Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
//      entry           Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Eintrags verwendet wird.

func void Log_CreateTopic(string topic, int section);
//      Der Befehl fügt unter der Sektion 'section' ein neues Topic mit Namen 'topic' ein.
//      Sollte bereits ein Topic dieses Names in irgendeiner Sektion existieren, wird das Topic nicht hinzugefügt.
//
//      Parameter:
//      - topic         Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Topics verwendet wird.
//      - section       Eine, in der Constants.d definierte Konstante, die besagt, in welcher Sektion das Topic angezeigt werden soll.
//                      Die Konstante kann folgende Werte annehmen:
//                          - LOG_MISSION
//                          - LOG_NOTE

func void Log_SetTopicStatus(string topic, int status);
//      Der Befehl ändert den Status des Topics 'name' in 'status'.
//      Er sollte nur für Topics verwendet werden, die für die Sektion LOG_MISSION erzeugt wurden.
//
//      Parameter:
//      - topic         Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
//      - status        Eine, in der Constants.d definierte Konstante, die besagt, unter welchem Status die Mission dargestellt werden soll.
//                      Folgende Werte sind möglich:
//                              - LOG_RUNNING
//                              - LOG_SUCCESS
//                              - LOG_FAILED
//                              - LOG_OBSOLETE

func void Mdl_ApplyOverlayMds(C_NPC npc, string mdsName);
// mit diesem Befehl werden Animationen auf einem höheren Layer gestartet
// (z.B. hat der Zombie ein Overlay über die normalen Human-Animationen)

func void Mdl_ApplyOverlayMDSTimed(C_NPC npc, string mdsName, int timeTicks);
// Overlay-MDS wird gestartet und automatisch nach der angegebenen Zeit abgeschaltet

func void Mdl_ApplyRandomAni(C_NPC npc, string animationName, string alias);
// Fügt Zufallsanimationen (am Kopf kratzen, an den Sack packen etc (s2)) für bestimmte Animationszustände (s1) ein

func void Mdl_ApplyRandomAniFreq(C_NPC npc, string animationName, float freq);
// hiermit kann die Frequenz betimmt werden, wie oft die für den Animationszustand (s1) deklarierten Randomanis abgespielt werden

func void Mdl_ApplyRandomFaceAni(C_NPC npc, string animationName, float minTime, float minTimeVar, float maxTime, float maxTimeVar, float probMin);
// Starte zufällige Gesichtsanimation
// Mdl_ApplyRandomFaceAni ( npc, animationName, minTime, minTimeVar, maxTime, maxTimeVar, probMin)
// minTime      = Minimum an Zeit nachdem Ani startet (in Sekunden)
// maxTime      = Maximum an Zeit nachdem Ani startet (in Sekunden)
// minTimeVar   = Abweichung von minTime (in Sekunden)
// maxTimeVar   = Abweichung von maxTime (in Sekunden)
// probMin      = Wahrscheinlichkeit [0..1] ob obere oder untere Grenze der Abweichung benutzt wird

func void Mdl_RemoveOverlayMDS(C_NPC npc, string mdsName);
// Entferne ein Overlay-MDS

func void Mdl_SetModelFatness(C_NPC npc, float fatness);
// Setzt Model-Z-Skalierung

func void Mdl_SetModelScale(C_NPC npc, float x, float y, float z);
// zum skalieren des Meshes (Breite,Höhe,Tiefe) 1 = 100%, also Normalgröße!

func void Mdl_SetVisual(C_NPC npc, string mdsName);

func void Mdl_SetVisualBody(C_NPC npc, string bodyMesh, int bodyTex, int skinColor, string headMMS, int headTex, int toothTex, int armor);

func void Mdl_StartFaceAni(C_NPC npc, string animationName, float intensity, float holdTime);
// Starte Gesichtsanimation // intensity 1 = 100% // holdTime -1 = forever

func void Mis_AddMissionEntry(instance n0, string s1);

func int Mis_GetStatus(int missionName);
// Liefert aktuellen Status einer Mission zurück ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc.

func bool Mis_OnTime(int missionName);
// Liefert TRUE, wenn sich Spieler noch innerhalb des Zeitlimits für diese Mission befindet

func void Mis_RemoveMission(instance n0);

func void Mis_SetStatus(int missionName, int newStatus);
// Setzt den Status einer Mission ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc. )

func void Mob_CreateItems(string mobName, int itemInstance, int amount);
// Erzeuge "amount" Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen.

func int Mob_HasItems(string mobName, int itemInstance);
// Liefere Anzahl der Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen

func bool Npc_AreWeStronger(C_NPC npc, C_NPC other);
// ermittelt den stärkeren Nsc,
// Wenn die Summe der Level aller NPCs (human, Monster), die zu mir feindlich sind und die zu <other> freundlich sind
// MEHR ALS DOPPELT SO HOCH ist (x > 2*y), wie die Summe der Level aller Leute, die zu mir freundlich sind, und die
// zu <other> feindlich sind, dann NEIN,
// sonst JA (= wir SIND stärker, nicht echt, aber ab doppelter Gegnerstärke zu fliehen ist früh genug, sonst kommen
// die Jungs aus dem Rennen nicht mehr raus
// Beachten:
// 1) Monster können feindlich zu beiden Menschen sein --> egal, zählen auf beiden Seiten
// 2) Jemand, der z.B. zu mir freundlich und zu <other> feindlich ist, wird demnach doppelt gewertet --> ok so

func bool Npc_CanSeeItem(C_NPC npc, C_ITEM item);
// Prueft ob Npc den Gegenstand sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False

func bool Npc_CanSeeNpc(C_NPC npc, C_NPC other);
// Prueft ob NPC npc NPC other sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False Blickwinkelabhängig (+,-100°)+LOS von der Hüfte aus

func bool Npc_CanSeeNpcFreeLOS(C_NPC npc, C_NPC other);
// Prüft ob NSC anderen NSC sehen kann, ohne dabei den Winkel zu berücksichtigen (nur LineOfSight-Check)

func bool Npc_CanSeeSource(C_NPC npc);
// Checkt, ob NSC die Soundquelle sehen kann True/False

func void Npc_ChangeAttribute(C_NPC npc, int attribute, int value);
// ändert den Wert des Attributs (attribute) um (value) Einheiten.

func bool Npc_CheckAvailableMission(C_NPC npc, int missionState, bool important);
// Prüfe ob zwischen dem NSC eine aktuelle Mission (AVAILABLE,RUNNING) besteht und vergleiche den Status mit den angegebenen.
// Falls dieser identisch ist, wird "1" geliefert.

func bool Npc_CheckInfo(C_NPC npc, bool important);
// Überprüft,ob der NSC gültige Infos für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").

func bool Npc_CheckOfferMission(C_NPC npc, bool important);
// Überprueft ob der NSC dem Spieler einen Auftrag anbieten kann, wenn ja, wird der Offer-Block gestartet und "1" zurückgeliefert.

func bool Npc_CheckRunningMission(C_NPC npc, bool important);
// Überprueft ob zwischen dem NSC und dem Spieler eine laufende Mission besteht und startet den entsprechenden Skriptblock.
// (Returnwert 1 : Mission aktiv)

func void Npc_ClearAIQueue(C_NPC npc);
// hiermit werden alle Befehle mit sofortiger Wirkung aus der AI_Queue rausgeworfen

func void Npc_ClearInventory(C_NPC npc);
// leert das Inventar des NPC

func void Npc_CreateSpell(C_NPC npc, int spellnr);
// NSc bekommt Zauberspruch zugewiesen kann diesen aber noch nicht gebrauchen
// (erscheint ausgegraut im Auswahlkranz)

func bool Npc_DeleteNews(C_NPC npc, int newsID);
// entfernt Nachricht bei npc, gibt 1 zurück wenn erfolgreich, sonst 0

func void Npc_ExchangeRoutine(C_NPC npc, string routineName);
// Tausche meherere Tagesabläufe aus

func int Npc_GetActiveSpell(C_NPC npc);
// liefert den Zauber zurück, der auf der Hand des NPC ist
// liefert -1 zurück, wenn kein Zauber auf der Hand ist

func int Npc_GetActiveSpellCat(C_NPC npc);
// Unterscheidet zwischen den drei Kategorien (Spell_Bad, Spell_neutral,Spell_Good) Spellkat ist Rückgabewert

func bool Npc_GetActiveSpellIsScroll(C_NPC npc);
// Gibt zurück ob der aktive Zauber auf der Hand des NPC npc eine Spruchrolle ist.
// Wird benutzt um die korrekten Manakosten des Zaubers zu ermitteln, da sich Manakosten von Spruchrollen und Runen unterscheiden.

func int Npc_GetActiveSpellLevel(C_NPC npc);
// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist

func int Npc_GetAttitude(C_NPC npc, C_NPC other);
// Gibt die Attitüde von npc zu other zurück (temp. / perm. / Gilden )

func int Npc_GetBodyState(C_NPC npc);
// Ermittle BodyState ( Liefert BS_-Konstanten )
// !!! VORSICHT !!!: Diese Funktion gibt den Bodystate mit allen eventuellen
// ver-oderten Modifiern wie "brennend". Besser 'C_BodyStateContains(self,bodystate)' benutzen

func int Npc_GetComrades(C_NPC npc);
// gibt 1 zurück, wenn der NPC npc Freunde hat, sonst - 0.

func string Npc_GetDetectedMob(C_NPC npc);
// liefert den Schemanamen des Mobsi zurück, das der Nsc entdeckt hat.
// Schemaname ist das String-Kürzel des Mobs, daß im Visualnamen vor dem ersten "_"-Zeichen steht, also z.B. "DOOR" wenn der Visualname "DOOR_OCR__135" ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
// WORKAROUND: zur Zeit werden hiermit nur Tür-MOBSIs zurückgegeben, dies soll aber wieder zurückgebaut werden

func int Npc_GetDistToItem(C_NPC npc, C_ITEM item);
// Liefert Entfernung ( in cm ! ) zwischen NSC und Gegenstand

func int Npc_GetDistToNpc(C_NPC npc, C_NPC other);
// Liefert Entfernung ( in cm ! ) zwischen den beiden NSCs

func int Npc_GetDistToPlayer(C_NPC npc);
// Liefert Entfernung ( in cm ! ) zwischen npc und Spieler

func int Npc_GetDistToWP(C_NPC npc, string waypointName);
// liefert die Entfernung vom NSC 'npc' zum angegebenen Waypoint in cm

func C_ITEM Npc_GetEquippedArmor(C_NPC npc);
// Liefert die angelegte Rüstung des NSCs.

func C_ITEM Npc_GetEquippedMeleeWeapon(C_NPC npc);
// Liefert die gegurtete Nahkampfwaffe des NSCs.

func C_ITEM Npc_GetEquippedRangedWeapon(C_NPC npc);
// Liefert die gegurtete Fernkampfwaffe des NSCs.

func int Npc_GetGuildAttitude(C_NPC npc, C_NPC other);
// Ermittelt die Gildenattitüde von zwei Nsc´s direkt im Gegensatz zu Wld_GetGuildAttitude

func int Npc_GetHeightToItem(C_NPC npc, C_ITEM item);
// Distanz in cm

func int Npc_GetHeightToNpc(C_NPC npc1, C_NPC npc2);
// Gothic 2 only function
// Liefert Höhendifferenz ( in cm ! ) zwischend den beiden NSCs

func bool Npc_GetInvItem(C_NPC npc, int itemInstance);
// Ermittle ItemInstanz aus Inventory
// befüllt globale Variable "item" mit dem gefundenen C_Item, falls das item gültig ist. Gibt TRUE zurück, wenn das item gültig ist.

func int Npc_GetInvItemBySlot(C_NPC npc, int category, int slotNr);
// Mit diesem Befehl läßt sich nachsehen, ob in einem bestimmten Slot einer bestimmten Kategorie ein item vorhanden ist
// ist das der Fall, wird dieses Item in die globale Variable item geschrieben
// gibt jetzt die Anzahl zurueck, wenn das Item stackable ist.
// Den Transfer machst Du dann per Npc_RemoveInvItems() und Npc_CreateInvItems().

func int Npc_GetLastHitSpellCat(C_NPC npc);
// Gothic 2 only function
// liefert die Category des Zaubers zurück, der den NSC zuletzt getroffen hat

func int Npc_GetLastHitSpellID(C_NPC npc);
// Gothic 2 only function
// liefert den Zauber zurück, der den NSC zuletzt getroffen hat

func C_NPC Npc_GetLookAtTarget(C_NPC npc);
// setzt irgendeine globale Variable? target?

func string Npc_GetNearestWP(C_NPC npc);
// liefert den Namen des am nächsten gelegenen Waypoints zurück

func C_NPC Npc_GetNewsOffender(C_NPC npc, int newsID);
// Ermittelt Täter der News und gibt eine Instanz der Klasse C_NPC zurück

func C_NPC Npc_GetNewsVictim(C_NPC npc, int newsID);
// Ermittle Opfer der News und gibt eine Instanz der Klasse C_NPC zurück

func C_NPC Npc_GetNewsWitness(C_NPC npc, int newsID);
// Ermittle Zeuge der News und gibt eine Instanz der Klasse C_NPC zurück

func bool Npc_GetNextTarget(C_NPC npc);
// Aktive Suche nach einem Gegner. Wird ein Gegner gefunden, so wird er als internes Ziel
// übernommen und in 'other' geschrieben, wenn kein Gegner gefunden wurde wird das
// interne Ziel gelöscht und 'other' ungültig gemacht.
// Kriterien:   1. gesetzter Gegner, der nicht tot/bewußtlos ist wird genommen...
//              2. ...dann der nächstbeste aNSC demgegenüber 'npc' HOSTILE ist (auch weder tot noch bewußtlos)
// - return:    neues Ziel gefunden     -> TRUE
//              kein Ziel gefunden      -> FALSE
// VORSICHT:    Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
//              Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
//              Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden

func string Npc_GetNextWP(C_NPC npc);
// Liefert den zweitnahesten WP vom NSC zurück

func int Npc_GetPermAttitude(C_NPC npc, C_NPC other);
// Ermittle die permanente Attitude von "npc" zu "other"

func int Npc_GetPortalGuild(C_NPC npc);

func C_NPC Npc_GetPortalOwner(C_NPC npc);

func C_ITEM Npc_GetReadiedWeapon(C_NPC npc);
// Liefert die gezogene Waffe des NSCs.

func int Npc_GetStateTime(C_NPC npc);
// Liefert Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.

func int Npc_GetTalentSkill(C_NPC npc, int talent);
// gibt talent level zurück. z.B. 5 für fünter Kreis der Magie. oder 0/1 für Taschendiebstahl gelernt/ungelernt

func int Npc_GetTalentValue(C_NPC npc, int talent);
// gibt talent value zurück. Wird in gothic 1 benutzt für 1h/2h/bow/crossbow/picklock/pickpocket

func bool Npc_GetTarget(C_NPC npc);
// Befüllt 'other' mit dem aktuellen Ziel. Das aktuelle Ziel wird intern gespeichert, wird
// durch Npc_SetTarget() bzw. Npc_GetNextTarget() gesetzt.
// - return: aktuelles Ziel gespeichert     -> TRUE
//           kein Ziel gespeichert          -> FALSE

func int Npc_GetTrueGuild(C_NPC npc);
// liefert immer!!! die wahre Gilde zurück, ignoriert also auch ohne die Regeln die Verkleidung

func bool NPC_GiveInfo(C_NPC npc, bool important);
// Überprüft, ob der NSC eine (!) gültige Info für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").

func void Npc_GiveItem(C_NPC npc, C_ITEM_ID item, C_NPC other);
// Der NSC "npc" gibt dem NSC "other" den angegebenen Gegenstand "item". Der Gegenstand wandert sofort ins Inventory des anderen.

func bool Npc_HasBodyFlag(C_NPC npc, int bodyFlag);
// Liefert 1, falls BodyFlag gesetzt ist, sonst 0

func bool Npc_HasDetectedNpc(C_NPC npc, C_NPC other);
// liefert zurück, ob der Spieler in Sinnesreichweite und entdeckt ist

func bool Npc_HasEquippedArmor(C_NPC npc);
// Armor angezogen True/False

func bool Npc_HasEquippedMeleeWeapon(C_NPC npc);
// gibt zurück, ob eine Nahkampfwaffe Equipped ist

func bool Npc_HasEquippedRangedWeapon(C_NPC npc);
// Fernkampwaffe auf dem Rücken True/False

func bool Npc_HasEquippedWeapon(C_NPC npc);
// gibt eins zurück, wenn der npc eine Waffe sichtbar mit sich rumträgt

func bool Npc_HasFightTalent(C_NPC npc, int talent);
// NICHT IMPLEMENTIERT. Spezialabfrage auf Kampftalente (z.B. 1hSword) ansonsten wie Npc_HasTalent

func int Npc_HasItems(C_NPC npc, int itemInstance);
// Liefert zurück wie viele Items der NSC vom angegebenen Typ besitzt

func int Npc_HasNews(C_NPC npc, int newsID, C_NPC offender, C_NPC victim);
// Liefert Newsnummer>0 (für weitere Referenzen) falls entsprechende News vorhanden.
// Nicht benötigte Parameter können mit "NULL" leergelassen werden

func bool Npc_HasOffered(C_NPC npc, C_NPC other, int itemInstance);
// Bietet Spieler dem NSC einen Gegenstand übers Trade-Modul an ? True/False

func bool Npc_HasRangedWeaponWithAmmo(C_NPC npc);
// Gibt TRUE zurück, wenn 'npc' irgendeine Fernkampfwaffe im Inventory oder in der Hand hat UND
// dazu auch passende Munition vorhanden ist. Sonst FALSE.

func bool Npc_HasReadiedMeleeWeapon(C_NPC npc);
// Nahkampfwaffe in der Hand

func bool Npc_HasReadiedRangedWeapon(C_NPC npc);
// Fernkampfwaffe in der Hand

func bool Npc_HasReadiedWeapon(C_NPC npc);
// gibt zurück, ob die Waffe schon in der Hand ist

func bool Npc_HasSpell(C_NPC npc, int spellID);
// Kann der NSC den angegebenen Zauberspruch benutzen ?

func bool Npc_HasTalent(C_NPC npc, int talent);
// NICHT IMPLEMENTIERT. liefert eins zurück, wenn der geprüfte Charakter das Talent talent hat

func bool Npc_IsAiming(C_NPC npc, C_NPC other);
// liefert eins zurück, wenn der zweite Character auf den ersten zielt (Fernkampfwaffen und Zauber)

func bool Npc_IsDead(C_NPC npc);
// Funktion liefert einen Wert zurück, falls Dead true ist

func int Npc_IsDetectedMobOwnedByGuild(C_NPC user, int ownerGuild);
// Liefert >0, falls "ownerGuild" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!

func int Npc_IsDetectedMobOwnedByNpc(C_NPC user, C_NPC owner);
// Liefert >0, falls "owner" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!

func int Npc_IsDrawingSpell(C_NPC npc);
// liefert spellID???

func int Npc_IsDrawingWeapon(C_NPC npc));
// ???

func bool Npc_IsInCutscene(C_NPC npc);
// liefert eins zurück, wenn der entsprechende Nsc in einer Cutscene ist

func bool Npc_IsInFightMode(C_NPC npc, int fightMode);
// liefert eins zurück, wenn der Charakter im angegebenen Fightmode (z.B. FMODE_MAGIC) ist

func bool Npc_IsInPlayersRoom(C_NPC npc);
// liefert zurück ob der npc im Raum des Spielers ist???

func bool Npc_IsInRoutine(C_NPC npc, func state);
// Check ob der angegebene Zustand der aktuelle TA des NSCs ist. True/FAlse

func bool Npc_IsInState(C_NPC npc, func state);
// Abfrage auf den aktuellen Zustand einer Spielfigur True/False

func bool Npc_IsNear(C_NPC npc, C_NPC other);
// liefert eins zurück, wenn geprüfte Instanz sich im Bereich von drei Metern zur prüfenden Instanz befindet

func bool Npc_IsNewsGossip(C_NPC npc, int newsID);
// Liefert 1, falls News "Gossip" ist

func bool Npc_IsNextTargetAvailable(C_NPC npc);
// Sucht genauso wie Npc_GetNextTarget nach einem neuen Ziel, überschreibt aber weder
// das interne Ziel, noch 'other'
// - return:    neues Ziel gefunden     -> TRUE
//              kein Ziel gefunden      -> FALSE
// VORSICHT:    Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
//              Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
//              Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden

func bool Npc_IsOnFP(C_NPC npc, string freepointName);
// Abfrage darauf, ob der Nsc auf einem Freepoint mit freepointName Teilstring steht

func bool Npc_IsPlayer(C_NPC npc);
// liefert eins zurück, wenn der geprüfte Charakter der Spieler himself ist

func bool Npc_IsPlayerInMyRoom(C_NPC npc);
// gibt TRUE zurück, wenn sich SC im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE

func bool Npc_IsVoiceActive(C_NPC npc);
// ???

func bool Npc_IsWayBlocked(C_NPC npc);
// Liefert "1", falls Weg von NSC durch Hindernis versperrt ist.

func bool Npc_KnowsInfo(C_NPC npc, int instanceInfo);
// Liefert TRUE, wenn der angegebene Spieler die Info schon einmal erhalten hat.
// VORSICHT: auch wenn eine permanente Info schon einmal dem Spieler erzählt wurde, so gibt diese Funktion trotzdem FALSE zurück!

func bool Npc_KnowsPlayer(C_NPC npc, C_NPC player);
// Kennt der NSC den Spieler? True/False

func void Npc_LearnSpell(C_NPC npc, int spell);
// Was könnte das wohl heißen ? Ich glaube damit kann man den (spell) Zauber zuweisen

func void Npc_MemoryEntry(C_NPC npc, int source, C_NPC offender, int newsID, C_NPC victim);
// mit dieser Funktion wird eine NSC bezogene News geschrieben newsID : News ID source > 0 : "gossip", ansonsten "witness",
// npc: NSC, bei dem News eingespeist werden soll, offender: Täter, victim : Opfer

func void Npc_MemoryEntryGuild(C_NPC npc, int source, C_NPC offender, int newsID, C_NPC victimGuild);
// wie MemoryEntry nur, das die Memory für die ganze Gilde kreiert wird

func bool Npc_OwnedByGuild(C_ITEM item, int guild);
// Liefert "1", wenn der angegebenen Gilde das Item gehört ( Gildenbesitz )

func bool Npc_OwnedByNpc(C_ITEM item, C_NPC npc);
// Liefert "1", wenn dem NSC das Item gehört ( persönliches Besitzflag )

func void Npc_PercDisable(C_NPC npc, int percID);
// Deaktiviere Perception

func void Npc_PerceiveAll(C_NPC npc);
// Nimm alle Objekte in Wahrnehmungsreichweite wahr, die dann mit WLD_DetectNpc und Wld_DetectItem auswertbar sind

func void Npc_PercEnable(C_NPC npc, int percID, func action);
// Aktiviere Perception

func void Npc_PlayAni(C_NPC npc, string animationName);

func bool Npc_RefuseTalk(C_NPC npc);
// Abfrage ob Dialog-Refuse Counter noch aktiv ist True/False

func bool Npc_RemoveInvItem(C_NPC owner, int instanceItem);
// das angegebene Item wird aus dem Inventory des NSCs entfernt und gelöscht
// True wenn erfolgreich

func bool Npc_RemoveInvItems(C_NPC owner, int instanceItem, int amount);
// das angegebene Anzahl des Multi-Items wird aus dem Inventory des NSCs entfernt und gelöscht
// wie Npc_RemoveInvItem, nur das Multislotgegenstände gelöscht werden
// True wenn erfolgreich

func void Npc_SendPassivePerc(C_NPC npc1, int perc_type, C_NPC victim, C_NPC offender);
// Sende eine passive Wahrnehmung aus.Npc1 = wer schickt victim = Opfer, offender = Täter

func void Npc_SendSinglePerc(C_NPC npc, C_NPC target, int percID);
// verschicke Wahrnehmung an einzelnen NSC

func int Npc_SetActiveSpellInfo(C_NPC npc, int value);
// Hier kann ein Wert für den Zauberspruch gesetzt werden.
// Was dieser Wert bewirkt, haengt allein von der Nutzung im Skript ab. Das Programm benutzt diesen nicht.
// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist

func void Npc_SetAttitude(C_NPC npc, int attitude);
// setzt die permanente Attitüde auf einen festen Wert

func void Npc_SetKnowsPlayer(C_NPC npc, C_NPC player);
// NSC kennt SC

func void Npc_SetPercTime(C_NPC npc, float seconds);
// Setze Zeitdelta für aktive Wahrnehmungen, alle Zeitdelta-Sekunden wird WN gesendet

func void Npc_SetRefuseTalk(C_NPC npc, int seconds);
// Dialog Refuse Counter auf seconds Sekunden setzen

func void Npc_SetStateTime(C_NPC npc, int seconds);
// _Setzt_ Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
// --> Das ist dann über Npc_GetStateTime abfragbar.

func void Npc_SetTalentSkill(C_NPC npc, int talent, int level);
// Setzt das Skill-Level des Talents

func void Npc_SetTalentValue(C_NPC npc, int talent, int value);
// setzt talent value auf den angegebenen Wert. Wird in gothic 1 benutzt für 1h/2h/bow/crossbow/picklock/pickpocket

func void Npc_SetTarget(C_NPC npc, C_NPC other);
// Gibt dem Nsc 'npc' das interne Ziel 'other'.
// --> Nur wenn per GetTarget auch der other "geholt" wird ist er vorhanden,
// da hier interne Variablen, die in den Skripten nicht vorhanden sind verwendet werden

func void Npc_SetTeleportPos(C_NPC npc);
// Magie/Zauberstein Teleport Spruch : letzte Position des NSCs merken, zwecks späteren dahinbeamens.

func void Npc_SetTempAttitude(C_NPC npc, int attitude);
// setzt die temporäre Attitüde auf einen Wert (att)

func void Npc_SetToFightMode(C_NPC npc, int weapon);
// Setzt den NSC beim Start in den der Waffe entsprechenden Kampfmodus (Waffe wird erzeugt)

func void Npc_SetToFistMode(C_NPC npc);
// Setzt den NSC beim Start in den Faustkampfmodus (zB.Monster)

func int Npc_SetTrueGuild(C_NPC npc, int guildID);
// Setzt die wahre Gilde des NSCs

func bool Npc_StartItemReactModules(C_NPC npc, C_NPC other, C_ITEM item);
// Prüfe alle ItemReact-Module von "npc" auf Gegenstand "item" von Geber "other" und starte passende Reaction-Funktion
// liefert True beim finden eines Moduls

func void Npc_StopAni(C_NPC npc, string animationName);

func bool Npc_WasInState(C_NPC npc, func state);
// Abfrage auf den vorherigen Zustand einer Spielfigur True/False

func bool Npc_WasPlayerInMyRoom(C_NPC npc);
// gibt TRUE zurück, wenn sich SC vor dem letzten Raumwechsel im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen

func void Perc_SetRange(int percID, int range);
// Setze Reichweite fuer eine passive Wahrnehmung int cm

func bool PlayVideo(string bikName);
// Gothic 2 only function
// Spielt eine Videodatei ab.
//
//      bikName         - Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
//      [result]        - Boolean ob erfolgreich abgespielt

func bool PlayVideoEx(string bikName, bool screenBlend, bool exitSession);
// Gothic 2 only function
// Spielt eine Videodatei mit erweiterten Optionen ab.
//
//      bikName         - Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
//      ScreenBlend     - Boolean ob Effekt 'BLACK_SCREEN' danach abgespielt wird (nicht bei ExitSession)
//      ExitSession     - Boolean ob nach dem Video die Session beendet wird
//      [result]        - Boolean ob erfolgreich abgespielt, bei ExitSession immer erfolgreich

func void Print(string text);
// Ausgabebefehl, der Text ins Game schreibt (wie OutputunitTexte)

func void PrintDebug(string text);
// Printausgabe, die nur bei eingeschaltetem Debugmodus (Alt+D) im Spy/logfile ausgegeben wird

func void PrintDebugCh(int ch, string text);
// gibt nur den Text eines bestimmten Channels wieder, unabhängig von der Instanz

func void PrintDebugInst(string text);
// gibt nur den Debugtext der eingeschalteten (SetDebugFocus --> siehe Intranetpage Erweitertes Skript-Debugging) Instanz aus

func void PrintDebugInstCh(int ch, string text);
// gibt nur in einem bestimmten channel liegende Debuginfos einer Instanz aus

func int PrintDialog(int dialogNr, string text, int x, int y, string font, int timeSec);

func void PrintMulti(string text1, string text2, string text3, string text4, string text5);
// Printbefehl, der aus den angegebenen Strings einen Auswählt und auf den Bildschirm schreibt

func bool PrintScreen(int dialogNr, string text, int posX, int posY, string font, int timeSec);
// Variable dialogNr wird nicht benutzt
// Gibt den Text 'text' auf dem Bildschrim aus und benutzt dabei den Font 'font'.
// Die Position ist für jede Koordinate eine Zahl zwischen 0 und 99 und gibt die prozentuale Position an.
// Der Ursprung befindet sich oben links (also 0% X und 0% Y)
// Liefert False, wenn font nicht gefunden wurde, sonst True

func void Rtn_Exchange(string oldRoutine, string newRoutine);
// Tausche aktuellen Tagesablauf des NSC "self" gegen angegebenen aus
// (Name wird automatisch mit "RTN_" am Anfang und NSC-Skript-ID am Ende ergänzt)

func void SetPercentDone(int percentDone);
// Gothic 2 only function
// Setz die Fortschrittsanzeige im Ladebalken.
//
//      PercentDone     - Aktueller Fortschritt des Ladevorgangs in Prozent (0-100)

func int Snd_GetDistToSource(C_NPC npc);
// Liefert Entfernung ( in cm ! ) zum letzten logischen Sound

func bool Snd_IsSourceItem(C_NPC npc);
// Check, ob Quelle des letzten Sound Item war (Return >0) und setzt "item" auf diesen Gegenstand

func bool Snd_IsSourceNpc(C_NPC npc);
// Check, ob Quelle des letzten Sound NPC war (Return >0) und setzt "other" auf diesen NPC

func void Snd_Play(string sfxName);
// spielt einen Sound ab

func void Snd_Play3D(C_NPC npc, string sfxName);
// spielt einen 3D-Sound ab.

func void TA(C_NPC npc, int start_h, int stop_h, func state, string waypointName);
// Mit _(Zustandsname) wird ein neuer Tagesablauf generiert, siehe TA.d

func void TA_BeginOverlay(C_NPC npc);
// Melde einen Overlay-Tagesablauf an

func void TA_CS(C_NPC npc, string csName, string roleName);
// Cutscene an den zuletzt angegebenen Tagesablaufpunkt hängen
// csName:      Name der Cutscene ( der Name des "CS" - Files )
// roleName:    Die Rolle die der NSC dabei übernehmen soll.

func void TA_EndOverlay(C_NPC npc);
// Beende einen Overlay-Tagesablauf

func void TA_Min(C_NPC npc, int start_h, int start_m, int stop_h, int stop_m, func state, string waypointName);
// Tagesablaufpunkt minutengenau angeben

func void TA_RemoveOverlay(C_NPC npc);
// Entferne aktiven TA-Overlay

func void Tal_Configure(int talent, int value);
// wird nicht benutzt?

func void Update_ChoiceBox(string s0);

func void Wld_AssignRoomToGuild(string room, int guild);
// Ordnet den Raum: 'room' der Gilde 'guild' zu

func void Wld_AssignRoomToNpc(string room, C_NPC roomOwner);
// Ordnet den Raum: 'room' dem speziellen Nsc 'roomOwner' zu

func bool Wld_DetectItem(C_NPC npc, int flags);
// liefert eins zurück, wenn ein Item mit dem Entsprechende Flag (z.B.ITEM_KAT_FOOD )gefunden wurde
// Globale Variable 'item' wird mit dem gefundenen Gegenstand initialisiert

func bool Wld_DetectNpc(C_NPC npc, int instanceNpc, func aiState, int guild);
// Diese Methode initilisiert die globale Skriptvariable "other" mit einem NSC, den "npc" beim letzten Warnehmungscheck wargenommen hat.
// instanceNpc  = Name der zu suchenden Instanz                                 ( "-1" angeben, wenn Instanzname unwichtig )
// guild        = Der zu suchende NSC muss Mitglied dieser Gilde sein           ( "-1" angeben, wenn Gilde unwichtig )
// aiState      = Der AI-Zustandsname, in dem sich der NSC befinden soll        ( NOFUNC angeben, wenn AI-State unwichtig )
// Wenn die Methode einen entsprechenden NSC gefunden hat, liefert diese "1" und 'other' ist initialisiert
// ansonsten wird "0" geliefert und "other" wird nicht verändert.

func bool Wld_DetectNpcEx(C_NPC npc, int instanceNpc, func aiState, int guild, bool detectPlayer);
// Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.

func int Wld_DetectNpcExAtt(C_NPC npc, int instanceName, func state, int guild, bool detectPlayer, int Attitude);

func bool Wld_DetectPlayer(C_NPC npc);
// liefert eins zurück, wenn der Player in der Nähe ist

func void Wld_ExchangeGuildAttitudes(string tableName);
// Tausche Gilden-AttitudenTabelle aus.

func int Wld_GetDay();
// Liefert den aktuellen Tag zurück (Zähler), hierbei ist der StartTag (Gamestart) = 0.

func int Wld_GetFormerPlayerPortalGuild();
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert

func C_NPC Wld_GetFormerPlayerPortalOwner();
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'

func int Wld_GetGuildAttitude(int guild1, int guild2);
// Ermittle Gildenattitude

func int Wld_GetMobState(C_NPC npc, string schemeName);
// Liefert den State zum nächsten Mob mit 'schemeName' zurück bzw. '-1' wenn kein solches Mob gefunden wurde

func int Wld_GetPlayerPortalGuild();
// liefert Gilde des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert

func C_NPC Wld_GetPlayerPortalOwner();
// liefert den NSC des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'

func void Wld_InsertItem(int itemInstance, string fpOrWPName);
// Füge Item in Welt ein entweder an einem WP oder einem FP
// Vorsicht, funktioniert nicht, Items werden immer im Mittelpunkt der Welt inserted

func void Wld_InsertNpc(int npcInstance, string fpOrWPName);
// Füge NSC in Welt ein. Wobei Spawnpoint entweder ein WP oder ein FP sein darf.

func void Wld_InsertNpcAndRespawn(int instanceNpc, string fpOrWPName, int spawnDelaySec);
// Füge NSC in Welt ein. Wobei Spawnpoint entweder ein WP oder ein FP sein darf. Stirbt dieser NSC wird
// nach "spawnDelay"-Sekunden ein neuer NSC am Spawnpoint erzeugt.

func void Wld_InsertObject(string objName, string fpOrWPName);

func bool Wld_IsFPAvailable(C_NPC npc, string freepointName);
// Sucht einen Freepoint im Umkreis von 20m vom NSC und liefert TRUE falls vorhanden und frei ('npc' zählt als Blockierer nicht!) und sichtbar

func bool Wld_IsMobAvailable(C_NPC npc, string schemeName);
// Sucht sich ein Mobsi im Umkreis von 10m und liefert TRUE falls gefunden. MOB wird nur zurückgeliefert, wenn es nicht besetzt ist.

func bool Wld_IsNextFPAvailable(C_NPC npc, string freepointName);
// wie Wld_IsFPAvailable(), aber es wird immer der nahegelegenste genommen und 'npc' zählt als Blockierer!

func bool Wld_IsRaining();

func bool Wld_IsTime(int hour1, int min1, int hour2, int min2);
// Liefert zurück ob die aktuelle Weltzeit zwischen den beiden angegebenen Zeiten liegt (von - bis)

func void Wld_PlayEffect(string effectVFXName, C_INSTANCE originvob, C_INSTANCE targetvob, int effectLevel, int damage, int damageType, bool bIsProjectile);
// effectVFXName: Name der VisualFX-Instanz
// originvob: Ursprung/Verursacher (muss existieren!) NPC, item, freepoint, ...
// targetvob: Ziel fuer Effekt + Schaden
// effectLevel: Bei Angabe von effectLevel wird nach einer VisualFX-Instanz mit dem Namen _L gesucht und verwendet falls sie
// gefunden wurde. (es muss trotzdem eine VisualFX-Instanz mit dem Namen definiert worden sein!)
// damage: Hoehe des Schadens
// damageType: Schadensart (DAM_Xxx)
// bIsProjectile: Effekt ist Projektil

func bool Wld_RemoveItem(C_ITEM item);
// Hiermit wird das angegebene Item aus der Welt entfernt und gelöscht, gibt TRUE zurück wenn erfolgreich

func void Wld_RemoveNpc(int npcInstance);

func void Wld_SendTrigger(string vobName);
// Sendet eine Trigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.

func void Wld_SendUntrigger(string vobName);
// Sendet eine UnTrigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.

func void Wld_SetGuildAttitude(int guild1, int attitude, int guild2);
// Setze Gildenattitude neu

func void Wld_SetMobRoutine(int hour, int min, string objName, int state);
// _Alle_ Mobs mit diesem _Schemanamen_ werden getriggert.

func void Wld_SetObjectRoutine(int hour, int min, string objName, int state);
// _Ein_ Objekt mit diesem _Vobnamen_ wird getriggert

func void Wld_SetTime(int hour, int min);
// Setzt die Uhrzeit auf hour:min. hour kann größer als 23 sein, um zum nächsten Tag zu springen.

func void Wld_SpawnNpcRange(C_NPC summoner, C_NPC_ID creature, int count, float radius);
// NPC summoner beschwört die Diener creature der Anzahl count. Die Diener erscheinen in einer Entfernung von radius (in cm).

func void Wld_StopEffect(string vfxName);

