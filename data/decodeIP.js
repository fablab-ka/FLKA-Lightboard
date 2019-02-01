if (typeof XMLHttpRequest === "undefined") {
    XMLHttpRequest = function () {
        try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); } catch (e) {}
        try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); } catch (e) {}
        try { return new ActiveXObject("Microsoft.XMLHTTP"); } catch (e) {}
        throw new Error("This browser does not support XMLHttpRequest.");
    };
}
  

function codeLine(octElement)  {
    this.element = octElement;
  
    this.init = function() {
        this.IPaddress="";
        this.status="error";
        this.adrType="single";
        this.addCodeLine(1);
        this.element.addEventListener("change", this);
    };

    this.getIp = function() {
        return(this.IPaddress);
    };

    this.getStatus = function() {
        return(this.status);
    };

    this.addCodeLine= function (LineId) {
        let elemId="codeLine-"+LineId;
        let toAdd = document.querySelector("#"+elemId);
        if (toAdd == null)  {
            let codeDiv = document.createElement("div");
            codeDiv.className="codeLine";
            codeDiv.id=elemId;
            var content = "";
            for(let i=0; i<7; i++) {
                content = content + "<div class=\"styled-select\" ><select id=\""+elemId+"-"+i+"\" class=\"octPos\" onchange=\"\" required >";
                content = content + "<option class=\"o\" value=\"0\" selected> &nbsp; </option>";
                for(let j=1; j<8; j++) {
                    content=content+"<option class=\"o\" value=\""+j+"\"> &nbsp; </option>";
                }
                content = content + "</select></div>";
            }
            content = content + "</div>";
            codeDiv.innerHTML=content;
            this.element.appendChild(codeDiv);
        }    
    };

    this.removeCodeLine= function (LineId) {
        let elemId="codeLine"+LineId;
        let toRemove = document.querySelector("#"+elemId);
        if (toRemove !== null) {
            this.element.removeChild(toRemove);
        }  
    };
    this.addChangeCallback = function(callback) {
        this.callOnChange=callback;
    };


    this.checkCode = function() {
        let lines = ["",""];
        let status = "ok";
        let dir = "unknown";
        let adrType = "single"; 
        let IpAsInt = 0;
        let IpAsStr = "";
        if (typeof this.callOnChange === "function") {
            this.callOnChange();
        }
        for (let LineId of [1, 2]){
            let elemId="codeLine-"+LineId;
            let currLine = document.querySelector("#"+elemId);
            if (currLine !== null) {
                let digits = currLine.querySelectorAll(".octPos");
                for (let i = 0; i< digits.length; i++){
                    lines[LineId-1] += digits[i].value;
                }    
            } else {
                lines[LineId-1]="0000000" ; 
            }
        }
        //doChecks
        if (lines[0].charAt(0)=="6") { dir ="reverse"; }
        if (lines[0].charAt(6)=="6") { dir ="forward"; }
        if (dir == "reverse") {
            if ( "2345".indexOf(lines[0].charAt(6)) > -1 ) {
                adrType = "double";
            }
        } else {
            if ( "2345".indexOf(lines[0].charAt(0)) > -1 ) {
                adrType = "double";
            }
        }
        if (adrType == "double") {
            this.addCodeLine(2);
            for( let j of [0,1]) {
                if (dir == "reverse"){
                    lines[j]=lines[j].split("").reverse().join("");
                }  
                if (lines[j].charAt(6) !== "6") {status="error";}
                lines[j]=lines[j].substr(0,6);
            }
            if( (6 & ( parseInt(lines[0].charAt(0)) | parseInt(lines[1].charAt(0))) ) !== 6  ){ 
                status ="error";
            } else {
                let firstWord, secondWord = 0;
                if ((2 & parseInt(lines[0].charAt(0))) == 2) {
                    firstWord = parseInt(lines[0], 8)-65536;
                    secondWord = parseInt(lines[1], 8)-131072;
                } else {
                    firstWord = parseInt(lines[1], 8)-65536;
                    secondWord = parseInt(lines[0], 8)-131072;
                }  
                if ((firstWord >= 65535) || (secondWord>=65535)  || (secondWord < 256 ) ) {
                    status = "error"  ;
                } else {
                    IpAsInt=(secondWord*65536) + (firstWord) ;
                }
            }
        } else {
            if (dir == "reverse"){ lines[0]=lines[0].split("").reverse().join(""); }
            if (lines[0].charAt(6) !== "6") {status="error";}
            if (lines[0].charAt(0) > "1") {
                status="error";
            } else {
                lines[0]=lines[0].substr(0,6); 
                let twoBytes = parseInt(lines[0],8);
                if (twoBytes >= 65535) { 
                    status="error"; 
                } else {
                    IpAsInt=192*256*256*256 + 168*256*256 + parseInt(lines[0],8) ;
                }  
            }
        }
        if ( status !== "error") {
            IpAsStr = (IpAsInt >>> 24) + "." +
                  ((IpAsInt >>> 16) & 0xFF) + "." +
                  ((IpAsInt >>>  8) & 0xFF) + "." +
                  (IpAsInt & 0xFF);
            this.IPaddress = IpAsStr;
        } else {
            this.IPaddress =  "none";
        }
        this.status = status;              

        // eslint-disable-next-line no-console
        console.log(IpAsStr, status);
    };


    this.handleEvent = function(evt) {
        if (evt.type == "change") {
            evt.originalTarget.parentNode.className="styled-select o"+evt.originalTarget.value;
            this.checkCode();
        } 
    };  

    this.init();  
}  

// eslint-disable-next-line no-unused-vars
var codeLines  = null;
codeLines = new codeLine(document.querySelector("#codeContainer"));

function retrieveMenu(_responseText){
    let menu=document.getElementById("menu");
    menu.innerHTML=_responseText;
}

function resetColorCode() {
    let elem = document.querySelector("#codeContainer");
    elem.innerHTML="";  
    elem = document.querySelector("#output");
    elem.innerHTML="";  
    codeLines = new codeLine(document.querySelector("#codeContainer"));
    codeLines.addChangeCallback(cleanIPinfo);
}
document.querySelector("#resetCode").addEventListener("click", resetColorCode);

function decodeIP() {
    let elem =  document.querySelector("#output");
    elem.innerHTML="";  
    if (codeLines.getStatus() === "ok") {
        let ip=codeLines.getIp();  
        let response ="Die neue IP-Adresse des Lightboard ist:&nbsp; &nbsp;<a href=\"#\" onclick='window.open(\"http://"+ip+"\", \"_blank\")'>"+ip+"</a>";
        console.log(response);
        //let response=ip;
        elem.innerHTML=response;
    } else {
        elem.innerHTML="FEHLER: Keine gültige IP-Adresse!";
    }
}
document.querySelector("#decodeIP").addEventListener("click", decodeIP);

function cleanIPinfo() {
    let elem =  document.querySelector("#output");
    elem.innerHTML="";
}
codeLines.addChangeCallback(cleanIPinfo);


function doGetRequest(_url, _func_onReady, requestIsBinary ) {
    requestIsBinary = requestIsBinary || false;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", _url, true);
    if (requestIsBinary) { xhr.responseType = "arraybuffer"; }
    xhr.onreadystatechange = function() {
        if(xhr.readyState == 4 && xhr.status == 200) {
            if (requestIsBinary) {
                var ByteArray = new Uint8Array(xhr.response);
                _func_onReady(ByteArray);
            } else {
                _func_onReady(xhr.responseText);
            }  
        }
    };
    xhr.send(null);
}  

doGetRequest( "/.menu.html", retrieveMenu);
  

