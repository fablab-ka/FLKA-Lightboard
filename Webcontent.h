// Diese Datei nutzt das ISO8859.1 Encoding
// Die Arduino IDE konvertiert seit Version 1.6.6 automatisch nach UTF-8
// Sobald diese Datei in der Arduino-IDE editiert wird, wird der Inhalt nach UTF-8 konvertiert!
// Daher bitte in externem Editor öffnen.

const char HTML_HEAD[]             PROGMEM = "<html><head><title>{title}</title></head>";
const char HTML_BASIC_STYLES[]     PROGMEM = "<style>.table{display: table;border: 1px solid blue;padding: 4px;} .cell{display: table-cell;border:none;padding:3px;} .SSID{width:20em;} .QUALITY{width:3em;} .SECURITY{width:7em;} .CHANNEL{width:2em;} .MAC{align:left;} .row{display: table-row;border:none;padding:1px;}</style>";
const char HTML_BODYSTART[]        PROGMEM = "<body>";
const char HTML_BODYEND[]          PROGMEM = "</body></html>";
const char HTML_SELECTSCRIPT[]     PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTML_ACCESSPOINT_INFO[] PROGMEM = "<div class=\"cell SSID\"><a href='#p' onclick='c(this)'>{wlan}</a></div><div class=\"cell QUALITY\">{quality}</div><div class=\"cell SECURITY\">{security}</div><div class=\"cell CHANNEL\">{ch}</div><div class=\"cell MAC\">{mac}</div>";
const char HTML_DIVTABLESTART[]    PROGMEM = "<div class=\"table\">";
const char HTML_ROWTABLESTART[]    PROGMEM = "<div class=\"row\">";
const char HTML_DIVEND[]           PROGMEM = "</div>";
const char HTML_FORM_SSID_PWD[]    PROGMEM = "<br/><form method='post' action='/wifiRestartAP'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/><br/><br/><button type='submit'>Speichern</button></form><br/>";
const char HTML_LINK_SCAN_CONFIG[] PROGMEM = "<a href=\"/wificonnectAP\">Suche und Verbinde zu WLANs</a>";
const char HTML_LINK_AP_CONFIG[]   PROGMEM = "<a href=\"/wifiRestartSTA\">Konfiguriere als Accesspoint</a>";
const char HTML_LINK_WPS[]         PROGMEM = "<a href=\"/wpsconfig\">Verbinde per WPS (Pushbutton)</a>";
const char HTML_NO_WLAN_FOUND[]    PROGMEM = "<b>Es wurden keine WLANs gefunden. Reload, um nochmal zu suchen!</b>";
const char HTML_MSG_RESTART_STA[]  PROGMEM = "<h1>Versuche zu WLAN zu verbinden!</h1><br/><b> Wenn keine Verbindung möglich ist, startet das System wieder als Accesspoint.</b><br/> IP <b>192.168.4.1</b>&nbsp;&nbsp;&nbsp;SSID: <b>{SSID}</b>&nbsp;&nbsp;&nbsp; Passwort:<b>{PWD}</b><br/><hr/>";
const char HTML_MSG_RESTART_AP[]   PROGMEM = "<h1>Restart erfolgt als Access-Point!</h1><br/>IP: <b>192.168.4.1</b>  SSID: <b>{SSID}</b> Passwort: <b>{PWD}</b><br/><hr/>";
const char HTML_GLOBAL_CONF[]      PROGMEM = "<form method=\'post\' action=\'/wificonfigAP\'><fieldset class=\"table\"><legend>WiFi Accesspoint Konfiguration</legend><div class=\"row\"><div class=\"cell\">SSID: </div>    <div class=\"cell\"><input name=\"SSID\" id=\"SSID\" value=\"{SSID}\" size=30></div></div><div class=\"row\"><div class=\"cell\">Passwort: </div><div class=\"cell\"><input name=\"WLANPWD\" id=\"WLANPWD\" value=\"{WLANPWD}\" size=30></div></div><div class=\"row\"><div class=\"cell\">Kanal (1-13) </div>    <div class=\"cell\"><input type=\"number\" min=\"1\" max=\"13\" name=\"Kanal\" id=\"Kanal\" value=\"{CH}\" size=5></div></div></fieldset>  <br><fieldset class=\"table\"><legend>Sonstige Konfiguration</legend><div class=\"row\"><div class=\"cell\">Administrator-Passwort: </div><div class=\"cell\"><input name=\"ADMPWD\" id=\"ADMPWD\" value=\"{ADMPWD}\" size=20>&nbsp;&nbsp;Wiederholung: <input name=\"ADMPWD2\" id=\"ADMPWD2\" value=\"{ADMPWD}\" size=20><br> (Benutzer: <i><b>admin</b></i>)</div></div></fieldset><br><button type=\'submit\'>Speichern</button></form>";
const char HTML_REDIRECT_CONTENT[] PROGMEM = "<html><head><meta http-equiv=\"refresh\" content=\"1;url=/content\"/></head><body><h1>Konfiguration gespeichert</h1></body></html>";
const char HTML_TITLE_CONFIG[]     PROGMEM = "Allgemeine Konfiguration";
const char HTML_TITLE_APCONNECT[]  PROGMEM = "Suche und Verbinde zu WLANs";
const char HTML_PWD_WARN[]         PROGMEM = "<h1 style=\"color: red;\">Die beiden Administrationspasswörter sind nicht gleich oder leer!</h1>";
const char JS_SORT_BY_KEY[]        PROGMEM = "function sbk(array, key) {return array.sort(function(a, b) {var x = a[key]; var y = b[key]; return ((x < y) ? -1 : ((x > y) ? 1 : 0)); });}";
const char JS_DO_GET_REQUEST[]     PROGMEM = "function dgr(_url, _func_onReady ) {xhr = new XMLHttpRequest();xhr.open(\"GET\", _url, true, \"\", \"\");xhr.onreadystatechange = function() {if(xhr.readyState == 4 && xhr.status == 200) {_func_onReady(xhr.responseText);}};xhr.send(null);}";
const char JS_POPULATE_SELECTION[] PROGMEM = "function pGS(_rT) {var pics=JSON.parse(_rT); pics=sbk(pics, \'name\');selectList=document.getElementById(\"graphics\");selectList.innerHTML=\"\";for(var i = 0; i < pics.length; i++){if(pics[i].type === \"file\") {var node = document.createElement(\"option\");var picname = (/(.*)\\.[^.]+$/.exec(pics[i].name)[1]).split(\'/\').pop();node.value=pics[i].name;var textNode = document.createTextNode(picname);node.appendChild(textNode);document.getElementById(\"graphics\").appendChild(node);}}}";
const char JS_INIT_AFTER_LOAD[]    PROGMEM = "function stInit(){dgr( \"/edit?list=/pix\", pGS);} document.addEventListener(\"DOMContentLoaded\", stInit, false);";

