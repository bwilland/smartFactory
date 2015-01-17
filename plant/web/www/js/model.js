//------------Model------------------------

function getModelList(){
	var loc = "ModelManage.action";
	$.get(loc,{
		METHOD : "ModelList"
	},function(data) {dealModelList(data);});
}

function dealModelList(data) {
	var html="";

	$(data).find('model').each( function ()
	{
		var id = parseInt($(this).children('id').text());
		
		html += "<div class='item' onclick='showModelInfo("+$(this).children('id').text()+");'><label class='area-model'>";
		if(id<5){
			html += "<font color='red'>"+$(this).children('name').text() +"</font></label></div>";
		}else{
			html += $(this).children('name').text() +"</label></div>";
		}
	});

// console.log(html);
	$("#MODELLIST").html(html);


}


function showModelInfo(modelId){
	var loc = "ModelManage.action";
	$.get(loc,{
		METHOD : "ModelInfo",
		MODELID : modelId
	},function(data) {dealModelInfo(data);});
}

function dealModelInfo(data) {
	$(data).find('model').each( function ()
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


function newModel(){
	$("#modelId").val("-1");
	$("#modelName").val("");
	$("#ppf1").val("0.00");
	$("#ppf2").val("0.00");
	$("#ppf3").val("0.00");
	$("#ppf4").val("0.00");
	
	$("#mode").val("0");
	$("#direction").val("0");
	$("#speed").val("0.00");
	$("#distance").val("0.00");
	

	$("#modelName").focus();
}

function saveModel(objButton){
	var modelID = $("#modelId").val();
	if(modelID>0 && modelID<5){
			alert("This model is system model, do not change.");
			return false;
	}
	
	$(objButton).attr('disabled',"true");
	
		var ppf1 = $("#ppf1").val();
		var ppf2 = $("#ppf2").val();
		var ppf3 = $("#ppf3").val();
		var ppf4 = $("#ppf4").val();


		var mode =$("#mode").val();
		var direction =$("#direction").val();
		var speed =$("#speed" ).val();
		var distance =$("#distance").val();

	
	var modelName = $("#modelName").val();

	if(modelName=="" ){
		alert("Model name input please.");
		$("#modelName").focus();
		$(objButton).removeAttr("disabled");	
		return false;
	}
	
modelName = modelName.replace(/ /g,"^");


	saveModelDB(modelID,modelName,ppf1 ,ppf2 ,ppf3 ,ppf4 ,mode,direction,speed,distance,objButton);

}
function saveModelDB(modelID,modelName,ppf1 ,ppf2 ,ppf3 ,ppf4 ,mode,direction,speed,distance,objButton){
	var loc = "ModelManage.action";
	$.get(loc,{
		METHOD : "SaveModel",
		MODELID : modelID, 
		MODELNAME :  modelName, 
		PPF1  :  ppf1 , 
		PPF2  :  ppf2 , 
		PPF3  :  ppf3 , 
		PPF4  :  ppf4 , 
		MODE  :  mode , 
		DIRECTION  : direction , 
		SPEED : speed,
		LENGTH : distance
		},function(data) {	$(objButton).removeAttr("disabled");	$("#modelId").val(dealReturnMessage(data,"Save model"));getModelList(false);});
}
function deleteModel(){
	var modelID = $("#modelId").val();
	

	if(modelID==-1){
		alert("Please select one model.");
		return false;
	}
	if(modelID<3){
			alert("This model is system model, do not delete.");
			return false;
		}
	
	if(!confirm("You are deleting model,aren't you?")){
		return false;
	}
	var loc = "ModelManage.action";
	$.get(loc,{
		METHOD : "DeleteModel",
		MODELID : modelID
		},function(data) {if(dealReturnMessage(data,"Delete model")>0){newModel();getModelList(false);}});
}



// JavaScript Document
