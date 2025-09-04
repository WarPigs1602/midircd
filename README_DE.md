# midircd � Moderner IRC-Server (basiert auf ircu, Fork von snircd)

Zuletzt aktualisiert: 2025-09-04

**midircd** ist ein leistungsstarker, hoch konfigurierbarer IRC-Daemon mit modernen Funktionen und umfangreicher Kontrolle f�r Channels und Benutzer. Ziel ist es, einen sicheren, flexiblen und funktionsreichen IRC-Server bereitzustellen.

---

## Hauptfunktionen

- **Konfigurierbares Feature-System:**  
  Alle Serverfunktionen k�nnen zur Laufzeit oder �ber die Konfiguration (F-Lines) aktiviert oder deaktiviert werden. Siehe `include/ircd_features.h` und `doc/readme.midircd`.

- **Erweiterte Channel-Modi:**  
  - `+c`: Blockiert mIRC/ANSI-Farbcodes  
  - `+c`: Blockiert CTCPs  
  - `+N`: Blockiert Channel-Notices  
  - `+M`: Nur authentifizierte Nutzer d�rfen sprechen  
  - `+T`: Blockiert Multi-Target-Nachrichten  
  - `+D/+d`: Verz�gerter Beitritt (unsichtbar bis zur Aktivit�t)  
  - `+u`: Versteckt Quit/Part-Nachrichten  
  - `+j`: Anti-Join-Flood  
  - `+L`: Channel-Verlinkung  
  - `+e`: Bann-Ausnahme  
  - `+Z`: Nur TLS-Channel

- **Erweiterte Nutzer- und Oper-Modi:**  
  - `+X`: Oper-Override f�r Channel-Beschr�nkungen  
  - `+k`: Schutz f�r Netzwerkdienste (kein Kick/Deop/Kill)  
  - `+n`: Versteckt Channels in /whois  
  - `+I`: Versteckt Idle-Zeit  
  - `+R`: Akzeptiert nur Nachrichten von authentifizierten Nutzern  
  - Viele weitere (siehe unten und `doc/readme.midircd`)

- **IRCv3-Unterst�tzung:**  
  - F�higkeiten:  
    - `sasl` (PLAIN, EXTERNAL, SCRAM, etc.)  
    - `account-notify`, `away-notify`, `chghost`, `echo-message`, `extended-join`, `invite-notify`,  
      `message-tags`, `standard-replies`, `draft/channel-rename`

- **SASL-Authentifizierung:**  
  Volle SASL-Unterst�tzung mit Weiterleitung an konfigurierbare Dienste.

- **Sethost:**  
  Opers und autorisierte Nutzer k�nnen ihren Hostnamen �ndern (Spoofing), mit feingranularer Steuerung �ber S:- und F:-Lines.

- **Automatische Channel-Modi:**  
  Neue Channels erhalten Standardmodi (z.B. `ntCN`), konfigurierbar �ber `AUTOCHANMODES` und `AUTOCHANMODES_LIST`.

- **Erweiterte /check- und /stats-Befehle:**  
  Detaillierte Informationen zu Channels, Nutzern und Servern.

- **MOTD-Verwaltung:**  
  Mehrere MOTDs, Auswahl nach Hostmaske/IP/Klasse, Caching, dynamisches Nachladen, Banner-Unterst�tzung.

- **Anti-Abuse:**  
  Klon- und Flood-Limits, IP-Pr�fungen, TLS-only-Channels, verbessertes Host-Hiding/Cloaking, G-Lines, Schutz vor Join/Klon/Nick-Missbrauch.

- **HEAD_IN_SAND-Features:**  
  Granulare Kontrolle �ber Sichtbarkeit und Zug�nglichkeit von Serverinformationen und Statistiken (siehe `doc/readme.midircd`).

- **Service-Weiterleitung:**  
  SASL und andere Authentifizierungsbefehle k�nnen an einen konfigurierbaren Dienst weitergeleitet werden.

- **Erweitertes Logging und Debugging:**  
  Flexibles Logging, Debug-Level und Feature-Reporting.

---

## Channel-Privilegien & Hierarchie

| Modus | Pr�fix | Name         | Beschreibung                                                            |
|-------|--------|--------------|-------------------------------------------------------------------------|
| +S    | !      | ChanService  | Channel-Service-Bot, h�chste Berechtigung, kann alle Beschr�nkungen umgehen |
| +q    | ~      | Owner        | Channel-Besitzer, kann alle niedrigeren Modi setzen/entfernen und Channel verwalten |
| +a    | &      | Admin        | Channel-Admin, kann die meisten niedrigeren Modi setzen und Channel verwalten |
| +o    | @      | Operator     | Channel-Operator, kann Nutzer verwalten und Channel moderieren          |
| +h    | %      | Half-Op      | Eingeschr�nkte Moderationsrechte (kann keine h�heren Modi setzen)       |
| +v    | +      | Voice        | Nutzer mit Voice, kann in moderierten (+m) Channels sprechen           |

**Hierarchie:**  
ChanService > Owner > Admin > Operator > Half-Op > Voice > normaler Nutzer

- Nur Nutzer mit h�herer Berechtigung k�nnen Modi f�r Nutzer mit niedrigerer Berechtigung setzen oder entfernen.
- Half-Ops k�nnen keine Operator-/Admin-/Owner-Modi setzen oder entfernen.
- Channel-Service-Bots (+S) k�nnen alle Beschr�nkungen umgehen.
- Das Privilegiensystem wird f�r alle Mode-�nderungen, Kicks und andere Moderationsaktionen durchgesetzt.

---

## Channel-Typen

- `#`: Globaler Channel (netzwerkweit)
- `&`: Lokaler Channel (serverlokal)
- `!`: Netzwerk-Channel (eindeutige ID, erweiterte Funktionen)
- `+`: Modelloser Channel (keine Ops/Modi)

---

## Wichtige IRC-Befehle

Alle Befehle verwenden die `/BEFEHL`-Syntax. G�ngige Befehle (siehe `/HELP` im IRC f�r Details):

- `/ADMIN [server]` � Zeigt Admin-Informationen
- `/AWAY [Nachricht]` � Setzt Abwesenheitsstatus
- `/BYE`, `/EXIT`, `/QUIT` � Sitzung beenden
- `/CHANNEL [channel]` � Channel betreten
- `/CLEAR` � Bildschirm leeren
- `/HELP [Befehl]` � Hilfe anzeigen
- `/CHECK` � Status von Channels, Nutzern, Servern anzeigen
- `/STATS [Typ]` � Statistiken anzeigen (a/c/d/e/f/g/i/j/J etc.)
- `/MODE` � Channel-/Nutzermodi setzen
- `/SETHOST` � Hostname �ndern (Opers)
- `/OPER` � Operator werden
- `/WHOIS`, `/WHOWAS`, `/LIST`, `/NAMES`, `/TOPIC`, `/KICK`, `/BAN`, `/RENAME`, etc.

---

## Wichtige Einstellungen / Limits

- Maximale Channels pro Nutzer: 10
- Maximale Nickl�nge: 15
- Maximale Bans pro Channel: 45

---

## Build (kurz)

Dieses Projekt verwendet GNU Autotools. Eine minimale Build-Sequenz:

```bash
./configure
make
# optional als root: make install
```

Benötigt werden ein C-Compiler (gcc/clang), make und die Autotools (autoconf/automake). Für TLS-Unterstützung installiert die OpenSSL-Entwicklerpakete (z.B. libssl-dev).

## Mitwirken

Kleine Dokumentationsänderungen, Patches und Issues sind willkommen. Bitte eröffne ein GitHub-Issue oder einen Pull Request. Für Code-Änderungen beschreibe kurz die Änderung und füge, wenn möglich, einen Test oder eine kurze Prüfung bei.
- Maximale Ban-L�nge: 40 Zeichen
- Klon-Limit pro IP: 4
- Channel-L�nge: 200 Zeichen
- Standard-Port: 4400
- TLS: TLSv1.1, TLSv1.2, TLSv1.3 unterst�tzt

---

## Zus�tzliche Funktionen

- Host-Cloaking und -Hiding (konfigurierbar)
- WebIRC-Cloaking
- Erweiterte Logging- und Debugging-Optionen
- Automatisches Operator-Level-Management
- Lokale Channels, Zannels, Topic-Burst
- Vorgefertigte Service- und Feature-URLs

---

## Feature-Konfiguration (F-Lines)

Alle Features k�nnen �ber F-Lines in der Konfigurationsdatei aktiviert/deaktiviert oder konfiguriert werden.  
Siehe `include/ircd_features.h` und `doc/readme.midircd` f�r eine vollst�ndige Liste und Dokumentation.

**Beispiele:**
- `F:SETHOST:TRUE` � /sethost-Befehl f�r Opers aktivieren
- `F:SETHOST_USER:TRUE` � /sethost f�r Nutzer mit Passwort aktivieren
- `F:SETHOST_AUTO:TRUE` � Automatisches Sethost bei passender Ident/Host aktivieren
- `F:AUTOCHANMODES:TRUE` � Automatische Standard-Channel-Modi aktivieren
- `F:AUTOCHANMODES_LIST:ntCN` � Standard-Channel-Modi f�r neue Channels setzen
- `F:MAXCHANNELSPERUSER:10` � Maximale Channels pro Nutzer setzen
- `F:SOCKSENDBUF:61440` � Socket-Sendepuffergr��e setzen
- `F:SOCKRECVBUF:61440` � Socket-Empfangspuffergr��e setzen
- `F:USER_HIDECHANS:TRUE` � Nutzern erlauben, Channels in /whois zu verstecken
- `F:HIS_USERGLINE:TRUE` � /GLINE-Befehl f�r Nutzer aktivieren
- `F:AUTOINVISIBLE:TRUE` � +i (unsichtbar) f�r alle neuen Nutzer setzen
- `F:HIS_USERIP:TRUE` � /USERIP f�r Nutzer aktivieren/deaktivieren

Siehe `doc/readme.midircd` f�r alle verf�gbaren F-Lines und deren Wirkung.

---

## ISUPPORT / 005-Tokens

midircd k�ndigt folgende ISUPPORT-Tokens an (siehe `include/supported.h`):

- `CHANTYPES=#&!+`
- `PREFIX=(Sqaohv)!~&@%+`
- `CHANMODES=b,e,j,k,l,L,imnpstrDducCNMTZ`
- `NICKLEN=15`
- `MAXCHANNELS=10`
- `MAXBANS=45`
- `CHANNELLEN=200`
- `EXCEPTS`
- `CHANID`
- `!CHAN`
- `USERIP`
- `WALLCHOPS`
- `WALLVOICES`
- `CPRIVMSG`
- `CNOTICE`
- `SILENCE`
- `MODES`
- `STATUSMSG`
- `CASEMAPPING`
- `NETWORK`
- ...und mehr

---

## Dokumentation & Hilfe

F�r detaillierte Informationen zu Features, Konfiguration und Befehlen siehe:  
- `doc/irc.1` � Nutzerbefehle  
- `doc/ircd.8` � Serveroptionen  
- `ircd/ircd_features.c` & `include/ircd_features.h` � Feature-Definitionen  
- `doc/readme.midircd` � Feature- und Konfigurationsdokumentation

---

## Lizenz

Siehe LICENSE-Datei im Repository.

---

**Kontakt/Support:**  
Fragen und Beitr�ge �ber GitHub-Issue oder direkt an die Projektbetreuer.