  // primer script de efectos
  var ipValue;
  var WSconnection;
  var myTimer1 = 0;

   function WSConnect() {
      var text = document.getElementById('webSocketCell').value;
      ipValue = text;
      WSconnection = new WebSocket(ipValue, ['arduino']);
      WSconnection.onopen = function() {
          document.getElementById("webSocketCell").style.backgroundColor = "lightgreen";
          //myStartTimer1();
          myTimer1 = 0;
          console.log("WebSocket: status", WSconnection.readyState);
          $("#status").text("Connecting...");
      };

      WSconnection.onclose = function(evt) {
          document.getElementById("webSocketCell").style.backgroundColor = "red";
          console.log("WebSocket: closed", WSconnection.readyState);
          //WSlost();
          $("#status").text("Cerrando...");
      };



      WSconnection.onmessage = function(message) {
          document.getElementById("webSocketCell").style.backgroundColor = "lightgreen";
          var JsonObj = JSON.parse(message.data);

          console.log(JsonObj); // muestra el dato en la consola
          //alert(JsonObj); // para DEBUG 
          //
          // El mensaje es de un pattern ?
          $("#status").text("mensaje...");
          if (JsonObj.messageName == "js_pat") {


              // clear pattern list
              $("#menu-efectos").find("option").remove();

              // load pattern list
              for (var i = 0; i < JsonObj.patterns.length; i++) {
                  var pattern = JsonObj.patterns[i];
                  $("#menu-efectos").append("<option value='" + i + "'>" + pattern + "</option>");
              }

              // select the current pattern
              $("#menu-efectos").val(JsonObj.currentPattern.index);
              $("#_efecto").text("current pattern: " + JsonObj.currentPattern.name);

              //$("#status").text("Ready");
          };
          // El mensaje es de un HB ?
          if (JsonObj.messageName == "js_hbe") { // si recibo el HB resetea el timer
              //myStopTimer1();
              console.log('HB');
              //myStartTimer1();
              FlashHb();
          };
      };

      WSconnection.onerror = function(evt) {
          if (WSconnection.readyState == 1) {
              console.log('ws onerror: ' + evt.type);
              $("#status").text("error...");
          };
      };
  }


  // determina que fecto fue seleccionado 
  function efectos(efecto_selected) {
      var toSend = "a" + efecto_selected;
      //alert(toSend);
      WSconnection.send(toSend);
  }

  // fin script de efectos

  //FlashHb -- 
  //
  function FlashHb() {
      var div = document.getElementById("Hbeat");

      if (div.style.background === "white") {
          div.style.background = "black";
      } else {
          div.style.background = "white";
      };
  }


  // Timer para actualizar reloj
  var myVar = setInterval(myTimer, 1000);

  function myTimer() {
      var reloj = new Date();
      document.getElementById("_tiempo").innerHTML = reloj.toLocaleTimeString();
  }



  function myStartTimer1() {
      if (!window.myTimer1) { // evito dispararlo si ya lo hice
          myTimer1 = setInterval(WSlost, 5000);
      }
  }

  function myStopTimer1() {
      clearInterval(myTimer1);
  }

  function WSlost() {
      document.getElementById("webSocketCell").style.backgroundColor = "red";
      console.log("WSLost connection");
      //setTimeout(WSConnect, 5000);
      myTimer1 = 0;
  }


  // change on menu  jquery  document ready function -->
  $(function() {


      $("select").on({
          change: function() {
              var efecto_selected = $(this).val();
              //var efecto_selected = $("#menu-efectos").find(":selected").text();
              efectos(efecto_selected);
          }
      });
      // / wheel color picker
      //$('#color-hsv').wheelColorPicker({ format: 'hsv' });
      $('#color-input').on('sliderup', function() {
          $('#event-slider').val('Released');
      });
      $('#color-input').on('sliderdown', function() {
          $('#event-color').val($(this).wheelColorPicker('getValue', 'rgb'));
          $('#event-slider').val('pressed');
      });
      $('#color-input').on('focus', function() {
          $('#event-input').val('Focused');
      });
      $('#color-input').on('blur', function() {
          $('#event-input').val('Blurred');
      });
      $('#color-input').on('change', function() {
          $('#event-input').val('Changed');
      });
      $('#color-input').on('slidermove', function() {
          $('#event-color').val($(this).wheelColorPicker('getValue', 'rgb'));
          var hsvValue = $(this).wheelColorPicker('getValue', 'hsv%');
          var hsvValue2 = $('#color-input').val();
          var value3 = $.fn.wheelColorPicker.rgbToHsv(200, 100, 50);
          $('#event-color2').val(hsvValue);
          console.log('w' + " " + hsvValue);
          WSconnection.send("w" + hsvValue);

      });
  });
  // change on slider -->  al mover el slider ejecuta esta funcion
  function slider_react(newValue, newName) {
      var slider_value = newValue;
      var slider_name = newName.substr(6, 7); // toma la ultima letra del nombre del slider
      //alert(slider_value);
      //console.log(slider_value);
      //console.log(slider_name + slider_value);

      /* Act on the event */
      WSconnection.send(slider_name + slider_value);
      console.log(slider_name + " " + slider_value);
  }
