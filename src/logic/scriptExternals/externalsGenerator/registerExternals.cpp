/*
snd (5)
*/
	// int Snd_GetDistToSource(c_npc self);
	// Liefert Entfernung ( in cm ! ) zum letzten logischen Sound
	vm->registerExternalFunction("Snd_GetDistToSource", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Snd_GetDistToSource";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Snd_GetDistToSource(selfVob));
		vm.setReturn(0);
	});

	// bool Snd_IsSourceItem(c_npc self);
	// Check, ob Quelle des letzten Sound Item war (Return >0) und setzt "item" auf diesen Gegenstand
	vm->registerExternalFunction("Snd_IsSourceItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Snd_IsSourceItem";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Snd_IsSourceItem(selfVob));
		vm.setReturn(0);
	});

	// bool Snd_IsSourceNpc(c_npc self);
	// Check, ob Quelle des letzten Sound NPC war (Return >0) und setzt "other" auf diesen NPC
	vm->registerExternalFunction("Snd_IsSourceNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Snd_IsSourceNpc";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Snd_IsSourceNpc(selfVob));
		vm.setReturn(0);
	});

	// void Snd_Play(string s0);
	// spielt einen Sound ab
	vm->registerExternalFunction("Snd_Play", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Snd_Play";
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::Snd_Play(s0);
	});

	// void Snd_Play3D(c_npc n0, string s1);
	// spielt einen 3D-Sound ab.
	vm->registerExternalFunction("Snd_Play3D", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Snd_Play3D";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::Snd_Play3D(n0Vob, s1);
	});

/*
hlp (9)
*/
	// bool Hlp_CutscenePlayed(string csName);
	// Abfrage, ob Cutscene schon gespielt wurde (0 = Nein / 1 = Ja)
	vm->registerExternalFunction("Hlp_CutscenePlayed", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_CutscenePlayed";
		std::string csName = vm.popString(); LogInfo() << "csName: " << csName;
		vm.setReturn(Externals::Hlp_CutscenePlayed(csName));
		vm.setReturn(0);
	});

	// int Hlp_GetInstanceID(c_npc npc);
	// liefert die interne ID ( nicht Var aus der Instanz) zurück, um z.B. other mit self vergleichen zu können ( integer Vergleich)
	vm->registerExternalFunction("Hlp_GetInstanceID", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_GetInstanceID";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Hlp_GetInstanceID(npcVob));
		vm.setReturn(0);
	});

	// int Hlp_GetInstanceID2(c_item item);
	// liefert die interne ID ( nicht Var aus der Instanz) zurück, um zwei items miteinander vergleichen zu können ( integer Vergleich)
	vm->registerExternalFunction("Hlp_GetInstanceID2", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_GetInstanceID2";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		vm.setReturn(Externals::Hlp_GetInstanceID2(item));
		vm.setReturn(0);
	});

	// c_npc Hlp_GetNpc(int instanceName);
	// Ermittle einen NSC über den Instanznamen. Dieser kann einer Variablen zugewiesen werden und ist somit gezielt verfügbar
	vm->registerExternalFunction("Hlp_GetNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_GetNpc";
		int32_t instanceName = vm.popDataValue(); LogInfo() << "instanceName: " << instanceName;
		vm.setReturnVar(Externals::Hlp_GetNpc(instanceName));
		vm.setReturnVar(0);
	});

	// bool Hlp_IsItem(c_item item, int instanceName);
	// Prüft, ob der Gegenstand 'item' den Instanznamen 'instanceName' trägt. Will man z.B. testen,
	// ob die globale Variable item Pfeile sind (ItMuArrow) ruft man Hlp_IsItem(item,ItMuArrow) auf.
	// Gribt bei Gleichheit TRUE, sonst FALSE zurück.
	vm->registerExternalFunction("Hlp_IsItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_IsItem";
		int32_t instanceName = vm.popDataValue(); LogInfo() << "instanceName: " << instanceName;
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		vm.setReturn(Externals::Hlp_IsItem(item, instanceName));
		vm.setReturn(0);
	});

	// bool Hlp_IsValidItem(c_item item);
	// Prüfe ob Item-Instanz gültig und initialisiert ist True/False
	vm->registerExternalFunction("Hlp_IsValidItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_IsValidItem";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		vm.setReturn(Externals::Hlp_IsValidItem(item));
		vm.setReturn(0);
	});

	// bool Hlp_IsValidNpc(c_npc self);
	// Prüfe ob NSC-Instanz gültig und initialisiert ist. True/False
	vm->registerExternalFunction("Hlp_IsValidNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_IsValidNpc";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Hlp_IsValidNpc(selfVob));
		vm.setReturn(0);
	});

	// int Hlp_Random(int n0);
	// erzeugt einen Zufallswert (wobei n0 Obergrenze : [0..n0-1] ) Rückgabewert integer
	vm->registerExternalFunction("Hlp_Random", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_Random";
		int32_t n0 = vm.popDataValue(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Hlp_Random(n0));
		vm.setReturn(0);
	});

	// bool Hlp_StrCmp(string s1, string s2);
	// Hilfsfunktion um zwei Strings miteinander zu vergleichen liefert bei Gleichheit eins zurück
	vm->registerExternalFunction("Hlp_StrCmp", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Hlp_StrCmp";
		std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		vm.setReturn(Externals::Hlp_StrCmp(s1, s2));
		vm.setReturn(0);
	});

/*
mis (5)
*/
	// void Mis_AddMissionEntry(instance n0, string s1);
	vm->registerExternalFunction("Mis_AddMissionEntry", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mis_AddMissionEntry";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Mis_AddMissionEntry(n0, s1);
	});

	// int Mis_GetStatus(int missionName);
	// Liefert aktuellen Status einer Mission zurück ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc.
	vm->registerExternalFunction("Mis_GetStatus", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mis_GetStatus";
		int32_t missionName = vm.popDataValue(); LogInfo() << "missionName: " << missionName;
		vm.setReturn(Externals::Mis_GetStatus(missionName));
		vm.setReturn(0);
	});

	// bool Mis_OnTime(int missionName);
	// Liefert TRUE, wenn sich Spieler noch innerhalb des Zeitlimits für diese Mission befindet
	vm->registerExternalFunction("Mis_OnTime", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mis_OnTime";
		int32_t missionName = vm.popDataValue(); LogInfo() << "missionName: " << missionName;
		vm.setReturn(Externals::Mis_OnTime(missionName));
		vm.setReturn(0);
	});

	// void Mis_RemoveMission(instance n0);
	vm->registerExternalFunction("Mis_RemoveMission", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mis_RemoveMission";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Mis_RemoveMission(n0);
	});

	// void Mis_SetStatus(int missionName, int newStatus);
	// Setzt den Status einer Mission ( Bezogen auf den Spieler ) -> RUNNING, SUCCESS, FAILED etc. )
	vm->registerExternalFunction("Mis_SetStatus", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mis_SetStatus";
		int32_t newStatus = vm.popDataValue(); LogInfo() << "newStatus: " << newStatus;
		int32_t missionName = vm.popDataValue(); LogInfo() << "missionName: " << missionName;
		Externals::Mis_SetStatus(missionName, newStatus);
	});

/*
other (47)
*/
	// void Apply_Options_Audio();
	vm->registerExternalFunction("Apply_Options_Audio", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Apply_Options_Audio";
		Externals::Apply_Options_Audio();
	});

	// void Apply_Options_Controls();
	vm->registerExternalFunction("Apply_Options_Controls", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Apply_Options_Controls";
		Externals::Apply_Options_Controls();
	});

	// void Apply_Options_Game();
	vm->registerExternalFunction("Apply_Options_Game", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Apply_Options_Game";
		Externals::Apply_Options_Game();
	});

	// void Apply_Options_Performance();
	vm->registerExternalFunction("Apply_Options_Performance", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Apply_Options_Performance";
		Externals::Apply_Options_Performance();
	});

	// void Apply_Options_Video();
	vm->registerExternalFunction("Apply_Options_Video", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Apply_Options_Video";
		Externals::Apply_Options_Video();
	});

	// string ConcatStrings(string str1, string str2);
	// Erzeugt eine Kopie von dem ersten angegebenen String, hängt den zweiten an diesen an und gibt den neuen String zurück
	vm->registerExternalFunction("ConcatStrings", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "ConcatStrings";
		std::string str2 = vm.popString(); LogInfo() << "str2: " << str2;
		std::string str1 = vm.popString(); LogInfo() << "str1: " << str1;
		vm.setReturn(Externals::ConcatStrings(str1, str2));
		vm.setReturn();
	});

	// void CreateInvItem(c_npc n0, int n1);
	// gibt der angegebenen Instanz (self oder other) ein Item ins Inventory
	vm->registerExternalFunction("CreateInvItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "CreateInvItem";
		int32_t n1 = vm.popDataValue(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::CreateInvItem(n0Vob, n1);
	});

	// void CreateInvItems(c_npc n0, int n1, int n2);
	// wie vorherige Funktion, nur das Multi-Items (z.B.: 100 Pfeile) ins Inventory kreiert werden
	vm->registerExternalFunction("CreateInvItems", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "CreateInvItems";
		int32_t n2 = vm.popDataValue(); LogInfo() << "n2: " << n2;
		int32_t n1 = vm.popDataValue(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::CreateInvItems(n0Vob, n1, n2);
	});

	// void EquipItem(c_npc n0, int n1);
	// gibt dem Nsc direkt das angegebene Item an den Gürtel
	vm->registerExternalFunction("EquipItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "EquipItem";
		int32_t n1 = vm.popDataValue(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::EquipItem(n0Vob, n1);
	});

	// void ExitGame();
	// Gothic 2 only function
	// Beendet Gothic.
	vm->registerExternalFunction("ExitGame", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "ExitGame";
		Externals::ExitGame();
	});

	// void ExitSession();
	// Gothic 2 only function
	// Beendet das laufende Spiel.
	vm->registerExternalFunction("ExitSession", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "ExitSession";
		Externals::ExitSession();
	});

	// int FloatToInt(float x);
	// !!! konvertiert einen float in einen int, hier wäre es noch ganz schön zu wissen, wie mit dem float umgesprungen wird --> schneiden oder runden ???
	vm->registerExternalFunction("FloatToInt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "FloatToInt";
		float x = vm.popFloatValue(); LogInfo() << "x: " << x;
		vm.setReturn(Externals::FloatToInt(x));
		vm.setReturn(0);
	});

	// string FloatToString(float r0);
	vm->registerExternalFunction("FloatToString", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "FloatToString";
		float r0 = vm.popFloatValue(); LogInfo() << "r0: " << r0;
		vm.setReturn(Externals::FloatToString(r0));
		vm.setReturn();
	});

	// void Game_InitEnglish();
	// Gothic 2 only function
	// Setzt die interne Engine-Variablen auf English.
	//
	//      [result]        - Zeigt an, ob die Sprache erfolgreich geladen wurde.
	vm->registerExternalFunction("Game_InitEnglish", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Game_InitEnglish";
		Externals::Game_InitEnglish();
	});

	// void Game_InitGerman();
	// Gothic 2 only function
	// Setzt die interne Engine-Variablen auf Deutsch.
	//
	//      [result]        - Zeigt an, ob die Sprache erfolgreich geladen wurde.
	vm->registerExternalFunction("Game_InitGerman", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Game_InitGerman";
		Externals::Game_InitGerman();
	});

	// void Info_AddChoice(int i0, string s1, func f2);
	vm->registerExternalFunction("Info_AddChoice", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Info_AddChoice";
		uint32_t f2 = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "f2: " << f2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t i0 = vm.popDataValue(); LogInfo() << "i0: " << i0;
		Externals::Info_AddChoice(i0, s1, f2);
	});

	// void Info_ClearChoices(int i0);
	vm->registerExternalFunction("Info_ClearChoices", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Info_ClearChoices";
		int32_t i0 = vm.popDataValue(); LogInfo() << "i0: " << i0;
		Externals::Info_ClearChoices(i0);
	});

	// bool InfoManager_HasFinished();
	vm->registerExternalFunction("InfoManager_HasFinished", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "InfoManager_HasFinished";
		vm.setReturn(Externals::InfoManager_HasFinished());
		vm.setReturn(0);
	});

	// void IntroduceChapter(string titel, string untertitel, string texture, string sound, int waitTime);
	// Gothic 2 only function
	// Zeigt den Kapitelwechsel-Bildschirm an.
	//
	//      Titel           - Text des Titels
	//      Untertitel      - text des Untertitels
	//      Texture         - Dateiname der Hintergrundtextur
	//      Sound           - Dateiname des abgespielten Sounds
	//      WaitTime        - Anzahl in Millisekunden, die der Bildschirm angezeigt wird
	vm->registerExternalFunction("IntroduceChapter", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "IntroduceChapter";
		int32_t waitTime = vm.popDataValue(); LogInfo() << "waitTime: " << waitTime;
		std::string sound = vm.popString(); LogInfo() << "sound: " << sound;
		std::string texture = vm.popString(); LogInfo() << "texture: " << texture;
		std::string untertitel = vm.popString(); LogInfo() << "untertitel: " << untertitel;
		std::string titel = vm.popString(); LogInfo() << "titel: " << titel;
		Externals::IntroduceChapter(titel, untertitel, texture, sound, waitTime);
	});

	// float IntToFloat(int x);
	// !!! wie float to int, nur daß ein integer in float umgewandelt wird ???
	vm->registerExternalFunction("IntToFloat", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "IntToFloat";
		int32_t x = vm.popDataValue(); LogInfo() << "x: " << x;
		vm.setReturn(Externals::IntToFloat(x));
		vm.setReturn(0.0f);
	});

	// string IntToString(int x);
	// !!!Funktion überträgt eine Integer variable in eine Stringvariable (Rückgabewert : string)???
	vm->registerExternalFunction("IntToString", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "IntToString";
		int32_t x = vm.popDataValue(); LogInfo() << "x: " << x;
		vm.setReturn(Externals::IntToString(x));
		vm.setReturn();
	});

	// void Log_AddEntry(string topic, string entry);
	//      Der Befehl fügt dem Topics topic ein neues Entrz namens entry hinzu. Wie auch bei der Erzeugung von Topics dient hier der
	//      Name sowohl zur Identifikation als auch zur Anzeige und muss somit eindeutig sein. Doppelte Einträge werden auch hier ignoriert.
	//
	//      Parameter
	//      topic           Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
	//      entry           Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Eintrags verwendet wird.
	vm->registerExternalFunction("Log_AddEntry", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Log_AddEntry";
		std::string entry = vm.popString(); LogInfo() << "entry: " << entry;
		std::string topic = vm.popString(); LogInfo() << "topic: " << topic;
		Externals::Log_AddEntry(topic, entry);
	});

	// void Log_CreateTopic(string name, int section);
	//      Der Befehl fügt unter der Sektion 'section' ein neues Topic mit Namen 'name' ein.
	//      Sollte bereits ein Topic dieses Names in irgendeiner Sektion existieren, wird das Topic nicht hinzugefügt.
	//
	//      Parameter:
	//      - name          Eine eindeutige Zeichenkette, die sowohl zur Identifikation als auch zur Anzeige des Topics verwendet wird.
	//      - section       Eine, in der Constants.d definierte Konstante, die besagt, in welcher Sektion das Topic angezeigt werden soll.
	//                      Die Konstante kann folgende Werte annehmen:
	//                          - LOG_MISSION
	//                          - LOG_NOTE
	vm->registerExternalFunction("Log_CreateTopic", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Log_CreateTopic";
		int32_t section = vm.popDataValue(); LogInfo() << "section: " << section;
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		Externals::Log_CreateTopic(name, section);
	});

	// void Log_SetTopicStatus(string name, int status);
	//      Der Befehl ändert den Status des Topics 'name' in 'status'.
	//      Er sollte nur für Topics verwendet werden, die für die Sektion LOG_MISSION erzeugt wurden.
	//
	//      Parameter:
	//      - name          Diejenige Zeichenkette, die bei der Erstellung des Topics per Log_CreateTopic() angegeben wurde.
	//      - status        Eine, in der Constants.d definierte Konstante, die besagt, unter welchem Status die Mission dargestellt werden soll.
	//                      Folgende Werte sind möglich:
	//                              - LOG_RUNNING
	//                              - LOG_SUCCESS
	//                              - LOG_FAILED
	//                              - LOG_OBSOLETE
	vm->registerExternalFunction("Log_SetTopicStatus", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Log_SetTopicStatus";
		int32_t status = vm.popDataValue(); LogInfo() << "status: " << status;
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		Externals::Log_SetTopicStatus(name, status);
	});

	// void Mob_CreateItems(string mobName, int itemInstance, int amount);
	// Erzeuge "amount" Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen.
	vm->registerExternalFunction("Mob_CreateItems", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mob_CreateItems";
		int32_t amount = vm.popDataValue(); LogInfo() << "amount: " << amount;
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		std::string mobName = vm.popString(); LogInfo() << "mobName: " << mobName;
		Externals::Mob_CreateItems(mobName, itemInstance, amount);
	});

	// int Mob_HasItems(string mobName, int itemInstance);
	// Liefere Anzahl der Items der Instanz "itemInstance" in oCMobContainer mit angegebenen Vobnamen
	vm->registerExternalFunction("Mob_HasItems", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mob_HasItems";
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		std::string mobName = vm.popString(); LogInfo() << "mobName: " << mobName;
		vm.setReturn(Externals::Mob_HasItems(mobName, itemInstance));
		vm.setReturn(0);
	});

	// void Perc_SetRange(int percID, int range);
	// Setze Reichweite fuer eine passive Wahrnehmung int cm
	vm->registerExternalFunction("Perc_SetRange", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Perc_SetRange";
		int32_t range = vm.popDataValue(); LogInfo() << "range: " << range;
		int32_t percID = vm.popDataValue(); LogInfo() << "percID: " << percID;
		Externals::Perc_SetRange(percID, range);
	});

	// bool PlayVideo(string filename);
	// Gothic 2 only function
	// Spielt eine Videodatei ab.
	//
	//      Filename        - Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
	//      [result]        - Boolean ob erfolgreich abgespielt
	vm->registerExternalFunction("PlayVideo", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PlayVideo";
		std::string filename = vm.popString(); LogInfo() << "filename: " << filename;
		vm.setReturn(Externals::PlayVideo(filename));
		vm.setReturn(0);
	});

	// bool PlayVideoEx(string filename, int screenBlend, int exitSession);
	// Gothic 2 only function
	// Spielt eine Videodatei mit erweiterten Optionen ab.
	//
	//      Filename        - Dateiname des Videos (mit Dateierweiterung, relativ zu [VIDEOS]\ )
	//      ScreenBlend     - Boolean ob Effekt 'BLACK_SCREEN' danach abgespielt wird (nicht bei ExitSession)
	//      ExitSession     - Boolean ob nach dem Video die Session beendet wird
	//      [result]        - Boolean ob erfolgreich abgespielt, bei ExitSession immer erfolgreich
	vm->registerExternalFunction("PlayVideoEx", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PlayVideoEx";
		int32_t exitSession = vm.popDataValue(); LogInfo() << "exitSession: " << exitSession;
		int32_t screenBlend = vm.popDataValue(); LogInfo() << "screenBlend: " << screenBlend;
		std::string filename = vm.popString(); LogInfo() << "filename: " << filename;
		vm.setReturn(Externals::PlayVideoEx(filename, screenBlend, exitSession));
		vm.setReturn(0);
	});

	// void Print(string s0);
	// Ausgabebefehl, der Text ins Game schreibt (wie OutputunitTexte)
	vm->registerExternalFunction("Print", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Print";
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::Print(s0);
	});

	// void PrintDebug(string s);
	// Printausgabe, die nur bei eingeschaltetem Debugmodus (Alt+D) im Spy/logfile ausgegeben wird
	vm->registerExternalFunction("PrintDebug", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintDebug";
		std::string s = vm.popString(); LogInfo() << "s: " << s;
		Externals::PrintDebug(s);
	});

	// void PrintDebugCh(int ch, string text);
	// gibt nur den Text eines bestimmten Channels wieder, unabhängig von der Instanz
	vm->registerExternalFunction("PrintDebugCh", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintDebugCh";
		std::string text = vm.popString(); LogInfo() << "text: " << text;
		int32_t ch = vm.popDataValue(); LogInfo() << "ch: " << ch;
		Externals::PrintDebugCh(ch, text);
	});

	// void PrintDebugInst(string text);
	// gibt nur den Debugtext der eingeschalteten (SetDebugFocus --> siehe Intranetpage Erweitertes Skript-Debugging) Instanz aus
	vm->registerExternalFunction("PrintDebugInst", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintDebugInst";
		std::string text = vm.popString(); LogInfo() << "text: " << text;
		Externals::PrintDebugInst(text);
	});

	// void PrintDebugInstCh(int ch, string text);
	// gibt nur in einem bestimmten channel liegende Debuginfos einer Instanz aus
	vm->registerExternalFunction("PrintDebugInstCh", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintDebugInstCh";
		std::string text = vm.popString(); LogInfo() << "text: " << text;
		int32_t ch = vm.popDataValue(); LogInfo() << "ch: " << ch;
		Externals::PrintDebugInstCh(ch, text);
	});

	// int PrintDialog(int i0, string s1, int i2, int i3, string s4, int i5);
	vm->registerExternalFunction("PrintDialog", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintDialog";
		int32_t i5 = vm.popDataValue(); LogInfo() << "i5: " << i5;
		std::string s4 = vm.popString(); LogInfo() << "s4: " << s4;
		int32_t i3 = vm.popDataValue(); LogInfo() << "i3: " << i3;
		int32_t i2 = vm.popDataValue(); LogInfo() << "i2: " << i2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t i0 = vm.popDataValue(); LogInfo() << "i0: " << i0;
		vm.setReturn(Externals::PrintDialog(i0, s1, i2, i3, s4, i5));
		vm.setReturn(0);
	});

	// void PrintMulti(string s0, string s1, string s2, string s3, string s4);
	// Printbefehl, der aus den angegebenen Strings einen Auswählt und auf den Bildschirm schreibt
	vm->registerExternalFunction("PrintMulti", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintMulti";
		std::string s4 = vm.popString(); LogInfo() << "s4: " << s4;
		std::string s3 = vm.popString(); LogInfo() << "s3: " << s3;
		std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::PrintMulti(s0, s1, s2, s3, s4);
	});

	// void PrintScreen(int dialogNr, string msg, int posx, int posy, string font, int timeSec);
	// Gibt den Text 'msg' auf dem Bildschrim aus und benutzt dabei den Font 'font'.
	// Die Position ist für jede Koordinate eine Zahl zwischen 0 und 99 und gibt die prozentuale Position an.
	// Der Ursprung befindet sich oben links (also 0% X und 0% Y)
	vm->registerExternalFunction("PrintScreen", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "PrintScreen";
		int32_t timeSec = vm.popDataValue(); LogInfo() << "timeSec: " << timeSec;
		std::string font = vm.popString(); LogInfo() << "font: " << font;
		int32_t posy = vm.popDataValue(); LogInfo() << "posy: " << posy;
		int32_t posx = vm.popDataValue(); LogInfo() << "posx: " << posx;
		std::string msg = vm.popString(); LogInfo() << "msg: " << msg;
		int32_t dialogNr = vm.popDataValue(); LogInfo() << "dialogNr: " << dialogNr;
		Externals::PrintScreen(dialogNr, msg, posx, posy, font, timeSec);
	});

	// void Rtn_Exchange(string oldRoutine, string newRoutine);
	// Tausche aktuellen Tagesablauf des NSC "self" gegen angegebenen aus
	// (Name wird automatisch mit "RTN_" am Anfang und NSC-Skript-ID am Ende ergänzt)
	vm->registerExternalFunction("Rtn_Exchange", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Rtn_Exchange";
		std::string newRoutine = vm.popString(); LogInfo() << "newRoutine: " << newRoutine;
		std::string oldRoutine = vm.popString(); LogInfo() << "oldRoutine: " << oldRoutine;
		Externals::Rtn_Exchange(oldRoutine, newRoutine);
	});

	// void SetPercentDone(int percentDone);
	// Gothic 2 only function
	// Setz die Fortschrittsanzeige im Ladebalken.
	//
	//      PercentDone     - Aktueller Fortschritt des Ladevorgangs in Prozent (0-100)
	vm->registerExternalFunction("SetPercentDone", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "SetPercentDone";
		int32_t percentDone = vm.popDataValue(); LogInfo() << "percentDone: " << percentDone;
		Externals::SetPercentDone(percentDone);
	});

	// void TA(c_npc self, int start_h, int stop_h, func state, string waypoint);
	// Mit _(Zustandsname) wird ein neuer Tagesablauf generiert, siehe TA.d
	vm->registerExternalFunction("TA", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "TA";
		std::string waypoint = vm.popString(); LogInfo() << "waypoint: " << waypoint;
		uint32_t state = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "state: " << state;
		int32_t stop_h = vm.popDataValue(); LogInfo() << "stop_h: " << stop_h;
		int32_t start_h = vm.popDataValue(); LogInfo() << "start_h: " << start_h;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::TA(selfVob, start_h, stop_h, state, waypoint);
	});

	// void TA_BeginOverlay(c_npc self);
	// Melde einen Overlay-Tagesablauf an
	vm->registerExternalFunction("TA_BeginOverlay", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "TA_BeginOverlay";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::TA_BeginOverlay(selfVob);
	});

	// void TA_CS(c_npc self, string csName, string roleName);
	// Cutscene an den zuletzt angegebenen Tagesablaufpunkt hängen
	// csName:      Name der Cutscene ( der Name des "CS" - Files )
	// roleName:    Die Rolle die der NSC dabei übernehmen soll.
	vm->registerExternalFunction("TA_CS", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "TA_CS";
		std::string roleName = vm.popString(); LogInfo() << "roleName: " << roleName;
		std::string csName = vm.popString(); LogInfo() << "csName: " << csName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::TA_CS(selfVob, csName, roleName);
	});

	// void TA_EndOverlay(c_npc self);
	// Beende einen Overlay-Tagesablauf
	vm->registerExternalFunction("TA_EndOverlay", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "TA_EndOverlay";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::TA_EndOverlay(selfVob);
	});

	// void TA_Min(c_npc self, int start_h, int start_m, int stop_h, int stop_m, func state, string waypoint);
	// Tagesablaufpunkt minutengenau angeben
	vm->registerExternalFunction("TA_Min", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "TA_Min";
		std::string waypoint = vm.popString(); LogInfo() << "waypoint: " << waypoint;
		uint32_t state = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "state: " << state;
		int32_t stop_m = vm.popDataValue(); LogInfo() << "stop_m: " << stop_m;
		int32_t stop_h = vm.popDataValue(); LogInfo() << "stop_h: " << stop_h;
		int32_t start_m = vm.popDataValue(); LogInfo() << "start_m: " << start_m;
		int32_t start_h = vm.popDataValue(); LogInfo() << "start_h: " << start_h;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::TA_Min(selfVob, start_h, start_m, stop_h, stop_m, state, waypoint);
	});

	// void TA_RemoveOverlay(c_npc self);
	// Entferne aktiven TA-Overlay
	vm->registerExternalFunction("TA_RemoveOverlay", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "TA_RemoveOverlay";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::TA_RemoveOverlay(selfVob);
	});

	// void Tal_Configure(int i0, int i1);
	vm->registerExternalFunction("Tal_Configure", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Tal_Configure";
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t i0 = vm.popDataValue(); LogInfo() << "i0: " << i0;
		Externals::Tal_Configure(i0, i1);
	});

	// void Update_ChoiceBox(string s0);
	vm->registerExternalFunction("Update_ChoiceBox", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Update_ChoiceBox";
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::Update_ChoiceBox(s0);
	});

/*
mdl (11)
*/
	// void Mdl_ApplyOverlayMds(c_npc n0, string s1);
	// mit diesem Befehl werden Animationen auf einem höheren Layer gestartet
	// (z.B. hat der Zombie ein Overlay über die normalen Human-Animationen)
	vm->registerExternalFunction("Mdl_ApplyOverlayMds", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_ApplyOverlayMds";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::Mdl_ApplyOverlayMds(n0Vob, s1);
	});

	// void Mdl_ApplyOverlayMDSTimed(c_npc self, string overlayname, float timeTicks);
	// Overlay-MDS wird gestartet und automatisch nach der angegebenen Zeit abgeschaltet
	vm->registerExternalFunction("Mdl_ApplyOverlayMDSTimed", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_ApplyOverlayMDSTimed";
		float timeTicks = vm.popFloatValue(); LogInfo() << "timeTicks: " << timeTicks;
		std::string overlayname = vm.popString(); LogInfo() << "overlayname: " << overlayname;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Mdl_ApplyOverlayMDSTimed(selfVob, overlayname, timeTicks);
	});

	// void Mdl_ApplyRandomAni(c_npc n0, string s1, string s2);
	// Fügt Zufallsanimationen (am Kopf kratzen, an den Sack packen etc (s2)) für bestimmte Animationszustände (s1) ein
	vm->registerExternalFunction("Mdl_ApplyRandomAni", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_ApplyRandomAni";
		std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::Mdl_ApplyRandomAni(n0Vob, s1, s2);
	});

	// void Mdl_ApplyRandomAniFreq(c_npc n0, string s1, float f2);
	// hiermit kann die Frequenz betimmt werden, wie oft die für den Animationszustand (s1) deklarierten Randomanis abgespielt werden
	vm->registerExternalFunction("Mdl_ApplyRandomAniFreq", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_ApplyRandomAniFreq";
		float f2 = vm.popFloatValue(); LogInfo() << "f2: " << f2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::Mdl_ApplyRandomAniFreq(n0Vob, s1, f2);
	});

	// void Mdl_ApplyRandomFaceAni(c_npc self, string name, float timeMin, float timeMinVar, float timeMax, float timeMaxVar, float probMin);
	// Starte zufällige Gesichtsanimation
	// Mdl_ApplyRandomFaceAni ( self, ANINAME, minTime, minTimeVar, maxTime, maxTimeVar, probMin)
	// minTime      = Minimum an Zeit nachdem Ani startet (in Sekunden)
	// maxTime      = Maximum an Zeit nachdem Ani startet (in Sekunden)
	// minTimeVar   = Abweichung von minTime (in Sekunden)
	// maxTimeVar   = Abweichung von maxTime (in Sekunden)
	// probMin      = Wahrscheinlichkeit [0..1] ob obere oder untere Grenze der Abweichung benutzt wird
	vm->registerExternalFunction("Mdl_ApplyRandomFaceAni", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_ApplyRandomFaceAni";
		float probMin = vm.popFloatValue(); LogInfo() << "probMin: " << probMin;
		float timeMaxVar = vm.popFloatValue(); LogInfo() << "timeMaxVar: " << timeMaxVar;
		float timeMax = vm.popFloatValue(); LogInfo() << "timeMax: " << timeMax;
		float timeMinVar = vm.popFloatValue(); LogInfo() << "timeMinVar: " << timeMinVar;
		float timeMin = vm.popFloatValue(); LogInfo() << "timeMin: " << timeMin;
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Mdl_ApplyRandomFaceAni(selfVob, name, timeMin, timeMinVar, timeMax, timeMaxVar, probMin);
	});

	// void Mdl_RemoveOverlayMDS(c_npc self, string overlayName);
	// Entferne ein Overlay-MDS
	vm->registerExternalFunction("Mdl_RemoveOverlayMDS", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_RemoveOverlayMDS";
		std::string overlayName = vm.popString(); LogInfo() << "overlayName: " << overlayName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Mdl_RemoveOverlayMDS(selfVob, overlayName);
	});

	// void Mdl_SetModelFatness(c_npc self, float fatness);
	// Setzt Model-Z-Skalierung
	vm->registerExternalFunction("Mdl_SetModelFatness", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_SetModelFatness";
		float fatness = vm.popFloatValue(); LogInfo() << "fatness: " << fatness;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Mdl_SetModelFatness(selfVob, fatness);
	});

	// void Mdl_SetModelScale(c_npc self, float x, float y, float z);
	// zum skalieren des Meshes (Breite,Höhe,Tiefe) 1 = 100%, also Normalgröße!
	vm->registerExternalFunction("Mdl_SetModelScale", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_SetModelScale";
		float z = vm.popFloatValue(); LogInfo() << "z: " << z;
		float y = vm.popFloatValue(); LogInfo() << "y: " << y;
		float x = vm.popFloatValue(); LogInfo() << "x: " << x;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Mdl_SetModelScale(selfVob, x, y, z);
	});

	// void Mdl_SetVisual(instance n0, string s1);
	vm->registerExternalFunction("Mdl_SetVisual", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_SetVisual";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Mdl_SetVisual(n0, s1);
	});

	// void Mdl_SetVisualBody(instance n0, string s1, int i2, int i3, string s4, int i5, int i6, int i7);
	vm->registerExternalFunction("Mdl_SetVisualBody", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_SetVisualBody";
		int32_t i7 = vm.popDataValue(); LogInfo() << "i7: " << i7;
		int32_t i6 = vm.popDataValue(); LogInfo() << "i6: " << i6;
		int32_t i5 = vm.popDataValue(); LogInfo() << "i5: " << i5;
		std::string s4 = vm.popString(); LogInfo() << "s4: " << s4;
		int32_t i3 = vm.popDataValue(); LogInfo() << "i3: " << i3;
		int32_t i2 = vm.popDataValue(); LogInfo() << "i2: " << i2;
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Mdl_SetVisualBody(n0, s1, i2, i3, s4, i5, i6, i7);
	});

	// void Mdl_StartFaceAni(c_npc self, string name, float intensity, float holdTime);
	// Starte Gesichtsanimation // intensity 1 = 100% // holdTime -1 = forever
	vm->registerExternalFunction("Mdl_StartFaceAni", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Mdl_StartFaceAni";
		float holdTime = vm.popFloatValue(); LogInfo() << "holdTime: " << holdTime;
		float intensity = vm.popFloatValue(); LogInfo() << "intensity: " << intensity;
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Mdl_StartFaceAni(selfVob, name, intensity, holdTime);
	});

/*
doc (15)
*/
	// int Doc_Create();
	// G1: erstellet ein Dokument und liefert ein Handle zurück, daß für alle weiteren Doc_...-Befehle nötig ist
	// Erzeugt ein Dokument.
	//
	//      [result]        - Handle auf das Dokument (-1 = konnte nicht erzeugt werden)
	vm->registerExternalFunction("Doc_Create", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_Create";
		vm.setReturn(Externals::Doc_Create());
		vm.setReturn(0);
	});

	// int Doc_CreateMap();
	// Gothic 2 only function
	// Erzeugt ein Dokument (Karte).
	//
	//      [result]        - Handle auf das Dokument (-1 = konnte nicht erzeugt werden)
	vm->registerExternalFunction("Doc_CreateMap", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_CreateMap";
		vm.setReturn(Externals::Doc_CreateMap());
		vm.setReturn(0);
	});

	// void Doc_Font(string fontName);
	// Gothic 1 only function
	// Benutzt den angegebenen Font für nachfolgende "Doc_Print"s.
	vm->registerExternalFunction("Doc_Font", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_Font";
		std::string fontName = vm.popString(); LogInfo() << "fontName: " << fontName;
		Externals::Doc_Font(fontName);
	});

	// void Doc_MapCoordinates(string levelName, float gameX1, float gameY1, float pixelX1, float pixelY1, float gameX2, float gameY2, float pixelX2, float pixelY2);
	// Gothic 1 only function
	// Karte initialisieren : levelName = Name des Levels (.ZEN) aus dem die Karte einen Ausschnitt zeigt
	// gamex1,gamey1 : Koordinaten im Spiel (linke obere Ecke der Karte)
	// pixelx1,pixely1 : PixelKoordinaten linke obere Ecke // der Rest : alles nochmal für die rechte untere Ecke
	vm->registerExternalFunction("Doc_MapCoordinates", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_MapCoordinates";
		float pixelY2 = vm.popFloatValue(); LogInfo() << "pixelY2: " << pixelY2;
		float pixelX2 = vm.popFloatValue(); LogInfo() << "pixelX2: " << pixelX2;
		float gameY2 = vm.popFloatValue(); LogInfo() << "gameY2: " << gameY2;
		float gameX2 = vm.popFloatValue(); LogInfo() << "gameX2: " << gameX2;
		float pixelY1 = vm.popFloatValue(); LogInfo() << "pixelY1: " << pixelY1;
		float pixelX1 = vm.popFloatValue(); LogInfo() << "pixelX1: " << pixelX1;
		float gameY1 = vm.popFloatValue(); LogInfo() << "gameY1: " << gameY1;
		float gameX1 = vm.popFloatValue(); LogInfo() << "gameX1: " << gameX1;
		std::string levelName = vm.popString(); LogInfo() << "levelName: " << levelName;
		Externals::Doc_MapCoordinates(levelName, gameX1, gameY1, pixelX1, pixelY1, gameX2, gameY2, pixelX2, pixelY2);
	});

	// void Doc_Open(string texture);
	// Gothic 1 only function
	// Öffnet ein Dokument mit dem Bild "texture" als Hintergrund
	vm->registerExternalFunction("Doc_Open", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_Open";
		std::string texture = vm.popString(); LogInfo() << "texture: " << texture;
		Externals::Doc_Open(texture);
	});

	// void Doc_Print(string text);
	// Gothic 1 only function
	// Angegebener Text wird in das aktuelle (vorher mit Doc_Open geöffnete) Dokument geschrieben
	vm->registerExternalFunction("Doc_Print", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_Print";
		std::string text = vm.popString(); LogInfo() << "text: " << text;
		Externals::Doc_Print(text);
	});

	// void Doc_PrintLine(int document, int page, string text);
	// G1: Gibt Text für die Seite 'page' an. Der Text wird NICHT umgebrochen
	// Setzt den Text einer Seite. Der Text wird _nicht_ umgebrochen.
	//
	//      Document        - Handle auf das Dokument
	//      Page            - Index der Seite (beginnt mit 0)
	//      Text            - Textzeile
	vm->registerExternalFunction("Doc_PrintLine", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_PrintLine";
		std::string text = vm.popString(); LogInfo() << "text: " << text;
		int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_PrintLine(document, page, text);
	});

	// void Doc_PrintLines(int document, int page, string text);
	// G1: Gibt Text für die Seite 'page' an. Der Text WIRD umgebrochen, falls er nicht auf die in Doc_SetPage() angegebene Grafik passt.
	// Setzt den Text einer Seite. Der Text wird umgebrochen falls notwendig.
	//
	//      Document        - Handle auf das Dokument
	//      Page            - Index der Seite (beginnt mit 0)
	//      Text            - Textzeilen
	vm->registerExternalFunction("Doc_PrintLines", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_PrintLines";
		std::string text = vm.popString(); LogInfo() << "text: " << text;
		int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_PrintLines(document, page, text);
	});

	// void Doc_SetFont(int document, int page, string font);
	// G1: setzt den Font für die angegebene Seite 'page' oder für das ganze Dokument ('page':-1)
	// G1: Fontname z.B. "Scrolls.tga"
	// Setzt den Font einer/aller Seite(n).
	//
	//      Document        - Handle auf das Dokument
	//      Page            - Index der Seite (beginnt mit 0, -1 = alle)
	//      Font            - Dateiname der Font-Textur (ohne Pfad - z.B. 'Font_Default.tga')
	vm->registerExternalFunction("Doc_SetFont", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_SetFont";
		std::string font = vm.popString(); LogInfo() << "font: " << font;
		int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_SetFont(document, page, font);
	});

	// void Doc_SetLevel(int document, string level);
	// Gothic 2 only function
	// Setzt das Level-ZEN zur Anzeige der Spieler-Position (Karten).
	//
	// (als Grenze gilt die BBox des statischen Level-Meshes)
	//      Document        - Handle auf das Dokument
	//      Level           - Dateiname des Level-ZENs (mit Pfad ab WORLDS - z.B. 'NewWorld\NewWorld.zen')
	vm->registerExternalFunction("Doc_SetLevel", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_SetLevel";
		std::string level = vm.popString(); LogInfo() << "level: " << level;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_SetLevel(document, level);
	});

	// void Doc_SetLevelCoords(int document, int left, int top, int right, int bottom);
	// Gothic 2 only function
	// Setzt die Grenzen des Level-Bereichs, der durch die Textur abgedeckt wird (Karten).
	//
	// (als Grenzen werden _absolute_ Level-Koordinaten angegeben - z.B.: '..., -28000, 50500, 95500, -42500')
	//      Document        - Handle auf das Dokument
	//      Left            - linke Grenze
	//      Top                     - obere Grenze
	//      Right           - rechte Grenze
	//      Bottom          - untere Grenze
	vm->registerExternalFunction("Doc_SetLevelCoords", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_SetLevelCoords";
		int32_t bottom = vm.popDataValue(); LogInfo() << "bottom: " << bottom;
		int32_t right = vm.popDataValue(); LogInfo() << "right: " << right;
		int32_t top = vm.popDataValue(); LogInfo() << "top: " << top;
		int32_t left = vm.popDataValue(); LogInfo() << "left: " << left;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_SetLevelCoords(document, left, top, right, bottom);
	});

	// void Doc_SetMargins(int document, int page, int left, int top, int right, int bottom, int pixels);
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
	vm->registerExternalFunction("Doc_SetMargins", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_SetMargins";
		int32_t pixels = vm.popDataValue(); LogInfo() << "pixels: " << pixels;
		int32_t bottom = vm.popDataValue(); LogInfo() << "bottom: " << bottom;
		int32_t right = vm.popDataValue(); LogInfo() << "right: " << right;
		int32_t top = vm.popDataValue(); LogInfo() << "top: " << top;
		int32_t left = vm.popDataValue(); LogInfo() << "left: " << left;
		int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_SetMargins(document, page, left, top, right, bottom, pixels);
	});

	// void Doc_SetPage(int document, int page, string texture, int scale);
	// G1: setzt für die Seite 'page' die Grafik-Datei 'pageimage'. Diese muß im TGA-Format vorliegen und
	// G1: z.B. "aufgeschlagenesBuch_links.tga"  heißen
	// G1: wird bei 'page' -1 übergeben, so bezieht sich der Befehl auf ALLE Seiten des Dokuments
	// Setzt die Hintergrund-Textur der Seite eines Dokuments.
	//
	//      Document        - Handle auf das Dokument
	//      Page            - Index der Seite (beginnt mit 0, -1 = alle)
	//      Scale           - Flag ob Textur gestreckt werden soll
	vm->registerExternalFunction("Doc_SetPage", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_SetPage";
		int32_t scale = vm.popDataValue(); LogInfo() << "scale: " << scale;
		std::string texture = vm.popString(); LogInfo() << "texture: " << texture;
		int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_SetPage(document, page, texture, scale);
	});

	// void Doc_SetPages(int document, int count);
	// G1: setzt die Anzahl der Seiten dieses Dokuments. Bei mehr als zwei Seiten wird versucht ALLE
	// G1: Seiten auf den Bildschirm zu bekommen
	// Setzt die Anzahl der Seiten eines Dokuments.
	//
	//      Document        - Handle auf das Dokument
	//      Count           - Anzahl der Seiten
	vm->registerExternalFunction("Doc_SetPages", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_SetPages";
		int32_t count = vm.popDataValue(); LogInfo() << "count: " << count;
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_SetPages(document, count);
	});

	// void Doc_Show(int document);
	// G1: nachdem das Dokument mit den obenstehenden Befehlen befüllt und formatiert wurde, wird es hiermit nun auf den Bildschirm geschrieben!
	// Zeigt das Dokument auf dem Bildschirm an.
	//
	//      Document        - Handle auf das Dokument
	vm->registerExternalFunction("Doc_Show", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Doc_Show";
		int32_t document = vm.popDataValue(); LogInfo() << "document: " << document;
		Externals::Doc_Show(document);
	});

/*
npc (124)
*/
	// bool Npc_AreWeStronger(c_npc self, c_npc other);
	// ermittelt den stärkeren Nsc,
	// Wenn die Summe der Level aller NPCs (human, Monster), die zu mir feindlich sind und die zu <other> freundlich sind
	// MEHR ALS DOPPELT SO HOCH ist (x > 2*y), wie die Summe der Level aller Leute, die zu mir freundlich sind, und die
	// zu <other> feindlich sind, dann NEIN,
	// sonst JA (= wir SIND stärker, nicht echt, aber ab doppelter Gegnerstärke zu fliehen ist früh genug, sonst kommen
	// die Jungs aus dem Rennen nicht mehr raus
	// Beachten:
	// 1) Monster können feindlich zu beiden Menschen sein --> egal, zählen auf beiden Seiten
	// 2) Jemand, der z.B. zu mir freundlich und zu <other> feindlich ist, wird demnach doppelt gewertet --> ok so
	vm->registerExternalFunction("Npc_AreWeStronger", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_AreWeStronger";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_AreWeStronger(selfVob, otherVob));
		vm.setReturn(0);
	});

	// bool Npc_CanSeeItem(c_npc npc1, c_item item);
	// Prueft ob Npc1 den Gegenstand sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False
	vm->registerExternalFunction("Npc_CanSeeItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CanSeeItem";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		if (!npc1Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CanSeeItem(npc1Vob, item));
		vm.setReturn(0);
	});

	// bool Npc_CanSeeNpc(c_npc npc1, c_npc npc2);
	// Prueft ob Npc1 den Npc2 sehen kann ( ein Ray wird gecastet -> Bitte sparsam einsetzen ) True/False Blickwinkelabhängig (+,-100°)+LOS von der Hüfte aus
	vm->registerExternalFunction("Npc_CanSeeNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CanSeeNpc";
		int32_t npc2 = vm.popVar(); LogInfo() << "npc2: " << npc2;
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		VobTypes::NpcVobInformation npc2Vob = getNPCByInstance(npc2);
		if (!npc1Vob.isValid() || !npc2Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CanSeeNpc(npc1Vob, npc2Vob));
		vm.setReturn(0);
	});

	// bool Npc_CanSeeNpcFreeLOS(c_npc self, c_npc other);
	// Prüft ob NSC anderen NSC sehen kann, ohne dabei den Winkel zu berücksichtigen (nur LineOfSight-Check)
	vm->registerExternalFunction("Npc_CanSeeNpcFreeLOS", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CanSeeNpcFreeLOS";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CanSeeNpcFreeLOS(selfVob, otherVob));
		vm.setReturn(0);
	});

	// bool Npc_CanSeeSource(c_npc self);
	// Checkt, ob NSC die Soundquelle sehen kann True/False
	vm->registerExternalFunction("Npc_CanSeeSource", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CanSeeSource";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CanSeeSource(selfVob));
		vm.setReturn(0);
	});

	// void Npc_ChangeAttribute(c_npc self, int atr, int value);
	// ändert den Wert des Attributs (atr) um (value) Einheiten.
	vm->registerExternalFunction("Npc_ChangeAttribute", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_ChangeAttribute";
		int32_t value = vm.popDataValue(); LogInfo() << "value: " << value;
		int32_t atr = vm.popDataValue(); LogInfo() << "atr: " << atr;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_ChangeAttribute(selfVob, atr, value);
	});

	// bool Npc_CheckAvailableMission(c_npc npc, int missionState, bool important);
	// Prüfe ob zwischen dem NSC eine aktuelle Mission (AVAILABLE,RUNNING) besteht und vergleiche den Status mit den angegebenen.
	// Falls dieser identisch ist, wird "1" geliefert.
	vm->registerExternalFunction("Npc_CheckAvailableMission", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CheckAvailableMission";
		bool important = static_cast<bool>(vm.popDataValue()); LogInfo() << "important: " << important;
		int32_t missionState = vm.popDataValue(); LogInfo() << "missionState: " << missionState;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CheckAvailableMission(npcVob, missionState, important));
		vm.setReturn(0);
	});

	// bool Npc_CheckInfo(c_npc npc, bool important);
	// Überprüft,ob der NSC gültige Infos für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").
	vm->registerExternalFunction("Npc_CheckInfo", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CheckInfo";
		bool important = static_cast<bool>(vm.popDataValue()); LogInfo() << "important: " << important;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CheckInfo(npcVob, important));
		vm.setReturn(0);
	});

	// bool Npc_CheckOfferMission(c_npc npc, bool important);
	// Überprueft ob der NSC dem Spieler einen Auftrag anbieten kann, wenn ja, wird der Offer-Block gestartet und "1" zurückgeliefert.
	vm->registerExternalFunction("Npc_CheckOfferMission", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CheckOfferMission";
		bool important = static_cast<bool>(vm.popDataValue()); LogInfo() << "important: " << important;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CheckOfferMission(npcVob, important));
		vm.setReturn(0);
	});

	// bool Npc_CheckRunningMission(c_npc npc, bool important);
	// Überprueft ob zwischen dem NSC und dem Spieler eine laufende Mission besteht und startet den entsprechenden Skriptblock.
	// (Returnwert 1 : Mission aktiv)
	vm->registerExternalFunction("Npc_CheckRunningMission", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CheckRunningMission";
		bool important = static_cast<bool>(vm.popDataValue()); LogInfo() << "important: " << important;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_CheckRunningMission(npcVob, important));
		vm.setReturn(0);
	});

	// void Npc_ClearAIQueue(c_npc self);
	// hiermit werden alle Befehle mit sofortiger Wirkung aus der AI_Queue rausgeworfen
	vm->registerExternalFunction("Npc_ClearAIQueue", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_ClearAIQueue";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_ClearAIQueue(selfVob);
	});

	// void Npc_ClearInventory(instance n0);
	vm->registerExternalFunction("Npc_ClearInventory", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_ClearInventory";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Npc_ClearInventory(n0);
	});

	// void Npc_CreateSpell(c_npc self, int spellnr);
	// NSc bekommt Zauberspruch zugewiesen kann diesen aber noch nicht gebrauchen
	// (erscheint ausgegraut im Auswahlkranz)
	vm->registerExternalFunction("Npc_CreateSpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_CreateSpell";
		int32_t spellnr = vm.popDataValue(); LogInfo() << "spellnr: " << spellnr;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_CreateSpell(selfVob, spellnr);
	});

	// int Npc_DeleteNews(instance n0, int i1);
	vm->registerExternalFunction("Npc_DeleteNews", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_DeleteNews";
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_DeleteNews(n0, i1));
		vm.setReturn(0);
	});

	// void Npc_ExchangeRoutine(c_npc self, string routineName);
	// Tausche meherere Tagesabläufe aus
	vm->registerExternalFunction("Npc_ExchangeRoutine", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_ExchangeRoutine";
		std::string routineName = vm.popString(); LogInfo() << "routineName: " << routineName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_ExchangeRoutine(selfVob, routineName);
	});

	// int Npc_GetActiveSpell(c_npc self);
	// liefert den Zauber zurück, der auf der Hand ist (self oder other)
	// liefert -1 zurück, wenn kein Zauber auf der Hand ist
	vm->registerExternalFunction("Npc_GetActiveSpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetActiveSpell";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetActiveSpell(selfVob));
		vm.setReturn(0);
	});

	// int Npc_GetActiveSpellCat(c_npc self);
	// Unterscheidet zwischen den drei Kategorien (Spell_Bad, Spell_neutral,Spell_Good) Spellkat ist Rückgabewert
	vm->registerExternalFunction("Npc_GetActiveSpellCat", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetActiveSpellCat";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetActiveSpellCat(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_GetActiveSpellIsScroll(instance n0);
	vm->registerExternalFunction("Npc_GetActiveSpellIsScroll", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetActiveSpellIsScroll";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_GetActiveSpellIsScroll(n0));
		vm.setReturn(0);
	});

	// int Npc_GetActiveSpellLevel(c_npc self);
	// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist
	vm->registerExternalFunction("Npc_GetActiveSpellLevel", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetActiveSpellLevel";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetActiveSpellLevel(selfVob));
		vm.setReturn(0);
	});

	// int Npc_GetAttitude(c_npc self, c_npc other);
	// Gibt die Attitüde von self zu other zurück (temp. / perm. / Gilden )
	vm->registerExternalFunction("Npc_GetAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetAttitude";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetAttitude(selfVob, otherVob));
		vm.setReturn(0);
	});

	// int Npc_GetBodyState(c_npc self);
	// Ermittle BodyState ( Liefert BS_-Konstanten )
	// !!! VORSICHT !!!: Diese Funktion gibt den Bodystate mit allen eventuellen
	// ver-oderten Modifiern wie "brennend". Besser 'C_BodyStateContains(self,bodystate)' benutzen
	vm->registerExternalFunction("Npc_GetBodyState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetBodyState";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetBodyState(selfVob));
		vm.setReturn(0);
	});

	// int Npc_GetComrades(instance n0);
	vm->registerExternalFunction("Npc_GetComrades", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetComrades";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_GetComrades(n0));
		vm.setReturn(0);
	});

	// string Npc_GetDetectedMob(c_npc self);
	// liefert den Schemanamen des Mobsi zurück, das der Nsc entdeckt hat. Schemaname ist das String-Kürzel des Mobs, daß im Visualnamen vor dem ersten "_"-Zeichen steht, also z.B. "DOOR" wenn der Visualname "DOOR_OCR__135" ist.
	// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
	// WORKAROUND: zur Zeit werden hiermit nur Tür-MOBSIs zurückgegeben, dies soll aber wieder zurückgebaut werden
	vm->registerExternalFunction("Npc_GetDetectedMob", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetDetectedMob";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn();
			return;
		}
		vm.setReturn(Externals::Npc_GetDetectedMob(selfVob));
		vm.setReturn();
	});

	// int Npc_GetDistToItem(c_npc npc, c_item item);
	// Liefert Entfernung ( in cm ! ) zwischend NSC und Gegenstand
	vm->registerExternalFunction("Npc_GetDistToItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetDistToItem";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetDistToItem(npcVob, item));
		vm.setReturn(0);
	});

	// int Npc_GetDistToNpc(c_npc npc1, c_npc npc2);
	// Liefert Entfernung ( in cm ! ) zwischend den beiden NSCs
	vm->registerExternalFunction("Npc_GetDistToNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetDistToNpc";
		int32_t npc2 = vm.popVar(); LogInfo() << "npc2: " << npc2;
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		VobTypes::NpcVobInformation npc2Vob = getNPCByInstance(npc2);
		if (!npc1Vob.isValid() || !npc2Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetDistToNpc(npc1Vob, npc2Vob));
		vm.setReturn(0);
	});

	// int Npc_GetDistToPlayer(c_npc npc1);
	// Liefert Entfernung ( in cm ! ) zwischend den beiden NSCs
	vm->registerExternalFunction("Npc_GetDistToPlayer", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetDistToPlayer";
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		if (!npc1Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetDistToPlayer(npc1Vob));
		vm.setReturn(0);
	});

	// int Npc_GetDistToWP(c_npc self, string wpName);
	// liefert die Entfernung vom NSC 'self' zum angegebenen Waypoint in cm
	vm->registerExternalFunction("Npc_GetDistToWP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetDistToWP";
		std::string wpName = vm.popString(); LogInfo() << "wpName: " << wpName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetDistToWP(selfVob, wpName));
		vm.setReturn(0);
	});

	// c_item Npc_GetEquippedArmor(c_npc n0);
	// Liefert die angelegte Rüstung des NSCs.
	vm->registerExternalFunction("Npc_GetEquippedArmor", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetEquippedArmor";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetEquippedArmor(n0Vob));
		vm.setReturnVar(0);
	});

	// c_item Npc_GetEquippedMeleeWeapon(c_npc n0);
	// Liefert die gegurtete Nahkampfwaffe des NSCs.
	vm->registerExternalFunction("Npc_GetEquippedMeleeWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetEquippedMeleeWeapon";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetEquippedMeleeWeapon(n0Vob));
		vm.setReturnVar(0);
	});

	// c_item Npc_GetEquippedRangedWeapon(c_npc n0);
	// Liefert die gegurtete Fernkampfwaffe des NSCs.
	vm->registerExternalFunction("Npc_GetEquippedRangedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetEquippedRangedWeapon";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetEquippedRangedWeapon(n0Vob));
		vm.setReturnVar(0);
	});

	// int Npc_GetGuildAttitude(c_npc npc1, c_npc npc2);
	// Ermittelt die Gildenattitüde von zwei Nsc´s direkt im Gegensatz zu Wld_GetGuildAttitude
	vm->registerExternalFunction("Npc_GetGuildAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetGuildAttitude";
		int32_t npc2 = vm.popVar(); LogInfo() << "npc2: " << npc2;
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		VobTypes::NpcVobInformation npc2Vob = getNPCByInstance(npc2);
		if (!npc1Vob.isValid() || !npc2Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetGuildAttitude(npc1Vob, npc2Vob));
		vm.setReturn(0);
	});

	// int Npc_GetHeightToItem(instance n0, instance n1);
	vm->registerExternalFunction("Npc_GetHeightToItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetHeightToItem";
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_GetHeightToItem(n0, n1));
		vm.setReturn(0);
	});

	// int Npc_GetHeightToNpc(c_npc npc1, c_npc npc2);
	// Gothic 2 only function
	// Liefert Höhendifferenz ( in cm ! ) zwischend den beiden NSCs
	vm->registerExternalFunction("Npc_GetHeightToNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetHeightToNpc";
		int32_t npc2 = vm.popVar(); LogInfo() << "npc2: " << npc2;
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		VobTypes::NpcVobInformation npc2Vob = getNPCByInstance(npc2);
		if (!npc1Vob.isValid() || !npc2Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetHeightToNpc(npc1Vob, npc2Vob));
		vm.setReturn(0);
	});

	// c_item Npc_GetInvItem(c_npc self, int itemInstance);
	// Ermittle ItemInstanz aus Inventory
	vm->registerExternalFunction("Npc_GetInvItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetInvItem";
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetInvItem(selfVob, itemInstance));
		vm.setReturnVar(0);
	});

	// int Npc_GetInvItemBySlot(c_npc self, int category, int slotNr);
	// Mit diesem Befehl läßt sich nachsehen, ob in einem bestimmten Slot einer bestimmten Kategorie ein item vorhanden ist
	// ist das der Fall, wird dieses Item in die globale Variable item geschrieben
	// gibt jetzt die Anzahl zurueck, wenn das Item stackable ist.
	// Den Transfer machst Du dann per Npc_RemoveInvItems() und Npc_CreateInvItems().
	vm->registerExternalFunction("Npc_GetInvItemBySlot", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetInvItemBySlot";
		int32_t slotNr = vm.popDataValue(); LogInfo() << "slotNr: " << slotNr;
		int32_t category = vm.popDataValue(); LogInfo() << "category: " << category;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetInvItemBySlot(selfVob, category, slotNr));
		vm.setReturn(0);
	});

	// int Npc_GetLastHitSpellCat(c_npc self);
	// Gothic 2 only function
	// liefert die Category des Zaubers zurück, der den NSC zuletzt getroffen hat
	vm->registerExternalFunction("Npc_GetLastHitSpellCat", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetLastHitSpellCat";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetLastHitSpellCat(selfVob));
		vm.setReturn(0);
	});

	// int Npc_GetLastHitSpellID(c_npc self);
	// Gothic 2 only function
	// liefert den Zauber zurück, der den NSC zuletzt getroffen hat
	vm->registerExternalFunction("Npc_GetLastHitSpellID", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetLastHitSpellID";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetLastHitSpellID(selfVob));
		vm.setReturn(0);
	});

	// instance Npc_GetLookAtTarget(instance n0);
	vm->registerExternalFunction("Npc_GetLookAtTarget", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetLookAtTarget";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturnVar(Externals::Npc_GetLookAtTarget(n0));
		vm.setReturnVar(0);
	});

	// string Npc_GetNearestWP(c_npc self);
	// liefert den Namen des am nächsten gelegenen Waypoints zurück
	vm->registerExternalFunction("Npc_GetNearestWP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetNearestWP";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn();
			return;
		}
		vm.setReturn(Externals::Npc_GetNearestWP(selfVob));
		vm.setReturn();
	});

	// c_npc Npc_GetNewsOffender(c_npc self, int newsNumber);
	// Ermittelt Täter der News und gibt eine INstanz der Klasse C_Npc zurück
	vm->registerExternalFunction("Npc_GetNewsOffender", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetNewsOffender";
		int32_t newsNumber = vm.popDataValue(); LogInfo() << "newsNumber: " << newsNumber;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetNewsOffender(selfVob, newsNumber));
		vm.setReturnVar(0);
	});

	// c_npc Npc_GetNewsVictim(c_npc self, int newsNumber);
	// Ermittle Opfer der News und gibt eine INstanz der Klasse C_Npc zurück
	vm->registerExternalFunction("Npc_GetNewsVictim", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetNewsVictim";
		int32_t newsNumber = vm.popDataValue(); LogInfo() << "newsNumber: " << newsNumber;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetNewsVictim(selfVob, newsNumber));
		vm.setReturnVar(0);
	});

	// c_npc Npc_GetNewsWitness(c_npc self, int newsNumber);
	// Ermittle Zeuge der News und gibt eine INstanz der Klasse C_Npc zurück
	vm->registerExternalFunction("Npc_GetNewsWitness", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetNewsWitness";
		int32_t newsNumber = vm.popDataValue(); LogInfo() << "newsNumber: " << newsNumber;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetNewsWitness(selfVob, newsNumber));
		vm.setReturnVar(0);
	});

	// bool Npc_GetNextTarget(c_npc self);
	// Aktive Suche nach einem Gegner. Wird ein Gegner gefunden, so wird er als internes Ziel
	// übernommen und in 'other' geschrieben, wenn kein Gegner gefunden wurde wird das
	// interne Ziel gelöscht und 'other' ungültig gemacht.
	// Kriterien:   1. gesetzter Gegner, der nicht tot/bewußtlos ist wird genommen...
	//              2. ...dann der nächstbeste aNSC demgegenüber 'self' HOSTILE ist (auch weder tot noch bewußtlos)
	// - return:    neues Ziel gefunden     -> TRUE
	//              kein Ziel gefunden      -> FALSE
	// VORSICHT:    Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
	//              Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
	//              Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden
	vm->registerExternalFunction("Npc_GetNextTarget", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetNextTarget";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetNextTarget(selfVob));
		vm.setReturn(0);
	});

	// string Npc_GetNextWP(c_npc self);
	// Liefert den zweitnahesten WP vom NSC zurück
	vm->registerExternalFunction("Npc_GetNextWP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetNextWP";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn();
			return;
		}
		vm.setReturn(Externals::Npc_GetNextWP(selfVob));
		vm.setReturn();
	});

	// int Npc_GetPermAttitude(c_npc self, c_npc other);
	// Ermittle die permanente Attitude von "self" zu "other"
	vm->registerExternalFunction("Npc_GetPermAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetPermAttitude";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetPermAttitude(selfVob, otherVob));
		vm.setReturn(0);
	});

	// int Npc_GetPortalGuild(instance n0);
	vm->registerExternalFunction("Npc_GetPortalGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetPortalGuild";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_GetPortalGuild(n0));
		vm.setReturn(0);
	});

	// instance Npc_GetPortalOwner(instance n0);
	vm->registerExternalFunction("Npc_GetPortalOwner", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetPortalOwner";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturnVar(Externals::Npc_GetPortalOwner(n0));
		vm.setReturnVar(0);
	});

	// c_item Npc_GetReadiedWeapon(c_npc n0);
	// Liefert die gezogene Waffe des NSCs.
	vm->registerExternalFunction("Npc_GetReadiedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetReadiedWeapon";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			vm.setReturnVar(0);
			return;
		}
		vm.setReturnVar(Externals::Npc_GetReadiedWeapon(n0Vob));
		vm.setReturnVar(0);
	});

	// int Npc_GetStateTime(c_npc self);
	// Liefert Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
	vm->registerExternalFunction("Npc_GetStateTime", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetStateTime";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetStateTime(selfVob));
		vm.setReturn(0);
	});

	// int Npc_GetTalentSkill(instance n0, int i1);
	vm->registerExternalFunction("Npc_GetTalentSkill", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetTalentSkill";
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_GetTalentSkill(n0, i1));
		vm.setReturn(0);
	});

	// int Npc_GetTalentValue(instance n0, int i1);
	vm->registerExternalFunction("Npc_GetTalentValue", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetTalentValue";
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_GetTalentValue(n0, i1));
		vm.setReturn(0);
	});

	// bool Npc_GetTarget(c_npc self);
	// Befüllt 'other' mit dem aktuellen Ziel. Das aktuelle Ziel wird intern gespeichert, wird
	// durch Npc_SetTarget() bzw. Npc_GetNextTarget() gesetzt.
	// - return: aktuelles Ziel gespeichert     -> TRUE
	//           kein Ziel gespeichert          -> FALSE
	vm->registerExternalFunction("Npc_GetTarget", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetTarget";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetTarget(selfVob));
		vm.setReturn(0);
	});

	// int Npc_GetTrueGuild(c_npc npc);
	// liefert immer!!! die wahre Gilde zurück, ignoriert also auch ohne die Regeln die Verkleidung
	vm->registerExternalFunction("Npc_GetTrueGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GetTrueGuild";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_GetTrueGuild(npcVob));
		vm.setReturn(0);
	});

	// bool NPC_GiveInfo(c_npc npc, bool important);
	// Überprüft, ob der NSC eine (!) gültige Info für den Spieler hat und startet diese gegebenenfalls (Returnwert "1").
	vm->registerExternalFunction("NPC_GiveInfo", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "NPC_GiveInfo";
		bool important = static_cast<bool>(vm.popDataValue()); LogInfo() << "important: " << important;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::NPC_GiveInfo(npcVob, important));
		vm.setReturn(0);
	});

	// void Npc_GiveItem(c_npc n0, c_item n1, c_npc n2);
	// Der NSC "self" gibt den NSC "other" den angegebenen Gegenstand "item". Der Gegenstand wandert sofort ins Inventory des anderen.
	vm->registerExternalFunction("Npc_GiveItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_GiveItem";
		int32_t n2 = vm.popVar(); LogInfo() << "n2: " << n2;
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		VobTypes::NpcVobInformation n2Vob = getNPCByInstance(n2);
		if (!n0Vob.isValid() || !n2Vob.isValid())
		{
			return;
		}
		Externals::Npc_GiveItem(n0Vob, n1, n2Vob);
	});

	// bool Npc_HasBodyFlag(c_npc self, int bodyFlag);
	// Liefert >0, falls BodyFlag gesetzt ist.
	vm->registerExternalFunction("Npc_HasBodyFlag", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasBodyFlag";
		int32_t bodyFlag = vm.popDataValue(); LogInfo() << "bodyFlag: " << bodyFlag;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasBodyFlag(selfVob, bodyFlag));
		vm.setReturn(0);
	});

	// bool Npc_HasDetectedNpc(c_npc self, c_npc other);
	// liefert zurück, ob der Spieler in Sinnesreichweite und entdeckt ist
	vm->registerExternalFunction("Npc_HasDetectedNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasDetectedNpc";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasDetectedNpc(selfVob, otherVob));
		vm.setReturn(0);
	});

	// bool Npc_HasEquippedArmor(c_npc self);
	// Armor angezogen True/False
	vm->registerExternalFunction("Npc_HasEquippedArmor", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasEquippedArmor";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasEquippedArmor(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasEquippedMeleeWeapon(c_npc self);
	// gibt eins zurück, wenn eine Nahkampfwaffe Equipped ist
	vm->registerExternalFunction("Npc_HasEquippedMeleeWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasEquippedMeleeWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasEquippedMeleeWeapon(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasEquippedRangedWeapon(c_npc self);
	// Fernkampwaffe auf dem Rücken True/False
	vm->registerExternalFunction("Npc_HasEquippedRangedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasEquippedRangedWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasEquippedRangedWeapon(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasEquippedWeapon(c_npc self);
	// gibt eins zurück, wenn die abgefragte Instanz (self oder other) eine Waffe sichtbar mit sich rumträgt
	vm->registerExternalFunction("Npc_HasEquippedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasEquippedWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasEquippedWeapon(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasFightTalent(c_npc self, int tal);
	// Spezialabfrage auf Kampftalente (z.B. 1hSword) ansonsten wie Npc_HasTalent
	vm->registerExternalFunction("Npc_HasFightTalent", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasFightTalent";
		int32_t tal = vm.popDataValue(); LogInfo() << "tal: " << tal;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasFightTalent(selfVob, tal));
		vm.setReturn(0);
	});

	// int Npc_HasItems(c_npc n0, int itemInstance);
	// Liefert zurück wie viele Items der NSC vom angegebenen Typ besitzt
	vm->registerExternalFunction("Npc_HasItems", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasItems";
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasItems(n0Vob, itemInstance));
		vm.setReturn(0);
	});

	// int Npc_HasNews(c_npc self, int newsID, c_npc offender, c_npc victim);
	// Liefert Newsnummer>0 (für weitere Referenzen) falls entsprechende News vorhanden.
	// Nicht benötigte Parameter können mit "NULL" leergelassen werden
	vm->registerExternalFunction("Npc_HasNews", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasNews";
		int32_t victim = vm.popVar(); LogInfo() << "victim: " << victim;
		int32_t offender = vm.popVar(); LogInfo() << "offender: " << offender;
		int32_t newsID = vm.popDataValue(); LogInfo() << "newsID: " << newsID;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation offenderVob = getNPCByInstance(offender);
		VobTypes::NpcVobInformation victimVob = getNPCByInstance(victim);
		if (!selfVob.isValid() || !offenderVob.isValid() || !victimVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasNews(selfVob, newsID, offenderVob, victimVob));
		vm.setReturn(0);
	});

	// bool Npc_HasOffered(c_npc self, c_npc other, int itemInstance);
	// Bietet Spieler dem NSC einen Gegenstand übers Trade-Modul an ? True/False
	vm->registerExternalFunction("Npc_HasOffered", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasOffered";
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasOffered(selfVob, otherVob, itemInstance));
		vm.setReturn(0);
	});

	// bool Npc_HasRangedWeaponWithAmmo(c_npc npc);
	// Gibt TRUE zurück, wenn 'npc' irgendeine Fernkampfwaffe im Inventory oder in der Hand hat UND
	// dazu auch passende Munition vorhanden ist. Sonst FALSE.
	vm->registerExternalFunction("Npc_HasRangedWeaponWithAmmo", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasRangedWeaponWithAmmo";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasRangedWeaponWithAmmo(npcVob));
		vm.setReturn(0);
	});

	// bool Npc_HasReadiedMeleeWeapon(c_npc self);
	// Nahkampfwaffe in der Hand
	vm->registerExternalFunction("Npc_HasReadiedMeleeWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasReadiedMeleeWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasReadiedMeleeWeapon(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasReadiedRangedWeapon(c_npc self);
	// Fernkampfwaffe in der Hand
	vm->registerExternalFunction("Npc_HasReadiedRangedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasReadiedRangedWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasReadiedRangedWeapon(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasReadiedWeapon(c_npc self);
	// gibt eins zurück, wenn die Waffe schon in der Hand ist
	vm->registerExternalFunction("Npc_HasReadiedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasReadiedWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasReadiedWeapon(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_HasSpell(c_npc self, int spellID);
	// Kann der NSC den angegebenen Zauberspruch benutzen ?
	vm->registerExternalFunction("Npc_HasSpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasSpell";
		int32_t spellID = vm.popDataValue(); LogInfo() << "spellID: " << spellID;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasSpell(selfVob, spellID));
		vm.setReturn(0);
	});

	// bool Npc_HasTalent(c_npc self, int tal);
	// liefert eins zurück, wenn der geprüfte Charakter das Talent tal hat
	vm->registerExternalFunction("Npc_HasTalent", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_HasTalent";
		int32_t tal = vm.popDataValue(); LogInfo() << "tal: " << tal;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_HasTalent(selfVob, tal));
		vm.setReturn(0);
	});

	// bool Npc_IsAiming(c_npc self, c_npc other);
	// liefert eins zurück, wenn der zweite Character auf den ersten zielt (Fernkampfwaffen und Zauber)
	vm->registerExternalFunction("Npc_IsAiming", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsAiming";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsAiming(selfVob, otherVob));
		vm.setReturn(0);
	});

	// bool Npc_IsDead(c_npc n0);
	// Funktion liefert einen Wert zurück, falls Dead true ist
	vm->registerExternalFunction("Npc_IsDead", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsDead";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsDead(n0Vob));
		vm.setReturn(0);
	});

	// bool Npc_IsDetectedMobOwnedByGuild(c_npc user, int ownerguild);
	// Liefert >0, falls "ownerguild" der Besitzer des vom "user" benutzten Mob ist.
	// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
	vm->registerExternalFunction("Npc_IsDetectedMobOwnedByGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsDetectedMobOwnedByGuild";
		int32_t ownerguild = vm.popDataValue(); LogInfo() << "ownerguild: " << ownerguild;
		int32_t user = vm.popVar(); LogInfo() << "user: " << user;
		VobTypes::NpcVobInformation userVob = getNPCByInstance(user);
		if (!userVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsDetectedMobOwnedByGuild(userVob, ownerguild));
		vm.setReturn(0);
	});

	// bool Npc_IsDetectedMobOwnedByNpc(c_npc user, c_npc owner);
	// Liefert >0, falls "owner" der Besitzer des vom "user" benutzten Mob ist.
	// VORSICHT: Diese Funktion ist nur in Verbindung mit PERC_MOVEMOB erlaubt !!!
	vm->registerExternalFunction("Npc_IsDetectedMobOwnedByNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsDetectedMobOwnedByNpc";
		int32_t owner = vm.popVar(); LogInfo() << "owner: " << owner;
		int32_t user = vm.popVar(); LogInfo() << "user: " << user;
		VobTypes::NpcVobInformation userVob = getNPCByInstance(user);
		VobTypes::NpcVobInformation ownerVob = getNPCByInstance(owner);
		if (!userVob.isValid() || !ownerVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsDetectedMobOwnedByNpc(userVob, ownerVob));
		vm.setReturn(0);
	});

	// bool Npc_IsDrawingSpell(instance n0);
	vm->registerExternalFunction("Npc_IsDrawingSpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsDrawingSpell";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_IsDrawingSpell(n0));
		vm.setReturn(0);
	});

	// bool Npc_IsDrawingWeapon(instance n0);
	vm->registerExternalFunction("Npc_IsDrawingWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsDrawingWeapon";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_IsDrawingWeapon(n0));
		vm.setReturn(0);
	});

	// bool Npc_IsInCutscene(c_npc self);
	// liefert eins zurück, wenn der entsprechende Nsc in einer Cutscene ist
	vm->registerExternalFunction("Npc_IsInCutscene", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsInCutscene";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsInCutscene(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_IsInFightMode(c_npc self, int fmode);
	// liefert eins zurück, wenn der Charakter im angegebenen Fightmode (z.B. FMODE_MAGIC) ist
	vm->registerExternalFunction("Npc_IsInFightMode", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsInFightMode";
		int32_t fmode = vm.popDataValue(); LogInfo() << "fmode: " << fmode;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsInFightMode(selfVob, fmode));
		vm.setReturn(0);
	});

	// bool Npc_IsInPlayersRoom(instance n0);
	vm->registerExternalFunction("Npc_IsInPlayersRoom", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsInPlayersRoom";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_IsInPlayersRoom(n0));
		vm.setReturn(0);
	});

	// bool Npc_IsInRoutine(c_npc self, func state);
	// Check ob der angegebene Zustand der aktuelle TA des NSCs ist. True/FAlse
	vm->registerExternalFunction("Npc_IsInRoutine", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsInRoutine";
		uint32_t state = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "state: " << state;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsInRoutine(selfVob, state));
		vm.setReturn(0);
	});

	// bool Npc_IsInState(c_npc self, func state);
	// Abfrage auf den aktuellen Zustand einer Spielfigur True/False
	vm->registerExternalFunction("Npc_IsInState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsInState";
		uint32_t state = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "state: " << state;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsInState(selfVob, state));
		vm.setReturn(0);
	});

	// bool Npc_IsNear(c_npc self, c_npc other);
	// liefert eins zurück, wenn geprüfte Instanz sich im Bereich von drei Metern zur prüfenden Instanz befindet
	vm->registerExternalFunction("Npc_IsNear", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsNear";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsNear(selfVob, otherVob));
		vm.setReturn(0);
	});

	// bool Npc_IsNewsGossip(c_npc self, int newsNumber);
	// Liefert >0, falls News "Gossip" ist
	vm->registerExternalFunction("Npc_IsNewsGossip", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsNewsGossip";
		int32_t newsNumber = vm.popDataValue(); LogInfo() << "newsNumber: " << newsNumber;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsNewsGossip(selfVob, newsNumber));
		vm.setReturn(0);
	});

	// bool Npc_IsNextTargetAvailable(c_npc self);
	// Sucht genauso wie Npc_GetNextTarget nach einem neuen Ziel, überschreibt aber weder
	// das interne Ziel, noch 'other'
	// - return:    neues Ziel gefunden     -> TRUE
	//              kein Ziel gefunden      -> FALSE
	// VORSICHT:    Beruht auf der aktuellsten VOB-Liste, die durch aktive Wahrnehmungen oder durch
	//              Npc_PerceiveAll() erstellt wurde. Wird diese Funktion in einem Zustand ohne aktive
	//              Wahrnehmungen benutzt, muß vorher ein Npc_PerceiveAll() aufgerufen werden
	vm->registerExternalFunction("Npc_IsNextTargetAvailable", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsNextTargetAvailable";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsNextTargetAvailable(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_IsOnFP(c_npc self, string name);
	// Abfrage darauf, ob der Nsc auf einem Freepoint mit name Teilstring steht
	vm->registerExternalFunction("Npc_IsOnFP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsOnFP";
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsOnFP(selfVob, name));
		vm.setReturn(0);
	});

	// bool Npc_IsPlayer(c_npc player);
	// liefert eins zurück, wenn der geprüfte Charakter der Spieler himself ist
	vm->registerExternalFunction("Npc_IsPlayer", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsPlayer";
		int32_t player = vm.popVar(); LogInfo() << "player: " << player;
		VobTypes::NpcVobInformation playerVob = getNPCByInstance(player);
		if (!playerVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsPlayer(playerVob));
		vm.setReturn(0);
	});

	// bool Npc_IsPlayerInMyRoom(c_npc npc);
	// gibt TRUE zurück, wenn sich SC im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
	vm->registerExternalFunction("Npc_IsPlayerInMyRoom", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsPlayerInMyRoom";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsPlayerInMyRoom(npcVob));
		vm.setReturn(0);
	});

	// bool Npc_IsVoiceActive(instance n0);
	vm->registerExternalFunction("Npc_IsVoiceActive", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsVoiceActive";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Npc_IsVoiceActive(n0));
		vm.setReturn(0);
	});

	// bool Npc_IsWayBlocked(c_npc self);
	// Liefert "1", falls Weg von NSC durch Hindernis versperrt ist.
	vm->registerExternalFunction("Npc_IsWayBlocked", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_IsWayBlocked";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_IsWayBlocked(selfVob));
		vm.setReturn(0);
	});

	// bool Npc_KnowsInfo(c_npc self, int infoInstance);
	// Liefert TRUE, wenn der angegebene Spieler die Info schon einmal erhalten hat.
	// VORSICHT: auch wenn eine permanente Info schon einmal dem Spieler erzählt wurde, so gibt diese Funktion trotzdem FALSE zurück!
	vm->registerExternalFunction("Npc_KnowsInfo", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_KnowsInfo";
		int32_t infoInstance = vm.popDataValue(); LogInfo() << "infoInstance: " << infoInstance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_KnowsInfo(selfVob, infoInstance));
		vm.setReturn(0);
	});

	// bool Npc_KnowsPlayer(c_npc self, c_npc player);
	// Kennt der NSC den Spieler? True/False
	vm->registerExternalFunction("Npc_KnowsPlayer", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_KnowsPlayer";
		int32_t player = vm.popVar(); LogInfo() << "player: " << player;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation playerVob = getNPCByInstance(player);
		if (!selfVob.isValid() || !playerVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_KnowsPlayer(selfVob, playerVob));
		vm.setReturn(0);
	});

	// void Npc_LearnSpell(c_npc self, int spellnr);
	// Was könnte das wohl heißen ? Ich glaube damit kann man den (spellnr) Zauber zuweisen
	vm->registerExternalFunction("Npc_LearnSpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_LearnSpell";
		int32_t spellnr = vm.popDataValue(); LogInfo() << "spellnr: " << spellnr;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_LearnSpell(selfVob, spellnr);
	});

	// void Npc_MemoryEntry(c_npc self, int source, c_npc offender, int newsid, c_npc victim);
	// mit dieser Funktion wird eine NSC bezogene News geschrieben newsid : News ID source > 0 : "gossip", ansonsten "witness",
	// self: NSC, bei dem News eingespeist werden soll, other: Täter victim : Opfer
	vm->registerExternalFunction("Npc_MemoryEntry", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_MemoryEntry";
		int32_t victim = vm.popVar(); LogInfo() << "victim: " << victim;
		int32_t newsid = vm.popDataValue(); LogInfo() << "newsid: " << newsid;
		int32_t offender = vm.popVar(); LogInfo() << "offender: " << offender;
		int32_t source = vm.popDataValue(); LogInfo() << "source: " << source;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation offenderVob = getNPCByInstance(offender);
		VobTypes::NpcVobInformation victimVob = getNPCByInstance(victim);
		if (!selfVob.isValid() || !offenderVob.isValid() || !victimVob.isValid())
		{
			return;
		}
		Externals::Npc_MemoryEntry(selfVob, source, offenderVob, newsid, victimVob);
	});

	// void Npc_MemoryEntryGuild(c_npc self, int source, c_npc offender, int newsid, c_npc victimguild);
	// wie MemoryEntry nur, das die Memory für die ganze Gilde kreiert wird
	vm->registerExternalFunction("Npc_MemoryEntryGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_MemoryEntryGuild";
		int32_t victimguild = vm.popVar(); LogInfo() << "victimguild: " << victimguild;
		int32_t newsid = vm.popDataValue(); LogInfo() << "newsid: " << newsid;
		int32_t offender = vm.popVar(); LogInfo() << "offender: " << offender;
		int32_t source = vm.popDataValue(); LogInfo() << "source: " << source;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation offenderVob = getNPCByInstance(offender);
		VobTypes::NpcVobInformation victimguildVob = getNPCByInstance(victimguild);
		if (!selfVob.isValid() || !offenderVob.isValid() || !victimguildVob.isValid())
		{
			return;
		}
		Externals::Npc_MemoryEntryGuild(selfVob, source, offenderVob, newsid, victimguildVob);
	});

	// bool Npc_OwnedByGuild(c_item item, int guild);
	// Liefert "1", wenn der angegebenen Gilde das Item gehört ( Gildenbesitz )
	vm->registerExternalFunction("Npc_OwnedByGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_OwnedByGuild";
		int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		vm.setReturn(Externals::Npc_OwnedByGuild(item, guild));
		vm.setReturn(0);
	});

	// bool Npc_OwnedByNpc(c_item item, c_npc npc);
	// Liefert "1", wenn dem NSC das Item gehört ( persönliches Besitzflag )
	vm->registerExternalFunction("Npc_OwnedByNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_OwnedByNpc";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_OwnedByNpc(item, npcVob));
		vm.setReturn(0);
	});

	// void Npc_PercDisable(c_npc self, int percID);
	// Deaktiviere Perception
	vm->registerExternalFunction("Npc_PercDisable", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_PercDisable";
		int32_t percID = vm.popDataValue(); LogInfo() << "percID: " << percID;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_PercDisable(selfVob, percID);
	});

	// void Npc_PerceiveAll(c_npc self);
	// Nimm alle Objekte in Wahrnehmungsreichweite wahr, die dann mit WLD_DetectNpc und Wld_DetectItem auswertbar sind
	vm->registerExternalFunction("Npc_PerceiveAll", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_PerceiveAll";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_PerceiveAll(selfVob);
	});

	// void Npc_PercEnable(c_npc self, int percID, func function);
	// Aktiviere Perception
	vm->registerExternalFunction("Npc_PercEnable", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_PercEnable";
		uint32_t function = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "function: " << function;
		int32_t percID = vm.popDataValue(); LogInfo() << "percID: " << percID;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_PercEnable(selfVob, percID, function);
	});

	// void Npc_PlayAni(instance n0, string s1);
	vm->registerExternalFunction("Npc_PlayAni", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_PlayAni";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Npc_PlayAni(n0, s1);
	});

	// bool Npc_RefuseTalk(c_npc self);
	// Abfrage ob Dialog-Refuse Counter noch aktiv ist True/False
	vm->registerExternalFunction("Npc_RefuseTalk", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_RefuseTalk";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_RefuseTalk(selfVob));
		vm.setReturn(0);
	});

	// void Npc_RemoveInvItem(c_npc owner, int itemInstance);
	// das angegebene Item wird aus dem Inventory des NSCs entfernt und gelöscht
	vm->registerExternalFunction("Npc_RemoveInvItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_RemoveInvItem";
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t owner = vm.popVar(); LogInfo() << "owner: " << owner;
		VobTypes::NpcVobInformation ownerVob = getNPCByInstance(owner);
		if (!ownerVob.isValid())
		{
			return;
		}
		Externals::Npc_RemoveInvItem(ownerVob, itemInstance);
	});

	// void Npc_RemoveInvItems(c_npc owner, int itemInstance, int amount);
	// das angegebene Anzahl des Multi-Items wird aus dem Inventory des NSCs entfernt und gelöscht
	// wie Npc_RemoveInvItem, nur das Multislotgegenstände gelöscht werden
	vm->registerExternalFunction("Npc_RemoveInvItems", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_RemoveInvItems";
		int32_t amount = vm.popDataValue(); LogInfo() << "amount: " << amount;
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t owner = vm.popVar(); LogInfo() << "owner: " << owner;
		VobTypes::NpcVobInformation ownerVob = getNPCByInstance(owner);
		if (!ownerVob.isValid())
		{
			return;
		}
		Externals::Npc_RemoveInvItems(ownerVob, itemInstance, amount);
	});

	// void Npc_SendPassivePerc(c_npc npc1, int perc_type, c_npc npc2, c_npc npc3);
	// Sende eine passive Wahrnehmung aus.Npc1 = wer schickt Npc2 = Opfer, Npc3 = Täter
	vm->registerExternalFunction("Npc_SendPassivePerc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SendPassivePerc";
		int32_t npc3 = vm.popVar(); LogInfo() << "npc3: " << npc3;
		int32_t npc2 = vm.popVar(); LogInfo() << "npc2: " << npc2;
		int32_t perc_type = vm.popDataValue(); LogInfo() << "perc_type: " << perc_type;
		int32_t npc1 = vm.popVar(); LogInfo() << "npc1: " << npc1;
		VobTypes::NpcVobInformation npc1Vob = getNPCByInstance(npc1);
		VobTypes::NpcVobInformation npc2Vob = getNPCByInstance(npc2);
		VobTypes::NpcVobInformation npc3Vob = getNPCByInstance(npc3);
		if (!npc1Vob.isValid() || !npc2Vob.isValid() || !npc3Vob.isValid())
		{
			return;
		}
		Externals::Npc_SendPassivePerc(npc1Vob, perc_type, npc2Vob, npc3Vob);
	});

	// void Npc_SendSinglePerc(c_npc self, c_npc target, int percID);
	// verschicke Wahrnehmung an einzelnen NSC
	vm->registerExternalFunction("Npc_SendSinglePerc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SendSinglePerc";
		int32_t percID = vm.popDataValue(); LogInfo() << "percID: " << percID;
		int32_t target = vm.popVar(); LogInfo() << "target: " << target;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation targetVob = getNPCByInstance(target);
		if (!selfVob.isValid() || !targetVob.isValid())
		{
			return;
		}
		Externals::Npc_SendSinglePerc(selfVob, targetVob, percID);
	});

	// int Npc_SetActiveSpellInfo(c_npc npc, int i1);
	// Hier kann ein Wert für den Zauberspruch gesetzt werden.
	// Was dieser Wert bewirkt, haengt allein von der Nutzung im Skript ab. Das Programm benutzt diesen nicht.
	// liefert den Spell-Level des Zaubers zurück, der auf der Hand ist
	vm->registerExternalFunction("Npc_SetActiveSpellInfo", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetActiveSpellInfo";
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_SetActiveSpellInfo(npcVob, i1));
		vm.setReturn(0);
	});

	// void Npc_SetAttitude(c_npc self, int att);
	// setzt die permanente Attitüde auf einen festen Wert
	vm->registerExternalFunction("Npc_SetAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetAttitude";
		int32_t att = vm.popDataValue(); LogInfo() << "att: " << att;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetAttitude(selfVob, att);
	});

	// void Npc_SetKnowsPlayer(c_npc self, c_npc player);
	// NSC kennt SC
	vm->registerExternalFunction("Npc_SetKnowsPlayer", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetKnowsPlayer";
		int32_t player = vm.popVar(); LogInfo() << "player: " << player;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation playerVob = getNPCByInstance(player);
		if (!selfVob.isValid() || !playerVob.isValid())
		{
			return;
		}
		Externals::Npc_SetKnowsPlayer(selfVob, playerVob);
	});

	// void Npc_SetPercTime(c_npc self, float seconds);
	// Setze Zeitdelta für aktive Wahrnehmungen, alle Zeitdelta-Sekunden wird WN gesendet
	vm->registerExternalFunction("Npc_SetPercTime", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetPercTime";
		float seconds = vm.popFloatValue(); LogInfo() << "seconds: " << seconds;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetPercTime(selfVob, seconds);
	});

	// void Npc_SetRefuseTalk(c_npc self, int timeSec);
	// Dialog Refuse Counter aus "x" Sekunden setzen
	vm->registerExternalFunction("Npc_SetRefuseTalk", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetRefuseTalk";
		int32_t timeSec = vm.popDataValue(); LogInfo() << "timeSec: " << timeSec;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetRefuseTalk(selfVob, timeSec);
	});

	// void Npc_SetStateTime(c_npc self, int seconds);
	// _Setzt_ Anzahl Sekunden, wie lange der NSC sich in diesem Zustand im "Loop" befindet.
	// --> Das ist dann über Npc_GetStateTime abfragbar.
	vm->registerExternalFunction("Npc_SetStateTime", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetStateTime";
		int32_t seconds = vm.popDataValue(); LogInfo() << "seconds: " << seconds;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetStateTime(selfVob, seconds);
	});

	// void Npc_SetTalentSkill(instance n0, int i1, int i2);
	vm->registerExternalFunction("Npc_SetTalentSkill", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetTalentSkill";
		int32_t i2 = vm.popDataValue(); LogInfo() << "i2: " << i2;
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Npc_SetTalentSkill(n0, i1, i2);
	});

	// void Npc_SetTalentValue(instance n0, int i1, int i2);
	vm->registerExternalFunction("Npc_SetTalentValue", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetTalentValue";
		int32_t i2 = vm.popDataValue(); LogInfo() << "i2: " << i2;
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Npc_SetTalentValue(n0, i1, i2);
	});

	// void Npc_SetTarget(c_npc self, c_npc other);
	// Gibt dem Nsc 'self' das interne Ziel 'other'.
	// --> Nur wenn per GetTarget auch der other "geholt" wird ist er vorhanden,
	// da hier interne Variablen, die in den Skripten nicht vorhanden sind verwendet werden
	vm->registerExternalFunction("Npc_SetTarget", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetTarget";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::Npc_SetTarget(selfVob, otherVob);
	});

	// void Npc_SetTeleportPos(c_npc self);
	// Magie/Zauberstein Teleport Spruch : letzte Position des NSCs merken, zwecks späteren dahinbeamens.
	vm->registerExternalFunction("Npc_SetTeleportPos", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetTeleportPos";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetTeleportPos(selfVob);
	});

	// void Npc_SetTempAttitude(c_npc self, int att);
	// setzt die temporäre Attitüde auf einen Wert (att)
	vm->registerExternalFunction("Npc_SetTempAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetTempAttitude";
		int32_t att = vm.popDataValue(); LogInfo() << "att: " << att;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetTempAttitude(selfVob, att);
	});

	// void Npc_SetToFightMode(c_npc self, int weapon);
	// Setzt den NSC beim Start in den der Waffe entsprechenden Kampfmodus (Waffe wird erzeugt)
	vm->registerExternalFunction("Npc_SetToFightMode", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetToFightMode";
		int32_t weapon = vm.popDataValue(); LogInfo() << "weapon: " << weapon;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetToFightMode(selfVob, weapon);
	});

	// void Npc_SetToFistMode(c_npc self);
	// Setzt den NSC beim Start in den Faustkampfmodus (zB.Monster)
	vm->registerExternalFunction("Npc_SetToFistMode", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetToFistMode";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::Npc_SetToFistMode(selfVob);
	});

	// int Npc_SetTrueGuild(c_npc npc, int guildID);
	// Setzt die wahre Gilde des NSCs
	vm->registerExternalFunction("Npc_SetTrueGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_SetTrueGuild";
		int32_t guildID = vm.popDataValue(); LogInfo() << "guildID: " << guildID;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_SetTrueGuild(npcVob, guildID));
		vm.setReturn(0);
	});

	// bool Npc_StartItemReactModules(c_npc self, c_npc other, c_item item);
	// Prüfe alle ItemReact-Module von "self" auf Gegenstand "item" von Geber "other" und starte passende Reaction-Funktion
	// liefert True beim finden eines Moduls
	vm->registerExternalFunction("Npc_StartItemReactModules", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_StartItemReactModules";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_StartItemReactModules(selfVob, otherVob, item));
		vm.setReturn(0);
	});

	// void Npc_StopAni(instance n0, string s1);
	vm->registerExternalFunction("Npc_StopAni", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_StopAni";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Npc_StopAni(n0, s1);
	});

	// bool Npc_WasInState(c_npc self, func state);
	// Abfrage auf den vorherigen Zustand einer Spielfigur True/False
	vm->registerExternalFunction("Npc_WasInState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_WasInState";
		uint32_t state = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "state: " << state;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_WasInState(selfVob, state));
		vm.setReturn(0);
	});

	// bool Npc_WasPlayerInMyRoom(c_npc npc);
	// gibt TRUE zurück, wenn sich SC vor dem letzten Raumwechsel im Raum des 'npc' oder seiner Gilde befindet, sonst FALSE
	// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
	vm->registerExternalFunction("Npc_WasPlayerInMyRoom", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Npc_WasPlayerInMyRoom";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Npc_WasPlayerInMyRoom(npcVob));
		vm.setReturn(0);
	});

/*
wld (35)
*/
	// void Wld_AssignRoomToGuild(string s0, int guild);
	// Ordnet den Raum: 's0' der Gilde 'guild' zu
	vm->registerExternalFunction("Wld_AssignRoomToGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_AssignRoomToGuild";
		int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::Wld_AssignRoomToGuild(s0, guild);
	});

	// void Wld_AssignRoomToNpc(string s0, c_npc roomowner);
	// Ordnet den Raum: 's0' dem speziellen Nsc 'roomowner' zu
	vm->registerExternalFunction("Wld_AssignRoomToNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_AssignRoomToNpc";
		int32_t roomowner = vm.popVar(); LogInfo() << "roomowner: " << roomowner;
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		VobTypes::NpcVobInformation roomownerVob = getNPCByInstance(roomowner);
		if (!roomownerVob.isValid())
		{
			return;
		}
		Externals::Wld_AssignRoomToNpc(s0, roomownerVob);
	});

	// bool Wld_DetectItem(c_npc self, int flags);
	// liefert eins zurück, wenn ein Item mit dem Entsprechende Flag (z.B.ITEM_KAT_FOOD )gefunden wurde
	// Globale Variable 'item' wird mit dem gefundenen Gegenstand initialisiert
	vm->registerExternalFunction("Wld_DetectItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_DetectItem";
		int32_t flags = vm.popDataValue(); LogInfo() << "flags: " << flags;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_DetectItem(selfVob, flags));
		vm.setReturn(0);
	});

	// bool Wld_DetectNpc(c_npc self, int instance, func aiState, int guild);
	// Diese Methode initilisiert die globale Skriptvariable "other" mit einem NSC, den "self" beim letzten Warnehmungscheck wargenommen hat.
	// instance     = Name der zu suchenden Instanz                                 ( "-1" angeben, wenn Instanzname unwichtig )
	// guild        = Der zu suchende NSC muss Mitglied dieser Gilde sein           ( "-1" angeben, wenn Gilde unwichtig )
	// aiState      = Der AI-Zustandsname, in dem sich der NSC befinden soll        ( NOFUNC angeben, wenn AI-State unwichtig )
	// Wenn die Methode einen entsprechenden NSC gefunden hat, liefert diese "1" und 'other' ist initialisiert
	// ansonsten wird "0" geliefert und "other" wird nicht verändert.
	vm->registerExternalFunction("Wld_DetectNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_DetectNpc";
		int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;
		uint32_t aiState = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "aiState: " << aiState;
		int32_t instance = vm.popDataValue(); LogInfo() << "instance: " << instance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_DetectNpc(selfVob, instance, aiState, guild));
		vm.setReturn(0);
	});

	// bool Wld_DetectNpcEx(c_npc self, int npcInstance, func aiState, int guild, bool detectPlayer);
	// Wie Wld_DetectNpc(). Zusätzlich kann per detectPlayer=0 der Spieler ignoriert werden.
	vm->registerExternalFunction("Wld_DetectNpcEx", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_DetectNpcEx";
		bool detectPlayer = static_cast<bool>(vm.popDataValue()); LogInfo() << "detectPlayer: " << detectPlayer;
		int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;
		uint32_t aiState = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "aiState: " << aiState;
		int32_t npcInstance = vm.popDataValue(); LogInfo() << "npcInstance: " << npcInstance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_DetectNpcEx(selfVob, npcInstance, aiState, guild, detectPlayer));
		vm.setReturn(0);
	});

	// int Wld_DetectNpcExAtt(instance n0, int i1, func f2, int i3, int i4, int i5);
	vm->registerExternalFunction("Wld_DetectNpcExAtt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_DetectNpcExAtt";
		int32_t i5 = vm.popDataValue(); LogInfo() << "i5: " << i5;
		int32_t i4 = vm.popDataValue(); LogInfo() << "i4: " << i4;
		int32_t i3 = vm.popDataValue(); LogInfo() << "i3: " << i3;
		uint32_t f2 = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "f2: " << f2;
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		vm.setReturn(Externals::Wld_DetectNpcExAtt(n0, i1, f2, i3, i4, i5));
		vm.setReturn(0);
	});

	// bool Wld_DetectPlayer(c_npc self);
	// liefert eins zurück, wenn der Player in der Nähe ist
	vm->registerExternalFunction("Wld_DetectPlayer", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_DetectPlayer";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_DetectPlayer(selfVob));
		vm.setReturn(0);
	});

	// void Wld_ExchangeGuildAttitudes(string name);
	// Tausche Gilden-AttitudenTabelle aus.
	vm->registerExternalFunction("Wld_ExchangeGuildAttitudes", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_ExchangeGuildAttitudes";
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		Externals::Wld_ExchangeGuildAttitudes(name);
	});

	// int Wld_GetDay();
	// Liefert den aktuellen Tag zurück (Zähler), hierbei ist der StartTag (Gamestart) = 0.
	vm->registerExternalFunction("Wld_GetDay", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetDay";
		vm.setReturn(Externals::Wld_GetDay());
		vm.setReturn(0);
	});

	// int Wld_GetFormerPlayerPortalGuild();
	// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
	// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
	// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
	// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert
	vm->registerExternalFunction("Wld_GetFormerPlayerPortalGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetFormerPlayerPortalGuild";
		vm.setReturn(Externals::Wld_GetFormerPlayerPortalGuild());
		vm.setReturn(0);
	});

	// c_npc Wld_GetFormerPlayerPortalOwner();
	// liefert den NSC des Raums, in dem sich der SC vor dem letzten "Raumwechsel" befunden hat, zurück
	// Raumwechsel ist dabei: Außen->Raum1, Raum1->Raum2, -> Raum1->Außen
	// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
	// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'
	vm->registerExternalFunction("Wld_GetFormerPlayerPortalOwner", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetFormerPlayerPortalOwner";
		vm.setReturnVar(Externals::Wld_GetFormerPlayerPortalOwner());
		vm.setReturnVar(0);
	});

	// int Wld_GetGuildAttitude(int guild1, int guild2);
	// Ermittle Gildenattitude
	vm->registerExternalFunction("Wld_GetGuildAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetGuildAttitude";
		int32_t guild2 = vm.popDataValue(); LogInfo() << "guild2: " << guild2;
		int32_t guild1 = vm.popDataValue(); LogInfo() << "guild1: " << guild1;
		vm.setReturn(Externals::Wld_GetGuildAttitude(guild1, guild2));
		vm.setReturn(0);
	});

	// int Wld_GetMobState(c_npc self, string schemeName);
	// Liefert den State zum nächsten Mob mit 'schemeName' zurück bzw. '-1' wenn kein solches Mob gefunden wurde
	vm->registerExternalFunction("Wld_GetMobState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetMobState";
		std::string schemeName = vm.popString(); LogInfo() << "schemeName: " << schemeName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_GetMobState(selfVob, schemeName));
		vm.setReturn(0);
	});

	// int Wld_GetPlayerPortalGuild();
	// liefert Gilde des aktiven Raums, in dem sich der SC gerade befindet, zurück
	// - wenn der SC 'draußen' ist, dann wird GIL_NONE zurückgegeben
	// - wenn der aktive Raum gildenlos ist, dann wird GIL_NONE zurückgeliefert
	vm->registerExternalFunction("Wld_GetPlayerPortalGuild", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetPlayerPortalGuild";
		vm.setReturn(Externals::Wld_GetPlayerPortalGuild());
		vm.setReturn(0);
	});

	// c_npc Wld_GetPlayerPortalOwner();
	// liefert den NSC des aktiven Raums, in dem sich der SC gerade befindet, zurück
	// - wenn der SC 'draußen' ist, dann ist der Rückgabe-Npc 'notValid'
	// - wenn der aktive Raum besitzerlos ist, dann ist der Rückgabe-Npc 'notValid'
	vm->registerExternalFunction("Wld_GetPlayerPortalOwner", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_GetPlayerPortalOwner";
		vm.setReturnVar(Externals::Wld_GetPlayerPortalOwner());
		vm.setReturnVar(0);
	});

	// void Wld_InsertItem(int itemInstance, string spawnPoint);
	// Füge Item in Welt ein entweder an einem WP oder einem FP
	// Vorsicht, funktioniert nicht, Items werden immer im Mittelpunkt der Welt inserted
	vm->registerExternalFunction("Wld_InsertItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_InsertItem";
		std::string spawnPoint = vm.popString(); LogInfo() << "spawnPoint: " << spawnPoint;
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		Externals::Wld_InsertItem(itemInstance, spawnPoint);
	});

	// void Wld_InsertNpc(int npcInstance, string spawnPoint);
	// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf.
	vm->registerExternalFunction("Wld_InsertNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_InsertNpc";
		std::string spawnPoint = vm.popString(); LogInfo() << "spawnPoint: " << spawnPoint;
		int32_t npcInstance = vm.popDataValue(); LogInfo() << "npcInstance: " << npcInstance;
		Externals::Wld_InsertNpc(npcInstance, spawnPoint);
	});

	// void Wld_InsertNpcAndRespawn(int instance, string spawnPoint, float spawnDelay);
	// Füge NSC in Welt ein. Wobei SPawnpoint entweder ein WP oder ein FP sein darf. Stirbt dieser NSC wird
	// nach "spawnDelay"-Sekunden ein neuer NSC am Spawnpoint erzeugt.
	vm->registerExternalFunction("Wld_InsertNpcAndRespawn", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_InsertNpcAndRespawn";
		float spawnDelay = vm.popFloatValue(); LogInfo() << "spawnDelay: " << spawnDelay;
		std::string spawnPoint = vm.popString(); LogInfo() << "spawnPoint: " << spawnPoint;
		int32_t instance = vm.popDataValue(); LogInfo() << "instance: " << instance;
		Externals::Wld_InsertNpcAndRespawn(instance, spawnPoint, spawnDelay);
	});

	// void Wld_InsertObject(string s0, string s1);
	vm->registerExternalFunction("Wld_InsertObject", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_InsertObject";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::Wld_InsertObject(s0, s1);
	});

	// bool Wld_IsFPAvailable(c_npc self, string fpName);
	// Sucht einen Freepoint im Umkreis von 20m vom NSC und liefert TRUE falls vorhanden und frei ('self' zählt als Blockierer nicht!) und sichtbar
	vm->registerExternalFunction("Wld_IsFPAvailable", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_IsFPAvailable";
		std::string fpName = vm.popString(); LogInfo() << "fpName: " << fpName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_IsFPAvailable(selfVob, fpName));
		vm.setReturn(0);
	});

	// bool Wld_IsMobAvailable(c_npc self, string schemeName);
	// Sucht sich ein Mobsi im Umkreis von 10m und liefert TRUE falls gefunden. MOB wird nur zurückgeliefert, wenn es nicht besetzt ist.
	vm->registerExternalFunction("Wld_IsMobAvailable", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_IsMobAvailable";
		std::string schemeName = vm.popString(); LogInfo() << "schemeName: " << schemeName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_IsMobAvailable(selfVob, schemeName));
		vm.setReturn(0);
	});

	// bool Wld_IsNextFPAvailable(c_npc self, string fpName);
	// wie Wld_IsFPAvailable(), aber es wird immer der nahegelegenste genommen und 'self' zählt als Blockierer!
	vm->registerExternalFunction("Wld_IsNextFPAvailable", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_IsNextFPAvailable";
		std::string fpName = vm.popString(); LogInfo() << "fpName: " << fpName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::Wld_IsNextFPAvailable(selfVob, fpName));
		vm.setReturn(0);
	});

	// bool Wld_IsRaining();
	vm->registerExternalFunction("Wld_IsRaining", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_IsRaining";
		vm.setReturn(Externals::Wld_IsRaining());
		vm.setReturn(0);
	});

	// bool Wld_IsTime(int hour1, int min1, int hour2, int min2);
	// Liefert zurück ob die aktuelle Weltzeit zwischen den beiden angegebenen Zeiten liegt (von - bis)
	vm->registerExternalFunction("Wld_IsTime", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_IsTime";
		int32_t min2 = vm.popDataValue(); LogInfo() << "min2: " << min2;
		int32_t hour2 = vm.popDataValue(); LogInfo() << "hour2: " << hour2;
		int32_t min1 = vm.popDataValue(); LogInfo() << "min1: " << min1;
		int32_t hour1 = vm.popDataValue(); LogInfo() << "hour1: " << hour1;
		vm.setReturn(Externals::Wld_IsTime(hour1, min1, hour2, min2));
		vm.setReturn(0);
	});

	// void Wld_PlayEffect(string effectinstance, int originvob, int targetvob, int effectlevel, int damage, int damagetype, int bisprojectile);
	// effectInstance: Name der VisualFX-Instanz
	// originVob: Ursprung/Verursacher (muss existieren!)
	// targetVob: Ziel fuer Effekt + Schaden
	// effectLevel: Bei Angabe von effectLevel wird nach einer VisualFX-Instanz mit dem Namen _L gesucht und verwendet falls sie
	// gefunden wurde. (es muss trotzdem eine VisualFX-Instanz mit dem Namen definiert worden sein!)
	// damage: Hoehe des Schadens
	// damageType: Schadensart (DAM_Xxx)
	// bIsProjectile: Effekt ist Projektil
	vm->registerExternalFunction("Wld_PlayEffect", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_PlayEffect";
		int32_t bisprojectile = vm.popDataValue(); LogInfo() << "bisprojectile: " << bisprojectile;
		int32_t damagetype = vm.popDataValue(); LogInfo() << "damagetype: " << damagetype;
		int32_t damage = vm.popDataValue(); LogInfo() << "damage: " << damage;
		int32_t effectlevel = vm.popDataValue(); LogInfo() << "effectlevel: " << effectlevel;
		int32_t targetvob = vm.popDataValue(); LogInfo() << "targetvob: " << targetvob;
		int32_t originvob = vm.popDataValue(); LogInfo() << "originvob: " << originvob;
		std::string effectinstance = vm.popString(); LogInfo() << "effectinstance: " << effectinstance;
		Externals::Wld_PlayEffect(effectinstance, originvob, targetvob, effectlevel, damage, damagetype, bisprojectile);
	});

	// int Wld_RemoveItem(c_item item);
	// Hiermit wird das angegebene Item aus der Welt entfernt und gelöscht
	vm->registerExternalFunction("Wld_RemoveItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_RemoveItem";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		vm.setReturn(Externals::Wld_RemoveItem(item));
		vm.setReturn(0);
	});

	// void Wld_RemoveNpc(int i0);
	vm->registerExternalFunction("Wld_RemoveNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_RemoveNpc";
		int32_t i0 = vm.popDataValue(); LogInfo() << "i0: " << i0;
		Externals::Wld_RemoveNpc(i0);
	});

	// void Wld_SendTrigger(string vobname);
	// Sendet eine Trigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.
	vm->registerExternalFunction("Wld_SendTrigger", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SendTrigger";
		std::string vobname = vm.popString(); LogInfo() << "vobname: " << vobname;
		Externals::Wld_SendTrigger(vobname);
	});

	// void Wld_SendUntrigger(string vobname);
	// Sendet eine UnTrigger-Nachricht an das VOB (z.B. Mover) mit dem angegeben Namen.
	vm->registerExternalFunction("Wld_SendUntrigger", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SendUntrigger";
		std::string vobname = vm.popString(); LogInfo() << "vobname: " << vobname;
		Externals::Wld_SendUntrigger(vobname);
	});

	// void Wld_SetGuildAttitude(int guild1, int attitude, int guild2);
	// Setze Gildenattitude neu
	vm->registerExternalFunction("Wld_SetGuildAttitude", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SetGuildAttitude";
		int32_t guild2 = vm.popDataValue(); LogInfo() << "guild2: " << guild2;
		int32_t attitude = vm.popDataValue(); LogInfo() << "attitude: " << attitude;
		int32_t guild1 = vm.popDataValue(); LogInfo() << "guild1: " << guild1;
		Externals::Wld_SetGuildAttitude(guild1, attitude, guild2);
	});

	// void Wld_SetMobRoutine(int hour1, int min1, string objName, int state);
	// _Alle_ Mobs mit diesem _Schemanamen_ werden getriggert.
	vm->registerExternalFunction("Wld_SetMobRoutine", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SetMobRoutine";
		int32_t state = vm.popDataValue(); LogInfo() << "state: " << state;
		std::string objName = vm.popString(); LogInfo() << "objName: " << objName;
		int32_t min1 = vm.popDataValue(); LogInfo() << "min1: " << min1;
		int32_t hour1 = vm.popDataValue(); LogInfo() << "hour1: " << hour1;
		Externals::Wld_SetMobRoutine(hour1, min1, objName, state);
	});

	// void Wld_SetObjectRoutine(int hour1, int min1, string objName, int state);
	// _Ein_ Objekt mit diesem _Vobnamen_ wird getriggert
	vm->registerExternalFunction("Wld_SetObjectRoutine", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SetObjectRoutine";
		int32_t state = vm.popDataValue(); LogInfo() << "state: " << state;
		std::string objName = vm.popString(); LogInfo() << "objName: " << objName;
		int32_t min1 = vm.popDataValue(); LogInfo() << "min1: " << min1;
		int32_t hour1 = vm.popDataValue(); LogInfo() << "hour1: " << hour1;
		Externals::Wld_SetObjectRoutine(hour1, min1, objName, state);
	});

	// void Wld_SetTime(int hour, int min);
	// Setzt die Uhrzeit auf hour:min. hour kann größer als 23 sein, um zum nächsten Tag zu springen.
	vm->registerExternalFunction("Wld_SetTime", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SetTime";
		int32_t min = vm.popDataValue(); LogInfo() << "min: " << min;
		int32_t hour = vm.popDataValue(); LogInfo() << "hour: " << hour;
		Externals::Wld_SetTime(hour, min);
	});

	// void Wld_SpawnNpcRange(instance n0, int i1, int i2, float r3);
	vm->registerExternalFunction("Wld_SpawnNpcRange", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_SpawnNpcRange";
		float r3 = vm.popFloatValue(); LogInfo() << "r3: " << r3;
		int32_t i2 = vm.popDataValue(); LogInfo() << "i2: " << i2;
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::Wld_SpawnNpcRange(n0, i1, i2, r3);
	});

	// void Wld_StopEffect(string s0);
	vm->registerExternalFunction("Wld_StopEffect", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "Wld_StopEffect";
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		Externals::Wld_StopEffect(s0);
	});

/*
ai (76)
*/
	// void AI_AimAt(c_npc attacker, c_npc target);
	// NPC zielt mit Fernkampfwaffe auf Target-NPC
	vm->registerExternalFunction("AI_AimAt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_AimAt";
		int32_t target = vm.popVar(); LogInfo() << "target: " << target;
		int32_t attacker = vm.popVar(); LogInfo() << "attacker: " << attacker;
		VobTypes::NpcVobInformation attackerVob = getNPCByInstance(attacker);
		VobTypes::NpcVobInformation targetVob = getNPCByInstance(target);
		if (!attackerVob.isValid() || !targetVob.isValid())
		{
			return;
		}
		Externals::AI_AimAt(attackerVob, targetVob);
	});

	// void AI_AlignToFP(c_npc self);
	// richtet den Nsc am Freepoint aus (im Spacer gesetzte Pfeilrichtung)
	vm->registerExternalFunction("AI_AlignToFP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_AlignToFP";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_AlignToFP(selfVob);
	});

	// void AI_AlignToWP(c_npc self);
	// richtet den Nsc am Waypoint aus (im Spacer gesetzte Pfeilrichtung)
	vm->registerExternalFunction("AI_AlignToWP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_AlignToWP";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_AlignToWP(selfVob);
	});

	// void AI_Ask(c_npc self, func anserYes, func answerNo);
	// die angegeben Instanz (self), gibt eine Frage aus und verweist auf die selbst zu definierenden Funktionen,
	// die für die Fälle Spieler sagt ja (Daumen nach oben) und Spieler sagt nein (Daumen unten) vorhanden sein müssen
	vm->registerExternalFunction("AI_Ask", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Ask";
		uint32_t answerNo = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "answerNo: " << answerNo;
		uint32_t anserYes = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "anserYes: " << anserYes;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_Ask(selfVob, anserYes, answerNo);
	});

	// void AI_AskText(c_npc self, func funcYes, func funcNo, string strYes, string strNo);
	// wie AI_Ask, nur das außer den Funktionen auch noch Strings für die Antworten Ja/Nein mit angegeben werden können
	vm->registerExternalFunction("AI_AskText", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_AskText";
		std::string strNo = vm.popString(); LogInfo() << "strNo: " << strNo;
		std::string strYes = vm.popString(); LogInfo() << "strYes: " << strYes;
		uint32_t funcNo = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "funcNo: " << funcNo;
		uint32_t funcYes = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "funcYes: " << funcYes;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_AskText(selfVob, funcYes, funcNo, strYes, strNo);
	});

	// void AI_Attack(c_npc self);
	// Startet Kampf AI (sollte in der ZS_Attack-Loop stehen)
	// Es wird das interne Ziel verwendet, das mit Npc_SetTarget() oder Npc_GetNextTarget() gesetzt
	// wurde.
	vm->registerExternalFunction("AI_Attack", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Attack";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_Attack(selfVob);
	});

	// void AI_CanSeeNpc(instance n0, instance n1, func f2);
	vm->registerExternalFunction("AI_CanSeeNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_CanSeeNpc";
		uint32_t f2 = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "f2: " << f2;
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_CanSeeNpc(n0, n1, f2);
	});

	// void AI_CombatReactToDamage(instance n0);
	vm->registerExternalFunction("AI_CombatReactToDamage", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_CombatReactToDamage";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_CombatReactToDamage(n0);
	});

	// void AI_ContinueRoutine(c_npc self);
	// Setze Tagesablauf fort
	// Enthält Standup
	vm->registerExternalFunction("AI_ContinueRoutine", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_ContinueRoutine";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_ContinueRoutine(selfVob);
	});

	// void AI_Defend(c_npc self);
	// Der Befehl ist als Overlay-Message implementiert. Das heisst, dass er neben anderen Nachrichten
	// aktiv bleibt. Er wird erst beendet, wenn der NPC eine Parade (ausgeloest durch die Attacke eines
	// anderen NPCs) durchgefuert hat.
	vm->registerExternalFunction("AI_Defend", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Defend";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_Defend(selfVob);
	});

	// void AI_Dodge(c_npc npc);
	// Der Nsc weicht ein Stück nach hinten aus
	vm->registerExternalFunction("AI_Dodge", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Dodge";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			return;
		}
		Externals::AI_Dodge(npcVob);
	});

	// void AI_DrawWeapon(c_npc n0);
	// Equipte Waffe wird gezogen
	vm->registerExternalFunction("AI_DrawWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_DrawWeapon";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::AI_DrawWeapon(n0Vob);
	});

	// void AI_DropItem(c_npc self, int itemid);
	// Item(itemid) wird auf den Boden fallen gelassen
	vm->registerExternalFunction("AI_DropItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_DropItem";
		int32_t itemid = vm.popDataValue(); LogInfo() << "itemid: " << itemid;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_DropItem(selfVob, itemid);
	});

	// void AI_DropMob(instance n0);
	vm->registerExternalFunction("AI_DropMob", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_DropMob";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_DropMob(n0);
	});

	// void AI_EquipArmor(c_npc owner, c_item armorFromOwnersInventory);
	// Ziehe die angebene Rüstung dem NSC "owner" an, diese muss sich in seinem Inventory befinden.
	vm->registerExternalFunction("AI_EquipArmor", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_EquipArmor";
		int32_t armorFromOwnersInventory = vm.popVar(); LogInfo() << "armorFromOwnersInventory: " << armorFromOwnersInventory;
		int32_t owner = vm.popVar(); LogInfo() << "owner: " << owner;
		VobTypes::NpcVobInformation ownerVob = getNPCByInstance(owner);
		if (!ownerVob.isValid())
		{
			return;
		}
		Externals::AI_EquipArmor(ownerVob, armorFromOwnersInventory);
	});

	// void AI_EquipBestArmor(c_npc self);
	// Wunder, Wunder hier wird die beste im Inventory vorhandene Rüstung angezogen
	vm->registerExternalFunction("AI_EquipBestArmor", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_EquipBestArmor";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_EquipBestArmor(selfVob);
	});

	// void AI_EquipBestMeleeWeapon(c_npc self);
	// sucht im Inventory nach der besten Nahkampfwaffe und hängt sie an den Gürtel
	vm->registerExternalFunction("AI_EquipBestMeleeWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_EquipBestMeleeWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_EquipBestMeleeWeapon(selfVob);
	});

	// void AI_EquipBestRangedWeapon(c_npc self);
	// sucht im Inventory nach der besten Fernkampfwaffe und ploppt sie auf den Rücken der Instanz
	vm->registerExternalFunction("AI_EquipBestRangedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_EquipBestRangedWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_EquipBestRangedWeapon(selfVob);
	});

	// void AI_FinishingMove(c_npc self, c_npc other);
	// Führt den logischen Finishing Move inklusive Anis aus den Skripten heraus aus
	vm->registerExternalFunction("AI_FinishingMove", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_FinishingMove";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_FinishingMove(selfVob, otherVob);
	});

	// void AI_Flee(c_npc self);
	// Der Befehl muss, wie AI_Attack(), in der ZS-Loop regelmaessig aufgerufen werden und setzt voraus, dass
	// vorher mit Npc_SetTarget( self, <var c_npc enemy> ) ein Gegner gesetzt wurde, vor dem der Npc fliehen soll.
	vm->registerExternalFunction("AI_Flee", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Flee";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_Flee(selfVob);
	});

	// void AI_GotoFP(c_npc self, string fpName);
	// Sucht sich einen Freepoint im Umkreis von 20m vom NSC, bewegt sich dorthin und richtet sich entsprechend aus.
	// Suchkriterium wie bei Wld_IsFPAvailable()
	vm->registerExternalFunction("AI_GotoFP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_GotoFP";
		std::string fpName = vm.popString(); LogInfo() << "fpName: " << fpName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_GotoFP(selfVob, fpName);
	});

	// void AI_GotoItem(c_npc self, c_item item);
	// "self" geht zu "item"
	vm->registerExternalFunction("AI_GotoItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_GotoItem";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_GotoItem(selfVob, item);
	});

	// void AI_GotoNextFP(c_npc self, string fpName);
	// wie AI_GotoFP() allerdings Suchkriterium wie bei Wld_IsNextFPAvailable()
	vm->registerExternalFunction("AI_GotoNextFP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_GotoNextFP";
		std::string fpName = vm.popString(); LogInfo() << "fpName: " << fpName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_GotoNextFP(selfVob, fpName);
	});

	// void AI_GotoNpc(c_npc self, c_npc other);
	// "self" geht zu "other"
	vm->registerExternalFunction("AI_GotoNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_GotoNpc";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_GotoNpc(selfVob, otherVob);
	});

	// void AI_GotoSound(c_npc n0);
	// Npc läuft zum Sound
	vm->registerExternalFunction("AI_GotoSound", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_GotoSound";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::AI_GotoSound(n0Vob);
	});

	// void AI_GotoWP(c_npc n0, string s0);
	// Npc-Instanz läuft zum namentlich angegeben Waypoint
	vm->registerExternalFunction("AI_GotoWP", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_GotoWP";
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::AI_GotoWP(n0Vob, s0);
	});

	// void AI_LookAt(c_npc self, string name);
	// Schaue auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)
	vm->registerExternalFunction("AI_LookAt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_LookAt";
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_LookAt(selfVob, name);
	});

	// void AI_LookAtNpc(c_npc self, c_npc other);
	// Schaue zu einem NSC
	vm->registerExternalFunction("AI_LookAtNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_LookAtNpc";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_LookAtNpc(selfVob, otherVob);
	});

	// void AI_LookForItem(c_npc self, int instance);
	// gibt die Möglichkeit nach bestimmten Items zu suchen (z.B.:Das goldene Schwert der Zerstörung, wenn vorhanden)
	vm->registerExternalFunction("AI_LookForItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_LookForItem";
		int32_t instance = vm.popDataValue(); LogInfo() << "instance: " << instance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_LookForItem(selfVob, instance);
	});

	// void AI_Output(c_npc self, c_npc target, string outputName);
	// Cutscene mit entsprechender Id wird abgenudelt
	vm->registerExternalFunction("AI_Output", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Output";
		std::string outputName = vm.popString(); LogInfo() << "outputName: " << outputName;
		int32_t target = vm.popVar(); LogInfo() << "target: " << target;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation targetVob = getNPCByInstance(target);
		if (!selfVob.isValid() || !targetVob.isValid())
		{
			return;
		}
		Externals::AI_Output(selfVob, targetVob, outputName);
	});

	// void AI_OutputSVM(c_npc self, c_npc target, string svmname);
	// Outputbefehl um Svms abzuspielen
	vm->registerExternalFunction("AI_OutputSVM", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_OutputSVM";
		std::string svmname = vm.popString(); LogInfo() << "svmname: " << svmname;
		int32_t target = vm.popVar(); LogInfo() << "target: " << target;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation targetVob = getNPCByInstance(target);
		if (!selfVob.isValid() || !targetVob.isValid())
		{
			return;
		}
		Externals::AI_OutputSVM(selfVob, targetVob, svmname);
	});

	// void AI_OutputSVM_Overlay(c_npc self, c_npc target, string svmname);
	// wie AI_OutputSVM, wartet jedoch NICHT mit der Ausführung des nächsten AI_...-Befehls, bis
	// das SVM zuende gespielt wird. (Für Kommentare kurz vor und während dem Kampf!)
	vm->registerExternalFunction("AI_OutputSVM_Overlay", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_OutputSVM_Overlay";
		std::string svmname = vm.popString(); LogInfo() << "svmname: " << svmname;
		int32_t target = vm.popVar(); LogInfo() << "target: " << target;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation targetVob = getNPCByInstance(target);
		if (!selfVob.isValid() || !targetVob.isValid())
		{
			return;
		}
		Externals::AI_OutputSVM_Overlay(selfVob, targetVob, svmname);
	});

	// void AI_PlayAni(c_npc n0, string s0);
	// Npc-Instanz spielt die angegebene Animation ab
	vm->registerExternalFunction("AI_PlayAni", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PlayAni";
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::AI_PlayAni(n0Vob, s0);
	});

	// void AI_PlayAniBS(c_npc npc, string aniname, int bodystate);
	// Beim Abspielen einer Ani mit diesem Befehl kann ein Bodystate angemeldet werden
	vm->registerExternalFunction("AI_PlayAniBS", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PlayAniBS";
		int32_t bodystate = vm.popDataValue(); LogInfo() << "bodystate: " << bodystate;
		std::string aniname = vm.popString(); LogInfo() << "aniname: " << aniname;
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			return;
		}
		Externals::AI_PlayAniBS(npcVob, aniname, bodystate);
	});

	// void AI_PlayCutscene(c_npc self, string csName);
	// Eine Cutscene aus den Scripten heraus starten
	vm->registerExternalFunction("AI_PlayCutscene", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PlayCutscene";
		std::string csName = vm.popString(); LogInfo() << "csName: " << csName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_PlayCutscene(selfVob, csName);
	});

	// void AI_PlayFX(instance n0, instance n1, string s2);
	vm->registerExternalFunction("AI_PlayFX", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PlayFX";
		std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_PlayFX(n0, n1, s2);
	});

	// void AI_PointAt(c_npc self, string name);
	// Zeige auf einen Wegpunkt (Wegpunktname angeben) oder auf ein anderes Objekt (Vobname angeben)
	vm->registerExternalFunction("AI_PointAt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PointAt";
		std::string name = vm.popString(); LogInfo() << "name: " << name;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_PointAt(selfVob, name);
	});

	// void AI_PointAtNpc(c_npc self, c_npc other);
	// Zeige auf einen NSC
	vm->registerExternalFunction("AI_PointAtNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PointAtNpc";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_PointAtNpc(selfVob, otherVob);
	});

	// int AI_PrintScreen(string s0, int i1, int i2, string s3, int i4);
	vm->registerExternalFunction("AI_PrintScreen", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_PrintScreen";
		int32_t i4 = vm.popDataValue(); LogInfo() << "i4: " << i4;
		std::string s3 = vm.popString(); LogInfo() << "s3: " << s3;
		int32_t i2 = vm.popDataValue(); LogInfo() << "i2: " << i2;
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
		vm.setReturn(Externals::AI_PrintScreen(s0, i1, i2, s3, i4));
		vm.setReturn(0);
	});

	// void AI_ProcessInfos(instance n0);
	vm->registerExternalFunction("AI_ProcessInfos", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_ProcessInfos";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_ProcessInfos(n0);
	});

	// void AI_Quicklook(c_npc self, c_npc other);
	// NSC kurz ( 2 sec) anschauen ( nur Kopf bewegt sich )
	vm->registerExternalFunction("AI_Quicklook", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Quicklook";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_Quicklook(selfVob, otherVob);
	});

	// void AI_ReadyMeleeWeapon(c_npc self);
	// Ziehe equippte Nahkampfwaffe
	vm->registerExternalFunction("AI_ReadyMeleeWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_ReadyMeleeWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_ReadyMeleeWeapon(selfVob);
	});

	// void AI_ReadyRangedWeapon(c_npc self);
	// Ziehe equippte Fernkampfwaffe
	vm->registerExternalFunction("AI_ReadyRangedWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_ReadyRangedWeapon";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_ReadyRangedWeapon(selfVob);
	});

	// void AI_ReadySpell(c_npc self, int spellID, int investMana);
	// Lasse zauberspruch auf Hand erscheinen.
	vm->registerExternalFunction("AI_ReadySpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_ReadySpell";
		int32_t investMana = vm.popDataValue(); LogInfo() << "investMana: " << investMana;
		int32_t spellID = vm.popDataValue(); LogInfo() << "spellID: " << spellID;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_ReadySpell(selfVob, spellID, investMana);
	});

	// void AI_RemoveWeapon(c_npc n0);
	// Gezogene Waffe wird weggesteckt
	vm->registerExternalFunction("AI_RemoveWeapon", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_RemoveWeapon";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::AI_RemoveWeapon(n0Vob);
	});

	// void AI_SetNpcsToState(c_npc self, func aiStateFunc, int radius);
	// Setze alle NSCs im Umkreis von x cm in entsprechenden AI-Zustand
	// VORSICHT: Diese Funktion ist Buggy, wird aber derzeit auch NICHT verwendet!
	// -> FINGER WEG!!! (SN)
	vm->registerExternalFunction("AI_SetNpcsToState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_SetNpcsToState";
		int32_t radius = vm.popDataValue(); LogInfo() << "radius: " << radius;
		uint32_t aiStateFunc = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "aiStateFunc: " << aiStateFunc;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_SetNpcsToState(selfVob, aiStateFunc, radius);
	});

	// void AI_SetWalkmode(c_npc n, int n0);
	// gibt an mit welchem Walkmode Run etc der Character durch das Level läuft
	// NPC_RUN               : Rennen
	// NPC_WALK              : Gehen
	// NPC_SNEAK             : Schleichen
	// NPC_RUN_WEAPON        : Rennen mit gezogener Waffe
	// NPC_WALK_WEAPON       : Gehen mit gezogener Waffe
	// NPC_SNEAK_WEAPON      : Schleichen mit gezogener Waffe
	vm->registerExternalFunction("AI_SetWalkmode", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_SetWalkmode";
		int32_t n0 = vm.popDataValue(); LogInfo() << "n0: " << n0;
		int32_t n = vm.popVar(); LogInfo() << "n: " << n;
		VobTypes::NpcVobInformation nVob = getNPCByInstance(n);
		if (!nVob.isValid())
		{
			return;
		}
		Externals::AI_SetWalkmode(nVob, n0);
	});

	// void AI_ShootAt(c_npc attacker, c_npc target);
	// NPC feuert mit Fernkampfwaffe auf Target-NPC
	vm->registerExternalFunction("AI_ShootAt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_ShootAt";
		int32_t target = vm.popVar(); LogInfo() << "target: " << target;
		int32_t attacker = vm.popVar(); LogInfo() << "attacker: " << attacker;
		VobTypes::NpcVobInformation attackerVob = getNPCByInstance(attacker);
		VobTypes::NpcVobInformation targetVob = getNPCByInstance(target);
		if (!attackerVob.isValid() || !targetVob.isValid())
		{
			return;
		}
		Externals::AI_ShootAt(attackerVob, targetVob);
	});

	// void AI_Snd_Play(instance n0, string s1);
	vm->registerExternalFunction("AI_Snd_Play", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Snd_Play";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_Snd_Play(n0, s1);
	});

	// void AI_Snd_Play3D(instance n0, instance n1, string s2);
	vm->registerExternalFunction("AI_Snd_Play3D", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Snd_Play3D";
		std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_Snd_Play3D(n0, n1, s2);
	});

	// void AI_StandUp(c_npc self);
	// - Ist der Nsc in einem Animatinsstate, wird die passende Rücktransition abgespielt.
	// - Benutzt der NSC gerade ein MOBSI, poppt er ins stehen.
	vm->registerExternalFunction("AI_StandUp", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StandUp";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_StandUp(selfVob);
	});

	// void AI_StandUpQuick(c_npc self);
	// Wie AI_StandUp(), jedoch werden keine Rücktransitionen abgespielt, sondern auch dort wird
	// sofort in den Grundzustand "gepoppt". Wichtig für sehr eilige Situationen!
	vm->registerExternalFunction("AI_StandUpQuick", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StandUpQuick";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_StandUpQuick(selfVob);
	});

	// void AI_StartState(c_npc self, func what, int stateBehaviour, string wpName);
	// Versetzt den Nsc aus den Skripten heraus in den entsprechenden ZS (what),
	// stateBehaviour sagt : "0"-aktuellen Zustand abbrechen "1"-aktuellen Zustand erst ordnungsgemäß beenden (End-Funktion aufrufen) ).
	vm->registerExternalFunction("AI_StartState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StartState";
		std::string wpName = vm.popString(); LogInfo() << "wpName: " << wpName;
		int32_t stateBehaviour = vm.popDataValue(); LogInfo() << "stateBehaviour: " << stateBehaviour;
		uint32_t what = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "what: " << what;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_StartState(selfVob, what, stateBehaviour, wpName);
	});

	// void AI_StopAim(c_npc attacker);
	// NPC beendet vorher gestartetes Zielen mit Fernkampfwaffe.
	vm->registerExternalFunction("AI_StopAim", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StopAim";
		int32_t attacker = vm.popVar(); LogInfo() << "attacker: " << attacker;
		VobTypes::NpcVobInformation attackerVob = getNPCByInstance(attacker);
		if (!attackerVob.isValid())
		{
			return;
		}
		Externals::AI_StopAim(attackerVob);
	});

	// void AI_StopFX(instance n0, string s1);
	vm->registerExternalFunction("AI_StopFX", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StopFX";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_StopFX(n0, s1);
	});

	// void AI_StopLookAt(c_npc self);
	// wieder geradeaus schauen
	vm->registerExternalFunction("AI_StopLookAt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StopLookAt";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_StopLookAt(selfVob);
	});

	// void AI_StopPointAt(c_npc self);
	// nicht mehr auf etwas zeigen
	vm->registerExternalFunction("AI_StopPointAt", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StopPointAt";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_StopPointAt(selfVob);
	});

	// void AI_StopProcessInfos(c_npc npc);
	// Der DialogModus wird beendet (Multiple Choice-Dialog)
	vm->registerExternalFunction("AI_StopProcessInfos", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_StopProcessInfos";
		int32_t npc = vm.popVar(); LogInfo() << "npc: " << npc;
		VobTypes::NpcVobInformation npcVob = getNPCByInstance(npc);
		if (!npcVob.isValid())
		{
			return;
		}
		Externals::AI_StopProcessInfos(npcVob);
	});

	// void AI_TakeItem(c_npc self, c_item item);
	// der Npc nimmt mit dieser Funktion die globale Item-Instanz auf
	vm->registerExternalFunction("AI_TakeItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_TakeItem";
		int32_t item = vm.popVar(); LogInfo() << "item: " << item;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_TakeItem(selfVob, item);
	});

	// void AI_TakeMob(instance n0, string s1);
	vm->registerExternalFunction("AI_TakeMob", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_TakeMob";
		std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_TakeMob(n0, s1);
	});

	// void AI_Teleport(c_npc self, string waypoint);
	// teleportiert den NSC zur angegebenene Location
	vm->registerExternalFunction("AI_Teleport", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Teleport";
		std::string waypoint = vm.popString(); LogInfo() << "waypoint: " << waypoint;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_Teleport(selfVob, waypoint);
	});

	// void AI_TurnAway(c_npc n0, c_npc n1);
	// Der NSC "self" dreht dem NSC "other" den Rücken zu.
	vm->registerExternalFunction("AI_TurnAway", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_TurnAway";
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		VobTypes::NpcVobInformation n1Vob = getNPCByInstance(n1);
		if (!n0Vob.isValid() || !n1Vob.isValid())
		{
			return;
		}
		Externals::AI_TurnAway(n0Vob, n1Vob);
	});

	// void AI_TurnToNpc(c_npc n0, c_npc n1);
	// drehe Dich zum angegeben (zweiten) Npc um
	vm->registerExternalFunction("AI_TurnToNpc", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_TurnToNpc";
		int32_t n1 = vm.popVar(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		VobTypes::NpcVobInformation n1Vob = getNPCByInstance(n1);
		if (!n0Vob.isValid() || !n1Vob.isValid())
		{
			return;
		}
		Externals::AI_TurnToNpc(n0Vob, n1Vob);
	});

	// void AI_TurnToSound(c_npc self);
	// Charakter dreht sich zur Geräuschquelle
	vm->registerExternalFunction("AI_TurnToSound", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_TurnToSound";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_TurnToSound(selfVob);
	});

	// void AI_UnequipArmor(c_npc self);
	// Unequippe aktuelle Rüstung
	vm->registerExternalFunction("AI_UnequipArmor", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_UnequipArmor";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_UnequipArmor(selfVob);
	});

	// void AI_UnequipWeapons(c_npc self);
	// Unequippe alle Waffen
	vm->registerExternalFunction("AI_UnequipWeapons", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_UnequipWeapons";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_UnequipWeapons(selfVob);
	});

	// void AI_UnreadySpell(c_npc self);
	// lasse zauberspruch aus Hand verschwinden
	vm->registerExternalFunction("AI_UnreadySpell", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_UnreadySpell";
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_UnreadySpell(selfVob);
	});

	// void AI_UseItem(c_npc self, int itemInstance);
	// Item bis zum Ende benutzen
	vm->registerExternalFunction("AI_UseItem", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_UseItem";
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_UseItem(selfVob, itemInstance);
	});

	// void AI_UseItemToState(c_npc self, int itemInstance, int state);
	// Item benutzen bis zum angegebenen State
	vm->registerExternalFunction("AI_UseItemToState", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_UseItemToState";
		int32_t state = vm.popDataValue(); LogInfo() << "state: " << state;
		int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_UseItemToState(selfVob, itemInstance, state);
	});

	// int AI_UseMob(c_npc self, string schemeName, int targetState);
	// Benutze Mob mit angegebenen Schema-Namen bis zum Zustand "targetState". Wird diese Funktion aufgerufen
	// und der angegebene 'targetState' ist bereits vorhanden, läuft der NSC zwar trotzdem zum MOB, tut dann aber nichts
	vm->registerExternalFunction("AI_UseMob", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_UseMob";
		int32_t targetState = vm.popDataValue(); LogInfo() << "targetState: " << targetState;
		std::string schemeName = vm.popString(); LogInfo() << "schemeName: " << schemeName;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			vm.setReturn(0);
			return;
		}
		vm.setReturn(Externals::AI_UseMob(selfVob, schemeName, targetState));
		vm.setReturn(0);
	});

	// void AI_Wait(c_npc n0, float n1);
	// Character wird für n1 Sekunden in einen Wait-Zustand versetzt,
	// d.h. er tut nichts, aber Treffer etc.(passive Wahrnehmung) werden registriert
	vm->registerExternalFunction("AI_Wait", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_Wait";
		float n1 = vm.popFloatValue(); LogInfo() << "n1: " << n1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		VobTypes::NpcVobInformation n0Vob = getNPCByInstance(n0);
		if (!n0Vob.isValid())
		{
			return;
		}
		Externals::AI_Wait(n0Vob, n1);
	});

	// void AI_WaitForQuestion(c_npc self, func scriptFunc);
	// NSC wartet 20 Sekunden, wird er in dieser Zeit vom Spieler angesprochen, wird die angegebene SkriptFunktion ausgeführt.
	vm->registerExternalFunction("AI_WaitForQuestion", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_WaitForQuestion";
		uint32_t scriptFunc = static_cast<uint32_t>(vm.popDataValue()); LogInfo() << "scriptFunc: " << scriptFunc;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		if (!selfVob.isValid())
		{
			return;
		}
		Externals::AI_WaitForQuestion(selfVob, scriptFunc);
	});

	// void AI_WaitMS(instance n0, int i1);
	vm->registerExternalFunction("AI_WaitMS", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_WaitMS";
		int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_WaitMS(n0, i1);
	});

	// void AI_WaitTillEnd(c_npc self, c_npc other);
	// 'self' wartet bis 'other' seinen laufenden AI-Befehl zu Ende gespielt hat (funzt allerdings nicht bei AI-Overlays!)
	vm->registerExternalFunction("AI_WaitTillEnd", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_WaitTillEnd";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_WaitTillEnd(selfVob, otherVob);
	});

	// void AI_WhirlAround(c_npc self, c_npc other);
	// schnelle Drehung zu other
	vm->registerExternalFunction("AI_WhirlAround", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_WhirlAround";
		int32_t other = vm.popVar(); LogInfo() << "other: " << other;
		int32_t self = vm.popVar(); LogInfo() << "self: " << self;
		VobTypes::NpcVobInformation selfVob = getNPCByInstance(self);
		VobTypes::NpcVobInformation otherVob = getNPCByInstance(other);
		if (!selfVob.isValid() || !otherVob.isValid())
		{
			return;
		}
		Externals::AI_WhirlAround(selfVob, otherVob);
	});

	// void AI_WhirlAroundToSource(instance n0);
	vm->registerExternalFunction("AI_WhirlAroundToSource", [=](Daedalus::DaedalusVM& vm) {
		if (verbose) LogInfo() << "AI_WhirlAroundToSource";
		int32_t n0 = vm.popVar(); LogInfo() << "n0: " << n0;
		Externals::AI_WhirlAroundToSource(n0);
	});

