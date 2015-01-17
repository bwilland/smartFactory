//------------Reproduction------------------------
/*
function getTimeSpan(){
		var loc = "ReproductionManage.action";
		$.get(loc,{
			METHOD : "TimeSpan"
		},function(data) {dealPhoto_TimeSpan(data);});
	}
function dealPhoto_TimeSpan(data) {
	var html="<ul>";
	$(data).find('TimeSpan').each( function ()
	{
		photoSpan = $(this).children('span').text();

		$("#TimeSpan").val(photoSpan);
		return false;
	});

}
*/
function getReproductionList(){
	var loc = "ReproductionManage.action";
	$.get(loc,{
		METHOD : "ReproductionList"
	},function(data) {dealReproductionList(data);});
}

function dealReproductionList(data) {
	var html="";
	$(data).find('reproduction').each( function ()
	{
		
		html += "<div class='item' onclick='showReproductionInfo("+$(this).children('id').text()+");'><label class='area-model'>";
		
		html += $(this).children('name').text() +"</label></div>";

	});


	$("#MODELLIST").html(html);


}


function showReproductionInfo(modelId){
	var loc = "ReproductionManage.action";
	$.get(loc,{
		METHOD : "ReproductionInfo",
		MODELID : modelId
	},function(data) {dealReproductionInfo(data);});
}

function dealReproductionInfo(data) {
	$(data).find('reproduction').each( function ()
	{
		$("#modelId").val($(this).children('id').text());
		$("#modelName").val($(this).children('name').text());
		$("#ppf1").val($(this).children('PPF1').text());
		$("#ppf2").val($(this).children('PPF2').text());
		$("#ppf3").val($(this).children('PPF3').text());
		$("#ppf4").val($(this).children('PPF4').text());
	
		$("#mode").val($(this).children('MODE').text());
		$("#direction").val($(this).children('DIRECTION').text());
		$("#speed" ).val($(this).children('SPEED').text());
		$("#distance").val($(this).children('DISTANCE').text());
	
		return false;
	});

}


function newReproduction(){
	$("#modelId").val("-1");
	$("#modelName").val("");
	$("#ppf1").val("0.00");
	$("#ppf2").val("0.00");
	$("#ppf3").val("0.00");
	$("#ppf4").val("0.00");
	
	
}

function saveReproduction(objButton){
	var modelID = $("#modelId").val();
	
	$(objButton).attr('disabled',"true");
	
	var modelName = $("#modelName").val();
	var ppf1 = $("#ppf1").val();
		var ppf2 = $("#ppf2").val();
		var ppf3 = $("#ppf3").val();
		var ppf4 = $("#ppf4").val();

	
	if(modelName=="" ){
		alert("Model name please.");
		$("#modelName").focus();
		$(objButton).removeAttr("disabled");	
		return false;
	}
	
	//var stime = $("#TimeSpan").val();
	
	var stime =1;
	
	if(photoSpan === stime){
		stime = 0;
			
		}
	

	saveReproductionDB(modelID,modelName,ppf1 ,ppf2 ,ppf3 ,ppf4,stime,objButton);

}
function saveReproductionDB(modelID,modelName,ppf1 ,ppf2 ,ppf3 ,ppf4,stime,objButton){
	var loc = "ReproductionManage.action";
	$.get(loc,{
		METHOD : "SaveReproduction",
		MODELID : modelID, 
		MODELNAME :  modelName, 
		PPF1  :  ppf1 , 
		PPF2  :  ppf2 , 
		PPF3  :  ppf3 , 
		PPF4  :  ppf4 , 
		MODE  :  0 , 
		DIRECTION  : 0 , 
		SPEED : 0,
		LENGTH : 0,
		TIMESPAN : stime
		},function(data) {	$(objButton).removeAttr("disabled");	$("#modelId").val(dealReturnMessage(data,"Save demo mode"));getReproductionList(false);});
}

function deleteReproduction(){
	var modelID = $("#modelId").val();
	
	if(modelID==-1){
		alert("Please select one model at least.");
		return false;
	}
	
	
	if(!confirm("You are deleting model, aren't you?")){
		return false;
	}
	var loc = "ReproductionManage.action";
	$.get(loc,{
		METHOD : "DeleteReproduction",
		MODELID : modelID
		},function(data) {if(dealReturnMessage(data,"Delete model")>0){newReproduction();getReproductionList(false);}});
}





// JavaScript Document
