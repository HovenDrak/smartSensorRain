function setContentElement(id, msg){
    document.getElementById(id).textContent = msg;
}

function setShadow(id, classShadow){
    document.getElementById(id).classList.remove("shadow-status-rain-detected", "shadow-status-window-open","shadow-status-ok");
    document.getElementById(id).classList.add(classShadow); 
}

function refreshStatusLogger(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {

            setContentElement("txtStatusRain", this.response["txtRain"]);
            setContentElement("imgStatusRain", this.response["imgRain"]);
            setContentElement("txtStatusWindow", this.response["txtWindow"]);
            setContentElement("imgStatusWindow", this.response["imgWindow"]);

            setShadow("divRain", this.response["shadowRain"]);
            setShadow("divWindow", this.response["shadowWindow"]);
        }
    };
    xhttp.open("GET", "/info_logger", true);
    xhttp.responseType = 'json';
    xhttp.send();
}

function refreshInfoLogger(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("txtLogger").innerHTML = this.response["txtLogger"];
        }
    };
    xhttp.open("GET", "/get_logger", true);
    xhttp.responseType = 'json';
    xhttp.send();
}

function disableLoader(){
    document.getElementById("loader").style.display = "none";
    document.getElementById("id-flex-debug").style.display = "flex";
}

setTimeout(refreshStatusLogger, 1100);
setTimeout(refreshInfoLogger, 1150);
setTimeout(disableLoader, 1200);

setInterval(refreshStatusLogger, 1866);
setInterval(refreshInfoLogger, 3562);


