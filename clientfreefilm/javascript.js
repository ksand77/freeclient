//разбор элементов
function ExtractValue (value) {
      var match = [];
      var elements = document.getElementsByClassName('type');

    var ln = elements.length;
      for (var ii = 0; ii < ln; ii++) {
        if(elements[ii].innerText==value)return(elements[ii].nextSibling.lastChild.innerHTML);
        }
      return("!ok");
};


