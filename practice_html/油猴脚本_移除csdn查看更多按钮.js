(function() {
    'use strict';
    var paras =document.getElementById("article_content");
    var hideButton=document.getElementsByClassName("hide-article-box text-center");
    var boxBottom=document.getElementsByClassName("pulllog-box");
    hideButton[0].parentNode.removeChild( hideButton[0]);
    paras.style="height: *px; ";
    boxBottom[0].parentNode.removeChild( boxBottom[0]);
})();