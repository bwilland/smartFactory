$.ajaxSetup({
	cache : false
});


Date.prototype.format = function(format) {
	var o = {
		"M+" : this.getMonth()+1, //month
		"d+" : this.getDate(),//day
		"h+" : this.getHours(),//hour
		"m+" : this.getMinutes(),//minute
		"s+" : this.getSeconds(),//second
		"q+" : Math.floor((this.getMonth()+3)/3),//quarter
		"S" : this.getMilliseconds()//millisecond
	}

	if(/(y+)/.test(format)) {
		format = format.replace(RegExp.$1, (this.getFullYear()+"").substr(4 - RegExp.$1.length));
	}

	for(var k in o) {
		if(new RegExp("("+ k +")").test(format)) {
			format = format.replace(RegExp.$1, RegExp.$1.length==1 ? o[k] : ("00"+ o[k]).substr((""+ o[k]).length));
		}
	}
	return format;
}

function initSubMenu(){

	$("#mainMenu").hover(function() {
			var itemwidth = $(this).width(); /* Getting the LI width */
			$(this).prepend("<div class='hover'></div>"); /* Inserting a blank div into within li above the <a> tag*/
			$(this).find("div").fadeIn('10000').css({ 'width' : itemwidth}); /* Using the itemwidth for the div to display properly*/
			$(this).find("ul").fadeIn('1000').slideDown('10000').css("display", "block");
		
	} , function() {
		$(this).find("div").slideUp('1000').fadeOut('1000');/* sliding up and fading out the hover div */
		$(this).find("div").remove();/* removing the <div> code from html at every mouseout event*/
		$(this).find("ul").fadeOut('1000'); /* fading out the sub menu */
			});
			$("#helpMenu").hover(function() {
			var itemwidth = $(this).width(); /* Getting the LI width */
			$(this).prepend("<div class='hover'></div>"); /* Inserting a blank div into within li above the <a> tag*/
			$(this).find("div").fadeIn('10000').css({ 'width' : itemwidth}); /* Using the itemwidth for the div to display properly*/
			$(this).find("ul").fadeIn('1000').slideDown('10000').css("display", "block");
		
	} , function() {
		$(this).find("div").slideUp('1000').fadeOut('1000');/* sliding up and fading out the hover div */
		$(this).find("div").remove();/* removing the <div> code from html at every mouseout event*/
		$(this).find("ul").fadeOut('1000'); /* fading out the sub menu */
		
	});


}
function dealReturnMessage_eM(data,msg,errorMsg) {
//	 alert(data);
	var html;
	var returnValue = "-1";
	if(msg == null){
		msg="Setting";
	}
 	$(data).find('return').each( function ()  
	{ 	
		if(parseInt($(this).children('code').text()) > 0){
			html = msg + " succeeded.";
			returnValue = $(this).children('code').text();
		}else{
			html = msg +" failed."+errorMsg;
			
		}
	 });

	alert(html);	

	return returnValue;
}
function dealReturnMessage(data,msg) {
	 //alert(data);
	var html;
	var returnValue = "-1";
	if(msg == null){
		msg="Setting";
	}
 	$(data).find('return').each( function ()  
	{ 	
		if(parseInt($(this).children('code').text()) > 0){
			html = msg + " succeeded.";
			returnValue = $(this).children('code').text();
		}else{
			html = msg +" failed.";
			
		}
	 });

	alert(html);	

	return returnValue;
}
function dealReturnMessageOK(data,msg) {
	if(msg == null){
		msg="Setting";
	}
 	$(data).find('return').each( function ()  
	{ 	
		if($(this).children('code').text()== "0"){
			alert( msg + " failed.");
			
		}
	 });

}

function logout(){
	var loc = "HomeManage.action";
	$.get(loc,{
		METHOD : "LogOut"
	},function(data) {dealLogOut(data);});
}

function dealLogOut(data){
	$(data).find('return').each( function ()  
	{ 	
				if(parseInt($(this).children('code').text()) > 0){
					location.href="login.html";
				}else{
					alert("Quit failed ,please try again.");
				}
	 });
}

function aboutMe(){
	var model = [
	'<div class="modal" tabindex="-1" role="dialog" aria-labelledby="About" aria-hidden="true"><div class="modal-dialog modal-sm"><div class="modal-content">',
    '<div class="modal-header"><button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button><h4 class="modal-title">Plant lighting control system</h4></div>',
    '<div class="modal-body"><dl><dt>Version:</dt><dd>2.0.1.216</dd><dt>CopyRight:</dt><dd>Biolumic Inc. <a href="http://www.biolumic.com" target="_blank">http://www.biolumic.com</a></dd></dl></div>',
    '<div class="modal-footer"><button type="button" class="btn btn-primary" data-dismiss="modal">Close</button></div>',
    '</div></div></div>'
  ];
    
  $(model.join('')).modal('show');
}

function closeMe(){
}


function preventEvent(event){
		event.stopPropagation();
}

function selectArea(id){
//	alert(event);
		var isChecked = $("#isAreaSelected"+id).prop("checked");
			$("#isAreaSelected"+id).prop("checked",!isChecked);
}

function selectModel(id){

		$("#isModelSelected"+id).prop("checked",true);
}

function modiServerName(){
	var name=$("#lblServerName").html();
	$("#serverName").val(name);
	$("#lblServerName").hide();
	$("#serverName").show();
	$("#serverName").focus();
}

function inputServerName(evnt){
	if(evnt.keyCode===13){
		saveServerName();
	}	
}

function saveServerName(){
	var name=$("#serverName").val();
	$("#lblServerName").html(name);
	$("#lblServerName").show();
	$("#serverName").hide();
	var loc = "SystemManage.action";
	$.get(loc,{
		METHOD : "SaveServerName",
		SERVERNAME:name
	},function(data) {});
	
}

function getServerName(){

	var loc = "SystemManage.action";
	$.get(loc,{
		METHOD : "GetServerName"
	},function(data) {showServerName(data);});
	
}

function showServerName(data){
	$(data).find('server').each( function (){
		 $("#lblServerName").val($(this).children('serverName').text());
		 return false;
		});
}


function disableAllBtn(){
	$(".btn").each(function(){
		$(this).attr('disabled',"true");
	});

}

function enableAllBtn(){
	$(".btn").each(function(){
		$(this).removeAttr('disabled');
	});
	
}
