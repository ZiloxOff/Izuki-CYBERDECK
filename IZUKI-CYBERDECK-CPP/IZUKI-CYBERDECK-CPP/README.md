# IZUKI CYBERDECK 0.1.0 Alpha — version C++ (IZUKI TEAM)
C++17, bibliothèque standard + API POSIX/Linux uniquement. Aucune dépendance externe.

    make          # compile ./cyberdeck
    make test     # compile et lance les tests
    ./cyberdeck stats | sha256 abc | json '{"a":1}' | resolve example.com | conns | plugins

## État réel
Implémenté et testé (Linux) : SHA-256, Base64, hex, URL, JSON (validation stricte), regex, compteur de texte,
mot de passe, UUID v4, timestamp, stats système (CPU/RAM/disque/uptime/IP via /proc), résolution DNS (getaddrinfo),
connexions TCP locales + règle de ports suspects, plugins (manifeste + processus séparé, timeout 10 s, pas un sandbox),
historique en fichier plat.

NON implémenté : interface graphique Qt/QML (Qt absent à la construction), terminal PTY/ConPTY, SHA-512/MD5,
Base de données SQLite, requêtes DNS typées (MX/TXT/…), WHOIS, traceroute, ping, TLS, Red Team, OSINT, Tool Manager,
support Windows (le monitoring lit /proc), GPU/températures, packaging.
Les fonctions réseau non testées en conditions réelles (pas d'Internet dans l'environnement de build) : resolve vérifié sur localhost seulement.
