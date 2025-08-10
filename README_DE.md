# midircd – Moderner IRC-Server (basiert auf ircu, Fork von snircd)

**midircd** ist ein leistungsstarker, hoch konfigurierbarer IRC-Daemon mit modernen Funktionen und umfangreicher Kontrolle für Channels und Benutzer. Ziel ist es, einen sicheren, flexiblen und funktionsreichen IRC-Server bereitzustellen.

---

## Hauptfunktionen

- **Konfigurierbares Feature-System:**  
  Alle Serverfunktionen können zur Laufzeit oder über die Konfiguration (F-Lines) aktiviert oder deaktiviert werden. Siehe `include/ircd_features.h` und `doc/readme.midircd`.

- **Erweiterte Channel-Modi:**  
  - `+c`: Blockiert mIRC/ANSI-Farbcodes  
  - `+C`: Blockiert CTCPs  
  - `+N`: Blockiert Channel-Notices  
  - `+M`: Nur authentifizierte Nutzer dürfen sprechen  
  - `+T`: Blockiert Multi-Target-Nachrichten  
  - `+D/+d`: Verzögerter Beitritt (unsichtbar bis zur Aktivität)  
  - `+u`: Versteckt Quit/Part-Nachrichten  
  - `+j`: Anti-Join-Flood  
  - `+L`: Channel-Verlinkung  
  - `+e`: Bann-Ausnahme  
  - `+Z`: Nur TLS-Channel

- **Erweiterte Nutzer- und Oper-Modi:**  
  - `+X`: Oper-Override für Channel-Beschränkungen  
  - `+k`: Schutz für Netzwerkdienste (kein Kick/Deop/Kill)  
  - `+n`: Versteckt Channels in /whois  
  - `+I`: Versteckt Idle-Zeit  
  - `+R`: Akzeptiert nur Nachrichten von authentifizierten Nutzern  
  - Viele weitere (siehe unten und `doc/readme.midircd`)

- **IRCv3-Unterstützung:**  
  - Fähigkeiten:  
    - `sasl` (PLAIN, EXTERNAL, SCRAM, etc.)  
    - `account-notify`, `away-notify`, `chghost`, `echo-message`, `extended-join`, `invite-notify`,  
      `message-tags`, `standard-replies`, `draft/channel-rename`

- **SASL-Authentifizierung:**  
  Volle SASL-Unterstützung mit Weiterleitung an konfigurierbare Dienste.

- **Sethost:**  
  Opers und autorisierte Nutzer können ihren Hostnamen ändern (Spoofing), mit feingranularer Steuerung über S:- und F:-Lines.

- **Automatische Channel-Modi:**  
  Neue Channels erhalten Standardmodi (z.B. `ntCN`), konfigurierbar über `AUTOCHANMODES` und `AUTOCHANMODES_LIST`.

- **Erweiterte /check- und /stats-Befehle:**  
  Detaillierte Informationen zu Channels, Nutzern und Servern.

- **MOTD-Verwaltung:**  
  Mehrere MOTDs, Auswahl nach Hostmaske/IP/Klasse, Caching, dynamisches Nachladen, Banner-Unterstützung.

- **Anti-Abuse:**  
  Klon- und Flood-Limits, IP-Prüfungen, TLS-only-Channels, verbessertes Host-Hiding/Cloaking, G-Lines, Schutz vor Join/Klon/Nick-Missbrauch.

- **HEAD_IN_SAND-Features:**  
  Granulare Kontrolle über Sichtbarkeit und Zugänglichkeit von Serverinformationen und Statistiken (siehe `doc/readme.midircd`).

- **Service-Weiterleitung:**  
  SASL und andere Authentifizierungsbefehle können an einen konfigurierbaren Dienst weitergeleitet werden.

- **Erweitertes Logging und Debugging:**  
  Flexibles Logging, Debug-Level und Feature-Reporting.

---

## Channel-Privilegien & Hierarchie

| Modus | Präfix | Name         | Beschreibung                                                            |
|-------|--------|--------------|-------------------------------------------------------------------------|
| +S    | !      | ChanService  | Channel-Service-Bot, höchste Berechtigung, kann alle Beschränkungen umgehen |
| +q    | ~      | Owner        | Channel-Besitzer, kann alle niedrigeren Modi setzen/entfernen und Channel verwalten |
| +a    | &      | Admin        | Channel-Admin, kann die meisten niedrigeren Modi setzen und Channel verwalten |
| +o    | @      | Operator     | Channel-Operator, kann Nutzer verwalten und Channel moderieren          |
| +h    | %      | Half-Op      | Eingeschränkte Moderationsrechte (kann keine höheren Modi setzen)       |
| +v    | +      | Voice        | Nutzer mit Voice, kann in moderierten (+m) Channels sprechen           |

**Hierarchie:**  
ChanService > Owner > Admin > Operator > Half-Op > Voice > normaler Nutzer

- Nur Nutzer mit höherer Berechtigung können Modi für Nutzer mit niedrigerer Berechtigung setzen oder entfernen.
- Half-Ops können keine Operator-/Admin-/Owner-Modi setzen oder entfernen.
- Channel-Service-Bots (+S) können alle Beschränkungen umgehen.
- Das Privilegiensystem wird für alle Mode-Änderungen, Kicks und andere Moderationsaktionen durchgesetzt.

---

## Channel-Typen

- `#`: Globaler Channel (netzwerkweit)
- `&`: Lokaler Channel (serverlokal)
- `!`: Netzwerk-Channel (eindeutige ID, erweiterte Funktionen)
- `+`: Modelloser Channel (keine Ops/Modi)

---

## Wichtige IRC-Befehle

Alle Befehle verwenden die `/BEFEHL`-Syntax. Gängige Befehle (siehe `/HELP` im IRC für Details):

- `/ADMIN [server]` — Zeigt Admin-Informationen
- `/AWAY [Nachricht]` — Setzt Abwesenheitsstatus
- `/BYE`, `/EXIT`, `/QUIT` — Sitzung beenden
- `/CHANNEL [channel]` — Channel betreten
- `/CLEAR` — Bildschirm leeren
- `/HELP [Befehl]` — Hilfe anzeigen
- `/CHECK` — Status von Channels, Nutzern, Servern anzeigen
- `/STATS [Typ]` — Statistiken anzeigen (a/c/d/e/f/g/i/j/J etc.)
- `/MODE` — Channel-/Nutzermodi setzen
- `/SETHOST` — Hostname ändern (Opers)
- `/OPER` — Operator werden
- `/WHOIS`, `/WHOWAS`, `/LIST`, `/NAMES`, `/TOPIC`, `/KICK`, `/BAN`, `/RENAME`, etc.

---

## Wichtige Einstellungen / Limits

- Maximale Channels pro Nutzer: 10
- Maximale Nicklänge: 15
- Maximale Bans pro Channel: 45
- Maximale Ban-Länge: 40 Zeichen
- Klon-Limit pro IP: 4
- Channel-Länge: 200 Zeichen
- Standard-Port: 4400
- TLS: TLSv1.1, TLSv1.2, TLSv1.3 unterstützt

---

## Zusätzliche Funktionen

- Host-Cloaking und -Hiding (konfigurierbar)
- WebIRC-Cloaking
- Erweiterte Logging- und Debugging-Optionen
- Automatisches Operator-Level-Management
- Lokale Channels, Zannels, Topic-Burst
- Vorgefertigte Service- und Feature-URLs

---

## Feature-Konfiguration (F-Lines)

Alle Features können über F-Lines in der Konfigurationsdatei aktiviert/deaktiviert oder konfiguriert werden.  
Siehe `include/ircd_features.h` und `doc/readme.midircd` für eine vollständige Liste und Dokumentation.

**Beispiele:**
- `F:SETHOST:TRUE` — /sethost-Befehl für Opers aktivieren
- `F:SETHOST_USER:TRUE` — /sethost für Nutzer mit Passwort aktivieren
- `F:SETHOST_AUTO:TRUE` — Automatisches Sethost bei passender Ident/Host aktivieren
- `F:AUTOCHANMODES:TRUE` — Automatische Standard-Channel-Modi aktivieren
- `F:AUTOCHANMODES_LIST:ntCN` — Standard-Channel-Modi für neue Channels setzen
- `F:MAXCHANNELSPERUSER:10` — Maximale Channels pro Nutzer setzen
- `F:SOCKSENDBUF:61440` — Socket-Sendepuffergröße setzen
- `F:SOCKRECVBUF:61440` — Socket-Empfangspuffergröße setzen
- `F:USER_HIDECHANS:TRUE` — Nutzern erlauben, Channels in /whois zu verstecken
- `F:HIS_USERGLINE:TRUE` — /GLINE-Befehl für Nutzer aktivieren
- `F:AUTOINVISIBLE:TRUE` — +i (unsichtbar) für alle neuen Nutzer setzen
- `F:HIS_USERIP:TRUE` — /USERIP für Nutzer aktivieren/deaktivieren

Siehe `doc/readme.midircd` für alle verfügbaren F-Lines und deren Wirkung.

---

## ISUPPORT / 005-Tokens

midircd kündigt folgende ISUPPORT-Tokens an (siehe `include/supported.h`):

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

Für detaillierte Informationen zu Features, Konfiguration und Befehlen siehe:  
- `doc/irc.1` — Nutzerbefehle  
- `doc/ircd.8` — Serveroptionen  
- `ircd/ircd_features.c` & `include/ircd_features.h` — Feature-Definitionen  
- `doc/readme.midircd` — Feature- und Konfigurationsdokumentation

---

## Lizenz

Siehe LICENSE-Datei im Repository.

---

**Kontakt/Support:**  
Fragen und Beiträge über GitHub-Issue oder direkt an die Projektbetreuer.