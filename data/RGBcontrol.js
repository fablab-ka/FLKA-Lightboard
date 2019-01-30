if (typeof XMLHttpRequest === "undefined") {
  XMLHttpRequest = function () {
    try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); } catch (e) {}
    try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); } catch (e) {}
    try { return new ActiveXObject("Microsoft.XMLHTTP"); } catch (e) {}
    throw new Error("This browser does not support XMLHttpRequest.");
  };
}


var websocket = null;

function startWebsocket(){
    if ("http:" === location.protocol) {
      websocket = new WebSocket("ws://" + location.host +"/ws");
    } else  if ("https:" === location.protocol) {
      //websocket = new WebSocket("wss://" + location.host + "/ws");
    }
    if (websocket !== null) {
      websocket.onopen = function(){
        console.log('connected!');
      };
      websocket.onmessage = function(e){
        console.log(e.data);
      };
      websocket.onclose = function(){
        console.log('closed!');
        //reconnect now
        checkWebsocket();
      }  
    };
}

function checkWebsocket(){
    if(!websocket || websocket.readyState == 3) startWebsocket();
}

startWebsocket();

setInterval(checkWebsocket, 3000);



//var wsUri = "ws://192.168.170.148/ws"  //192.168.170.146  //192.168.1.250
//var websocket = new WebSocket(wsUri);
//websocket.onmessage=function (evt) {
//  console.log(evt.data);
//}

function webSocketRGB(r,g,b) {
	
  console.log('{"magic": "FLKA","cmd": "RGB", "value": ['+r+','+g+','+b+'], delay: 70}');
  if (websocket !== null) {
    websocket.send('{"magic": "FLKA","cmd": "RGB", "value": ['+r+','+g+','+b+'], delay: 70}');
  }	  

}

// Circular Object for picking a Hue-Value

function ColorPicker(element) {
    this.element = element;

    this.init = function(hue, saturation, lightness) {
        this.innerHTML="";
        this.hue= ((hue>=0) || (hue<=360)) ? hue : 0;
        this.saturation= ((saturation>=0) || (saturation<=100)) ? saturation : 100;
        this.lightness= ((lightness>=0) || (lightness<=100)) ? lightness : 50;
        var diameter = Math.min(this.element.offsetWidth, this.element.offsetHeight);
        var hueRing = document.createElement('canvas');
        var hueSelector = document.createElement('canvas');
        hueRing.height = diameter;
        hueRing.width = diameter;
        hueSelector.height = diameter;
        hueSelector.width = diameter;
        hueRing.style.cssText="position: absolute;top: 0;left: 0; bottom: 0; right: 0;z-index: 0;";
        hueSelector.style.cssText="position: absolute;top: 0;left: 0; bottom: 0; right: 0; z-index: 1;";
        this.hueRing = hueRing;
        this.hueSelector = hueSelector;
        element.appendChild(hueRing);  
        element.appendChild(hueSelector);
        var midX = diameter / 2;
        this.midX = midX;
        var midY = diameter / 2;
        this.midY = midY;
        var stripWidth = diameter / 10;
        this.stripWidth = stripWidth;
        var outerSpace = 12;
        this.outerSpace = outerSpace;
        var radius = (diameter / 2 );
        this.radius = radius;
        var innerRadius = this.radius-this.stripWidth-this.outerSpace;
        var outerRadius = this.radius-this.outerSpace;
        this.innerRadius = innerRadius;
        this.outerRadius = outerRadius;
        this.renderColorMap();
        this.hueSelector.addEventListener('click', this);
        this.hueSelector.addEventListener('mousemove', this);
    };
    this.setHue= function(Hue)  {
      this.hue=Hue;
    }
    this.setSaturation= function(Sat) {
      this.saturation=Sat;
    }
    this.setLightness= function(Light) {
      this.lightness=Light;
    }

    this.renderColorMap = function() {
        var hueRing = this.hueRing;
        var ctx = hueRing.getContext('2d');
        var toRad = (2 * Math.PI) / 360;
        
        ctx.clearRect(0, 0, hueRing.width, hueRing.height);
        for(var i = 0; i < 360; i +=1) {
            ctx.beginPath();
            ctx.lineWidth = this.stripWidth;
            ctx.strokeStyle = 'hsl(' +  i + ', 100%, 50%)';
            ctx.arc(this.radius, this.radius, this.radius-(this.stripWidth/2)-this.outerSpace, i*toRad, (i+1.5)*toRad);
            ctx.stroke();
        }
        ctx.beginPath();
        ctx.strokeStyle = "#000";
        ctx.lineWidth = 1;
        ctx.arc(this.radius, this.radius, this.innerRadius, 0, 2 * Math.PI, true);
        ctx.stroke();
        ctx.beginPath();
        ctx.arc(this.radius, this.radius, this.outerRadius, 0, 2 * Math.PI, true);
        ctx.stroke();
    };
  
    this.mouseMatch = function(x,y) {
      var hitRadius = Math.sqrt((x-this.radius) **2 + (y-this.radius) **2);
      if ((hitRadius <= this.outerRadius) && (hitRadius >=this.innerRadius)){
        let Hue = (((Math.atan2((y-this.radius), (x-this.radius))*(180/Math.PI))+360) | 0) % 360;
        this.hue = Hue;
        if (typeof this.callOnChange === "function") {
          this.callOnChange();
        }
        this.drawColor();
      }
    }

    this.handleEvent = function(evt) {
        //the name HandleEvent is a 'must' !
        if (((1 & evt.buttons) == 1) || (evt.type == "click")) {
          var rect = evt.target.getBoundingClientRect();
          this.mouseMatch((evt.clientX - rect.left), (evt.clientY - rect.top));
        }  
    }
  
    this.drawColor = function() {
        var Hue = ((this.hue | 0) +360) %360 ;
        var HueRad=Hue/360*2*Math.PI;
        var ctx = this.hueSelector.getContext('2d');
        ctx.clearRect(0, 0, this.hueSelector.width, this.hueSelector.height);
        rad = this.radius - (this.stripWidth/2)  - this.outerSpace;
        ctx.beginPath();
        ctx.strokeStyle = "#000";
        ctx.lineWidth = 3;
        ctx.arc(Math.cos(HueRad)*rad+this.radius, Math.sin(HueRad)*rad+this.radius, (this.stripWidth+this.outerSpace)/2, 0, 2*Math.PI, true);
        ctx.fillStyle = 'hsl('+ Hue +', 100%, 50%)'
        ctx.fill();
        ctx.stroke();
        ctx.beginPath();
        ctx.fillStyle = 'hsl('+ Hue +', '+this.saturation+'%, '+this.lightness+'%)'
        ctx.lineWidth = 2;
        ctx.arc(this.radius, this.radius, this.innerRadius-this.stripWidth, 0, 2 * Math.PI, true);
        ctx.fill();
        ctx.stroke();
        
    }
 
    this.addChangeCallback = function(callback) {
        this.callOnChange=callback;
    }
    
    this.init();

}

// Instance of that object
var HueWheel = new ColorPicker(document.querySelector('#color-wheel'));
// Show initial color (red)
HueWheel.drawColor();
//window.addEventListener("resize", function(){HueWheel = new ColorPicker(document.querySelector('#color-wheel'));HueWheel.drawColor();});


HueWheel.addChangeCallback(hueWheelCallback);
                           
                           
function hueWheelCallback() {
  sysl("hueVal", this.hue );
  sysl("hueSlider", this.hue );
  setHueButton();
  cssSetSaturationTrack(this.hue, this.lightness); 
  cssSetLightnessTrack(this.hue, this.saturation);
  updateRGB();
}                           


function resizeHueWheel(){
  let hue=HueWheel.hue;
  let saturation=HueWheel.saturation;
  let lightness=HueWheel.lightness;
  document.querySelector('#color-wheel').innerHTML="";
  HueWheel = new ColorPicker(document.querySelector('#color-wheel'));
  HueWheel.setHue(hue);
  HueWheel.setSaturation(saturation);
  HueWheel.setLightness(lightness);
  HueWheel.drawColor();
  HueWheel.addChangeCallback(hueWheelCallback);
  }
  
window.addEventListener("resize",resizeHueWheel);


// Convert RGB to HSL and return values as integers
function rgbToHsl(r, g, b) {
    r /= 255, g /= 255, b /= 255;
    var max = Math.max(r, g, b), min = Math.min(r, g, b);
    var h, s, l = (max + min) / 2;

    if (max == min) {
      h = s = 0; // achromatic
    } else {
      var d = max - min;
      s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

      switch (max) {
        case r: h = (g - b) / d + (g < b ? 6 : 0); break;
        case g: h = (b - r) / d + 2; break;
        case b: h = (r - g) / d + 4; break;
      }
      h /= 6;
    }
  return [ h*360 | 0, s*100 | 0, l*100 | 0 ];
  }

// Convert HSL to RGB and return values as integers
function hslToRgb(h, s, l) {
   h/=360; s/=100;l/=100;
     var r, g, b;

  if (s == 0) {
  r = g = b = l; // achromatic
  } else {
  function hue2rgb(p, q, t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1/6) return p + (q - p) * 6 * t;
    if (t < 1/2) return q;
    if (t < 2/3) return p + (q - p) * (2/3 - t) * 6;
    return p;
  }

  var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
  var p = 2 * l - q;

  r = hue2rgb(p, q, h + 1/3);
  g = hue2rgb(p, q, h);
  b = hue2rgb(p, q, h - 1/3);
  }

  return [ (r * 255)|0, (g * 255)|0, (b * 255)|0 ];
}

//Helper function 
function sysl(elem, val){ 
  document.getElementById(elem).value=val;
}

// Set CSS-Variable in Saturation Track Slider to change color
function cssSetSaturationTrack(hue, light) {
  let gradient="linear-gradient(90deg";
  for (i = 0; i <= 100; i +=5) {
    gradient += ', hsl('+hue+', '+i+'%, '+light+'%) '+i+'%';
  }
  gradient += ')';
  document.getElementById('satSlider').style.setProperty('--satGradient', gradient);
  setSaturationButton();
}
// Set CSS-Variable in Lightness Track Slider to change color
function cssSetLightnessTrack(hue, sat) {
  let gradient="linear-gradient(90deg";
  for (i = 0; i <= 100; i +=5) {
    gradient += ', hsl('+hue+', '+sat+'%, '+i+'%) '+i+'%';
  }
  gradient += ')';
  document.getElementById('lightSlider').style.setProperty('--lightGradient', gradient);
  setLightnessButton();
}
function setHueButton() {
  let buttonColor = 'hsl('+HueWheel.hue+', 100%, 50%)';
  document.getElementById('hueSlider').style.setProperty('--hueButton', buttonColor);
}
function setLightnessButton() {
  let buttonColor = 'hsl('+HueWheel.hue+', '+HueWheel.saturation+'%, '+HueWheel.lightness+'%)';
  document.getElementById('lightSlider').style.setProperty('--lightButton', buttonColor);
}

function setSaturationButton() {
  let buttonColor = 'hsl('+HueWheel.hue+', '+HueWheel.saturation+'%, '+HueWheel.lightness+'%)';
  document.getElementById('satSlider').style.setProperty('--satButton', buttonColor);
} 

//Update HSL-Values and Display, after RGB-values have changed
function updateHSL(){ 
  let redVal=document.getElementById("redVal").value;
  let greenVal=document.getElementById("greenVal").value;
  let blueVal=document.getElementById("blueVal").value;
  webSocketRGB(redVal,greenVal,blueVal);
  let [hueVal, satVal, lightVal] = rgbToHsl(redVal, greenVal, blueVal);
  sysl("hueVal", hueVal);
  sysl("satVal", satVal);
  sysl("lightVal", lightVal);
  sysl("hueSlider", hueVal);
  sysl("satSlider", satVal);
  sysl("lightSlider", lightVal);
  HueWheel.setHue(hueVal);
  HueWheel.setSaturation(satVal);
  HueWheel.setLightness(lightVal);
  setHueButton();
  HueWheel.drawColor();
  cssSetSaturationTrack(hueVal, lightVal); 
  cssSetLightnessTrack(hueVal, satVal);
}

//Update RGB-Button Colors
function setRGBButtons() {
  var ButtonNames = ["redSlider", "greenSlider", "blueSlider"];
  let red   = 'rgb('+document.getElementById('redSlider').value+', 0, 0';
  let green = 'rgb(0, '+document.getElementById('greenSlider').value+', 0';
  let blue  = 'rgb(0, 0, '+document.getElementById('blueSlider').value+'';
  document.getElementById('redSlider').style.setProperty('--redButton', red);
  document.getElementById('greenSlider').style.setProperty('--greenButton', green);
  document.getElementById('blueSlider').style.setProperty('--blueButton', blue);
};
   


//Update RGB-Values and Display, after HSL-values have changed
function updateRGB(){ 
  let hueVal=document.getElementById("hueVal").value;
  let satVal=document.getElementById("satVal").value;
  let lightVal=document.getElementById("lightVal").value;
  let [redVal, greenVal, blueVal] = hslToRgb(hueVal, satVal, lightVal);
  webSocketRGB(redVal,greenVal,blueVal);
  sysl("redVal", redVal| 0);
  sysl("greenVal", greenVal| 0)
  sysl("blueVal", blueVal| 0);
  sysl("redSlider", redVal| 0);
  sysl("greenSlider", greenVal| 0);
  sysl("blueSlider", blueVal| 0);
  setRGBButtons();
}

function checkValidity(elem) {
	if (!elem.validity.valid) {
		elem.value=0;
	}	
}	

// Initialization of the document
// Add onchange-Events to all Sliders
var slidernames = ["red", "green", "blue"];
slidernames.forEach( function (a){
             let docId = a + "Val";
             let refId = a + "Slider";
               document.getElementById(docId).addEventListener('input', function(){
									  checkValidity(document.getElementById(docId));
                                      sysl(refId, this.value);
                                      setRGBButtons();
                                      updateHSL();
                                    }, true );
               document.getElementById(refId).addEventListener('input', function(){
                                      sysl(docId, this.value);
                                      setRGBButtons();
                                      updateHSL();
                                    }, true );
});
var slidernames = ["hue", "sat", "light"];
slidernames.forEach( function (a){
             let docId = a + "Val";
             let refId = a + "Slider";
             if (a === "hue") {
               document.getElementById(docId).addEventListener('input', function(){
				                                                                   checkValidity(document.getElementById(docId));
				                                                                   sysl(refId, this.value);
                                                                                   HueWheel.setHue(this.value);
                                                                                   HueWheel.drawColor();
                                                                                   updateRGB();
                                                                                   cssSetSaturationTrack(HueWheel.hue, HueWheel.lightness);
                                                                                   cssSetLightnessTrack(HueWheel.hue, HueWheel.saturation);
                                                                                   setHueButton();
                                                                                  }, true );
               document.getElementById(refId).addEventListener('input', function(){
                                                                                   sysl(docId, this.value);
                                                                                   HueWheel.setHue(this.value);
                                                                                   HueWheel.drawColor();
                                                                                   updateRGB();
                                                                                   cssSetSaturationTrack(HueWheel.hue, HueWheel.lightness);
                                                                                   cssSetLightnessTrack(HueWheel.hue, HueWheel.saturation);
                                                                                   setHueButton();
                                                                                  }, true );
             };
             if (a === "sat") {
               document.getElementById(docId).addEventListener('input', function(){
				                          checkValidity(document.getElementById(docId));
                                          sysl(refId, this.value);
                                          HueWheel.setSaturation(this.value);
                                          HueWheel.drawColor();
                                          setLightnessButton();
                                          setSaturationButton();
                                          setHueButton();
                                          updateRGB();}, true );
               document.getElementById(refId).addEventListener('input', function(){
				                          sysl(docId, this.value);
                                          HueWheel.setSaturation(this.value);
                                          HueWheel.drawColor();
                                          setLightnessButton();
                                          setSaturationButton();
                                          setHueButton();
                                          updateRGB();
                                          }, true );
              
             };
             if (a === "light") {
               document.getElementById(docId).addEventListener('input', function(){
				                          checkValidity(document.getElementById(docId));
                                          sysl(refId, this.value);
                                          HueWheel.setLightness(this.value);
                                          HueWheel.drawColor();
                                          setLightnessButton();
                                          setSaturationButton();
                                          setHueButton();
                                          updateRGB();
                                           }, true );
               document.getElementById(refId).addEventListener('input', function(){sysl(docId, this.value);
                                          HueWheel.setLightness(this.value);
                                          HueWheel.drawColor();
                                          setLightnessButton();
                                          setSaturationButton();
                                          setHueButton();
                                          updateRGB();
                                           }, true );
              
             };
           });
                           

function retrieveMenu(_responseText){
  let menu=document.getElementById("menu");
  menu.innerHTML=_responseText;
}

function doGetRequest(_url, _func_onReady, requestIsBinary ) {
  requestIsBinary = requestIsBinary || false;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", _url, true, "admin", "admin");
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
  }
  xhr.send(null);
}  

document.getElementById("rainbowButton").addEventListener("click", function(){websocket.send('{"magic": "FLKA","cmd": "RAINBOW", delay: 30}');});
document.getElementById("cycleButton").addEventListener("click", function(){websocket.send('{"magic": "FLKA","cmd": "CYCLE", delay: 30}');});

doGetRequest( "/.menu.html", retrieveMenu);
                           
                           
                           
                           
                           
