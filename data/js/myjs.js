  // primer script de efectos
  var ipValue;
  var connection;


  function WSConnect(){
var text = document.getElementById('webSocketCell').value;
ipValue = text; 
connection = new WebSocket(ipValue, ['arduino']);   
connection.onopen = function() {
  document.getElementById("webSocketCell").style.backgroundColor = "lightgreen"; 
};
connection.onclose = function() {
  document.getElementById("webSocketCell").style.backgroundColor = "red"; 
};
connection.onmessage = function(evt) {
    console.log((evt.data));
    bigString2Vars(evt.data);
    updateHTML();
};

console.log(text)
console.log("IP value changed to:"+ipValue);
updateHTML();

} 


  // determina que fecto fue seleccionado 
  function efectos(efecto_selected) {
      var toSend = "a" + efecto_selected;
      //alert(toSend);
      connection.send(toSend);
  }

  // fin script de efectos

  // change on menu -->
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
          var hsvValue=$(this).wheelColorPicker('getValue', 'hsv%');
          var hsvValue2=$('#color-input').val();
          var value3=$.fn.wheelColorPicker.rgbToHsv(200 , 100, 50);
          $('#event-color2').val(hsvValue);
         console.log('w'+ " " +hsvValue);
         connection.send("w"+ hsvValue);
         
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
      connection.send(slider_name + slider_value);
      console.log(slider_name + " "+ slider_value);
  };
