//-------------Node -----------------------
function getNodeList(pageNo) {
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD : "AllNodeList",
        PAGENO : pageNo
    },
    function(data) {
    				//dealPage(data);
        dealNodeList(data);
    });
}

function getNodeListPage(pageNo) {

    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD : "AllNodeList",
        PAGENO : pageNo
    },
    function(data) {
        dealNodeList(data);
    });
}	
	
function dealNodeList(data) {
    var ledhtml = "<h5>LED nodes:</h5>";
    var motorhtml = "<h5>Motor nodes:</h5>";
    
    $(data).find('node').each(function() {
        var html;
		var id = $(this).children('id').text();
        if ($(this).children('isValid').text() == "0") {
            html = "<div class='item inActivity' >";
        } else {
            html = "<div class='item' onclick='showNodeInfo(" + id + ");'>";
        }
		html += "<input onclick='showNodeInfo(" + id + ");' id='isNodeSelected"+ id +"' name='isNodeSelected' type='checkbox' value='"+ id+"' />";
        html += "<label class='nodes' onclick='showNodeInfo(" + id + ");'>&nbsp;"+$(this).children('memo').text();
        html += "</label><label class='areaName'>" + $(this).children('areaname').text() + "</label></div>";
        if ($(this).children('type').text() === '0') {
            ledhtml += html;
        } else {
            motorhtml += html;
        }
    }); 
    $("#NODELIST").html(ledhtml+motorhtml);
    
    $("#selectAll").prop("checked",false);

}



function initPagination() {
    $('#JQpagination').jqPagination({
        page_string:'Page {current_page} of {max_page} ',
        paged: function(page) {
            currentPage = page;
            	getNodeListPage(page);
        }
    });
}

/*
function getAreaList() {
    var loc = "AreaManage.action";
    $.get(loc, {
        METHOD: "AreaList"
    },
    function(data) {

        dealSelector(data);

    });
}
function dealSelector(data) {
    var selObj = $("#nodeArea");
    $(data).find('domain').each(function() {
        selObj.append("<option value='" + $(this).children('id').text() + "'>" + $(this).children('name').text() + "</option>");

    });
}
*/

function showNodeInfo(nodeId) {
	//$("#isNodeSelected"+nodeId).prop("checked",true);	
	
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "NodeInfo",
        NODEID: nodeId
    },
    function(data) {
        dealNodeInfo(data);
    });
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
	$("#pulses").val($(this).children('pulses').text());

	if($("#nodeType").val()=="1"){
		$("#maxSpeed").attr("disabled",false);
		$("#maxDistance").attr("disabled",false);
		$("#pulses").attr("disabled",false);
		
		$("#channel1Current").attr("disabled",true);
		$("#channel2Current").attr("disabled",true);
		$("#channel3Current").attr("disabled",true);
		$("#channel4Current").attr("disabled",true);

		$("#channel1Current").val("0.0");
		$("#channel2Current").val("0.0");
		$("#channel3Current").val("0.0");
		$("#channel4Current").val("0.0");

	}else{
		$("#maxSpeed").attr("disabled",true);
		$("#maxDistance").attr("disabled",true);
		$("#pulses").attr("disabled",true);
		$("#channel1Current").attr("disabled",false);
		$("#channel2Current").attr("disabled",false);
		$("#channel3Current").attr("disabled",false);
		$("#channel4Current").attr("disabled",false);

		$("#channel1Current").val($(this).children('curr1').text());
		$("#channel2Current").val($(this).children('curr2').text());
		$("#channel3Current").val($(this).children('curr3').text());
		$("#channel4Current").val($(this).children('curr4').text());
	}
        return false;
    });

}

function changeType(obj) {
	if($(obj).val()=="1"){
		$("#maxSpeed").attr("disabled",false);
		$("#maxDistance").attr("disabled",false);
		$("#pulses").attr("disabled",false);
		$("#channel1Current").attr("disabled",true);
		$("#channel2Current").attr("disabled",true);
		$("#channel3Current").attr("disabled",true);
		$("#channel4Current").attr("disabled",true);

	}else{
		$("#maxSpeed").attr("disabled",true);
		$("#maxDistance").attr("disabled",true);
		$("#pulses").attr("disabled",true);
	
		$("#channel1Current").attr("disabled",false);
		$("#channel2Current").attr("disabled",false);
		$("#channel3Current").attr("disabled",false);
		$("#channel4Current").attr("disabled",false);
	}
}
function autoSearch() {
	$("#AutoSearchMode").dialog('open');
}
function doAautoSearch(){
	$("#BTN_autoSearch").attr('disabled',"true");
	$("#AutoSearchMode").dialog('close');
	
	var isReset = 0;
	if($("#isResetCOO").prop("checked")){
		isReset = 1;
	}
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "AutoSearch",
	ISRESET:isReset
    },
    function(data) {
	$("#AutoSearchDialog").dialog( 'close' );
    	  $("#BTN_autoSearch").removeAttr("disabled");	
        if (dealReturnMessage(data, "Auto search") > 0) {
            getNodeList(1);
        }
      
    });
    $(  "#AutoSearchDialog" ).dialog( 'open' );
}
function initComp(){
	$("#AutoSearchDialog").dialog({
        autoOpen: false,
        height: 180,
        width: 200,
        modal: true,
        resizable: false
    });
 	$("#AutoSearchMode").dialog({
        autoOpen: false,
        height: 230,
        width: 350,
        modal: true,
        resizable: false,
		buttons: {
            "OK": function() {
                doAautoSearch();
            },

            "Cancel": function() {
                $(this).dialog("close");
            }
        }
    }); 
    
}

function newNode() {
    $("#nodeId").val("-1");
    $("#nodeMac").val("");
    $("#memo").val("");
   // $("#nodeArea").val(0);
    $("#nodeType").val(0);
	$("#maxSpeed").val("0");
	$("#maxDistance").val("0");
$("#pulses").val("0");
    $("#nodeMac").focus();
	$("#maxSpeed").attr("disabled",true);
	$("#maxDistance").attr("disabled",true);
$("#pulses").attr("disabled",true);


		$("#channel1Current").attr("disabled",false);
		$("#channel2Current").attr("disabled",false);
		$("#channel3Current").attr("disabled",false);
		$("#channel4Current").attr("disabled",false);

		$("#channel1Current").val("100.0");
		$("#channel2Current").val("100.0");
		$("#channel3Current").val("100.0");
		$("#channel4Current").val("100.0");
}

function saveNode(objButton) {
	$(objButton).attr('disabled',"true");
    var nodeID = $("#nodeId").val();
    var memo = $("#memo").val();
    //var nodeArea = $("#nodeArea").val();
	var nodeArea = 0;
    var nodeMac = $("#nodeMac").val();
    var nodeType = $("#nodeType").val();
    var maxSpeed = $("#maxSpeed").val();
    var maxDistance = $("#maxDistance").val();
    var pulses = $("#pulses").val();

    if (memo == "") {
        alert("Memo please .");
        $("#memo").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }
    if (nodeMac.length != 8) {
        alert("MAC's length should be 8 bytes.");
        $("#nodeMac").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }

    var reg = /^[0-9a-fA-F]+$/;
    if (!reg.test(nodeMac)) {
        alert("MAC is incorrect,please input again.");
        $("#nodeMac").focus();
        $(objButton).removeAttr("disabled");	
        return false;
    }
	memo = memo.replace(/ /g,"^");

	var current1 = $("#channel1Current").val();
	var current2 = $("#channel2Current").val();
	var current3 = $("#channel3Current").val();
	var current4 = $("#channel4Current").val();
    saveNodeDB(nodeID, memo, nodeArea, nodeMac,nodeType,maxSpeed,maxDistance,pulses,current1,current2,current3,current4,objButton);
    
}
function saveNodeDB(nodeID, memo, nodeArea, nodeMac,nodeType,maxSpeed,maxDistance,pulses,current1,current2,current3,current4,objButton) {
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "SaveNode",
        NODEID: nodeID,
        MEMO: memo,
        NODEAREA: nodeArea,
        NODEMAC: nodeMac,
	NODETYPE:nodeType,
	SPEED:maxSpeed,
	DISTANCE:maxDistance,
	PULSES:pulses,
	CURRENT1:current1,
	CURRENT2:current2,
	CURRENT3:current3,
	CURRENT4:current4
    },
    function(data) {
    	$(objButton).removeAttr("disabled");	
    	var returnCode = dealReturnMessage_eM(data, "Save node ","The nodes has been saved already or the device with this MAC is not exist.");
    	if(returnCode>0){
        $("#nodeId").val(returnCode);
alert(returnCode);
        getNodeList(currentPage);
     }
        

    });
}
function deleteNode() {
   // var nodeID = $("#nodeId").val();
   var nodeIDs = "";
   var count = 0;
	$("[name='isNodeSelected']").each(function(){
				if($(this).prop("checked")){
					nodeIDs += $(this).val()+",";
					count++;
				}
	});
	if(count==0){
		alert("please select one node at least.");
		return false;
	}
	
	if(!confirm("You are deleting "+count+" nodes,aren't you?")){
		return false;
	}
	
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "DeleteNode",
        NODEID: nodeIDs
    },
    function(data) {
        if (dealReturnMessage(data, "Delete nodes") > 0) {
            newNode();
            getNodeList(currentPage);
        }
    });
}



function SetLEDCurrent() {
   // var nodeID = $("#nodeId").val();
	if($("#nodeType").val()=="1"){
		alert("Motor can not set current.");
		return;
	}
	var nodeId = $("#nodeId").val();
	var current1 = $("#channel1Current").val();
	var current2 = $("#channel2Current").val();
	var current3 = $("#channel3Current").val();
	var current4 = $("#channel4Current").val();

	if(nodeId==-1){
		alert("Please select a node.");
		return;
	}
	if(current1=="" ||current2==""||current3==""||current4==""){
		alert("Please input current.");
		return;
	}

    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "SetLEDCurrent",
        NODEID: nodeId,
	CURRENT1:current1,
	CURRENT2:current2,
	CURRENT3:current3,
	CURRENT4:current4
	
    },

    function(data) {
        dealReturnMessage(data, "Set LED Current");  
    });
}



function selectAllNode(obj){
		var isSelected = $(obj).prop("checked");
		$("[name='isNodeSelected']").each(function(){
				$(this).prop("checked",isSelected);
		});
}





function resetNode() {
    var nodeId = $("#nodeId").val();

	if(nodeId==-1){
	alert("Please select a node.");
	return;
	}
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "ResetNode",
        NODEID: nodeId
	
    },
    function(data) {

	dealReturnMessage(data, "Reset node");

    });
}

function getNodeStatus() {
    var nodeId = $("#nodeId").val();

	if(nodeId==-1){
	alert("Please select a node.");
	return;
	}
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "GetNodeStatus",
        NODEID: nodeId
	
    },
    function(data) {
	if($("#nodeType").val()=="0"){
		dealNodeStatus(data);
	}else{
		dealMotorStatus(data);
	}
    });
}

function dealNodeStatus(data) {
	var html = "<table width='100%' border='1' cellspacing='0' cellpadding='0'><tr><th width='20%'> &nbsp;</th><th> LEDs </th><th> Type</th><th> PPF (µmol/s)</th><th> PPF2C Sum Calibrated <br/>(µmol/s/mA)</th><th> Current</th><th> Minimum Current (mA)</th><th> Full Scale Current (mA)</th></tr>";

	var i = 1;    
	$(data).find('node').each(function() {
		html +="<tr><td>Channel"+i+"</td><td>"+$(this).children('number').text()+"</td><td>"+$(this).children('type').text()+"</td><td>"+$(this).children('ppf').text()+"</td><td>"+$(this).children('caliberated').text()+"</td><td>"+$(this).children('current').text()+"</td><td>"+$(this).children('miniCurrent').text()+"</td><td>"+$(this).children('fullCurrent').text()+"</td><tr>";

	i++;
	    });
	html +="</table>";
	$("#NODESTATUS").html(html);
}
function dealMotorStatus(data) {
	var html = "<table width='100%' border='1' cellspacing='0' cellpadding='0'><tr><th> Mode</th><th> Direction </th><th> Switch flag</th><th> Error</th><th> Distance </th><th> Current distance</th><th> speed</th><th> Pulses per mm</th><th> Max Speed</th><th> Max Length</th></tr>";

	 
	$(data).find('node').each(function() {
		html +="<tr><td>"+$(this).children('mode').text()+"</td><td>"+$(this).children('direction').text()+"</td><td>"+$(this).children('switchFlag').text()+"</td><td>"+$(this).children('error').text()+"</td><td>"+$(this).children('distance').text()+"</td><td>"+$(this).children('currentDistance').text()+"</td><td>"+$(this).children('speed').text()+"</td><td>"+$(this).children('pulses').text()+"</td><td>"+$(this).children('maxSpeed').text()+"</td><td>"+$(this).children('maxLength').text()+"</td><tr>";

	return false;
	    });
	html +="</table>";
	$("#NODESTATUS").html(html);
}

//---------------
function getNodeInfo() {
    var nodeId = $("#nodeId").val();

   if(nodeId==-1){
	alert("Please select a node.");
	return;
	}
    var loc = "NodeManage.action";
    $.get(loc, {
        METHOD: "GetNodeInfo",
        NODEID: nodeId
	
    },
    function(data) {
	
	dealGetNodeInfo(data);
	
    });
}

function dealGetNodeInfo(data) {
	var html = "<table width='100%' border='1' cellspacing='0' cellpadding='0'><tr><th> Type </th><th> Hard Version</th><th> Firmware version</th></tr>";

	var i = 1;    
	$(data).find('node').each(function() {
		html +="<tr><td>"+$(this).children('type').text()+"</td><td>"+$(this).children('hardVer').text()+"</td><td>"+$(this).children('firVer').text()+"</td><tr>";

	i++;
	    });
	html +="</table>";
	$("#NODESTATUS").html(html);
}
	

	

