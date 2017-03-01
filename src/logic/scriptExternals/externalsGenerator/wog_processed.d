func void AI_AimAt(c_npc attacker, c_npc target);
// NPC zielt mit Fernkampfwaffe auf Target-NPC

func void AI_AlignToFP(c_npc self);
// richtet den Nsc am Freepoint aus (im Spacer gesetzte Pfeilrichtung)

func void AI_AlignToWP(c_npc self);
// richtet den Nsc am Waypoint aus (im Spacer gesetzte Pfeilrichtung)

func void AI_Ask(c_npc self, func anseryes, func answerno);
// die angegeben Instanz (self), gibt eine Frage aus und verweist auf die selbst zu definierenden Funktionen,
// die für die Fälle Spieler sagt ja (Daumen nach oben) und Spieler sagt nein (Daumen unten) vorhanden sein müssen

func void AI_AskText(c_npc self, func funcyes, func funcno, string stryes, string strno);
// wie AI_Ask, nur das außer den Funktionen auch noch Strings für die Antworten Ja/Nein mit angegeben werden können

func void AI_Attack(c_npc self);
// Startet Kampf AI (sollte in der ZS_Attack-Loop stehen)
// Es wird das interne Ziel verwendet, das mit Npc_SetTarget() oder Npc_GetNextTarget() gesetzt
// wurde.

func void AI_CanSeeNpc(instance n0, instance n1, func f2);

func void AI_CombatReactToDamage(instance n0);

func void AI_ContinueRoutine(c_npc self);
// Setze Tagesablauf fort
// Enthält Standup

func void AI_Defend(c_npc self);
// Der Befehl ist als Overlay-Message implementiert. Das heisst, dass er neben anderen Nachrichten
// aktiv bleibt. Er wird erst beendet, wenn der NPC eine Parade (ausgeloest durch die Attacke eines
// anderen NPCs) durchgefuert hat.

func void AI_Dodge(c_npc npc);
// Der Nsc weicht ein Stück nach hinten aus

func void AI_DrawWeapon(c_npc n0);
// Equipte Waffe wird gezogen

func void AI_DropItem(c_npc self, int itemid);
// Item(itemid) wird auf den Boden fallen gelassen

func void AI_DropMob(instance n0);

func void AI_EquipArmor(c_npc owner, c_item armor_from_owners_inventory);
// Ziehe die angebene Rüstung dem NSC "owner" an, diese muss sich in seinem Inventory befinden.

func void AI_EquipBestArmor(c_npc self);
// Wunder, Wunder hier wird die beste im Inventory vorhandene Rüstung angezogen

func void AI_EquipBestMeleeWeapon(c_npc self);
// sucht im Inventory nach der besten Nahkampfwaffe und hängt sie an den Gürtel

func void AI_EquipBestRangedWeapon(c_npc self);
// sucht im Inventory nach der besten Fernkampfwaffe und ploppt sie auf den Rücken der Instanz

func void AI_FinishingMove(c_npc self, c_npc other);
// Führt den logischen Finishing Move inklusive Anis aus den Skripten heraus aus

func void AI_Flee(c_npc self);
// Der Befehl muss, wie AI_Attack(), in der ZS-Loop regelmaessig aufgerufen werden und setzt voraus, dass
// vorher mit Npc_SetTarget( self, ) ein Gegner gesetzt wurde, vor dem der Npc fliehen soll.

func void AI_GotoFP(c_npc self, string fpname);
// Sucht sich einen Freepoint im Umkreis von 20m vom NSC, bewegt sich dorthin und richtet sich entsprechend aus.
// Suchkriterium wie bei Wld_IsFPAvailable()

func void AI_GotoItem(c_npc self, c_item item);
// "self" geht zu "item"

func void AI_GotoNextFP(c_npc self, string fpname);
// wie AI_GotoFP() allerdings Suchkriterium wie bei Wld_IsNextFPAvailable()

func void AI_GotoNpc(c_npc self, c_npc other);
// "self" geht zu "other"

func void AI_GotoSound(c_npc n0);
// Npc läuft zum Sound

func void AI_GotoWP(c_npc n0, string s0);
// Npc-Instanz läuft zum namentlich angegeben Waypoint

func void AI_LookAt(c_npc self, string name);
// Schaue auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)

func void AI_LookAtNpc(c_npc self, c_npc other);
// Schaue zu einem NSC

func void AI_LookForItem(c_npc self, int instance);
// gibt die Möglichkeit nach bestimmten Items zu suchen (z.B.:Das goldene Schwert der Zerstörung, wenn vorhanden)

func void AI_Output(c_npc self, c_npc target, string outputname);
// Cutscene mit entsprechender Id wird abgenudelt

func void AI_OutputSVM(c_npc self, c_npc target, string svmname);
// Outputbefehl um Svms abzuspielen

func void AI_OutputSVM_Overlay(c_npc self, c_npc target, string svmname);
// wie AI_OutputSVM, wartet jedoch NICHT mit der Ausführung des nächsten AI_...-Befehls, bis
// das SVM zuende gespielt wird. (Für Kommentare kurz vor und während dem Kampf!)

func void AI_PlayAni(c_npc n0, string s0);
// Npc-Instanz spielt die angegebene Animation ab

func void AI_PlayAniBS(c_npc npc, string aniname, int bodystate);
// Beim Abspielen einer Ani mit diesem Befehl kann ein Bodystate angemeldet werden

func void AI_PlayCutscene(c_npc self, string csname);
// Eine Cutscene aus den Scripten heraus starten

func void AI_PlayFX(instance n0, instance n1, string s2);

func void AI_PointAt(c_npc self, string name);
// Zeige auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)

func void AI_PointAtNpc(c_npc self, c_npc other);
// Zeige auf einen NSC

func int AI_PrintScreen(string s0, int i1, int i2, string s3, int i4);

func void AI_ProcessInfos(instance n0);

func void AI_Quicklook(c_npc self, c_npc other);
// NSC kurz ( 2 sec) anschauen ( nur Kopf bewegt sich )

func void AI_ReadyMeleeWeapon(c_npc self);
// Ziehe equippte Nahkampfwaffe

func void AI_ReadyRangedWeapon(c_npc self);
// Ziehe equippte Fernkampfwaffe

func void AI_ReadySpell(c_npc self, int spellid, int investmana);

func void AI_RemoveWeapon(c_npc n0);
// Gezogene Waffe wird weggesteckt

func void AI_SetNpcsToState(c_npc self, func aistatefunc, int radius);
// Setze alle NSCs im Umkreis von x cm in entsprechenden AI-Zustand
// VORSICHT: Diese Funktion ist Buggy, wird aber derzeit auch NICHT verwendet!
// -> FINGER WEG!!! (SN)

func void AI_SetWalkmode(c_npc n, int n0);
// gibt an mit welchem Walkmode Run etc der Character durch das Level läuft
// NPC_RUN : Rennen
// NPC_WALK : Gehen
// NPC_SNEAK : Schleichen
// NPC_RUN_WEAPON : Rennen mit gezogener Waffe
// NPC_WALK_WEAPON : Gehen mit gezogener Waffe
// NPC_SNEAK_WEAPON : Schleichen mit gezogener Waffe

func void AI_ShootAt(c_npc attacker, c_npc target);
// NPC feuert mit Fernkampfwaffe auf Target-NPC

func void AI_Snd_Play(instance n0, string s1);

func void AI_Snd_Play3D(instance n0, instance n1, string s2);

func void AI_StandUp(c_npc self);
// - Ist der Nsc in einem Animatinsstate, wird die passende Rücktransition abgespielt.
// - Benutzt der NSC gerade ein MOBSI, poppt er ins stehen.

func void AI_StandUpQuick(c_npc self);
// Wie AI_StandUp(), jedoch werden keine Rücktransitionen abgespielt, sondern auch dort wird
// sofort in den Grundzustand "gepoppt". Wichtig für sehr eilige Situationen!

func void AI_StartState(c_npc self, func what, int statebehaviour, string wpname);
// Versetzt den Nsc aus den Skripten heraus in den entsprechenden ZS (what),
// stateBehaviour sagt : "0"-aktuellen Zustand abbrechen "1"-aktuellen Zustand erst ordnungsgemäß beenden (End-Funktion aufrufen) ).

func void AI_StopAim(c_npc attacker);
// NPC beendet vorher gestartetes Zielen mit Fernkampfwaffe.

func void AI_StopFX(instance n0, string s1);

func void AI_StopLookAt(c_npc self);
// wieder geradeaus schauen

func void AI_StopPointAt(c_npc self);
// nicht mehr auf etwas zeigen

func void AI_StopProcessInfos(c_npc npc);
// Der DialogModus wird beendet (Multiple Choice-Dialog)

func void AI_TakeItem(c_npc self, c_item item);
// der Npc nimmt mit dieser Funktion die globale Item-Instanz auf

func void AI_TakeMob(instance n0, string s1);

func void AI_Teleport(c_npc self, string waypoint);
// teleportiert den NSC zur angegebenene Location

func void AI_TurnAway(c_npc n0, c_npc n1);
// Der NSC "self" dreht dem NSC "other" den Rücken zu.

func void AI_TurnToNpc(c_npc n0, c_npc n1);
// drehe Dich zum angegeben (zweiten) Npc um

func void AI_TurnToSound(c_npc self);
// Charakter dreht sich zur Geräuschquelle

func void AI_UnequipArmor(c_npc self);
// Unequippe aktuelle Rüstung

func void AI_UnequipWeapons(c_npc self);
// Unequippe alle Waffen

func void AI_UnreadySpell(c_npc self);
// lasse zauberspruch aus Hand verschwinden

func void AI_UseItem(c_npc self, int iteminstance);
// Item bis zum Ende benutzen

func void AI_UseItemToState(c_npc self, int iteminstance, int state);
// Item benutzen bis zum angegebenen State

func int AI_UseMob(c_npc self, string schemename, int targetstate);
// Benutze Mob mit angegebenen Schema-Namen bis zum Zustand "targetState". Wird diese Funktion aufgerufen
// und der angegebene 'targetState' ist bereits vorhanden, läuft der NSC zwar trotzdem zum MOB, tut dann aber nichts

func void AI_Wait(c_npc n0, float n1);
// Character wird für n1 Sekunden in einen Wait-Zustand versetzt,
// d.h. er tut nichts, aber Treffer etc.(passive Wahrnehmung) werden registriert

func void AI_WaitForQuestion(c_npc self, func scriptfunc);
// NSC wartet 20 Sekunden, wird er in dieser Zeit vom Spieler angesprochen, wird die angegebene SkriptFunktion ausgeführt.

func void AI_WaitMS(instance n0, int i1);

func void AI_WaitTillEnd(c_npc self, c_npc other);
// 'self' wartet bis 'other' seinen laufenden AI-Befehl zu Ende gespielt hat (funzt allerdings nicht bei AI-Overlays!)

func void AI_WhirlAround(c_npc self, c_npc other);
// schnelle Drehung zu other

func void AI_WhirlAroundToSource(instance n0);

func void Apply_Options_Audio();

func void Apply_Options_Controls();

func void Apply_Options_Game();

func void Apply_Options_Performance();

func void Apply_Options_Video();

func string ConcatStrings(string str1, string str2);
// !!!Erzeugt eine Kopie von dem ersten angegebenen String, hängt den zweiten an diesen an und gibt den neuen String zurück ???
// Die folgenden Methoden geben den Text nur unter bestimmten zusätzlichen Bedingungen aus. Für die Methoden mit dem Suffix Inst trifft dies zu, wenn die Instanz von der die Debug-Methode aufgerufen wird, zum Debuggen aktiviert wurde. Die Methoden mit dem Suffix Ch geben den Text nur aus, wenn der angegebene Channel zum Debuggen aktiviert wurde.

func void CreateInvItem(c_npc n0, int n1);
// gibt der angegebenen Instanz (self oder other) ein Item ins Inventory

func void CreateInvItems(c_npc n0, int n1, int n2);
// wie vorherige Funktion, nur das Multi-Items (z.B.: 100 Pfeile) ins Inventory kreiert werden

func int Doc_Create();

func int Doc_CreateMap();

func void Doc_Font(string Font);

func void Doc_MapCoordinates(string Level, float GameX1, float GameY1, float PixelX1, float PixelY1, float GameX2, float GameY2, float PixelX2, float PixelY2);

func void Doc_Open(string Texture);

func void Doc_Print(string Text);

func void Doc_PrintLine(int document, int page, string text);

func void Doc_PrintLines(int document, int page, string text);

func void Doc_SetFont(int document, int page, string font);

func void Doc_SetLevel(int document, string level);

func void Doc_SetLevelCoords(int document, int left, int top, int right, int bottom);

func void Doc_SetMargins(int document, int page, int left, int top, int right, int bottom, int pixels);

func void Doc_SetPage(int document, int page, string texture, int scale);

func void Doc_SetPages(int document, int count);

func void Doc_Show(int document);

func void EquipItem(c_npc n0, int n1);
// gibt dem Nsc direkt das angegebene Item an den Gürtel

func void ExitGame();

func void ExitSession();

func int FloatToInt(float x);
// !!! konvertiert einen float in einen int, hier wäre es noch ganz schön zu wissen, wie mit dem float umgesprungen wird --> schneiden oder runden ???

func string FloatToString(float r0);

func void Game_InitEnglish();

func void Game_InitGerman();

func int Hlp_CutscenePlayed(string csname);
// Abfrage, ob Cutscene schon gespielt wurde (0 = Nein / 1 = Ja)

func int Hlp_GetInstanceID(c_item item);
// liefert die interne ID ( nicht Var aus der Instanz) zurück, um zwei items miteinander vergleichen zu können ( integer Vergleich)

func c_npc Hlp_GetNpc(int instancename);
// Ermittle einen NSC über den Instanznamen. Dieser kann einer Variablen zugewiesen werden und ist somit gezielt verfügbar

func int Hlp_IsItem(c_item item, int instancename);
// Prüft, ob der Gegenstand 'item' den Instanznamen 'instanceName' trägt. Will man z.B. testen,
// ob die globale Variable item Pfeile sind (ItMuArrow) ruft man Hlp_IsItem(item,ItMuArrow) auf.
// Gribt bei Gleichheit TRUE, sonst FALSE zurück.

func int Hlp_IsValidItem(c_item item);
// Prüfe ob Item-Instanz gültig und initialisiert ist True/False

func int Hlp_IsValidNpc(c_npc self);
// Prüfe ob NSC-Instanz gültig und initialisiert ist. True/False

func int Hlp_Random(int n0);
// erzeugt einen Zufallswert (wobei n0 Obergrenze : [0..n0-1] ) Rückgabewert integer

func int Hlp_StrCmp(string s1, string s2);
// Hilfsfunktion um zwei Strings miteinander zu vergleichen liefert bei Gleichheit eins zurück

func void Info_AddChoice(int i0, string s1, func f2);

func void Info_ClearChoices(int i0);

func int InfoManager_HasFinished();

func void IntroduceChapter(string titel, string untertitel, string texture, string sound, int waittime);

func float IntToFloat(int x);
// !!! wie float to int, nur daß ein integer in float umgewandelt wird ???

func string IntToString(int x);
// !!!Funktion überträgt eine Integer variable in eine Stringvariable (Rückgabewert : string)???

func void Log_AddEntry(string topic, string entry);
// Der Befehl fügt dem Topics topic ein neues Entrz namens entry hinzu. Wie auch bei der Erzeugung von Topics dient hier der
// Name sowohl zur Identifikation als auch zur Anzeige und muss somit eindeutig sein. Doppelte Einträge werden auch hier ignoriert.
// Parameter
// topic Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
// entry Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Eintrags verwendet wird.

func void Log_CreateTopic(string name, int section);
// Der Befehl fügt unter der Sektion 'section' ein neues Topic mit Namen 'name' ein.
// Sollte bereits ein Topic dieses Names in irgendeiner Sektion existieren, wird das Topic nicht hinzugefügt.
// Parameter:
// - name Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Topics verwendet wird.
// - section Eine, in der Constants.d definierte Konstante, die besagt, in welcher Sektion das Topic angezeigt werden soll.
// Die Konstante kann folgende Werte annehmen:
// - LOG_MISSION
// - LOG_NOTE

func void Log_SetTopicStatus(string name, int status);
// Der Befehl ändert den Status des Topics 'name' in 'status'.
// Er sollte nur für Topics verwendet werden, die für die Sektion LOG_MISSION erzeugt wurden.
// Parameter:
// - name Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
// - status Eine, in der Constants.d definierte Konstante, die besagt, unter welchem Status die Mission dargestellt werden soll.
// Folgende Werte sind möglich:
// - LOG_RUNNING
// - LOG_SUCCESS
// - LOG_FAILED
// - LOG_OBSOLETE

func void Mdl_ApplyOverlayMds(c_npc n0, string s1);
// mit diesem Befehl werden Animationen auf einem höheren Layer gestartet
// (z.B. hat der Zombie ein Overlay über die normalen Human-Animationen)

func void Mdl_ApplyOverlayMDSTimed(c_npc self, string overlayname, float timeticks);
// Overlay-MDS wird gestartet und automatisch nach der angegebenen Zeit abgeschaltet

func void Mdl_ApplyRandomAni(c_npc n0, string s1, string s2);
// Fügt Zufallsanimationen (am Kopf kratzen, an den Sack packen etc (s2)) für bestimmte Animationszustände (s1) ein

func void Mdl_ApplyRandomAniFreq(c_npc n0, string s1, float f2);
// hiermit kann die Frequenz betimmt werden, wie oft die für den Animationszustand (s1) deklarierten Randomanis abgespielt werden

func void Mdl_ApplyRandomFaceAni(c_npc self, string name, float timemin, float timeminvar, float timemax, float timemaxvar, float probmin);
// Starte zufällige Gesichtsanimation
// Mdl_ApplyRandomFaceAni ( self, ANINAME, minTime, minTimeVar, maxTime, maxTimeVar, probMin)
// minTime = Minimum an Zeit nachdem Ani startet (in Sekunden)
// maxTime = Maximum an Zeit nachdem Ani startet (in Sekunden)
// minTimeVar = Abweichung von minTime (in Sekunden)
// maxTimeVar = Abweichung von maxTime (in Sekunden)
// probMin = Wahrscheinlichkeit [0..1] ob obere oder untere Grenze der Abweichung benutzt wird

func void Mdl_RemoveOverlayMDS(c_npc self, string overlayname);
// Entferne ein Overlay-MDS

func void Mdl_SetModelFatness(c_npc self, float fatness);
// Setzt Model-Z-Skalierung

func void Mdl_SetModelScale(c_npc self, float x, float y, float z);
// zum skalieren des Meshes (Breite,Höhe,Tiefe) 1 = 100%, also Normalgröße!

func void Mdl_SetVisual(instance n0, string s1);

func void Mdl_SetVisualBody(instance n0, string s1, int i2, int i3, string s4, int i5, int i6, int i7);

func void Mdl_StartFaceAni(c_npc self, string name, float intensity, float holdtime);
// Starte Gesichtsanimation // intensity 1 = 100% // holdTime -1 = forever

func void Mis_AddMissionEntry(instance n0, string s1);

func int Mis_GetStatus(int missionname);
// Liefert aktuellen Status einer Mission zurück ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc.

func int Mis_OnTime(int missionname);
// Liefert TRUE, wenn sich Spieler noch innerhalb des Zeitlimits für diese Mission befindet

func void Mis_RemoveMission(instance n0);

func void Mis_SetStatus(int missionname, int newstatus);
// Setzt den Status einer Mission ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc. )

func void Mob_CreateItems(string mobname, int iteminstance, int amount);
// Erzeuge "amount" Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen.

func int Mob_HasItems(string mobname, int iteminstance);
// Liefere Anzahl der Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen

func int Npc_AreWeStronger(c_npc self, c_npc other);
// ermittelt den stärkeren Nsc,
// Wenn die Summe der Level aller NPCs (human, Monster), die zu mir feindlich sind und die zu freundlich sind
// MEHR ALS DOPPELT SO HOCH ist (x > 2*y), wie die Summe der Level aller Leute, die zu mir freundlich sind, und die
// zu feindlich sind, dann NEIN,
// sonst JA (= wir SIND stärker, nicht echt, aber ab doppelter Gegnerstärke zu fliehen ist früh genug, sonst kommen
// die Jungs aus dem Rennen nicht mehr raus
// Beachten:
// 1) Monster können feindlich zu beiden Menschen sein --> egal, zählen auf beiden Seiten
// 2) Jemand, der z.B. zu mir freundlich und zu feindlich ist, wird demnach doppelt gewertet --> ok so

func int Npc_CanSeeItem(c_npc npc1, c_item item);
// Prueft ob Npc1 den Gegenstand sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False

func int Npc_CanSeeNpc(c_npc npc1, c_npc npc2);
// Prueft ob Npc1 den Npc2 sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False Blickwinkelabhängig (+,-100°)+LOS von der Hüfte aus

func int Npc_CanSeeNpcFreeLOS(c_npc self, c_npc other);
// Prüft ob NSC anderen NSC sehen kann, ohne dabei den Winkel zu berücksichtigen (nur LineOfSight-Check)

func int Npc_CanSeeSource(c_npc self);
// Checkt, ob NSC die Soundquelle sehen kann True/False

func void Npc_ChangeAttribute(c_npc self, int atr, int value);
// ändert den Wert des Attributs (atr) um (value) Einheiten.

func int Npc_CheckAvailableMission(c_npc npc, int missionstate, int important);
// Prüfe ob zwischen dem NSC eine aktuelle Mission (AVAILABLE,RUNNING) besteht und vergleiche den Status mit den angegebenen.
// Falls dieser identisch ist, wird "1" geliefert.

func int Npc_CheckInfo(c_npc npc, int important);
// Überprüft,ob der NSC gültige Infos für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").

func int Npc_CheckOfferMission(c_npc npc, int important);
// Überprueft ob der NSC dem Spieler einen Auftrag anbieten kann, wenn ja, wird der Offer-Block gestartet und "1" zurückgeliefert.

func int Npc_CheckRunningMission(c_npc npc, int important);
// Überprueft ob zwischen dem NSC und dem Spieler eine laufende Mission besteht und startet den entsprechenden Skriptblock.
// (Returnwert 1 : Mission aktiv)

func void Npc_ClearAIQueue(c_npc self);
// hiermit werden alle Befehle mit sofortiger Wirkung aus der AI_Queue rausgeworfen

func void Npc_ClearInventory(instance n0);

func void Npc_CreateSpell(c_npc self, int spellnr);
// NSc bekommt Zauberspruch zugewiesen kann diesen aber noch nicht gebrauchen
// (erscheint ausgegraut im Auswahlkranz)

func int Npc_DeleteNews(instance n0, int i1);

func void Npc_ExchangeRoutine(c_npc self, string routinename);
// Tausche meherere Tagesabläufe aus

func int Npc_GetActiveSpell(c_npc self);
// liefert den Zauber zurück, der auf der Hand ist (self oder other)
// liefert -1 zurück, wenn kein Zauber auf der Hand ist

func int Npc_GetActiveSpellCat(c_npc self);
// Unterscheidet zwischen den drei Kategorien (Spell_Bad, Spell_neutral,Spell_Good) Spellkat ist Rückgabewert

func int Npc_GetActiveSpellIsScroll(instance n0);

func int Npc_GetActiveSpellLevel(c_npc self);
// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist

func int Npc_GetAttitude(c_npc self, c_npc other);
// Gibt die Attitüde von self zu other zurück (temp. / perm. / Gilden )

func int Npc_GetBodyState(c_npc self);
// Ermittle BodyState ( Liefert BS_-Konstanten )
// !!! VORSICHT !!!: Diese Funktion gibt den Bodystate mit allen eventuellen
// ver-oderten Modifiern wie "brennend". Besser 'C_BodyStateContains(self,bodystate)' benutzen

func int Npc_GetComrades(instance n0);

func string Npc_GetDetectedMob(c_npc self);
// liefert den Schemanamen des Mobsi zurück, das der Nsc entdeckt hat. Schemaname ist das String-Kürzel des Mobs, daß im Visualnamen vor dem ersten "_"-Zeichen steht, also z.B. "DOOR" wenn der Visualname "DOOR_OCR__135" ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
// WORKAROUND: zur Zeit werden hiermit nur Tür-MOBSIs zurückgegeben, dies soll aber wieder zurückgebaut werden

func int Npc_GetDistToItem(c_npc npc, c_item item);
// Liefert Entfernung ( ín cm ! ) zwischend NSC und Gegenstand

func int Npc_GetDistToNpc(c_npc npc1, c_npc npc2);
// Liefert Entfernung ( ín cm ! ) zwischend den beiden NSCs

func int Npc_GetDistToPlayer(c_npc npc1);
// Liefert Entfernung ( ín cm ! ) zwischend den beiden NSCs

func int Npc_GetDistToWP(c_npc self, string wpname);
// liefert die Entfernung vom NSC 'self' zum angegebenen Waypoint in cm

func c_item Npc_GetEquippedArmor(c_npc n0);
// Liefert die angelegte Rüstung des NSCs.

func c_item Npc_GetEquippedMeleeWeapon(c_npc n0);
// Liefert die gegurtete Nahkampfwaffe des NSCs.

func c_item Npc_GetEquippedRangedWeapon(c_npc n0);
// Liefert die gegurtete Fernkampfwaffe des NSCs.

func int Npc_GetGuildAttitude(c_npc npc, c_npc npc);
// Ermittelt die Gildenattitüde von zwei Nsc´s direkt im Gegensatz zu Wld_GetGuildAttitude

func int Npc_GetHeightToItem(instance n0, instance n1);

func int Npc_GetHeightToNpc(c_npc npc1, c_npc npc2);
// Liefert Höhendifferenz ( ín cm ! ) zwischend den beiden NSCs

func c_item Npc_GetInvItem(c_npc self, int iteminstance);
// Ermittle ItemInstanz aus Inventory

func int Npc_GetInvItemBySlot(c_npc self, int category, int slotnr);
// Mit diesem Befehl läßt sich nachsehen, ob in einem bestimmten Slot einer bestimmten Kategorie ein item vorhanden ist
// ist das der Fall, wird dieses Item in die globale Variable item geschrieben
// gibt jetzt die Anzahl zurueck, wenn das Item stackable ist
// . Den Transfer machst Du dann per Npc_RemoveInvItems() und Npc_CreateInvItems().

func int Npc_GetLastHitSpellCat(c_npc self);
// liefert die Category des Zaubers zurück, der den NSC zuletzt getroffen hat

func int Npc_GetLastHitSpellID(c_npc self);
// liefert den Zauber zurück, der den NSC zuletzt getroffen hat

func instance Npc_GetLookAtTarget(instance n0);

func string Npc_GetNearestWP(c_npc self);
// liefert den Namen des am nächsten gelegenen Waypoints zurück

func c_npc Npc_GetNewsOffender(c_npc self, int newsnumber);
// Ermittelt Täter der News und gibt eine INstanz der Klasse C_Npc zurück

func c_npc Npc_GetNewsVictim(c_npc self, int newsnumber);
// Ermittle Opfer der News und gibt eine INstanz der Klasse C_Npc zurück

func c_npc Npc_GetNewsWitness(c_npc self, int newsnumber);
// Ermittle Zeuge der News und gibt eine INstanz der Klasse C_Npc zurück

func int Npc_GetNextTarget(c_npc self);
// Aktive Suche nach einem Gegner. Wird ein Gegner gefunden, so wird er als internes Ziel
// übernommen und in 'other' geschrieben, wenn kein Gegner gefunden wurde wird das
// interne Ziel gelöscht und 'other' ungültig gemacht.
// Kriterien: 1. gesetzter Gegner, der nicht tot/bewußtlos ist wird genommen...
// 2. ...dann der nächstbeste aNSC demgegenüber 'self' HOSTILE ist (auch weder tot noch bewußtlos)
// - return: neues Ziel gefunden -> TRUE
// kein Ziel gefunden -> FALSE
// VORSICHT: Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
// Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
// Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden

func string Npc_GetNextWP(c_npc self);
// Liefert den zweitnahesten WP vom NSC zurück

func int Npc_GetPermAttitude(c_npc self, c_npc other);
// Ermittle die permanente Attitude von "self" zu "other"

func int Npc_GetPortalGuild(instance n0);

func instance Npc_GetPortalOwner(instance n0);

func c_item Npc_GetReadiedWeapon(c_npc n0);
// Liefert die gezogene Waffe des NSCs.

func int Npc_GetStateTime(c_npc self);
// Liefert Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.

func int Npc_GetTalentSkill(instance n0, int i1);

func int Npc_GetTalentValue(instance n0, int i1);

func int Npc_GetTarget(c_npc self);
// Befüllt 'other' mit dem aktuellen Ziel. Das aktuelle Ziel wird intern gespeichert, wird
// durch Npc_SetTarget() bzw. Npc_GetNextTarget() gesetzt.
// - return: aktuelles Ziel gespeichert -> TRUE
// kein Ziel gespeichert -> FALSE

func int Npc_GetTrueGuild(c_npc npc);
// liefert immer!!! die wahre Gilde zurück, ignoriert also auch ohne die Regeln die Verkleidung

func int NPC_GiveInfo(c_npc npc, int important);
// Überprüft,ob der NSC eine (!) gültige Info für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").

func void Npc_GiveItem(c_npc n0, c_item n1, c_npc n2);
// Der NSC "self" gibt den NSC "other" den angegebenen Gegenstand "item". Der Gegenstand wandert sofort ins Inventory des anderen.

func int Npc_HasBodyFlag(c_npc self, int bodyflag);
// Liefert >0, falls BodyFlag gesetzt ist.

func int Npc_HasDetectedNpc(c_npc self, c_npc other);
// liefert eins zurück, wenn der Spieler in Sinnesreichweite und entdeckt ist // SPIELER ???

func int Npc_HasEquippedArmor(c_npc self);
// Armor angezogen True/False

func int Npc_HasEquippedMeleeWeapon(c_npc self);
// gibt eins zurück, wenn eine Nahkampfwaffe Equipped ist

func int Npc_HasEquippedRangedWeapon(c_npc self);
// Fernkampwaffe auf dem Rücken True/False

func int Npc_HasEquippedWeapon(c_npc self);
// gibt eins zurück, wenn die abgefragte Instanz (self oder other) eine Waffe sichtbar mit sich rumträgt

func int Npc_HasFightTalent(c_npc self, int tal);
// Spezialabfrage auf Kampftalente (z.B. 1hSword) ansonsten wie Npc_HasTalent

func int Npc_HasItems(c_npc n0, int iteminstance);
// Liefert "1", wenn NSC die angegebene Anzahl von Items besitzt.-> NpcHasItem ist damit obsolete

func int Npc_HasNews(c_npc self, int newsid, c_npc offender, c_npc victim);
// Liefert Newsnummer>0 (für weitere Referenzen) falls entsprechende News vorhanden.
// Nicht benötigte Parameter können mit "NULL" leergelassen werden

func int Npc_HasOffered(c_npc self, c_npc other, int iteminstance);
// Bietet Spieler dem NSC einen Gegenstand übers Trade-Modul an ? True/False

func int Npc_HasRangedWeaponWithAmmo(c_npc npc);
// Gibt TRUE zurück, wenn 'npc' irgendeine Fernkampfwaffe im Inventory oder in der Hand hat UND
// dazu auch passende Munition vorhanden ist. Sonst FALSE.

func int Npc_HasReadiedMeleeWeapon(c_npc self);
// Nahkampfwaffe in der Hand

func int Npc_HasReadiedRangedWeapon(c_npc self);
// Fernkampfwaffe in der Hand

func int Npc_HasReadiedWeapon(c_npc self);
// gibt eins zurück, wenn die Waffe schon in der Hand ist

func int Npc_HasSpell(c_npc self, int spellid);
// Kann der NSC den angegebenen Zauberspruch benutzen ?

func int Npc_HasTalent(c_npc self, int tal);
// liefert eins zurück, wenn der geprüfte Charakter das Talent tal hat

func int Npc_IsAiming(c_npc self, c_npc other);
// liefert eins zurück, wenn der zweite Character auf den ersten zielt (Fernkampfwaffen und Zauber)

func int Npc_IsDead(c_npc n0);
// Funktion liefert einen Wert zurück, falls Dead true ist

func int Npc_IsDetectedMobOwnedByGuild(c_npc user, int ownerguild);
// Liefert >0, falls "ownerguild" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!

func int Npc_IsDetectedMobOwnedByNpc(c_npc user, c_npc owner);
// Liefert >0, falls "owner" der Besitzer des vom "user" benutzten Mob ist.
// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!

func int Npc_IsDrawingSpell(instance n0);

func int Npc_IsDrawingWeapon(instance n0);

func int Npc_IsInCutscene(c_npc self);
// liefert eins zurück, wenn der entsprechende Nsc in einer Cutscene ist

func int Npc_IsInFightMode(c_npc self, int fmode);
// liefert eins zurück, wenn der Charakter im angegebenen Fightmode (z.B. FMODE_MAGIC) ist

func int Npc_IsInPlayersRoom(instance n0);

func int Npc_IsInRoutine(c_npc self, func state);
// Check ob der angegebene Zustand der aktuelle TA des NSCs ist. True/FAlse

func int Npc_IsInState(c_npc self, func state);
// Abfrage auf den aktuellen Zustand einer Spielfigur True/False

func int Npc_IsNear(c_npc self, c_npc other);
// liefert eins zurück, wenn geprüfte Instanz sich im Bereich von drei Metern zur prüfenden Instanz befindet

func int Npc_IsNewsGossip(c_npc self, int newsnumber);
// Liefert >0, falls News "Gossip" ist

func int Npc_IsNextTargetAvailable(c_npc self);
// Sucht genauso wie Npc_GetNextTarget nach einem neuen Ziel, überschreibt aber weder
// das interne Ziel, noch 'other'
// - return: neues Ziel gefunden -> TRUE
// kein Ziel gefunden -> FALSE
// VORSICHT: Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
// Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
// Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden

func int Npc_IsOnFP(c_npc self, string name);
// Abfrage darauf, ob der Nsc auf einem Freepoint mit name Teilstring steht

func int Npc_IsPlayer(c_npc player);
// liefert eins zurück, wenn der geprüfte Charakter der Spieler himself ist

func int Npc_IsPlayerInMyRoom(c_npc npc);
// gibt TRUE zurück, wenn sich SC im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE

func int Npc_IsVoiceActive(instance n0);

func int Npc_IsWayBlocked(c_npc self);
// Liefert "1", falls Weg von NSC durch Hindernis versperrt ist.

func int Npc_KnowsInfo(c_npc self, int infoinstance);
// Liefert TRUE, wenn der angegebene Spieler die Info schon einmal erhalten hat.
// VORSICHT: auch wenn eine permanente Info schon einmal dem Spieler erzählt wurde, so gibt diese Funktion trotzdem FALSE zurück!

func int Npc_KnowsPlayer(c_npc self, c_npc player);
// Kennt der NSC den Spieler? True/False

func void Npc_LearnSpell(c_npc self, int spellnr);
// Was könnte das wohl heißen ? Ich glaube damit kann man den (spellnr) Zauber zuweisen

func void Npc_MemoryEntry(c_npc self, int source, c_npc offender, int newsid, c_npc victim);
// mit dieser Funktion wird eine NSC bezogene News geschrieben newsid : News ID source > 0 : "gossip", ansonsten "witness",
// self: NSC, bei dem News eingespeist werden soll, other: Täter victim : Opfer

func void Npc_MemoryEntryGuild(c_npc self, int source, c_npc offender, int newsid, c_npc victimguild);
// wie MemoryEntry nur, das die Memory für die ganze Gilde kreiert wird

func int Npc_OwnedByGuild(c_item item, int guild);
// Liefert "1", wenn der angegebenen Gilde das Item gehört ( Gildenbesitz )

func int Npc_OwnedByNpc(c_item item, c_npc npc);
// Liefert "1", wenn dem NSC das Item gehört ( persönliches Besitzflag )

func void Npc_PercDisable(c_npc self, int percid);
// Deaktiviere Perception

func void Npc_PerceiveAll(c_npc self);
// Nimm alle Objekte in Wahrnehmungsreichweite wahr, die dann mit WLD_DetectNpc und Wld_DetectItem auswertbar sind

func void Npc_PercEnable(c_npc self, int percid, func function);
// Aktiviere Perception

func void Npc_PlayAni(instance n0, string s1);

func int Npc_RefuseTalk(c_npc self);
// Abfrage ob Dialog-Refuse Counter noch aktiv ist True/False

func void Npc_RemoveInvItem(c_npc owner, int iteminstance);
// das angegebene Item wird aus dem Inventory des NSCs entfernt und gelöscht

func void Npc_RemoveInvItems(c_npc owner, int iteminstance, int amount);
// das angegebene Anzahl des Multi-Items wird aus dem Inventory des NSCs entfernt und gelöscht

func void Npc_SendPassivePerc(c_npc npc1, int perc_type, c_npc npc2, c_npc npc3);
// Sende eine passive Wahrnehmung aus.Npc1 = wer schickt Npc2 = Opfer, Npc3 = Täter

func void Npc_SendSinglePerc(c_npc self, c_npc target, int percid);
// verschicke Wahrnehmung an einzelnen NSC

func int Npc_SetActiveSpellInfo(c_npc npc, int i1);
// Hier kann ein Wert für den Zauberspruch gesetzt werden. Was dieser Wert bewirkt, haengt allein von der Nutzung im
// Skript ab. Das Programm benutzt diesen nicht.
// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist

func void Npc_SetAttitude(c_npc self, int att);
// setzt die permanente Attitüde auf einen festen Wert

func void Npc_SetKnowsPlayer(c_npc self, c_npc player);
// NSC kennt SC

func void Npc_SetPercTime(c_npc self, float seconds);
// Setze Zeitdelta für aktive Wahrnehmungen, alle Zeitdelta-Sekunden wird WN gesendet

func void Npc_SetRefuseTalk(c_npc self, int timesec);
// Dialog Refuse Counter aus "x" Sekunden setzen

func void Npc_SetStateTime(c_npc self, int seconds);
// _Setzt_ Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
// --> Das ist dann über Npc_GetStateTime abfragbar.

func void Npc_SetTalentSkill(instance n0, int i1, int i2);

func void Npc_SetTalentValue(instance n0, int i1, int i2);

func void Npc_SetTarget(c_npc self, c_npc other);
// Gibt dem Nsc 'self' das interne Ziel 'other'. --> Nur wenn per GetTarget auch der other "geholt" wird ist er vorhanden, da hier interne Variablen, die in den Skripten nicht vorhanden sind verwendet werden

func void Npc_SetTeleportPos(c_npc self);
// Magie/Zauberstein Teleport Spruch : letzte Position des NSCs merken, zwecks späteren dahinbeamens.

func void Npc_SetTempAttitude(c_npc self, int att);
// setzt die temporäre Attitüde auf einen Wert (att)

func void Npc_SetToFightMode(c_npc self, int weapon);
// Setzt den NSC beim Start in den der Waffe entsprechenden Kampfmodus (Waffe wird erzeugt)

func void Npc_SetToFistMode(c_npc self);
// Setzt den NSC beim Start in den Faustkampfmodus (zB.Monster)

func int Npc_SetTrueGuild(c_npc npc, int guildid);
// Setzt die wahre Gilde des NSCs

func int Npc_StartItemReactModules(c_npc self, c_npc other, c_item item);
// Prüfe alle ItemReact-Module von "self" auf Gegenstand "item" von Geber "other" und starte passende Reaction-Funktion
// liefert True beim finden eines Moduls

func void Npc_StopAni(instance n0, string s1);

func int Npc_WasInState(c_npc self, func state);
// Abfrage auf den vorherigen Zustand einer Spielfigur True/False

func int Npc_WasPlayerInMyRoom(c_npc npc);
// gibt TRUE zurück, wenn sich SC vor dem letzten Raumwechsel im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen

func void Perc_SetRange(int percid, int range);
// Setze Reichweite fuer eine passive Wahrnehmung int cm

func int PlayVideo(string filename);

func int PlayVideoEx(string filename, int screenblend, int exitsession);

func void Print(string s0);
// Ausgabebefehl, der Text ins Game schreibt (wie OutputunitTexte)

func void PrintDebug(string s);
// Printausgabe, die nur bei eingeschaltetem Debugmodus (Alt+D) im Spy/logfile ausgegeben wird

func void PrintDebugCh(int ch, string text);
// gibt nur den Text eines bestimmten Channels wieder, unabhängig von der Instanz ???

func void PrintDebugInst(string text);
// !!! gibt nur den Debugtext der eingeschalteten (SetDebugFocus --> siehe Intranetpage Erweitertes Skript-Debugging) Instanz aus ???

func void PrintDebugInstCh(int ch, string text);
// !!! gibt nur in einem bestimmten channel liegende Debuginfos einer Instanz aus ???

func int PrintDialog(int i0, string s1, int i2, int i3, string s4, int i5);

func void PrintMulti(string s0, string s1, string s2, string s3, string s4);
// Printbefehl, der aus den angegebenen Strings einen Auswählt und auf den Bildschirm schreibt

func void PrintScreen(int dialognr, string msg, int posx, int posy, string font, int timesec);
// Gibt den Text 'msg' auf dem Bildschrim aus und benutzt dabei den Font 'font'.
// Die Position ist für jede Koordinate eine Zahl zwischen 0 und 99 und gibt die prozentuale Position an.
// Der Ursprung befindet sich oben links (also 0% X und 0% Y)

func void Rtn_Exchange(string oldroutine, string newroutine);
// Tausche aktuellen Tagesablauf des NSC "self" gegen angegebenen aus
// (Name wird automatisch mit "RTN_" am Anfang und NSC-Skript-ID am Ende ergänzt)

func void SetPercentDone(int percentdone);

func int Snd_GetDistToSource(c_npc self);
// Liefert Entfernung ( in cm ! ) zum letzten logischen Sound

func int Snd_IsSourceItem(c_npc self);
// Check, ob Quelle des letzten Sound Item war (Return >0) und setzt "item" auf diesen Gegenstand

func int Snd_IsSourceNpc(c_npc self);
// Check, ob Quelle des letzten Sound NPC war (Return >0) und setzt "other" auf diesen NPC

func void Snd_Play(string s0);
// spielt einen Sound ab

func void Snd_Play3D(c_npc n0, string s1);
// spielt einen 3D-Sound ab.

func void TA(c_npc self, int start_h, int stop_h, func state, string waypoint);
// Mit _(Zustandsname) wird ein neuer Tagesablauf generiert, siehe TA.d

func void TA_BeginOverlay(c_npc self);
// Melde einen Overlay-Tagesablauf an

func void TA_CS(c_npc self, string csname, string rolename);
// Cutscene an den zuletzt angegebenen Tagesablaufpunkt hängen
// csName : Name der Cutscene ( der Name des "CS" - Files )
// roleName : Die Rolle die der NSC dabei übernehmen soll.

func void TA_EndOverlay(c_npc self);
// Beende einen Overlay-Tagesablauf

func void TA_Min(c_npc self, int start_h, int start_m, int stop_h, int stop_m, func state, string waypoint);
// Tagesablaufpunkt minutengenau angeben

func void TA_RemoveOverlay(c_npc self);
// Entferne aktiven TA-Overlay

func void Tal_Configure(int i0, int i1);

func void Update_ChoiceBox(string s0);

func void Wld_AssignRoomToGuild(string s0, int guild);
// Ordnet den Raum: 's0' der Gilde 'guild' zu

func void Wld_AssignRoomToNpc(string s0, c_npc roomowner);
// Ordnet den Raum: 's0' dem speziellen Nsc 'roomowner' zu

func int Wld_DetectItem(c_npc self, int flags);
// liefert eins zurück, wenn ein Item mit dem Entsprechende Flag (z.B.ITEM_KAT_FOOD )gefunden wurde
// Globale Variable 'item' wird mit dem gefundenen Gegenstand initialisiert

func int Wld_DetectNpc(c_npc self, int instance, func aistate, int guild);
// Diese Methode initilisiert die globale Skriptvariable "other" mit einem NSC, den "self" beim letzten Warnehmungscheck wargenommen hat.
// instance = Name der zu suchenden Instanz ( "-1" angeben, wenn Instanzname unwichtig )
// guild = Der zu suchende NSC muss Mitglied dieser Gilde sein ( "-1" angeben, wenn Gilde unwichtig )
// aiState = Der AI-Zustandsname, in dem sich der NSC befinden soll ( NOFUNC angeben, wenn AI-State unwichtig )
// Wenn die Methode einen entsprechenden NSC gefunden hat, liefert diese "1" und 'other' ist initialisiert
// ansonsten wird "0" geliefert und "other" wird nicht verändert.
// Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.

func int Wld_DetectNpcEx(c_npc self, int npcInstance, func aiState, int guild, int detectPlayer);
// Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.

func int Wld_DetectNpcExAtt(instance n0, int i1, func f2, int i3, int i4, int i5);

func int Wld_DetectPlayer(c_npc self);
// liefert eins zurück, wenn der Player in der Nähe ist

func void Wld_ExchangeGuildAttitudes(string name);
// Tausche Gilden-AttitudenTabelle aus.

func int Wld_GetDay();
// Liefert den aktuellen Tag zurück (Zähler), hierbei ist der StartTag (Gamestart) = 0.

func int Wld_GetFormerPlayerPortalGuild();
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert

func c_npc Wld_GetFormerPlayerPortalOwner();
// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'

func int Wld_GetGuildAttitude(int guild1, int guild2);
// Ermittle Gildenattitude

func int Wld_GetMobState(c_npc self, string schemename);
// Liefert den State zum nächsten Mob mit 'schemeName' zurück bzw. '-1' wenn kein solches Mob gefunden wurde

func int Wld_GetPlayerPortalGuild();
// liefert Gilde des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert

func c_npc Wld_GetPlayerPortalOwner();
// liefert den NSC des aktiven Raums, in dem sich der SC gerade befindet, zurück
// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'

func void Wld_InsertItem(int iteminstance, string spawnpoint);
// Füge Item in Welt ein entweder an einem WP oder einem FP
// Vorsicht, funktioniert nicht, Items werden immer im Mittelpunkt der Welt inserted

func void Wld_InsertNpc(int npcinstance, string spawnpoint);
// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf.

func void Wld_InsertNpcAndRespawn(int instance, string spawnpoint, float spawndelay);
// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf. Stirbt dieser NSC wird
// nach "spawnDelay"-Sekunden ein neuer NSC am Spawnpoint erzeugt.

func void Wld_InsertObject(string s0, string s1);

func int Wld_IsFPAvailable(c_npc self, string fpname);
// Sucht einen Freepoint im Umkreis von 20m vom NSC und liefert TRUE falls vorhanden und frei ('self' zählt als Blockierer nicht!) und sichtbar

func int Wld_IsMobAvailable(c_npc self, string schemename);
// Sucht sich ein Mobsi im Umkreis von 10m und liefert TRUE falls gefunden. MOB wird nur zurückgeliefert, wenn es nicht besetzt ist.

func int Wld_IsNextFPAvailable(c_npc self, string fpname);
// wie Wld_IsFPAvailable(), aber es wird immer der nahegelegenste genommen und 'self' zählt als Blockierer!

func int Wld_IsRaining();

func int Wld_IsTime(int hour1, int min1, int hour2, int min2);
// Liefert "1" wenn aktuelle Weltzeit zwischen den beiden angegebenen Zeiten liegt (von - bis)

func void Wld_PlayEffect(string effectinstance, int originvob, int targetvob, int effectlevel, int damage, int damagetype, int bisprojectile);
// effectInstance: Name der VisualFX-Instanz
// originVob: Ursprung/Verursacher (muss existieren!)
// targetVob: Ziel fuer Effekt + Schaden
// effectLevel: Bei Angabe von effectLevel wird nach einer VisualFX-Instanz mit dem Namen _L gesucht und verwendet falls sie
// gefunden wurde. (es muss trotzdem eine VisualFX-Instanz mit dem Namen definiert worden sein!)
// damage: Hoehe des Schadens
// damageType: Schadensart (DAM_Xxx)
// bIsProjectile: Effekt ist Projektil

func int Wld_RemoveItem(c_item item);
// Hiermit wird das angegebene Item aus der Welt entfernt und gelöscht

func void Wld_RemoveNpc(int i0);

func void Wld_SendTrigger(string vobname);
// Sendet eine Trigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.

func void Wld_SendUntrigger(string vobname);
// Sendet eine UnTrigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.

func void Wld_SetGuildAttitude(int guild1, int attitude, int guild2);
// Setze Gildenattitude neu

func void Wld_SetMobRoutine(int hour1, int min1, string objname, int state);
// _Alle_ Mobs mit diesem _Schemanamen_ werden getriggert.

func void Wld_SetObjectRoutine(int hour1, int min1, string objname, int state);
// _Ein_ Objekt mit diesem _Vobnamen_ wird getriggert

func void Wld_SetTime(int hour, int min);
// Setzt die Uhrzeit auf hour:min. hour kann größer als 23 sein, um zum nächsten Tag zu springen.

func void Wld_SpawnNpcRange(instance n0, int i1, int i2, float r3);

func void Wld_StopEffect(string s0);

