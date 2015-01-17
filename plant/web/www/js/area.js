//-------------Area------------------------

function getAreaList(){
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AreaList"
	},function(data) {
		
			dealAreaList(data);
		
		});
}

function dealAreaList(data) {
	var html="";
		$(data).find('domain').each( function (){
		html += "<div class='item' onclick='showAreaInfo("+$(this).children('id').text()+");'><label class='area-model'>";
		
		html += $(this).children('id').text()+".&nbsp;&nbsp;"+$(this).children('name').text() +"</label>&nbsp;&nbsp;&nbsp;&nbsp;<label class='node-count'>"+ $(this).children('nodecount').text()+"&nbsp;LED panels</label></div>";
		});
		

	$("#AREALIST").html(html);


}

function getAreaSortList(){
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AreaList"
	},function(data) {
		
			dealAreaSortList(data);
		
		});
}

function dealAreaSortList(data) {
	var html="<ul id='sortable'>";
		$(data).find('domain').each( function (){
			html += "<li id="+$(this).children('id').text() +">"+$(this).children('name').text()+"</li>";
		});
		html +="</ul>";
		$("#AREASORTLIST").html(html);
	$("#sortable").sortable();

}

function showAreaInfo(areaId){
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AreaInfo",
		AREAID : areaId
	},function(data) {dealAreaInfo(data);});
}

function dealAreaInfo(data) {
	var isResult = 0;
	$(data).find('domain').each( function ()
	{
		$("#areaId").val($(this).children('id').text());
		$("#areaName").val($(this).children('name').text());
		$("#areaMemo").val($(this).children('memo').text());
 //$("#photonRed").val($(this).children('kr').text());
	//$("#photonBlue").val($(this).children('kb').text());

		$("#groupID").val($("#areaId").val());

		isResult = 1;
		return false;
	});
	if(isResult==0){
			alert("No area.");
		}
}

function newArea(){
	$("#areaId").val("-1");
	$("#areaName").val("");
	$("#areaMemo").val("");
	$("#groupID").val("");
	$("#areaName").focus();
	
 //getPhoto_KrKb();
}
/*
function getPhoto_KrKb(){
		var loc = "ModelManage.action";
		$.get(loc,{
			METHOD : "KBKR"
		},function(data) {dealPhoto_KrKb(data);});
	}
	
function dealPhoto_KrKb(data) {
	$(data).find('krkb').each( function ()
	{
 $("#photonRed").val($(this).children('kr').text());
	$("#photonBlue").val($(this).children('kb').text());
		return false;
	});
}
*/
function saveArea(objButton){
 $(objButton).attr('disabled',"true");      
	var areaID = $("#areaId").val();
	var	areaName = $("#areaName").val();
	var	areaMeno = $("#areaMemo").val();
	areaName = areaName.replace(/ /g,"^");
	areaMeno = areaMeno.replace(/ /g,"^");

	if(areaName=="" ){
		alert("Area name please.");
		$("#areaName").focus();
		 $(objButton).removeAttr("disabled");	
		return false;
	}
	
	
	
	saveAreaDB(areaID,areaName,areaMeno,objButton);
}
function saveAreaDB(areaID,areaName,areaMeno,objButton){
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "SaveArea",
		AREAID : areaID,
		AREANAME : areaName,
		AREAMEMO : areaMeno
		},function(data) {$(objButton).removeAttr("disabled");	$("#areaId").val(dealReturnMessage(data,"Save Area"));getAreaList(true);});
}
function deleteArea(){
	var areaID = $("#areaId").val();
	if(areaID==-1){
		alert("please select an area.");
		return false;
	}
	if(!confirm("You are deleting area, aren't you?")){
		return false;
	}
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "DeleteArea",
		AREAID : areaID
		},function(data) {if(dealReturnMessage(data,"Delete area")){newArea();getAreaList(true);}});
}


function getAreaNodes(areaID,type){
	//alert(type);
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AreaNodeAdd",
		AREAID : areaID,
		TYPE: type
		},function(data) {dealAreaNode(data);});
}

function dealAreaNode(data){
	var html ="";
	var isChecked = "";
	html="<table class='tableStyle' cellspacing='0' cellpadding='0'>";
 	$(data).find('node').each( function ()  
	{ 
	var id = $(this).children('id').text();
		isChecked = $(this).children('domain').text()=="0"?"":"checked";
		html+="<tr class='trStyle' onclick='selectArea("+id+");'><td width='120'>"+$(this).children('mac').text()+"</td><td width='180'>";
		html+=$(this).children('memo').text()+"</td><td width='80'>";
		html+="<input id='isAreaSelected"+ id +"' name='isSelected' type='checkbox' onclick='preventEvent(event);' value='"+ id+"' "+isChecked+"/> </td><tr>";
	   });

 	html+="</table>";
//alert(html);
	
	$("#NODELIST").html(html);
}

function addAreaNode(type){
	var areaID = $("#areaId").val();
	if(areaID==-1){
		alert("Please save this area at first.");
		return false;
	}
	getAreaNodes(areaID,type);
	nodeTypeSelected = type;
	$( "#AREANODELIST" ).dialog("open");
}

function areaSort(){
	getAreaSortList();

	$( "#AREASORT" ).dialog("open");
	
}

function areaQuery(){
	$( "#AREAQUERY" ).dialog("open");
	
}

function initComp(){
$( "#AREANODELIST" ).dialog({
			autoOpen: false,
			height: 500,
			width: 400,
			modal: true,
			resizable: false,
			buttons: {
				"Select all": function(){
						selectAllNodes(true);
					},
						"Select none": function(){
						selectAllNodes(false);
					},
				"OK": function() {
					confirmed();
						},
					
				"Cancel" : function(){
					$( this ).dialog( "close" );
						}
					}
	});
$( "#AREASORT" ).dialog({
			autoOpen: false,
			height: 500,
			width: 300,
			modal: true,
			resizable: false,
			buttons: {
				"OK": function() {
					saveSort();
						},
					
				"Cancel" : function(){
					$( this ).dialog( "close" );
						}
					}
	});
$( "#AREAQUERY" ).dialog({
			autoOpen: false,
			height: 250,
			width: 300,
			modal: true,
			resizable: false,
			buttons: {
				"Search": function() {
					doAreaQuery();
						},
					
				"Cancel" : function(){
					$( this ).dialog( "close" );
						}
					}
	});

}

function doAreaQuery(){
	var mac = $("#areaSort_mac").val();
	if(mac==""){
		alert("MAC please.");
		return false;
	}
	$( "#AREAQUERY" ).dialog("close");
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AreaQuery",
		MAC : mac
		},function(data) {dealAreaInfo(data);});
}

function confirmed(){
	var nodes = "";
	$("[name='isSelected']").each(function(){
				if($(this).prop("checked")){
					nodes += $(this).val()+";";
				}
		});
	
	if(nodes==""){
		alert("Please select one node at least.");
		return false;
	}
		
	addAreaNode2DB(nodes);
}

function addAreaNode2DB(nodes){
//alert(nodeTypeSelected);
	var areaID = $("#areaId").val();
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AddAreaNode",
		AREAID : areaID,
		NODE : nodes,
		TYPE:nodeTypeSelected
		},function(data) {if(dealReturnMessage(data,"Add nodes")){$( "#AREANODELIST" ).dialog("close");}});
}

function saveSort(){
	var areaIDs ="";
	areaIDs += $( "#sortable" ).sortable( "toArray" );
	//alert(areaIDs);
	
	var loc = "AreaManage.action";
	$.get(loc,{
		METHOD : "AreaSort",
		AREAIDS : areaIDs
		},function(data) {if(dealReturnMessage(data,"Reorder")){$( "#AREASORT" ).dialog("close");getAreaList();}});
	
}

function selectAllNodes(isSelected){
	$("[name='isSelected']").each(function(){
			$(this).prop("checked",isSelected);
		});
	}
