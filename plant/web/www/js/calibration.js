//-------------Node -----------------------
function getNodeList() {
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD : "AllLEDNodeList"
    },
    function(data) {
    				//dealPage(data);
        dealNodeList(data);
    });
}

	
	
function dealNodeList(data) {
    var html = "";
    
    $(data).find('node').each(function() {
		var id = $(this).children('id').text();
        if ($(this).children('isValid').text() == "0") {
            html += "<div class='item inActivity' onclick='showNodeInfo(" + id + ",1);'>";
        } else {
            html += "<div class='item' onclick='showNodeInfo(" + id + ");'>";
        }
		html += "<input onclick='preventEvent(event);' id='isNodeSelected"+ id +"' name='isNodeSelected' type='checkbox' value='"+ id+"' />&nbsp;";
        html += "<label class='nodes' for='isNodeSelected"+ id+"'>"+$(this).children('memo').text();
        html += "</label><label class='areaName'>" + $(this).children('areaname').text() + "</label></div>";

    }); 
    html += ""; 
    $("#NODELIST").html(html);
    
    $("#selectAll").prop("checked",false);

}


//get node caliberation from DB
function showNodeInfo(nodeId,channel) {
$("#nodeId").val(nodeId);
/*

    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "NodeInfo",
        NODEID: nodeId
    },
    function(data) {
        dealNodeInfo(data);
    });
*/
}

function dealNodeInfo(data) {
    $(data).find('node').each(function() {
        $("#nodeId").val($(this).children('id').text());
        $("#nodeMac").val($(this).children('mac').text());
        $("#memo").val($(this).children('memo').text());
	$("#nodeType").val($(this).children('Type').text());

       // $("#nodeArea").val($(this).children('domain').text());
	$("#maxSpeed").val($(this).children('speed').text());
	$("#maxDistance").val($(this).children('distance').text());

	if($("#nodeType").val()=="1"){
		$("#maxSpeed").attr("disabled",false);
		$("#maxDistance").attr("disabled",false);

	}else{
		$("#maxSpeed").attr("disabled",true);
		$("#maxDistance").attr("disabled",true);
	}
        return false;
    });

}
function saveCaliberation(objButton) {
	$(objButton).attr('disabled',"true");
    var nodeID = $("#nodeId").val();
   
}


function turnOffAllLed(objButton){
	$(objButton).attr('disabled',"true");
	//var nodeID = $("#nodeId").val();
	var nodeID= "";
	var count = 0;

	$("[name='isNodeSelected']").each(function(){
				if($(this).prop("checked")){
					nodeID += $(this).val()+",";
					count++;
				}
	});
	if(count==0){
	alert("please select one Node at least.");
			return false;
	}
	var loc = "NodeManage.action";
	$.get(loc, {
		METHOD: "TurnOffLed",
		NODEID: nodeID

	},
	function(data) {
		$(objButton).removeAttr("disabled");	
		dealReturnMessage(data, "Trun off LEDs");

	});

}
function sendCaliberation(objButton) {
	$(objButton).attr('disabled',"true");
    	//var nodeID = $("#nodeId").val();
	var nodeID= "";

	var channel=$("#channel").val();
	var pwm = $("#pwm").val();
	var current = $("#current").val();
	var ppf = $("#ppf").val();

	var leds="";
	var count = 0;

	$("[name='isNodeSelected']").each(function(){
				if($(this).prop("checked")){
					nodeID += $(this).val()+",";
					count++;
				}
	});
	if(count==0){
	alert("please select one Node at least.");
			return false;
	}
	count=0;
	$("[name='leds']").each(function(){
				if($(this).prop("checked")){
					leds += $(this).val()+",";
					count++;
				}
	});

   if(count==0){
		alert("please select one LED at least.");
		return false;
	}
console.log(nodeID);
if(nodeID=="-1"){
alert("please select one Node at least.");
		return false;
}
var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "setNodeCaliberate",
        NODEID: nodeID,
        CHANNEL: channel,
        PWM: pwm,
        LEDS: leds,
	CURRENT:current,
	PPF:ppf
	
    },
    function(data) {
    	$(objButton).removeAttr("disabled");	
    	dealReturnMessage(data, "Set node caliberate");

    });

}


